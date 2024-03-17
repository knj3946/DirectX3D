#pragma once
#include "Objects/Basic/GameObject.h"

struct VT {
	ID3D11Resource* texture = nullptr;
	ID3D11ShaderResourceView* shader_resource_view = nullptr;
	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
	uint32_t xres = 0;
	uint32_t yres = 0;
    bool create(uint32_t new_xres, uint32_t new_yres, DXGI_FORMAT new_format, bool is_dynamic) {
        xres = new_xres;
        yres = new_yres;
        format = new_format;
        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = xres;
        desc.Height = yres;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = format;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        if (is_dynamic) {
            desc.Usage = D3D11_USAGE_DYNAMIC;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        }
        ID3D11Texture2D* tex2d = nullptr;
        HRESULT hr = DEVICE->CreateTexture2D(&desc, nullptr, (ID3D11Texture2D**)&texture);
        if (FAILED(hr))
            return false;

        // Create a resource view so we can use the data in a shader
        D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
        ZeroMemory(&srv_desc, sizeof(srv_desc));
        srv_desc.Format = new_format;
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srv_desc.Texture2D.MipLevels = desc.MipLevels;
        hr = DEVICE->CreateShaderResourceView(texture, &srv_desc, &shader_resource_view);
        if (FAILED(hr))
            return false;

        return true;
    }

    void activate(int slot) const {
        DC->PSSetShaderResources(slot, 1, &shader_resource_view);
    }

    void destroy() {
        SAFE_RELEASE(texture);
        SAFE_RELEASE(shader_resource_view);
    }

    bool updateFromIYUV(const uint8_t* data, size_t data_size) {
        assert(data);
        D3D11_MAPPED_SUBRESOURCE ms;
        HRESULT hr = DC->Map(texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
        if (FAILED(hr))
            return false;

        uint32_t bytes_per_texel = 1;
        assert(format == DXGI_FORMAT_R8_UNORM);
        assert(data_size == xres * yres * 3 / 4);

        const uint8_t* src = data;
        uint8_t* dst = (uint8_t*)ms.pData;

        // Copy the Y lines
        uint32_t nlines = yres / 2;
        uint32_t bytes_per_row = xres * bytes_per_texel;
        for (uint32_t y = 0; y < nlines; ++y) {
            memcpy(dst, src, bytes_per_row);
            src += bytes_per_row;
            dst += ms.RowPitch;
        }

        // Now the U and V lines, need to add Width/2 pixels of padding between each line
        uint32_t uv_bytes_per_row = bytes_per_row / 2;
        for (uint32_t y = 0; y < nlines; ++y) {
            memcpy(dst, src, uv_bytes_per_row);
            src += uv_bytes_per_row;
            dst += ms.RowPitch;
        }

        DC->Unmap(texture, 0);
        return true;
    }
};

struct VTInternalData {

    IMFSourceResolver* pSourceResolver = NULL;
    IUnknown* uSource = NULL;
    IMFMediaSource* mediaFileSource = NULL;
    IMFAttributes* pVideoReaderAttributes = NULL;
    IMFSourceReader* pSourceReader = NULL;
    IMFMediaType* pReaderOutputType = NULL, * pFirstOutputType = NULL;
    MF_OBJECT_TYPE     ObjectType = MF_OBJECT_INVALID;

    // Start processing frames.
    IMFSample* pVideoSample = NULL;
    DWORD      streamIndex = 0, flags = 0;
    LONGLONG   llVideoTimeStamp = 0, llSampleDuration = 0;
    int        sampleCount = 0;
    DWORD      sampleFlags = 0;

    VT* target_texture = nullptr;
    uint32_t  width = 0;
    uint32_t  height = 0;
    float     fps = 0.0f;

    float     clock_time = 0.0f;
    LONGLONG  video_time = 0;
    bool      finished = false;
    bool      paused = false;
    bool      autoloop = true;

    bool readOutputMediaFormat() {
        assert(pSourceReader);
        HRESULT hr;

        IMFMediaType* pNativeType = nullptr;
        CHECK_HR(pSourceReader->GetNativeMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, MF_SOURCE_READER_CURRENT_TYPE_INDEX, &pNativeType),
            "Error retrieving GetNativeMediaType.");

        SAFE_RELEASE(pFirstOutputType);
        CHECK_HR(pSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, &pFirstOutputType), "Error reading GetCurrentMediaType");

