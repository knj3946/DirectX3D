#include "Framework.h"
#include "ColliderManager.h"

ColliderManager::ColliderManager()
{
}

ColliderManager::~ColliderManager()
{
}

void  ColliderManager::ControlPlayer(Vector3* dir)
{
	Vector3 prePos = player->Pos();
	Vector3 vel = { dir->x, dir->y, dir->z };

	for (Collider* collider : obstacles)
	{
		playerCollider->Pos() += vel.x * 20 * DELTA * -1; // 이동 수행

		if (collider->IsCapsuleCollision(playerCollider)) {
						


		}
		
		playerCollider->Pos() += vel.z * 20 * DELTA * -1; // 이동 수행

		if (collider->IsCapsuleCollision(playerCollider)) {
						


		}
	}
}
