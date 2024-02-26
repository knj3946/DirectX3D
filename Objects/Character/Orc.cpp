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
    SetEvent(ATTACK, bind(&Orc::EndAttack, this), 0.99f);
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
    collider->SetActive(false); //spawn �Ҷ� Ȱ��ȭ

    // ���� �浹ü
    leftHand = new Transform();
    leftWeaponCollider = new CapsuleCollider(8,50);
    leftWeaponCollider->Pos().y += 20;
    leftWeaponCollider->SetParent(leftHand); // �ӽ÷� ���� �浹ü�� "��" Ʈ�������� �ֱ�
    leftWeaponCollider->SetActive(false); //attack �Ҷ� Ȱ��ȭ

    weaponColliders.push_back(leftWeaponCollider);

    rightHand = new Transform();
    rightWeaponCollider = new CapsuleCollider(6, 30);
    //rightWeaponCollider->Pos().y += 20;
    rightWeaponCollider->SetParent(rightHand); // �ӽ÷� ���� �浹ü�� "��" Ʈ�������� �ֱ�
    rightWeaponCollider->SetActive(false); //attack �Ҷ� Ȱ��ȭ

    weaponColliders.push_back(rightWeaponCollider);

    // hp UI
    hpBar = new ProgressBar(L"Textures/UI/hp_bar.png", L"Textures/UI/hp_bar_BG.png");
    hpBar->Scale() *= 0.01f;
    hpBar->SetAmount(curHP / maxHp);

    exclamationMark = new Quad(L"Textures/UI/Exclamationmark.png");
    questionMark = new Quad(L"Textures/UI/QuestionMark.png");
    exclamationMark->Scale() *= 0.1f;
    questionMark->Scale() *= 0.1f;

    //aStar = new AStar(512, 512);
    
    //aStar->SetNode();

    computeShader = Shader::AddCS(L"Compute/ComputePicking.hlsl");
    rayBuffer = new RayBuffer();
    
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
    for (Collider* wcollider : weaponColliders)
        delete wcollider;
}

void Orc::SetType(NPC_TYPE _type) {

    switch (_type)
    {
    case Orc::NPC_TYPE::ATTACK:// ��ũ Ÿ�Կ� ���� Ž�� ���� ���� (���� �ӽ�)
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
  
    Direction();//
    
    CalculateEyeSight();
    CalculateEarSight();
    SoundPositionCheck();
    eyesRayDetect = EyesRayToDetectTarget();

    Detection();
  
    RangeCheck();
   
    ExecuteEvent();
    UpdateUI();
    TimeCalculator();

    {
        Vector3 OrcSkyPos = transform->Pos();
        OrcSkyPos.y += 100;
        Ray groundRay = Ray(OrcSkyPos, Vector3(transform->Down()));
        TerainComputePicking(feedBackPos, groundRay);
    }

    if (!collider->Active())return;

    if (isHit)
    {
        //�´� ���̸� �ٸ����� �� �� ����
        wateTime = 0;
    }
    else
    {
        if (behaviorstate == NPC_BehaviorState::CHECK)return;
        if (isTracking == false && path.empty())
        {
            //IdleAIMove();
        }
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
                leftWeaponCollider->SetActive(true);
                rightWeaponCollider->SetActive(true);
                break;
            }
            default:
                //if (!isAttackable)break;
                Move();
                leftWeaponCollider->SetActive(false);
                rightWeaponCollider->SetActive(false);
            }
            wateTime -= DELTA;
        }
    }
    
    //root->SetWorld(instancing->GetTransformByNode(index, 3));
    transform->SetWorld(instancing->GetTransformByNode(index, 5));
    collider->UpdateWorld();
    transform->UpdateWorld();

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
    hpBar->Render();
    //aStar->Render();
}

void Orc::PostRender()
{
    if (!transform->Active())return;
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
    ImGui::Text("eyesRayDetect : %d", eyesRayDetect);
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

    // ��������
    if (behaviorstate == NPC_BehaviorState::IDLE) {
        velocity = PatrolPos[nextPatrol] - transform->GlobalPos();
        isTracking = false;
    }
    else if (behaviorstate == NPC_BehaviorState::SOUNDCHECK) {
        velocity = CheckPoint - transform->GlobalPos();
    }
    else if(behaviorstate==NPC_BehaviorState::DETECT) {
        velocity = target->GlobalPos() - transform->GlobalPos();
    }
}

