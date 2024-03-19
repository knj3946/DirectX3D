  #pragma once

class Boss
{
private:
    enum class BOSS_STATE {
        IDLE,
        DETECT,
        ATTACK,
        FIND,
    };

    enum STATE {
        IDLE,
        WALK,
        RUN,
        ATTACK,
        ROAR,
        HIT,
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

    struct SpecialKeyUI
    {
        string name;
        char key;
        Quad* quad;
        bool active;
    };

    typedef VertexUVNormalTangentAlpha VertexType;

    typedef TerrainEditor LevelData;


private:
    ModelAnimatorInstancing* instancing;
    ModelAnimatorInstancing::Motion* motion;
    Transform* transform;
    Vector3 cross;
    float searchCoolDown = 0.0f;

    UINT index;

    BOSS_STATE state = BOSS_STATE::IDLE;
    STATE curState;
    CapsuleCollider* collider;
    CapsuleCollider* moveCollider;

    float moveSpeed = 25;
    float runSpeed = 30;
    float walkSpeed = 10;
    float curHP = 20;
    float maxHP = 20;
    float destHP;
    Transform* leftHand;
    CapsuleCollider* leftCollider;


    ProgressBar* hpBar;
    vector<map<float, Event>> totalEvent;
    vector<map<float, Event>::iterator> eventIters;

    Transform* target;
    UINT count = 0;
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


    Transform* Mouth;
    CapsuleCollider* RoarCollider;
    ParticleSystem* Roarparticle;

    ParticleSystem* Runparticle[3];
    UINT currunparticle = 0;

    Ray ray;


    ComputeShader* computeShader;

    bool bFind = false;
    float DetectionStartTime = 0.f;
    float DetectionEndTime = 2.f;
    float eyeSightRange = 40.f;
    float eyeSightangle = 45.f;
    bool bDetection = false;

    vector<Vector3> PatrolPos; //보스 왔다갔다 위치.
    UINT curPatrol = 0;
  
    bool bWait = false;
    float WaitTime = 0.f;

    float totargetlength;// 타겟과의 거리
    Vector3 dir;//가는 방향


    float AttackRange = 5.f;
    float RoarRange = 10.f;
    float RoarCoolTime = 0.f;
    bool bRoar = false;

    Vector3 FindPos;
    bool bArriveFindPos = false;
    bool MarkActiveTime = 0.f;

    Particle* hiteffect;
    bool IsHit = false;// 플레이어를 가격했는지 체크
    float attackdamage = 20.f;
    float Roardamage = 40.f;

    Vector3 feedBackPos;
    float curRayCoolTime = 0.f;
    DepthStencilState* depthState[2];
    BlendState* blendState[2];
    bool outLine = false;

    map<string, SpecialKeyUI> specialKeyUI;
    bool isHit = false;
    bool isDying = false;
    bool isAssassinated = false;
    float DetectionRange;
    bool IsPlayer = false;
    bool bRotate = false;// 한번은 무조건회전
    bool isRayToDetectTarget = false;
    Vector3 eyesPos;
private:
    void SetEyePos() {
        eyesPos = transform->GlobalPos();
        eyesPos.y += 5.f;
    }
    void CoolDown()
    {
        if (curRayCoolTime <= 0.0f)
        {
            curRayCoolTime = 0.2f;
        }
        else
            curRayCoolTime -= DELTA;
    }

    void MarkTimeCheck();
    void CoolTimeCheck();
   // bool IsFindTarget() { return bFind; };
    void AddObstacleObj(Collider* collider);
    void Idle();
    void Direction();// 방향지정
    void Move();
    void IdleMove();
    void Roar();
    void EndHit();
    void JumpAttack();
    bool CalculateHit();
    void Die();
    void Find();
    void Rotate();
    void Control();// 패턴 조정
    void UpdateUI();
    void SetState(STATE state, float scale = 1.0f, float takeTime = 0.2f);
   
    void ExecuteEvent();
    void SetEvent(int clip, Event event, float timeRatio);
    void Detection();

    void EndAttack();

    void StartAttack();
   
    void EndRoar();
   
    void EndDying();
 //   void EndJumpAttack();
    bool IsPatrolPos();
    bool IsFindPos();
    void IdleWalk();
    bool OnColliderFloor(Vector3& feedback);
  
    void Run();
    void SetRay();
    void SetPosY();


    void CollisionCheck();
    bool TerrainComputePicking(Vector3& feedback, Ray ray);

    void ProcessHpBar();
public:
    bool GetIsHit() { return IsHit; }
    void Hit(float damage, Vector3 collisionPos,bool _btrue=true);
    bool GetIsDying() { return isDying; }
    void ActiveSpecialKey(Vector3 playPos, Vector3 offset);
    void OnOutLineByRay(Ray ray);
    Transform* GetTransform() { return transform; }
    void SetPath(Vector3 targetPos);
    Boss();
    ~Boss();
    void SetTerrain(LevelData* terrain) { this->terrain = terrain; }
    void SetAStar(AStar* aStar) { this->aStar = aStar; }
    void SetPatrolPos(Vector3 _pos) { PatrolPos.push_back(_pos); if (1 == PatrolPos.size()) transform->Pos() = _pos; }
    void SetTargetCollider(CapsuleCollider* collider) { targetCollider = collider; }
    Collider* GetCollider() { return collider; }
    Collider* GetMoveCollider() { return moveCollider; }
    void Render();
    void Update();
    void PostRender();
    void GUIRender();
    float GetInformRange() { return informRange; }

    void SetTarget(Transform* _target) { this->target = _target; }
    void CalculateEyeSight();
    bool CalculateEyeSight(bool _bFind);
    void CalculateEarSight();//귀
    bool IsOutLine() { return outLine; };
    void Assassinated(Vector3 collisionPos, Transform* attackerTrf);
    bool EyesRayToDetectTarget(Collider* targetCol, Vector3 orcEyesPos);
};