        // The sample uses the pNativeType here, but we want the resolution required to 'render' the texture as if the height is not x16 the conversion
        // YUV to RGB has problems
        CHECK_HR(MFGetAttributeSize(pFirstOutputType, MF_MT_FRAME_SIZE, &width, &height), "Read input resolution");

        // Read fps
        uint64_t Val;
        hr = pNativeType->GetUINT64(MF_MT_FRAME_RATE, &Val);
        fps = (float)HI32(Val) / (float)LO32(Val);

        SAFE_RELEASE(pNativeType);

        dbg("Source reader output media type: %dx%d %s (@ %f fps)\n", width, height, GetMediaTypeDescription(pFirstOutputType).c_str(), fps);
        return true;
    }

public:
    ~VTInternalData() {
        if (target_texture) {
            target_texture->destroy();
            target_texture = nullptr;
        }
        SAFE_RELEASE(pSourceResolver);
        SAFE_RELEASE(uSource);
        SAFE_RELEASE(mediaFileSource);
        SAFE_RELEASE(pVideoReaderAttributes);
        SAFE_RELEASE(pSourceReader);
        SAFE_RELEASE(pReaderOutputType);
        SAFE_RELEASE(pFirstOutputType);
    }

    bool open(const char* filename) {
        finished = false;

        wchar_t wfilename[256];
        mbstowcs(wfilename, filename, sizeof(wfilename) / sizeof(wchar_t));
        HRESULT hr = S_OK;

        // Need the color converter DSP for conversions between YUV, RGB etc.
        // Lots of examples use this explicit colour converter load. Seems
        // like most cases it gets loaded automatically.
       /* CHECK_HR(MFTRegisterLocalByCLSID(
          __uuidof(CColorConvertDMO),
          MFT_CATEGORY_VIDEO_PROCESSOR,
          L"",
          MFT_ENUM_FLAG_SYNCMFT,
          0,
          NULL,
          0,
          NULL),
          "Error registering colour converter DSP.");*/

          // Set up the reader for the file.
        CHECK_HR(MFCreateSourceResolver(&pSourceResolver),
            "MFCreateSourceResolver failed.");

        CHECK_HR(pSourceResolver->CreateObjectFromURL(
            wfilename,		        // URL of the source.
            MF_RESOLUTION_MEDIASOURCE,  // Create a source object.
            NULL,                       // Optional property store.
            &ObjectType,				        // Receives the created object type. 
            &uSource					          // Receives a pointer to the media source.
        ),
            "Failed to create media source resolver for file.");

        CHECK_HR(uSource->QueryInterface(IID_PPV_ARGS(&mediaFileSource)),
            "Failed to create media file source.");

        CHECK_HR(MFCreateAttributes(&pVideoReaderAttributes, 2),
            "Failed to create attributes object for video reader.");

        CHECK_HR(pVideoReaderAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID),
            "Failed to set dev source attribute type for reader config.");

        CHECK_HR(pVideoReaderAttributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, 1),
            "Failed to set enable video processing attribute type for reader config.");

        CHECK_HR(pVideoReaderAttributes->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_IYUV),
            "Failed to set media sub type on source reader output media type.");

        CHECK_HR(MFCreateSourceReaderFromMediaSource(mediaFileSource, pVideoReaderAttributes, &pSourceReader),
            "Error creating media source reader.");

        CHECK_HR(MFCreateMediaType(&pReaderOutputType), "Failed to create source reader output media type.");
        CHECK_HR(pReaderOutputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video), "Failed to set major type on source reader output media type.");
        CHECK_HR(pReaderOutputType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_IYUV), "Failed to set media sub type on source reader output media type.");

        CHECK_HR(pSourceReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, pReaderOutputType),
            "Failed to set output media type on source reader.");

        if (!readOutputMediaFormat())
            return false;

        CHECK_HR(pSourceReader->SetStreamSelection(MF_SOURCE_READER_FIRST_VIDEO_STREAM, TRUE), "Enable Video Stream");
        CHECK_HR(pSourceReader->SetStreamSelection(MF_SOURCE_READER_FIRST_AUDIO_STREAM, FALSE), "Disable Audio Stream");

        clock_time = 0.0f;

        if (target_texture)
            target_texture->destroy();

        // Just to ensure we have something as the first frame, and also ensure we have the real required size (height % 16 should be 0)
        update(0.0f);

        return true;
    }

    void update(float elapsed) {

        if (paused)
            return;

        if (finished)
            return;

        clock_time += elapsed;

        LONGLONG uct = (LONGLONG)(clock_time * 10000000);
        if (uct < video_time)
            return;

        assert(pSourceReader);
        flags = 0;

        HRESULT hr;
        hr = pSourceReader->ReadSample(
            MF_SOURCE_READER_FIRST_VIDEO_STREAM,
            0,                              // Flags.
            &streamIndex,                   // Receives the actual stream index. 
            &flags,                         // Receives status flags.
            &llVideoTimeStamp,              // Receives the time stamp.
            &pVideoSample                   // Receives the sample or NULL.
        );

        if (!SUCCEEDED(hr)) {
            finished = true;
            return;
        }

        video_time = llVideoTimeStamp;

        if (flags & MF_SOURCE_READERF_STREAMTICK)
        {
            dbg("\tStream tick.\n");
        }
        if (flags & MF_SOURCE_READERF_ENDOFSTREAM)
        {
            dbg("\tEnd of stream.\n");
            if (autoloop) {
                PROPVARIANT var = { 0 };
                var.vt = VT_I8;
                hr = pSourceReader->SetCurrentPosition(GUID_NULL, var);
                if (hr != S_OK) { dbg("Failed to set source reader position."); }
                clock_time = 0.0f;
            }
            else
                finished = true;
        }
        if (flags & MF_SOURCE_READERF_NEWSTREAM)
        {
            dbg("\tNew stream.\n");
            finished = true;
        }
        if (flags & MF_SOURCE_READERF_NATIVEMEDIATYPECHANGED)
        {
            dbg("\tNative type changed.\n");
            finished = true;
        }
        if (flags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED)
        {
            dbg("\tCurrent type changed: from %dx%d %s (@ %f fps)\n", width, height, GetMediaTypeDescription(pFirstOutputType).c_str(), fps);
            if (!readOutputMediaFormat()) {
                finished = true;
                return;
            }

        }

        if (pVideoSample) {
            hr = pVideoSample->SetSampleTime(llVideoTimeStamp);
            assert(SUCCEEDED(hr));

            hr = pVideoSample->GetSampleDuration(&llSampleDuration);
            assert(SUCCEEDED(hr));

            hr = pVideoSample->GetSampleFlags(&sampleFlags);
            assert(SUCCEEDED(hr));

            // 
            IMFMediaBuffer* buf = NULL;
            DWORD bufLength;

            hr = pVideoSample->ConvertToContiguousBuffer(&buf);
            hr = buf->GetCurrentLength(&bufLength);

            byte* byteBuffer = NULL;
            DWORD buffMaxLen = 0, buffCurrLen = 0;
            hr = buf->Lock(&byteBuffer, &buffMaxLen, &buffCurrLen);
            assert(SUCCEEDED(hr));

            // Some videos report one resolution and after the first frame change the height to the next multiple of 16 (using the event MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED)
            if (!target_texture) {
                target_texture = new VT();
                int texture_height = height * 2;
                if (!target_texture->create(width, texture_height, DXGI_FORMAT_R8_UNORM, true))
                    return;
            }

            target_texture->updateFromIYUV(byteBuffer, buffCurrLen);
            float elapsed = DELTA;

            dbg("Sample count %d, Sample flags %d, sample duration %I64d, sample time %I64d. CT:%I64d. Buffer: %ld/%ld. Elapsed:%f\n", sampleCount, sampleFlags, llSampleDuration, llVideoTimeStamp, uct, buffCurrLen, buffMaxLen, elapsed);

            hr = buf->Unlock();
            assert(SUCCEEDED(hr));

            SAFE_RELEASE(buf);
            sampleCount++;

            SAFE_RELEASE(pVideoSample);
        }

    }

};

class VideoTexture : public GameObject
{
public:
	VideoTexture(string fileName);
	~VideoTexture();
	static bool createAPI();
	static void destroyAPI();

	bool create(const char* filename);
	void destroy();
	bool update(float dt);

	void pause();
	void resume();
	bool hasFinished();
	VT* getTexture();
	float getAspectRatio() const;

public:
    VTInternalData* internal_data = nullptr;

};