float Orc::Hit()
{
    float r = 0.0f;
    if (curState == ATTACK)
    {
        r = 10.0f;
    }
    return r;
}
float Orc::GetDamage()
{
    float r = 0.0f;
    if (curState == ATTACK)
    {
        r = 10.0f;
    }
    return r;
}

void Orc::Hit(float damage)
{
    if (!isHit)
    {
        destHP = (curHP - damage > 0) ? curHP - damage : 0;

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
    SetState(IDLE); // ������ ��å

    curHP = maxHp;
    hpBar->SetAmount(curHP / maxHp);

    transform->Pos() = pos;

    transform->SetActive(true); //��Ȱ��ȭ���ٸ� Ȱ��ȭ ����
    collider->SetActive(true);
    //leftWeaponCollider->SetActive(true);
    //rightWeaponCollider->SetActive(true);
}

void Orc::AttackTarget()
{
    return;
    if (!bFind)
    {
        bFind = true;
        isTracking = true;
        //if (curState == IDLE)
            SetState(RUN);
    }
}



void Orc::Findrange()
{
    if (curState == ATTACK)return;
    // Ž���� ������ ���� �ֿ��� ����
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
    if (behaviorstate == NPC_BehaviorState::SOUNDCHECK)return;
    if (searchCoolDown > 1)
    {
        Vector3 dist = target->Pos() - transform->GlobalPos();

        // �þ߿� ���Դٸ�
        if (bDetection && eyesRayDetect)
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
                    SetState(ATTACK,3.0f);
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
                  //      DetectionStartTime = 2.0f;
                    }

                }
                //else Move();
            }
            else
            {
                if (!missTargetTrigger)
                {
                    missTargetTrigger = true;
             //       DetectionStartTime = 2.0f;
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
        if (behaviorstate == NPC_BehaviorState::CHECK)
            return;
        SetState(RUN);
        return;
    }
    float f=velocity.Length();
    if (curState == IDLE) return; 
    if (curState == ATTACK) return;
    if (curState == HIT) return;
    if (curState == DYING) return;
    if (behaviorstate == NPC_BehaviorState::CHECK)return;
    if (velocity.Length() < 0.1f&&!missTargetTrigger) return;
    if (velocity.Length() < 0.1f && missTargetTrigger) return;

    if (!path.empty())
    {
        // ������ ���� �ִ�
     
        if(behaviorstate!=NPC_BehaviorState::SOUNDCHECK&&!bSound)
            SetState(RUN);  // �����̱� + �޸��� ���� ����
        else
            SetState(WALK);
        //���ͷ� ���� ��θ� �ϳ��� ã�ư��鼭 �����̱�

        Vector3 dest = path.back(); // ������ �̸��� ����� ������
        // = �������� ���� ���� ���� ������ ������
        // ��ΰ� �� �Ųٷγ�? -> ��ã�� �˰����򿡼� 
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

        

        //���� ������
        
        Vector3 destFeedBackPos;
        Vector3 destPos = transform->Pos() + velocity * moveSpeed * DELTA;
        Vector3 OrcSkyPos = destPos;
        OrcSkyPos.y += 100;
        Ray groundRay = Ray(OrcSkyPos, Vector3(transform->Down()));
        TerainComputePicking(destFeedBackPos, groundRay);

        //destFeedBackPos : ������ �ͷ���Pos
        //feedBackPos : ���� �ͷ���Pos

        //�������� ���� ���ϱ�
        Vector3 destDir = destFeedBackPos - feedBackPos;
        Vector3 destDirXZ = destDir;
        destDirXZ.y = 0;

        //����
        float radianHeightAngle = acos(abs(destDirXZ.Length()) / abs(destDir.Length()));

        transform->Pos() += velocity * moveSpeed * DELTA; // �̵� ����
        feedBackPos.y = destFeedBackPos.y;
        transform->Pos().y = feedBackPos.y;
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
            float randY = Random(0.0f, XM_2PI) * 2;
            //float randY = Random(XM_PIDIV2, XM_PI) * 2;
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

        // TODO : ���� �� ���� ���� �ε����ٸ� �ٷ� IsAiCooldown=true ��  
        aiCoolTime -= DELTA;

        //���� ������
        Vector3 direction = transform->Back();

        Vector3 destFeedBackPos;
        Vector3 destPos = transform->Pos() + direction * walkSpeed * DELTA;
        Vector3 OrcSkyPos = destPos;
        OrcSkyPos.y += 100;
        Ray groundRay = Ray(OrcSkyPos, Vector3(transform->Down()));
        TerainComputePicking(destFeedBackPos, groundRay);

        //destFeedBackPos : ������ �ͷ���Pos
        //feedBackPos : ���� �ͷ���Pos

        //�������� ���� ���ϱ�
        Vector3 destDir = destFeedBackPos - feedBackPos;
        Vector3 destDirXZ = destDir;
        destDirXZ.y = 0;

        //����
        float radianHeightAngle = acos(abs(destDirXZ.Length()) / abs(destDir.Length()));


        if ((radianHeightAngle < XMConvertToRadians(60) || destFeedBackPos.y <= feedBackPos.y)) //���� 60������ �۾ƾ� �̵�, Ȥ�� ������ ���̰� �� ���ƾ���
        {
            transform->Pos() += direction * walkSpeed * DELTA; // �̵� ����
            feedBackPos.y = destFeedBackPos.y;
        }
            
        transform->Pos().y = feedBackPos.y;
    }
     */
 
  
}

