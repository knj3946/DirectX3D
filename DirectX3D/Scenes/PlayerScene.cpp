#include "Framework.h"
#include "PlayerScene.h"

PlayerScene::PlayerScene()
{
	player = new Player();

	box1 = new BoxCollider();
	box1->Pos() = { 0, 0, -500 };
	box1->Scale().x *= 1000;
	box1->Scale().z *= 100;
	box1->Scale().y *= 1000;
	box1->SetTag("1");

	box2 = new BoxCollider();
	box2->Pos() = { 0, 0, -1000 };
	box2->Scale().x *= 500;
	box2->Scale().z *= 100;
	box2->Scale().y *= 1000;
	box2->SetTag("2");

	/*CAM->SetTarget(player);
	CAM->TargetOptionLoad("Player");
	CAM->LookAtTarget();*/

	colliders.push_back(box1);
	colliders.push_back(box2);
}

PlayerScene::~PlayerScene()
{
	delete player;

	for (Collider* collider : colliders)
		delete collider;
}

void PlayerScene::Update()
{
	//player->Wall(box);

	player->Update();

	FOR(2)
		colliders[i]->UpdateWorld();


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
		if(collider->IsRayCollision(player->GetRay(), &cont) && cont.distance < 500.0f && cont.distance < minDistance)
		{
			player->ResetTarget(collider, cont);
			isSearching = true;
		}
		else
			collider->SetColor({ 0, 0, 1, 0 });
	}
	if(!isSearching)
		player->ResetTarget(nullptr, cont);
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
