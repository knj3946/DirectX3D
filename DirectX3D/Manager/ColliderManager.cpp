#include "Framework.h"
#include "ColliderManager.h"

ColliderManager::ColliderManager()
{
}

ColliderManager::~ColliderManager()
{
}

void ColliderManager::PushPlayer()
{
	bool isPushed = false;

	for (Collider* obstacle : obstacles)
		if (obstacle->PushCollision(playerCollider) && !isPushed)
			isPushed = true;

	player->SetIsPushed(isPushed);
}

void ColliderManager::SetHeight()
{
	Contact underObj;
	Vector3 rayPos = player->Pos();

	Ray upRay = Ray(playerCollider->GlobalPos(), {0, 1, 0});	//이거 dir 값이 대체 어떻게 해야할지 모르겠음, 플레이어 머리 위로 쏘면 천장에 부딪히는거 구현 완료
	upRay.pos.y += 137.0f;

	for (Collider* obstacle : obstacles)
	{
		if (obstacle->IsRayCollision(upRay))
			obstacle->SetColor({ 1, 0, 0, 0 });
		else
			obstacle->SetColor({ 0, 1, 0, 0 });

		obstacle->IsRayCollision(upRay);
		if (obstacle->IsCapsuleCollision(playerCollider) && obstacle->IsRayCollision(upRay))
		{
			player->SetIsCeiling(true);
			return;
		}
		else if (obstacle->IsRayCollision(*playerFoot, &underObj))
		{
			if (underObj.hitPoint.y > maxHeight)
				maxHeight = underObj.hitPoint.y;
		}
	}


}

void ColliderManager::GuiRender()
{
	ImGui::DragFloat3("fds", (float*)&rayHeight, 0.5f);
}

//bool ColliderManager::ControlPlayer(Vector3* dir)
//{
//	maxHeight = 0.0f;
//
//	for (int i = 0; i < obstacles.size(); i++)
//	{
//		if (SetPlayerHeight(obstacles[i]))	//높이를 정하는 함수를 따로해야하나?
//			continue;
//
//		if (obstacles[i]->PushCollision(playerCollider))	//여러 면이 부딪혀서 이상한 경우에는 return 지우기
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
	for (Collider* ob : obstacles)
	{
		Contact con;
		ob->IsRayCollision(ray, &con);

		float hitDistance = Distance(con.hitPoint, ray.pos);
		if (minDistance > hitDistance)
			minDistance = hitDistance;
	}

	return minDistance;
}
