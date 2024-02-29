#include "Framework.h"

ArrowManager::ArrowManager()
{
	arrowInstancing = new ModelInstancing("Kunai");

	arrows.reserve(SIZE);
	FOR(SIZE)
	{
		Transform* transform = arrowInstancing->Add();
		transform->SetActive(false);
		Arrow* arrow = new Arrow(transform,count++);
		arrows.push_back(arrow);
	}

	// �ӽ÷� ������ ȭ��� ����
	Transform* transform = arrowInstancing->Add(true);
	transform->SetActive(true);
	Arrow* arrow = new Arrow(transform, count++,true);
	arrow->GetTransform()->Pos() = Vector3(60, 5, 90);
	arrow->GetTransform()->Scale() *= 3;
	arrows.push_back(arrow);


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
	for (Arrow* arrow : arrows)
		delete arrow;

	delete arrowInstancing;
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
		if (!arrow->GetTransform()->Active() && arrow->IsDropItem())
		{
			arrow->Throw(pos, dir);
			playerArrowCount--;
			return;
		}
	}
}

bool ArrowManager::IsCollision()
{
	// �÷��̾�� �ε����� üũ
	for (Arrow* arrow : arrows)
	{
		//�ݶ��̴� �޴�����, ����� ���� ���Ͱ� ��� ������ ����
		//���߹ݺ������� �浹 üũ�ϱ�
	}
	// �ٸ� ��ֹ��� �ε����� üũ
	// ���� �������ٸ� ��濡 ���� ��ֹ��� ���� �ݶ��̴��� �ȵȴٸ� 
	// pos�� ��濡 �ε����� �Ǵ��� �߰��� ��
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

void ArrowManager::ActiveSpecialKey(Vector3 playPos, Vector3 offset)
{
	for (pair<const string, SpecialKeyUI>& iter : specialKeyUI) {

		iter.second.active = false;
		//iter.second.quad->Pos() = { 0,0,0 };
		//iter.second.quad->UpdateWorld();
	}

	for (Arrow* arrow : arrows)
	{
		float dis = Distance(arrow->GetTransform()->GlobalPos(), playPos);
		if (arrow->IsOutLine() &&  dis < 10.f)
		{
			//�ƿ������� Ȱ��ȭ�ǰ�, �Ÿ��� 10 �����϶�
			SpecialKeyUI& sk = specialKeyUI["getItem"];
			sk.active = true;
			sk.quad->Pos() = CAM->WorldToScreen(arrow->GetTransform()->Pos() + offset);
			sk.quad->UpdateWorld();
		}
	}
}

void ArrowManager::ExecuteSpecialKey()// �������� �ֿ����� �ϰԵ� ����
{
	for (Arrow* arrow : arrows)
	{
		if (arrow->IsOutLine())
		{
			arrow->GetItem();
			playerArrowCount++;
		}
	}
}
