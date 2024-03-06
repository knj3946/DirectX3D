#include "Framework.h"
#include "GameMapScene.h"

GameMapScene::GameMapScene()
{
	skyBox = new SkyBox(L"Textures/Landscape/BlueSky.dds");

	FOR(2)
		blendState[i] = new BlendState();
	blendState[1]->AlphaToCoverage(true); // 알파 혹은 지정된 배경색을 외부 픽셀과 결합할 것인가

	terrain = new TerrainEditor(256, 256);
	terrain->GetMaterial()->SetDiffuseMap(L"Textures/Landscape/Sand.png");
	terrain->GetMaterial()->SetSpecularMap(L"Textures/Color/Black.png");
	terrain->GetMaterial()->SetNormalMap(L"Textures/Landscape/Sand_Normal.png");
	terrain->SetHeightMap(L"Textures/HeightMaps/HeightMapCustom.png");
	aStar = new AStar(128, 128);
	aStar->SetNode(terrain);

	player = new Player();
	player->Scale() = { 0.03f,0.03f,0.03f };
	player->Pos() = { 60,0,90 };
	player->SetTerrain(terrain);
	player->SetMoveSpeed(50);

	CAM->SetTarget(player);
	CAM->TargetOptionLoad("GameMapScenePlayer");
	CAM->LookAtTarget();



	ColliderManager::Get()->SetPlayer(player);
	KunaiManager::Get();
}

GameMapScene::~GameMapScene()
{
	for (ColliderModel* colliderModel : colliderModels)
	{
		delete colliderModel;
	}

	delete model;
	delete terrain;
	delete aStar;
	delete skyBox;
	KunaiManager::Delete();
	FOR(2)
		delete blendState[i];
}

void GameMapScene::Update()
{
	terrain->UpdateWorld();
	aStar->Update();

	player->Update();

	
}

void GameMapScene::PreRender()
{
}

void GameMapScene::Render()
{
	skyBox->Render();
	terrain->Render();



	player->Render();
	
}

void GameMapScene::PostRender()
{

	player->PostRender();
}

void GameMapScene::GUIRender()
{
	player->GUIRender();
	/*
	for (ColliderModel* cm : colliderModels)
	{
		//cm->GUIRender();
	}
	*/


}