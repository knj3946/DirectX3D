#pragma once
class MonsterManager : public Singleton<MonsterManager>
{
private:
    typedef TerrainEditor LevelData;
    //typedef Terrain LevelData;
    UINT SIZE = 2;
public:
    MonsterManager();
    ~MonsterManager();

    void Update();
    void Render();
    void PostRender();
    void GUIRender();

    void SetTarget(Transform* target);
    void SetTargetCollider(CapsuleCollider* collider);

    bool IsCollision(Ray ray, Vector3& hitPoint);

    void SetOrcSRT(int index, Vector3 scale, Vector3 rot, Vector3 pos);

    void AddOrcObstacleObj(Collider* collider);
    void SetTerrain(LevelData* terrain);

    void Blocking(Collider* collider);

    void Fight(Player* player);

    void CalculateDistance();
    void PushPosition(Vector3 _pos) { vecDetectionPos.push_back(_pos); }

    void OnOutLineByRay(Ray ray);


private:
    void Collision();

private:

    ModelAnimatorInstancing* orcInstancing;
    vector<Orc*> orcs;
    vector<Vector3> scales;
    vector<Vector3> rots;
    vector<Vector3> positions;


    Transform* target;

    float time = 0;


    vector<Vector3> vecDetectionPos;// 타겟을 탐지한애들 위치 모으기


};

