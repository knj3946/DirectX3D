#include "Framework.h"
#include "TestNpcScene.h"

TestNpcScene::TestNpcScene()
{
    terrain = new TerrainEditor();

    aStar2 = new AStar();
    aStar2->SetNode(terrain);
    boss = new Boss;
    boss->SetAStar(aStar2);
    boss->SetTerrain(terrain);
    naruto = new Naruto();
    boss->SetTarget(naruto);
    boss->SetPatrolPos(Vector3(0, 0, 0));
    boss->SetPatrolPos(Vector3(100, 0, 100));
    ColliderManager::Get()->PushCollision(ColliderManager::PLAYER, naruto->GetCollider());

    MonsterManager::Get()->SetTarget(naruto); //�̱��� ���� ��, ǥ�� ��������

    MonsterManager::Get()->SetOrcSRT(0, Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(80, 0, 80));
    MonsterManager::Get()->SetType(0, Orc::NPC_TYPE::INFORM);
    MonsterManager::Get()->SetPatrolPos(0, Vector3(160, 0, 160));


    MonsterManager::Get()->SetOrcSRT(1, Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(60, 0, 150));
    MonsterManager::Get()->SetPatrolPos(1, Vector3(130, 0, 100));
    MonsterManager::Get()->SetType(1, Orc::NPC_TYPE::ATTACK);
    MonsterManager::Get()->SetTerrain(terrain);
    MonsterManager::Get()->SetAStar(aStar2);

    //#pragma region �׸���
    //    shadow = new Shadow();
    //
    //    // ���� �˾ƺ��� Ȱ��� : �� ȣ�� Ȥ�� ����� (<-�� ��� ���)
    //
    //    light = Environment::Get()->GetLight(0); // �⺻���� ������ �� ��������
    //    //light = Environment::Get()->AddLight(); //���ο� ���� ���� ��ȯ�ޱ�
    //
    //    light->type = 1;               //���� ���� (�� ������ ���Ŀ�)
    //    light->pos = { 0, 50, -500 }; //���� ��ġ
    //    light->range = 3000;           //���� ���� (���� ������ ��� ����)
    //
    //    light->direction = { 0, -1, 1 }; //���� ���� (���簪 : �տ���, �񽺵���, �Ʒ���)
    //    light->color = { 1, 1, 1, 1 };   //������ �� (���簪 : ���)
    //
    //    light->length;  //���� ����      (���� �������� ����....�� �� ����)
    //
    //    // ���� ��� ��/���� �ʴ� ���� ����ų ���, ���� �������� �Ʒ� ������ ���� ����ϸ� �ȴ�
    //    light->inner;   //���� ���� ���� (���� ���ߵǾ� ���̴� ����...�� ����)
    //    light->outer;   //���� �ܰ� ���� (���� ������� ��ġ�� ����...�� ����)
    //#pragma endregion


    FOR(4) {
        cube[i] = new Cube;
        cube[i]->SetTag(to_string(i));
        ColliderManager::Get()->PushCollision(ColliderManager::WALL, cube[i]->GetCollider());
        aStar2->AddObstacle(cube[i]->GetCollider()); 
    
    }
    cube[0]->Pos() = { 90.f,10.f,72.f };
    cube[0]->Scale() = { 20.f,20.f,1.f };
    cube[1]->Pos() = { 100.f,10.f,62.f };
    cube[1]->Scale() = { 1.f,20.f,20.f };
    cube[2]->Scale() = { 1.f,20.f,20.f };
    cube[2]->Pos() = { 80.f,10.f,62.f };
    cube[3]->Pos() = { 90.f,10.f,52.f };
    cube[3]->Scale() = { 20.f,20.f,1.f };
    FOR(2) blendState[i] = new BlendState();
    blendState[1]->AlphaToCoverage(true); //����� ���� + ���� ó���� ������ ���� ����

}

TestNpcScene::~TestNpcScene()
{
    delete terrain;
     delete aStar2;
  
    MonsterManager::Delete();
    delete naruto;
    delete boss;
    FOR(4)
        delete cube[i];
    //delete shadow;
    FOR(2) delete blendState[i];

}

void TestNpcScene::Update()
{
    aStar2->Update();
 
    naruto->Update();
    boss->Update();
 //   MonsterManager::Get()->Update();
    FOR(4)
        cube[i]->Update();
    //if (KEY_DOWN('1')) light->type = 0;
}

void TestNpcScene::PreRender()
{
    //�׸����� ���� Ÿ�� ����(�� �غ�)
    /*shadow->SetRenderTarget();

    fox->SetShader(L"Light/DepthMap.hlsl");
    fox->Render();

    instancing->SetShader(L"Light/DepthMap.hlsl");
    instancing->Render();*/
}

void TestNpcScene::Render()
{
    blendState[1]->SetState();
    terrain->Render();
    boss->Render();
    //aStar->Render();
   // aStar2->Render();
    naruto->Render();
    
  //  MonsterManager::Get()->Render();
    FOR(4)
        cube[i]->Render();

    blendState[0]->SetState();
    // �׸��� ����
    //shadow->SetRender();

    ////�׸��ڸ� �ޱ� ���� ���̴� ����
    //fox->SetShader(L"Light/Shadow.hlsl");
    //instancing->SetShader(L"Light/Shadow.hlsl");

    ////���̴��� ���õ� ���� �ΰ��� ��¥ ȣ��
    //fox->Render();
    //instancing->Render();
}

void TestNpcScene::PostRender()
{
    boss->PostRender();
}

void TestNpcScene::GUIRender()
{
    //terrain->GUIRender();

    boss->GUIRender();
}