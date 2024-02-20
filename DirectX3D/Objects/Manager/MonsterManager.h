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

    void SetTarget(Transform* target);

    bool IsCollision(Ray ray, Vector3& hitPoint);

    void SetOrcSRT(int index,Vector3 scale,Vector3 rot,Vector3 pos);

    void AddOrcObstacleObj(Collider* collider);
    void SetTerrain(LevelData* terrain);

    void Blocking(Collider* collider);

    void Fight(Naruto* player); //임시로 나루토로 지정
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
};

