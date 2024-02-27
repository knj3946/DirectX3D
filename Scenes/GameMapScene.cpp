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
	{
		string mName = "building_V2";
		string mTag = "model1";
		Vector3 mScale = { 3,3,3 };
		Vector3 mRot = { 0,0,0 };
		Vector3 mPos = { 80,0,80 };

		ColliderModel* cm = ColliderManager::Get()->CreateColliderModel(mName, mTag, mScale, mRot, mPos);
		colliderModels.push_back(cm);
	}

	{
		string mName = "building_V4";
		string mTag = "model2";
		Vector3 mScale = { 3,3,3 };
		Vector3 mRot = { 0,0,0 };
		Vector3 mPos = { 160,0,80 };

		ColliderModel* cm = ColliderManager::Get()->CreateColliderModel(mName, mTag, mScale, mRot, mPos);
		colliderModels.push_back(cm);
	}

	{
		string mName = "building_V5";
		string mTag = "model3";
		Vector3 mScale = { 3,3,3 };
		Vector3 mRot = { 0,0,0 };
		Vector3 mPos = { 160,0,160 };

		ColliderModel* cm = ColliderManager::Get()->CreateColliderModel(mName, mTag, mScale, mRot, mPos);
		colliderModels.push_back(cm);
	}

	{
		string mName = "building_V4";
		string mTag = "model4";
		Vector3 mScale = { 3,3,3 };
		Vector3 mRot = { 0,0,0 };
		Vector3 mPos = { 80,0,150 };

		ColliderModel* cm = ColliderManager::Get()->CreateColliderModel(mName, mTag, mScale, mRot, mPos);
		colliderModels.push_back(cm);
	}

	{
		string mName = "building_V5";
		string mTag = "model5";
		Vector3 mScale = { 3,3,3 };
		Vector3 mRot = { 0,0,0 };
		Vector3 mPos = { 128,0,128 };

		ColliderModel* cm = ColliderManager::Get()->CreateColliderModel(mName, mTag, mScale, mRot, mPos);
		colliderModels.push_back(cm);
	}

	{
		string mName = "building_V2";
		string mTag = "model6";
		Vector3 mScale = { 3,3,3 };
		Vector3 mRot = { 0,0,0 };
		Vector3 mPos = { 80,0,215 };

		ColliderModel* cm = ColliderManager::Get()->CreateColliderModel(mName, mTag, mScale, mRot, mPos);
		colliderModels.push_back(cm);
	}

	{
		string mName = "building_V4";
		string mTag = "model7";
		Vector3 mScale = { 3,3,3 };
		Vector3 mRot = { 0,0,0 };
		Vector3 mPos = { 210,0,140 };

		ColliderModel* cm = ColliderManager::Get()->CreateColliderModel(mName, mTag, mScale, mRot, mPos);
		colliderModels.push_back(cm);
	}

	{
		string mName = "building_V5";
		string mTag = "model8";
		Vector3 mScale = { 3,3,3 };
		Vector3 mRot = { 0,0,0 };
		Vector3 mPos = { 200,0,80 };

		ColliderModel* cm = ColliderManager::Get()->CreateColliderModel(mName, mTag, mScale, mRot, mPos);
		colliderModels.push_back(cm);
	}

	{
		string mName = "building_V2";
		string mTag = "model9";
		Vector3 mScale = { 6,6,6 };
		Vector3 mRot = { 0,0,0 };
		Vector3 mPos = { 190,0,190 };

		ColliderModel* cm = ColliderManager::Get()->CreateColliderModel(mName, mTag, mScale, mRot, mPos);
		colliderModels.push_back(cm);
	}

	Audio::Get()->Add("bgm1", "Sounds/dramatic-choir.wav", true, true, false);

	player = new Player();
	player->Scale() = { 0.03f,0.03f,0.03f };
	player->Pos() = { 60,0,90 };
	player->SetTerrain(terrain);

	MonsterManager::Get()->SetTarget(player); //싱글턴 생성 후, 표적 설정까지
	MonsterManager::Get()->SetTargetCollider(player->GetCollider());
	MonsterManager::Get()->SetOrcSRT(0, Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(80, 0, 80));
	MonsterManager::Get()->SetOrcSRT(1, Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(60, 0, 150));
	MonsterManager::Get()->SetTerrain(terrain);
	MonsterManager::Get()->SetAStar(aStar);

	for (ColliderModel* colliderModel : colliderModels)
	{
		for (Collider* collider : colliderModel->GetColliders())
		{
			MonsterManager::Get()->AddOrcObstacleObj(collider);
		}
	}

	player->SetMoveSpeed(50);

	CAM->SetTarget(player);
	CAM->TargetOptionLoad("GameMapScenePlayer");
	CAM->LookAtTarget();

	ColliderManager::Get()->SetPlayer(player);
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

	FOR(2)
		delete blendState[i];
}

void GameMapScene::Update()
{
	terrain->UpdateWorld();
	aStar->Update();
	for (ColliderModel* cm : colliderModels)
	{
		cm->UpdateWorld();
	}
	Audio::Get()->Update();

	if (KEY_UP('7')) // 123456 은 플레이어 조작에 써야되서 바꿈
	{
		if (Audio::Get()->IsPlaySound("bgm1")) // 1번사운드가 플레이 중이라면
		{
			Audio::Get()->Stop("bgm1"); // 플레이 중지
		}
		else
		{
			Audio::Get()->Play("bgm1", 2.0f); // 1번사운드 플레이
		}
	}

	if (KEY_UP('P'))
	{
		if (Audio::Get()->IsPlaySound("bgm1"))
		{
			Audio::Get()->Pause("bgm1"); // 일시정지
		}
	}

	if (KEY_UP('R'))
	{
		if (Audio::Get()->IsPlaySound("bgm1"))
		{
			Audio::Get()->Resume("bgm1"); // 일시정지된 지점부터 다시 재생
		}
	}

	player->Update();
	MonsterManager::Get()->Update();


	for (ColliderModel* colliderModel : colliderModels)
	{
		for (Collider* collider : colliderModel->GetColliders())
		{
			MonsterManager::Get()->Blocking(collider);
		}
	}

	if (waitSettingTime >= 1)
	{
		MonsterManager::Get()->Fight(player);
	}
	else
		waitSettingTime += DELTA;

	
}

void GameMapScene::PreRender()
{
}

void GameMapScene::Render()
{
	skyBox->Render();
	terrain->Render();

	for (ColliderModel* cm : colliderModels)
	{
		cm->Render();
	}

	player->Render();
	MonsterManager::Get()->Render();
}

void GameMapScene::PostRender()
{
	MonsterManager::Get()->PostRender();
	player->PostRender();
}

void GameMapScene::GUIRender()
{
	//player->GUIRender();
	/*
	for (ColliderModel* cm : colliderModels)
	{
		//cm->GUIRender();
	}
	*/

	MonsterManager::Get()->GUIRender();
}