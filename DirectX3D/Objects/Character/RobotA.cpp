#include "Framework.h"
#include "RobotA.h"

RobotA::RobotA() : ModelAnimator("Robot")
{
    ReadClip("StandUp"); //�κ��� ���¿� ���� �ε��� ���� ���߱�
    ReadClip("Walk");
    ReadClip("Run");
    ReadClip("Hit");
    ReadClip("Dying");

    PlayClip(curState);
    Pos() = { 70, 0, 70 };
    Scale() *= 0.1f;
}

RobotA::~RobotA()
{
}

void RobotA::Update()
{
    Control();
    Move();
    Rotate();

    ModelAnimator::Update();
}

void RobotA::Render()
{
    ModelAnimator::Render();
}

void RobotA::Control()
{
    if (searchCoolDown > 1)
    {
        target->GlobalPos();
        // �ͷ��� ������ ���� ����� ��ǻƮ��ŷ����

        if (aStar->IsCollisionObstacle(GlobalPos(), target->GlobalPos())) // �߰��� ��ֹ��� ������
        {
            SetPath(); // ��ü���� ��� ��� ����
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

void RobotA::Move()
{
    if (path.empty())
    {
        SetState(Walk);
        return;
    }

    SetState(Walk);

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

void RobotA::Rotate()
{
    if (curState == StandUp) return;

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

void RobotA::SetState(State state)
{
    if (state == curState) return;
    curState = state;
    PlayClip(state);
}

void RobotA::SetPath()
{
    int startIndex = aStar->FindCloseNode(GlobalPos());
    int endIndex = aStar->FindCloseNode(target->GlobalPos()); // �������(+������Ʈ����) ������ ������

    aStar->GetPath(startIndex, endIndex, path); // ���� �� ���� path ���Ϳ� ����

    aStar->MakeDirectionPath(GlobalPos(), target->GlobalPos(), path); // ��ֹ��� ����� path�� ������

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
