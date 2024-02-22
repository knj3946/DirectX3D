#pragma once

class Orc
{
    // NPC ��ũ��Ʈ
    // �ۼ��� : �ڼ���
    // 2��15�� �ۼ�

private:
    enum State
    {
        IDLE, WALK
        , RUN, HIT, ATTACK, DYING
    };

    enum class NPC_BehaviorState {
        IDLE,// Ž�������ʰ� ��Ʈ�� ���� �Ǵ� �������ִ� ����
        CHECK,// �Ҹ��� ��� ���ų� �÷��̾ �Ѵٰ� �÷��̾ ������ Ž���ϴ� ����
        DETECT// �÷��̾ �ѾƼ� �����ϴ� ����.
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
    
    typedef TerrainEditor LevelData;
    //typedef Terrain LevelData;
    typedef VertexUVNormalTangentAlpha VertexType;

public:
    Orc(Transform* transform, ModelAnimatorInstancing* instancing, UINT index);
    ~Orc();

    void Update();
    void Render();
    void PostRender();
    void GUIRender();

    void SetTerrain(LevelData* terrain);
    void SetAStar(AStar* aStar) { this->aStar = aStar; }
    void SetTarget(Transform* target) { this->target = target; }
    void SetTargetCollider(CapsuleCollider* collider) { targetCollider = collider; }
    void SetSRT(Vector3 scale, Vector3 rot, Vector3 transform);
    void SetStartPos(Vector3 pos) { this->startPos = pos; }

    Transform* GetTransform() { return transform; }
    CapsuleCollider* GetCollider() { return collider; }

    void Hit();
    void Spawn(Vector3 pos);

    void AddObstacleObj(Collider* collider);

    bool IsFindTarget() { return bFind; };

    void AttackTarget();

    float GetCurAttackCoolTime();
    void SetAttackCoolDown();
    void FillAttackCoolTime();

    bool FindTarget() { return bSensor; }

    void Findrange() { NearFind = true; behaviorstate = NPC_BehaviorState::DETECT; }

private:
    void Control();
    void Move();
    void IdleAIMove();
    void UpdateUI();

    void SetState(State state);
    void SetPath(Vector3 targetPos);

    void SetEvent(int clip, Event event, float timeRatio);
    void ExecuteEvent();

    void EndHit();
    void EndDying();

    void CalculateEyeSight();
    void CalculateEarSight();//��
    void Detection();
    void SetRay(Vector3& _pos);

    bool IsStartPos();
    bool TerainComputePicking(Vector3& feedback, Ray ray);

private:
    Ray ray;// ����
    Vector3 StorePos;// �Ҹ��� �� ���� �� ��ġ ����
    Vector3 CheckPoint;// �Ҹ��� �� ����
    float earRange = 1000.f;// ��� ����
    bool bSound = false;// �Ҹ� üũ
    bool NearFind = false;
    bool bSensor = false;
 
    NPC_BehaviorState behaviorstate = NPC_BehaviorState::IDLE;

    State curState = IDLE;

    float moveSpeed = 25;
    float runSpeed = 25;
    float walkSpeed = 10;
    float rotSpeed = 10;

    Vector3 velocity;

    vector<Vector3> path;
    Vector3 startPos = { 5,0,5 }; // ������ ��ġ�� �ִ�. -> �� ��ġ���� �÷��̾ �߰� �� �Ѿư��� ������ �Ѵ�.
    // ��ġ�� �ٽ� ���� ��ġ�� ���ͷ� ����


    LevelData* terrain;
    AStar* aStar;

    float searchCoolDown = 0.0f;
    Transform* target;
    CapsuleCollider* targetCollider;

    ModelAnimatorInstancing* instancing;
    ModelAnimatorInstancing::Motion* motion;

    //������Ʈ Ȥ�� �����ӿ� ���� �̺�Ʈ ����
    vector<map<float, Event>> totalEvent;
    vector<map<float, Event>::iterator> eventIters;

    UINT index;

    //Transform* root;
    Transform* transform;
    CapsuleCollider* collider;

    ProgressBar* hpBar;
    float curHP = 100, maxHp = 100;
    float destHP;
    bool isHit = false;

    Quad* questionMark;
    Quad* exclamationMark;

    Transform* leftHand;
    Transform* rightHand;
    CapsuleCollider* leftWeaponCollider;
    CapsuleCollider* rightWeaponCollider;

    float eyeSightRange = 20.f;
    float eyeSightangle = 45.f;
    bool bDetection = false;
    bool bFind = false;
    float DetectionStartTime = 0.f;
    float DetectionEndTime = 2.f;
    bool missTarget = false;
    bool missTargetTrigger = false;
    float missStartTime = 0.f;
    float missEndTime = 1.0f;

    bool isTracking = false;
    float wateTime = 0;

    bool IsAiCooldown = false;
    float aiCoolTime = 2.0f;
    bool isAIWaitCooldown = false;
    float aiWaitCoolTime = 1.5f;
    float curAttackCoolTime = 1.0f;
    float attackCoolTime = 1.0f;

    RayBuffer* rayBuffer;
    StructuredBuffer* structuredBuffer;
    vector<InputDesc> inputs;
    vector<OutputDesc> outputs;

    UINT terrainTriangleSize;

    ComputeShader* computeShader;

    int limitGroundHeight = 10;

    Vector3 feedBackPos;
};
