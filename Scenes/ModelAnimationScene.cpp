#include "Framework.h"
#include "ModelAnimationScene.h"

ModelAnimationScene::ModelAnimationScene()
{

//	background = new Model("Forest");
//	background->SetShader(L"Basic/Texture.hlsl");
//	background->Scale() *= 10.0f;
//	background->UpdateWorld();
//
//	skyBox = new SkyBox(L"Textures/Landscape/BlueSky.dds");
//
//	FOR(2)
//		blendState[i] = new BlendState();
//	blendState[1]->AlphaToCoverage(true); // ���� Ȥ�� ������ ������ �ܺ� �ȼ��� ������ ���ΰ�
//
	
	//terrainEditor = new TerrainEditor();

	/*
	{
		string modelName = "crypto"; //���� �̸�
		string filePath = "Models/FBX/" + modelName + ".fbx";

		ModelExporter* exporter = new ModelExporter(modelName, filePath);
		exporter->ExportMaterial();
		exporter->ExportMesh();

		delete exporter;
	}
	*/
	//Ŭ�� �����صα� 
	string modelName = "akai";
	string clipNames[] = {
		"Bow/Attack/Standing Aim Overdraw"
		/*"character1@idle1"
		,"character1@idle2"
		,"character1@idle3"
		,"character1@idle4"
		,"character1@idle5"
		,"character1@jump"
		,"character1@run"
		,"character1@run2"
		,"character1@run3"
		,"character1@walk"
		,"character1@walk3"
		,"character1@walkshield"*/
		//"character1@atack1"
		// "character1@atack2"
		//,"character1@atack3"
		//,"character1@atack4"
		//,"character1@atack5"
		//,"character1@atack6"
		//,"character1@atack7"
		//,"character1@atack8"
		//,"character1@atack9"
		//,"character1@atack10"
		//,"character1@atack11"
		//,"character1@atack12"
		//,"character1@atack13"
		//,"character1@atack14"
		//,"character1@atack15"
		//,"character1@atack16"
		//,"character1@atack17"
		//,"character1@atack18"
		//,"character1@atack19"
		//,"character1@atack20"
		//,"character1@atack21"
		//,"character1@atack22"
		//,"character1@atack23"
		//,"character1@atack24"
		//,"character1@atack25"
		//,"character1@atack26"
		//,"character1@backwalkshield"
		//,"character1@sneakcrunchback"
		//,"character1@sneakwalk"
		//,"character1@sneakwalk2"
		//,"character1@strafeleft"
		//,"character1@straferight"
		//,"character1@death1"
		//,"character1@death2"
		//,"character1@death3"
		//,"character1@death4"
		//,"character1@dodge"
		//,"character1@gethit"
	};
	/*string clipNames[] = {
		"Side Kick"
		,"Punching"
		,"Head Hit"
	};*/
	/*
	string modelName = "character1";
	string clipNames[] = {
		"Idle"
		,"kick"
		
	};
	*/
	model = new ModelAnimator("akai");
	clipSize = (sizeof(clipNames) / sizeof(*clipNames));
	/*
	for(string clipName : clipNames)
	{
		string clipPath = "Models/Animations/" + modelName + "/" + clipName + ".fbx";
		ModelExporter* exporter = new ModelExporter(modelName, clipPath);
		exporter->ExportClip(clipName);
		delete exporter;
	}
	*/
	//model = new ModelAnimator("character1");
	for (string clipName : clipNames)
	{
		model->ReadClip(clipName,0);
	}

	

	//model = new ModelAnimator("Human");
	//model->ReadClip("Idle",1); // ������ 0�ڿ� 1���� ����
	//model->ReadClip("Run",1);
	//model->ReadClip("Jump",1);

	//���� �� �ڵ�� ������ ���� �� �����Դϴ�
	// ���� : �ΰ�.FBX ���Ͽ� ������ 0���� �ƴ� 1���� ���� �׷����ϴ�
	// (������ ���� ������� ���� 0-1,1-1...������ ������ �ս��ϴ�)
	//	�̷� ������ ������ ������ ���� ���� ���� (����Ȯ�� �ʿ�)
	//�Ű������� �����־�� �մϴ�
}

