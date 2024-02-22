#include "Framework.h"
#include "PlayerScene.h"

PlayerScene::PlayerScene()
{
	ColliderManager::Get();
<<<<<<< HEAD

	player = new Player();
=======
>>>>>>> b40f8b5ae3a8100a0a483a3fe97de15bb1be7890

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

<<<<<<< HEAD
	CAM->SetTarget(player);
	CAM->TargetOptionLoad("Naruto");
	//CAM->LookAtTarget();
=======
	player = new Player();
	player->Scale() = { 0.03f,0.03f,0.03f };
	player->Pos() = { 60,0,90 };

	player->SetMoveSpeed(50);

	CAM->SetTarget(player);
	CAM->TargetOptionLoad("GameMapScenePlayer");
	CAM->LookAtTarget();
>>>>>>> b40f8b5ae3a8100a0a483a3fe97de15bb1be7890

	colliders.push_back(box1);
	colliders.push_back(box2);

<<<<<<< HEAD
	ColliderManager::Get()->SetPlayer(player, player->GetCollider());
=======
	ColliderManager::Get()->SetPlayer(player);
>>>>>>> b40f8b5ae3a8100a0a483a3fe97de15bb1be7890
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
<<<<<<< HEAD


	bool isSearching = false;

	//if (box1->IsRayCollision(player->GetRay(), &cont) && Distance(player->Pos(), box1->Pos()) < 500.0f)
	//{
	//	player->ResetTarget(box1);
	//	box1->SetColor({ 1, 0, 0, 0 });
	//	isSearching = true;
	//}
	//else {
	//	box1->SetColor({ 0, 0, 1, 0 });
	//}
	Contact cont;
	float minDistance = 100000000000.0f;
	for(Collider* collider : colliders)
	{
		if(collider->IsRayCollision(player->GetRay(), &cont) && cont.distance < 80.0f && cont.distance < minDistance)
		{
			player->ResetTarget(collider, cont);
			isSearching = true;
		}
		//else
			//collider->SetColor({ 0, 0, 1, 0 });
	}
	if(!isSearching)
		player->ResetTarget(nullptr, cont);
=======
>>>>>>> b40f8b5ae3a8100a0a483a3fe97de15bb1be7890
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

	ImGui::Value("a", a);
	ImGui::Value("b", a);
}
