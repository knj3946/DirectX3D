#pragma once
class ColliderManager : public Singleton<ColliderManager>
{
private:
    friend class Singleton;

    ColliderManager();
    ~ColliderManager();

public:
<<<<<<< HEAD
    void SetPlayer(Player* player, CapsuleCollider* playerCollider) { this->player = player; this->playerCollider = playerCollider; }
    void SetObstacles(Collider* obstacle) { obstacles.push_back(obstacle); }

    bool ControlPlayer(Vector3* dir);
=======
    void SetPlayer(Player* player) { this->player = player; playerCollider = player->GetCollider(); playerFoot = player->GetFootRay(); }
    void SetObstacles(Collider* obstacle) { obstacles.push_back(obstacle); }

    bool ControlPlayer(Vector3* dir);
    bool SetPlayerHeight(Collider* obstacle);

>>>>>>> b40f8b5ae3a8100a0a483a3fe97de15bb1be7890

private:
    Player* player;
    CapsuleCollider* playerCollider;
    vector<Collider*> obstacles;
<<<<<<< HEAD
=======
    Ray* playerFoot;

    float maxHeight;
>>>>>>> b40f8b5ae3a8100a0a483a3fe97de15bb1be7890
};

