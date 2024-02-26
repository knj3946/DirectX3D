#pragma once
class ColliderManager : public Singleton<ColliderManager>
{
private:
    friend class Singleton;

   

    ColliderManager();
    ~ColliderManager();

public:
    enum Collision_Type {
        PLAYER,
        ORC,
        WALL,
        ARROW,
        END,


    };

    void SetPlayer(Player* player) { this->player = player; playerCollider = player->GetCollider(); playerFoot = player->GetFootRay(); }    void SetObstacles(Collider* obstacle) { obstacles.push_back(obstacle); }

    void PushPlayer();
    void SetHeight();

    void GuiRender();

    float CloseRayCollisionColliderDistance(Ray ray);

    ColliderModel* CreateColliderModel(string mName, string mTag, Vector3 mScale, Vector3 mRot, Vector3 mPos);



    bool ControlPlayer(Vector3* dir);
    bool SetPlayerHeight(Collider* obstacle);


    void PushCollision(Collision_Type Type, Collider* _pCollider) { vecCol[Type].push_back(_pCollider); }

    vector<Collider*>& Getvector(Collision_Type _type) { return vecCol[_type]; }


    bool CollisionCheck(Collider* _pCollider, Collision_Type _vecCol);
    bool CollisionCheck(Collision_Type _type1,Collision_Type _type2);
private:
    Player* player;
    CapsuleCollider* playerCollider;
    vector<Collider*> obstacles;



    vector<Collider*> vecCol[Collision_Type::END];


    Ray* playerFoot;

    float maxHeight;

    Vector3 rayHeight;
};

