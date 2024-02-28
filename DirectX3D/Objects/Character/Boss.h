#pragma once

class Boss :
    public ModelAnimator
{
private:
    enum class BOSS_STATE {
        IDLE,
        DETECT,
        ATTACK,
    };

    enum STATE {
        IDLE,
        RUN,
        ATTACK,
        ROAR,
        JUMPATTACK,
        DEATH

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
    typedef VertexUVNormalTangentAlpha VertexType;

    typedef TerrainEditor LevelData;


private:
    BOSS_STATE state =BOSS_STATE::IDLE;
    CapsuleCollider* collider;

    float moveSpeed;
    float curHP = 200;
    float maxHP = 200;
    Transform* leftHand;
    CapsuleCollider* leftCollider;
    ProgressBar* hpbar;
    vector<map<float, Event>> totalEvent;
    vector<map<float, Event>::iterator> eventIters;

    Transform* target;
    UINT count=0;
    LevelData* terrain;
    AStar* aStar;

    vector<Vector3> path;

    ProgressBar* hpBar;
    Quad* questionMark;
    Quad* exclamationMark;
    RayBuffer* rayBuffer;
    StructuredBuffer* structuredBuffer;
    vector<InputDesc> inputs;
    vector<OutputDesc> outputs;

    UINT terrainTriangleSize;


private:
    void Direction();// 방향지정

   // bool IsFindTarget() { return bFind; };
    void AddObstacleObj(Collider* collider);

public:

    Boss();
    ~Boss();
    void SetTerrain(LevelData* terrain) { this->terrain = terrain; }
    void SetAStar(AStar* aStar) { this->aStar = aStar; }

    Collider* GetCollider() { return collider; }
    void Render();
    void Update();
    void GUIRender();

    void SetTarget(Transform* _target) { this->target = _target; }
    
};

