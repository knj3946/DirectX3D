#include "Framework.h"
#include "ColliderManager.h"

ColliderManager::ColliderManager()
{
}

ColliderManager::~ColliderManager()
{
	vecCol->clear();

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
