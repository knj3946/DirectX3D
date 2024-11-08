#include "Framework.h"

KunaiManager::KunaiManager()
{
	kunaiInstancing = new ModelInstancing("Kunai");

	kunaies.reserve(SIZE);
	FOR(SIZE)
	{
		Transform* transform = kunaiInstancing->Add();
		transform->SetActive(false);
		Kunai* kunai = new Kunai(transform);
		kunaies.push_back(kunai);
	}
	wallColiders = ColliderManager::Get()->Getvector(ColliderManager::Collision_Type::WALL);
	
}

KunaiManager::~KunaiManager()
{
	for (Kunai* kunai : kunaies)
		delete kunai;

	delete kunaiInstancing;
}

void KunaiManager::Update()
{
	//IsCollision();

	kunaiInstancing->Update(); // 모델 업데이트
	for (Kunai* kunai : kunaies)
		kunai->Update(); // 데이터도 업데이트

}

void KunaiManager::Render()
{
	kunaiInstancing->Render(); // 모델 render
	for (Kunai* kunai : kunaies)
		kunai->Render();	// 데이터 render
				// 지금 호출된 쿠나이 render는 원래 필요없다.(어디까지나 충돌체 렌더용)
}

void KunaiManager::Throw(Vector3 pos, Vector3 dir)
{
	for (Kunai* kunai : kunaies)
	{
		// 아직 안 던진 순번을 처음부터 판별해서 하나만 던지고 바로 종료
		if (!kunai->GetTransform()->Active())
		{
			kunai->Throw(pos, dir);
			return;
		}
	}
}

bool KunaiManager::IsCollision()
{
    // 플레이어랑 부딪혔나 체크
	for (Kunai* kunai : kunaies)
	{
		// 플레이어랑 부딪혔나
		if (ColliderManager::Get()->GetPlayer()->GetCollider()->IsCollision(kunai->GetCollider()))
		{
			kunai->GetTransform()->SetActive(false);
			ColliderManager::Get()->GetPlayer()->Hit(kunai->GetDamage());
			return true;
		}
	}
    // 다른 장애물과 부딪혔나 체크
	// 맵이 정해진다면 배경에 따라 장애물에 따라 콜라이더가 안된다면 
	// pos로 배경에 부딪혔나 판단을 추가할 것
	for (Kunai* kunai : kunaies)
	{
		for (Collider* c : wallColiders)
		{
			if (c->IsCollision(kunai->GetCollider()))
			{
				kunai->GetTransform()->SetActive(false);
				kunai->GetTrail()->SetActive(false);
				return true;
			}
		}
	}


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
