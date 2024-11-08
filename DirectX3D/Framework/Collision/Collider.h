#pragma once
#include "Objects/Basic/GameObject.h"

struct Ray
{
    Vector3 pos;
    Vector3 dir;

    Ray()
    {
    }

    Ray(Vector3 pos, Vector3 dir) : pos(pos), dir(dir)
    {
    }
};

struct Contact
{
    string tag;

    Vector3 hitPoint;
    float distance;
};

class Collider : public GameObject
{
public:
    enum Type
    {
        BOX, SPHERE, CAPSULE
    };

    enum Collider_Role {
        BLOCK,
        OPEN
    };

    enum Collider_Special {
        NONE,
        TYPE_Z_OVER, //Z축기준 콜라이더 포지션보다 큰 부분에서만 등반 가능
        TYPE_Z_UNDER, //Z축기준 콜라이더 포지션보다 작은 부분에서만 등반 가능
        TYPE_X_OVER,
        TYPE_X_UNDER
    };

    const float PUSH_SPEED = 5.0f;

public:
    Collider();
    ~Collider();

    void Render();

    bool IsCollision(Collider* collider);

    virtual bool IsRayCollision(IN Ray ray, OUT Contact* contact = nullptr) = 0;
    virtual bool IsBoxCollision(class BoxCollider* collider) = 0;
    virtual bool IsSphereCollision(class SphereCollider* collider) = 0;
    virtual bool IsCapsuleCollision(class CapsuleCollider* collider) = 0;

    bool PushCollision(Collider* collider);
    bool PushCollision(Collider* collider, vector<Vector3>* dirs);
    
    void SetColor(Float4 color) { material->GetData().diffuse = color; }
    void SetColor(float r, float g, float b)
    {
        material->GetData().diffuse = { r, g, b, 1 };
    }

    static void RenderOnOff() { isRender = !isRender; }

    virtual Vector3 GetHalfSize() { return Vector3(0, 0, 0); };

    Type GetType() { return type; }

    Collider_Role& Role() { return role; }
    Collider_Special& Special() { return special; }

    void SetPickFlag(bool flag) { this->pickFlag = flag; }

    bool IsPickFlag() { return pickFlag; }
    bool IsTrustedRelation(Vector3 playPos);

    void SetPickContact(Contact con) { this->pickContact = con; }
    Contact GetPickContact() { return pickContact; }
    Vector3 GetCollisionPoint() { return CollisionPoint; }
    void ResetCollisionPoint() { CollisionPoint = {}; }
private:
    virtual void MakeMesh() = 0;

protected:
    Type type;    

    Mesh<Vertex>* mesh;
    Collider_Role role;
    Collider_Special special;
    bool pickFlag = false;
    Contact pickContact;
    Vector3 CollisionPoint;

private:
    static bool isRender;
};