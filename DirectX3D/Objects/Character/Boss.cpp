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
	
	totalEvent.resize(instancing->GetClipSize()); //���� ���� ���� ���ڸ�ŭ �̺�Ʈ ������¡
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
	//Ÿ�� ����
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
	
			// ���ڿ� ������ �����۾� �ϱ�..
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
	//�ʿ��ִ� ��ũ�� �ٺθ��� ���Ÿ� ����
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

	// �����϶� ����ǥ ��ũ �׸���
	Vector3 barPos = transform->Pos() + Vector3(0, 6, 0);
	/*
		
		 questionMark->Pos() = CAM->WorldToScreen(barPos + Vector3(0, 1, 0));
	*/
	
	exclamationMark->Pos() = CAM->WorldToScreen(barPos + Vector3(0, 1, 0));
	exclamationMark->UpdateWorld();
}



void Boss::SetState(STATE state, float scale, float takeTime)
{
	if (state == curState) return; // �̹� �� ���¶�� ���� ��ȯ �ʿ� ����


	curState = state;
	instancing->PlayClip(index, (int)state); //�ν��Ͻ� �� �ڱ� Ʈ���������� ���� ���� ����
	eventIters[state] = totalEvent[state].begin(); //�̺�Ʈ �ݺ��ڵ� ��ϵ� �̺�Ʈ ���۽�������


}

void Boss::SetPath(Vector3 targetPos)
{
	int startIndex = aStar->FindCloseNode(transform->GlobalPos());
	int endIndex = aStar->FindCloseNode(targetPos); // �������(+������Ʈ����) ������ ������

	aStar->GetPath(startIndex, endIndex, path); // ���� �� ���� path ���Ϳ� ����

	aStar->MakeDirectionPath(transform->GlobalPos(), targetPos, path); // ��ֹ��� ����� path�� ������

	UINT pathSize = path.size(); // ó�� ���� ��� ���� ũ�⸦ ����

	while (path.size() > 2) // "������" ��� ��尡 1���� ���ϰ� �� ������
	{
		vector<Vector3> tempPath = path; // ���� �ӽ� ��� �޾ƿ���
		tempPath.erase(tempPath.begin()); // ���� ������ ����� (��ֹ��� �־��� ������ ������ �߰��� ���� ��)
		tempPath.pop_back(); // ���� ��ġ�� �������� �����ϱ� (�̹� �������� �����ϱ�)

		// ������ ������ ���� ����� ���ο� ���۰� ���� ����
		Vector3 start = path.back();
		Vector3 end = path.front();

		//�ٽ� ���� ��ο��� ��ֹ� ����� �� ���
		aStar->MakeDirectionPath(start, end, tempPath);

		//��� ��� �ǵ��
		path.clear();
		path = tempPath;

		//��� ���Ϳ� ���ο� ���۰� ���� ����
		path.insert(path.begin(), end);
		path.push_back(start);

		// ����� �ٽ� �ߴµ� ���� ũ�Ⱑ �״�ζ�� = ���� �ٲ��� �ʾҴ�
		if (pathSize == path.size()) break; // �� �̻� ����� ����� ������ ����
		else pathSize = path.size(); // �پ�� ��� ����� �ݿ��� ���ְ� �ݺ����� �ٽ� ����
	}

	// �ٽ� ������, ���� �� �� �ִ� ��ο�, ���� �������� �ٽ� �ѹ� �߰��Ѵ�
	path.insert(path.begin(), targetPos);

}

void Boss::ExecuteEvent()
{
	int index = curState; //���� ���� �޾ƿ���
	if (totalEvent[index].empty()) return;
	if (eventIters[index] == totalEvent[index].end()) return;

	float ratio = motion->runningTime / motion->duration; //����� �ð� ������ ��ü ����ð�

	if (eventIters[index]->first > ratio) return; // ���� �ð��� ������ ���ؿ� �� ��ġ�� ����(�����)

	eventIters[index]->second(); //��ϵ� �̺�Ʈ ����
	eventIters[index]++;
}

void Boss::SetEvent(int clip, Event event, float timeRatio)
{
	if (totalEvent[clip].count(timeRatio) > 0) return; // ���� ����� �̺�Ʈ�� ������ ����    
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
		eventIters[ATTACK] = totalEvent[ATTACK].begin(); //�̺�Ʈ �ݺ��ڵ� ��ϵ� �̺�Ʈ ���۽�������

	
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
		if (aStar->IsCollisionObstacle(transform->GlobalPos(), target->GlobalPos()))// �߰��� ��ֹ��� ������
		{
			SetPath(target->GlobalPos()); // ��ü���� ��� ��� ����
		}
		else
		{
			path.clear(); // ���� ��ֹ����µ� ��ã�� �ʿ� x
			path.push_back(target->GlobalPos()); // ������ ���� ��ο� ����ֱ�
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


		if (aStar->IsCollisionObstacle(transform->GlobalPos(), target->GlobalPos()))// �߰��� ��ֹ��� ������
		{
			SetPath(target->GlobalPos()); // ��ü���� ��� ��� ����
		}
		else
		{
			path.clear(); // ���� ��ֹ����µ� ��ã�� �ʿ� x
			path.push_back(target->GlobalPos()); // ������ ���� ��ο� ����ֱ�
		}
	}

	else
	{
			
		path.clear();
	
		SetState(ATTACK, 3.0f);
	
		bWait = true;
	}
}

