#include "Framework.h"
#include "ColliderManager.h"

ColliderManager::ColliderManager()
{
    footRay = new Ray();
    headRay = new Ray();

    headRay->dir = { 0, 1, 0 };
    footRay->dir = { 0, -1, 0 };



    //특수키 추가
    {
        SpecialKeyUI sk;
        Quad* quad = new Quad(Vector2(130, 50));
        quad->GetMaterial()->SetShader(L"Basic/Texture.hlsl");
        quad->GetMaterial()->SetDiffuseMap(L"Textures/UI/SpecialKeyUI_climb.png");
        sk.name = "climb";
        sk.key = 'F';
        sk.quad = quad;
        sk.active = false;
        specialKeyUI.insert(make_pair(sk.name, sk));
    }
    //{
    //    SpecialKeyUI sk;
    //    Quad* quad = new Quad(Vector2(130, 50));
    //    quad->GetMaterial()->SetShader(L"Basic/Texture.hlsl`");
    //    quad->GetMaterial()->SetDiffuseMap(L"Textures/UI/SpecialKeyUI_climb.png");
    //    sk.name = "newClimb";
    //    sk.key = 'F';
    //    sk.quad = nullptr;
    //    sk.active = false;
    //    specialKeyUI.insert(make_pair(sk.name, sk));
    //}
}

ColliderManager::~ColliderManager()
{
}

void ColliderManager::SetObstacles(Collider* obstacle)
{
    PushCollision(ColliderManager::WALL, obstacle);
}

vector<Collider*>& ColliderManager::GetObstacles()
{
    return Getvector(ColliderManager::WALL);
}

void ColliderManager::PushPlayer()
{
    bool isPushed = false;

    for (Collider* obstacle : GetObstacles())
    {
        if (obstacle->Role() == Collider::BLOCK)
        {
            if (obstacle != onBlock && obstacle->PushCollision(playerCollider) && !isPushed)
            {
                isPushed = true;
                break;
            }
        }
    }

    player->SetIsPushed(isPushed);
}

void ColliderManager::SetHeight()
{
    headRay->pos = playerCollider->GlobalPos();
    headRay->pos.y = player->Pos().y + 6.2f;
    footRay->pos = playerCollider->GlobalPos();

    Contact maxHeadPoint;
    float maxHeadHeight = 99999.f;
    player->SetHeadCrash(false);

    Contact underObj;
    maxHeight = 0.0f;
    onBlock = nullptr;

    for (Collider* obstacle : GetObstacles())
    {
        //위 물체의 높이가 너무 낮으면 스페이스 입력을 무시할지말지 결정하는 법도 고려
        if (obstacle->IsRayCollision(*headRay, &maxHeadPoint) && maxHeadPoint.distance < FLT_EPSILON)
        {
            player->SetHeadCrash(true);
        }
        else if (obstacle->IsRayCollision(*footRay, &underObj) && underObj.hitPoint.y > maxHeight)
        {
            maxHeight = underObj.hitPoint.y;
            onBlock = obstacle;
        }
    }

    player->SetHeightLevel(maxHeight);
}

void ColliderManager::Render()
{
}


void ColliderManager::PostRender()
{
    //특수키 출력
    for (pair<const string, SpecialKeyUI>& iter : specialKeyUI) {

        if (iter.second.active)
        {
            iter.second.quad->Render();
        }
    }
}

void ColliderManager::GuiRender()
{
    //ImGui::DragFloat3("fds", (float*)&rayHeight, 0.5f);
    ImGui::DragFloat3("fds", (float*)&headRay->pos, 0.5f);
    ImGui::Value("CamDistance", Distance(CAM->GlobalPos(), player->Pos()));
}

float ColliderManager::CloseRayCollisionColliderDistance(Ray ray)
{
    float minDistance = FLT_MAX;
    for (Collider* ob : GetObstacles())
    {
        Contact con;
        if (ob->IsRayCollision(ray, &con))
        {
            if (abs(con.distance) > 0)
            {
                float hitDistance = Distance(con.hitPoint, ray.pos);
                if (minDistance > hitDistance)
                    minDistance = hitDistance;
            }
        }
    }

    return minDistance;
}
bool ColliderManager::CloseRayCollisionColliderContact(Ray ray, Contact& con)
{
    float minDistance = FLT_MAX;
    bool flag = false;
    for (Collider* ob : GetObstacles())
    {
        Contact tempCon;
        if (ob->IsRayCollision(ray, &tempCon))
        {
            if (abs(tempCon.distance) > 0)
            {
                float hitDistance = Distance(tempCon.hitPoint, ray.pos);
                if (minDistance > hitDistance)
                {
                    con = tempCon;
                    minDistance = hitDistance;
                    flag = true;
                }
            }
        }
    }
    return flag;
}

