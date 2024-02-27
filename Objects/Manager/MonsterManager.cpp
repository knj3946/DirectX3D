#include "Framework.h"
#include "MonsterManager.h"

MonsterManager::MonsterManager()
{
    orcInstancing = new ModelAnimatorInstancing("character1");
    orcInstancing->ReadClip("Orc_Idle");
    //orcInstancing->ReadClip("Orc_Walk");
    orcInstancing->ReadClip("character1@walk3");
    orcInstancing->ReadClip("Orc_Run");
    orcInstancing->ReadClip("Orc_Hit");
    
    // 아래 3개중 1개 모션으로 공격
    orcInstancing->ReadClip("character1@atack22");
    orcInstancing->ReadClip("character1@atack23");
    orcInstancing->ReadClip("character1@atack24");

    orcInstancing->ReadClip("character1@atack16");
    //orcInstancing->ReadClip("character1@atack4");

    orcInstancing->ReadClip("Orc_Death");

    FOR(SIZE)
    {
        scales.push_back(Vector3(0, 0, 0));
        rots.push_back(Vector3(0, 0, 0));
        positions.push_back(Vector3(0, 0, 0));
    }

    FOR(SIZE)
    {
        Transform* transform = orcInstancing->Add();
        transform->SetActive(false);
        Orc* orc = new Orc(transform, orcInstancing, i);
        ColliderManager::Get()->PushCollision(ColliderManager::ORC, orc->GetCollider());
        orcs.push_back(orc);
        orc->Spawn(positions[i]);
    }
}

MonsterManager::~MonsterManager()
{
    delete orcInstancing;

    for (Orc* orc : orcs)
        delete orc;
}

void MonsterManager::Update()
{
    Collision();
    orcInstancing->Update();

    int i = 0;


    orcs[0]->Update();
    orcs[1]->Update();
  //  for (Orc* orc : orcs)
  //  {
  //
  //
  //
  //      
  //      orc->Update();
  //      i++;
  //  }

    for (Orc* orc : orcs)
    {
        if (orc->IsFindTarget()) // 오크가 발견상태라면
        {
            for (Orc* nearOrc : orcs)
            {
                if (orc != nearOrc) // 자기 자신 제외
                {
                    float dis = Distance(orc->GetTransform()->GlobalPos(), nearOrc->GetTransform()->GlobalPos());

                    if (dis < 20.0f) // 거리가 20 미만이면
                    {
                        nearOrc->AttackTarget();
                    }
                }
            }
        }
    }

    vecDetectionPos.clear();
}

void MonsterManager::Render()
{
    orcInstancing->Render();
    for (Orc* orc : orcs) orc->Render();
}

void MonsterManager::PostRender()
{
    for (Orc* orc : orcs) orc->PostRender();
}

void MonsterManager::GUIRender()
{
    for (Orc* orc : orcs) orc->GUIRender();
}

void MonsterManager::SetTarget(Transform* target)
{
    this->target = target;
    for (Orc* orc : orcs)
        orc->SetTarget(target);
}

void MonsterManager::SetTargetCollider(CapsuleCollider* collider)
{
    for (Orc* orc : orcs)
        orc->SetTargetCollider(collider);
}

bool MonsterManager::IsCollision(Ray ray, Vector3& hitPoint)
{
    //외부 호출용

    Contact contact;
    float minDistance = FLT_MAX;

    for (Orc* orc : orcs)
    {
        if (orc->GetCollider()->IsRayCollision(ray, &contact))
        {
            if (contact.distance < minDistance)
            {
                minDistance = contact.distance;
                hitPoint = contact.hitPoint;
            }
        }
    }

    return minDistance != FLT_MAX;
}

void MonsterManager::SetOrcSRT(int index, Vector3 scale, Vector3 rot, Vector3 pos)
{
    orcs[index]->SetSRT(scale, rot, pos);
    orcs[index]->SetStartPos(pos);
}

void MonsterManager::SetPatrolPos(UINT idx, Vector3 Pos)
{
    orcs[idx]->SetPatrolPos(Pos);
}



void MonsterManager::AddOrcObstacleObj(Collider* collider)
{
    for (Orc* orc : orcs)
    {
        orc->AddObstacleObj(collider);
    }
}

