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

	kunaiInstancing->Update(); // �� ������Ʈ
	for (Kunai* kunai : kunaies)
		kunai->Update(); // �����͵� ������Ʈ

}

void KunaiManager::Render()
{
	kunaiInstancing->Render(); // �� render
	for (Kunai* kunai : kunaies)
		kunai->Render();	// ������ render
				// ���� ȣ��� ������ render�� ���� �ʿ����.(�������� �浹ü ������)
}

void KunaiManager::Throw(Vector3 pos, Vector3 dir)
{
	for (Kunai* kunai : kunaies)
	{
		// ���� �� ���� ������ ó������ �Ǻ��ؼ� �ϳ��� ������ �ٷ� ����
		if (!kunai->GetTransform()->Active())
		{
			kunai->Throw(pos, dir);
			return;
		}
	}
}

bool KunaiManager::IsCollision()
{
    // �÷��̾�� �ε����� üũ
	for (Kunai* kunai : kunaies)
	{
		// �÷��̾�� �ε�����
		if (ColliderManager::Get()->GetPlayer()->GetCollider()->IsCollision(kunai->GetCollider()))
		{
			kunai->GetTransform()->SetActive(false);
			ColliderManager::Get()->GetPlayer()->Hit(kunai->GetDamage());
			return true;
		}
	}
    // �ٸ� ��ֹ��� �ε����� üũ
	// ���� �������ٸ� ��濡 ���� ��ֹ��� ���� �ݶ��̴��� �ȵȴٸ� 
	// pos�� ��濡 �ε����� �Ǵ��� �߰��� ��
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