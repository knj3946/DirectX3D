  #pragma once

class Boss 
{
private:
    enum class BOSS_STATE {
        IDLE,
        DETECT,
        ATTACK,
    };

    enum STATE {
        IDLE,
        WALK,
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
    ModelAnimatorInstancing* instancing;
    ModelAnimatorInstancing::Motion* motion;
    Transform* transform;


    UINT index;

    BOSS_STATE state =BOSS_STATE::IDLE;
    STATE curState;
    CapsuleCollider* collider;

    float moveSpeed = 25;
    float runSpeed = 30;
    float walkSpeed = 10;
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

    ComputeShader* computeShader;

    bool bFind = false;
    float DetectionStartTime = 0.f;
    float DetectionEndTime = 2.f;
    float eyeSightRange = 40.f;
    float eyeSightangle = 45.f;
    bool bDetection = false;
    
    vector<Vector3> PatrolPos; //보스 왔다갔다 위치.
    UINT curPatrol=0;

    bool bWait = false;
    float WaitTime=0.f;

    float totargetlength;// 타겟과의 거리
    Vector3 dir;//가는 방향

  
    float AttackRange = 5.f;
    float JumpAttackRange = 20.f;
    float JumpAttackSpeed = 100.f;
    bool bJumpAttack;

private:

   // bool IsFindTarget() { return bFind; };
    void AddObstacleObj(Collider* collider);
    void Idle();
    void Direction();// 방향지정
    void Move();
    void IdleMove();
    void DoingAttack();
    void JumpAttack();

    void Die();

    void Control();// 패턴 조정
    void UpdateUI();
    void SetState(STATE state, float scale = 1.0f, float takeTime = 0.2f);
    void SetPath(Vector3 targetPos);
    void ExecuteEvent();
    void SetEvent(int clip, Event event, float timeRatio);
    void Detection();

    void EndAttack();
    void EndJumpAttack();
    void EndHit();
    void EndDying();
 //   void EndJumpAttack();
    bool IsPatrolPos();
    void IdleWalk();



    void Run();
public:

    Boss();
    ~Boss();
    void SetTerrain(LevelData* terrain) { this->terrain = terrain; }
    void SetAStar(AStar* aStar) { this->aStar = aStar; }
    void SetPatrolPos(Vector3 _pos) { PatrolPos.push_back(_pos); }
    void SetTargetCollider(CapsuleCollider* collider) { targetCollider = collider; }
    Collider* GetCollider() { return collider; }
    void Render();
    void Update();
    void PostRender();
    void GUIRender();
    float GetInformRange() { return informRange; }

    void SetTarget(Transform* _target) { this->target = _target; }
    void CalculateEyeSight();
    void CalculateEarSight();//귀
    

};

