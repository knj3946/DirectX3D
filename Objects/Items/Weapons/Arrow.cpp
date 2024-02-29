#include "Framework.h"
#include "Arrow.h"

Arrow::Arrow(Transform* transform, int id,bool isDropItem)
	: transform(transform),index(id),isDropItem(isDropItem)
{
	transform->Scale() = { 3,3,3 }; // 크기 기본 값은 1 -> 수정하기 쉽게 작성된 코드

	collider = new SphereCollider();
	collider->SetParent(transform);

	collider->Scale() = { 0.2,0.2,0.2 }; // 크기 기본 값은 1
	collider->Pos() = {};			// 위치 기본 값 : 부모 위치

	// 궤적 사용하기

	startEdge = new Transform();
	startEdge->Pos() = transform->Pos();
	endEdge = new Transform();
	endEdge->Pos() = transform->Pos();
	startEdge->UpdateWorld();
	endEdge->UpdateWorld();
	trail = new Trail(L"Textures/Effect/Trail.png", startEdge, endEdge, 5, 5);
	trail->Init();
	trail->SetActive(false);
}

Arrow::~Arrow()
{
	delete collider;

	delete trail;
	delete startEdge;
	delete endEdge;
}

void Arrow::Update()
{
	// 비활성화 중에는 안 움직임
	if (!transform->Active()||isDropItem)
	{
		return;
	}

	time += DELTA; // 시간 경과에 따라 변수에 누적

	if (time > LIFE_SPAN)
	{
		trail->SetActive(false);
		transform->SetActive(false);
	}

	transform->Pos() += direction * speed * DELTA;

	collider->UpdateWorld();


	if (trail != nullptr)
	{
		startEdge->Pos() = transform->GlobalPos() + transform->Up() * 0.5f; // 20.0f :모델크기반영
		endEdge->Pos() = transform->GlobalPos() + transform->Down() * 0.5f; // 20.0f :모델크기반영

		// 찾아낸 꼭지점 위치를 업데이트 
		startEdge->UpdateWorld();
		endEdge->UpdateWorld(); // 이러면 트랜스폼에 위치가 담긴다
		trail->Update();
	}
}

void Arrow::Render()
{
	collider->Render();

	if (isDropItem)return;

	if (trail != nullptr)
		trail->Render();
}

void Arrow::Throw(Vector3 pos, Vector3 dir)
{
	// 활성화
	transform->SetActive(true);
	transform->Pos() = pos;
	direction = dir;

	startEdge->Pos() = transform->Pos();
	endEdge->Pos() = transform->Pos();
	startEdge->UpdateWorld();
	endEdge->UpdateWorld();
	trail->Init();
	trail->SetActive(true);

	// 방향에 맞게 모델(=트랜스폼) 회전 적용
	transform->Rot().y = atan2(dir.x, dir.z) - XM_PIDIV2; // 방향 적용 + 모델 정면에 따른 보정
	// 쿠나이 모델은 90도 돌아가 있었음

//transform->Rot().y = atan2(dir.x, dir.z) - XMConvertToRadians(90);
// XMConvertToRadians 는 각도를 호도로 바꿔주는 함수 - 추천 x

	time = 0; // 경과시간 리셋
}

void Arrow::SetOutLine(bool flag)
{
	if (transform->Active() == false)return;
	outLine = flag;
	ArrowManager::Get()->GetInstancing()->SetOutLine(index, flag);
}
