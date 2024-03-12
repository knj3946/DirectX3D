#include "Framework.h"
#include "GameMapScene.h"

GameMapScene::GameMapScene()
{
	MenuManager::Get(); //기본 메뉴 생성
}

GameMapScene::~GameMapScene()
{
	for (ColliderModel* colliderModel : colliderModels)
	{
		delete colliderModel;
	}
	delete bow;
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
	MenuManager::Get()->FirstLoading(bind(&GameMapScene::FirstLoading, this));

	if (!MenuManager::Get()->IsPickGameMenu())
	{
		MenuManager::Get()->GameMenuUpdate();
		return;
	}

	if (MenuManager::Get()->IsPickGameExit())
	{
		exit(0);
	}


	if (MenuManager::Get()->GetSelectGameMenu() == 1) //인게임
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

		MonsterManager::Get()->Update();
		KunaiManager::Get()->Update();
		player->Update();
		if(!bow->Active())
		bow->Update();
		boss->Update();

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
}

void GameMapScene::PreRender()
{
	if (MenuManager::Get()->GetSelectGameMenu() == 1)
	{
		shadow->SetRenderTarget();
		player->SetShader(L"Light/DepthMap.hlsl");
		player->Render();
		MonsterManager::Get()->Render(true);
	}
}

void GameMapScene::Render()
{
	if (MenuManager::Get()->GetSelectGameMenu() == 1)
	{
		//순서 중요!
		skyBox->Render();

		//그림자 렌더설정
		shadow->SetRender();
		player->SetShader(L"Light/Shadow.hlsl");

		terrain->Render();

		for (ColliderModel* cm : colliderModels)
		{
			cm->Render();
		}

		player->Render();
		MonsterManager::Get()->Render();
		KunaiManager::Get()->Render();

		boss->Render();
		if(bow->Active())
		bow->Render();

		//aStar->Render();
	}
}

void GameMapScene::PostRender()
{
	MenuManager::Get()->PostRender();

	if (MenuManager::Get()->GetSelectGameMenu() == 1)
	{
		MonsterManager::Get()->PostRender();
		ArrowManager::Get()->PostRender();
		ColliderManager::Get()->PostRender();
		player->PostRender();
		boss->PostRender();

		player->TextRender();
	}

}

void GameMapScene::GUIRender()
{

	MenuManager::Get()->GUIRender();

	if (MenuManager::Get()->GetSelectGameMenu() == 1)
	{
		//player->GUIRender();
		/*
		for (ColliderModel* cm : colliderModels)
		{
			//cm->GUIRender();
		}
		*/

		//MonsterManager::Get()->GUIRender();
		//KunaiManager::Get()->GUIRender();

		Timer::Get()->GUIRender();
		if (bow->Active())
			bow->GUIRender();
	}

}

