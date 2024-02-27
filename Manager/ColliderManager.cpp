#include "Framework.h"
#include "ColliderManager.h"

ColliderManager::ColliderManager()
{
	footRay = new Ray();
	headRay = new Ray();

	headRay->dir = { 0, 1, 0 };
	footRay->dir = { 0, -1, 0 };
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
		if (obstacle != onBlock && obstacle->PushCollision(playerCollider) && !isPushed)
		{
			isPushed = true;
			break;
		}
	}

	player->SetIsPushed(isPushed);
}

void ColliderManager::SetHeight()
{
	onBlock = nullptr;

	maxHeight = 0.0f;

	headRay->pos = playerCollider->GlobalPos();
	footRay->pos = playerCollider->GlobalPos();

	headRay->pos.y = player->Pos().y + 6.2f;

	Contact underObj;

	for (Collider* obstacle : GetObstacles())
	{
		if (obstacle->IsCapsuleCollision(playerCollider) && obstacle->IsRayCollision(*headRay))
		{
			player->SetIsCeiling(true);
			continue;
		}
		else if (obstacle->IsRayCollision(*footRay, &underObj))
		{
			if (underObj.hitPoint.y > maxHeight)
			{
				maxHeight = underObj.hitPoint.y;
				onBlock = obstacle;
			}
		}
	}

	player->SetHeightLevel(maxHeight);
}

void ColliderManager::GuiRender()
{
	//ImGui::DragFloat3("fds", (float*)&rayHeight, 0.5f);
	ImGui::DragFloat3("fds", (float*)&headRay->pos, 0.5f);
}

//bool ColliderManager::ControlPlayer(Vector3* dir)
//{
//	maxHeight = 0.0f;
//
//	for (int i = 0; i < obstacles.size(); i++)
//	{
//		if (SetPlayerHeight(obstacles[i]))	//���̸� ���ϴ� �Լ��� �����ؾ��ϳ�?
//			continue;
//
//		if (obstacles[i]->PushCollision(playerCollider))	//���� ���� �ε����� �̻��� ��쿡�� return �����
//		{
//			player->SetHeightLevel(maxHeight);
//			return false;
//		}
//	}
//
//	player->SetHeightLevel(maxHeight);
//
//	return true;
//}
//
//bool ColliderManager::SetPlayerHeight(Collider* obstacle)
//{
//	Contact underObj;
//
//	if (playerFoot->pos.y < obstacle->Pos().y)
//		return false;
//
//	if (obstacle->IsRayCollision(*playerFoot, &underObj)) {
//		if (underObj.hitPoint.y > maxHeight) {
//			maxHeight = underObj.hitPoint.y;
//			return true;
//		}
//	}
//
//	return false;
//}

