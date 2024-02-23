#include "Framework.h"

Orc::Orc(Transform* transform, ModelAnimatorInstancing* instancing, UINT index)
    :transform(transform), instancing(instancing), index(index)
{
    //Ŭ�� �����صα� 
    string modelName = "Orc";

    //root = new Transform();

 
    motion = instancing->GetMotion(index);
    totalEvent.resize(instancing->GetClipSize()); //���� ���� ���� ���ڸ�ŭ �̺�Ʈ ������¡
    eventIters.resize(instancing->GetClipSize());

    //�̺�Ʈ ����
    SetEvent(HIT, bind(&Orc::EndHit, this), 0.99f);
    SetEvent(DYING, bind(&Orc::EndDying, this), 0.9f);

    FOR(totalEvent.size())
    {
        eventIters[i] = totalEvent[i].begin(); // ��ϵǾ� ���� �̺�Ʈ�� ������������ �ݺ��� ����
    }

    //�浹ü
    collider = new CapsuleCollider(30, 120);
    collider->SetParent(transform);
    //collider->Rot().z = XM_PIDIV2 - 0.2f;
    collider->Pos() = { -15, 80, 0 };
    collider->SetActive(true);

    // ���� �浹ü
    leftHand = new Transform();
    leftWeaponCollider = new CapsuleCollider(8,50);
    leftWeaponCollider->Pos().y += 20;
    leftWeaponCollider->SetParent(leftHand); // �ӽ÷� ���� �浹ü�� "��" Ʈ�������� �ֱ�
    leftWeaponCollider->SetActive(true);

    rightHand = new Transform();
    rightWeaponCollider = new CapsuleCollider(6, 30);
    //rightWeaponCollider->Pos().y += 20;
    rightWeaponCollider->SetParent(rightHand); // �ӽ÷� ���� �浹ü�� "��" Ʈ�������� �ֱ�
    rightWeaponCollider->SetActive(true);

    // hp UI
    hpBar = new ProgressBar(L"Textures/UI/hp_bar.png", L"Textures/UI/hp_bar_BG.png");
    hpBar->Scale() *= 0.6f;
    hpBar->SetAmount(curHP / maxHp);

    exclamationMark = new Quad(L"Textures/UI/Exclamationmark.png");
    questionMark = new Quad(L"Textures/UI/QuestionMark.png");
    exclamationMark->Scale() *= 0.1f;
    questionMark->Scale() *= 0.1f;

    //aStar = new AStar(512, 512);
    
    //aStar->SetNode();

}

Orc::~Orc()
{
    delete leftHand;
    delete rightHand;
    delete leftWeaponCollider;
    delete rightWeaponCollider;
    delete collider;
    //delete terrain;
   // delete aStar;
    delete instancing;
    delete motion;
    //delete root;
    delete transform;
    delete exclamationMark;
    delete questionMark;
}

void Orc::SetType(NPC_TYPE _type) {

    switch (_type)
    {
    case Orc::NPC_TYPE::ATTACK:
        informrange = 100;
        type = NPC_TYPE::ATTACK;
        break;
    case Orc::NPC_TYPE::INFORM:
        informrange = 750;
        type = NPC_TYPE::INFORM;
        break;
    default:
        break;
    }
}

void Orc::Update()
{
    if (!transform->Active()) return;

    Direction();
    
    CalculateEyeSight();
    Detection();
    RangeCheck();
    ExecuteEvent();
    UpdateUI();
    if (!collider->Active())return;

    if (isHit)
    {
        //�´� ���̸� �ٸ����� �� �� ����
        wateTime = 0;
    }
    else
    {
        if (isTracking == false && path.empty())
            IdleAIMove();
        else
        {
            Control();

            switch (curState)
            {
            case ATTACK:
            {
                if (wateTime <= 0)
                {
                    Move();
                    wateTime = 1.0f; //ATTACK �ִϸ��̼��� ������ �̵��ؾ� �ϱ� ����
                }
                break;
            }
            default:
                Move();
            }
            wateTime -= DELTA;
        }
    }

    //root->SetWorld(instancing->GetTransformByNode(index, 3));
    transform->SetWorld(instancing->GetTransformByNode(index, 5));
    collider->UpdateWorld();
    transform->UpdateWorld();

    // TODO : ����� �ִϸ��̼� ���缭 �����̰�, �����ݶ��̴���
    leftHand->SetWorld(instancing->GetTransformByNode(index, 170));
    leftWeaponCollider->UpdateWorld();
    rightHand->SetWorld(instancing->GetTransformByNode(index, 187)); 
    rightWeaponCollider->UpdateWorld();
}