void GameMapScene::FirstLoading()
{
	if (MenuManager::Get()->GetLoadingSequence() == 0)
	{
		skyBox = new SkyBox(L"Textures/Landscape/BlueSky.dds");

		FOR(2)
			blendState[i] = new BlendState();
		blendState[1]->AlphaToCoverage(true); // 알파 혹은 지정된 배경색을 외부 픽셀과 결합할 것인가
		MenuManager::Get()->IncreaseLoadingSequence();
		MenuManager::Get()->SetLoadingRate(10.f);

		LightBuffer::Light* light = Environment::Get()->GetLight(0); // 기본으로 설정된 빛 가져오기

		light->type = 1;               //광원 종류 (상세 설명은 이후에)
		light->pos = { 0, 50, -50 }; //광원 위치
		light->range = 3000;           //조명 범위 (빛이 실제로 닿는 범위)
	}
	else if (MenuManager::Get()->GetLoadingSequence() == 1)
	{
		terrain = new TerrainEditor(256, 256);
		terrain->GetMaterial()->SetDiffuseMap(L"Textures/Landscape/Sand.png");
		terrain->GetMaterial()->SetSpecularMap(L"Textures/Color/Black.png");
		terrain->GetMaterial()->SetNormalMap(L"Textures/Landscape/Sand_Normal.png");
		terrain->SetHeightMap(L"Textures/HeightMaps/SamepleHeightMap02.png");
		terrain->GetMaterial()->SetShader(L"Light/Shadow.hlsl");
		MenuManager::Get()->IncreaseLoadingSequence();
		MenuManager::Get()->SetLoadingRate(25.f);
	}
	else if (MenuManager::Get()->GetLoadingSequence() == 2)
	{
		{
			string mName = "building_V2";
			string mTag = "model1";
			Vector3 mScale = { 3,3,3 };
			Vector3 mRot = { 0,0,0 };
			Vector3 mPos = { 80.2,0,54.1 };

			ColliderModel* cm = ColliderManager::Get()->CreateColliderModel(mName, mTag, mScale, mRot, mPos);
			colliderModels.push_back(cm);
		}

		{
			string mName = "building_V4";
			string mTag = "model2";
			Vector3 mScale = { 3,3,3 };
			Vector3 mRot = { 0,0,0 };
			Vector3 mPos = { 138.f,0.f,48.1f };

			ColliderModel* cm = ColliderManager::Get()->CreateColliderModel(mName, mTag, mScale, mRot, mPos);
			colliderModels.push_back(cm);
		}

		{
			string mName = "building_V5";
			string mTag = "model3";
			Vector3 mScale = { 3,3,3 };
			Vector3 mRot = { 0,0,0 };
			Vector3 mPos = { 61.1f,0,98.3f };

			ColliderModel* cm = ColliderManager::Get()->CreateColliderModel(mName, mTag, mScale, mRot, mPos);
			colliderModels.push_back(cm);
		}

		{
			string mName = "building_V4";
			string mTag = "model4";
			Vector3 mScale = { 3,3,3 };
			Vector3 mRot = { 0,0,0 };
			Vector3 mPos = { 65.7f,0,153.4f };

			ColliderModel* cm = ColliderManager::Get()->CreateColliderModel(mName, mTag, mScale, mRot, mPos);
			colliderModels.push_back(cm);
		}

		{
			string mName = "building_V5";
			string mTag = "model5";
			Vector3 mScale = { 3,3,3 };
			Vector3 mRot = { 0.f,0,0 };
			Vector3 mPos = { 132.f,0,190.6f };

			ColliderModel* cm = ColliderManager::Get()->CreateColliderModel(mName, mTag, mScale, mRot, mPos);
			colliderModels.push_back(cm);
		}

		{
			string mName = "building_V2";
			string mTag = "model6";
			Vector3 mScale = { 3,3,3 };
			Vector3 mRot = { 0,0,0 };
			Vector3 mPos = { 80,0,208.2f };

			ColliderModel* cm = ColliderManager::Get()->CreateColliderModel(mName, mTag, mScale, mRot, mPos);
			colliderModels.push_back(cm);
		}

		{
			string mName = "building_V4";
			string mTag = "model7";
			Vector3 mScale = { 3,3,3 };
			Vector3 mRot = { 0,0,0 };
			Vector3 mPos = { 203.4f,9.5f,185.1f };

			ColliderModel* cm = ColliderManager::Get()->CreateColliderModel(mName, mTag, mScale, mRot, mPos);
			colliderModels.push_back(cm);
		}

		{
			string mName = "building_V5";
			string mTag = "model8";
			Vector3 mScale = { 3,3,3 };
			Vector3 mRot = { 0.f,0,0 };
			Vector3 mPos = { 164.9f,0,122.3f };

			ColliderModel* cm = ColliderManager::Get()->CreateColliderModel(mName, mTag, mScale, mRot, mPos);
			colliderModels.push_back(cm);
		}

		{
			string mName = "building_V2";
			string mTag = "model9";
			Vector3 mScale = { 6,6,6 };
			Vector3 mRot = { 0.f,0, XMConvertToRadians(-90) };
			Vector3 mPos = { 202.4f,0,96.3f };

			ColliderModel* cm = ColliderManager::Get()->CreateColliderModel(mName, mTag, mScale, mRot, mPos);
			colliderModels.push_back(cm);
		}

		BoxCollider* bc1 = new BoxCollider;
		BoxCollider* bc2 = new BoxCollider;
		bc1->Pos() = { 164.4f,7.5f,168.4f };
		bc2->Pos() = { 164.4f,7.5f,214.8f };
		bc1->Scale() = { 5.f,15.f,27.f };
		bc2->Scale() = { 5.f,15.f,27.f };
		bc1->Role() = Collider::Collider_Role::BLOCK;
		bc2->Role() = Collider::Collider_Role::BLOCK;
		bc1->Special() = Collider::Collider_Special::NONE;
		bc2->Special() = Collider::Collider_Special::NONE;

		ColliderManager::Get()->SetObstacles(bc1);
		ColliderManager::Get()->SetObstacles(bc2);
		aStar = new AStar(128, 128);
		aStar->SetNode(terrain);

		MenuManager::Get()->IncreaseLoadingSequence();
		MenuManager::Get()->SetLoadingRate(55.f);
	}
	else if (MenuManager::Get()->GetLoadingSequence() == 3)
	{
		Audio::Get()->Add("bgm1", "Sounds/dramatic-choir.wav", true, true, false);

		player = new Player();
		player->Scale() = { 0.03f,0.03f,0.03f };
		player->Pos() = { 60,0,90 };
		player->SetTerrain(terrain);
		//player->SetMoveSpeed(50);
		bow = new Bow;
		bow->SetTarget(player);
		player->SetBow(bow->GetModel());
		ArrowManager::Get()->SetBowTransform(bow->GetModel());
		CAM->SetTarget(player);
		CAM->TargetOptionLoad("GameMapScenePlayer");
		CAM->LookAtTarget();

		MenuManager::Get()->IncreaseLoadingSequence();
		MenuManager::Get()->SetLoadingRate(75.f);

		shadow = new Shadow();
	}
	else if (MenuManager::Get()->GetLoadingSequence() == 4)
	{
		InteractManager::Get()->SetPlayer(player);
		MonsterManager::Get()->SetOrcSRT(0, Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(150.f, 0.f, 175.f));
		MonsterManager::Get()->SetPatrolPos(0, Vector3(150.f, 0.f, 210.f));
		MonsterManager::Get()->SetOrcSRT(1, Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(100, 0, 185));
		MonsterManager::Get()->SetPatrolPos(1, Vector3(50.f, 0.f, 185.f));
		MonsterManager::Get()->SetOrcSRT(2, Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(85, 0, 120));
		MonsterManager::Get()->SetPatrolPos(2, Vector3(40.f, 0.f, 120.f));   MonsterManager::Get()->SetPatrolPos(2, Vector3(40.f, 0.f, 70.f));   MonsterManager::Get()->SetPatrolPos(2, Vector3(85.f, 0.f, 70.f));
		MonsterManager::Get()->SetOrcSRT(3, Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(62, 0, 40));
		MonsterManager::Get()->SetPatrolPos(3, Vector3(62.f, 0.f, 65.f));
		MonsterManager::Get()->SetOrcSRT(4, Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(110, 0, 80));
		MonsterManager::Get()->SetPatrolPos(4, Vector3(110.f, 0.f, 40.f));
		MonsterManager::Get()->SetOrcSRT(5, Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(170, 0, 40));
		MonsterManager::Get()->SetPatrolPos(5, Vector3(170.f, 0.f, 80.f)); MonsterManager::Get()->SetPatrolPos(5, Vector3(120.f, 0.f, 80.f));   MonsterManager::Get()->SetPatrolPos(5, Vector3(170.f, 0.f, 80.f));
		MonsterManager::Get()->SetOrcSRT(6, Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(160, 0, 120));
		MonsterManager::Get()->SetPatrolPos(6, Vector3(170.f, 0.f, 120.f));
		MonsterManager::Get()->SetOrcSRT(7, Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(50, 0, 135));
		MonsterManager::Get()->SetPatrolPos(7, Vector3(100.f, 0.f, 135.f));
		MonsterManager::Get()->SetType(7,1);// 1이 알리는애
		MonsterManager::Get()->SetTerrain(terrain);
		MonsterManager::Get()->SetAStar(aStar);
		MonsterManager::Get()->SetTarget(player);
		MonsterManager::Get()->SetTargetCollider(player->GetCollider()); 
		MonsterManager::Get()->SetTargetStateInfo(player->GetStateInfo());

		boss = new Boss;
		boss->SetTarget(player);
		boss->SetTerrain(terrain);
		boss->SetAStar(aStar);
		boss->GetCollider()->UpdateWorld();
		player->SetBoss(boss);
		boss->SetPatrolPos(Vector3(205.f, 0.f, 110.f));
		boss->SetPatrolPos(Vector3(205.f, 0.f, 80.f));
		ColliderManager::Get()->PushCollision(ColliderManager::BOSS, boss->GetCollider());
		ColliderManager::Get()->SetBoss(boss);
		MenuManager::Get()->IncreaseLoadingSequence();
		MenuManager::Get()->SetLoadingRate(95.f);
		MonsterManager::Get()->SetBoss(boss);
	}
	else if (MenuManager::Get()->GetLoadingSequence() == 5)
	{
		for (ColliderModel* colliderModel : colliderModels)
		{
			for (Collider* collider : colliderModel->GetColliders())
			{
				if (collider->Role() == Collider::Collider_Role::BLOCK)
					MonsterManager::Get()->AddOrcObstacleObj(collider);
			}
		}


		ColliderManager::Get()->SetPlayer(player);
		KunaiManager::Get();
		MenuManager::Get()->IncreaseLoadingSequence();
		MenuManager::Get()->SetLoadingRate(99.f);

		MonsterManager::Get()->SetOrcGround();

		//리소스 불러오기 용으로 한번 호출
		skyBox->Render();
		terrain->Render();

		for (ColliderModel* cm : colliderModels)
		{
			cm->Render();
		}

		player->PreRender();
		player->Render();
		MonsterManager::Get()->Render();
		KunaiManager::Get()->Render();
	
		if (!bow->Active())
			bow->Render();
	}

	if (MenuManager::Get()->GetLoadingSequence() == 6)
	{
		MenuManager::Get()->SetLoadingRate(100.f);
	}
}