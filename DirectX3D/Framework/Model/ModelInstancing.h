#pragma once

class ModelInstancing : public Model
{
public:
    ModelInstancing(string name);
    ~ModelInstancing();

    void Update();
    void Render();
    void GUIRender();

    Transform* Add(bool isDrop = false);

    void SetOutLine(UINT index, bool flag);

private:
    vector<Transform*> transforms;
    InstanceData instanceDatas[MAX_INSTANCE];

    VertexBuffer* instanceBuffer;

    UINT drawCount = 0;

    DepthStencilState* depthStencilState[2];
    BlendState* blendState[2];
    vector<bool> outLines;
    vector<bool> dropItems;
};