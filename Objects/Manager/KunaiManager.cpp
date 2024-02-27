#include "Framework.h"
#include "KunaiManager.h"

KunaiManager::KunaiManager()
{
    kunaiInstancing = new ModelInstancing("Kunai");

    kunaies.reserve(SIZE);
    FOR(SIZE)
    {
        Transform* transform = kunaiInstancing->Add();
        transform->SetActive(false);
        Kunai* kunai = new Kunai(transform);
        Transform* startEdge = new Transform(); // �غ� �ϱ�
        Transform* endEdge = new Transform();
        Trail* trail = new Trail(L"Textures/Etc/sun.jpg", startEdge, endEdge, 20, 100);
        trail->SetActive(false);
        kunai->SetTrail(trail);
        kunaies.push_back(kunai);
    }

    //particle = new Sprite(L"Textures/Effect/explosion.png", 150, 150, 5, 3);

    //particle = new ParticleSystem("TextData/Particles/flowerHit.fx");
    //particle = new ParticleSystem("TextData/Particles/flowerHit2.fx");
    //particle = new ParticleSystem("TextData/Particles/flowerHit3.fx");
    //particle = new ParticleSystem("TextData/Particles/frizmHit.fx");
    particle = new ParticleSystem("TextData/Particles/frizmHit2.fx");
}

KunaiManager::~KunaiManager()
{
    //���� �����
    for (Kunai* kunai : kunaies)
        delete kunai;

    //�𵨵� �����
    delete kunaiInstancing;
    delete particle;
}

void KunaiManager::Update()
{
    kunaiInstancing->Update(); //�� ������Ʈ
    particle->Update();

    for (Kunai* kunai : kunaies)
    {
        if (kunai->GetTransform()->Active())
        {
            kunai->GetTrail()->GetStartEdge()->Pos() = kunai->GetTransform()->GlobalPos() + kunai->GetTransform()->Up() * 5.0f; // 20.0f : 10%ũ�� �ݿ�
            kunai->GetTrail()->GetEndEdge()->Pos() = kunai->GetTransform()->GlobalPos() - kunai->GetTransform()->Up() * 5.0f;

            kunai->GetTrail()->GetStartEdge()->UpdateWorld();
            kunai->GetTrail()->GetEndEdge()->UpdateWorld();
        }

        kunai->Update(); //trail update�� ����ִ�
    }
}

void KunaiManager::Render()
{
    kunaiInstancing->Render(); //�� Render
    //for (Kunai* kunai : kunaies) kunai->Render(); //�����͵� Render
                    //���� ȣ��� ����Render�� ������ �ʿ䰡 ����(�������� �浹ü ������)

    particle->Render();
    

    for (Kunai* kunai : kunaies)
    {
        if (kunai->GetTransform()->Active())
        {
            kunai->GetTrail()->Render();
        }
    }
}

void KunaiManager::Throw(Vector3 pos, Vector3 dir)
{
    for (Kunai* kunai : kunaies)
    {
        //���� �� ���� ������ ó������ �Ǻ��ؼ� �ϳ��� ������ �ٷ� ����
        if (!kunai->GetTransform()->Active())
        {
            kunai->Throw(pos, dir);
            kunai->GetTrail()->SetActive(true);

            return;
        }
    }
}

bool KunaiManager::IsCollision()
{
    // �÷��̾�� �ε����� üũ
    if (ColliderManager::Get()->CollisionCheck(
        ColliderManager::Collision_Type::PLAYER,
        ColliderManager::Collision_Type::ORC_KUNAI))
    {
        // player->hit() �����ϱ�
        
    }

    // �ٸ� ��ֹ��� �ε����� üũ
    if()


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
