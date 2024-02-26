#include "Framework.h"
#include "EyesRaySearchScene.h"

EyesRaySearchScene::EyesRaySearchScene()
{
	terrain = new TerrainEditor(256, 256);
	terrain->GetMaterial()->SetDiffuseMap(L"Textures/Landscape/Sand.png");
	terrain->GetMaterial()->SetSpecularMap(L"Textures/Color/Black.png");
	terrain->GetMaterial()->SetNormalMap(L"Textures/Landscape/Sand_Normal.png");
	terrain->SetHeightMap(L"Textures/HeightMaps/HeightMapCustom.png");

	box1 = new BoxCollider();
	box1->Pos() = { 60, 0, 140 };
	box1->Scale().x *= 20;
	box1->Scale().z *= 5;
	box1->Scale().y *= 15;
	box1->SetTag("1");

	player = new Player();
	player->Scale() = { 0.03f,0.03f,0.03f };
	player->Pos() = { 60,0,90 };

	ColliderManager::Get()->SetPlayer(player);
	ColliderManager::Get()->SetObstacles(box1);

	MonsterManager::Get()->SetTarget(player); //싱글턴 생성 후, 표적 설정까지
	MonsterManager::Get()->SetTargetCollider(player->GetCollider());
	MonsterManager::Get()->SetOrcSRT(0, Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(80, 0, 80));
	MonsterManager::Get()->SetOrcSRT(1, Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(60, 0, 150));
	MonsterManager::Get()->SetTerrain(terrain);
	

	player->SetMoveSpeed(50);

	CAM->SetTarget(player);
	CAM->TargetOptionLoad("GameMapScenePlayer");
	CAM->LookAtTarget();
}

EyesRaySearchScene::~EyesRaySearchScene()
{
	delete box1;
	delete player;
}

void EyesRaySearchScene::Update()
{
	terrain->UpdateWorld();

	player->Update();
	MonsterManager::Get()->Update();
	MonsterManager::Get()->Blocking(box1);
}

void EyesRaySearchScene::PreRender()
{
}

void EyesRaySearchScene::Render()
{
	terrain->Render();
	box1->Render();
	player->Render();
	MonsterManager::Get()->Render();
}

void EyesRaySearchScene::PostRender()
{
}

void EyesRaySearchScene::GUIRender()
{
	//box1->GUIRender();
	//player->GUIRender();

	MonsterManager::Get()->GUIRender();
}
