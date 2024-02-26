#pragma once
class Player : public ModelAnimator
{
private:
    enum State
    {
        IDLE,
        RUN_F, RUN_B, RUN_L, RUN_R,
        RUN_DL, RUN_DR,
        JUMP1, JUMP2, JUMP3,
        TO_COVER, C_IDLE, C_R, C_L, TO_STAND,
        TO_ASSASIN,
        KICK,
        DAGGER1, DAGGER2, DAGGER3,
        HIT
    };

    enum WeaponState
    {
        NONE, DAGGER
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

    void SetMoveSpeed(float speed) { this->moveSpeed = speed; }
    void SetHeightLevel(float heightLevel) { this->heightLevel = heightLevel; }

    Vector3 GetVelocity() { return velocity; }
    CapsuleCollider* GetCollider() { return collider; }
    Ray* GetFootRay() { return footRay; }
    
    
    Ray GetRay() { return straightRay; }    //아직 안쓰는거

    vector<Collider*>& GetWeaponColliders() { return weaponColliders; }

    void ResetTarget(Collider* collider, Contact contact) { targetObject = collider; this->contact = contact; }

    void SetTerrain(LevelData* terrain);

    float GetDamage();
    void Hit(float damage);

    void SetIsPushed(bool value) { isPushed = value; }
    void SetIsCeiling(bool value) { isCeiling = value; }

private:
    void Control();
    void Move();
    void UpdateUI();

    void Rotate();
    void Walking();
    void Jump();
    void AfterJumpAnimation();
    void Jumping();
    void Cover();
    void Assasination();

    void Attack();
    void AttackCombo();

    void SetAnimation();
    void SetState(State state, float scale = 1.0f, float takeTime = 0.2f);
    void SetIdle();
    void Searching();
    void Targeting();
    bool InTheAir();

    void EndHit();

    bool OnColliderFloor(Vector3& feedback);
    bool TerainComputePicking(Vector3& feedback, Ray ray);

private:

    POINT clientCenterPos = { WIN_WIDTH / 2, WIN_HEIGHT >> 1 }; //<- 연산자는 샘플 

    State curState = IDLE;
    WeaponState weaponState = DAGGER;

    Vector3 velocity;
    Vector3 targetPos;

    float moveSpeed = 200;
    float rotSpeed = 0.3;
    float deceleration = 10; //감속

    float heightLevel = 0.0f;

    float jumpVel = 0;
    int jumpN = 0;
    float nextJump = 0;

    float force1 = 215.f;
    float force2 = 250.0f;
    float force3 = 350.0f;

    float jumpSpeed = 0.156f;
    float gravityMult = 55.0;

    float landingT = 3.0f;
    float landing = 0.0f;
        
    CapsuleCollider* collider;
    vector<Collider*> weaponColliders;

    Ray straightRay;
    Ray diagnolLRay;
    Ray diagnolRRay;

    Collider* targetObject;
    Contact contact;
    Transform* targetTransform;

    float teleport = 110.000f;

    float temp = 12.5f;
    bool camera = true;

    Ray* footRay;
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

    int comboStack = 0;
    float comboHolding = 0.0f;   //comboStack이 유지되는 시간, 이 변수의 값은 공격의 진행시간마다 다르게 초기화
    bool combo = false;    //대거 1번 2번 공격이 스무스하게 연결되도록 대거 1번 애니메이션 진행 중간쯤에 이 변수 true로 만들기

    Transform* rightHand;
    Dagger* dagger;

    Transform* leftFoot;
    CapsuleCollider* leftFootCollider;
    
    Transform* rightFoot;
    CapsuleCollider* rightFootCollider;
    
    //left foot : 57
    //right foot : 62
    int rightHandNode = 35;
    int leftFootNode = 57;
    int rightFootNode = 62;

    bool isPushed = false;
    bool isCeiling = false;
};