void Orc::UpdateUI()
{
    Vector3 barPos = transform->Pos() + Vector3(0, 6, 0);

    if (bDetection)
    {
        if (bFind)
        {
            questionMark->SetActive(false);
            exclamationMark->SetActive(true);
            exclamationMark->Pos() = CAM->WorldToScreen(barPos + Vector3(0, 1, 0));
            exclamationMark->UpdateWorld();
        }
        else
        {
            exclamationMark->SetActive(false);
            questionMark->SetActive(true);
            questionMark->Pos() = CAM->WorldToScreen(barPos + Vector3(0, 1, 0));
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
                questionMark->Pos() = CAM->WorldToScreen(barPos + Vector3(0, 1, 0));
                questionMark->UpdateWorld();
            }
            else
            {
                questionMark->SetActive(false);
                exclamationMark->SetActive(true);
                exclamationMark->Pos() = CAM->WorldToScreen(barPos + Vector3(0, 1, 0));
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

    hpBar->Pos() = transform->Pos() + Vector3(0, 6, 0);
    Vector3 dir = hpBar->Pos() - CAM->Pos();
    hpBar->Rot().y = atan2(dir.x, dir.z);

    float scale = 1 / velocity.Length();
    scale *= 0.03f;
    scale = Clamp(0.01f, 0.5f, scale);
    hpBar->Scale() = { scale, scale, scale };
    
    hpBar->UpdateWorld();
}

void Orc::TimeCalculator()
{
    // false ���� �� ���� ����
    if (!isAttackable)
    {
        curAttackSpeed += DELTA;
        if (curAttackSpeed > attackSpeed)
        {
            isAttackable = true;
            curAttackSpeed = 0;
        }
    }
}



void Orc::SetState(State state, float scale, float takeTime)
{
    if (state == curState) return; // �̹� �� ���¶�� ���� ��ȯ �ʿ� ����

    // ���� �ӵ� ���� -> Attack������ attackSpeed��ŭ �ð��� ������ �ʾҴٸ� ���� x
    if (state == ATTACK && !isAttackable)
    {
        state = IDLE;
        //return;
    }

    curState = state; //�Ű������� ���� ���� ��ȭ
    if (state == ATTACK)
    {
        int randNum = GameMath::Random(0, 3);
        //�ν��Ͻ� �� �ڱ� Ʈ���������� ���� ���� ����
        switch (randNum)
        {
        case 0:
            instancing->PlayClip(index, (int)state + randNum, 0.7);
            break;
        case 1:
            instancing->PlayClip(index, (int)state + randNum, 0.7);
            break;
        case 2:
            instancing->PlayClip(index, (int)state + randNum, 0.7);
            break;

        default:
            break;
        }
    }
    else
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
    path.insert(path.begin(), targetPos);


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


void Orc::EndAttack()
{
    isAttackable = false;
    SetState(IDLE);
    //SetState(ATTACK);
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

        SetRay(target->GlobalPos());
            if (leftdir1 <= Enemytothisangle && rightdir1 >= Enemytothisangle) {
                //�߰�
                for (UINT i = 0; i < ColliderManager::Get()->Getvector(ColliderManager::WALL).size(); ++i) {
                    if (ColliderManager::Get()->Getvector(ColliderManager::WALL)[i]->IsRayCollision(ray))
                    {
                        if (bDetection) {
                            bDetection = false;
                            DetectionStartTime = 0.001f;
                        }
                        return;
                   }
            
                }
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
                    //   
                    for (UINT i = 0; i < ColliderManager::Get()->Getvector(ColliderManager::WALL).size(); ++i) {
                    if (ColliderManager::Get()->Getvector(ColliderManager::WALL)[i]->IsRayCollision(ray))
                    {
                        if (bDetection) {
                            bDetection = false;
                            DetectionStartTime = 0.001f;
                        }
                        return;
                    }

                }
    
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
    if (behaviorstate != NPC_BehaviorState::IDLE)return;
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
        SetState(WALK);
        behaviorstate = NPC_BehaviorState::SOUNDCHECK;
        CheckPoint = pos;
        StorePos = transform->GlobalPos();
        path.clear();
        bSound = true;
        SetPath(CheckPoint);
       
    }


}

void Orc::Detection()
{


    if (bDetection) {
        if (!bFind)
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
                if (behaviorstate != NPC_BehaviorState::CHECK)
                    rangeDegree = XMConvertToDegrees(transform->Rot().y);
                behaviorstate = NPC_BehaviorState::CHECK;
                SetState(IDLE);
                missTargetTrigger = false;

            }
        }
    }
    if (DetectionEndTime <= DetectionStartTime) {
        if (bFind)return;
        bFind = true;
        isTracking = true;
        Float4 pos;
        behaviorstate = NPC_BehaviorState::DETECT;
        pos.x = transform->GlobalPos().x;
        pos.y = transform->GlobalPos().y;
        pos.z = transform->GlobalPos().z;
        pos.w = informrange;
        MonsterManager::Get()->PushPosition(pos);
        MonsterManager::Get()->CalculateDistance();
        if (curState == IDLE)
            SetState(RUN);
    }

}

void Orc::SetRay(Vector3 _pos)
{
    ray.pos = transform->GlobalPos();
    ray.pos.y += 10.f;

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

    
    float curdegree= XMConvertToDegrees(transform->Rot().y);//
    if (0 == m_uiRangeCheck)
    {
        if (rangeDegree + 45.f < curdegree) {// �÷��̾ ��ģ �� �������� �������� 45 ������
            m_uiRangeCheck++;
        }
    }
    else
    {
        if (rangeDegree - 45.f > curdegree) {// �÷��̾ ��ģ �� �������� �������� 45 ������
            m_uiRangeCheck++;
        }
    }
    

    if (!m_uiRangeCheck) {
        transform->Rot().y += DELTA*1.5f;
    }
    else
        transform->Rot().y -= DELTA*2.F;
    

    if (m_uiRangeCheck == 2)
    {
        behaviorstate = NPC_BehaviorState::IDLE;
        m_uiRangeCheck = 0;
        SetState(WALK);
        missTargetTrigger = false;
        isTracking = false;
        rangeDegree = 0.f   ;
        bFind = false;
        path.clear();
    }

}

void Orc::SoundPositionCheck()
{

    if (behaviorstate == NPC_BehaviorState::SOUNDCHECK) {
        if (CheckPoint.x + 1.0f > transform->Pos().x && CheckPoint.x - 1.0f < transform->Pos().x &&
            CheckPoint.z + 1.0f > transform->Pos().z && CheckPoint.z - 1.0f < transform->Pos().z)
        {
            behaviorstate = NPC_BehaviorState::CHECK;
            rangeDegree = XMConvertToDegrees(transform->Rot().y);
            path.clear();
            SetPath(StorePos);
            SetState(IDLE);
            bSound = false;
        }
    }
  
}

void Orc::AddObstacleObj(Collider* collider)
{
    aStar->AddObstacleObj(collider);
}

bool Orc::TerainComputePicking(Vector3& feedback, Ray ray)
{
    if (terrain && structuredBuffer)
    {
        rayBuffer->Get().pos = ray.pos;
        rayBuffer->Get().dir = ray.dir;
        rayBuffer->Get().triangleSize = terrainTriangleSize;

        rayBuffer->SetCS(0);

        DC->CSSetShaderResources(0, 1, &structuredBuffer->GetSRV());
        DC->CSSetUnorderedAccessViews(0, 1, &structuredBuffer->GetUAV(), nullptr);

        computeShader->Set();

        UINT x = ceil((float)terrainTriangleSize / 64.0f);

        DC->Dispatch(x, 1, 1);

        structuredBuffer->Copy(outputs.data(), sizeof(OutputDesc) * terrainTriangleSize);

        float minDistance = FLT_MAX;
        int minIndex = -1;

        UINT index = 0;
        for (OutputDesc output : outputs)
        {
            if (output.picked)
            {
                if (minDistance > output.distance)
                {
                    minDistance = output.distance;
                    minIndex = index;
                }
            }
            index++;
        }

        if (minIndex >= 0)
        {
            feedback = ray.pos + ray.dir * minDistance;
            return true;
        }
    }

    return false;
}

bool Orc::EyesRayToDetectTarget()
{
    // �þ߾ȿ� ������ ��� ��¥�� ���̴� ����ΰ� Ȯ�� EyesRaySearchScene ����
    // ��ǥ �ݶ��̴� ���� �� �Ʒ� �� ���� �� ������ �߾� 5������ ���̸� ���� Ȯ�� (0.8�� ���ؼ� �� �������� ���)

    if (!bDetection) // �þ߾ȿ� ������ �ʾ����� �׳� ����
        return false;

    Vector3 orcEyesPos = transform->GlobalPos(); //��ũ �� ��ġ �����ʿ�
    orcEyesPos.y += 5;

    Vector3 top = targetCollider->GlobalPos() + (transform->Up() * targetCollider->Height() * 0.4 + transform->Up() * targetCollider->Radius() * 0.8);
    Vector3 bottom = targetCollider->GlobalPos() + (transform->Down() * targetCollider->Height() * 0.4 + transform->Down() * targetCollider->Radius() * 0.8);
    Vector3 left = targetCollider->GlobalPos() + (transform->Right() * targetCollider->Radius() * 0.8);
    Vector3 right = targetCollider->GlobalPos() + (transform->Left() * targetCollider->Radius() * 0.8);
    Vector3 center = targetCollider->GlobalPos();

    Contact topCon;
    Contact bottomCon;
    Contact leftCon;
    Contact rightCon;
    Contact centerCon;

    Ray topRay = Ray(orcEyesPos, (top - orcEyesPos).GetNormalized());
    Ray bottomRay = Ray(orcEyesPos, (bottom - orcEyesPos).GetNormalized());
    Ray leftRay = Ray(orcEyesPos, (left - orcEyesPos).GetNormalized());
    Ray rightRay = Ray(orcEyesPos, (right - orcEyesPos).GetNormalized());
    Ray centerRay = Ray(orcEyesPos, (center - orcEyesPos).GetNormalized());

    targetCollider->IsRayCollision(topRay, &topCon);
    targetCollider->IsRayCollision(bottomRay, &bottomCon);
    targetCollider->IsRayCollision(leftRay, &leftCon);
    targetCollider->IsRayCollision(rightRay, &rightCon);
    targetCollider->IsRayCollision(centerRay, &centerCon);

    float topBlockdistance = ColliderManager::Get()->CloseRayCollisionColliderDistance(topRay);
    float bottomBlockdistance = ColliderManager::Get()->CloseRayCollisionColliderDistance(bottomRay);
    float leftBlockdistance = ColliderManager::Get()->CloseRayCollisionColliderDistance(leftRay);
    float rightBlockdistance = ColliderManager::Get()->CloseRayCollisionColliderDistance(rightRay);
    float centerBlockdistance = ColliderManager::Get()->CloseRayCollisionColliderDistance(centerRay);

    if (topBlockdistance < Distance(topCon.hitPoint, topRay.pos)
        && bottomBlockdistance < Distance(bottomCon.hitPoint, bottomRay.pos)
        && leftBlockdistance < Distance(leftCon.hitPoint, leftRay.pos)
        && rightBlockdistance < Distance(rightCon.hitPoint, rightRay.pos)
        && centerBlockdistance < Distance(centerCon.hitPoint, centerRay.pos)
        )
    {
        //��� �κп��� ����Ǿ���
        return false;
    }

    return true;
}

void Orc::SetOutLine(bool flag)
{
    instancing->SetOutLine(index,flag);
}