void Orc::Render()
{
    collider->Render();
    leftWeaponCollider->Render();
    rightWeaponCollider->Render();

    //aStar->Render();
}

void Orc::PostRender()
{
    if (!transform->Active())return;
    hpBar->Render();
    
    questionMark->Render();
    exclamationMark->Render();

}

void Orc::SetTerrain(LevelData* terrain)
{
    this->terrain = terrain;
   
}

void Orc::SetSRT(Vector3 scale, Vector3 rot, Vector3 pos)
{
    transform->Scale() = scale;
    transform->Rot() = rot;
    transform->Pos() = pos;

    transform->UpdateWorld();
}

void Orc::GUIRender()
{
    ImGui::Text("bFind : %d", bFind);
    ImGui::Text("bDetection : %d", bDetection);
    ImGui::Text("isTracking : %d", isTracking);

    /*
    if (!path.empty())
    {
        ImGui::Text("path.x : %f", path.back().x);
        ImGui::Text("path.y : %f", path.back().y);
        ImGui::Text("path.z : %f", path.back().z);

        ImGui::Text("start.x : %f", startPos.x);
        ImGui::Text("start.y : %f", startPos.y);
        ImGui::Text("start.z : %f", startPos.z);

        ImGui::Text("pathsize : %f", path.size());
    }
    */
}

void Orc::Direction()
{
    if (behaviorstate == NPC_BehaviorState::IDLE) {
        velocity = PatrolPos[nextPatrol] - transform->GlobalPos();
    }
    else {
        velocity = target->GlobalPos() - transform->GlobalPos();
    }
}

void Orc::Hit()
{
    if (!isHit)
    {
        destHP = (curHP - 30 > 0) ? curHP - 30 : 0; //������ ���� ���� 30

        collider->SetActive(false);
        leftWeaponCollider->SetActive(false);
        rightWeaponCollider->SetActive(false);
        if (destHP <= 0)
        {
            SetState(DYING);

            return;
        }

        SetState(HIT);

        isHit = true;
    }
    //// ���� �� �׾����� �� �κ���� �´� ���� ����
    //curState = HIT;
    //instancing->PlayClip(index, HIT);
    //eventIters[HIT] = totalEvent[HIT].begin();
}

void Orc::Spawn(Vector3 pos)
{
    transform->SetActive(true); //��Ȱ��ȭ���ٸ� Ȱ��ȭ ����
    collider->SetActive(true);

    SetState(IDLE); // ������ ��å

    curHP = maxHp;
    hpBar->SetAmount(curHP / maxHp);

    transform->Pos() = pos;
}

void Orc::AttackTarget()
{
    if (!bFind)
    {
        bFind = true;
        isTracking = true;
        //if (curState == IDLE)
            SetState(RUN);
    }
}

float Orc::GetCurAttackCoolTime()
{
    return curAttackCoolTime;
}

void Orc::SetAttackCoolDown()
{
    curAttackCoolTime = attackCoolTime; // ������Ÿ��
}

void Orc::FillAttackCoolTime()
{
    curAttackCoolTime -= DELTA;
    if (curAttackCoolTime < 0)
        curAttackCoolTime = 0;
}

