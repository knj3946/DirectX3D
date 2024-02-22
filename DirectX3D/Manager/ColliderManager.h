#pragma once
class ColliderManager : public Singleton<ColliderManager>
{
private:
    friend class Singleton;

    ColliderManager();
    ~ColliderManager();

public:
    void SetPlayer(Player* player, CapsuleCollider* playerCollider) { this->player = player; this->playerCollider = playerCollider; }
    void SetObstacles(Collider* obstacle) { obstacles.push_back(obstacle); }

    bool ControlPlayer(Vector3* dir);

private:
    Player* player;
    CapsuleCollider* playerCollider;
    vector<Collider*> obstacles;
};

