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
        Transform* startEdge = new Transform(); // 준비만 하기
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
    //벡터 지우기
    for (Kunai* kunai : kunaies)
        delete kunai;

    //모델도 지우기
    delete kunaiInstancing;
    delete particle;
}

void KunaiManager::Update()
{
    kunaiInstancing->Update(); //모델 업데이트
    particle->Update();

    for (Kunai* kunai : kunaies)
    {
        if (kunai->GetTransform()->Active())
        {
            kunai->GetTrail()->GetStartEdge()->Pos() = kunai->GetTransform()->GlobalPos() + kunai->GetTransform()->Up() * 5.0f; // 20.0f : 10%크기 반영
            kunai->GetTrail()->GetEndEdge()->Pos() = kunai->GetTransform()->GlobalPos() - kunai->GetTransform()->Up() * 5.0f;

            kunai->GetTrail()->GetStartEdge()->UpdateWorld();
            kunai->GetTrail()->GetEndEdge()->UpdateWorld();
        }

        kunai->Update(); //trail update가 들어있다
    }
}

void KunaiManager::Render()
{
    kunaiInstancing->Render(); //모델 Render
    //for (Kunai* kunai : kunaies) kunai->Render(); //데이터도 Render
                    //지금 호출된 쿠나이Render는 원래는 필요가 없다(어디까지나 충돌체 렌더용)

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
        //아직 안 던진 순번을 처음부터 판별해서 하나만 던지고 바로 종료
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
    // 플레이어랑 부딪혔나 체크
    if (ColliderManager::Get()->CollisionCheck(
        ColliderManager::Collision_Type::PLAYER,
        ColliderManager::Collision_Type::ORC_KUNAI))
    {
        // player->hit() 실행하기
        
    }

    // 다른 장애물과 부딪혔나 체크
    if()


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
