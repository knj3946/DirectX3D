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
}

Dagger::~Dagger()
{
	delete collider;
}

void Dagger::Update()
{
	UpdateWorld();
	collider->UpdateWorld();
}

void Dagger::Render()
{
	Model::Render();
	collider->Render();
}

void Dagger::PostRender()
{
}

void Dagger::GUIRender()
{

}
