#include "Framework.h"
#include "TestNpcScene.h"

TestNpcScene::TestNpcScene()
{
    terrain = new TerrainEditor();
    aStar = new AStar();
    aStar->SetNode(terrain); //지형에 맞추어서 노드 설치

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



//#pragma region 그림자
//    shadow = new Shadow();
//
//    // 같이 알아보는 활용법 : 빛 호출 혹은 만들기 (<-빛 사용 방법)
//
//    light = Environment::Get()->GetLight(0); // 기본으로 설정된 빛 가져오기
//    //light = Environment::Get()->AddLight(); //새로운 빛을 만들어서 반환받기
//
//    light->type = 1;               //광원 종류 (상세 설명은 이후에)
//    light->pos = { 0, 50, -500 }; //광원 위치
//    light->range = 3000;           //조명 범위 (빛이 실제로 닿는 범위)
//
//    light->direction = { 0, -1, 1 }; //조명 방향 (현재값 : 앞에서, 비스듬히, 아래로)
//    light->color = { 1, 1, 1, 1 };   //조명의 색 (현재값 : 흰색)
//
//    light->length;  //광선 길이      (빛이 강해지는 범위....잘 안 쓴다)
//
//    // 빛이 닿는 곳/닿지 않는 곳을 대비시킬 경우, 초점 광원에서 아래 변수를 같이 고려하면 된다
//    light->inner;   //조명 집중 범위 (빛이 집중되어 쏘이는 범위...의 비중)
//    light->outer;   //조명 외곽 범위 (빛이 흩어져서 비치는 범위...의 비중)
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
    //그림자의 렌더 타겟 지정(및 준비)
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


    // 그림자 관련
    //shadow->SetRender();

    ////그림자를 받기 위한 셰이더 세팅
    //fox->SetShader(L"Light/Shadow.hlsl");
    //instancing->SetShader(L"Light/Shadow.hlsl");

    ////셰이더가 세팅된 배경과 인간을 진짜 호출
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