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

    void PushPlayer();
    void SetHeight();

    void GuiRender();

    float CloseRayCollisionColliderDistance(Ray ray);
    bool CloseRayCollisionColliderContact(Ray ray, Contact& con);




    bool ControlPlayer(Vector3* dir);
    bool SetPlayerHeight(Collider* obstacle);

    ColliderModel* CreateColliderModel(string mName, string mTag, Vector3 mScale, Vector3 mRot, Vector3 mPos);

private:
    Player* player;
    CapsuleCollider* playerCollider;
    vector<Collider*> obstacles;
    Ray* playerFoot;

    float maxHeight;

    Vector3 rayHeight;
};

