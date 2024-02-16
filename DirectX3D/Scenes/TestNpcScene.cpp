#include "Framework.h"
#include "TestNpcScene.h"

TestNpcScene::TestNpcScene()
{
    terrain = new TerrainEditor();
    aStar = new AStar();
    aStar->SetNode(terrain); //������ ���߾ ��� ��ġ

    fox = new Fox();
    fox->SetTerrain(terrain);
    fox->SetAStar(aStar);


    instancing = new ModelAnimatorInstancing("character1");

    instancing->ReadClip("Orc_Idle");
    instancing->ReadClip("Orc_Walk");
    instancing->ReadClip("Orc_Run");
    instancing->ReadClip("Orc_Hit");
    //instancing->ReadClip("Orc_Attack");
    instancing->ReadClip("character1@atack4");
    instancing->ReadClip("Orc_Death");

    aStar2 = new AStar();
    aStar2->SetNode(terrain);

    Transform* rt = instancing->Add();
    orc = new Orc(rt, instancing, orcCount++);
    orc->GetTransform()->Scale() *= 0.1f;

    orc->SetTerrain(terrain);
    orc->SetAStar(aStar2);
    orc->SetTarget(fox);
    orc->SetTargetCollider(fox->GetCollider());



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

}

TestNpcScene::~TestNpcScene()
{
    delete terrain;
    delete aStar;
    delete aStar2;
    delete fox;
    delete orc;
    //delete shadow;
}

void TestNpcScene::Update()
{
    aStar2->Update();
    aStar->Update();
    fox->Update();
    orc->Update();
    instancing->Update();
    orc->GetTransform()->Scale() *= 0.1f;
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
    aStar2->Render();
    fox->Render();
    instancing->Render();
    orc->Render();


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
    orc->PostRender();
}

void TestNpcScene::GUIRender()
{
}