void Orc::Findrange()
{
    if (curState == ATTACK)return;
    bFind = true; bDetection = true;
    DetectionStartTime = DetectionEndTime;
    isTracking = true;
    SetState(RUN);
    if (aStar->IsCollisionObstacle(transform->GlobalPos(), target->GlobalPos()))// �߰��� ��ֹ��� ������
    {
        SetPath(target->GlobalPos()); // ��ü���� ��� ��� ����
    }
    else
    {
        path.clear(); // ���� ��ֹ����µ� ��ã�� �ʿ� x
        path.push_back(target->GlobalPos()); // ������ ���� ��ο� ����ֱ�
    }
    behaviorstate = NPC_BehaviorState::DETECT;
}
void Orc::Control()
{
    if (behaviorstate == NPC_BehaviorState::CHECK)return;

    if (searchCoolDown > 1)
    {
        Vector3 dist = target->Pos() - transform->GlobalPos();

        // �þ߿� ���Դٸ�
        if (bDetection)
        {
            // �߰����� 2�ʰ� �Ƴ�
            if (bFind)
            {
                // ���� �����Ÿ��ȿ� ���� ���ϸ�
                if (dist.Length() > 5.0f)
                {
                    if (aStar->IsCollisionObstacle(transform->GlobalPos(), target->GlobalPos()))// �߰��� ��ֹ��� ������
                    {
                        SetPath(target->GlobalPos()); // ��ü���� ��� ��� ����
                    }
                    else
                    {
                        path.clear(); // ���� ��ֹ����µ� ��ã�� �ʿ� x
                        path.push_back(target->GlobalPos()); // ������ ���� ��ο� ����ֱ�
                    }

                    //Move();
                }
                else
                {
                    path.clear();
                    SetState(ATTACK);
                }

            }
            else
            {
                // õõ�� �������� �ɾ��.
                //Move();
                
            }
        }
        // �þ߿� �Ⱥ��δٸ�
        else
        {
            // ������ target�� �߰��� �������.
            if (bFind)
            {
                // ���� �ֱ� ������ ������ �̵�
                // ���θ� �ڵ����� �ֱٱ��� �̵��Ѵ�. 
                if (path.empty())
                {
                    // ��ΰ� ����ٸ� ���� �ֱ� target ��ġ���� �̵��� ��.
                    if (!missTargetTrigger)
                    {
                        missTargetTrigger = true;
                        DetectionStartTime = 2.0f;
                    }

                }
                //else Move();
            }
            else
            {
                if (!missTargetTrigger)
                {
                    missTargetTrigger = true;
                    DetectionStartTime = 2.0f;
                    path.clear();
                }


                //path.clear();
                
                if (aStar->IsCollisionObstacle(transform->GlobalPos(), PatrolPos[nextPatrol]))// �߰��� ��ֹ��� ������
                {
                    SetPath(PatrolPos[nextPatrol]); // ��ü���� ��� ��� ����
                }
                else
                {
                    path.clear(); // ���� ��ֹ����µ� ��ã�� �ʿ� x
                    path.push_back(PatrolPos[nextPatrol]); // ������ ���� ��ο� ����ֱ�
                }

                if (IsStartPos())
                {
                    isTracking = false;
                    SetState(IDLE);
                    path.clear();
                }
            }
        }
        searchCoolDown -= 1;
    }
    else
        searchCoolDown += DELTA;
}

