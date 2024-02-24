#include "Framework.h"
#include "TestNpcScene.h"

TestNpcScene::TestNpcScene()
{
    terrain = new TerrainEditor();

    aStar2 = new AStar();
    aStar2->SetNode(terrain);

    naruto = new Naruto();
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


    FOR(2) {
        cube[i] = new Cube;
        ColliderManager::Get()->PushCollision(ColliderManager::WALL, cube[i]->GetCollider());
    }
}

TestNpcScene::~TestNpcScene()
{
    delete terrain;
     delete aStar2;
  
    MonsterManager::Delete();
    delete naruto;
    FOR(2)
        delete cube[i];
    //delete shadow;
}

void TestNpcScene::Update()
{
    aStar2->Update();
 
    naruto->Update();

    MonsterManager::Get()->Update();
    FOR(2)
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
    terrain->Render();
    //aStar->Render();
   // aStar2->Render();
    naruto->Render();
    
    MonsterManager::Get()->Render();
    FOR(2)
        cube[i]->Render();

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
  
}

void TestNpcScene::GUIRender()
{
    terrain->GUIRender();
    FOR(2)
        cube[i]->GUIRender();
}