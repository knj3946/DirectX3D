#include "Framework.h"
#include "ColliderManager.h"

ColliderManager::ColliderManager()
{
}

ColliderManager::~ColliderManager()
{
}

bool ColliderManager::ControlPlayer(Vector3* dir)
{
	maxHeight = 0.0f;

	for (int i = 0; i < obstacles.size(); i++)
	{
		if (SetPlayerHeight(obstacles[i]))	//���̸� ���ϴ� �Լ��� �����ؾ��ϳ�?
			continue;

		if (obstacles[i]->PushCollision(playerCollider))	//���� ���� �ε����� �̻��� ��쿡�� return �����
		{
			player->SetHeightLevel(maxHeight);
			return false;
		}
	}

	player->SetHeightLevel(maxHeight);

	return true;
}

bool ColliderManager::SetPlayerHeight(Collider* obstacle)
{
	Contact underObj;

	if (obstacle->IsRayCollision(*playerFoot, &underObj)) {
		if (underObj.hitPoint.y > maxHeight) {
			maxHeight = underObj.hitPoint.y;
			return true;
		}
	}

	return false;
}

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
