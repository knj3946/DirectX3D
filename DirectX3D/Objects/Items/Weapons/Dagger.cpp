#include "Framework.h"
#include "Dagger.h"

Dagger::Dagger(Transform* parent) : Model("dagger")
{
	SetParent(parent);

	Scale() *= 100;
	Pos() = { 6.1, 10, -2.1 };
	Rot().z = -1.5f;

	collider = new CapsuleCollider(.1, .5);
	collider->SetParent(this); // 임시로 만든 충돌체를 "손" 트랜스폼에 주기
	collider->Pos().y = 0.2f; // 임시로 만든 충돌체를 "손" 트랜스폼에 주기

	startEdge = new Transform();
	startEdge->Pos() = Pos();
	endEdge = new Transform();
	endEdge->Pos() = Pos();
	startEdge->UpdateWorld();
	endEdge->UpdateWorld();

	trail = new Trail(L"Textures/Effect/wind.jpg", startEdge, endEdge, 100, 100);
	trail->Init();
	trail->SetActive(false);
}

Dagger::~Dagger()
{
	delete collider;

	delete trail;
	delete startEdge;
	delete endEdge;
}

void Dagger::Update()
{
	UpdateWorld();
	collider->UpdateWorld();


	if (trail != nullptr)
	{
		startEdge->Pos() = GlobalPos() + Up() * 0.5f; // 20.0f :모델크기반영
		endEdge->Pos() = GlobalPos() + Down() * 0.5f; // 20.0f :모델크기반영

		// 찾아낸 꼭지점 위치를 업데이트 
		startEdge->UpdateWorld();
		endEdge->UpdateWorld(); // 이러면 트랜스폼에 위치가 담긴다
		trail->Update();
	}
}

void Dagger::Render()
{
	Model::Render();
	//collider->Render();

	if (trail != nullptr)
		trail->Render();
}

void Dagger::PostRender()
{
}

void Dagger::GUIRender()
{
	ImGui::InputFloat3("daggerPos", (float*)&Pos());
	ImGui::InputFloat3("daggerRot", (float*)&Rot());
}
