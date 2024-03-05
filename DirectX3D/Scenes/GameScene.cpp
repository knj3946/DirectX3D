#include "Framework.h"
#include "GameScene.h"

GameScene::GameScene()
{
    background = new Model("Forest");
    background->SetShader(L"Basic/Texture.hlsl");
    background->Scale() *= 10.0f;
    background->UpdateWorld();

    naruto = new Naruto();
   
    cube = new Cube;
    cube->Scale().z *= 1000;
    cube->Scale().x *= 100;
    cube->Scale().y *= 1000;
    cube->Pos().y += 500;
    cube->Pos().x += 100;
    cube->UpdateWorld();
    cube->GetCollider()->UpdateWorld();
    naruto->PushCol(cube);
    cube2 = new Cube;
    cube2->SetTag("cube2");
    cube2->Scale().z *= 1000;
    cube2->Scale().x *= 100;
    cube2->Scale().y *= 1000;
 
    cube2->Pos().y += 500;
    cube2->Pos().x += 600;
    cube2->Pos().z += 450;
    cube2->Rot().y += XMConvertToRadians(90);
    cube2->UpdateWorld();
    cube2->GetCollider()->UpdateWorld();
    naruto->PushCol(cube2);
    //// �κ��� ����(��) ����
    //robotInstancing = new ModelAnimatorInstancing("Robot");
    //robotInstancing->ReadClip("StandUp");
    //robotInstancing->ReadClip("Walk");
    //robotInstancing->ReadClip("Run");
    //robotInstancing->ReadClip("Hit");
    //robotInstancing->ReadClip("Dying");
    //
    ////�κ� ���� ������ ����
    //Transform* t = robotInstancing->Add(); // ���� ����ϱ� ���� Ʈ�������� �ν��Ͻ̿��� �߰�
    //robot = new Robot(t, robotInstancing, 0); //���� �κ��� ��¿� Ʈ�������� �ε��� �ֱ�
    ////->�̷ν� �κ��� ���� ������(�κ� Ŭ����)��, ����(��)��, �� ���� �̾��� Ʈ������(t)�� �� ���´�

    //robot->SetTarget(naruto);

    KunaiManager::Get(); // �ƹ��͵� �� �ϴ� �ǹ̾��� �ڵ�����,
                         // �� �ڵ尡 ����Ǹ鼭 �̱����� �����ȴٴ� ���� ����
    
   // RobotManager::Get()->SetTarget(naruto); //�̱��� ���� ��, ǥ�� ��������

    CAM->SetTarget(naruto); //�ȷο�ķ + ������� ����
    CAM->TargetOptionLoad("Naruto"); // �����信 ���� ī�޶� ��ġ ���� �ε� (������ �ε� ������ �״��)
    CAM->LookAtTarget(); // �ȷο�ķ + ���� ��� ���� + �� ���� ����� �������� ���� (�� ���� ������)

    FOR(2)
        blendState[i] = new BlendState();
    blendState[1]->AlphaToCoverage(true); // ���� Ȥ�� ������ ������ �ܺ� �ȼ��� ������ ���ΰ�


   // skyBox = new SkyBox(L"Textures/Landscape/BlueSky.dds");

   // building = new Model("building_V2");

  //  building->Scale() = { 100,100,100 };
   // building->Rot() = {XM_PIDIV2,0,0};
    
}

GameScene::~GameScene()
{
    delete naruto;
    delete background;
   // delete skyBox;
    delete cube;
    delete cube2;

    FOR(2)
        delete blendState[i];

    KunaiManager::Get()->Delete();
  //  RobotManager::Get()->Delete();
}

void GameScene::Update()
{
    naruto->Update();
    cube->Update();
    cube2->Update();
    //robotInstancing->Update();
    //robot->Update();
  //  building->UpdateWorld();

    KunaiManager::Get()->Update();
   // RobotManager::Get()->Update();
}

void GameScene::PreRender()
{
}

void GameScene::Render()
{
    //skyBox->Render();
    cube->Render();
    cube2->Render();
    blendState[1]->SetState(); // ���� �����ϱ�
    background->Render();      // ���� ����� ���¿��� ��游 ����ϱ�
    blendState[0]->SetState(); // ���� �����ϱ�

    naruto->Render();
    //robotInstancing->Render();
    //robot->Render();
    
    KunaiManager::Get()->Render();
  //  RobotManager::Get()->Render();

  //  building->Render();
}

void GameScene::PostRender()
{
    naruto->PostRender();
 //   RobotManager::Get()->PostRender();
}

void GameScene::GUIRender()
{
  //  skyBox->GUIRender();
    //building->GUIRender();
    naruto->GUIRender();
    cube->GUIRender();
    cube2->GUIRender();
}
