#include "Framework.h"

Boss::Boss()
	
{
	instancing = new ModelAnimatorInstancing("Warrok W Kurniawan");
	transform = instancing->Add();
	instancing->AddModelInfo(transform, 0);

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
	
	Mouth = new Transform;
	RoarCollider = new CapsuleCollider();
	RoarCollider->Pos().z -= 140.f;
	RoarCollider->Rot().x = XMConvertToRadians(90);
	RoarCollider->Scale() = { 95.f,60.f,93.f };
	RoarCollider->SetParent(Mouth);
	RoarCollider->SetActive(false);
	Roarparticle = new ParticleSystem("TextData/Particles/Roar.fx");
	
	Roarparticle->GetQuad()->SetParent(Mouth);
	Roarparticle->GetQuad()->Pos() = { 4.f,-9.f,-6.f };
	Roarparticle->GetQuad()->Scale() = { 25.f,36.f,42.f };
	for (int i = 0; i < 3; ++i) {
		Runparticle[i] = new ParticleSystem("TextData/Particles/Smoke.fx");
		
	}
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
	SetEvent(ATTACK, bind(&Boss::EndAttack, this), 0.9f);
	SetEvent(ATTACK, bind(&Collider::SetActive, leftCollider, true), 0.5f); //콜라이더 켜는 시점 설정
	SetEvent(ATTACK, bind(&Collider::SetActive, leftCollider, false), 0.98f); //콜라이더 꺼지는 시점 설정
	SetEvent(ROAR, bind(&Collider::SetActive, RoarCollider, true), 0.31f); //콜라이더 켜는 시점 설정
	SetEvent(ROAR, bind(&Collider::SetActive, RoarCollider, false), 0.9f); //콜라이더 꺼지는 시점 설정

	SetEvent(ROAR, bind(&Boss::Roar, this), 0.3f);
	SetEvent(ROAR, bind(&Boss::EndRoar, this), 0.91f);
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
	Audio::Get()->Add("Boss_Roar", "Sounds/Roar.mp3",false,false,true);
	Audio::Get()->Add("Boss_Splash", "Sounds/BossSplash.mp3", false, false, true);
	Audio::Get()->Add("Boss_Run", "Sounds/Bossfootstep.mp3", false, false, true);
	Audio::Get()->Add("Boss_Walk", "Sounds/Bosswalk.mp3", false, false, true);
	hiteffect = new Sprite(L"Textures/Effect/HitEffect.png", 25, 25, 5, 2, false);
	leftCollider->SetActive(false);
	FOR(2) blendState[i] = new BlendState();
	FOR(2) depthState[i] = new DepthStencilState();
	rangeBar->SetAlpha(0.5f);
	blendState[1]->Additive(); //투명색 적용 + 배경색 처리가 있으면 역시 적용
	depthState[1]->DepthWriteMask(D3D11_DEPTH_WRITE_MASK_ALL);  // 다 가리기
}

Boss::~Boss()
{
	delete hiteffect;
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
	delete Mouth;
	delete RoarCollider;
	delete Roarparticle;
	for (int i = 0; i < 3; ++i)
		delete Runparticle[i];

	FOR(2) {
		delete blendState[i] ;
		delete depthState[i] ;
	}
}

void Boss::Render()
{


	instancing->Render();
	hpBar->Render();
	
	leftCollider->Render();
	
	RoarCollider->Render();
	hiteffect->Render();
	collider->Render();
	Roarparticle->Render();
	for (int i = 0; i < 3; ++i)
		Runparticle[i]->Render();
	blendState[1]->SetState();
	if (!bFind)
		rangeBar->Render();
	blendState[0]->SetState();
}

void Boss::Update()
{

	instancing->Update();
	if (curHP <= 0)return;
	Idle();
	Direction();
	Control();
	Find();
	CollisionCheck();
	CoolTimeCheck();
	MarkTimeCheck();
	Move();
	Rotate();
	
	Die();

	leftHand->SetWorld(instancing->GetTransformByNode(index, 14));

	transform->UpdateWorld();

	SetPosY();
	leftCollider->UpdateWorld();
	collider->UpdateWorld();
	hpBar->Pos() = transform->Pos() + Vector3(0, 6, 0);
	Vector3 dir = hpBar->Pos() - CAM->Pos();
	hpBar->Rot().y = atan2(dir.x, dir.z);


	ExecuteEvent();

	Mouth->SetWorld(instancing->GetTransformByNode(index, 9));	
	RoarCollider->UpdateWorld();
	Roarparticle->Update();
	hpBar->UpdateWorld();
	rangeBar->UpdateWorld();
	hiteffect->Update();
	for (int i = 0; i < 3; ++i)
		Runparticle[i]->Update();


	UpdateUI();

	CoolDown();
}

