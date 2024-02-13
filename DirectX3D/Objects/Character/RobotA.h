#pragma once
class RobotA : public ModelAnimator
{
private:
    enum State
    {
        StandUp, Walk
        , Run, Hit, Dying
    };

    typedef Terrain LevelData;
    //typedef TerrainEditor LevelData;

public:
    RobotA();
    ~RobotA();

    void Update();
    void Render();

    void SetTerrain(LevelData* terrain) { this->terrain = terrain; }
    void SetAStar(AStar* aStar) { this->aStar = aStar; }
    void SetTarget(Transform* target) { this->target = target; }

private:
    void Control();
    void Move();
    void Rotate();

    void SetState(State state);
    void SetPath();

private:

    State curState = StandUp;

    float moveSpeed = 15;
    float rotSpeed = 10;

    Vector3 velocity;

    vector<Vector3> path;

    LevelData* terrain;
    AStar* aStar;

    float searchCoolDown = 0.0f;

    Transform* target;

};