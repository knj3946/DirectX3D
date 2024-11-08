#pragma once

class TerrainEditor : public GameObject
{
private:
    enum BrushType
    {
        CIRCLE, SOFT_CIRCLE, RECT
    };

    enum EditType
    {
        HEIGHT, ALPHA
    };

    typedef VertexUVNormalTangentAlpha VertexType;

    const float MIN_HEIGHT = 0.0f;
    const float MAX_HEIGHT = 400.0f;

    const UINT MAX_SIZE = 256;

    class BrushBuffer : public ConstBuffer
    {
    private:
        struct Data
        {
            int type = 0;
            Float3 pickingPos;

            float range = 10.0f;
            Float3 color = { 0, 1, 0 };
        };

    public:
        BrushBuffer() : ConstBuffer(&data, sizeof(Data))
        {
        }

        Data& Get() { return data; }

    private:
        Data data;
    };

    class RayBuffer : public ConstBuffer
    {
    private:
        struct Data
        {
            Float3 pos;
            UINT triangleSize;

            Float3 dir;
            float padding;
        };

    public:
        RayBuffer() : ConstBuffer(&data, sizeof(Data))
        {
        }

        Data& Get() { return data; }

    private:
        Data data;
    };

    struct InputDesc
    {
        Float3 v0, v1, v2;
    };

    struct OutputDesc
    {
        int picked;
        float distance;
    };

public:
    TerrainEditor();
    TerrainEditor(UINT width, UINT height);
    ~TerrainEditor();

    void Update();
    void Render() override;
    void GUIRender() override;

    float GetHeight(const Vector3& pos, Vector3* normal = nullptr);

    Vector3 Picking();
    bool ComputePicking(Vector3& pos);

    void SetHeightMap(wstring fileName);

    UINT GetSizeWidth() { return this->width; }
    UINT GetSizeHeight() { return this->height; }

    Mesh<VertexType>* GetMesh() { return mesh; }
private:
    void MakeMesh();
    void MakeNormal();
    void MakeTangent();
    void MakeComputeData();

    void Resize();
    void UpdateHeight();

    void AdjustHeight();
    void AdjustAlpha();

    void SaveHeightMap();
    void LoadHeightMap();

    void SaveAlphaMap();
    void LoadAlphaMap();

private:
    string projectPath;

    UINT width, height;
    UINT triangleSize;

    float adjustValue = 10.0f;
    BrushType brushType = CIRCLE;
    EditType editType = ALPHA;

    UINT selectMap = 0;

    Vector3 pickingPos;

    Mesh<VertexType>* mesh;
    BrushBuffer* brushBuffer;
    RayBuffer* rayBuffer;
    StructuredBuffer* structuredBuffer;

    vector<InputDesc> inputs;
    vector<OutputDesc> outputs;

    Texture* heightMap;
    Texture* secondMap;
    Texture* thirdMap;

    ComputeShader* computeShader;
};