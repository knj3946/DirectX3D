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
}

KunaiManager::~KunaiManager()
{
	for (Kunai* kunai : kunaies)
		delete kunai;

	delete kunaiInstancing;
}

void KunaiManager::Update()
{
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

bool KunaiManager::IsCollision(Collider* collider)
{
	for (Kunai* kunai : kunaies)
	{
		if (kunai->GetCollider()->IsCollision(collider))
		{
			//맞았을 때, 총알이 수행할 코드를 추가하면 됨


			// 샘플 코드 : 충돌 후 사라짐
			kunai->GetTransform()->SetActive(false);
			kunai->GetTrail()->SetActive(false);
			return true;
		}
	}
	return false;
}
