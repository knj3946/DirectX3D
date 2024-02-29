#include "Framework.h"

ArrowManager::ArrowManager()
{
	arrowInstancing = new ModelInstancing("Kunai");

	arrows.reserve(SIZE);
	FOR(SIZE)
	{
		Transform* transform = arrowInstancing->Add();
		transform->SetActive(false);
		Arrow* arrow = new Arrow(transform);
		arrows.push_back(arrow);
	}
	wallColiders = ColliderManager::Get()->Getvector(ColliderManager::Collision_Type::WALL);
}

ArrowManager::~ArrowManager()
{
	for (Arrow* arrow : arrows)
		delete arrow;

	delete arrowInstancing;
}

void ArrowManager::Update()
{
	arrowInstancing->Update(); // 모델 업데이트
	for (Arrow* arrow : arrows)
		arrow->Update(); // 데이터도 업데이트
}

void ArrowManager::Render()
{
	arrowInstancing->Render(); // 모델 render
	for (Arrow* arrow : arrows)
		arrow->Render();	// 데이터 render
	// 지금 호출된 쿠나이 render는 원래 필요없다.(어디까지나 충돌체 렌더용)
}

void ArrowManager::Throw(Vector3 pos, Vector3 dir)
{
	for (Arrow* arrow : arrows)
	{
		// 아직 안 던진 순번을 처음부터 판별해서 하나만 던지고 바로 종료
		if (!arrow->GetTransform()->Active())
		{
			arrow->Throw(pos, dir);
			return;
		}
	}
}

bool ArrowManager::IsCollision()
{
	// 플레이어랑 부딪혔나 체크
	// 
	// 다른 장애물과 부딪혔나 체크
	// 맵이 정해진다면 배경에 따라 장애물에 따라 콜라이더가 안된다면 
	// pos로 배경에 부딪혔나 판단을 추가할 것
	for (Arrow* arrow : arrows)
	{
		for (Collider* c : wallColiders)
		{
			if (c->IsCollision(arrow->GetCollider()))
			{
				arrow->GetTransform()->SetActive(false);
				arrow->GetTrail()->SetActive(false);
				return true;
			}
		}
	}

	return false;
}
