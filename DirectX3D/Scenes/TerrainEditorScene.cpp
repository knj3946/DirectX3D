#include "Framework.h"
#include "TerrainEditorScene.h"

TerrainEditorScene::TerrainEditorScene()
{
  
	terrainEditor = new TerrainEditor(256, 256);
	terrainEditor->GetMaterial()->SetDiffuseMap(L"Textures/Landscape/Sand.png");
	terrainEditor->GetMaterial()->SetSpecularMap(L"Textures/Color/Black.png");
	terrainEditor->GetMaterial()->SetNormalMap(L"Textures/Landscape/Sand_Normal.png");
	terrainEditor->SetHeightMap(L"Textures/HeightMaps/SamepleHeightMap02.png");
    //ByteAddress();

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
	FOR(2)
	{
		bc[i] = new BoxCollider;
	}
	bc[0]->Pos() = { 164.4f,7.5f,168.4f };
	bc[1]->Pos() = { 164.4f,7.5f,214.8f };
	bc[0]->Scale() = { 5.f,15.f,27.f };
	bc[1]->Scale() = { 5.f,15.f	,27.f };
}

TerrainEditorScene::~TerrainEditorScene()
{
    delete terrainEditor;
	for (ColliderModel* colliderModel : colliderModels)
	{
		delete colliderModel;
	}
	
}

void TerrainEditorScene::Update()
{
    terrainEditor->Update();

	for (ColliderModel* cm : colliderModels)
	{
		cm->UpdateWorld();
	}
	FOR(2)bc[i]->UpdateWorld();
	
}

void TerrainEditorScene::PreRender()
{
}

void TerrainEditorScene::Render()
{
    terrainEditor->Render();
	for (ColliderModel* cm : colliderModels)
	{
		cm->Render();
	}
	FOR(2)bc[i]->Render();
}

void TerrainEditorScene::PostRender()
{
}

void TerrainEditorScene::GUIRender()
{
    terrainEditor->GUIRender();
	for (ColliderModel* cm : colliderModels)
	{
		cm->GUIRender();
	}
	FOR(2)bc[i]->GUIRender();
}

void TerrainEditorScene::ByteAddress()
{
    ComputeShader* shader = Shader::AddCS(L"Compute/ByteAddress.hlsl");

    struct Output
    {
        UINT groupID[3];
        UINT groupThreadID[3];
        UINT dispatchThreadID[3];
        UINT groupIndex;
    };

    UINT groupX = 2;
    UINT size = 10 * 8 * 3 * groupX;

    Output* output = new Output[size];
    RawBuffer* buffer = new RawBuffer(sizeof(Output) * size);

    DC->CSSetUnorderedAccessViews(0, 1, &buffer->GetUAV(), nullptr);

    shader->Set();

    DC->Dispatch(groupX, 1, 1);

    buffer->Copy(output, sizeof(Output)* size);

    FILE* file;
    fopen_s(&file, "TextData/RawTest.csv", "w");

    for (UINT i = 0; i < size; i++)
    {
        Output temp = output[i];

        fprintf
        (
            file,
            "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
            i,
            temp.groupID[0], temp.groupID[1], temp.groupID[2],
            temp.groupThreadID[0], temp.groupThreadID[1], temp.groupThreadID[2],
            temp.dispatchThreadID[0], temp.dispatchThreadID[1], temp.dispatchThreadID[2],
            temp.groupIndex
        );
    }
}
