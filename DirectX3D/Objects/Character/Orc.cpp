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
    SetEvent(Hit, bind(&Orc::EndHit, this), 0.9f);
    SetEvent(Dying, bind(&Orc::EndDying, this), 0.9f);

    FOR(totalEvent.size())
    {
        eventIters[i] = totalEvent[i].begin(); // ��ϵǾ� ���� �̺�Ʈ�� ������������ �ݺ��� ����
    }



    //�浹ü
    // TODO : �𵨿� �ִϸ��̼� ��ü�� ��ġ�� �̵���Ų��. 
    collider = new CapsuleCollider(30, 160);
    collider->SetParent(root);
    //collider->Rot().z = XM_PIDIV2 - 0.2f;
    collider->Pos() = { -15, 10, 0 };
    collider->SetActive(true);
}

Orc::~Orc()
{
}

void Orc::Update()
{
    if (!transform->Active()) return;

    velocity = target->GlobalPos() - transform->GlobalPos();

    ExecuteEvent();
    Control();
    Move();

    root->SetWorld(instancing->GetTransformByNode(index, 3));
    collider->UpdateWorld();
}

void Orc::Render()
{
    collider->Render();
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

        if (dist.Length() < 25)
        {
            SetState(Attack);
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
    if (path.empty())  // ��ΰ� ����ִ� ���
    {
        if (curState == Attack)return;
        SetState(Idle); // ������ �ֱ�
        return;
    }

    // ������ ���� �ִ�
    SetState(Run);  // �����̱� + �޸��� ���� ����



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
}

void Orc::EndDying()
{
}