void Boss::PostRender()
{
	if (!transform->Active())return;
	questionMark->Render();
	exclamationMark->Render();
	
}

void Boss::GUIRender() {
	ImGui::DragInt("count",(int*)&count,1,0,15);

	Roarparticle->GUIRender();
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
	while (rightdir1 > 360.0f)
		rightdir1 -= 360.0f;
	while (leftdir1 > 360.0f)
		leftdir1 -= 360.0f;

	if (Distance(target->GlobalPos(), transform->GlobalPos()) < eyeSightRange) {
		SetRay();
		if (leftdir1 > 270 && rightdir1 < 90) {
			if (!((leftdir1 <= Enemytothisangle && rightdir1 + 360 >= Enemytothisangle) || (leftdir1 <= Enemytothisangle + 360 && rightdir1 >= Enemytothisangle)))
			{
				bDetection = false;
				return;
			}
		}
		else {
			if (!(leftdir1 <= Enemytothisangle && rightdir1 >= Enemytothisangle))
			{
				bDetection = false;
				

				return;
			}
		
		}

		bDetection = ColliderManager::Get()->CompareDistanceObstacleandPlayer(ray);
	}
	else
		bDetection = false;
}

bool Boss::CalculateEyeSight(bool _bFind)
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

	if (Distance(target->GlobalPos(), transform->GlobalPos())	 < eyeSightRange) {


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
	return bDetection;
}

void Boss::CalculateEarSight()
{
}

void Boss::MarkTimeCheck()
{
	if (exclamationMark->Active())
	{
		MarkActiveTime += DELTA;
		if (MarkActiveTime >= 2.f)
		{
			exclamationMark->SetActive(false);
			MarkActiveTime = 0.f;
		}
	}
}

void Boss::CoolTimeCheck()
{
	if (bRoar) {
		RoarCoolTime += DELTA;
	}
	if (RoarCoolTime >= 10.f) {
		bRoar = false;
		RoarCoolTime = 0.f;
	}
}

void Boss::AddObstacleObj(Collider* collider)
{
}

void Boss::Idle()
{
	//if (curState != STATE::IDLE)return;
	if (state != BOSS_STATE::IDLE)return;
	Detection();
	CalculateEyeSight();



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



	transform->Pos() += dir * moveSpeed * DELTA;
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
	Audio::Get()->Play("Boss_Roar",transform->GlobalPos());
//	RoarCollider->SetActive(true);
	Roarparticle->Play();
	IsHit = false;
}



void Boss::JumpAttack()
{

}

void Boss::Die()
{
}

void Boss::Find()
{
	/*
	if (state == BOSS_STATE::DETECT) {
		if (dynamic_cast<Player*>(target)->GetTest()) {
			state = BOSS_STATE::FIND;
			questionMark->SetActive(true);
			exclamationMark->SetActive(false);
			FindPos = aStar->FindPos(transform->GlobalPos(), 30.f);
			
		}
	}
	if (state == BOSS_STATE::FIND) {
		if (!dynamic_cast<Player*>(target)->GetTest()) {
			if (!CalculateEyeSight(true))
				return;

			state = BOSS_STATE::DETECT;
			questionMark->SetActive(false);
			exclamationMark->SetActive(true);
		
		}
	}
	*/
}

void Boss::Rotate()
{
	if (path.empty()) {
		transform->Rot().y = atan2(dir.x, dir.z) + XM_PI; // XY좌표 방향 + 전후반전(문워크 방지)


	}
	else {
		Vector3 dest = path.back();

		Vector3 direction = dest - transform->GlobalPos();

		direction.y = 0; // 필요하면 지형의 높이(굴곡) 반영을 해줄 수도 있고,
		// 역시 필요하면 그냥 좌우회전만 하는 걸로 (y 방향 일부러 주지 않음)

		if (direction.Length() < 0.5f)
		{
			path.pop_back();
		}
		dir = direction.GetNormalized();
		transform->Rot().y = atan2(dir.x, dir.z) + XM_PI; // XY좌표 방향 + 전후반전(문워크 방지)
	}
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

	
}

void Boss::Control()
{
	if (state != BOSS_STATE::DETECT)return;

	if (searchCoolDown > 1)
	{
		target->GlobalPos();
		// 터레인 에디터 쓰는 사람은 컴퓨트픽킹으로

		if (aStar->IsCollisionObstacle(transform->GlobalPos(), target->GlobalPos())) // 중간에 장애물이 있으면
		{
			SetPath(target->GlobalPos()); // 구체적인 경로 내어서 가기
		}
		else //장애물이 없는 경우
		{
			path.clear(); //3D에서 장애물도 없는데 굳이 길찾기를 쓸 필요 없음
			path.push_back(target->GlobalPos()); // 가야 할 곳만 경로 벡터에 집어넣기
			// -> 그러면 여우는 Move()로 목적지를 찾아갈 것
		}

		searchCoolDown -= 1;
	}
	else
		searchCoolDown += DELTA;
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

	questionMark->Pos() = CAM->WorldToScreen(barPos + Vector3(0, 1, 0));
	questionMark->UpdateWorld();
}