void Orc::Move()
{   
    if (!bFind)
    {
        if (path.empty())
        {
            Vector3 dist = transform->Pos() - startPos;

            if (dist.Length() <= 0.5f)
            {
                SetState(IDLE);
                return;
            }
        }
    }

    if (velocity.Length() >= 5 && curState == ATTACK)
    {
        SetState(RUN);
        return;
    }
    float f=velocity.Length();
    if (curState == IDLE) return; 
    if (curState == ATTACK) return;
    if (curState == HIT) return;
    if (curState == DYING) return;
    if (behaviorstate == NPC_BehaviorState::CHECK)return;
    if (velocity.Length() < 5&&!missTargetTrigger) return;
    if (velocity.Length() < 0.1f && missTargetTrigger) return;

    if (!path.empty())
    {
        // ������ ���� �ִ�
        SetState(RUN);  // �����̱� + �޸��� ���� ����

        //���ͷ� ���� ��θ� �ϳ��� ã�ư��鼭 �����̱�

        Vector3 dest = path.back(); // ������ �̸��� ����� ������
        // = �������� ���� ���� ���� ������ ������
        // ��ΰ� �� �Ųٷγ�? -> ��ã�� �˰��򿡼� 
        // ��θ� ���� ������ �ۼ��� ó������, 
        // �˻�� ��� �߰��� �ڿ������� �߱� ����

        Vector3 direction = dest - transform->Pos(); // �� ��ġ���� �������� �������� ����

        direction.y = 0; // �ʿ��ϸ� �����ǳ��� �ݿ��� ���� ���� �ְ�,
        // ���� �ʿ��ϸ� �׳� �¿�ȸ���� �ϴ°ɷ� (y���� �Ϻη� ���� ����)

        if (direction.Length() < 0.1f)  // ���� �� ������ (������Ʈ ȣ�� �������� ��� ����)
        {
            path.pop_back(); // �� �� �������� ���Ϳ��� ����
        }

        // �������� ���� ���� ���� �̵�
        velocity = direction.GetNormalized(); // �ӷ±��� (������ ����ȭ)
        transform->Pos() += velocity * moveSpeed * DELTA; // �ӷ±��� * �����ӷ� * �ð����

        //transform->Pos() += velocity.GetNormalized() * speed * DELTA;
        transform->Rot().y = atan2(velocity.x, velocity.z) + XM_PI; // XY��ǥ ���� + ���Ĺ���(����ũ ����)
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
}


void Orc::IdleAIMove()
{
    // WALK�ִϸ��̼� �ذ� -> Orc_Walk0.clip ��� character1@walk30.clip ����� ��

    if (behaviorstate == NPC_BehaviorState::CHECK)return;
     Patrol();
     if (PatrolChange) {
         if (WaitTime >= 2.f) {
             WaitTime = 0.f;
             PatrolChange = false;
             SetState(WALK);
         }
         else {
             WaitTime += DELTA;
         }
     }

     if (curState == WALK)
     {
         transform->Rot().y = atan2(velocity.x, velocity.z) + XM_PI;
         transform->Pos() += DELTA * walkSpeed * transform->Back();
     }

    /*
    
      if (IsAiCooldown)
    {
        if (isAIWaitCooldown)
        {
            float randY = Random(XM_PIDIV2, XM_PI) * 2;
            transform->Rot().y = randY + XM_PI;
            //XMMatrixRotationY(randY + XM_PI);
            IsAiCooldown = false;
            aiCoolTime = 3.f; // ���߿� ��������
            SetState(WALK);
            isAIWaitCooldown = false;
            aiWaitCoolTime = 1.5f;
        }
        else
        {
            aiWaitCoolTime -= DELTA;
            if (aiWaitCoolTime <= 0)
                isAIWaitCooldown = true;
        }
        
    }
    else
    {
        if (aiCoolTime <= 0)
        {
            IsAiCooldown = true;
            SetState(IDLE);
            return;
        }

        // ���� �� ���� ���� �ε����ٸ� �ٷ� IsAiCooldown=true ��  
        aiCoolTime -= DELTA;
        transform->Pos() += DELTA * walkSpeed * transform->Back();
    }
    */
  
}

void Orc::UpdateUI()
{
    Vector3 barPos = transform->Pos() + Vector3(0, 20, 0);

    if (bDetection)
    {
        if (bFind)
        {
            questionMark->SetActive(false);
            exclamationMark->SetActive(true);
            exclamationMark->Pos() = CAM->WorldToScreen(barPos + Vector3(0, 5, 0));
            exclamationMark->UpdateWorld();
        }
        else
        {
            exclamationMark->SetActive(false);
            questionMark->SetActive(true);
            questionMark->Pos() = CAM->WorldToScreen(barPos + Vector3(0, 5, 0));
            questionMark->UpdateWorld();
        }
    }
    else
    {
        if (bFind)
        {
            if (missTargetTrigger)
            {
                exclamationMark->SetActive(false);
                questionMark->SetActive(true);
                questionMark->Pos() = CAM->WorldToScreen(barPos + Vector3(0, 5, 0));
                questionMark->UpdateWorld();
            }
            else
            {
                questionMark->SetActive(false);
                exclamationMark->SetActive(true);
                exclamationMark->Pos() = CAM->WorldToScreen(barPos + Vector3(0, 5, 0));
                exclamationMark->UpdateWorld();
            }
        }
        else
        {
            exclamationMark->SetActive(false);
            questionMark->SetActive(false);
        }
    }

    if (isHit)
    {
        curHP -= DELTA * 30 * 2;

        if (curHP <= destHP)
        {
            curHP = destHP;
            isHit = false;

            collider->SetActive(true);
            leftWeaponCollider->SetActive(true);
            rightWeaponCollider->SetActive(true);

            SetState(ATTACK); // ���� ������ �ϴ��� ���ݻ��·� ���ư��� ���� (���߿� ���� �ʿ��� ����)
        }
        hpBar->SetAmount(curHP / maxHp);
    }

    hpBar->UpdateWorld();

    if (!CAM->ContainPoint(barPos))
    {
        hpBar->SetActive(false);
        return;
    }

    if (!hpBar->Active()) hpBar->SetActive(true);
    hpBar->Pos() = CAM->WorldToScreen(barPos);

}



void Orc::SetState(State state)
{
    if (state == curState) return; // �̹� �� ���¶�� ���� ��ȯ �ʿ� ����

    curState = state; //�Ű������� ���� ���� ��ȭ
    //if(state==WALK)
      //  instancing->PlayClip(index, (int)state,0.5f); //�ν��Ͻ� �� �ڱ� Ʈ���������� ���� ���� ����
    //else 
        instancing->PlayClip(index, (int)state); //�ν��Ͻ� �� �ڱ� Ʈ���������� ���� ���� ����
    eventIters[state] = totalEvent[state].begin(); //�̺�Ʈ �ݺ��ڵ� ��ϵ� �̺�Ʈ ���۽�������

    // ->�̷��� ���¿� ������ �ϳ��� �����صθ�
    // ->�� ĳ���ʹ� ���¸� �ٲ��ָ� �ൿ�� �׶��׶� �˾Ƽ� �ٲ��ش�
}

void Orc::SetPath(Vector3 targetPos)
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
    path.insert(path.begin(), target->GlobalPos());


    //�����Ÿ��϶� ��ĭ��ĭ�̵��� �ʿ䰡 ���� -> ��ֹ� �������� �ϳ��� ���ͷ� ���� �ȴ� ->MakeDirectionPath�� ��������
}

