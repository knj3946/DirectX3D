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

void ArrowManager::Throw(Vector3 pos, Vector3 dir)
{
	for (Arrow* arrow : arrows)
	{
		// ���� �� ���� ������ ó������ �Ǻ��ؼ� �ϳ��� ������ �ٷ� ����
		if (!arrow->GetTransform()->Active())
		{
			arrow->Throw(pos, dir);
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