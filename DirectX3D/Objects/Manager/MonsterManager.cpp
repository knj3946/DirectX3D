#include "Framework.h"
#include "MonsterManager.h"

MonsterManager::MonsterManager()
{
    orcInstancing = new ModelAnimatorInstancing("character1");
    orcInstancing->ReadClip("Orc_Idle");
    orcInstancing->ReadClip("Orc_Walk");
    orcInstancing->ReadClip("Orc_Run");
    orcInstancing->ReadClip("Orc_Hit");
    //orcInstancing->ReadClip("Orc_Attack");
    orcInstancing->ReadClip("character1@atack4");
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

    
    for (Orc* orc : orcs)
        orc->Update();


    for (Orc* orc : orcs)
    {
        if (orc->IsFindTarget()) // ��ũ�� �߰߻��¶��
        {
            for (Orc* nearOrc : orcs)
            {
                if (orc != nearOrc) // �ڱ� �ڽ� ����
                {
                    float dis = Distance(orc->GetTransform()->GlobalPos(), nearOrc->GetTransform()->GlobalPos());

                    if (dis < 20.0f) // �Ÿ��� 20 �̸��̸�
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

bool MonsterManager::IsCollision(Ray ray, Vector3& hitPoint)
{
    //�ܺ� ȣ���

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

void MonsterManager::SetOrcSRT(int index,Vector3 scale, Vector3 rot, Vector3 pos)
{
    orcs[index]->SetSRT(scale,rot,pos);
    orcs[index]->SetStartPos(pos);
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

            // --- ���� ���� �ڵ� : ����� ������κ��� �и� ---

            if (NearlyEqual(dir.y, 1.0f)) continue; // ������ ���� ���

            orc->GetTransform()->Pos() += dir * 50.0f * DELTA;

        }
    }
    
}

void MonsterManager::Fight(Naruto* player)
{
    for (Collider* collider : static_cast<Naruto*>(player)->GetWeaponColliders())
    {
        for (Orc* orc : orcs)
        {
            //���Ͱ� ������
            if (collider)
            {
                if (collider->IsCapsuleCollision(orc->GetCollider()) && player->GetCurAttackCoolTime() == 0) //�� �浹ü�� Ÿ���̶� ��ĥ�� //���� ���� �ϳ��� �ѹ��� Ÿ�ݸ� �ؾ���
                {
                    player->SetAttackCoolDown();
                    orc->Hit();
                }
                else
                {
                    player->FillAttackCoolTime();
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
            for (UINT i = 0; i < vecDetectionPos.size(); ++i) {
                if (Distance(vecDetectionPos[i], p->GetTransform()->GlobalPos()) <= 500) {
                    p->Findrange();
                }
            }
        }
    }
}

void MonsterManager::Collision()
{
    for (Orc* orc : orcs)
    {

    }
}
