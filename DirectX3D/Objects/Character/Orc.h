#pragma once

class Orc
{
private:
    enum State
    {
        IDLE, WALK
        , RUN, HIT, ASSASSINATED, ATTACK, THROW, DYING
    };// throw 는 attack16 클립
    


    enum class NPC_BehaviorState {
        IDLE,// 탐색하지않고 패트롤 상태 또는 가만히있는 상태
        CHECK,// 소리를 듣고 가거나 플레이어를 쫓다가 플레이어가 숨을때 탐색하는 상태
        SOUNDCHECK,
        DETECT// 플레이어를 쫓아서 공격하는 상태.
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
        INFORM//탐지범위가 긴 npc

    };

    // 일단 보류
    //enum HitState // 뭐에 맞았나 -> 맞았을 때 HitState값에 따라 다른 동작 구현 ex)파티클 등
    //              // 죽었을 때, HitState로 뭐에 맞고 죽었는지 판단 후 다른 애니메이션 처리
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

    void TimeCalculator(); // 시간 계산하는 것들은 여기서 한번에 관리하는게 편할거같아서 추가해본 함수

    void SetState(State state, float scale = 1.0f, float takeTime = 0.2f);
    void SetPath(Vector3 targetPos);

    void SetEvent(int clip, Event event, float timeRatio);
    void ExecuteEvent();

    void EndAttack();
    void EndHit();
    void EndAssassinated();
    void EndDying();
   

    void CalculateEyeSight();
    void CalculateEarSight();//귀
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
    Ray ray;// 레이
    Vector3 StorePos;// 소리난 곳 가기 전 위치 저장
    Vector3 CheckPoint;// 소리난 곳 저장
    Vector3 eyesPos;
    float earRange = 12.f;// 듣는 범위 -> 오크가 쿠나이 던지는 거리가 9
    bool bSound = false;// 소리 체크
    bool NearFind = false;
    bool bSensor = false;

    float informrange;// 탐지범위
    NPC_TYPE type= NPC_TYPE::ATTACK;//
    vector<Vector3> PatrolPos;// 순찰지
    UINT nextPatrol = 0;// 순찰지 위치
    float DetectionRange;
    NPC_BehaviorState behaviorstate = NPC_BehaviorState::IDLE;

    State curState = IDLE;

    float moveSpeed = 25;
    float runSpeed = 25;
    float walkSpeed = 10;
    float rotSpeed = 10;

    Vector3 velocity;

    vector<Vector3> path;
    Vector3 startPos = { 5,0,5 }; // 각자의 위치가 있다. -> 그 위치에서 플레이어를 발견 시 쫓아가는 것으로 한다.
    // 놓치면 다시 원래 위치로 복귀로 가정


    LevelData* terrain;
    AStar* aStar;

    float searchCoolDown = 0.0f;
    float searchStartCoolDown = 0.0f;
    Transform* target;
    CapsuleCollider* targetCollider;

    ModelAnimatorInstancing* instancing;
    ModelAnimatorInstancing::Motion* motion;

    //스테이트 혹은 움직임에 따른 이벤트 지정
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
    bool missTargetTrigger = false; // 쫓아가다가 사라졌을 때만 true 

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
