#pragma once

class Orc
{
    // NPC ��ũ��Ʈ
    // �ۼ��� : �ڼ���
    // 2��15�� �ۼ�

private:
    enum State
    {
        Idle, Walk
        , Run, Hit, Attack, Dying
    };

    typedef TerrainEditor LevelData;

public:
    Orc(Transform* transform, ModelAnimatorInstancing* instancing, UINT index);
    ~Orc();

    void Update();
    void Render();

    void SetTerrain(LevelData* terrain) { this->terrain = terrain; }
    void SetAStar(AStar* aStar) { this->aStar = aStar; }
    void SetTarget(Transform* target) { this->target = target; }

    Transform* GetTransform() { return transform; }
    CapsuleCollider* GetCollider() { return collider; }

private:
    void Control();
    void Move();
    //void Rotate();

    void SetState(State state);
    void SetPath();

    void SetEvent(int clip, Event event, float timeRatio);
    void ExecuteEvent();

    void EndHit();
    void EndDying();


private:

    State curState = Idle;

    float moveSpeed = 25;
    float rotSpeed = 10;

    Vector3 velocity;

    vector<Vector3> path;

    LevelData* terrain;
    AStar* aStar;

    float searchCoolDown = 0.0f;

    Transform* target;

    ModelAnimatorInstancing* instancing;
    ModelAnimatorInstancing::Motion* motion;

    //������Ʈ Ȥ�� �����ӿ� ���� �̺�Ʈ ����
    vector<map<float, Event>> totalEvent;
    vector<map<float, Event>::iterator> eventIters;

    UINT index;

    Transform* root;
    Transform* transform;
    CapsuleCollider* collider;
};

