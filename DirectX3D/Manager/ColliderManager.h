#pragma once
class ColliderManager : public Singleton<ColliderManager>
{
private:
    friend class Singleton;

    ColliderManager();
    ~ColliderManager();

public:
    void SetPlayer(Player* player) { this->player = player; playerCollider = player->GetCollider(); }
    void SetObstacles(Collider* obstacle) { obstacles.push_back(obstacle); }

    void PushPlayer();
    void SetHeight();

    void GuiRender();

    float CloseRayCollisionColliderDistance(Ray ray);




    bool ControlPlayer(Vector3* dir);
    bool SetPlayerHeight(Collider* obstacle);

    ColliderModel* CreateColliderModel(string mName, string mTag, Vector3 mScale, Vector3 mRot, Vector3 mPos);

private:
    Player* player;
    CapsuleCollider* playerCollider;
    vector<Collider*> obstacles;

    Collider* onBlock;  //현재 밑에있는 콜라이더 Contact로 대체 가능할듯

    float maxHeight;

    Vector3 rayHeight;

    Ray* footRay;
    Ray* headRay;
};

