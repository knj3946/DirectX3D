#include "Framework.h"
#include "Arrow.h"

Arrow::Arrow(Transform* transform, int id,bool isDropItem)
	: transform(transform),index(id),isDropItem(isDropItem)
{
	transform->Scale() = { 0.05, 0.05 , 0.02 }; // 크기 기본 값은 1 -> 수정하기 쉽게 작성된 코드
	//transform->Rot().y += 1.57f;

	collider = new SphereCollider();
	collider->Scale().z *= 20.0f;
	collider->SetParent(transform);

	collider->Pos() = {};			// 위치 기본 값 : 부모 위치

	// 궤적 사용하기

	startEdge = new Transform();
	startEdge->Pos() = transform->Pos();
	endEdge = new Transform();
	endEdge->Pos() = transform->Pos();
	startEdge->UpdateWorld();
	endEdge->UpdateWorld();
	//trail = new Trail(L"Textures/Effect/Trail.png", startEdge, endEdge, 5, 5);
	trail = new Trail(L"Textures/Effect/wind.jpg", startEdge, endEdge, 3, 6); // 스피드 임시로 6으로 설정
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
	// 비활성화 중에는 안 움직임
	if (!transform->Active() || isDropItem)
	{
		return;
	}

	time += DELTA; // 시간 경과에 따라 변수에 누적

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
	HitEffect->Render();
	Wallparticle->Render();
	if (isDropItem)return;

	if (trail != nullptr)
		trail->Render();
}

void Arrow::Throw(Vector3 pos, Vector3 dir)
{
	// 활성화
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

	// 방향에 맞게 모델(=트랜스폼) 회전 적용
	transform->Rot().y = atan2(dir.x, dir.z) - XM_PI; // 방향 적용 + 모델 정면에 따른 보정
	// 쿠나이 모델은 90도 돌아가 있었음

//transform->Rot().y = atan2(dir.x, dir.z) - XMConvertToRadians(90);
// XMConvertToRadians 는 각도를 호도로 바꿔주는 함수 - 추천 x

	time = 0; // 경과시간 리셋
}

void Arrow::Throw(Vector3 pos, Vector3 dir, float chargingT)
{
	// 활성화
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

	// 방향에 맞게 모델(=트랜스폼) 회전 적용
	transform->Rot().y = atan2(dir.x, dir.z) - XM_PI; // 방향 적용 + 모델 정면에 따른 보정
	// 쿠나이 모델은 90도 돌아가 있었음

//transform->Rot().y = atan2(dir.x, dir.z) - XMConvertToRadians(90);
// XMConvertToRadians 는 각도를 호도로 바꿔주는 함수 - 추천 x

	speed = chargingT;
	//데미지 바꾸는것도 고려하기

	time = 0; // 경과시간 리셋
}

void Arrow::SetOutLine(bool flag)
{
	if (transform->Active() == false)return;
	outLine = flag;
	ArrowManager::Get()->GetInstancing()->SetOutLine(index, flag);
}

void Arrow::GetItem()// 이 아이템을 플레이어가 주웠을 때
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
