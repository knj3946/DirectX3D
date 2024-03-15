#pragma once

class Orc
{
private:
    enum State
    {
        IDLE, WALK
        , RUN, HIT, ASSASSINATED, ATTACK, THROW, DYING
    };// throw �� attack16 Ŭ��
    


    enum class NPC_BehaviorState {
        IDLE,// Ž�������ʰ� ��Ʈ�� ���� �Ǵ� �������ִ� ����
        CHECK,// �Ҹ��� ��� ���ų� �÷��̾ �Ѵٰ� �÷��̾ ������ Ž���ϴ� ����
        SOUNDCHECK,
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
    typedef VertexUVNormalTangentAlpha VertexType;
    
    typedef TerrainEditor LevelData;
    //typedef Terrain LevelData;

public:
    enum class NPC_TYPE {
        ATTACK,
        INFORM//Ž�������� �� npc

    };

    // �ϴ� ����
    //enum HitState // ���� �¾ҳ� -> �¾��� �� HitState���� ���� �ٸ� ���� ���� ex)��ƼŬ ��
    //              // �׾��� ��, HitState�� ���� �°� �׾����� �Ǵ� �� �ٸ� �ִϸ��̼� ó��
    //{
    //    DAGGER, ARROW, ASSASSINATED,NONE
    //};

    Orc(Transform* transform, ModelAnimatorInstancing* instancing, UINT index);
    ~Orc();

    void Update();
    void Render();
    void PostRender();
    void GUIRender();
    void SetPatrolPos(Vector3& _pos) { PatrolPos.push_back(_pos); };
    void Direction();


    void SetTerrain(LevelData* terrain);
    void SetAStar(AStar* aStar) { this->aStar = aStar; }
    void SetTarget(Transform* target) { this->target = target; }
    void SetTargetCollider(CapsuleCollider* collider) { targetCollider = collider; }
    void SetTargetStateInfo(StateInfo* stateInfo) { this->targetStateInfo = stateInfo; }
    void SetSRT(Vector3 scale, Vector3 rot, Vector3 transform);
    void SetStartPos(Vector3 pos) { this->startPos = pos; this->PatrolPos.push_back(startPos); }
    void SetType(NPC_TYPE _type);
    float GetInformRange() { return informrange; }


    Transform* GetTransform() { return transform; }
    CapsuleCollider* GetCollider() { return collider; }
    CapsuleCollider* GetMoveCollider() { return moveCollider; }
    vector<Collider*>& GetWeaponColliders() { return weaponColliders; }
    void RotationRestore();
    float GetDamage();
    void Hit(float damage,Vector3 collisionPos,bool _btrue=false);
    void Spawn(Vector3 pos = {0,0,0});

    void AddObstacleObj(Collider* collider);

    bool IsFindTarget() { return bFind; };

    void AttackTarget();

    bool FindTarget() { return bSensor; }

    void Findrange(float startCool);

    void SetOutLine(bool flag);
    bool IsOutLine() { return outLine; };
    bool IsDetectTarget() { return bDetection; };

    void Assassinated(Vector3 collisionPos, Transform* attackerTrf);

    bool GetIsDelete() { return isDelete; }
    bool GetIsDying() { return isDying; }

    void CoolDown();
    void SetGroundPos();

    void SetStartCoolDown(float cool) { this->searchStartCoolDown = cool; }
private:
    void Control();
    void Move();
    void IdleAIMove();      
    void UpdateUI();

    void TimeCalculator(); // �ð� ����ϴ� �͵��� ���⼭ �ѹ��� �����ϴ°� ���ҰŰ��Ƽ� �߰��غ� �Լ�

    void SetState(State state, float scale = 1.0f, float takeTime = 0.2f);
    void SetPath(Vector3 targetPos);

    void SetEvent(int clip, Event event, float timeRatio);
    void ExecuteEvent();

    void EndAttack();
    void EndHit();
    void EndAssassinated();
    void EndDying();
   

