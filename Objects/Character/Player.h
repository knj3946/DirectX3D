﻿#pragma once
class Player : public ModelAnimator
{
private:
    enum State
    {
        IDLE,                                   //0
        RUN_F, RUN_B, RUN_L, RUN_R,             //1 ~ 4
        RUN_DL, RUN_DR,                         //5 ~ 6
        JUMP1, JUMP2, JUMP3,                    //7 ~ 9
        TO_COVER, C_IDLE, C_R, C_L, TO_STAND,   //10 ~ 14
        HIT,                                    //15
        CLIMBING,                               //16
        KICK,                                   //17
        ASSASSINATION1, ASSASSINATION2,         //18 ~ 19
        DAGGER1, DAGGER2, DAGGER3,              //20 ~ 22
        B_IDLE,                                 //23
        B_RUN_F, B_RUN_B, B_RUN_L, B_RUN_R,     //24 ~ 27
        B_DRAW, B_ODRAW, B_AIM, B_RECOIL,       //28 ~ 31
        B_S_TO_C, B_C_TO_S, B_C_IDLE,           //32 ~ 33
        B_C_F, B_C_B, B_C_L, B_C_R              //34 ~ 37
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


    vector<Collider*>& GetWeaponColliders() { return weaponColliders; }

    void ResetTarget(Collider* collider, Contact contact) { targetObject = collider; this->contact = contact; }

    void SetTerrain(LevelData* terrain);

    float GetDamage();
    void Hit(float damage);

    void SetIsPushed(bool value) { isPushed = value; }
    void SetIsCeiling(bool value) { isCeiling = value; }

    void Assassination();

    void Climb(Collider* col,Vector3 climbPos);

private:
    void CameraMove(); // 벽에 가려지는 플레이어 현상 해결을 위한 함수

    void Control();
    void Move();
    void UpdateUI();

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

private:

    POINT clientCenterPos = { WIN_WIDTH / 2, WIN_HEIGHT >> 1 }; //<- �����ڴ� ���� 

    State curState = IDLE;
    WeaponState weaponState = DAGGER;

    Vector3 velocity;
    Vector3 targetPos;

    float moveSpeed1 = 50;
    float moveSpeed2 = 20;
    float rotSpeed = 0.3;
    float deceleration = 10; //����

    float heightLevel = 0.0f;

    float jumpVel = 0;
    int jumpN = 0;
    float nextJump = 0;

    float force1 = 250.f;
    float force2 = 250.0f;
    float force3 = 350.0f;

    float jumpSpeed = 0.156f;
    float gravityMult = 55.0;

    float landingT = 3.0f;
    float landing = 0.0f;

    CapsuleCollider* collider;
    vector<Collider*> weaponColliders;

    Collider* targetObject;
    Contact contact;
    Transform* targetTransform;

    float teleport = 110.000f;

    float temp = 12.5f;
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
public: //임시
    bool headCrash;
    Transform* aimT;
    Quad* crosshair;
};