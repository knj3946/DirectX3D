#pragma once
class ColliderManager : public Singleton<ColliderManager>
{
private:
    friend class Singleton;

   

    ColliderManager();
    ~ColliderManager();

public:
<<<<<<< HEAD
    void SetPlayer(Player* player) { this->player = player; playerCollider = player->GetCollider(); }
=======
    enum Collision_Type {
        PLAYER,
        ORC,
        WALL,
        ARROW,
        END,


    };

    void SetPlayer(Player* player, CapsuleCollider* playerCollider) { this->player = player; this->playerCollider = playerCollider; }
>>>>>>> 906ee3bd1174cd299fa7a8e1e11ede24d4789463
    void SetObstacles(Collider* obstacle) { obstacles.push_back(obstacle); }

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

<<<<<<< HEAD
    Collider* onBlock;  //현재 밑에있는 콜라이더 Contact로 대체 가능할듯
=======


    vector<Collider*> vecCol[Collision_Type::END];


    Ray* playerFoot;
>>>>>>> 906ee3bd1174cd299fa7a8e1e11ede24d4789463

    float maxHeight;

    Vector3 rayHeight;

    Ray* footRay;
    Ray* headRay;
};

