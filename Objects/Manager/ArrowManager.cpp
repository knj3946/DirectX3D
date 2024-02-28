#include "Framework.h"

ArrowManager::ArrowManager()
{
	arrowInstancing = new ModelInstancing("Kunai");

	Arrows.reserve(SIZE);
	FOR(SIZE)
	{
		Transform* transform = arrowInstancing->Add();
		transform->SetActive(false);
		Arrow* arrow = new Arrow(transform);
		Arrows.push_back(arrow);
	}
}

ArrowManager::~ArrowManager()
{
	for (Arrow* arrow : Arrows)
		delete arrow;

	delete arrowInstancing;
}

void ArrowManager::Update()
{
	arrowInstancing->Update(); // 모델 업데이트
	for (Arrow* arrow : Arrows)
		arrow->Update(); // 데이터도 업데이트
}

void ArrowManager::Render()
{
	arrowInstancing->Render(); // 모델 render
	for (Arrow* arrow : Arrows)
		arrow->Render();	// 데이터 render
	// 지금 호출된 쿠나이 render는 원래 필요없다.(어디까지나 충돌체 렌더용)
}

void ArrowManager::Throw(Vector3 pos, Vector3 dir)
{
	for (Arrow* arrow : Arrows)
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
	//if (ColliderManager::Get()->CollisionCheck(
	//	ColliderManager::Collision_Type::PLAYER,
	//	ColliderManager::Collision_Type::ORC_KUNAI))
	//{
		// player->hit() 실행하기

	//}

	// 다른 장애물과 부딪혔나 체크
	//if()


	//for (Kunai* kunai : kunaies)
	//{
	//    if (kunai->GetCollider()->IsCollision(collider))
	//    {
	//        //총알이 맞았을 때, "총알이" 수행할 코드를 추가
	//        
	//        particle->Play(kunai->GetCollider()->GlobalPos()); // 거기서 파티클 재생

	//        //샘플 코드 : 충돌 후 사라지게 하기
	//        kunai->GetTransform()->SetActive(false); // <-이 줄이 없으면 관통탄이 된다
	//        return true;
	//    }
	//}

	return false;
}
