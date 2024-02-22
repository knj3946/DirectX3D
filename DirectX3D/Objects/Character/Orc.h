#pragma once

class Orc
{
    // NPC 스크립트
    // 작성자 : 박성진
    // 2월15일 작성

private:
    enum State
    {
        IDLE, WALK
        , RUN, HIT, ATTACK, DYING
    };

    enum class NPC_BehaviorState {
        IDLE,// 탐색하지않고 패트롤 상태 또는 가만히있는 상태
        CHECK,// 소리를 듣고 가거나 플레이어를 쫓다가 플레이어가 숨을때 탐색하는 상태
        DETECT// 플레이어를 쫓아서 공격하는 상태.
    };


    typedef TerrainEditor LevelData;
    //typedef Terrain LevelData;
public:
    enum class NPC_TYPE {
        ATTACK,
        INFORM//탐지범위가 긴 npc

    };

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
    void SetSRT(Vector3 scale, Vector3 rot, Vector3 transform);
    void SetStartPos(Vector3 pos) { this->startPos = pos; this->PatrolPos.push_back(startPos); }
    void SetType(NPC_TYPE _type);
    float GetInformRange() { return informrange; }


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
    void CalculateEarSight();//귀
    void Detection();
    void SetRay(Vector3& _pos);
    void Patrol();
    bool IsStartPos();

private:
    Ray ray;// 레이
    Vector3 StorePos;// 소리난 곳 가기 전 위치 저장
    Vector3 CheckPoint;// 소리난 곳 저장
    float earRange = 1000.f;// 듣는 범위
    bool bSound = false;// 소리 체크
    bool NearFind = false;
    bool bSensor = false;

    float informrange;// 탐지범위
    NPC_TYPE type;//
    vector<Vector3> PatrolPos;// 순찰지
    UINT nextPatrol = 0;// 순찰지 위치
 
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
    Transform* target;
    CapsuleCollider* targetCollider;

    ModelAnimatorInstancing* instancing;
    ModelAnimatorInstancing::Motion* motion;

    //스테이트 혹은 움직임에 따른 이벤트 지정
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

    friend class MonsterManager;
    bool PatrolChange = false;
    float WaitTime = 0.f;

};
