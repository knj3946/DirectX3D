#include "Framework.h"

Boss::Boss()
	
{
	instancing = new ModelAnimatorInstancing("Warrok W Kurniawan");
	transform = instancing->Add();
	instancing->ReadClip("Idle");
	instancing->ReadClip("Walking");
	instancing->ReadClip("Run Forward");
	instancing->ReadClip("Mutant Swiping");
	instancing->ReadClip("Mutant Roaring");
	
	instancing->ReadClip("Falling Forward Death");
	
	totalEvent.resize(instancing->GetClipSize()); //모델이 가진 동작 숫자만큼 이벤트 리사이징
	eventIters.resize(instancing->GetClipSize());
	index = 0;
	collider = new CapsuleCollider(50, 100);
	collider->SetParent(transform);
	collider->Pos().y += 100;

	leftHand = new Transform;
	leftCollider = new CapsuleCollider(10, 10);
	leftCollider->Scale() *= 2.2f;

	leftCollider->SetParent(leftHand);

	exclamationMark = new Quad(L"Textures/UI/Exclamationmark.png");
	questionMark = new Quad(L"Textures/UI/QuestionMark.png");
	exclamationMark->Scale() *= 0.1f;
	questionMark->Scale() *= 0.1f;
	questionMark->SetActive(false);
	exclamationMark->SetActive(false);
	motion = instancing->GetMotion(index);
	transform->Scale() *= 0.03f;


	SetEvent(ATTACK, bind(&Boss::StartAttack, this), 0.f);
	SetEvent(ATTACK, bind(&Boss::EndAttack, this), 0.99f);
	SetEvent(ATTACK, bind(&Boss::DoingAttack, this), 0.6f);
	SetEvent(ROAR, bind(&Boss::Roar, this), 0.5f);
	SetEvent(DEATH, bind(&Boss::EndDying, this), 0.9f);



	computeShader = Shader::AddCS(L"Compute/ComputePicking.hlsl");
	rayBuffer = new RayBuffer();

	hpBar = new ProgressBar(L"Textures/UI/hp_bar.png", L"Textures/UI/hp_bar_BG.png");
	hpBar->Scale() *= 0.01f;
	hpBar->SetAmount(curHP / maxHP);
	rangeBar = new ProgressBar(L"Textures/UI/Range_bar.png", L"Textures/UI/Range_bar_BG.png");
	rangeBar->SetAmount(DetectionStartTime / DetectionEndTime);
	rangeBar->SetParent(transform);
	rangeBar->Rot() = { XMConvertToRadians(90.f),0,XMConvertToRadians(-90.f) };
	rangeBar->Pos() = { -15.f,1.f,-650.f };
	rangeBar->Scale() = { 1.f / transform->Scale().x,1.f / transform->Scale().y,1.f / transform->Scale().z};
	rangeBar->Scale() *= (eyeSightRange / 100);
}

Boss::~Boss()
{
	delete leftHand;
	delete leftCollider;
	delete collider;
	delete transform;
	delete instancing;
	delete motion;
	delete rangeBar;
	delete hpBar;
	delete exclamationMark;
	delete questionMark;
}
void Boss::DoingAttack() {
	//if(leftCollider->IsCollision(target->))
	//타겟 공격
}
void Boss::Render()
{
	instancing->Render();
	hpBar->Render();
	if(!bFind)
		rangeBar->Render();
	leftCollider->Render();
	
	collider->Render();


}

void Boss::Update()
{




	Idle();
	Direction();
	
	Move();

	
	Die();
	Control();
	ExecuteEvent();
	


	instancing->Update();
	leftHand->SetWorld(instancing->GetTransformByNode(index, 14));

	transform->UpdateWorld();


	leftCollider->UpdateWorld();
	collider->UpdateWorld();
	hpBar->Pos() = transform->Pos() + Vector3(0, 6, 0);
	Vector3 dir = hpBar->Pos() - CAM->Pos();
	hpBar->Rot().y = atan2(dir.x, dir.z);


	hpBar->UpdateWorld();
	rangeBar->UpdateWorld();


	UpdateUI();
}

void Boss::PostRender()
{
	if (!transform->Active())return;
	questionMark->Render();
	exclamationMark->Render();
	
}

void Boss::GUIRender() {
	
	

}

void Boss::CalculateEyeSight()
{

	Vector3 direction = target->GlobalPos() - transform->GlobalPos();
	direction.Normalize();

	float degree = XMConvertToDegrees(transform->Rot().y);

	float dirz = transform->Forward().z;
	float rightdir1 = -(180.f - eyeSightangle) + degree + 360;

	bool breverse = false;
	float leftdir1 = (180.f - eyeSightangle) + degree;
	float Enemytothisangle = XMConvertToDegrees(atan2(direction.x, direction.z));
	if (Enemytothisangle < 0) {
		Enemytothisangle += 360;
	}

	if (Distance(target->GlobalPos(), transform->GlobalPos()) < eyeSightRange) {


		if (leftdir1 <= Enemytothisangle && rightdir1 >= Enemytothisangle) {
	
			// 벽뒤에 숨을때 레이작업 하기..
			bDetection = true;
		}
		else {
			if (Enemytothisangle > 0) {
				Enemytothisangle += 360;
			}

			if (leftdir1 <= Enemytothisangle && rightdir1 >= Enemytothisangle) {
			
				bDetection = true;
			}
		}
	}
	else
		bDetection = false;
}

