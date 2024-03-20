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
		startEdge->Pos() = GlobalPos() + Up() * 0.5f; // 20.0f :��ũ��ݿ�
		endEdge->Pos() = GlobalPos() + Down() * 0.5f; // 20.0f :��ũ��ݿ�

		// ã�Ƴ� ������ ��ġ�� ������Ʈ 
		startEdge->UpdateWorld();
		endEdge->UpdateWorld(); // �̷��� Ʈ�������� ��ġ�� ����
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
