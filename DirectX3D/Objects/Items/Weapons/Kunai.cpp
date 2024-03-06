#include "Framework.h"
#include "Kunai.h"

Kunai::Kunai(Transform* transform)
	: transform(transform)
{
	transform->Scale() = { 3,3,3 }; // ũ�� �⺻ ���� 1 -> �����ϱ� ���� �ۼ��� �ڵ�

	collider = new SphereCollider();
	collider->SetParent(transform);

	collider->Scale() = { 0.1,0.1,0.1 }; // ũ�� �⺻ ���� 1
	collider->Pos() = {};			// ��ġ �⺻ �� : �θ� ��ġ

	// ���� ����ϱ�

	startEdge = new Transform();
	startEdge->Pos() = transform->Pos();
	endEdge = new Transform();
	endEdge->Pos() = transform->Pos();
	startEdge->UpdateWorld();
	endEdge->UpdateWorld();
	trail = new Trail(L"Textures/Effect/images.jpg", startEdge, endEdge, 1.1f, 20);
	trail->Init();
	trail->SetActive(false);
}

Kunai::~Kunai()
{
	delete collider;

	delete trail;
	delete startEdge;
	delete endEdge;
}

void Kunai::Update()
{
	// ��Ȱ��ȭ �߿��� �� ������
	if (!transform->Active())
	{
		return;
	}

	time += DELTA; // �ð� ����� ���� ������ ����

	if (time > LIFE_SPAN)
	{
		trail->SetActive(false);
		transform->SetActive(false);
	}

	transform->Pos() += direction * speed * DELTA;

	collider->UpdateWorld();


	if (trail != nullptr)
	{
		startEdge->Pos() = transform->GlobalPos() + transform->Up() * 0.15f; // 20.0f :��ũ��ݿ�
		endEdge->Pos() = transform->GlobalPos() + transform->Down() * 0.15f; // 20.0f :��ũ��ݿ�

		// ã�Ƴ� ������ ��ġ�� ������Ʈ 
		startEdge->UpdateWorld();
		endEdge->UpdateWorld(); // �̷��� Ʈ�������� ��ġ�� ����
		trail->Update();
	}
}

void Kunai::Render()
{
	if (!transform->Active())return;
	collider->Render();

	if (trail != nullptr)
		trail->Render();
}

void Kunai::Throw(Vector3 pos, Vector3 dir)
{
	// Ȱ��ȭ
	transform->SetActive(true);
	transform->Pos() = pos;
	direction = dir;

	startEdge->Pos() = transform->Pos();
	endEdge->Pos() = transform->Pos();
	startEdge->UpdateWorld();
	endEdge->UpdateWorld();
	trail->Init();
	trail->SetActive(true);

	// ���⿡ �°� ��(=Ʈ������) ȸ�� ����
	transform->Rot().y = atan2(dir.x, dir.z) - XM_PIDIV2; // ���� ���� + �� ���鿡 ���� ����
														  // ������ ���� 90�� ���ư� �־���

	//transform->Rot().y = atan2(dir.x, dir.z) - XMConvertToRadians(90);
	// XMConvertToRadians �� ������ ȣ���� �ٲ��ִ� �Լ� - ��õ x

	time = 0; // ����ð� ����
}