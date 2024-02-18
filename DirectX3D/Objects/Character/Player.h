#pragma once
class Player : public ModelAnimator
{
private:
    enum State
    {
        IDLE, 
        RUN_F, RUN_B, RUN_L, RUN_R, 
        JUMP1, JUMP2, JUMP3, 
        TO_COVER, C_IDLE, C_R, C_L, TO_STAND
    };

public:
	Player();
	~Player();

    void Update();
    void Render();
    void PostRender();
    void GUIRender();

    Ray GetRay() { return ray; }
    void Wall(BoxCollider* other);

    void ResetTarget(Collider* collider, Contact contact) { targetObject = collider; this->contact = contact;  }

private:
    void Control();
    void Move();

    void Rotate();
    void Walking();
    void Attack();
    void Jump();
    void Jumping();
    void EndJump();
    void Cover();

    void SetAnimation();
    void SetState(State state);
    void SetIdle();
    void Searching();
    bool InTheAir();


private:

    POINT clientCenterPos = { WIN_WIDTH / 2, WIN_HEIGHT >> 1 }; //<- 연산자는 샘플 

    State curState = IDLE;

    Vector3 velocity;
    Vector3 targetPos;

    float moveSpeed = 200;
    float rotSpeed = 1;
    float deceleration = 5; //감속
    
    float jumpVel = 0;
    int jumpN = 0;
    float nextJump = 0;    

    float force1 = 600.0f;
    float force2 = 250.0f;
    float force3 = 350.0f;

    float jumpSpeed = 1.5f;
    float gravityMult = 200.0f;

    float landingT = 3.0f;
    float landing = 0.0f;

    CapsuleCollider* collider;

    Ray ray;

    Collider* targetObject;
    Contact contact;
    Transform* tempTransform;

    bool toCover = false;

    float teleport = 110.000f;
};