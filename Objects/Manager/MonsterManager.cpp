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
    orcInstancing->ReadClip("character1@dodge"); //�ϻ���ϱ�

    // �Ʒ� 3���� 1�� ������� ����
    orcInstancing->ReadClip("character1@atack22");
    orcInstancing->ReadClip("character1@atack23");
    orcInstancing->ReadClip("character1@atack24");

    orcInstancing->ReadClip("character1@atack16");// �����̴�����
    //orcInstancing->ReadClip("character1@atack4");

    orcInstancing->ReadClip("Orc_Death");

    // ���� ����
    FOR(SIZE)
    {
        CreateOrc();
    }

    //Ư��Ű �߰�
    {
        SpecialKeyUI sk;
        Quad* quad = new Quad(Vector2(100, 50));
        quad->GetMaterial()->SetShader(L"Basic/Texture.hlsl");
        quad->GetMaterial()->SetDiffuseMap(L"Textures/UI/SpecialKeyUI_ass.png");
        sk.name = "assassination";
        sk.key = 'Z';
        sk.quad = quad;
        sk.active = false;
        specialKeyUI.insert(make_pair(sk.name, sk));
    }
    
    FOR(2) blendState[i] = new BlendState();
    FOR(2) depthState[i] = new DepthStencilState();
    blendState[1]->AlphaToCoverage(true); //������ ���� + ���� ó���� ������ ���� ����
    depthState[1]->DepthWriteMask(D3D11_DEPTH_WRITE_MASK_ALL);  // �� ������

}

MonsterManager::~MonsterManager()
{
    delete orcInstancing;

    for (pair<int,OrcInfo> item : orcs)
        delete item.second.orc;
}

void MonsterManager::CreateOrc()
{
    OrcInfo orcInfo;
    Transform* transform = new Transform();
    transform->SetActive(false);
    Orc* orc = new Orc(transform, orcInstancing, orcIndex);
    orcInstancing->AddOrc(transform, orcIndex);

    ColliderManager::Get()->PushCollision(ColliderManager::ORC, orc->GetCollider());

    orcInfo.orc = orc;
    orcInfo.scale=Vector3(0, 0, 0);
    orcInfo.rot=Vector3(0, 0, 0);
    orcInfo.position=Vector3(0, 0, 0);
    
    orcs.insert(make_pair(orcIndex,orcInfo));
    orc->Spawn(orcInfo.position);

    orcIndex++;
}

void MonsterManager::Update()
{
    Collision();
    

    int i = 0;

    
    for (const pair<int, OrcInfo>& item : orcs)
    {
        if(item.second.isActive)
            item.second.orc->Update();
    }

    /*for (pair<int, OrcInfo>item : orcs)
    {
        if(item.second.orc->GetIsDelete())
            MonsterManager::Get()->DieOrc(item.first);
    }*/


    /*orcs[0]->Update();
    orcs[1]->Update();*/
  //  for (Orc* orc : orcs)
  //  {
  //
  //
  //
  //      
  //      orc->Update();
  //      i++;
  //  }

    for (const pair<int, OrcInfo>& item : orcs)
    {
        if (item.second.orc->IsFindTarget()) // ��ũ�� �߰߻��¶��
        {
            for (pair<int, OrcInfo> nearItem : orcs)
            {
                if (item.first != nearItem.first) // �ڱ� �ڽ� ����
                {
                    float dis = Distance(item.second.orc->GetTransform()->GlobalPos(), nearItem.second.orc->GetTransform()->GlobalPos());

                    if (dis < 20.0f) // �Ÿ��� 20 �̸��̸�
                    {
                        nearItem.second.orc->AttackTarget();
                    }
                }
            }
        }
    }
    orcInstancing->Update();
    vecDetectionPos.clear();
}

void MonsterManager::Render()
{
    orcInstancing->Render();
    for (const pair<int, OrcInfo>& item : orcs) item.second.orc->Render();
}

void MonsterManager::PostRender()
{
    for (const pair<int, OrcInfo>& item : orcs) item.second.orc->PostRender();

    //Ư��Ű ���
    for (pair<const string, SpecialKeyUI>& iter : specialKeyUI) {

        if (iter.second.active)
        {
            iter.second.quad->Render();
        }
    }
}

void MonsterManager::GUIRender()
{
    for (const pair<int, OrcInfo>& item : orcs) item.second.orc->GUIRender();
}

void MonsterManager::SetTarget(Transform* target)
{
    this->target = target;
    for (const pair<int, OrcInfo>& item : orcs)
        item.second.orc->SetTarget(target);
}

void MonsterManager::SetTargetCollider(CapsuleCollider* collider)
{
    for (const pair<int, OrcInfo>& item : orcs)
        item.second.orc->SetTargetCollider(collider);
}

