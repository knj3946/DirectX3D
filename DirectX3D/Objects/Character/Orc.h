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
    //typedef Terrain LevelData;
public:
    Orc(Transform* transform, ModelAnimatorInstancing* instancing, UINT index);
    ~Orc();

    void Update();
    void Render();
    void PostRender();
    void GUIRender();

    void SetTerrain(LevelData* terrain);
    void SetAStar(AStar* aStar) { this->aStar = aStar; }
    void SetTarget(Transform* target) { this->target = target; }
    void SetTargetCollider(CapsuleCollider* collider) { targetCollider = collider; }
    void SetSRT(Vector3 scale, Vector3 rot, Vector3 transform);
    void SetStartPos(Vector3 pos) { this->startPos = pos; }

    Transform* GetTransform() { return transform; }
    CapsuleCollider* GetCollider() { return collider; }

    void Hit();
    void Spawn(Vector3 pos);

    void AddObstacleObj(Collider* collider);

    bool IsFindTarget() { return bFind; };

    void AttackTarget();

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
    void Detection();

private:
    State curState = IDLE;

    float moveSpeed = 25;
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

    Transform* root;
    Transform* transform;
    CapsuleCollider* collider;

    ProgressBar* hpBar;
    float curHP = 100, maxHp = 100;
    float destHP;
    bool isHit = false;

    Quad* questionMark;
    Quad* exclamationMark;

    Transform* mainHand;
    SphereCollider* tmpCollider;

    float eyeSightRange = 40.f;
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
};
