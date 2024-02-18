#include "Framework.h"

Orc::Orc(Transform* transform, ModelAnimatorInstancing* instancing, UINT index)
    :transform(transform), instancing(instancing), index(index)
{
    //Ŭ�� �����صα� 
    string modelName = "Orc";

    root = new Transform();


    motion = instancing->GetMotion(index);
    totalEvent.resize(instancing->GetClipSize()); //���� ���� ���� ���ڸ�ŭ �̺�Ʈ ������¡
    eventIters.resize(instancing->GetClipSize());

    //�̺�Ʈ ����
    SetEvent(HIT, bind(&Orc::EndHit, this), 0.9f);
    SetEvent(DYING, bind(&Orc::EndDying, this), 0.99f);

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
    /* delete target;
     delete targetCollider;*/ // �ٸ������� ����
    delete instancing;
    delete motion;
    delete root;
    delete transform;
}

void Orc::Update()
{
    if (!transform->Active()) return;

    // TODO : ����� �ִϸ��̼� ���缭 �����̰�, �����ݶ��̴���
    mainHand->SetWorld(instancing->GetTransformByNode(index, 30));
    tmpCollider->UpdateWorld();

    velocity = target->GlobalPos() - transform->GlobalPos();

    CalculateEyeSight();
    Detection();

    ExecuteEvent();
    if (!collider->Active())return;

    Control();
    Move();
    UpdateUI();
    
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
    hpBar->Render();
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

void Orc::Hit()
{
    curHP -= 50; // �ӽ÷� ������
    hpBar->SetAmount(curHP / maxHp);
    collider->SetActive(false);
    if (curHP < 0)
    {
        SetState(DYING);

        return;
    }

    SetState(HIT);
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

void Orc::Control()
{
    if (searchCoolDown > 1)
    {
        Vector3 dist = target->Pos() - transform->GlobalPos();

        if (dist.Length() > 100)// �ʹ� �־����� ���Ѿư���
        {
            path.clear();
            return;
        }
        
        if (dist.Length() < 5)
        {
            SetState(ATTACK);
            path.clear();
            return;
        }
        
        if (aStar->IsCollisionObstacle(transform->GlobalPos(), target->GlobalPos()))// �߰��� ��ֹ��� ������
        {
            SetPath(); // ��ü���� ��� ��� ����
        }
        else
        {
            path.clear(); // ���� ��ֹ����µ� ��ã�� �ʿ� x
            path.push_back(target->GlobalPos()); // ������ ���� ��ο� ����ֱ�
            // ->����� Move�� �������� ã�ư� ��
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
        SetState(IDLE);
        return;
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

void Orc::UpdateUI()
{
    Vector3 barPos = transform->Pos() + Vector3(0, 20, 0);
    if (!CAM->ContainPoint(barPos))
    {
        //hpBar->Scale() = {0, 0, 0};
        hpBar->SetActive(false);
        return;
    }

    if (!hpBar->Active()) hpBar->SetActive(true);
    hpBar->Pos() = CAM->WorldToScreen(barPos);
    /*curHP -= DELTA*3;
    hpBar->SetAmount(curHP / maxHp);*/
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

void Orc::SetPath()
{
    int startIndex = aStar->FindCloseNode(transform->GlobalPos());
    int endIndex = aStar->FindCloseNode(target->GlobalPos()); // �������(+������Ʈ����) ������ ������

    aStar->GetPath(startIndex, endIndex, path); // ���� �� ���� path ���Ϳ� ����

    aStar->MakeDirectionPath(transform->GlobalPos(), target->GlobalPos(), path); // ��ֹ��� ����� path�� ������

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
    hpBar->SetActive(false);
    collider->SetActive(false);
    transform->SetActive(false);
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
                bDetection = true;
            }
            else {
                if (Enemytothisangle > 0) {
                    Enemytothisangle += 360;
                }

                if (leftdir1 <= Enemytothisangle && rightdir1 >= Enemytothisangle) {
                    //�߰�
                    bDetection = true;
                    
                }
            }
    }
}

void Orc::Detection()
{
    if (bDetection) {
        DetectionStartTime += DELTA;
    }
    else {
        if (DetectionStartTime > 0.f) {
            DetectionStartTime -= DELTA;
            if (DetectionStartTime <= 0.f) {
                DetectionStartTime = 0.f;
            }
        }
    }
    if (DetectionEndTime <= DetectionStartTime) {
        bFind = true;
        if (curState == IDLE)
            SetState(RUN);
    }
}

void Orc::AddObstacleObj(Collider* collider)
{
    aStar->AddObstacleObj(collider);
}