float ColliderManager::CloseRayCollisionColliderDistance(Ray ray)
{
	float minDistance = FLT_MAX;
	for (Collider* ob : GetObstacles())
	{
		Contact con;
		ob->IsRayCollision(ray, &con);

		if (abs(con.distance) > 0)
		{
			float hitDistance = Distance(con.hitPoint, ray.pos);
			if (minDistance > hitDistance)
				minDistance = hitDistance;
		}
	}

	return minDistance;
}
bool ColliderManager::CloseRayCollisionColliderContact(Ray ray,Contact& con)
{
	float minDistance = FLT_MAX;
	bool flag = false;
	for (Collider* ob : GetObstacles())
	{
		Contact tempCon;
		ob->IsRayCollision(ray, &tempCon);

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
	return flag;
}

ColliderModel* ColliderManager::CreateColliderModel(string mName, string mTag, Vector3 mScale, Vector3 mRot, Vector3 mPos)
{
	int colCount = 0;
	vector<Vector3> colPoss;
	vector<Vector3> colScales;
	vector<Vector3> colRots;

	if (mName == "building_V2")
	{
		mRot.x += XM_PIDIV2;
		colCount = 11;
		colPoss.push_back(Vector3(4.f, -1.6f, -1.5f));
		colPoss.push_back(Vector3(4.f, 1.6f, -1.5f));
		colPoss.push_back(Vector3(0.f, 0.f, -2.7f));
		colPoss.push_back(Vector3(0.3f, 2.5f, -1.5f));
		colPoss.push_back(Vector3(-3.9f, 0.f, -1.5f));
		colPoss.push_back(Vector3(-3.6f, -2.4f, -1.5f));
		colPoss.push_back(Vector3(-0.9f, -2.4f, -1.5f));
		colPoss.push_back(Vector3(3.7f, -2.4f, -1.5f));
		colPoss.push_back(Vector3(1.5f, -2.4f, -2.6f));

		colPoss.push_back(Vector3(1.5f, -2.4f, -1.6f));
		colPoss.push_back(Vector3(1.5f, -2.4f, -0.6f));

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

		colScales.push_back(Vector3(0.4f, 1.9f, 3.f));
		colScales.push_back(Vector3(0.4f, 1.9f, 3.f));
		colScales.push_back(Vector3(8.5f, 5.1f, 0.9f));
		colScales.push_back(Vector3(7.7f, 0.2f, 3.f));
		colScales.push_back(Vector3(0.6f, 5.1f, 3.2f));
		colScales.push_back(Vector3(1.1f, 0.4f, 3.f));
		colScales.push_back(Vector3(1.2f, 0.4f, 3.f));
		colScales.push_back(Vector3(0.9f, 0.4f, 3.f));
		colScales.push_back(Vector3(3.5f, 0.4f, 1.1f));
		colScales.push_back(Vector3(1.8f, 0.4f, 1.3f));
		colScales.push_back(Vector3(3.5f, 0.4f, 1.2f));

	}
	else if (mName == "building_V4")
	{
		mRot.x += XM_PIDIV2;
		colCount = 17;
		colPoss.push_back(Vector3(4.f, -1.6f, -1.5f));
		colPoss.push_back(Vector3(4.f, 1.6f, -1.5f));
		colPoss.push_back(Vector3(0.f, 0.f, -2.7f));
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
		colPoss.push_back(Vector3(-2.1f, 4.9f, -2.9f));

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
		colRots.push_back(Vector3(0.f, 0.f, 0.f));
		colRots.push_back(Vector3(0.f, 0.f, 0.f));
		colRots.push_back(Vector3(0.f, 0.f, 0.f));
		colRots.push_back(Vector3(0.f, 0.f, 0.f));

		colScales.push_back(Vector3(0.4f, 1.9f, 3.f));
		colScales.push_back(Vector3(0.4f, 1.9f, 3.f));
		colScales.push_back(Vector3(8.5f, 5.1f, 0.9f));
		colScales.push_back(Vector3(4.2f, 0.2f, 3.f));
		colScales.push_back(Vector3(0.6f, 9.8f, 3.2f));
		colScales.push_back(Vector3(8.4f, 0.4f, 1.f));
		colScales.push_back(Vector3(8.4f, 0.4f, 1.3f));
		colScales.push_back(Vector3(3.9f, 0.4f, 1.3f));
		colScales.push_back(Vector3(1.8f, 0.4f, 1.3f));
		colScales.push_back(Vector3(0.9f, 0.4f, 1.3f));
		colScales.push_back(Vector3(1.5f, 0.4f, 3.f));
		colScales.push_back(Vector3(1.3f, 0.4f, 3.f));
		colScales.push_back(Vector3(0.3f, 4.6f, 1.1f));
		colScales.push_back(Vector3(0.3f, 4.6f, 1.2f));
		colScales.push_back(Vector3(0.3f, 1.8f, 1.3f));
		colScales.push_back(Vector3(0.3f, 2.0f, 1.3f));
		colScales.push_back(Vector3(4.1f, 4.9f, 0.4));

	}
	else if (mName == "building_V5")
	{
		mRot.x += XMConvertToRadians(165);
		colCount = 12;
		colPoss.push_back(Vector3(4.f, -1.1f, -2.0f));
		colPoss.push_back(Vector3(4.f, -2.0f, 1.2f));
		colPoss.push_back(Vector3(0.f, -2.8f, -0.7f));
		colPoss.push_back(Vector3(0.f, -0.9f, -2.7f));
		colPoss.push_back(Vector3(-3.9f, -1.5f, -0.4f));
		colPoss.push_back(Vector3(-3.6f, -2.1f, 1.9f));
		colPoss.push_back(Vector3(1.3f, -3.1f, 1.7f));
		colPoss.push_back(Vector3(1.3f, -1.2f, 2.2f));
		colPoss.push_back(Vector3(1.5f, -2.2f, 1.9f));
		colPoss.push_back(Vector3(-0.9f, -2.2f, 1.9f));
		colPoss.push_back(Vector3(3.7f, -2.2f, 1.9f));
		colPoss.push_back(Vector3(0.f, -3.5f, 2.8f));

		colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
		colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
		colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
		colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
		colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
		colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
		colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
		colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
		colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
		colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
		colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
		colRots.push_back(Vector3(XMConvertToRadians(180), 0.f, 0.f));

		colScales.push_back(Vector3(0.4f, 1.9f, 3.f));
		colScales.push_back(Vector3(0.4f, 1.9f, 3.f));
		colScales.push_back(Vector3(8.5f, 5.1f, 0.4f));
		colScales.push_back(Vector3(8.3f, 0.4f, 3.1f));
		colScales.push_back(Vector3(0.6f, 5.1f, 3.2f));
		colScales.push_back(Vector3(1.2f, 0.4f, 3.2f));
		colScales.push_back(Vector3(5.8f, 0.4f, 0.9f));
		colScales.push_back(Vector3(5.8f, 0.4f, 1.2f));
		colScales.push_back(Vector3(1.8f, 0.4f, 1.2f));
		colScales.push_back(Vector3(1.2f, 0.4f, 1.2f));
		colScales.push_back(Vector3(0.9f, 0.4f, 1.2f));
		colScales.push_back(Vector3(8.0f, 0.1f, 2.4f));
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
		col->UpdateWorld();
		model->AddCollider(col);
	}

	for (Collider* collider : model->GetColliders())
	{
		SetObstacles(collider);
	}

	return model;
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
