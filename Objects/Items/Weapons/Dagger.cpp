#include "Framework.h"
#include "Dagger.h"

Dagger::Dagger(Transform* parent) : Model("dagger")
{
	SetParent(parent);

	Scale() *= 100;
	Pos() = { 6.1, 10, -2.1 };
	Rot().z = -1.5f;

	collider = new CapsuleCollider(.1, .5);
	collider->SetParent(this); // �ӽ÷� ���� �浹ü�� "��" Ʈ�������� �ֱ�
	collider->Pos().y = 0.2f; // �ӽ÷� ���� �浹ü�� "��" Ʈ�������� �ֱ�
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
