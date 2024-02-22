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

		{
			//if (collider->IsCapsuleCollision(playerCollider))
			//{
			//	if (KEY_PRESS('W'))
			//		vel -= player->Forward();
			//	
			//	if (KEY_PRESS('S'))
			//		vel -= player->Back();
			//	
			//	if (KEY_PRESS('A'))
			//		vel -= player->Left();
			//	
			//	if (KEY_PRESS('W'))
			//		vel -= player->Right();

			//	dir->x = vel.x;
			//	dir->y = vel.y;
			//	dir->z = vel.z;

			//	collider.
			//}
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