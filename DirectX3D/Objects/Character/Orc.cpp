#include "Framework.h"

Orc::Orc(Transform* transform, ModelAnimatorInstancing* instancing, UINT index)
    :transform(transform), instancing(instancing), index(index)
{
    //Ŭ�� �����صα� 
    string modelName = "Orc";

    root = new Transform();

    transform->Pos() = startPos;

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
    mainHand = new Transform();
    tmpCollider = new SphereCollider();
    tmpCollider->Scale() *= 0.3;
    tmpCollider->SetParent(mainHand); // �ӽ÷� ���� �浹ü�� "��" Ʈ�������� �ֱ�


    // hp UI
    hpBar = new ProgressBar(L"Textures/UI/hp_bar.png", L"Textures/UI/hp_bar_BG.png");
    hpBar->Scale() *= 0.6f;
    hpBar->SetAmount(curHP / maxHp);

    exclamationMark = new Quad(L"Textures/UI/Exclamationmark.png");
    questionMark = new Quad(L"Textures/UI/QuestionMark.png");
    exclamationMark->Scale() *= 0.1f;
    questionMark->Scale() *= 0.1f;

    //aStar = new AStar(512, 512);
    aStar = new AStar(256, 256);
    aStar->SetNode();

}

Orc::~Orc()
{
    delete mainHand;
    delete tmpCollider;
    delete collider;
    delete terrain;
    delete aStar;
    delete instancing;
    delete motion;
    delete root;
    delete transform;
    delete exclamationMark;
    delete questionMark;
}

void Orc::Update()
{
    if (!transform->Active()) return;

    // TODO : ����� �ִϸ��̼� ���缭 �����̰�, �����ݶ��̴���
    mainHand->SetWorld(instancing->GetTransformByNode(index, 5));
    tmpCollider->UpdateWorld();

    velocity = target->GlobalPos() - transform->GlobalPos();

    CalculateEyeSight();
    Detection();
    ExecuteEvent();
    UpdateUI();

    if (!collider->Active())return;

    if (isTracking==false)
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

    /*
    if (collider->IsCapsuleCollision(targetCollider))
    {
        Hit();
    }
    */

    root->SetWorld(instancing->GetTransformByNode(index, 3));
    collider->UpdateWorld();
}

void Orc::Render()
{
    collider->Render();
    tmpCollider->Render();
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
    aStar->SetNode(terrain);
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
}

void Orc::Hit()
{
    isHit = true;

    destHP = (curHP - 30 > 0) ? curHP - 30 : 0;

    collider->SetActive(false);
    if (destHP<=0)
    {
        SetState(DYING);

        return;
    }

    SetState(HIT);
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
        if (curState == IDLE)
            SetState(RUN);
    }
}

void Orc::Control()
{
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
                        SetPath(target->Pos()); // ��ü���� ��� ��� ����
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
                //path.clear();

                if (aStar->IsCollisionObstacle(transform->GlobalPos(), startPos))// �߰��� ��ֹ��� ������
                {
                    SetPath(startPos);
                }
                else
                {
                    path.clear(); // ���� ��ֹ����µ� ��ã�� �ʿ� x
                    path.push_back(startPos); // ������ ���� ��ο� ����ֱ�
                }


                //Move();
                if (path.empty())
                {
                    isTracking = false;
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

    if (curState == IDLE) return; 
    if (curState == ATTACK) return;
    if (curState == HIT) return;
    if (curState == DYING) return;
    if (velocity.Length() < 5) return;
    
    if (path.size() > 0)
    {
        // ������ ���� �ִ�
        //SetState(RUN);  // �����̱� + �޸��� ���� ����

        //���ͷ� ���� ��θ� �ϳ��� ã�ư��鼭 �����̱�

        Vector3 dest = path.back(); // ������ �̸��� ����� ������
        // = �������� ���� ���� ���� ������ ������
        // ��ΰ� �� �Ųٷγ�? -> ��ã�� �˰��򿡼� 
        // ��θ� ���� ������ �ۼ��� ó������, 
        // �˻�� ��� �߰��� �ڿ������� �߱� ����

        Vector3 direction = dest - transform->Pos(); // �� ��ġ���� �������� �������� ����

        direction.y = 0; // �ʿ��ϸ� �����ǳ��� �ݿ��� ���� ���� �ְ�,
        // ���� �ʿ��ϸ� �׳� �¿�ȸ���� �ϴ°ɷ� (y���� �Ϻη� ���� ����)

        if (direction.Length() < 0.5f)  // ���� �� ������ (������Ʈ ȣ�� �������� ��� ����)
        {
            path.pop_back(); // �� �� �������� ���Ϳ��� ����
        }

        // �������� ���� ���� ���� �̵�
        velocity = direction.GetNormalized(); // �ӷ±��� (������ ����ȭ)
        transform->Pos() += velocity * moveSpeed * DELTA; // �ӷ±��� * �����ӷ� * �ð����

        //transform->Pos() += velocity.GetNormalized() * speed * DELTA;
        transform->Rot().y = atan2(velocity.x, velocity.z) + XM_PI; // XY��ǥ ���� + ���Ĺ���(����ũ ����)
    }
}


void Orc::IdleAIMove()
{

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

    if (!CAM->ContainPoint(barPos))
    {
        hpBar->SetActive(false);
        return;
    }

    if (!hpBar->Active()) hpBar->SetActive(true);
    hpBar->Pos() = CAM->WorldToScreen(barPos);
    
    if (isHit)
    {
        curHP -= DELTA*30;

        if (curHP <= destHP)
        {
            curHP = destHP;
            isHit = false;
        }
        hpBar->SetAmount(curHP / maxHp);
    }
    
    hpBar->UpdateWorld();
}



void Orc::SetState(State state)
{
    if (state == curState) return; // �̹� �� ���¶�� ���� ��ȯ �ʿ� ����

    curState = state; //�Ű������� ���� ���� ��ȭ
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
    mainHand->SetActive(false);
    tmpCollider->SetActive(false);
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


    if (leftdir1 < 0) {
        leftdir1 += 360;
        rightdir1 += 360;
        breverse = true;
    }

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

        if (!breverse)
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
                missTargetTrigger = false;
            }
        }
    }
    if (DetectionEndTime <= DetectionStartTime) {
        bFind = true;
        isTracking = true;
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

void Orc::AddObstacleObj(Collider* collider)
{
    aStar->AddObstacleObj(collider);
}