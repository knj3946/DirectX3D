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
    STATE curState;
    CapsuleCollider* collider;

    float moveSpeed;
    float curHP = 200;
    float maxHP = 200;
    Transform* leftHand;
    CapsuleCollider* leftCollider;
 
    ProgressBar* hpBar;
    vector<map<float, Event>> totalEvent;
    vector<map<float, Event>::iterator> eventIters;

    Transform* target;
    UINT count=0;
    LevelData* terrain;
    AStar* aStar;

    vector<Vector3> path;

    ProgressBar* rangeBar;
    Quad* questionMark;
    Quad* exclamationMark;
    RayBuffer* rayBuffer;
    StructuredBuffer* structuredBuffer;
    vector<InputDesc> inputs;
    vector<OutputDesc> outputs;
    UINT terrainTriangleSize;

    Collider* targetCollider;
    float informRange;
    Vector3 velocity;

    bool bFind = false;
    float DetectionStartTime = 0.f;
    float DetectionEndTime = 2.f;
    float eyeSightRange = 40.f;
    float eyeSightangle = 45.f;
    bool bDetection = false;
private:

   // bool IsFindTarget() { return bFind; };
    void AddObstacleObj(Collider* collider);
    void IDLE();
    void Direction();// 방향지정
    void Move();
    void Attack();
    void JumpAttack();

    void Die();

    void Control();// 패턴 조정
    void UpdateUI();
 
    void SetPath(Vector3 targetPos);
    void ExecuteEvent();

    void EndAttack();
    void EndHit();
    void EndDying();
    void EndJumpAttack();
public:

    Boss();
    ~Boss();
    void SetTerrain(LevelData* terrain) { this->terrain = terrain; }
    void SetAStar(AStar* aStar) { this->aStar = aStar; }
    void SetTargetCollider(CapsuleCollider* collider) { targetCollider = collider; }
    Collider* GetCollider() { return collider; }
    void Render();
    void Update();
    void GUIRender();
    float GetInformRange() { return informRange; }

    void SetTarget(Transform* _target) { this->target = _target; }
    void CalculateEyeSight();
    void CalculateEarSight();//귀
    

};

