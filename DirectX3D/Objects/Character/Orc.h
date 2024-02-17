#pragma once

class Orc
{
    // NPC ��ũ��Ʈ
    // �ۼ��� : �ڼ���
    // 2��15�� �ۼ�

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

    void SetTerrain(LevelData* terrain);
    void SetAStar(AStar* aStar) { this->aStar = aStar; }
    void SetTarget(Transform* target) { this->target = target; }
    void SetTargetCollider(CapsuleCollider* collider) { targetCollider = collider; }
    void SetSRT(Vector3 scale, Vector3 rot, Vector3 transform);

    Transform* GetTransform() { return transform; }
    CapsuleCollider* GetCollider() { return collider; }

    void Hit();
    void Spawn(Vector3 pos);

    void CalculateEyeSight();
    void Detection();

    void AddObstacleObj(Collider* collider);

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
    float eyeSightRange = 1000.f;
    float eyeSightangle = 45.f;
    bool bDetection = false;
    bool bFind = false;

    float DetectionStartTime = 0.f;

    float DetectionEndTime = 3.f;

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

    //������Ʈ Ȥ�� �����ӿ� ���� �̺�Ʈ ����
    vector<map<float, Event>> totalEvent;
    vector<map<float, Event>::iterator> eventIters;

    UINT index;

    Transform* root;
    Transform* transform;
    CapsuleCollider* collider;

    ProgressBar* hpBar;
    float curHP = 100, maxHp = 100;


    Transform* mainHand; //�ַ� ���� ��, �������� ��� ������
    SphereCollider* tmpCollider;
};
