#pragma once
class ColliderManager : public Singleton<ColliderManager>
{
private:
    friend class Singleton;

    ColliderManager();
    ~ColliderManager();

public:
    void SetPlayer(Player* player) { this->player = player; playerCollider = player->GetCollider(); playerFoot = player->GetFootRay(); }
    void SetObstacles(Collider* obstacle) { obstacles.push_back(obstacle); }

    bool ControlPlayer(Vector3* dir);
    bool SetPlayerHeight(Collider* obstacle);

    float CloseRayCollisionColliderDistance(Ray ray);

private:
    Player* player;
    CapsuleCollider* playerCollider;
    vector<Collider*> obstacles;
    Ray* playerFoot;

    float maxHeight;
};

