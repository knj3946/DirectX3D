#include "Framework.h"
#include "ShadowScene.h"

ShadowScene::ShadowScene()
{
    terrain = new Terrain();
    terrain->GetMaterial()->SetShader(L"Light/Shadow.hlsl");

    player = new Player();
    player->Scale() = { 0.03f,0.03f,0.03f };
    player->Pos() = { 50,0,50 };
    player->UpdateWorld();

    //CAM->SetTarget(player);
    //CAM->TargetOptionLoad("GameMapScenePlayer");
    //CAM->LookAtTarget();

    ColliderManager::Get()->SetPlayer(player);

    //shadow = new Shadow();

    light = Environment::Get()->GetLight(0); 

    light->type = 1;
    light->pos = { 0, 50, -50 };
    light->range = 3000;

}

ShadowScene::~ShadowScene()
{
    delete player;
    //delete shadow;
}

void ShadowScene::Update()
{
    terrain->UpdateWorld();
    player->Update();
}

void ShadowScene::PreRender()
{
    player->PreRender();
}

void ShadowScene::Render()
{
    terrain->Render(); //터레인 먼저
    player->Render(); //그후 캐릭터
}

void ShadowScene::PostRender()
{

}

void ShadowScene::GUIRender()
{

}
