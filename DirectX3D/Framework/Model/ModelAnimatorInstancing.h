#pragma once

class ModelAnimatorInstancing : public ModelAnimator
{
public:
    struct Motion
    {
        float takeTime = 0.2f;
        float tweenTime = 0.0f;
        float runningTime = 0.0f;
        float duration = 0.0f;

        Frame cur, next;

        Motion()
        {
            next.clip = -1;
        }
    };

    struct ModelInfo
    {
        //int index; 필요하면 추가
        Transform* transform; // 모델의 트랜스폼
        bool isOutLine; // 아웃라인
    };

private:
    class FrameInstancingBuffer : public ConstBuffer
    {
    public:
        struct Data
        {
            Motion motions[MAX_INSTANCE];
        };

    public:
        FrameInstancingBuffer() : ConstBuffer(&data, sizeof(Data))
        {
        }

        Data& Get() { return data; }

    private:
        Data data;
    };

public:
    
    ModelAnimatorInstancing(string name);
    ~ModelAnimatorInstancing();

    void Update();
    void Render(bool exceptOutLine = false);
    void GUIRender();

    Transform* Add();
    void AddModelInfo(Transform* transform, int index);
    void Remove(int index);

    void PlayClip(UINT instanceID, int clip, float scale = 1.0f, float takeTime = 0.1f);

    Matrix GetTransformByNode(UINT instanceID, int nodeIndex);


    Motion* GetMotion(UINT instanceID)
    { 
        return &frameInstancingBuffer->Get().motions[instanceID];
    }

    UINT GetClipSize() { return clips.size(); }

    void SetOutLine(UINT index,bool flag); // 바꾸기
  
private:    
    void UpdateFrame(UINT instanceID, Motion& motion);
    void UpdateTransforms();

private:
  
    //vector<Transform*> transforms;
    // orc의 고유 id 기반으로 처리하기 때문에 map으로 관리
    map<int, ModelInfo> modelInfoes;

    InstanceData instanceDatas[MAX_INSTANCE];

    VertexBuffer* instanceBuffer;
    FrameInstancingBuffer* frameInstancingBuffer;

    UINT drawCount = 0;

    DepthStencilState* depthStencilState[2];
    BlendState* blendState[2];
    //vector<bool> outLines;
};