float ColliderManager::CloseRayCollisionColliderDistance(Ray ray, Collider* _pCollider)
{
	float minDistance = FLT_MAX;
	
	Contact con;
	_pCollider->IsRayCollision(ray, &con);

	float hitDistance = Distance(con.hitPoint, ray.pos);
	if (minDistance > hitDistance)
		minDistance = hitDistance;
	

	return minDistance;
}

bool ColliderManager::CompareDistanceObstacleandPlayer(Ray ray)
{
	Collider* tmpCol=nullptr;
	for (Collider* ob : vecCol[WALL])
	{
		Contact con;
		if (ob->IsRayCollision(ray, &con))
		{
            
			tmpCol = ob;
			break;
		}
	}
	if (!tmpCol)
		return true;

	if (CloseRayCollisionColliderDistance(ray, tmpCol) >= CloseRayCollisionColliderDistance(ray, vecCol[Collision_Type::PLAYER][0])) {
		return true;
	}
	return false;


}

ColliderModel* ColliderManager::CreateColliderModel(string mName, string mTag, Vector3 mScale, Vector3 mRot, Vector3 mPos)
{
    int colCount = 0;
    vector<Vector3> colPoss;
    vector<Vector3> colScales;
    vector<Vector3> colRots;
    vector<Collider::Collider_Role> colRole;
    vector<Collider::Collider_Special> colSpecials;

    if (mName == "building_V2")
    {
        mRot.x += XM_PIDIV2;
        colCount = 13;
        colPoss.push_back(Vector3(4.f, -1.6f, -1.5f));
        colPoss.push_back(Vector3(4.f, 1.6f, -1.5f));
        colPoss.push_back(Vector3(0.f, 0.f, -2.7f)); // 바닥 콜라이더
        colPoss.push_back(Vector3(0.3f, 2.5f, -1.5f));
        colPoss.push_back(Vector3(-3.9f, 0.f, -1.5f));
        colPoss.push_back(Vector3(-3.6f, -2.4f, -1.5f));
        colPoss.push_back(Vector3(-0.9f, -2.4f, -1.5f));
        colPoss.push_back(Vector3(1.5f, -2.4f, -1.5f));
        colPoss.push_back(Vector3(3.7f, -2.4f, -1.5f));
        colPoss.push_back(Vector3(1.5f, -2.4f, -2.6f));
        colPoss.push_back(Vector3(1.5f, -2.4f, -1.6f));
        colPoss.push_back(Vector3(1.5f, -2.4f, -0.6f));
        colPoss.push_back(Vector3(0.f, 0.f, 0.23f)); //바닥 콜라이더


        colRots.push_back(Vector3(0.f, 0.f, 0.f));
        colRots.push_back(Vector3(0.f, 0.f, 0.f));
        colRots.push_back(Vector3(0.f, 0.f, 0.f)); // 바닥 콜라이더
        colRots.push_back(Vector3(0.f, 0.f, 0.f));
        colRots.push_back(Vector3(0.f, 0.f, 0.f));
        colRots.push_back(Vector3(0.f, 0.f, 0.f));
        colRots.push_back(Vector3(0.f, 0.f, 0.f));
        colRots.push_back(Vector3(0.f, 0.f, 0.f));
        colRots.push_back(Vector3(0.f, 0.f, 0.f));
        colRots.push_back(Vector3(0.f, 0.f, 0.f));
        colRots.push_back(Vector3(0.f, 0.f, 0.f));
        colRots.push_back(Vector3(0.f, 0.f, 0.f));
        colRots.push_back(Vector3(0.f, 0.f, 0.f)); //바닥 콜라이더

        colScales.push_back(Vector3(0.4f, 1.9f, 3.f));
        colScales.push_back(Vector3(0.4f, 1.9f, 3.f));
        colScales.push_back(Vector3(8.5f, 5.1f, 0.9f)); // 바닥 콜라이더
        colScales.push_back(Vector3(7.7f, 0.2f, 3.f));
        colScales.push_back(Vector3(0.6f, 5.1f, 3.2f));
        colScales.push_back(Vector3(1.1f, 0.4f, 3.f));
        colScales.push_back(Vector3(1.2f, 0.4f, 3.f));
        colScales.push_back(Vector3(1.8f, 0.4f, 3.f));
        colScales.push_back(Vector3(0.9f, 0.4f, 3.f));
        colScales.push_back(Vector3(3.9f, 0.4f, 1.1f));
        colScales.push_back(Vector3(1.8f, 0.4f, 1.3f));
        colScales.push_back(Vector3(3.7f, 0.4f, 1.2f));
        colScales.push_back(Vector3(8.5f, 5.1f, 0.9f)); //바닥 콜라이더

        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::OPEN); // 바닥 콜라이더
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::OPEN); //바닥 콜라이더

        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::TYPE_Z_OVER); //Z축기준 콜라이더 포지션보다 큰 부분에서만 등반 가능
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);

    }
    else if (mName == "building_V4")
    {
        mRot.x += XM_PIDIV2;
        colCount = 19;
        colPoss.push_back(Vector3(4.f, -1.6f, -1.5f));
        colPoss.push_back(Vector3(4.f, 1.6f, -1.5f));
        colPoss.push_back(Vector3(0.f, 0.f, -2.7f)); //바닥 콜라이더
        colPoss.push_back(Vector3(0.f, 0.f, 0.1f)); //바닥 콜라이더
        colPoss.push_back(Vector3(2.1f, 2.5f, -1.5f));
        colPoss.push_back(Vector3(-3.9f, 2.4f, -1.5f));
        colPoss.push_back(Vector3(0.f, -2.4f, -2.6f));
        colPoss.push_back(Vector3(0.f, -2.4f, -0.6f));
        colPoss.push_back(Vector3(-2.2f, -2.4f, -1.7f));
        colPoss.push_back(Vector3(1.5f, -2.4f, -1.7f));
        colPoss.push_back(Vector3(3.7f, -2.4f, -1.7f));
        colPoss.push_back(Vector3(-0.8f, 7.1f, -1.5f));
        colPoss.push_back(Vector3(-3.6f, 7.1f, -1.5f));
        colPoss.push_back(Vector3(-0.2f, 4.9f, -2.6f));
        colPoss.push_back(Vector3(-0.2f, 4.9f, -0.6f));
        colPoss.push_back(Vector3(-0.2f, 3.4f, -1.6f));
        colPoss.push_back(Vector3(-0.2f, 6.2f, -1.6f));
        colPoss.push_back(Vector3(-2.1f, 4.9f, -2.9f)); //바닥 콜라이더
        colPoss.push_back(Vector3(-2.2f, 4.9f, -0.1f)); //바닥 콜라이더

        colRots.push_back(Vector3(0.f, 0.f, 0.f));
        colRots.push_back(Vector3(0.f, 0.f, 0.f));
        colRots.push_back(Vector3(0.f, 0.f, 0.f));//바닥 콜라이더
        colRots.push_back(Vector3(0.f, 0.f, 0.f));//바닥 콜라이더
        colRots.push_back(Vector3(0.f, 0.f, 0.f));
        colRots.push_back(Vector3(0.f, 0.f, 0.f));
        colRots.push_back(Vector3(0.f, 0.f, 0.f));
        colRots.push_back(Vector3(0.f, 0.f, 0.f));
        colRots.push_back(Vector3(0.f, 0.f, 0.f));
        colRots.push_back(Vector3(0.f, 0.f, 0.f));
        colRots.push_back(Vector3(0.f, 0.f, 0.f));
        colRots.push_back(Vector3(0.f, 0.f, 0.f));
        colRots.push_back(Vector3(0.f, 0.f, 0.f));
        colRots.push_back(Vector3(0.f, 0.f, 0.f));
        colRots.push_back(Vector3(0.f, 0.f, 0.f));
        colRots.push_back(Vector3(0.f, 0.f, 0.f));
        colRots.push_back(Vector3(0.f, 0.f, 0.f));
        colRots.push_back(Vector3(0.f, 0.f, 0.f)); //바닥 콜라이더
        colRots.push_back(Vector3(0.f, 0.f, 0.f)); //바닥 콜라이더

	
        colScales.push_back(Vector3(0.4f, 1.9f, 3.f));
        colScales.push_back(Vector3(0.4f, 1.9f, 3.f));
        colScales.push_back(Vector3(8.5f, 5.1f, 0.9f)); //바닥 콜라이더
        colScales.push_back(Vector3(8.5f, 5.1f, 0.9f));//4 //바닥 콜라이더
        colScales.push_back(Vector3(4.2f, 0.2f, 3.f));
        colScales.push_back(Vector3(0.6f, 9.8f, 3.2f));
        colScales.push_back(Vector3(8.4f, 0.4f, 1.f));//7
        colScales.push_back(Vector3(8.4f, 0.4f, 1.3f));
        colScales.push_back(Vector3(3.9f, 0.4f, 1.3f));
        colScales.push_back(Vector3(1.8f, 0.4f, 1.3f));//10
        colScales.push_back(Vector3(0.9f, 0.4f, 1.3f));
        colScales.push_back(Vector3(1.5f, 0.4f, 3.f));
        colScales.push_back(Vector3(1.3f, 0.4f, 3.f));
        colScales.push_back(Vector3(0.3f, 4.6f, 1.1f));
        colScales.push_back(Vector3(0.3f, 4.6f, 1.2f));
        colScales.push_back(Vector3(0.3f, 1.8f, 1.3f));
        colScales.push_back(Vector3(0.3f, 2.0f, 1.3f));
        colScales.push_back(Vector3(4.1f, 4.9f, 0.4)); //바닥 콜라이더
        colScales.push_back(Vector3(4.1f, 4.9f, 0.4)); //바닥 콜라이더

        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::OPEN); //바닥 콜라이더
        colRole.push_back(Collider::OPEN); //바닥 콜라이더
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::OPEN); //바닥 콜라이더
        colRole.push_back(Collider::OPEN); //바닥 콜라이더

        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);

    }
    else if (mName == "building_V5")
    {
        mRot.x += XMConvertToRadians(165);
        colCount = 13;
        colPoss.push_back(Vector3(4.f, -1.1f, -2.0f));
        colPoss.push_back(Vector3(4.f, -2.0f, 1.2f));
        colPoss.push_back(Vector3(0.f, -2.8f, -0.7f)); //바닥 콜라이더
        colPoss.push_back(Vector3(0.f, 0.f, 0.1f)); //바닥 콜라이더
        colPoss.push_back(Vector3(0.f, -0.9f, -2.7f));
        colPoss.push_back(Vector3(-3.9f, -1.5f, -0.4f));
        colPoss.push_back(Vector3(-3.6f, -2.1f, 1.9f));
        colPoss.push_back(Vector3(1.3f, -3.1f, 1.7f));
        colPoss.push_back(Vector3(1.3f, -1.2f, 2.2f));
        colPoss.push_back(Vector3(1.5f, -2.2f, 1.9f));
        colPoss.push_back(Vector3(-0.9f, -2.2f, 1.9f));
        colPoss.push_back(Vector3(3.7f, -2.2f, 1.9f));
        colPoss.push_back(Vector3(0.f, -3.5f, 2.8f)); //바닥 콜라이더

        colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
        colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
        colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));//바닥 콜라이더
        colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));//바닥 콜라이더
        colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
        colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
        colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
        colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
        colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
        colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
        colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
        colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
        colRots.push_back(Vector3(XMConvertToRadians(180), 0.f, 0.f)); //바닥 콜라이더

        colScales.push_back(Vector3(0.4f, 1.9f, 3.f));
        colScales.push_back(Vector3(0.4f, 1.9f, 3.f));
        colScales.push_back(Vector3(8.5f, 5.1f, 0.4f));//바닥 콜라이더
        colScales.push_back(Vector3(8.5f, 5.1f, 0.4f));//바닥 콜라이더
        colScales.push_back(Vector3(8.3f, 0.4f, 3.1f));
        colScales.push_back(Vector3(0.6f, 5.1f, 3.2f));
        colScales.push_back(Vector3(1.2f, 0.4f, 3.2f));
        colScales.push_back(Vector3(5.8f, 0.4f, 0.9f));
        colScales.push_back(Vector3(5.8f, 0.4f, 1.2f));
        colScales.push_back(Vector3(1.8f, 0.4f, 1.2f));
        colScales.push_back(Vector3(1.2f, 0.4f, 1.2f));
        colScales.push_back(Vector3(0.9f, 0.4f, 1.2f));
        colScales.push_back(Vector3(8.0f, 0.1f, 2.4f)); //바닥 콜라이더

        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::OPEN);//바닥 콜라이더
        colRole.push_back(Collider::OPEN);//바닥 콜라이더
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::BLOCK);
        colRole.push_back(Collider::OPEN); //바닥 콜라이더

        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
        colSpecials.push_back(Collider::Collider_Special::NONE);
    }

    ColliderModel* model = new ColliderModel(mName);

    model->SetTag(mTag);
    model->Scale() = mScale;
    model->Pos() = mPos;
    model->Rot() = mRot;
    model->UpdateWorld();

    FOR(colCount)
    {
        Collider* col = new BoxCollider();
        string ctag = mTag + "_collider" + to_string(i);
        col->SetTag(ctag);
        col->SetParent(model);
        col->Scale() = colScales[i];
        col->Rot() = colRots[i];
        col->Pos() = colPoss[i];
        col->Role() = colRole[i];
        col->Special() = colSpecials[i];
        col->UpdateWorld();
        model->AddCollider(col);
    }

    for (Collider* collider : model->GetColliders())
    {
        SetObstacles(collider);
    }

    return model;
}

