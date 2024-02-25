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

    void SetPlayer(Player* player, CapsuleCollider* playerCollider) { this->player = player; this->playerCollider = playerCollider; }
    void SetObstacles(Collider* obstacle) { obstacles.push_back(obstacle); }

    bool ControlPlayer(Vector3* dir);


    void PushCollision(Collision_Type Type, Collider* _pCollider) { vecCol[Type].push_back(_pCollider); }

    vector<Collider*>& Getvector(Collision_Type _type) { return vecCol[_type]; }


    bool CollisionCheck(Collider* _pCollider, Collision_Type _vecCol);
    bool CollisionCheck(Collision_Type _type1,Collision_Type _type2);
private:
    Player* player;
    CapsuleCollider* playerCollider;
    vector<Collider*> obstacles;


    vector<Collider*> vecCol[Collision_Type::END];

};

