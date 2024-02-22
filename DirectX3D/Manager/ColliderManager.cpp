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

	for (Collider* collider : obstacles)
	{
		if(collider->PushCollision(playerCollider))
			return false;

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

	return true;
}