void Orc::SetEvent(int clip, Event event, float timeRatio)
{
    if (totalEvent[clip].count(timeRatio) > 0) return; // ���� ����� �̺�Ʈ�� ������ ����    
    totalEvent[clip][timeRatio] = event;
}

void Orc::ExecuteEvent()
{
    int index = curState; //���� ���� �޾ƿ���
    if (totalEvent[index].empty()) return;
    if (eventIters[index] == totalEvent[index].end()) return;

    float ratio = motion->runningTime / motion->duration; //����� �ð� ������ ��ü ����ð�

    if (eventIters[index]->first > ratio) return; // ���� �ð��� ������ ���ؿ� �� ��ġ�� ����(�����)

    eventIters[index]->second(); //��ϵ� �̺�Ʈ ����
    eventIters[index]++;
}


void Orc::EndHit()
{
    collider->SetActive(true);
}

void Orc::EndDying()
{
    transform->SetActive(false);
    hpBar->SetActive(false);
    collider->SetActive(false);
    leftHand->SetActive(false);
    leftWeaponCollider->SetActive(false);
    questionMark->SetActive(false);
    exclamationMark->SetActive(false);
}

void Orc::CalculateEyeSight()
{
    float rad = XMConvertToRadians(eyeSightangle);
    Vector3 front = Vector3(transform->Forward().x, 0, transform->Forward().z).GetNormalized();

    Vector3 eyevector = Vector3(sinf(rad) + transform->GlobalPos().x, 0, cos(rad) + transform->GlobalPos().z);
    Vector3 rightdir = eyevector * eyeSightRange;
    Vector3 leftdir = -eyevector * eyeSightRange;

    Vector3 direction = target->GlobalPos() - transform->GlobalPos();
    direction.Normalize();

    float degree = XMConvertToDegrees(transform->Rot().y);

    float dirz = transform->Forward().z;
    float rightdir1 = -(180.f - eyeSightangle) + degree + 360;

    bool breverse = false;
    float leftdir1 = (180.f - eyeSightangle) + degree;


 

    /*
        -135    135
        -135 -45 135-45
        -180    90


    */

    //degree

    float Enemytothisangle = XMConvertToDegrees(atan2(direction.x, direction.z));
    if (Enemytothisangle < 0) {
        Enemytothisangle += 360;
    }

    if (Distance(target->GlobalPos(), transform->GlobalPos()) < eyeSightRange) {

        
            if (leftdir1 <= Enemytothisangle && rightdir1 >= Enemytothisangle) {
                //�߰�
            //    for (UINT i = 0; i < RobotManager::Get()->GetCollider().size(); ++i) {
            //        if (RobotManager::Get()->GetCollider()[i]->IsRayCollision(ray))
            //        {
            //            return;
            //        }
            //
            //    }
                // ���� ��ũ�Ŵ������� ���� �� ��� ���� üũ�ؼ� ray�浹��ũ
                // behaviorstate = NPC_BehaviorState::DETECT;
                bDetection = true;
            }
            else {
                if (Enemytothisangle > 0) {
                    Enemytothisangle += 360;
                }

                if (leftdir1 <= Enemytothisangle && rightdir1 >= Enemytothisangle) {
                    //�߰�
                    //    for (UINT i = 0; i < RobotManager::Get()->GetCollider().size(); ++i) {
            //        if (RobotManager::Get()->GetCollider()[i]->IsRayCollision(ray))
            //        {
            //            return;
            //        }
            //
            //    }
                // ���� ��ũ�Ŵ������� ���� �� ��� ���� üũ�ؼ� ray�浹��ũ
                    //  behaviorstate = NPC_BehaviorState::DETECT;
                    bDetection = true;
                }
            }
    }
    else
        bDetection = false;
}

