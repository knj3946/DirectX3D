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
	
	totalEvent.resize(instancing->GetClipSize()); //���� ���� ���� ���ڸ�ŭ �̺�Ʈ ������¡
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
	SetEvent(ATTACK, bind(&Boss::DoingAttack, this), 0.6f);

	SetEvent(ROAR, bind(&Boss::Roar, this), 0.3f);
	SetEvent(ROAR, bind(&Boss::EndRoar, this), 0.9f);
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
	hiteffect = new Sprite(L"Textures/Effect/HitEffect.png", 50, 50, 5, 2, false);
	leftCollider->SetActive(false);
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
}
void Boss::DoingAttack() {
	//if(leftCollider->IsCollision(target->))
	//Ÿ�� ����
	leftCollider->SetActive(true);
}
void Boss::Render()
{
	instancing->Render();
	hpBar->Render();
	if(!bFind)
		rangeBar->Render();
	leftCollider->Render();
	
	RoarCollider->Render();
	collider->Render();
	Roarparticle->Render();
	for (int i = 0; i < 3; ++i)
		Runparticle[i]->Render();

	hiteffect->Render();

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

	for (int i = 0; i < 3; ++i)
		Runparticle[i]->Update();


	UpdateUI();
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
	//�ʿ��ִ� ��ũ�� �ٺθ��� ���Ÿ� ����
	Audio::Get()->Play("Boss_Roar",transform->GlobalPos());
	RoarCollider->SetActive(true);
	Roarparticle->Play();
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
		transform->Rot().y = atan2(dir.x, dir.z) + XM_PI; // XY��ǥ ���� + ���Ĺ���(����ũ ����)


	}
	else {
		Vector3 dest = path.back();

		Vector3 direction = dest - transform->GlobalPos();

		direction.y = 0; // �ʿ��ϸ� ������ ����(����) �ݿ��� ���� ���� �ְ�,
		// ���� �ʿ��ϸ� �׳� �¿�ȸ���� �ϴ� �ɷ� (y ���� �Ϻη� ���� ����)

		if (direction.Length() < 0.5f)
		{
			path.pop_back();
		}
		dir = direction.GetNormalized();
		transform->Rot().y = atan2(dir.x, dir.z) + XM_PI; // XY��ǥ ���� + ���Ĺ���(����ũ ����)
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
		// �ͷ��� ������ ���� ����� ��ǻƮ��ŷ����

		if (aStar->IsCollisionObstacle(transform->GlobalPos(), target->GlobalPos())) // �߰��� ��ֹ��� ������
		{
			SetPath(target->GlobalPos()); // ��ü���� ��� ��� ����
		}
		else //��ֹ��� ���� ���
		{
			path.clear(); //3D���� ��ֹ��� ���µ� ���� ��ã�⸦ �� �ʿ� ����
			path.push_back(target->GlobalPos()); // ���� �� ���� ��� ���Ϳ� ����ֱ�
			// -> �׷��� ����� Move()�� �������� ã�ư� ��
		}

		searchCoolDown -= 1;
	}
	else
		searchCoolDown += DELTA;
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

	questionMark->Pos() = CAM->WorldToScreen(barPos + Vector3(0, 1, 0));
	questionMark->UpdateWorld();
}



void Boss::SetState(STATE state, float scale, float takeTime)
{
	if (state == curState)
	{
	
		return; // �̹� �� ���¶�� ���� ��ȯ �ʿ� ����

	}


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
		if (aStar->IsCollisionObstacle(transform->GlobalPos(), target->GlobalPos())) // �߰��� ��ֹ��� ������
		{
			SetPath(target->GlobalPos()); // ��ü���� ��� ��� ����
		}
		else //��ֹ��� ���� ���
		{
			path.clear(); //3D���� ��ֹ��� ���µ� ���� ��ã�⸦ �� �ʿ� ����
			path.push_back(target->GlobalPos()); // ���� �� ���� ��� ���Ϳ� ����ֱ�
			// -> �׷��� ����� Move()�� �������� ã�ư� ��
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
	leftCollider->SetActive(false);
	if (totargetlength > AttackRange) {
		SetState(RUN);
		bWait = false;
		if (aStar->IsCollisionObstacle(transform->GlobalPos(), target->GlobalPos())) // �߰��� ��ֹ��� ������
		{
			SetPath(target->GlobalPos()); // ��ü���� ��� ��� ����
		}
		else //��ֹ��� ���� ���
		{
			path.clear(); //3D���� ��ֹ��� ���µ� ���� ��ã�⸦ �� �ʿ� ����
			path.push_back(target->GlobalPos()); // ���� �� ���� ��� ���Ϳ� ����ֱ�
			// -> �׷��� ����� Move()�� �������� ã�ư� ��
		}

	}
	else
	{
		eventIters[ATTACK] = totalEvent[ATTACK].begin(); //�̺�Ʈ �ݺ��ڵ� ��ϵ� �̺�Ʈ ���۽�������
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

}


void Boss::EndRoar()
{
	RoarCollider->SetActive(false);
	Roarparticle->Stop();
	bRoar = true;
	velocity = target->GlobalPos() - transform->GlobalPos();
	totargetlength = velocity.Length();
	moveSpeed = runSpeed;
	dir = velocity.GetNormalized();
	if (totargetlength > AttackRange) {
		SetState(RUN);
		bWait = false;
		if (aStar->IsCollisionObstacle(transform->GlobalPos(), target->GlobalPos())) // �߰��� ��ֹ��� ������
		{
			SetPath(target->GlobalPos()); // ��ü���� ��� ��� ����
		}
		else //��ֹ��� ���� ���
		{
			path.clear(); //3D���� ��ֹ��� ���µ� ���� ��ã�⸦ �� �ʿ� ����
			path.push_back(target->GlobalPos()); // ���� �� ���� ��� ���Ϳ� ����ֱ�
			// -> �׷��� ����� Move()�� �������� ã�ư� ��
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
	Vector3 dir = target->GlobalPos() - transform->GlobalPos();
	dir.Normalize();
	ray.dir = dir;
}

void Boss::CollisionCheck()
{
	if (!leftCollider->Active() || !RoarCollider->Active())return;
	if (IsHit)return;
	Player* player = dynamic_cast<Player*>(target);
	if (!player)return;
	if (leftCollider->Active())
	{
	
		if (leftCollider->IsCollision(player->GetCollider()))
			player->Hit(attackdamage);
		

	}
	if (RoarCollider->Active()) {
		if (RoarCollider->IsCollision(player->GetCollider()))
			player->Hit(Roardamage,true);
	}
		IsHit = true;
}