void ColliderManager::PopCollision(Collision_Type Type, Collider* collider)
{
    for (int i = 0; i < vecCol[Type].size(); i++)
    {
        if (collider == vecCol[Type][i])
        {
            vecCol[Type].erase(vecCol[Type].begin() + i);
            return;
        }
    }
}

bool ColliderManager::CollisionCheck(Collider* _pCollider, Collision_Type _type)
{
    for (int i = 0; i < vecCol[_type].size(); ++i) {
        if (_pCollider->IsCollision(vecCol[_type][i]))
        {
            return true;
        }
    }
    return false;
}

bool ColliderManager::CollisionCheck(Collision_Type _type1, Collision_Type _type2)
{
    for (int i = 0; i < vecCol[_type1].size(); ++i) {
        for (int j = 0; j < vecCol[_type2].size(); ++j) {
            if (vecCol[_type1][i]->IsCollision(vecCol[_type2][j]))
            {
                return true;
            }
        }
    }
    return false;
}

void ColliderManager::PickFlagByRay(Ray ray)
{
    float minDistance = FLT_MAX;
    Collider* targetCol = nullptr;

    for (Collider* col : GetObstacles())
    {
        Contact con;
        if (col->IsRayCollision(ray, &con))
        {
            float hitDistance = Distance(con.hitPoint, ray.pos);
            if (minDistance > hitDistance)
            {
                minDistance = hitDistance;
                targetCol = col;
            }
        }
        col->SetPickFlag(false);
        col->SetPickContact(con);
    }

    if (targetCol)
    {
        targetCol->SetPickFlag(true);
    }
}

void ColliderManager::ActiveSpecialKey(Vector3 playPos, Vector3 offset)
{
    ClearSpecialKey();

    for (Collider* col : GetObstacles())
    {
        float dis = Distance(col->GetPickContact().hitPoint, playPos);
        if (col->IsPickFlag() && col->IsTrustedRelation(playPos) && dis < 5.f)
        {
            SpecialKeyUI& sk = specialKeyUI["climb"];
            sk.active = true;
            sk.quad->Pos() = CAM->WorldToScreen(col->GetPickContact().hitPoint + offset);
            sk.quad->UpdateWorld();

            InteractManager::Get()->ActiveSkill("Climb", sk.key, bind(&InteractManager::Climb, InteractManager::Get(), col));
        }
    }

}

void ColliderManager::ClearSpecialKey()
{
    for (pair<const string, SpecialKeyUI>& iter : specialKeyUI) {

        iter.second.active = false;
    }
}