void Boss::CalculateEarSight()
{
}

void Boss::AddObstacleObj(Collider* collider)
{
}

void Boss::Idle()
{
	//if (curState != STATE::IDLE)return;
	if (state != BOSS_STATE::IDLE)return;
	CalculateEyeSight();
	Detection();



}

void Boss::Direction()
{
}

void Boss::Move()
{
	switch (curState)
	{
	case Boss::IDLE:
		IdleMove();
		break;
	case Boss::WALK:
		IdleWalk();
		break;
	case Boss::RUN:
		Run();
		break;

	}
	

	if (bWait)
		return;
	transform->Rot().y = atan2(dir.x, dir.z) + XM_PI;
	float value = XMConvertToDegrees(transform->Rot().y);

	if (value > 180.f) {
		value -= 180.f;
		value = -180.f + value;
		transform->Rot().y = XMConvertToRadians(value);

	}
	if (value < -180.f) {
		value += 180.f;
		value = 180.f + value;
		transform->Rot().y = XMConvertToRadians(value);

	}
	transform->Pos() += DELTA * moveSpeed * transform->Back();

}
void Boss::IdleMove() {
	if (state != BOSS_STATE::IDLE)return;
	if (IsPatrolPos())
	{

		if (!bWait)
		{
			bWait = true;
			SetState(IDLE);
		}
	}


	if (bWait) {
		WaitTime += DELTA;
	
		if (WaitTime >= 2.F) {
			WaitTime = 0.f;
			bWait = false;
			SetState(WALK);
			curPatrol += 1;
			if (curPatrol >= PatrolPos.size())
				curPatrol = 0;
		}
	}
	
	

}

void Boss::Roar()
{
	//맵에있는 오크들 다부르며 원거리 공격
}



void Boss::JumpAttack()
{

}

void Boss::Die()
{
}

void Boss::Control()
{
}

void Boss::UpdateUI()
{
	if (!exclamationMark->Active() && !questionMark->Active())return;

	// 은신일때 물음표 마크 그리기
	Vector3 barPos = transform->Pos() + Vector3(0, 6, 0);
	/*
		
		 questionMark->Pos() = CAM->WorldToScreen(barPos + Vector3(0, 1, 0));
	*/
	
	exclamationMark->Pos() = CAM->WorldToScreen(barPos + Vector3(0, 1, 0));
	exclamationMark->UpdateWorld();
}



void Boss::SetState(STATE state, float scale, float takeTime)
{
	if (state == curState) return; // 이미 그 상태라면 굳이 변환 필요 없음


	curState = state;
	instancing->PlayClip(index, (int)state); //인스턴싱 내 자기 트랜스폼에서 동작 수행 시작
	eventIters[state] = totalEvent[state].begin(); //이벤트 반복자도 등록된 이벤트 시작시점으로


}

void Boss::SetPath(Vector3 targetPos)
{
	int startIndex = aStar->FindCloseNode(transform->GlobalPos());
	int endIndex = aStar->FindCloseNode(targetPos); // 헤더에서(+업데이트에서) 정해진 목적지

	aStar->GetPath(startIndex, endIndex, path); // 길을 낸 다음 path 벡터에 저장

	aStar->MakeDirectionPath(transform->GlobalPos(), targetPos, path); // 장애물을 지우고 path에 덮어씌우기

	UINT pathSize = path.size(); // 처음 나온 경로 벡터 크기를 저장

	while (path.size() > 2) // "남겨진" 경로 노드가 1군데 이하가 될 때까지
	{
		vector<Vector3> tempPath = path; // 계산용 임시 경로 받아오기
		tempPath.erase(tempPath.begin()); // 최종 목적지 지우기 (장애물이 있었기 때문에 지금은 중간을 가는 중)
		tempPath.pop_back(); // 시작 위치도 생각에서 제외하기 (이미 지나가고 있으니까)

		// 최종과 시작이 빠진 경로의 새로운 시작과 끝을 내기
		Vector3 start = path.back();
		Vector3 end = path.front();

		//다시 나온 경로에서 장애물 계산을 또 계산
		aStar->MakeDirectionPath(start, end, tempPath);

		//계산 결과 피드백
		path.clear();
		path = tempPath;

		//경로 벡터에 새로운 시작과 끝을 포함
		path.insert(path.begin(), end);
		path.push_back(start);

		// 계산을 다시 했는데 벡터 크기가 그대로라면 = 길이 바뀌지 않았다
		if (pathSize == path.size()) break; // 이 이상 계산을 계속할 이유가 없다
		else pathSize = path.size(); // 줄어든 경로 계산을 반영을 해주고 반복문을 다시 진행
	}

	// 다시 조정된, 내가 갈 수 있는 경로에, 최종 목적지를 다시 한번 추가한다
	path.insert(path.begin(), targetPos);

}

