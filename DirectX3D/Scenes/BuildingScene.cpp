#include "Framework.h"
#include "BuildingScene.h"

BuildingScene::BuildingScene()
{
	{
		string mName = "building_V2";
		string mTag = "model1";
		Vector3 mScale = { 3,3,3 };
		Vector3 mRot = { 0,0,0 };
		Vector3 mPos = { 0,0,0 };

		ColliderModel* cm = ColliderManager::Get()->CreateColliderModel(mName, mTag, mScale, mRot, mPos);
		colliderModels.push_back(cm);
	}

	{
		string mName = "building_V4";
		string mTag = "model2";
		Vector3 mScale = { 3,3,3 };
		Vector3 mRot = { 0,0,0 };
		Vector3 mPos = { 40,0,0 };

		ColliderModel* cm = ColliderManager::Get()->CreateColliderModel(mName, mTag, mScale, mRot, mPos);
		colliderModels.push_back(cm);
	}

	{
		string mName = "building_V5";
		string mTag = "model3";
		Vector3 mScale = { 3,3,3 };
		Vector3 mRot = { 0,0,0 };
		Vector3 mPos = { -40,0,0 };

		ColliderModel* cm = ColliderManager::Get()->CreateColliderModel(mName, mTag, mScale, mRot, mPos);
		colliderModels.push_back(cm);
	}
}

BuildingScene::~BuildingScene()
{
	for (ColliderModel* colliderModel : colliderModels)
	{
		for (Collider* collider : colliderModel->GetColliders())
		{
			delete collider;
		}

		delete colliderModel;
	}
}

void BuildingScene::Update()
{
	for (ColliderModel* colliderModel : colliderModels)
	{
		for (Collider* collider : colliderModel->GetColliders())
		{
			collider->UpdateWorld();
		}
	}
}

void BuildingScene::PreRender()
{
}

void BuildingScene::Render()
{
	for (ColliderModel* colliderModel : colliderModels)
	{
		for (Collider* collider : colliderModel->GetColliders())
		{
			collider->Render();
		}
		colliderModel->Render();
	}
}

void BuildingScene::PostRender()
{
}

void BuildingScene::GUIRender()
{
	for (ColliderModel* colliderModel : colliderModels)
	{
		for (Collider* collider : colliderModel->GetColliders())
		{
			collider->GUIRender();
		}
	}
}
