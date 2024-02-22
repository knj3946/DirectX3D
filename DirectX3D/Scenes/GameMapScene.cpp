#include "Framework.h"
#include "GameMapScene.h"

GameMapScene::GameMapScene()
{
	skyBox = new SkyBox(L"Textures/Landscape/BlueSky.dds");

	FOR(2)
		blendState[i] = new BlendState();
	blendState[1]->AlphaToCoverage(true); // 알파 혹은 지정된 배경색을 외부 픽셀과 결합할 것인가

	terrain = new TerrainEditor(256,256);
	terrain->GetMaterial()->SetDiffuseMap(L"Textures/Landscape/Sand.png");
	terrain->GetMaterial()->SetSpecularMap(L"Textures/Color/Black.png");
	terrain->GetMaterial()->SetNormalMap(L"Textures/Landscape/Sand_Normal.png");
	terrain->SetHeightMap(L"Textures/HeightMaps/HeightMapCustom.png");

	{
		string mName = "building_V2";
		string mTag = "model1";
		Vector3 mScale = { 3,3,3 };
		Vector3 mRot = { 0,0,0 };
		Vector3 mPos = { 80,0,80 };

		CreateColliderModel(mName, mTag, mScale, mRot, mPos);
	}

	{
		string mName = "building_V4";
		string mTag = "model2";
		Vector3 mScale = { 3,3,3 };
		Vector3 mRot = { 0,0,0 };
		Vector3 mPos = { 160,0,80 };

		CreateColliderModel(mName, mTag, mScale, mRot, mPos);
	}

	{
		string mName = "building_V5";
		string mTag = "model3";
		Vector3 mScale = { 3,3,3 };
		Vector3 mRot = { 0,0,0 };
		Vector3 mPos = { 160,0,160 };

		CreateColliderModel(mName, mTag, mScale, mRot, mPos);
	}

	{
		string mName = "building_V4";
		string mTag = "model4";
		Vector3 mScale = { 3,3,3 };
		Vector3 mRot = { 0,0,0 };
		Vector3 mPos = { 80,0,150 };

		CreateColliderModel(mName, mTag, mScale, mRot, mPos);
	}

	{
		string mName = "building_V5";
		string mTag = "model5";
		Vector3 mScale = { 3,3,3 };
		Vector3 mRot = { 0,0,0 };
		Vector3 mPos = { 128,0,128 };

		CreateColliderModel(mName, mTag, mScale, mRot, mPos);
	}

	{
		string mName = "building_V2";
		string mTag = "model6";
		Vector3 mScale = { 3,3,3 };
		Vector3 mRot = { 0,0,0 };
		Vector3 mPos = { 80,0,215 };

		CreateColliderModel(mName, mTag, mScale, mRot, mPos);
	}

	{
		string mName = "building_V4";
		string mTag = "model7";
		Vector3 mScale = { 3,3,3 };
		Vector3 mRot = { 0,0,0 };
		Vector3 mPos = { 210,0,140 };

		CreateColliderModel(mName, mTag, mScale, mRot, mPos);
	}

	{
		string mName = "building_V5";
		string mTag = "model8";
		Vector3 mScale = { 3,3,3 };
		Vector3 mRot = { 0,0,0 };
		Vector3 mPos = { 200,0,80 };

		CreateColliderModel(mName, mTag, mScale, mRot, mPos);
	}

	{
		string mName = "building_V2";
		string mTag = "model9";
		Vector3 mScale = { 6,6,6 };
		Vector3 mRot = { 0,0,0 };
		Vector3 mPos = { 190,0,190 };

		CreateColliderModel(mName, mTag, mScale, mRot, mPos);
	}

	Audio::Get()->Add("bgm1", "Sounds/dramatic-choir.wav", true, true, false);



	player = new Player();
	player->Scale() = { 0.03f,0.03f,0.03f };
	player->Pos() = { 60,0,90 };
	MonsterManager::Get()->SetTarget(player); //싱글턴 생성 후, 표적 설정까지

	MonsterManager::Get()->SetOrcSRT(0, Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(80, 0, 80));
	MonsterManager::Get()->SetOrcSRT(1, Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(60, 0, 150));
	MonsterManager::Get()->SetTerrain(terrain);

	for (ColliderModel* colliderModel : colliderModels)
	{
		for (Collider* collider : colliderModel->GetColliders())
		{
			MonsterManager::Get()->AddOrcObstacleObj(collider);
		}
	}

	player->SetMoveSpeed(50);

	CAM->SetTarget(player);
	//CAM->TargetOptionLoad("Naruto2");
	CAM->LookAtTarget();
}