void Boss::ExecuteEvent()
{
	int index = curState; //현재 상태 받아오기
	if (totalEvent[index].empty()) return;
	if (eventIters[index] == totalEvent[index].end()) return;

	float ratio = motion->runningTime / motion->duration; //진행된 시간 나누기 전체 진행시간

	if (eventIters[index]->first > ratio) return; // 진행 시간이 정해진 기준에 못 미치면 종료(재시작)

	eventIters[index]->second(); //등록된 이벤트 수행
	eventIters[index]++;
}

void Boss::SetEvent(int clip, Event event, float timeRatio)
{
	if (totalEvent[clip].count(timeRatio) > 0) return; // 선행 예약된 이벤트가 있으면 종료    
	totalEvent[clip][timeRatio] = event;
}

void Boss::Detection()
{
	if (bDetection) {
		DetectionStartTime += DELTA;
	}
	else {
		DetectionStartTime -= DELTA;
		if (DetectionStartTime <= 0.f)
			DetectionStartTime = 0.f;
	}

	if (DetectionEndTime <= DetectionStartTime) {
		if (bFind)return;
		bFind = true;
		state = BOSS_STATE::DETECT;
		exclamationMark->SetActive(true);
		bWait = false;
		SetPath(target->GlobalPos());
	
		SetState(RUN);
	}
	rangeBar->SetAmount(DetectionStartTime / DetectionEndTime);
	if (DetectionEndTime*2.f <= DetectionStartTime) {
		exclamationMark->SetActive(false);
	}


}

void Boss::EndAttack()
{
	velocity = target->GlobalPos() - transform->GlobalPos();
	totargetlength = velocity.Length();
	moveSpeed = runSpeed;
	dir = velocity.GetNormalized();
	if (totargetlength > AttackRange) {
		SetState(RUN);
		bWait = false;
	
	}
	else
	{
		eventIters[ATTACK] = totalEvent[ATTACK].begin(); //이벤트 반복자도 등록된 이벤트 시작시점으로

	
	}

	
}

void Boss::EndHit()
{
}

void Boss::EndDying()
{
}

void Boss::EndJumpAttack()
{
	velocity = target->GlobalPos() - transform->GlobalPos();
	totargetlength = velocity.Length();
	bWait = false;
	if (totargetlength > AttackRange) {
		if (aStar->IsCollisionObstacle(transform->GlobalPos(), target->GlobalPos()))// 중간에 장애물이 있으면
		{
			SetPath(target->GlobalPos()); // 구체적인 경로 내어서 가기
		}
		else
		{
			path.clear(); // 굳이 장애물없는데 길찾기 필요 x
			path.push_back(target->GlobalPos()); // 가야할 곳만 경로에 집어넣기
		}
	}
	else
	{
		UINT random = rand() % 2;
		path.clear();
		if (!random)
			SetState(ATTACK, 3.0f);
	

	}
	
}

void Boss::StartAttack()
{
	bWait = true;
}

void Boss::StartJumpAttack()
{
	bWait = true;
}

bool Boss::IsPatrolPos()
{
	if((PatrolPos[curPatrol].x-1.f<transform->GlobalPos().x&& PatrolPos[curPatrol].x + 1.f > transform->GlobalPos().x)
		&& (PatrolPos[curPatrol].z - 1.f < transform->GlobalPos().z && PatrolPos[curPatrol].z + 1.f > transform->GlobalPos().z))
		return true;
	return false;
}

void Boss::IdleWalk()
{
	if (state != BOSS_STATE::IDLE)return;
	if (IsPatrolPos())
	{
		if (!bWait)
		{
			bWait = true;
			SetState(IDLE);
		}
	}
	velocity = PatrolPos[curPatrol] - transform->GlobalPos();
	totargetlength = velocity.Length();
	moveSpeed = walkSpeed;
	dir = velocity.GetNormalized();
}
void Boss::Run()
{
	velocity = target->GlobalPos() - transform->GlobalPos();
	totargetlength = velocity.Length();
	moveSpeed = runSpeed;
	dir = velocity.GetNormalized();
	if (totargetlength > AttackRange) {


		if (aStar->IsCollisionObstacle(transform->GlobalPos(), target->GlobalPos()))// 중간에 장애물이 있으면
		{
			SetPath(target->GlobalPos()); // 구체적인 경로 내어서 가기
		}
		else
		{
			path.clear(); // 굳이 장애물없는데 길찾기 필요 x
			path.push_back(target->GlobalPos()); // 가야할 곳만 경로에 집어넣기
		}
	}

	else
	{
			
		path.clear();
	
		SetState(ATTACK, 3.0f);
	
		bWait = true;
	}
}

