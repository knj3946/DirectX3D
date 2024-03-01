﻿#pragma once
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
        ORC_KUNAI,
        END,
    };
    
    void SetPlayer(Player* player) { this->player = player; playerCollider = player->GetCollider(); }
    void SetObstacles(Collider* obstacle);
    vector<Collider*>& GetObstacles();

    void PushPlayer();
    void SetHeight();

    void GuiRender();

    float CloseRayCollisionColliderDistance(Ray ray);
    bool CloseRayCollisionColliderContact(Ray ray, Contact& con);

    ColliderModel* CreateColliderModel(string mName, string mTag, Vector3 mScale, Vector3 mRot, Vector3 mPos);

    bool ControlPlayer(Vector3* dir);
    bool SetPlayerHeight(Collider* obstacle);

    void PushCollision(Collision_Type Type, Collider* _pCollider) { vecCol[Type].push_back(_pCollider); }
    void PopCollision(Collision_Type Type, int index) { vecCol[Type].erase(vecCol[Type].begin() + index); }

    vector<Collider*>& Getvector(Collision_Type _type) { return vecCol[_type]; }

    Player* GetPlayer() { return player; }
    bool CollisionCheck(Collider* _pCollider, Collision_Type _vecCol);
    bool CollisionCheck(Collision_Type _type1, Collision_Type _type2);

private:
    Player* player;
    CapsuleCollider* playerCollider;
    //vector<Collider*> obstacles; // 밑의 vecCol에 통합

    Collider * onBlock; 

    vector<Collider*> vecCol[Collision_Type::END];

    float maxHeight;

    Vector3 rayHeight;

    Ray* footRay;
    Ray* headRay;
};