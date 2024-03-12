#pragma once
class MonsterManager : public Singleton<MonsterManager>
{
private:

    struct OrcInfo
    {
        Orc* orc = nullptr;
        Vector3 scale;
        Vector3 rot;
        Vector3 position;
        bool isActive = true;
    };

    struct SpecialKeyUI
    {
        string name;
        char key;
        Quad* quad;
        bool active;
    };

    typedef TerrainEditor LevelData;
    //typedef Terrain LevelData;
    UINT SIZE = 1;
public:
    MonsterManager();
    ~MonsterManager();

    void CreateOrc();

    void Update();
    void Render(bool exceptOutLine = false);
    void PostRender();
    void GUIRender();

    void SetTarget(Transform* target);
    void SetTargetCollider(CapsuleCollider* collider);
    void SetTargetStateInfo(StateInfo* stateInfo);

    bool IsCollision(Ray ray, Vector3& hitPoint);

    void SetOrcSRT(int index, Vector3 scale, Vector3 rot, Vector3 pos);
    void SetType(int index,Orc::NPC_TYPE _type);
    void SetType(int index, UINT _type);
    void SetPatrolPos(UINT idx,Vector3 Pos);



    void AddOrcObstacleObj(Collider* collider);
    void SetTerrain(LevelData* terrain);
    void SetAStar(AStar* astar);

    void Blocking(Collider* collider);

    void Fight(Player* player);

    void CalculateDistance();
    void PushPosition(Float4 _pos) { vecDetectionPos.push_back(_pos); }

    UINT GetOrcN() { return SIZE; }
    Orc* GetOrc(UINT idx) { return orcs[idx].orc; }

    void OnOutLineByRay(Ray ray);
    void ActiveSpecialKey(Vector3 playPos, Vector3 offset);

    void DieOrc(int index);

    void SetOrcGround();

    void SetShader(wstring file);
    void SetBoss(Boss* _boss) { boss = _boss; }

private:
    void Collision();   

private:
    Boss* boss;
    ModelAnimatorInstancing* orcInstancing;
    
    //vector<Orc*> orcs;
    map<int, OrcInfo> orcs;
    
    /*vector<Vector3> scales;
    vector<Vector3> rots;
    vector<Vector3> positions;*/

    int orcIndex = 0;

    Transform* target;

    float time = 0;


    vector<Float4> vecDetectionPos;// 타겟을 탐지한애들 위치 모으기

    friend class Orc;

    map<string,SpecialKeyUI> specialKeyUI;

    BlendState* blendState[2];
    DepthStencilState* depthState[2];
    RasterizerState* rasterizerState[2];


    Shadow* shadow; // 그림자
};