void Orc::CalculateEarSight()
{
    Vector3 pos;
    float volume = -1.f;
    float distance = -1.f;
    if (Audio::Get()->IsPlaySound("PlayerWalk")) {

        pos.x = Audio::Get()->GetSoundPos("PlayerWalk").x;
        pos.y = Audio::Get()->GetSoundPos("PlayerWalk").y;
        pos.z = Audio::Get()->GetSoundPos("PlayerWalk").z;
        volume = Audio::Get()->GetVolume("PlayerWalk");
        distance = Distance(transform->GlobalPos(), pos);

        volume = Audio::Get()->GetVolume("PlayerWalk");
    }
    // ��ũ���� �ȴ� �Ҹ� ,�ϻ�Ҹ�  ����
    // �÷��̾� �Ҹ��� ���� �ü� ���� �Ҹ� �߰�




    if (distance == -1.f)return;

    if (distance < earRange * volume) {
        behaviorstate = NPC_BehaviorState::CHECK;
        CheckPoint = pos;
        StorePos = transform->GlobalPos();
        bFind = true;

    }


}

void Orc::Detection()
{
  //  if (NearFind) {
  //      bFind = true;
  //      bDetection = true;
  //  }
  //  else {
  //      if (bDetection) {
  //          DetectionStartTime += DELTA;
  //      }
  //      else {
  //          if (DetectionStartTime > 0.f) {
  //              DetectionStartTime -= DELTA;
  //              if (DetectionStartTime <= 0.f) {
  //                  DetectionStartTime = 0.f;
  //              }
  //          }
  //      }
  //      if (DetectionEndTime <= DetectionStartTime) {
  //          bFind = true;
  //          bSensor = true;
  //          behaviorstate = NPC_BehaviorState::DETECT;
  //          Vector3 pos;
  //          pos.x =transform->GlobalPos().x;
  //  pos.y =transform->GlobalPos().y;
  //  pos.z =transform->GlobalPos().z;
  //    pos.w=informrange;
  //          MonsterManager::Get()->PushPosition(transform->GlobalPos());
  //          MonsterManager::Get()->CalculateDistance();
  //      }
  //  } ���� ����
    
    if (bDetection) {
        DetectionStartTime += DELTA;
    }
    else {
        if (DetectionStartTime > 0.f) {
            if (missTargetTrigger)
            {
                DetectionStartTime -= DELTA * 2;
            }
            if (DetectionStartTime <= 0.f) {
                DetectionStartTime = 0.f;
                bFind = false;
                if (behaviorstate != NPC_BehaviorState::IDLE)
                {
                    behaviorstate = NPC_BehaviorState::CHECK;
                    SetState(IDLE);
                }missTargetTrigger = false;
            }
        }
    }
    if (DetectionEndTime <= DetectionStartTime) {
        bFind = true;
        isTracking = true;
        Float4 pos;
        behaviorstate = NPC_BehaviorState::DETECT;
        pos.x =transform->GlobalPos().x;
        pos.y =transform->GlobalPos().y;
        pos.z =transform->GlobalPos().z;
        pos.w = informrange;
        MonsterManager::Get()->PushPosition(pos);
        MonsterManager::Get()->CalculateDistance();

        if (curState == IDLE)
            SetState(RUN);
    }
}