void Boss::SetState(STATE state, float scale, float takeTime)
{
	if (state == curState)
	{
	
		return; // 이미 그 상태라면 굳이 변환 필요 없음

	}


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
		if (aStar->IsCollisionObstacle(transform->GlobalPos(), target->GlobalPos())) // 중간에 장애물이 있으면
		{
			SetPath(target->GlobalPos()); // 구체적인 경로 내어서 가기
		}
		else //장애물이 없는 경우
		{
			path.clear(); //3D에서 장애물도 없는데 굳이 길찾기를 쓸 필요 없음
			path.push_back(target->GlobalPos()); // 가야 할 곳만 경로 벡터에 집어넣기
			// -> 그러면 여우는 Move()로 목적지를 찾아갈 것
		}

	
		SetState(RUN);
	}
	rangeBar->SetAmount(DetectionStartTime / DetectionEndTime);
	


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
		if (aStar->IsCollisionObstacle(transform->GlobalPos(), target->GlobalPos())) // 중간에 장애물이 있으면
		{
			SetPath(target->GlobalPos()); // 구체적인 경로 내어서 가기
		}
		else //장애물이 없는 경우
		{
			path.clear(); //3D에서 장애물도 없는데 굳이 길찾기를 쓸 필요 없음
			path.push_back(target->GlobalPos()); // 가야 할 곳만 경로 벡터에 집어넣기
			// -> 그러면 여우는 Move()로 목적지를 찾아갈 것
		}

	}
	else
	{
		SetState(RUN);
		Audio::Get()->Play("Boss_Splash", transform->GlobalPos(), 1.f);
	
	}

	
}

void Boss::EndHit()
{
}

void Boss::EndDying()
{
	exclamationMark->SetActive(false);
	questionMark->SetActive(false);
}



void Boss::StartAttack()
{
	bWait = true;
	IsHit = false;
}


void Boss::EndRoar()
{
	Roarparticle->Stop();
	bRoar = true;
	velocity = target->GlobalPos() - transform->GlobalPos();
	totargetlength = velocity.Length();
	moveSpeed = runSpeed;
	dir = velocity.GetNormalized();
	if (totargetlength > AttackRange) {
		SetState(RUN);
		bWait = false;
		if (aStar->IsCollisionObstacle(transform->GlobalPos(), target->GlobalPos())) // 중간에 장애물이 있으면
		{
			SetPath(target->GlobalPos()); // 구체적인 경로 내어서 가기
		}
		else //장애물이 없는 경우
		{
			path.clear(); //3D에서 장애물도 없는데 굳이 길찾기를 쓸 필요 없음
			path.push_back(target->GlobalPos()); // 가야 할 곳만 경로 벡터에 집어넣기
			// -> 그러면 여우는 Move()로 목적지를 찾아갈 것
		}

	}
	else
	{
		path.clear();
		SetState(ATTACK, 3.0f);
		bWait = true;
		Audio::Get()->Play("Boss_Splash", transform->GlobalPos(), 1.f);

	}
	
}

bool Boss::IsPatrolPos()
{
	if((PatrolPos[curPatrol].x-1.f<transform->GlobalPos().x&& PatrolPos[curPatrol].x + 1.f > transform->GlobalPos().x)
		&& (PatrolPos[curPatrol].z - 1.f < transform->GlobalPos().z && PatrolPos[curPatrol].z + 1.f > transform->GlobalPos().z))
		return true;
	return false;
}

