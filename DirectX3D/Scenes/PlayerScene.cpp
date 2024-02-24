#include "Framework.h"
#include "PlayerScene.h"

PlayerScene::PlayerScene()
{
	ColliderManager::Get();

	box1 = new BoxCollider();
	box1->Pos() = { 0, 0, -50 };
	box1->Scale().x *= 1000;
	box1->Scale().z *= 100;
	box1->Scale().y *= 10;
	box1->SetTag("1");

	box2 = new BoxCollider();
	box2->Pos() = { 0, 0, -100 };
	box2->Scale().x *= 500;
	box2->Scale().z *= 100;
	box2->Scale().y *= 1000;
	box2->SetTag("2");

	player = new Player();
	player->Scale() = { 0.03f,0.03f,0.03f };
	player->Pos() = { 60,0,90 };

	player->SetMoveSpeed(50);

	CAM->SetTarget(player);
	CAM->TargetOptionLoad("GameMapScenePlayer");
	CAM->LookAtTarget();

	colliders.push_back(box1);
	colliders.push_back(box2);

	ColliderManager::Get()->SetPlayer(player);
	ColliderManager::Get()->SetObstacles(box1);
	ColliderManager::Get()->SetObstacles(box2);
}

PlayerScene::~PlayerScene()
{
	delete player;

	for (Collider* collider : colliders)
		delete collider;
}

void PlayerScene::Update()
{
	player->Update();

	FOR(2)
		colliders[i]->UpdateWorld();
}

void PlayerScene::PreRender()
{
}

void PlayerScene::Render()
{
	player->Render();

	for (Collider* collider : colliders)
		collider->Render();
}

void PlayerScene::PostRender()
{
	player->PostRender();
}

void PlayerScene::GUIRender()
{
	player->GUIRender();

	for (Collider* collider : colliders)
		collider->GUIRender();
}
