#include "Framework.h"

ArrowManager::ArrowManager()
{
	arrowInstancing = new ModelInstancing("arrow");

	arrows.reserve(SIZE);
	FOR(SIZE)
	{
		Transform* transform = arrowInstancing->Add();
		transform->SetActive(false);
		Arrow* arrow = new Arrow(transform,count++);
		arrows.push_back(arrow);
		ColliderManager::Get()->PushCollision(ColliderManager::Collision_Type::ARROW, arrow->GetCollider());
	}

	// 임시로 떨어진 화살들 세팅


	Transform* transform = arrowInstancing->Add(true);
	transform->SetActive(true);
	Arrow* arrow = new Arrow(transform, count++,true);
	arrow->GetTransform()->Pos() = Vector3(60, 5, 90);
	arrow->GetTransform()->Scale() *= 3;
	arrows.push_back(arrow);
	ColliderManager::Get()->PushCollision(ColliderManager::Collision_Type::ARROW, arrow->GetCollider());


	wallColiders = ColliderManager::Get()->Getvector(ColliderManager::Collision_Type::WALL);

	//특수키 추가
	{
		SpecialKeyUI sk;
		Quad* quad = new Quad(Vector2(100, 50));
		quad->GetMaterial()->SetShader(L"Basic/Texture.hlsl");
		quad->GetMaterial()->SetDiffuseMap(L"Textures/UI/SpecialKeyUI_dropItem.png");
		sk.name = "getItem";
		sk.quad = quad;
		sk.active = false;
		specialKeyUI.insert(make_pair(sk.name, sk));
	}
}

ArrowManager::~ArrowManager()
{
	delete arrowInstancing;
	for (Arrow* arrow : arrows)
		delete arrow;


	for (pair<string, SpecialKeyUI> key : specialKeyUI)
	{
		delete key.second.quad;
	}
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

void ArrowManager::PostRender()
{
	//특수키 출력
	for (pair<const string, SpecialKeyUI>& iter : specialKeyUI) {

		if (iter.second.active)
		{
			iter.second.quad->Render();
		}
	}
}

void ArrowManager::Throw(Vector3 pos, Vector3 dir)
{
	for (Arrow* arrow : arrows)
	{
		// 아직 안 던진 순번을 처음부터 판별해서 하나만 던지고 바로 종료
		// 떨어져 있지 않은 화살이면
		if (!arrow->GetTransform()->Active()&& !arrow->IsDropItem())
		{
			arrow->Throw(pos, dir);
			playerArrowCount--;
			return;
		}
	}
}

void ArrowManager::Throw(Vector3 pos, Vector3 dir, float chargingT)
{
	for (Arrow* arrow : arrows)
	{
		// 아직 안 던진 순번을 처음부터 판별해서 하나만 던지고 바로 종료
		// 떨어져 있지 않은 화살이면
		if (!arrow->GetTransform()->Active()&& !arrow->IsDropItem())
		{
			arrow->Throw(pos, dir, chargingT);
			playerArrowCount--;
			return;
		}
	}
}

bool ArrowManager::IsCollision()
{
	// 다른 장애물과 부딪혔나 체크
	// 맵이 정해진다면 배경에 따라 장애물에 따라 콜라이더가 안된다면 
	// pos로 배경에 부딪혔나 판단을 추가할 것
	
	//vector<Collider*> monsterColliders = ColliderManager::Get()->Getvector(ColliderManager::Collision_Type::ORC);
	if (!bStart) { bStart = true; return false; }
	for (Arrow* arrow : arrows)
	{
		if (!arrow->GetTransform()->Active() || arrow->IsDropItem())continue;

		for (int i = 0; i < MonsterManager::Get()->GetSIZE(); i++)
		{
			Orc* orc = MonsterManager::Get()->GetOrc(i);

			if (orc->GetCollider()->IsSphereCollision(arrow->GetCollider()))
			{
				arrow->GetCollider()->SetActive(false);
				//arrow->HitEffectActive();
				orc->Hit(20, arrow->GetTransform()->GlobalPos());// 화살데미지 임시설정
				arrow->GetTrail()->SetActive(false);
				arrow->GetTransform()->SetActive(false);

				orc->SetRestorePos(ColliderManager::Get()->Getvector(ColliderManager::PLAYER)[0]->GlobalPos());

				return true;
			}
			
		}
		
		/*
		for(int i = 0; i < ColliderManager::Get()->Getvector(ColliderManager::Collision_Type::ORC).size(); i++)
		{
			if (ColliderManager::Get()->Getvector(ColliderManager::Collision_Type::ORC)[i]->IsSphereCollision(arrow->GetCollider()))
			{
				arrow->GetCollider()->SetActive(false);
				//arrow->HitEffectActive();
				ColliderManager::Get()->Getvector(ColliderManager::Collision_Type::ORC)[i]->GetParent()

				MonsterManager::Get()->GetOrc(i)->Hit(20, arrow->GetTransform()->GlobalPos());// 화살데미지 임시설정
				arrow->GetTrail()->SetActive(false);
				arrow->GetTransform()->SetActive(false);
				return true;
			}
		}
		*/
	}
	for (Arrow* arrow : arrows)
	{
		for (Collider* c : wallColiders)
		{
			if (c->IsCollision(arrow->GetCollider()))
			{
				arrow->GetCollider()->SetActive(false);
				arrow->GetTransform()->SetActive(false);
				arrow->GetTrail()->SetActive(false);
				arrow->WallEffectActive();
				return true;
			}
		}
	}
	return false;
	for (Arrow* arrow : arrows)
	{
		if (arrow->IsDropItem())continue;
		if (ColliderManager::Get()->Getvector(ColliderManager::BOSS)[0]->IsSphereCollision(arrow->GetCollider())) {
			arrow->GetCollider()->SetActive(false);
			ColliderManager::Get()->GetBoss()->Hit(50,  arrow->GetTransform()->GlobalPos(),false);
			arrow->HitEffectActive();
		
		}
	}



	return false;
}

void ArrowManager::OnOutLineByRay(Ray ray)
{
	float minDistance = FLT_MAX;
	Arrow* targetArrow = nullptr;

	for (Arrow* arrow : arrows)
	{
		Contact con;
		if (arrow->GetCollider()->IsRayCollision(ray, &con))
		{
			float hitDistance = Distance(con.hitPoint, ray.pos);
			if (minDistance > hitDistance)
			{
				minDistance = hitDistance;
				targetArrow = arrow;
			}
		}
		arrow->SetOutLine(false);
	}

	if (targetArrow)
	{
		targetArrow->SetOutLine(true);
	}
}

void ArrowManager::ActiveSpecialKey(Vector3 playPos, Vector3 offset, bool _btrue)
{
	for (pair<const string, SpecialKeyUI>& iter : specialKeyUI) {

		iter.second.active = false;
		//iter.second.quad->Pos() = { 0,0,0 };
		//iter.second.quad->UpdateWorld();
	}

	if (_btrue) {
		SpecialKeyUI& sk = specialKeyUI["getItem"];
		sk.active = true;
		sk.quad->Pos() = CAM->WorldToScreen(bow->Pos() + offset);
		sk.quad->UpdateWorld();
	}

	
}

void ArrowManager::ExecuteSpecialKey()// 아이템을 주웠을 때 하게될 동작
{
	bow->SetActive(false);
	
	for (pair<const string, SpecialKeyUI>& iter : specialKeyUI) {

		iter.second.active = false;
		//iter.second.quad->Pos() = { 0,0,0 };
		//iter.second.quad->UpdateWorld();
	}
}