ModelAnimationScene::~ModelAnimationScene()
{
	delete model;
	delete only_model;
	delete only_model2;
	delete only_model3;
	FOR(7) {
		delete boxCollider[i];
	}
	delete terrainEditor;
	delete background;
	delete skyBox;

}

void ModelAnimationScene::Update()
{
	//terrainEditor->Update();
	/*
	if (KEY_PRESS('W')) {
		only_model->Pos() -= {1.f, 0.f, 0.f};

	}
	else if (KEY_PRESS('S'))
	{
		only_model->Pos() += {1.f, 0.f, 0.f};
	}
	FOR(7)
	{
		boxCollider[i]->UpdateWorld();
	}
	*/
	//only_model->UpdateWorld();
	//only_model2->UpdateWorld();
	//only_model3->UpdateWorld();
	
	model->Update();
	/*
	bool idleFlag = true;

	if (!KEY_PRESS(VK_RBUTTON))
	{
		if (KEY_PRESS('W'))
		{
			model->Pos() += model->Back() * 10.0f * DELTA;
			model->PlayClip(2, 1.0f, 2.0f);
			idleFlag = false;
		}

		if (KEY_PRESS('S'))
		{
			model->Pos() += model->Forward() * 10.0f * DELTA;
			model->PlayClip(2, 1.0f, 2.0f);
			idleFlag = false;
		}

		if (KEY_PRESS('A'))
		{
			model->Pos() += model->Right() * 10.0f * DELTA;
			model->PlayClip(2, 1.0f, 2.0f);
			idleFlag = false;
		}

		if (KEY_PRESS('D'))
		{
			model->Pos() += model->Left() * 10.0f * DELTA;
			model->PlayClip(2, 1.0f, 2.0f);
			idleFlag = false;
		}

		if (KEY_UP(VK_SPACE))
		{
			idleFlag = false;
			jumpFlag = true;
		}


	}

	if (jumpFlag)
	{
		jumpCoolDown += DELTA * 1.0f;
		if (jumpCoolDown <= 0.1f)
		{
			model->PlayClip(0, 1.0f, 0.1f);
		}
		else
		{
			if (oneFlag)
			{
				model->PlayClip(1, 0.6f, 1.0f);
				oneFlag = false;
			}
		}

		if (jumpCoolDown >= 1.1f)
		{
			jumpCoolDown = 0.0f;
			jumpFlag = false;
			oneFlag = true;
			model->PlayClip(0, 1.0f, 0.1f);
		}
	}
	else if(idleFlag)
		model->PlayClip(0, 1.0f, 2.0f);
		*/
}

void ModelAnimationScene::PreRender()
{
}

void ModelAnimationScene::Render()
{
	//skyBox->Render();

	//blendState[1]->SetState(); // ���� �����ϱ�
	////background->Render();      // ���� ����� ���¿��� ��游 ����ϱ�
	//blendState[0]->SetState(); // ���� �����ϱ�

	//terrainEditor->Render();
	model->Render();
	//only_model->Render();
	//only_model2->Render();
	//only_model3->Render();
	/*
	FOR(7)
	{
		boxCollider[i]->Render();
	}
	*/
}

void ModelAnimationScene::PostRender()
{
}

void ModelAnimationScene::GUIRender()
{
	//terrainEditor->GUIRender();
	model->GUIRender();
	//only_model->GUIRender();
	//only_model2->GUIRender(); only_model3->GUIRender();
	/*
	FOR(7)
	{
		boxCollider[i]->GUIRender();
	}
	*/
	ImGui::SliderInt("Clip", &clip, 0, clipSize-1);
	if (ImGui::Button("Play"))
	{
		model->PlayClip(clip);
	}
}
