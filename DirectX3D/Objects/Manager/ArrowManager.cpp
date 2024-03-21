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

	// �ӽ÷� ������ ȭ��� ����


	Transform* transform = arrowInstancing->Add(true);
	transform->SetActive(true);
	Arrow* arrow = new Arrow(transform, count++,true);
	arrow->GetTransform()->Pos() = Vector3(60, 5, 90);
	arrow->GetTransform()->Scale() *= 3;
	arrows.push_back(arrow);
	ColliderManager::Get()->PushCollision(ColliderManager::Collision_Type::ARROW, arrow->GetCollider());


	wallColiders = ColliderManager::Get()->Getvector(ColliderManager::Collision_Type::WALL);

	//Ư��Ű �߰�
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
	arrowInstancing->Update(); // �� ������Ʈ
	for (Arrow* arrow : arrows)
		arrow->Update(); // �����͵� ������Ʈ
}

void ArrowManager::Render()
{
	arrowInstancing->Render(); // �� render
	for (Arrow* arrow : arrows)
		arrow->Render();	// ������ render
	// ���� ȣ��� ���� render�� ���� �ʿ����.(�������� �浹ü ������)
}

void ArrowManager::PostRender()
{
	//Ư��Ű ���
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
		// ���� �� ���� ������ ó������ �Ǻ��ؼ� �ϳ��� ������ �ٷ� ����
		// ������ ���� ���� ȭ���̸�
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
		// ���� �� ���� ������ ó������ �Ǻ��ؼ� �ϳ��� ������ �ٷ� ����
		// ������ ���� ���� ȭ���̸�
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
	// �ٸ� ��ֹ��� �ε����� üũ
	// ���� �������ٸ� ��濡 ���� ��ֹ��� ���� �ݶ��̴��� �ȵȴٸ� 
	// pos�� ��濡 �ε����� �Ǵ��� �߰��� ��
	
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
				orc->Hit(20, arrow->GetTransform()->GlobalPos());// ȭ�쵥���� �ӽü���
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

				MonsterManager::Get()->GetOrc(i)->Hit(20, arrow->GetTransform()->GlobalPos());// ȭ�쵥���� �ӽü���
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

void ArrowManager::ExecuteSpecialKey()// �������� �ֿ��� �� �ϰԵ� ����
{
	bow->SetActive(false);
	
	for (pair<const string, SpecialKeyUI>& iter : specialKeyUI) {

		iter.second.active = false;
		//iter.second.quad->Pos() = { 0,0,0 };
		//iter.second.quad->UpdateWorld();
	}
}
