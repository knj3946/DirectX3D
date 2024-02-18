#include "Framework.h"

Fox::Fox() : ModelAnimator("Fox")
{
    ReadClip("Idle");
    ReadClip("Run", 0, "FoxTransform");

    PlayClip(curState);
    Pos() = { 65, 0, 65 };
    Scale() *= 0.5f;

    collider = new CapsuleCollider(2);
    collider->SetParent(this);
    collider->Pos() = { 0,15,0 };
    collider->Scale() *= 5;
}

Fox::~Fox()
{
    //����, A* ��� �ۿ��� ����� �����ϹǷ� ���⼭ ���� �ʿ� ����
}

void Fox::Update()
{
    Control();
    Move();
    Rotate();
    collider->UpdateWorld();

    ModelAnimator::Update();
}

void Fox::Render()
{
    collider->Render();
    ModelAnimator::Render();
}

void Fox::Control()
{
    // ���� �̹� ������� ������ A*�� �游 ã���� ��
    if (KEY_PRESS(VK_LSHIFT) && KEY_DOWN(VK_LBUTTON))
    {
        if (terrain->ComputePicking(destPos) == false) return;
        // �ͷ��� �����;����� computePicking ����

        if (aStar->IsCollisionObstacle(GlobalPos(), destPos))// �߰��� ��ֹ��� ������
        {
            SetPath(); // ��ü���� ��� ��� ����
        }
        else
        {
            path.clear(); // ���� ��ֹ����µ� ��ã�� �ʿ� x
            path.push_back(destPos); // ������ ���� ��ο� ����ֱ�
            // ->����� Move�� �������� ã�ư� ��
        }
    }
}

void Fox::Move()
{
    if (path.empty())
    {
        SetState(IDLE);
        return;
    }

    SetState(RUN);

    Vector3 dest = path.back();

    Vector3 direction = dest - GlobalPos();

    direction.y = 0; // �ʿ��ϸ� ������ ����(����) �ݿ��� ���� ���� �ְ�,
    // ���� �ʿ��ϸ� �׳� �¿�ȸ���� �ϴ� �ɷ� (y ���� �Ϻη� ���� ����)

    if (direction.Length() < 0.5f)
    {
        path.pop_back();
    }

    velocity = direction.GetNormalized();
    Pos() += velocity * moveSpeed * DELTA;
}

void Fox::Rotate()
{
    if (curState == IDLE) return;

    Vector3 forward = Forward();
    Vector3 cross = Cross(forward, velocity);

    if (cross.y < 0)
    {
        Rot().y += rotSpeed * DELTA;
    }
    else if (cross.y > 0)
    {
        Rot().y -= rotSpeed * DELTA;
    }
}

void Fox::SetState(State state)
{
    if (state == curState) return;
    curState = state;
    PlayClip(state);
}

void Fox::SetPath()
{
    int startIndex = aStar->FindCloseNode(GlobalPos());
    int endIndex = aStar->FindCloseNode(destPos); // �������(+������Ʈ����) ������ ������

    aStar->GetPath(startIndex, endIndex, path); // ���� �� ���� path ���Ϳ� ����

    aStar->MakeDirectionPath(GlobalPos(), destPos, path); // ��ֹ��� ����� path�� ������

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
    path.insert(path.begin(), destPos);


    //�����Ÿ��϶� ��ĭ��ĭ�̵��� �ʿ䰡 ���� -> ��ֹ� �������� �ϳ��� ���ͷ� ���� �ȴ� ->MakeDirectionPath�� ��������
}