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
}

TestNpcScene::~TestNpcScene()
{
    delete terrain;
    delete aStar;
    delete aStar2;
    delete fox;
    delete orc;
}

void TestNpcScene::Update()
{
    aStar2->Update();
    aStar->Update();
    fox->Update();
    orc->Update();
    instancing->Update();
}

void TestNpcScene::PreRender()
{
}

void TestNpcScene::Render()
{
    terrain->Render();
    //aStar->Render();
    aStar2->Render();
    fox->Render();
    instancing->Render();
    orc->Render();
}

void TestNpcScene::PostRender()
{
}

void TestNpcScene::GUIRender()
{
}
