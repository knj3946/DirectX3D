#include "Framework.h"

ModelInstancing::ModelInstancing(string name)
    : Model(name)
{
    instanceBuffer = new VertexBuffer(instanceDatas, sizeof(InstanceData), MAX_INSTANCE);
    SetShader(L"Model/ModelInstancing.hlsl");

    FOR(2)
        depthStencilState[i] = new DepthStencilState();

    depthStencilState[0]->SetMode(DepthStencilState::ALL);
    depthStencilState[1]->SetMode(DepthStencilState::MASK);

    FOR(2) blendState[i] = new BlendState();
    blendState[1]->Alpha(true); //이미지 배경색 투명화 적용
}

ModelInstancing::~ModelInstancing()
{
    delete instanceBuffer;

    for (Transform* transform : transforms)
        delete transform;
}

void ModelInstancing::Update()
{
    drawCount = 0;

    FOR(transforms.size())
    {
        if (transforms[i]->Active())
        {   
            transforms[i]->UpdateWorld();
            instanceDatas[drawCount].world = XMMatrixTranspose(transforms[i]->GetWorld());
            instanceDatas[drawCount].index = i;
            drawCount++;
        }
    }

    instanceBuffer->Update(instanceDatas, drawCount);
}

void ModelInstancing::Render()
{
    instanceBuffer->Set(1);

    for (ModelMesh* mesh : meshes)
        mesh->RenderInstanced(drawCount);

    //아웃라이너
    drawCount = 0;
    FOR(transforms.size())
    {
        //UpdateFrame(i, frameInstancingBuffer->Get().motions[i]);
        if (dropItems[i] && outLines[i])
        {
            /*transforms[i]->Scale() *= 1.1f;
            transforms[i]->Pos().y -= 0.35f;
            transforms[i]->UpdateWorld();
            instanceDatas[drawCount].world = XMMatrixTranspose(transforms[i]->GetWorld());
            transforms[i]->Scale() /= 1.1f;
            transforms[i]->Pos().y += 0.35f;
            instanceDatas[drawCount].index = i;
            drawCount++;*/
        }
    }
    instanceBuffer->Update(instanceDatas, drawCount);

    // 세이더파일
    SetShader(L"Landscape/Skybox.hlsl");
    depthStencilState[1]->SetState();
    blendState[1]->SetState();
    for (ModelMesh* mesh : meshes)
    {
        mesh->RenderInstanced(drawCount);
    }
    depthStencilState[0]->SetState();
    blendState[0]->SetState();
    SetShader(L"Model/ModelInstancing.hlsl");

}

void ModelInstancing::GUIRender()
{
    ImGui::Text("DrawCount : %d", drawCount);

    for (Transform* transform : transforms)
        transform->GUIRender();
}

Transform* ModelInstancing::Add(bool isDrop)
{
    Transform* transform = new Transform();
    transform->SetTag(name + "_" + to_string(transforms.size()));
    transforms.push_back(transform);
    outLines.push_back(false);
    dropItems.push_back(isDrop);
    return transform;
}

void ModelInstancing::SetOutLine(UINT index, bool flag)
{
    outLines[index] = flag;
}
