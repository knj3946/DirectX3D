#include "Framework.h"

Device::Device()
{
    UINT width = WIN_WIDTH;
    UINT height = WIN_HEIGHT;

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;    
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = hWnd;
    swapChainDesc.Windowed = true;//창모드 유무
    
    IDXGIFactory1 *pdxgiFactory = nullptr;
    IDXGIDevice *pdxgiDevice = nullptr;
    HRESULT hResult = S_OK;
    if (FAILED(hResult = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pdxgiFactory))) assert(false);

    size_t ui64VideoMemory;
    IDXGIAdapter* pAdapter;
    DXGI_ADAPTER_DESC adapterDesc;

    if (FAILED(pdxgiFactory->EnumAdapters(0, (IDXGIAdapter**)&pAdapter))) assert(false);
    
    pAdapter->GetDesc(&adapterDesc);
    ui64VideoMemory = (std::size_t)(adapterDesc.DedicatedVideoMemory + adapterDesc.SharedSystemMemory);

    //비디오 메모리 비교해서 더 좋은 GPU 찾기
    int gpu_idx = 0;
    int select = 0;
    std::size_t comparison_videoMemory;
    while (pdxgiFactory->EnumAdapters(gpu_idx, &pAdapter) != DXGI_ERROR_NOT_FOUND)
    {
        pAdapter->GetDesc(&adapterDesc);
        comparison_videoMemory = (std::size_t)(adapterDesc.DedicatedVideoMemory + adapterDesc.SharedSystemMemory);

        auto memory = comparison_videoMemory / 1024 / 1024;
        
        if (comparison_videoMemory > ui64VideoMemory)
        {
            ui64VideoMemory = comparison_videoMemory;
            select = gpu_idx;
        }
        ++gpu_idx;
    }

    pdxgiFactory->EnumAdapters(select, &pAdapter);

    D3D11CreateDeviceAndSwapChain(
        pAdapter,
        D3D_DRIVER_TYPE_UNKNOWN,
        0,
        D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &swapChainDesc,
        &swapChain,
        &device,
        nullptr,
        &deviceContext
    );

   
    ID3D11Texture2D* backBuffer;

    swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
    backBuffer->Release();       
    
    ID3D11Texture2D* depthBuffer;

    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    
    device->CreateTexture2D(&depthDesc, nullptr, &depthBuffer);

    D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc = {};
    depthViewDesc.Format = depthDesc.Format;
    depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

    device->CreateDepthStencilView(depthBuffer, &depthViewDesc, &depthStencilView);
}

Device::~Device()
{


   

    device->Release();
    deviceContext->Release();

    swapChain->Release();
    renderTargetView->Release();
}

void Device::Clear()
{
    deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

    float clearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };    
    deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Device::Present()
{
    swapChain->Present(0, 0);
}
