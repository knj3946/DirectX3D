#include "Framework.h"

ModelAnimatorInstancing::ModelAnimatorInstancing(string name)
    : ModelAnimator(name)
{
    SetShader(L"Model/ModelAnimationInstancing.hlsl");

    instanceBuffer = new VertexBuffer(instanceDatas, sizeof(InstanceData), MAX_INSTANCE);
    frameInstancingBuffer = new FrameInstancingBuffer();

    FOR(2)
        depthStencilState[i] = new DepthStencilState();

    depthStencilState[0]->SetMode(DepthStencilState::ALL);
    depthStencilState[1]->SetMode(DepthStencilState::MASK);

    FOR(2) blendState[i] = new BlendState();
    blendState[1]->Alpha(true); //이미지 배경색 투명화 적용
}


ModelAnimatorInstancing::~ModelAnimatorInstancing()
{
    delete instanceBuffer;
    delete frameInstancingBuffer;

    for (pair<int, OrcInfo> item : orcInfoes)
        delete item.second.transform;

    orcInfoes.clear();
}

void ModelAnimatorInstancing::Update()
{
    UpdateTransforms();
}

void ModelAnimatorInstancing::Render()
{
    if (texture == nullptr)
        CreateTexture();

    instanceBuffer->Set(1);
    frameInstancingBuffer->SetVS(4);
    DC->VSSetShaderResources(0, 1, &srv);

    for (ModelMesh* mesh : meshes)
        mesh->RenderInstanced(drawCount);



    //아웃라이너
    drawCount = 0;
    for(pair<int,OrcInfo> item : orcInfoes)
    {
        //UpdateFrame(i, frameInstancingBuffer->Get().motions[i]);
        if (item.second.isOutLine)
        {
            item.second.transform->Scale() *= 1.1f;
            item.second.transform->Pos().y -= 0.35f;
            item.second.transform->UpdateWorld();
            instanceDatas[drawCount].world = XMMatrixTranspose(item.second.transform->GetWorld());
            item.second.transform->Scale() /= 1.1f;
            item.second.transform->Pos().y += 0.35f;
            instanceDatas[drawCount].index = item.first;
            drawCount++;
        }
    }
    instanceBuffer->Update(instanceDatas, drawCount);

    SetShader(L"Model/ModelAnimationInstancingOutline.hlsl");
    depthStencilState[1]->SetState();
    blendState[1]->SetState();
    for (ModelMesh* mesh : meshes)
    {
        mesh->RenderInstanced(drawCount);
    }
    depthStencilState[0]->SetState();
    blendState[0]->SetState();
    SetShader(L"Model/ModelAnimationInstancing.hlsl");
    
}

void ModelAnimatorInstancing::GUIRender()
{
    ImGui::Text("DrawCount : %d", drawCount);

    for (pair<int, OrcInfo> item : orcInfoes)
        item.second.transform->GUIRender();
}

Transform* ModelAnimatorInstancing::Add()
{
    Transform* transform = new Transform();
    //transform->SetTag(name + "_" + to_string(transforms.size()));
    ////outLines.push_back(false);
    //
    //transforms.insert(make_pair(, transform));
    

    return transform;
}

void ModelAnimatorInstancing::AddOrc(Transform* transform, int index)
{
    transform->SetTag(name + "_" + to_string(index));
    OrcInfo orc = { transform, false };

    if (orcInfoes.find(index) == orcInfoes.end())
        orcInfoes.insert(make_pair(index, orc));
    else
        assert("Orc Key Crash!!!");
}

void ModelAnimatorInstancing::Remove(int index)
{
    // orc에서 삭제함
    //delete orcInfoes[index].transform;
    orcInfoes.erase(index);
    //outLines.erase(outLines.begin() + index);
    //transforms.erase(transforms.begin() + index);
}

void ModelAnimatorInstancing::PlayClip(UINT instanceID, int clip, float scale, float takeTime)
{
    frameInstancingBuffer->Get().motions[instanceID].next.clip = clip;
    frameInstancingBuffer->Get().motions[instanceID].next.scale = scale;
    frameInstancingBuffer->Get().motions[instanceID].takeTime = takeTime;
    frameInstancingBuffer->Get().motions[instanceID].runningTime = 0.0f;
}

Matrix ModelAnimatorInstancing::GetTransformByNode(UINT instanceID, int nodeIndex)
{
    if (texture == nullptr) return XMMatrixIdentity();

    Matrix curAnim;

    {//CurAnim
        Frame& curFrame = frameInstancingBuffer->Get().motions[instanceID].cur;

        Matrix cur = nodeTransforms[curFrame.clip].transform[curFrame.curFrame][nodeIndex];
        Matrix next = nodeTransforms[curFrame.clip].transform[curFrame.curFrame + 1][nodeIndex];

        curAnim = Lerp(cur, next, curFrame.time) * orcInfoes[instanceID].transform->GetWorld();
    }

    {//NextAnim
        Frame& nextFrame = frameInstancingBuffer->Get().motions[instanceID].next;

        if (nextFrame.clip == -1)
            return curAnim;

        Matrix cur = nodeTransforms[nextFrame.clip].transform[nextFrame.curFrame][nodeIndex];
        Matrix next = nodeTransforms[nextFrame.clip].transform[nextFrame.curFrame + 1][nodeIndex];

        Matrix nextAnim = Lerp(cur, next, nextFrame.time) * orcInfoes[instanceID].transform->GetWorld();

        float tweenTime = frameInstancingBuffer->Get().motions[instanceID].tweenTime;

        return Lerp(curAnim, nextAnim, tweenTime);
    }
}

void ModelAnimatorInstancing::SetOutLine(UINT index,bool flag)
{
    orcInfoes[index].isOutLine = flag;
    //outLines[index] = flag;
}

void ModelAnimatorInstancing::UpdateFrame(UINT instanceID, Motion& motion)
{
    {
        ModelClip* clip = clips[motion.cur.clip];

        motion.duration = clip->frameCount / clip->tickPerSecond;
        motion.runningTime += motion.cur.scale * DELTA;
        motion.cur.time += clip->tickPerSecond * motion.cur.scale * DELTA;

        if (motion.cur.time >= 1.0f)
        {
            motion.cur.curFrame = (motion.cur.curFrame + 1) % (clip->frameCount - 1);
            motion.cur.time -= 1.0f;
        }
    }

    {
        if (motion.next.clip < 0) return;

        ModelClip* clip = clips[motion.next.clip];

        motion.tweenTime += DELTA / motion.takeTime;

        if (motion.tweenTime >= 1.0f)
        {
            motion.cur = motion.next;
            motion.tweenTime = 0.0f;

            motion.next.clip = -1;
            motion.next.curFrame = 0;
            motion.next.time = 0.0f;
            return;
        }

        motion.next.time += clip->tickPerSecond * motion.next.scale * DELTA;

        if (motion.next.time >= 1.0f)
        {
            motion.next.curFrame = (motion.next.curFrame + 1) % (clip->frameCount - 1);
            motion.next.time -= 1.0f;
        }
    }
}

void ModelAnimatorInstancing::UpdateTransforms()
{
    drawCount = 0;

    for(pair<int,OrcInfo> item : orcInfoes)
    {
        if (item.second.transform->Active())
        {
            UpdateFrame(item.first, frameInstancingBuffer->Get().motions[item.first]);
            item.second.transform->UpdateWorld();
            instanceDatas[drawCount].world = XMMatrixTranspose(item.second.transform->GetWorld());
            instanceDatas[drawCount].index = item.first;
            drawCount++;
        }
    }

    instanceBuffer->Update(instanceDatas, drawCount);
}
