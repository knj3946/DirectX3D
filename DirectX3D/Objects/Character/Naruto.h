#pragma once

//���� �н��� ���� ���� ������ �ϴ��� �׳� �ִϸ����ͷ�

class Naruto : public ModelAnimator
{
private:
    enum State
    {
        IDLE, RUN_F, RUN_B, RUN_L, RUN_R, THROW, PUNCH
    };

public:
    Naruto();
    ~Naruto();

    void Update();

 
    void Render();
    void PostRender();
    void GUIRender();

    void UpdateUI();

    void SetMoveSpeed(float speed) { this->moveSpeed = speed; }

    Collider* GetCollider() { return collider; }
    void Blocking(Collider* collider);

    void PushCol(Cube* cube) { cubes.push_back(cube); }
    vector<Collider*>& GetWeaponColliders() { return weaponColliders; }

    float GetCurAttackCoolTime();
    void SetAttackCoolDown();
    void FillAttackCoolTime();
   // CapsuleCollider* GetCollider() { return collider; }

    bool GetTest() { return btest; }
private:
    void Control();
    void Move();
    void Rotate();
    void Attack();
    void SetAnimation();

    void SetState(State state);

    void Throw();
    void EndThrow();
    void MouseDirection();



   
private:
    bool btest = false;

    vector<Cube*> cubes;

    Transform* mainHand; //�ַ� ���� ��, �������� ��� ������

    SphereCollider* tmpCollider; //�ӽ� �浹ü
    CapsuleCollider* collider;

    Model* kunai; // <-���� ���������� ���� �� ���

    State curState = IDLE;

    float moveSpeed = 50;
    float rotSpeed = 1;
    float deceleration = 5; //����

    Vector3 velocity;
    Vector3 targetPos;

    bool isTarget;

    POINT clientCenterPos = { WIN_WIDTH / 2, WIN_HEIGHT >> 1 }; //<- �����ڴ� ����

    Quad* crosshair;

    ProgressBar* hpBar;
    float curHP = 100, maxHp = 100;
    float destHP;
    bool isHit = false;

    vector<Collider*> weaponColliders;

    float curAttackCoolTime = 1.0f;
    float attackCoolTime = 2.0f;
    float value;
};