void Orc::SetRay(Vector3& _pos)
{
    ray.pos = transform->GlobalPos();
    ray.pos.y += 100;

    ray.dir = _pos - ray.pos;
    ray.dir.Normalize();
}

void Orc::Patrol()
{
    if (PatrolPos[nextPatrol].x > transform->GlobalPos().x - 1.1f
        && PatrolPos[nextPatrol].x < transform->GlobalPos().x + 1.1f) {
        if (PatrolPos[nextPatrol].z > transform->GlobalPos().z - 1.1f
            && PatrolPos[nextPatrol].z < transform->GlobalPos().z + 1.1f)
        {
            nextPatrol += 1;
            if (nextPatrol >= PatrolPos.size()) {
                nextPatrol = 0;
            }
            PatrolChange = true;
            SetState(IDLE);
        }
    }
}

bool Orc::IsStartPos()
{
    
    if (PatrolPos[nextPatrol].x + 1.0f > transform->Pos().x && PatrolPos[nextPatrol].x - 1.0f < transform->Pos().x &&
        PatrolPos[nextPatrol].z + 1.0f > transform->Pos().z && PatrolPos[nextPatrol].z - 1.0f < transform->Pos().z)
        return true;
    else return false;
}

void Orc::RangeCheck()
{
    if (behaviorstate != NPC_BehaviorState::CHECK)return;

    rangetime += DELTA;
    
    if (rangetime >= 3.f) {
        m_uiRangeCheck++;
        rangetime = 0.f;
        
    }

    if (!m_uiRangeCheck) {
        transform->Rot().y += DELTA;
    }
    else
        transform->Rot().y -= 2*DELTA;
    

    if (m_uiRangeCheck == 2)
    {
        behaviorstate = NPC_BehaviorState::IDLE;
        m_uiRangeCheck = 0;
        SetState(WALK);
        missTargetTrigger = false;
    }
}

void Orc::AddObstacleObj(Collider* collider)
{
    aStar->AddObstacleObj(collider);
}