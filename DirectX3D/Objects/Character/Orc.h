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

    typedef TerrainEditor LevelData;

public:
    Orc(Transform* transform, ModelAnimatorInstancing* instancing, UINT index);
    ~Orc();

    void Update();
    void Render();
    void PostRender();

    void SetTerrain(LevelData* terrain) { this->terrain = terrain; }
    void SetAStar(AStar* aStar) { this->aStar = aStar; }
    void SetTarget(Transform* target) { this->target = target; }
    void SetTargetCollider(CapsuleCollider* collider) { targetCollider = collider; }

    Transform* GetTransform() { return transform; }
    CapsuleCollider* GetCollider() { return collider; }

    void Hit();

private:
    void Control();
    void Move();
    void UpdateUI();

    void SetState(State state);
    void SetPath();

    void SetEvent(int clip, Event event, float timeRatio);
    void ExecuteEvent();

    void EndHit();
    void EndDying();


private:

    State curState = HIT;

    float moveSpeed = 25;
    float rotSpeed = 10;

    Vector3 velocity;

    vector<Vector3> path;

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

    Transform* root;
    Transform* transform;
    CapsuleCollider* collider;

    ProgressBar* hpBar;
    float curHP = 100, maxHp = 100;


    Transform* mainHand; //주로 쓰는 손, 나루토의 경우 오른손
    SphereCollider* tmpCollider;
};
