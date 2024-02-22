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
        TO_ASSASIN
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
    Ray GetRay() { return straightRay; }

    vector<Collider*>& GetWeaponColliders() { return weaponColliders; }

    void ResetTarget(Collider* collider, Contact contact) { targetObject = collider; this->contact = contact; }

    float GetCurAttackCoolTime() { return curAttackCoolTime; }
    void SetAttackCoolDown() { curAttackCoolTime = attackCoolTime; }  // ������Ÿ��
    void FillAttackCoolTime() {
        curAttackCoolTime -= DELTA;
        if (curAttackCoolTime < 0)
            curAttackCoolTime = 0;
    }
    void SetTerrain(LevelData* terrain);


private:
    void Control();
    void Move();

    void Rotate();
    void Walking();
    void Attack();
    void Jump();
    void AfterJumpAnimation();
    void Jumping();
    void Cover();
    void Assasination();

    void SetAnimation();
    void SetState(State state);
    void SetIdle();
    void Searching();
    bool InTheAir();

    bool TerainComputePicking(Vector3& feedback, Ray ray);


private:

    POINT clientCenterPos = { WIN_WIDTH / 2, WIN_HEIGHT >> 1 }; //<- �����ڴ� ���� 

    State curState = IDLE;

    Vector3 velocity;
    Vector3 targetPos;

    float moveSpeed = 200;
    float rotSpeed = 0.3;
    float deceleration = 10; //����

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

    bool w = true;
    bool s = true;
    bool a = true;
    bool d = true;

    float curAttackCoolTime = 1.0f;
    float attackCoolTime = 2.0f;

    float temp = 12.5f;
    bool camera = true;

    Ray* footRay;
    LevelData * terrain;

    RayBuffer* rayBuffer;
    StructuredBuffer* structuredBuffer;
    vector<InputDesc> inputs;
    vector<OutputDesc> outputs;

    UINT terrainTriangleSize;

    ComputeShader* computeShader;

    int limitGroundHeight = 10;

    Vector3 feedBackPos;
};