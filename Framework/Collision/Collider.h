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

    const float PUSH_SPEED = 10.0f;

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
    
    void SetColor(Float4 color) { material->GetData().diffuse = color; }
    void SetColor(float r, float g, float b)
    {
        material->GetData().diffuse = { r, g, b, 1 };
    }

    static void RenderOnOff() { isRender = !isRender; }

    virtual Vector3 GetHalfSize() { return Vector3(0, 0, 0); };

    Type GetType() { return type; }

    Collider_Role& Role() { return role; }
private:
    virtual void MakeMesh() = 0;

protected:
    Type type;    

    Mesh<Vertex>* mesh;
    Collider_Role role;

private:
    static bool isRender;
};