#include "Framework.h"

Fox::Fox() : ModelAnimator("Fox")
{
    ReadClip("Idle");
    ReadClip("Run", 0, "FoxTransform");

    PlayClip(curState);
    Pos() = { 45, 0, 45 };
    Scale() *= 0.5f;

    collider = new CapsuleCollider(2);
    collider->SetParent(this);
    collider->Pos() = { 0,15,0 };
    collider->Scale() *= 5;
}

Fox::~Fox()
{
    //지형, A* 모두 밖에서 만들고 지정하므로 여기서 지울 필요 없음
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
    // 노드는 이미 만들어져 있으니 A*는 길만 찾으면 끝
    if (KEY_PRESS(VK_LSHIFT) && KEY_DOWN(VK_LBUTTON))
    {
        if (terrain->ComputePicking(destPos) == false) return;
        // 터레인 에디터쓰려면 computePicking 으로

        if (aStar->IsCollisionObstacle(GlobalPos(), destPos))// 중간에 장애물이 있으면
        {
            SetPath(); // 구체적인 경로 내어서 가기
        }
        else
        {
            path.clear(); // 굳이 장애물없는데 길찾기 필요 x
            path.push_back(destPos); // 가야할 곳만 경로에 집어넣기
            // ->여우는 Move로 목적지를 찾아갈 것
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

    direction.y = 0; // 필요하면 지형의 높이(굴곡) 반영을 해줄 수도 있고,
    // 역시 필요하면 그냥 좌우회전만 하는 걸로 (y 방향 일부러 주지 않음)

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
    int endIndex = aStar->FindCloseNode(destPos); // 헤더에서(+업데이트에서) 정해진 목적지

    aStar->GetPath(startIndex, endIndex, path); // 길을 낸 다음 path 벡터에 저장

    aStar->MakeDirectionPath(GlobalPos(), destPos, path); // 장애물을 지우고 path에 덮어씌우기

    UINT pathSize = path.size(); // 처음 나온 경로 벡터 크기를 저장

    while (path.size() > 2) // "남겨진" 경로 노드가 1군데 이하가 될 때까지
    {
        vector<Vector3> tempPath = path; // 계산용 임시 경로 받아오기
        tempPath.erase(tempPath.begin()); // 최종 목적지 지우기 (장애물이 있었기 때문에 지금은 중간을 가는 중)
        tempPath.pop_back(); // 시작 위치도 생각에서 제외하기 (이미 지나가고 있으니까)

        // 최종과 시작이 빠진 경로의 새로운 시작과 끝을 내기
        Vector3 start = path.back();
        Vector3 end = path.front();

        //다시 나온 경로에서 장애물 계산을 또 계산
        aStar->MakeDirectionPath(start, end, tempPath);

        //계산 결과 피드백
        path.clear();
        path = tempPath;

        //경로 벡터에 새로운 시작과 끝을 포함
        path.insert(path.begin(), end);
        path.push_back(start);

        // 계산을 다시 했는데 벡터 크기가 그대로라면 = 길이 바뀌지 않았다
        if (pathSize == path.size()) break; // 이 이상 계산을 계속할 이유가 없다
        else pathSize = path.size(); // 줄어든 경로 계산을 반영을 해주고 반복문을 다시 진행
    }

    // 다시 조정된, 내가 갈 수 있는 경로에, 최종 목적지를 다시 한번 추가한다
    path.insert(path.begin(), destPos);


    //직선거리일때 한칸한칸이동할 필요가 없다 -> 장애물 전까지는 하나의 벡터로 가도 된다 ->MakeDirectionPath를 쓰는이유
}