bool Boss::IsFindPos()
{
	if ((FindPos.x - 1.f < transform->GlobalPos().x && FindPos.x + 1.f > transform->GlobalPos().x)
		&& (FindPos.z - 1.f < transform->GlobalPos().z && FindPos.z + 1.f > transform->GlobalPos().z))
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
	Audio::Get()->Play("Boss_Walk",transform->GlobalPos(),0.3f);
}
bool Boss::OnColliderFloor(Vector3& feedback)
{
	Vector3 SkyPos = transform->Pos();
	SkyPos.y += 3;
	Ray groundRay = Ray(SkyPos, Vector3(transform->Down()));
	Contact con;
	if (ColliderManager::Get()->CloseRayCollisionColliderContact(groundRay, con))
	{
		feedback = con.hitPoint;
		//feedback.y += 0.1f; //살짝 띄움으로서 충돌 방지
		return true;
	}

	return false;
}
void Boss::Run()
{
	if (state == BOSS_STATE::DETECT) {

		velocity = target->GlobalPos() - transform->GlobalPos();
		totargetlength = velocity.Length();
		moveSpeed = runSpeed;

		if (totargetlength > AttackRange) {
			if (totargetlength < RoarRange) {
				if (RoarCoolTime <= 0.f)
				{
					path.clear();
					SetState(ROAR);
					bWait = true;
					return;
				}
			}

		
			Runparticle[currunparticle]->Play(transform->GlobalPos(), transform->Rot());	
			if(!Audio::Get()->IsPlaySound("Boss_Run"))
				Audio::Get()->Play("Boss_Run", transform->GlobalPos(), 1.f);
			currunparticle++;
			if (currunparticle >= 3)
				currunparticle = 0;
		
		
		}

		else
		{
			SetRay();

			path.clear();

			SetState(ATTACK, 3.0f);
			Audio::Get()->Play("Boss_Splash", transform->GlobalPos(), 1.f);
			bWait = true;
		}
	}
	else if(state==BOSS_STATE::FIND) {
		if (IsFindPos()) {
			FindPos = aStar->FindPos(transform->GlobalPos(), 30.f);

		}

		velocity = FindPos - transform->GlobalPos();
		totargetlength = velocity.Length();
		moveSpeed = runSpeed;
		dir = velocity.GetNormalized();
	

	}
}

void Boss::SetRay()
{
	ray.pos = transform->GlobalPos();
	ray.pos.y += 10;
	Vector3 vtarget = target->GlobalPos();
	vtarget.y += 10;
	Vector3 vtrasnform = transform->GlobalPos();
	vtrasnform.y += 10;
	Vector3 dir = vtarget - vtrasnform;
	dir.Normalize();
	ray.dir = dir;
}

void Boss::SetPosY()
{
	if (!OnColliderFloor(feedBackPos)) // 문턱올라가기 때문
	{
		if (curRayCoolTime <= 0.f)
		{
			Vector3 OrcSkyPos = transform->Pos();
			OrcSkyPos.y += 100;
			Ray groundRay = Ray(OrcSkyPos, Vector3(transform->Down()));
			TerrainComputePicking(feedBackPos, groundRay);
		}
	}	

	transform->Pos().y = feedBackPos.y;
}

void Boss::CollisionCheck()
{
	if (!leftCollider->Active() && !RoarCollider->Active())return;
	if (IsHit)return;
	Player* player = dynamic_cast<Player*>(target);
	if (!player)return;
	if (leftCollider->Active())
	{
		leftCollider->ResetCollisionPoint();
		if (leftCollider->IsCollision(player->GetCollider()))
		{
			Vector3 pos = leftCollider->GetCollisionPoint();
			player->SetHitEffectPos(pos);
			player->Hit(attackdamage);
			IsHit = true;
		}

	}
	if (RoarCollider->Active()) {
		if (RoarCollider->IsCollision(player->GetCollider()))
		{
			player->Hit(Roardamage, true);
			IsHit = true;
		}
	}
		
}

bool Boss::TerrainComputePicking(Vector3& feedback, Ray ray)
{
	if (terrain && structuredBuffer)
	{
		UINT w = terrain->GetSizeWidth();

		float minx = floor(ray.pos.x);
		float maxx = ceil(ray.pos.x);

		if (maxx == w - 1 && minx == maxx)
			minx--;
		else if (minx == maxx)
			maxx++;

		float minz = floor(ray.pos.z);
		float maxz = ceil(ray.pos.z);

		if (maxz == w - 1 && minz == maxz)
			minz--;
		else if (minz == maxz)
			maxz++;

		for (UINT z = w - maxz - 1; z < w - minz - 1; z++)
		{
			for (UINT x = minx; x < maxx; x++)
			{
				UINT index[4];
				index[0] = w * z + x;
				index[1] = w * z + x + 1;
				index[2] = w * (z + 1) + x;
				index[3] = w * (z + 1) + x + 1;

				vector<VertexType>& vertices = terrain->GetMesh()->GetVertices();

				Vector3 p[4];
				for (UINT i = 0; i < 4; i++)
					p[i] = vertices[index[i]].pos;

				float distance = 0.0f;
				if (Intersects(ray.pos, ray.dir, p[0], p[1], p[2], distance))
				{
					feedback = ray.pos + ray.dir * distance;
					return true;
				}

				if (Intersects(ray.pos, ray.dir, p[3], p[1], p[2], distance))
				{
					feedback = ray.pos + ray.dir * distance;
					return true;
				}
			}
		}
	}
	return false;
}

