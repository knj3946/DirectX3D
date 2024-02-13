#include "Framework.h"
#include "GameMapScene.h"

GameMapScene::GameMapScene()
{
	{
		string mName = "building_V2";
		string mTag = "model1";
		Vector3 mScale = { 3,3,3 };
		Vector3 mRot = { 0,0,0 };
		Vector3 mPos = { 0,0,0 };

		CreateColliderModel(mName, mTag, mScale, mRot, mPos);
	}

	{
		string mName = "building_V4";
		string mTag = "model2";
		Vector3 mScale = { 3,3,3 };
		Vector3 mRot = { 0,0,0 };
		Vector3 mPos = { 30,0,0 };

		CreateColliderModel(mName, mTag, mScale, mRot, mPos);
	}

	{
		string mName = "building_V5";
		string mTag = "model3";
		Vector3 mScale = { 3,3,3 };
		Vector3 mRot = { 0,0,0 };
		Vector3 mPos = { -30,0,0 };

		CreateColliderModel(mName, mTag, mScale, mRot, mPos);
	}
}

GameMapScene::~GameMapScene()
{
}

void GameMapScene::Update()
{
	for (ColliderModel* cm : colliderModels)
	{
		cm->UpdateWorld();
	}
}

void GameMapScene::PreRender()
{
}

void GameMapScene::Render()
{
	for (ColliderModel* cm : colliderModels)
	{
		cm->Render();
	}
}

void GameMapScene::PostRender()
{
}

void GameMapScene::GUIRender()
{
	for (ColliderModel* cm : colliderModels)
	{
		cm->GUIRender();
	}
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