bool MonsterManager::IsCollision(Ray ray, Vector3& hitPoint)
{
    //�ܺ� ȣ���

    Contact contact;
    float minDistance = FLT_MAX;

    for (const pair<int, OrcInfo>& item : orcs)
    {
        if (item.second.orc->GetCollider()->IsRayCollision(ray, &contact))
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
    orcs[index].orc->SetSRT(scale, rot, pos);
    orcs[index].orc->SetStartPos(pos);
}

void MonsterManager::SetPatrolPos(UINT idx, Vector3 Pos)
{
    orcs[idx].orc->SetPatrolPos(Pos);
}



void MonsterManager::AddOrcObstacleObj(Collider* collider)
{
    for (const pair<int, OrcInfo>& item : orcs)
    {
        item.second.orc->AddObstacleObj(collider);
    }
}

void MonsterManager::SetTerrain(LevelData* terrain)
{
    for (const pair<int, OrcInfo>& item : orcs)
    {
        item.second.orc->SetTerrain(terrain);
    }
}

void MonsterManager::SetAStar(AStar* astar)
{
    for (const pair<int, OrcInfo>& item : orcs)
    {
        item.second.orc->SetAStar(astar);
    }
}

void MonsterManager::Blocking(Collider* collider)
{
    for (const pair<int, OrcInfo>& item : orcs)
    {
        if (collider->IsCollision(item.second.orc->GetCollider()))
        {
            Vector3 dir = item.second.orc->GetCollider()->GlobalPos() - collider->GlobalPos();

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

            // --- ���� ���� �ڵ� : ����� �������κ��� �и� ---

            if (NearlyEqual(dir.y, 1.0f)) continue; // ������ ���� ���

            item.second.orc->GetTransform()->Pos() += dir * 50.0f * DELTA;

        }
    }

}

void MonsterManager::Fight(Player* player)
{

    // �÷��̾��� ����, ��,�� �ݶ��̴��� ���Ϳ� ��ҳ�
    for (Collider* collider : player->GetWeaponColliders())
    {
        for (const pair<int, OrcInfo>& item : orcs)
        {
            //���Ͱ� ������
            if (collider)
            {
                if (collider->Active() && collider->IsCapsuleCollision(item.second.orc->GetCollider())) //�� �浹ü�� Ÿ���̶� ��ĥ��
                {
                    item.second.orc->Hit(player->GetDamage(), collider->GlobalPos());
                }
            }

        }
    }
    // bow �ݸ������

    // ��ũ�� ���� �÷��̾�� ��ҳ�
    for (const pair<int, OrcInfo>& item : orcs)
    {
        for (Collider* collider : item.second.orc->GetWeaponColliders())
        {
            if (collider)
            {
                if (collider->Active() && collider->IsCapsuleCollision(player->GetCollider())) //��ũ�� ���� �浹ü�� Ÿ���̶� ��ĥ��
                {
                    player->Hit(item.second.orc->GetDamage());
                }
            }
        }

    }
    // ������ �ݶ��̴� ����
    KunaiManager::Get()->IsCollision();
}

void MonsterManager::CalculateDistance()
{
    for (const pair<int, OrcInfo>& item : orcs)
    {
        if (item.second.orc->FindTarget()) continue;
        else {
            Vector3 pos;
            for (UINT i = 0; i < vecDetectionPos.size(); ++i) {
                pos.x = vecDetectionPos[i].x;   
                pos.y = vecDetectionPos[i].y;
                pos.z = vecDetectionPos[i].z;
                if (Distance(pos, item.second.orc->GetTransform()->GlobalPos()) <= vecDetectionPos[i].w) {
                    item.second.orc->Findrange();
                }
            }
        }
    }
}

void MonsterManager::OnOutLineByRay(Ray ray)
{
    float minDistance = FLT_MAX;
    Orc* targetOrc = nullptr;

    for (const pair<int, OrcInfo>& item : orcs)
    {
        Contact con;
        if (item.second.orc->GetCollider()->IsRayCollision(ray, &con))
        {
            float hitDistance = Distance(con.hitPoint, ray.pos);
            if (minDistance > hitDistance)
            {
                minDistance = hitDistance;
                targetOrc = item.second.orc;
            }  
        }
        item.second.orc->SetOutLine(false);
    }

    if (targetOrc)
    {
        targetOrc->SetOutLine(true);
    }
}

void MonsterManager::ActiveSpecialKey(Vector3 playPos,Vector3 offset)
{
    for (pair<const string, SpecialKeyUI>& iter : specialKeyUI) {

        iter.second.active = false;
        //iter.second.quad->Pos() = { 0,0,0 };
        //iter.second.quad->UpdateWorld();
    }

    for (const pair<int, OrcInfo>& item : orcs)
    {
        float dis = Distance(item.second.orc->GetTransform()->GlobalPos(), playPos);
        if (!item.second.orc->GetIsDying() && item.second.orc->IsOutLine() && !item.second.orc->IsDetectTarget() && dis < 6.f)
        {
            //�ƿ������� Ȱ��ȭ�ǰ�, �÷��̾ �߰����� ������ ��, �Ÿ��� 6 �����϶�
            SpecialKeyUI& sk = specialKeyUI["assassination"];
            sk.active = true;
            sk.quad->Pos() = CAM->WorldToScreen(item.second.orc->GetTransform()->GlobalPos()+ offset);
            sk.quad->UpdateWorld();

            InteractManager::Get()->ActiveSkill("assassination",sk.key, bind(&InteractManager::Assassination, InteractManager::Get(), item.second.orc));
            /*sk.active = false;
            sk.quad->UpdateWorld();*/
        }
    }
}

void MonsterManager::DieOrc(int index)
{
    //delete orcs[index].orc;
    //orcs.erase(index);
    orcs[index].isActive = false;
}

void MonsterManager::Collision()
{
    for (const pair<int, OrcInfo>& item : orcs)
    {

    }
}


void MonsterManager::SetType(int index, Orc::NPC_TYPE _type) {
    orcs[index].orc->SetType(_type);
}