GameMapScene::~GameMapScene()
{
	for (ColliderModel* colliderModel : colliderModels)
	{
		delete colliderModel;
	}
	
	delete model;
	delete terrain;
	delete skyBox;
	
	FOR(2)
		delete blendState[i];
}

void GameMapScene::Update()
{
	terrain->UpdateWorld();

	for (ColliderModel* cm : colliderModels)
	{
		cm->UpdateWorld();
	}
	Audio::Get()->Update();

	if (KEY_UP('1'))
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
			//player->Blocking(collider); //플레이어 벽 뚫고 이동 못하게
			MonsterManager::Get()->Blocking(collider);
		}
	}

	MonsterManager::Get()->Fight(player);
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

	static_cast<Naruto*>(player)->Render();
	MonsterManager::Get()->Render();
}

void GameMapScene::PostRender()
{
	MonsterManager::Get()->PostRender();
	static_cast<Naruto*>(player)->PostRender();
}

void GameMapScene::GUIRender()
{
	//static_cast<Naruto*>(player)->GUIRender();

	for (ColliderModel* cm : colliderModels)
	{
		//cm->GUIRender();
	}

	//CAM->GUIRender();

	MonsterManager::Get()->GUIRender();
}

void GameMapScene::CreateColliderModel(string mName, string mTag, Vector3 mScale, Vector3 mRot, Vector3 mPos)
{
	int colCount = 0;
	vector<Vector3> colPoss;
	vector<Vector3> colScales;
	vector<Vector3> colRots;

	if (mName == "building_V2")
	{
		mRot.x += XM_PIDIV2;
		colCount = 7;
		colPoss.push_back(Vector3(4.f, -1.6f, -1.5f));
		colPoss.push_back(Vector3(4.f, 1.6f, -1.5f));
		colPoss.push_back(Vector3(0.f, 0.f, -2.7f));
		colPoss.push_back(Vector3(0.3f, 2.5f, -1.5f));
		colPoss.push_back(Vector3(-3.9f, 0.f, -1.5f));
		colPoss.push_back(Vector3(-3.6f, -2.4f, -1.5f));
		colPoss.push_back(Vector3(1.3f, -2.4f, -1.5f));

		colRots.push_back(Vector3(0.f, 0.f, 0.f));
		colRots.push_back(Vector3(0.f, 0.f, 0.f));
		colRots.push_back(Vector3(0.f, 0.f, 0.f));
		colRots.push_back(Vector3(0.f, 0.f, 0.f));
		colRots.push_back(Vector3(0.f, 0.f, 0.f));
		colRots.push_back(Vector3(0.f, 0.f, 0.f));
		colRots.push_back(Vector3(0.f, 0.f, 0.f));

		colScales.push_back(Vector3(0.4f, 1.9f, 3.f));
		colScales.push_back(Vector3(0.4f, 1.9f, 3.f));
		colScales.push_back(Vector3(8.5f, 5.1f, 0.9f));
		colScales.push_back(Vector3(7.7f, 0.2f, 3.f));
		colScales.push_back(Vector3(0.6f, 5.1f, 3.2f));
		colScales.push_back(Vector3(1.1f, 0.4f, 3.f));
		colScales.push_back(Vector3(5.8f, 0.4f, 3.f));

	}
	else if (mName == "building_V4")
	{
		mRot.x += XM_PIDIV2;
		colCount = 10;
		colPoss.push_back(Vector3(4.f, -1.6f, -1.5f));
		colPoss.push_back(Vector3(4.f, 1.6f, -1.5f));
		colPoss.push_back(Vector3(0.f, 0.f, -2.7f));
		colPoss.push_back(Vector3(2.1f, 2.5f, -1.5f));
		colPoss.push_back(Vector3(-3.9f, 2.4f, -1.5f));
		colPoss.push_back(Vector3(0.f, -2.4f, -1.5f));
		colPoss.push_back(Vector3(-0.8f, 7.1f, -1.5f));
		colPoss.push_back(Vector3(-3.6f, 7.1f, -1.5f));
		colPoss.push_back(Vector3(-0.2f, 4.9f, -1.5f));
		colPoss.push_back(Vector3(-2.1f, 4.9f, -2.9f));

		colRots.push_back(Vector3(0.f, 0.f, 0.f));
		colRots.push_back(Vector3(0.f, 0.f, 0.f));
		colRots.push_back(Vector3(0.f, 0.f, 0.f));
		colRots.push_back(Vector3(0.f, 0.f, 0.f));
		colRots.push_back(Vector3(0.f, 0.f, 0.f));
		colRots.push_back(Vector3(0.f, 0.f, 0.f));
		colRots.push_back(Vector3(0.f, 0.f, 0.f));
		colRots.push_back(Vector3(0.f, 0.f, 0.f));
		colRots.push_back(Vector3(0.f, 0.f, 0.f));
		colRots.push_back(Vector3(0.f, 0.f, 0.f));

		colScales.push_back(Vector3(0.4f, 1.9f, 3.f));
		colScales.push_back(Vector3(0.4f, 1.9f, 3.f));
		colScales.push_back(Vector3(8.5f, 5.1f, 0.9f));
		colScales.push_back(Vector3(4.2f, 0.2f, 3.f));
		colScales.push_back(Vector3(0.6f, 9.8f, 3.2f));
		colScales.push_back(Vector3(8.4f, 0.4f, 3.f));
		colScales.push_back(Vector3(1.5f, 0.4f, 3.f));
		colScales.push_back(Vector3(1.3f, 0.4f, 3.f));
		colScales.push_back(Vector3(0.3f, 4.6f, 3.f));
		colScales.push_back(Vector3(4.1f, 4.9f, 0.4));

	}
	else if (mName == "building_V5")
	{
		mRot.x += XMConvertToRadians(165);
		colCount = 8;
		colPoss.push_back(Vector3(4.f, -1.1f, -2.0f));
		colPoss.push_back(Vector3(4.f, -2.0f, 1.2f));
		colPoss.push_back(Vector3(0.f, -2.8f, -0.7f));
		colPoss.push_back(Vector3(0.f, -0.9f, -2.7f));
		colPoss.push_back(Vector3(-3.9f, -1.5f, -0.4f));
		colPoss.push_back(Vector3(-3.6f, -2.1f, 1.9f));
		colPoss.push_back(Vector3(1.3f, -2.1f, 1.9f));
		colPoss.push_back(Vector3(0.f, -3.5f, 2.8f));

		colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
		colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
		colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
		colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
		colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
		colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
		colRots.push_back(Vector3(XMConvertToRadians(105), 0.f, 0.f));
		colRots.push_back(Vector3(XMConvertToRadians(180), 0.f, 0.f));

		colScales.push_back(Vector3(0.4f, 1.9f, 3.f));
		colScales.push_back(Vector3(0.4f, 1.9f, 3.f));
		colScales.push_back(Vector3(8.5f, 5.1f, 0.4f));
		colScales.push_back(Vector3(8.3f, 0.4f, 3.1f));
		colScales.push_back(Vector3(0.6f, 5.1f, 3.2f));
		colScales.push_back(Vector3(1.2f, 0.4f, 3.2f));
		colScales.push_back(Vector3(5.8f, 0.4f, 3.2f));
		colScales.push_back(Vector3(8.0f, 0.1f, 2.4f));
	}

	ColliderModel* model = new ColliderModel(mName);

	model->SetTag(mTag);
	model->Scale() = mScale;
	model->Pos() = mPos;
	model->Rot() = mRot;
	model->UpdateWorld();

	FOR(colCount)
	{
		Collider* col = new BoxCollider();
		string ctag = mTag + "_collider" + to_string(i);
		col->SetTag(ctag);
		col->SetParent(model);
		col->Scale() = colScales[i];
		col->Rot() = colRots[i];
		col->Pos() = colPoss[i];
		col->UpdateWorld();
		model->AddCollider(col);
	}

	colliderModels.push_back(model);
}
