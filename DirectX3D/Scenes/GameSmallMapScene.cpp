#include "Framework.h"
#include "GameSmallMapScene.h"

GameSmallMapScene::GameSmallMapScene()
{
	MenuManager::Get(); //기본 메뉴 생성
}

GameSmallMapScene::~GameSmallMapScene()
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

void GameSmallMapScene::Update()
{
	MenuManager::Get()->FirstLoading(bind(&GameSmallMapScene::FirstLoading, this));

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
		if (!bow->Active())
			bow->Update();

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

void GameSmallMapScene::PreRender()
{
	if (MenuManager::Get()->GetSelectGameMenu() == 1)
	{
		shadow->SetRenderTarget();
		player->SetShader(L"Light/DepthMap.hlsl");
		player->Render();
		MonsterManager::Get()->Render(true);
	}
}

void GameSmallMapScene::Render()
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

		if (bow->Active())
			bow->Render();

		aStar->Render();
	}
}

void GameSmallMapScene::PostRender()
{
	MenuManager::Get()->PostRender();

	if (MenuManager::Get()->GetSelectGameMenu() == 1)
	{
		MonsterManager::Get()->PostRender();
		ArrowManager::Get()->PostRender();
		ColliderManager::Get()->PostRender();
		player->PostRender();

		player->TextRender();
	}

}

void GameSmallMapScene::GUIRender()
{

	MenuManager::Get()->GUIRender();
	//player->GUIRender();

	if (MenuManager::Get()->GetSelectGameMenu() == 1)
	{
		/*
		for (ColliderModel* cm : colliderModels)
		{
			//cm->GUIRender();
		}
		*/

		//MonsterManager::Get()->GUIRender();
		//KunaiManager::Get()->GUIRender();

		//Timer::Get()->GUIRender();
		/*
		if (bow->Active())
			bow->GUIRender();
		*/

		MonsterManager::Get()->GUIRender();
	}

}

void GameSmallMapScene::FirstLoading()
{
	if (MenuManager::Get()->GetLoadingSequence() == 0)
	{
		skyBox = new SkyBox(L"Textures/Landscape/BlueSky.dds");

		FOR(2)
			blendState[i] = new BlendState();
		blendState[1]->AlphaToCoverage(true); // 알파 혹은 지정된 배경색을 외부 픽셀과 결합할 것인가
		MenuManager::Get()->IncreaseLoadingSequence();
		int a=MenuManager::Get()->Getsequence();
		MenuManager::Get()->SetLoadingRate(10.f);

		LightBuffer::Light* light = Environment::Get()->GetLight(0); // 기본으로 설정된 빛 가져오기

		light->type = 1;               //광원 종류 (상세 설명은 이후에)
		light->pos = { 0, 50, -50 }; //광원 위치
		light->range = 3000;           //조명 범위 (빛이 실제로 닿는 범위)
	}
	else if (MenuManager::Get()->GetLoadingSequence() == 1)
	{
		terrain = new TerrainEditor(64, 64);
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
			Vector3 mPos = { 20.f,0,20.f };

			ColliderModel* cm = ColliderManager::Get()->CreateColliderModel(mName, mTag, mScale, mRot, mPos);
			colliderModels.push_back(cm);
		}
		aStar = new AStar(32, 32);
		aStar->SetNode(terrain);

		MenuManager::Get()->IncreaseLoadingSequence();
		MenuManager::Get()->SetLoadingRate(55.f);
	}
	else if (MenuManager::Get()->GetLoadingSequence() == 3)
	{
		Audio::Get()->Add("bgm1", "Sounds/dramatic-choir.wav", true, true, false);

		player = new Player();
		player->Scale() = { 0.03f,0.03f,0.03f };
		player->Pos() = { 30.f,0.f,10.f };
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
		MonsterManager::Get()->SetOrcSRT(0, Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(10.f, 0.f, 10.f));
		MonsterManager::Get()->SetPatrolPos(0, Vector3(10.f, 0.f, 60.f));
		MonsterManager::Get()->SetTerrain(terrain);
		MonsterManager::Get()->SetAStar(aStar);
		MonsterManager::Get()->SetTarget(player);
		MonsterManager::Get()->SetTargetCollider(player->GetCollider());
		MonsterManager::Get()->SetTargetStateInfo(player->GetStateInfo());

		MenuManager::Get()->IncreaseLoadingSequence();
		MenuManager::Get()->SetLoadingRate(95.f);
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