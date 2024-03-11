#pragma once
class Player : public ModelAnimator
{
private:
    enum State
    {
        IDLE,                                                   //0
        RUN_F, RUN_B, RUN_L, RUN_R,                             //1 ~ 4
        RUN_DL, RUN_DR,                                         //5 ~ 6
        JUMP1, JUMP2, JUMP3,                                    //7 ~ 9
        TO_COVER, C_IDLE, C_R, C_L, TO_STAND,                   //10 ~ 14
        HIT,                                                    //15
        CLIMBING1, CLIMBING2, CLIMBING3,                        //16 ~ 18
        CLIMBING_JUMP_L, CLIMBING_JUMP_R, CLIMBING_DOWN,        //16 ~ 18
        CLIMBING_JUMP_D,
        KICK,                                   //19
        ASSASSINATION1, ASSASSINATION2,         //20 ~ 21
        DAGGER1, DAGGER2, DAGGER3,              //22 ~ 24
        B_IDLE,                                 //25
        B_RUN_F, B_RUN_B, B_RUN_L, B_RUN_R,     //26 ~ 29
        B_DRAW, B_ODRAW, B_AIM, B_RECOIL,       //30 ~ 33
        B_S_TO_C, B_C_TO_S, B_C_IDLE,           //34 ~ 36
        B_C_F, B_C_B, B_C_L, B_C_R              //37 ~ 40
    };

    enum WeaponState
    {
        NONE, DAGGER, BOW
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
    //typedef Terrain LevelData;
    typedef TerrainEditor LevelData;
    typedef VertexUVNormalTangentAlpha VertexType;

    

public:
    Player();
    ~Player();

    void Update();
    void Render();
    void PostRender();
    void GUIRender();

    void SetMoveSpeed(float speed) { this->moveSpeed1 = speed; }
    void SetHeightLevel(float heightLevel) { this->heightLevel = heightLevel; }

    Vector3 GetVelocity() { return velocity; }
    CapsuleCollider* GetCollider() { return collider; }
    StateInfo* GetStateInfo() { return stateInfo; }


    vector<Collider*>& GetWeaponColliders() { return weaponColliders; }

    void ResetTarget(Collider* collider, Contact contact) { targetObject = collider; this->contact = contact; }

    void SetTerrain(LevelData* terrain);

    float GetDamage();
    void Hit(float damage);
    void Hit(float damage,bool camerashake);// 카메라쉐이크용

    void SetHeadCrash(bool headCrash) { this->headCrash = headCrash; }
    void SetBow(Transform* _transform) { BowInstallation = _transform; }

    void SetIsPushed(bool value) { isPushed = value; }
    void SetIsCeiling(bool value) { isCeiling = value; }

    void Assassination();

    void Climb(Collider* col,Vector3 climbPos);

    void SetClimbAnim();

    void Climbing();

    void SetHitEffectPos(Vector3& _pos) { particlepos = _pos; }

    void SetBoss(class Boss* _boss) { boss = _boss; }

private:
    void CameraMove(); // 벽에 가려지는 플레이어 현상 해결을 위한 함수

    void Control();
    void Move();
    void UpdateUI();

    void Cloaking();
    void Rotate();
    void Walking();
    void Jump();
    void AfterJumpAnimation();
    void Jumping();
    void Cover();

    void ComboAttack();
    void ShootArrow();

    void SetAnimation();
    void SetState(State state, float scale = 1.0f, float takeTime = 0.2f);

    void SetIdle();
    void SetCameraPos();
    void SetBowAnim();

    void Searching();
    void Targeting();
    void UseSkill();
    bool InTheAir();

    void EndAssassination(UINT num);
    void EndHit();
    void EndClimbing();

    bool OnColliderFloor(Vector3& feedback);

    //bool OnColliderFloor(Vector3& feedback);
    bool TerainComputePicking(Vector3& feedback, Ray ray);

    CapsuleCollider* GetDaggerCollider() { return dagger->GetCollider(); }

    void CoolDown();


private:
    class Boss* boss;
    Transform* BowInstallation;
    bool bgetBow = false;
    bool DrawSpecialkeyBowUI = false;
    POINT clientCenterPos = { WIN_WIDTH / 2, WIN_HEIGHT >> 1 }; //<- �����ڴ� ���� 

    State curState = IDLE;
    WeaponState weaponState = DAGGER;

    Vector3 velocity;
    Vector3 targetPos;

    float moveSpeed1 = 50;
    float moveSpeed2 = 15;
    float rotSpeed = 0.3;
    float deceleration = 10; //����

    float heightLevel = 0.0f;

    float jumpVel = 0;
    int jumpN = 0;
    float nextJump = 0;

    float force1 = 200.f;
    float force2 = 250.0f;
    float force3 = 350.0f;

    float jumpSpeed = 0.300f;
    float gravityMult = 55.0;

    float landingT = 3.0f;
    float landing = 0.0f;

    CapsuleCollider* collider;
    vector<Collider*> weaponColliders;

    Collider* targetObject;
    Contact contact;
    Transform* targetTransform;

    float teleport = 110.000f;

    bool camera = true;

    LevelData* terrain;

    RayBuffer* rayBuffer;
    StructuredBuffer* structuredBuffer;
    vector<InputDesc> inputs;
    vector<OutputDesc> outputs;

    UINT terrainTriangleSize;

    ComputeShader* computeShader;

    int limitGroundHeight = 10;

    Vector3 feedBackPos;

    ProgressBar* hpBar;
    float curHP = 100, maxHp = 100;
    float destHP;
    bool isHit = false;
    bool isDying = false;

    int comboStack = 0;
    float comboHolding = 0.0f;

    Transform* rightHand;
    Transform* leftHand;
    Dagger* dagger;
    Model* bow;

    Transform* leftFoot;
    CapsuleCollider* leftFootCollider;

    Transform* rightFoot;
    CapsuleCollider* rightFootCollider;

    //left foot : 57
    //right foot : 62

    int rightHandNode = 35;
    int leftHandNode = 11;
    int leftFootNode = 57;
    int rightFootNode = 62;

    bool isPushed = false;
    bool isCeiling = false;

    float preHeight = 0.0f;

    Quad* portrait;
    Quad* form;

    float remainClimbDis = 0.0f;
    bool isClimb = false;

    Vector3 particlepos;
    Particle* hiteffect;

    float rayCoolTime = 0.1f;
    float curRayCoolTime = 0.0f;
    bool isRayToDetectTarget = false;

    ParticleSystem* jumpparticle;

    //투명화
    StateInfo* stateInfo;
    wstring residualCloakingTime = L"";
    BlendState* blendState[2];

public: //임시
    bool headCrash;

    Transform* aimT;
    Quad* crosshair;
    const float initSpeed = 20.0f;
    const float maxSpeed = 200.0f;
    float chargingT = initSpeed;

    bool canClimbControl = false;
    Vector3 climbVel;
    Vector3 climbArrivePos;
    float climbJ_y;

    Transform* tempCam;

    State preState = IDLE;

public: //임시



};