void MonsterManager::SetTerrain(LevelData* terrain)
{
    for (Orc* orc : orcs)
    {
        orc->SetTerrain(terrain);
    }
}

void MonsterManager::SetAStar(AStar* astar)
{
    for (Orc* orc : orcs)
    {
        orc->SetAStar(astar);
    }
}

void MonsterManager::Blocking(Collider* collider)
{
    for (Orc* orc : orcs)
    {
        if (collider->IsCollision(orc->GetCollider()))
        {
            Vector3 dir = orc->GetCollider()->GlobalPos() - collider->GlobalPos();

            int maxIndex = 0;
            float maxValue = -99999.0f;

            for (int i = 0; i < 3; ++i)
            {

                Vector3 halfSize = ((BoxCollider*)collider)->GetHalfSize();

                if (i != 1)
                {
                    if (abs(dir[i]) - abs(halfSize[i]) > maxValue)
                    {
                        maxIndex = i;
                        maxValue = abs(dir[i]) - abs(halfSize[i]);
                    }
                }
            }

            switch (maxIndex)
            {
            case 0: // x
                dir.x = dir.x > 0 ? 1.0f : -1.0f;
                dir.y = 0;
                dir.z = 0;
                break;

            case 1: // y
                dir.x = 0;
                dir.y = dir.y > 0 ? 1.0f : -1.0f;;
                dir.z = 0;
                break;

            case 2: // z
                dir.x = 0;
                dir.y = 0;
                dir.z = dir.z > 0 ? 1.0f : -1.0f;;
                break;
            }

            dir.y = 0;

            // --- 이하 샘플 코드 : 대상은 블록으로부터 밀림 ---

            if (NearlyEqual(dir.y, 1.0f)) continue; // 법선이 밑인 경우

            orc->GetTransform()->Pos() += dir * 50.0f * DELTA;

        }
    }

}

void MonsterManager::Fight(Player* player)
{

    for (Collider* collider : player->GetWeaponColliders())
    {
        for (Orc* orc : orcs)
        {
            //몬스터가 맞을때
            if (collider)
            {
                if (collider->Active() && collider->IsCapsuleCollision(orc->GetCollider())) //손 충돌체가 타겟이랑 겹칠때
                {
                    orc->Hit(player->GetDamage());
                }
            }

        }
    }

    for (Orc* orc : orcs)
    {
        for (Collider* collider : orc->GetWeaponColliders())
        {
            if (collider)
            {
                if (collider->Active() && collider->IsCapsuleCollision(player->GetCollider())) //오크의 웨폰 충돌체가 타겟이랑 겹칠때
                {
                    player->Hit(orc->GetDamage());
                }
            }
        }

    }
}

void MonsterManager::CalculateDistance()
{
    for (auto p : orcs)
    {
        if (p->FindTarget()) continue;
        else {
            Vector3 pos;
            for (UINT i = 0; i < vecDetectionPos.size(); ++i) {
                pos.x = vecDetectionPos[i].x;   
                pos.y = vecDetectionPos[i].y;
                pos.z = vecDetectionPos[i].z;
                if (Distance(pos, p->GetTransform()->GlobalPos()) <= vecDetectionPos[i].w) {
                    p->Findrange();
                }
            }
        }
    }
}

void MonsterManager::OnOutLineByRay(Ray ray)
{
    float minDistance = FLT_MAX;
    Orc* targetOrc = nullptr;

    for (Orc* orc : orcs)
    {
        Contact con;
        if (orc->GetCollider()->IsRayCollision(ray, &con))
        {
            float hitDistance = Distance(con.hitPoint, ray.pos);
            if (minDistance > hitDistance)
            {
                minDistance = hitDistance;
                targetOrc = orc;
            }  
        }
        orc->SetOutLine(false);
    }

    if (targetOrc)
    {
        targetOrc->SetOutLine(true);
    }
}

void MonsterManager::Collision()
{
    for (Orc* orc : orcs)
    {

    }
}


void MonsterManager::SetType(int index, Orc::NPC_TYPE _type) {
    orcs[index]->SetType(_type);
}