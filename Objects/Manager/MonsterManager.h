#pragma once
class MonsterManager : public Singleton<MonsterManager>
{
private:

    struct SpecialKeyUI
    {
        string name;
        Quad* quad;
        bool active;
    };

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
    void SetType(int index,Orc::NPC_TYPE _type);
    void SetPatrolPos(UINT idx,Vector3 Pos);



    void AddOrcObstacleObj(Collider* collider);
    void SetTerrain(LevelData* terrain);
    void SetAStar(AStar* astar);

    void Blocking(Collider* collider);

    void Fight(Player* player);

    void CalculateDistance();
    void PushPosition(Float4 _pos) { vecDetectionPos.push_back(_pos); }

    Orc* GetOrc(UINT idx) { return orcs[idx]; }

    void OnOutLineByRay(Ray ray);
    void ActiveSpecialKey(Vector3 playPos, Vector3 offset);


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


    vector<Float4> vecDetectionPos;// 타겟을 탐지한애들 위치 모으기

    friend class Orc;

    map<string,SpecialKeyUI> specialKeyUI;

    BlendState* blendState[2];
    DepthStencilState* depthState[2];

};

