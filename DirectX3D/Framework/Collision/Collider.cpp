#include "Framework.h"

bool Collider::isRender = true;

Collider::Collider() : GameObject(L"Basic/Collider.hlsl")
{
    mesh = new Mesh<Vertex>();   

    SetColor(0, 1, 0);
}

Collider::~Collider()
{
    if (nullptr != mesh) {
        delete mesh;
        mesh = nullptr;
    }
}

void Collider::Render()
{
    if (!Active()) return;
    if (!isRender) return;

    SetRender();

    mesh->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

bool Collider::IsCollision(Collider* collider)
{
    if (!Active()) return false;
    if (!collider->Active()) return false;

    switch (collider->type)
    {
    case Collider::Type::BOX:
        return IsBoxCollision((BoxCollider*)collider);
    case Collider::Type::SPHERE:
        return IsSphereCollision((SphereCollider*)collider);
    case Collider::Type::CAPSULE:
        return IsCapsuleCollision((CapsuleCollider*)collider);    
    }

    return false;
}

bool Collider::PushCollision(Collider* collider)
{
    if (!IsCollision(collider)) return false;

    // 비스듬하게 가운데에 부딪혔을때 가장자리로 밀리는 현상 있어서 수정
    //Vector3 dir = (collider->GlobalPos() - GlobalPos()).GetNormalized();
    //collider->GetParent()->Pos() += dir * PUSH_SPEED * DELTA;
    
    Vector3 dir = collider->GlobalPos() - GlobalPos();

    int maxIndex = 0;
    float maxValue = -99999.0f;

    for (int i = 0; i < 3; ++i)
    {
        Vector3 halfSize = GetHalfSize();

        if (i != 1)
        {
            if (abs(dir[i]) - abs(halfSize[i]) > maxValue)
            {
                maxIndex = i;
                maxValue = abs(dir[i]) - abs(halfSize[i]);
            }
        }
    }

    switch (maxIndex)
    {
    case 0: // x
        dir.x = dir.x > 0 ? 1.0f : -1.0f;
        dir.y = dir.y > 0 ? 0.1f : -0.1f; //콜라이더 양쪽에서 미는 끼임 방지
        dir.z = dir.z > 0 ? 0.1f : -0.1f;
        break;

    case 1: // y
        dir.x = 0;
        dir.y = dir.y > 0 ? 1.0f : -1.0f;
        dir.z = 0;
        break;

    case 2: // z
        dir.x = dir.x > 0 ? 0.1f : -0.1f;
        dir.y = dir.y > 0 ? 0.1f : -0.1f;
        dir.z = dir.z > 0 ? 1.0f : -1.0f;
        break;
    }

    
    //collider->GetParent()->Pos() += dir * PUSH_SPEED * DELTA;
    collider->GetParent()->Pos() += dir;

    return true;
}
bool Collider::PushCollision(Collider* collider, vector<Vector3>* dirs)
{
    if (!IsCollision(collider)) return false;

    // 비스듬하게 가운데에 부딪혔을때 가장자리로 밀리는 현상 있어서 수정
    //Vector3 dir = (collider->GlobalPos() - GlobalPos()).GetNormalized();
    //collider->GetParent()->Pos() += dir * PUSH_SPEED * DELTA;
    
    Vector3 dir = collider->GlobalPos() - GlobalPos();

    int maxIndex = 0;
    float maxValue = -99999.0f;

    for (int i = 0; i < 3; ++i)
    {
        Vector3 halfSize = GetHalfSize();

        if (i != 1)
        {
            if (abs(dir[i]) - abs(halfSize[i]) > maxValue)
            {
                maxIndex = i;
                maxValue = abs(dir[i]) - abs(halfSize[i]);
            }
        }
    }

    switch (maxIndex)
    {
    case 0: // x
        dir.x = dir.x > 0 ? 1.0f : -1.0f;
        dir.y = dir.y > 0 ? 0.1f : -0.1f; //콜라이더 양쪽에서 미는 끼임 방지
        dir.z = dir.z > 0 ? 0.1f : -0.1f;
        break;

    case 1: // y
        dir.x = 0;
        dir.y = dir.y > 0 ? 1.0f : -1.0f;
        dir.z = 0;
        break;

    case 2: // z
        dir.x = dir.x > 0 ? 0.1f : -0.1f;
        dir.y = dir.y > 0 ? 0.1f : -0.1f;
        dir.z = dir.z > 0 ? 1.0f : -1.0f;
        break;
    }

    
    dirs->push_back(dir);

    return true;
}

bool Collider::IsTrustedRelation(Vector3 playPos)
{
    bool r = false;
    switch (special)
    {
        case TYPE_Z_OVER:
        {
            if (GlobalPos().z < playPos.z)
            {
                r = true;
            }
            break;
        }   
        default:
            r = false;
    }
    return r;
}
