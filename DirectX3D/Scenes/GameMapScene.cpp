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
		colliderModel = nullptr;
	}
	MenuManager::Delete();
	ColliderManager::Delete();
	InteractManager::Delete();
	delete bow;
	delete model;
	delete terrain;
	delete aStar;
	delete skyBox;
	delete shadow;
	delete player;
	delete boss;
	MonsterManager::Delete();

	KunaiManager::Delete();
	FOR(2)
		delete blendState[i];

	delete endingCredit;
	delete videoPlayer;
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
		if (MenuManager::Get()->GetSelectFailMenu() == 1)
		{
			//처음부터 시작하도록 로직작성

			player->Respawn(Vector3(60, 0, 90));
			MonsterManager::Get()->Respawn();


			FOR(MonsterManager::Get()->GetSIZE())
			{
				MonsterManager::Get()->SetOrcSRT(i, srt[i][0], srt[i][1], srt[i][2]);
				MonsterManager::Get()->SetPatrolPos(i, patrolPos[i]);
			}
			MonsterManager::Get()->GetOrc(0)->SetSpeed(8);
			MonsterManager::Get()->GetOrc(1)->SetSpeed(6);

			MenuManager::Get()->SetFailFlag(false);
			MenuManager::Get()->SetSelectFailMenu(0);
		}
		if (MenuManager::Get()->GetSelectFailMenu() == 2)
		{
			exit(0);
		}

		if (MenuManager::Get()->GetFailFlag() && MenuManager::Get()->GetSelectFailMenu() == 0)
		{
			MenuManager::Get()->FailMenuUpdate();
			return;
		}

		if (MenuManager::Get()->GetEndFlag())
		{
			if (videoPlayer->GetPlayTime() == 0.f)
			{
				SoundManager::Get()->GetPlayerAudio()->AllStop();
				SoundManager::Get()->GetBossAudio()->AllStop();
				FOR(MonsterManager::Get()->GetSIZE())
				{
					SoundManager::Get()->GetOrcAudio(i)->AllStop();
				}
				SoundManager::Get()->GetPlayerAudio()->Play("bgm4", 0.1f * VOLUME);
			}

			endingCredit->Pos().y += DELTA * 30;
			endingCredit->UpdateWorld();

			videoPlayer->Update();

			if (videoPlayer->GetPlayTime() >= 115.f)
			{
				GameManager::ReserveRestart();
				SoundManager::Get()->GetPlayerAudio()->AllStop();
			}
			return;
		}


		terrain->UpdateWorld();
		aStar->Update();
		for (ColliderModel* cm : colliderModels)
		{
			cm->UpdateWorld();
		}
		SoundManager::Get()->Update();

		if (KEY_UP('7')) // 123456 은 플레이어 조작에 써야되서 바꿈
		{
			if (PLAYERSOUND()->IsPlaySound("bgm1")) // 1번사운드가 플레이 중이라면
			{
				PLAYERSOUND()->Stop("bgm1"); // 플레이 중지
			}
			else
			{
				PLAYERSOUND()->Play("bgm1", 2.0f); // 1번사운드 플레이
			}
		}

		if (KEY_UP('P'))
		{
			if (PLAYERSOUND()->IsPlaySound("bgm1"))
			{
				PLAYERSOUND()->Pause("bgm1"); // 일시정지
			}
		}

		if (KEY_UP('R'))
		{
			if (PLAYERSOUND()->IsPlaySound("bgm1"))
			{
				PLAYERSOUND()->Resume("bgm1"); // 일시정지된 지점부터 다시 재생
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
		FOR(6) {
			MonsterManager::Get()->Blocking(HeightCollider[i]);

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
		//player->Render(); // 그림자 비활성화
		//MonsterManager::Get()->Render(true); // 그림자 비활성화
	}

	if (MenuManager::Get()->GetEndFlag())
	{
		videoPlayer->PreRender();
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
		//bc1->Render();
		//bc2->Render();

		/*
		FOR(6)
			HeightCollider[i]->Render();
		*/
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
	if (MenuManager::Get()->GetEndFlag())
	{
		endingCredit->Render();
		videoPlayer->PostRender();
	}
	else
	{
		MenuManager::Get()->PostRender();

		if (MenuManager::Get()->GetSelectGameMenu() == 1)
		{
			if (!MenuManager::Get()->GetFailFlag())
			{
				MonsterManager::Get()->PostRender();
				ArrowManager::Get()->PostRender();
				ColliderManager::Get()->PostRender();
				player->PostRender();
				boss->PostRender();
				player->TextRender();
			}
		}
	}
}

void GameMapScene::GUIRender()
{
	//MenuManager::Get()->GUIRender();
	

	if (MenuManager::Get()->GetSelectGameMenu() == 1)
	{
		//ImGui::Text("volume : %f", VOLUME);
		//ColliderManager::Get()->GuiRender();
		//player->GUIRender();
		/*
		for (ColliderModel* cm : colliderModels)
		{
			//cm->GUIRender();
		}
		*/

		MonsterManager::Get()->GUIRender();
		//KunaiManager::Get()->GUIRender();

		//Timer::Get()->GUIRender();

	}
}

void GameMapScene::FirstLoading()
{
	if (MenuManager::Get()->GetLoadingSequence() == 0)
	{
		ShowCursor(false);

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
		terrain->SetHeightMap(L"Textures/HeightMaps/SamepleHeightMap04.png");
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
			Vector3 mPos = { 205.4f,12.5f,185.1f };

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

		bc1 = new BoxCollider;
		bc2 = new BoxCollider;
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
		FOR(6)
			HeightCollider[i] = new BoxCollider;

		HeightCollider[0]->Pos() = {128.f,25.f,18.f};
		HeightCollider[0]->Scale() = {256.f,50.f,30.f};
		HeightCollider[1]->Pos() = { 128.f,25.f, 246.5f };
		HeightCollider[1]->Scale() = { 256.f,50.f,30.f };
		HeightCollider[2]->Pos() = { 239.f,25.f,128.f };
		HeightCollider[2]->Scale() = { 30.f,50.f,256.f };
		HeightCollider[3]->Pos() = { 13.7f,25.f, 128.f };
		HeightCollider[3]->Scale() = { 30.f,50.f,256.f };
		HeightCollider[4]->Pos() = { 128.f,15.f,129.f };
		HeightCollider[4]->Rot() = { 0.f,XMConvertToRadians(21),0.f };
		HeightCollider[4]->Scale() = { 14.7f,22.4f,59.4f };
		HeightCollider[5]->Pos() = { 183.f,15.f, 152.7f };
		HeightCollider[5]->Rot() = { 0.f,0.f,0.f };
		HeightCollider[5]->Scale() = {110.f,30.f,14.1f };
		FOR(6) {
			HeightCollider[i]->Role() = Collider::Collider_Role::BLOCK;
			HeightCollider[i]->UpdateWorld();
			ColliderManager::Get()->SetObstacles(HeightCollider[i]);
		}

	}
	else if (MenuManager::Get()->GetLoadingSequence() == 3)
	{
		// 사운드매니저에서 
		//Audio::Get()->Add("bgm1", "Sounds/dramatic-choir.wav", true, true, false);

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

		// 사운드 추가
		SoundManager::Get()->PlayerCreate(player);

		MenuManager::Get()->IncreaseLoadingSequence();
		MenuManager::Get()->SetLoadingRate(75.f);

		shadow = new Shadow();
	}
	else if (MenuManager::Get()->GetLoadingSequence() == 4)
	{
		InteractManager::Get()->SetPlayer(player);

		FOR(MonsterManager::Get()->GetSIZE())
		{
			MonsterManager::Get()->SetOrcSRT(i, srt[i][0],srt[i][1],srt[i][2]);
			MonsterManager::Get()->SetPatrolPos(i, patrolPos[i]);
		}
		//MonsterManager::Get()->SetOrcSRT(0, Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(150.f, 0.f, 175.f));
		//MonsterManager::Get()->SetPatrolPos(0, Vector3(150.f, 0.f, 210.f));
		//MonsterManager::Get()->SetOrcSRT(1, Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(100, 0, 185));
		//MonsterManager::Get()->SetPatrolPos(1, Vector3(50.f, 0.f, 185.f));
		//MonsterManager::Get()->GetOrc(0)->SetSpeed(8);
		//MonsterManager::Get()->GetOrc(1)->SetSpeed(6);
		//MonsterManager::Get()->SetOrcSRT(2, Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(85, 0, 120));
		//MonsterManager::Get()->SetPatrolPos(2, Vector3(40.f, 0.f, 120.f));   MonsterManager::Get()->SetPatrolPos(2, Vector3(40.f, 0.f, 70.f));   MonsterManager::Get()->SetPatrolPos(2, Vector3(85.f, 0.f, 70.f));
		//MonsterManager::Get()->SetOrcSRT(3, Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(62, 0, 40));
		//MonsterManager::Get()->SetPatrolPos(3, Vector3(62.f, 0.f, 65.f));
		//MonsterManager::Get()->SetOrcSRT(4, Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(110, 0, 80));
		//MonsterManager::Get()->SetPatrolPos(4, Vector3(110.f, 0.f, 40.f));
		//MonsterManager::Get()->SetOrcSRT(5, Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(170, 0, 40));
		//MonsterManager::Get()->SetPatrolPos(5, Vector3(170.f, 0.f, 80.f)); MonsterManager::Get()->SetPatrolPos(5, Vector3(120.f, 0.f, 80.f));   MonsterManager::Get()->SetPatrolPos(5, Vector3(170.f, 0.f, 80.f));
		//MonsterManager::Get()->SetOrcSRT(6, Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(160, 0, 120));
		//MonsterManager::Get()->SetPatrolPos(6, Vector3(170.f, 0.f, 120.f));
		//MonsterManager::Get()->SetOrcSRT(7, Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(50, 0, 135));
		//MonsterManager::Get()->SetPatrolPos(7, Vector3(100.f, 0.f, 135.f));
		//MonsterManager::Get()->SetType(7,1);// 1이 알리는애
		MonsterManager::Get()->SetType(3,1);// 1이 알리는애
		MonsterManager::Get()->SetTerrain(terrain);
		MonsterManager::Get()->SetAStar(aStar);
		MonsterManager::Get()->SetTarget(player);
		MonsterManager::Get()->SetTargetCollider(player->GetCollider()); 
		MonsterManager::Get()->SetTargetStateInfo(player->GetStateInfo());

		boss = new Boss;
		boss->SetTarget(player);
		boss->SetTargetCollider(player->GetCollider());
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

		endingCredit = new Quad(Vector2(1280, 720 * 6));
		endingCredit->GetMaterial()->SetDiffuseMap(L"Textures/Etc/endingCredit.png");
		endingCredit->Pos() = { CENTER_X,-720.f * 2.f,0.f };
		endingCredit->UpdateWorld();

		videoPlayer = new VideoPlayer(192 * 2, 108 * 2);
		videoPlayer->Pos() = { CENTER_X - 300 ,CENTER_Y + 100,0 };
		videoPlayer->UpdateWorld();
	}

	if (MenuManager::Get()->GetLoadingSequence() == 6)
	{
		MenuManager::Get()->SetLoadingRate(100.f);
	}
}