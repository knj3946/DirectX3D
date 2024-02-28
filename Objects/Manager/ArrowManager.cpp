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
	arrowInstancing->Update(); // �� ������Ʈ
	for (Arrow* arrow : Arrows)
		arrow->Update(); // �����͵� ������Ʈ
}

void ArrowManager::Render()
{
	arrowInstancing->Render(); // �� render
	for (Arrow* arrow : Arrows)
		arrow->Render();	// ������ render
	// ���� ȣ��� ���� render�� ���� �ʿ����.(�������� �浹ü ������)
}

void ArrowManager::Throw(Vector3 pos, Vector3 dir)
{
	for (Arrow* arrow : Arrows)
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
	//if (ColliderManager::Get()->CollisionCheck(
	//	ColliderManager::Collision_Type::PLAYER,
	//	ColliderManager::Collision_Type::ORC_KUNAI))
	//{
		// player->hit() �����ϱ�

	//}

	// �ٸ� ��ֹ��� �ε����� üũ
	//if()


	//for (Kunai* kunai : kunaies)
	//{
	//    if (kunai->GetCollider()->IsCollision(collider))
	//    {
	//        //�Ѿ��� �¾��� ��, "�Ѿ���" ������ �ڵ带 �߰�
	//        
	//        particle->Play(kunai->GetCollider()->GlobalPos()); // �ű⼭ ��ƼŬ ���

	//        //���� �ڵ� : �浹 �� ������� �ϱ�
	//        kunai->GetTransform()->SetActive(false); // <-�� ���� ������ ����ź�� �ȴ�
	//        return true;
	//    }
	//}

	return false;
}