    void CalculateEyeSight();
    void CalculateEarSight();//��
    void Detection();
    void SetRay(Vector3 _pos);
    void SetEyePos();
    void Patrol();
    bool IsStartPos();
    bool OnColliderFloor(Vector3& feedback);
    bool TerainComputePicking(Vector3& feedback, Ray ray);
    bool EyesRayToDetectTarget(Collider* targetCol, Vector3 orcEyesPos);
    void RangeCheck();
    void SoundPositionCheck();;
    
    void Throw();

    void StartAttack();
    bool GetDutyFlag();
    void SetParameter();
    bool CalculateHit();
    void PartsUpdate();
    void StateRevision();
    void ParticleUpdate();
    bool GetTargetAttack() { return battacktarget; }
private:

    int a = 300;
    Ray ray;// ����
    Vector3 StorePos;// �Ҹ��� �� ���� �� ��ġ ����
    Vector3 CheckPoint;// �Ҹ��� �� ����
    Vector3 eyesPos;
    float earRange = 12.f;// ��� ���� -> ��ũ�� ���� ������ �Ÿ��� 9
    bool bSound = false;// �Ҹ� üũ
    bool NearFind = false;
    bool bSensor = false;

    float informrange;// Ž������
    NPC_TYPE type= NPC_TYPE::ATTACK;//
    vector<Vector3> PatrolPos;// ������
    UINT nextPatrol = 0;// ������ ��ġ
    float DetectionRange;
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
    float searchStartCoolDown = 0.0f;
    Transform* target;
    CapsuleCollider* targetCollider;

    ModelAnimatorInstancing* instancing;
    ModelAnimatorInstancing::Motion* motion;

    //������Ʈ Ȥ�� �����ӿ� ���� �̺�Ʈ ����
    vector<map<float, Event>> totalEvent;
    vector<map<float, Event>::iterator> eventIters;

    UINT index;

    Transform* transform;
    CapsuleCollider* collider;
    CapsuleCollider* moveCollider;

    vector<Collider*> weaponColliders;

    ProgressBar* hpBar;
    ProgressBar* rangeBar;
    float curHP = 100, maxHp = 100;
    float destHP;
    bool isHit = false;

    bool isDying = false;

    bool isAssassinated = false;
    //HitState hitState = HitState::NONE;

    float curRange = 0.f, maxRange = 40.f;
    float destRange;

    Quad* questionMark;
    Quad* exclamationMark;

    Transform* leftHand;
    Transform* rightHand;
    CapsuleCollider* leftWeaponCollider;
    CapsuleCollider* rightWeaponCollider;

    float eyeSightRange = 40.f;
    float eyeSightangle = 45.f;
    bool bDetection = false;
    bool bFind = false;
    float DetectionStartTime = 0.f;
    float DetectionEndTime = 2.f;
    bool missTargetTrigger = false; // �Ѿư��ٰ� ������� ���� true 

    bool isTracking = false;
    float wateTime = 0;


    RayBuffer* rayBuffer;
    StructuredBuffer* structuredBuffer;
    vector<InputDesc> inputs;
    vector<OutputDesc> outputs;

    UINT terrainTriangleSize;

    ComputeShader* computeShader;

    int limitGroundHeight = 10;

    Vector3 feedBackPos;

    bool isAttackable=true;
    float attackSpeed = 1.5f;
    float curAttackSpeed=0;
    bool PatrolChange = false;
    float WaitTime = 0.f;
    float rangeDegree;
    UINT m_uiRangeCheck = 0;

    bool outLine = false;

    bool isDelete = false;

    Particle* particleHit;

    float rayCoolTime = 0.3f;
    float curRayCoolTime = 0.0f;
    bool isRayToDetectTarget = false;

    StateInfo* targetStateInfo;
    bool battacktarget = false;
    bool returntoPatrol = false;
    Vector3 restorePos;
};
