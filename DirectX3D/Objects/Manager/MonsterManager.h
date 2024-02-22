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

    bool IsCollision(Ray ray, Vector3& hitPoint);

    void SetOrcSRT(int index, Vector3 scale, Vector3 rot, Vector3 pos);

    void AddOrcObstacleObj(Collider* collider);
    void SetTerrain(LevelData* terrain);

    void Blocking(Collider* collider);

<<<<<<< HEAD
    void Fight(Naruto* player); //임시로 나루토로 지정
=======
    void Fight(Player* player);
>>>>>>> b40f8b5ae3a8100a0a483a3fe97de15bb1be7890

    void CalculateDistance();
    void PushPosition(Vector3 _pos) { vecDetectionPos.push_back(_pos); }


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

