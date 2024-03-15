#include "Framework.h"
#include "Arrow.h"

Arrow::Arrow(Transform* transform, int id,bool isDropItem)
	: transform(transform),index(id),isDropItem(isDropItem)
{
	transform->Scale() = { 0.05, 0.05 , 0.02 }; // ũ�� �⺻ ���� 1 -> �����ϱ� ���� �ۼ��� �ڵ�
	//transform->Rot().y += 1.57f;

	collider = new SphereCollider();
	collider->Scale().z *= 20.0f;
	collider->SetParent(transform);

	collider->Pos() = {};			// ��ġ �⺻ �� : �θ� ��ġ

	// ���� ����ϱ�

	startEdge = new Transform();
	startEdge->Pos() = transform->Pos();
	endEdge = new Transform();
	endEdge->Pos() = transform->Pos();
	startEdge->UpdateWorld();
	endEdge->UpdateWorld();
	//trail = new Trail(L"Textures/Effect/Trail.png", startEdge, endEdge, 5, 5);
	trail = new Trail(L"Textures/Effect/wind.jpg", startEdge, endEdge, 3, 6); // ���ǵ� �ӽ÷� 6���� ����
	trail->Init();
	trail->SetActive(false);
	HitEffect = new Sprite(L"Textures/Effect/HitEffect.png", 15, 15, 5, 2, false);
	Wallparticle = new ParticleSystem("TextData/Particles/WallEffect.fx");
}

Arrow::~Arrow()
{
	delete collider;
	delete trail;
	delete startEdge;
	delete endEdge;

	delete HitEffect;
	delete Wallparticle;
}

void Arrow::Update()
{
	// ��Ȱ��ȭ �߿��� �� ������
	if (!transform->Active() || isDropItem)
	{
		return;
	}

	time += DELTA; // �ð� ����� ���� ������ ����

	if (time > LIFE_SPAN || transform->GlobalPos().y < 0.0f)
	{
		trail->SetActive(false);
		transform->SetActive(false);
	}

	if (isDropItem)
		transform->Pos() += direction * speed * DELTA;
	else
	{
		transform->Pos() += direction * DELTA;
		direction.y -= 9.8f * DELTA * 2.f;

		Vector3 front = { direction.x, 0.0f, direction.z};
		if(direction.y >= 0.0f)
			transform->Rot().x = acos(front.Length() / direction.Length());
		else
			transform->Rot().x = -acos(front.Length() / direction.Length());
	}
	HitEffect->Update();
	Wallparticle->Update();
	collider->UpdateWorld();



	if (trail != nullptr)
	{
		startEdge->Pos() = transform->GlobalPos() + transform->Up() * 0.5f; // 20.0f :��ũ��ݿ�
		endEdge->Pos() = transform->GlobalPos() + transform->Down() * 0.5f; // 20.0f :��ũ��ݿ�

		// ã�Ƴ� ������ ��ġ�� ������Ʈ 
		startEdge->UpdateWorld();
		endEdge->UpdateWorld(); // �̷��� Ʈ�������� ��ġ�� ����
		trail->Update();
	}
}

void Arrow::Render()
{
	collider->Render();
	HitEffect->Render();
	Wallparticle->Render();
	if (isDropItem)return;

	if (trail != nullptr)
		trail->Render();
}

void Arrow::Throw(Vector3 pos, Vector3 dir)
{
	// Ȱ��ȭ
	transform->SetActive(true);
	collider->SetActive(true);
	transform->Pos() = pos;
	direction = dir;

	startEdge->Pos() = transform->Pos();
	endEdge->Pos() = transform->Pos();
	startEdge->UpdateWorld();
	endEdge->UpdateWorld();
	trail->Init();
	trail->SetActive(true);

	// ���⿡ �°� ��(=Ʈ������) ȸ�� ����
	transform->Rot().y = atan2(dir.x, dir.z) - XM_PI; // ���� ���� + �� ���鿡 ���� ����
	// ���� ���� 90�� ���ư� �־���

//transform->Rot().y = atan2(dir.x, dir.z) - XMConvertToRadians(90);
// XMConvertToRadians �� ������ ȣ���� �ٲ��ִ� �Լ� - ��õ x

	time = 0; // ����ð� ����
}

void Arrow::Throw(Vector3 pos, Vector3 dir, float chargingT)
{
	// Ȱ��ȭ
	transform->SetActive(true);
	collider->SetActive(true);
	transform->Pos() = pos;

	direction = dir * chargingT;

	startEdge->Pos() = transform->Pos();
	endEdge->Pos() = transform->Pos();
	startEdge->UpdateWorld();
	endEdge->UpdateWorld();
	trail->Init();
	trail->SetActive(true);

	// ���⿡ �°� ��(=Ʈ������) ȸ�� ����
	transform->Rot().y = atan2(dir.x, dir.z) - XM_PI; // ���� ���� + �� ���鿡 ���� ����
	// ���� ���� 90�� ���ư� �־���

//transform->Rot().y = atan2(dir.x, dir.z) - XMConvertToRadians(90);
// XMConvertToRadians �� ������ ȣ���� �ٲ��ִ� �Լ� - ��õ x

	speed = chargingT;
	//������ �ٲٴ°͵� ����ϱ�

	time = 0; // ����ð� ����
}

void Arrow::SetOutLine(bool flag)
{
	if (transform->Active() == false)return;
	outLine = flag;
	ArrowManager::Get()->GetInstancing()->SetOutLine(index, flag);
}

void Arrow::GetItem()// �� �������� �÷��̾ �ֿ��� ��
{
	collider->SetActive(false);
	ArrowManager::Get()->SetActiveSpecialKey(false);
	
	isDropItem = false;
	transform->SetActive(false);
}

void Arrow::HitEffectActive()
{
	HitEffect->Play(startEdge->Pos());
}

void Arrow::WallEffectActive()
{
	Wallparticle->Play(startEdge->Pos());
}
