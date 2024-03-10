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
	SetEvent(ATTACK, bind(&Collider::SetActive, leftCollider, true), 0.5f); //�ݶ��̴� �Ѵ� ���� ����
	SetEvent(ATTACK, bind(&Collider::SetActive, leftCollider, false), 0.98f); //�ݶ��̴� ������ ���� ����
	SetEvent(ROAR, bind(&Collider::SetActive, RoarCollider, true), 0.31f); //�ݶ��̴� �Ѵ� ���� ����
	SetEvent(ROAR, bind(&Collider::SetActive, RoarCollider, false), 0.9f); //�ݶ��̴� ������ ���� ����

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
	blendState[1]->Additive(); //����� ���� + ���� ó���� ������ ���� ����
	depthState[1]->DepthWriteMask(D3D11_DEPTH_WRITE_MASK_ALL);  // �� ������
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
bool Boss::OnColliderFloor(Vector3& feedback)
{
	Vector3 SkyPos = transform->Pos();
	SkyPos.y += 3;
	Ray groundRay = Ray(SkyPos, Vector3(transform->Down()));
	Contact con;
	if (ColliderManager::Get()->CloseRayCollisionColliderContact(groundRay, con))
	{
		feedback = con.hitPoint;
		//feedback.y += 0.1f; //��¦ ������μ� �浹 ����
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
	if (!OnColliderFloor(feedBackPos)) // ���οö󰡱� ����
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

