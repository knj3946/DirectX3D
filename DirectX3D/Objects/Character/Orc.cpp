#include "Framework.h"

Orc::Orc(Transform* transform, ModelAnimatorInstancing* instancing, UINT index)
    :transform(transform), instancing(instancing), index(index)
{
    //클립 생성해두기 
    string modelName = "Orc";

    root = new Transform();

    transform->Pos() = startPos;

    motion = instancing->GetMotion(index);
    totalEvent.resize(instancing->GetClipSize()); //모델이 가진 동작 숫자만큼 이벤트 리사이징
    eventIters.resize(instancing->GetClipSize());

    //이벤트 세팅
    SetEvent(HIT, bind(&Orc::EndHit, this), 0.99f);
    SetEvent(DYING, bind(&Orc::EndDying, this), 0.9f);

    FOR(totalEvent.size())
    {
        eventIters[i] = totalEvent[i].begin(); // 등록되어 있을 이벤트의 시작지점으로 반복자 설정
    }

    //충돌체
    collider = new CapsuleCollider(30, 120);
    collider->SetParent(transform);
    //collider->Rot().z = XM_PIDIV2 - 0.2f;
    collider->Pos() = { -15, 80, 0 };
    collider->SetActive(true);

    // 무기 충돌체
    mainHand = new Transform();
    tmpCollider = new SphereCollider();
    tmpCollider->Scale() *= 0.3;
    tmpCollider->SetParent(mainHand); // 임시로 만든 충돌체를 "손" 트랜스폼에 주기


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

    // TODO : 무기들 애니메이션 맞춰서 움직이게, 몸통콜라이더도
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
                    wateTime = 1.0f; //ATTACK 애니메이션이 끝나고 이동해야 하기 때문
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
    //// 아직 안 죽었으면 산 로봇답게 맞는 동작 수행
    //curState = HIT;
    //instancing->PlayClip(index, HIT);
    //eventIters[HIT] = totalEvent[HIT].begin();
}

void Orc::Spawn(Vector3 pos)
{
    transform->SetActive(true); //비활성화였다면 활성화 시작
    collider->SetActive(true);

    SetState(IDLE); // 가볍게 산책

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

        // 시야에 들어왔다면
        if (bDetection)
        {
            // 발견한지 2초가 됐나
            if (bFind)
            {
                // 공격 사정거리안에 들지 못하면
                if (dist.Length() > 5.0f)
                {

                    if (aStar->IsCollisionObstacle(transform->GlobalPos(), target->GlobalPos()))// 중간에 장애물이 있으면
                    {
                        SetPath(target->Pos()); // 구체적인 경로 내어서 가기
                    }
                    else
                    {
                        path.clear(); // 굳이 장애물없는데 길찾기 필요 x
                        path.push_back(target->GlobalPos()); // 가야할 곳만 경로에 집어넣기
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
                // 천천히 그쪽으로 걸어간다.
                //Move();
            }
        }
        // 시야에 안보인다면
        else
        {
            // 보였던 target이 중간에 사라졌다.
            if (bFind)
            {
                // 가장 최근 보였던 곳으로 이동
                // 놔두면 자동으로 최근까지 이동한다. 
                if (path.empty())
                {
                    // 경로가 비었다면 가장 최근 target 위치까지 이동한 것.
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

                if (aStar->IsCollisionObstacle(transform->GlobalPos(), startPos))// 중간에 장애물이 있으면
                {
                    SetPath(startPos);
                }
                else
                {
                    path.clear(); // 굳이 장애물없는데 길찾기 필요 x
                    path.push_back(startPos); // 가야할 곳만 경로에 집어넣기
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
        // 가야할 곳이 있다
        //SetState(RUN);  // 움직이기 + 달리는 동작 실행

        //벡터로 들어온 경로를 하나씩 찾아가면서 움직이기

        Vector3 dest = path.back(); // 나에게 이르는 경로의 마지막
        // = 목적지로 가기 위해 가는 최초의 목적지
        // 경로가 왜 거꾸로냐? -> 길찾기 알고리즘에서 
        // 경로를 위한 데이터 작성은 처음부터, 
        // 검산과 경로 추가는 뒤에서부터 했기 때문

        Vector3 direction = dest - transform->Pos(); // 내 위치에서 목적지로 가기위한 벡터

        direction.y = 0; // 필요하면 지형의높이 반영을 해줄 수도 있고,
        // 역시 필요하면 그냥 좌우회전만 하는걸로 (y방향 일부러 주지 않음)

        if (direction.Length() < 0.5f)  // 대충 다 왔으면 (업데이트 호출 과정에서 계속 갱신)
        {
            path.pop_back(); // 다 온 목적지를 벡터에서 빼기
        }

        // 목적지로 가기 위한 실제 이동
        velocity = direction.GetNormalized(); // 속력기준 (방향의 정규화)
        transform->Pos() += velocity * moveSpeed * DELTA; // 속력기준 * 실제속력 * 시간경과

        //transform->Pos() += velocity.GetNormalized() * speed * DELTA;
        transform->Rot().y = atan2(velocity.x, velocity.z) + XM_PI; // XY좌표 방향 + 전후반전(문워크 방지)
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
    if (state == curState) return; // 이미 그 상태라면 굳이 변환 필요 없음

    curState = state; //매개변수에 따라 상태 변화
    instancing->PlayClip(index, (int)state); //인스턴싱 내 자기 트랜스폼에서 동작 수행 시작
    eventIters[state] = totalEvent[state].begin(); //이벤트 반복자도 등록된 이벤트 시작시점으로

    // ->이렇게 상태와 동작을 하나로 통합해두면
    // ->이 캐릭터는 상태만 바꿔주면 행동은 그때그때 알아서 바꿔준다
}

void Orc::SetPath(Vector3 targetPos)
{
    int startIndex = aStar->FindCloseNode(transform->GlobalPos());
    int endIndex = aStar->FindCloseNode(targetPos); // 헤더에서(+업데이트에서) 정해진 목적지

    aStar->GetPath(startIndex, endIndex, path); // 길을 낸 다음 path 벡터에 저장

    aStar->MakeDirectionPath(transform->GlobalPos(), targetPos, path); // 장애물을 지우고 path에 덮어씌우기

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
    path.insert(path.begin(), target->GlobalPos());


    //직선거리일때 한칸한칸이동할 필요가 없다 -> 장애물 전까지는 하나의 벡터로 가도 된다 ->MakeDirectionPath를 쓰는이유
}

void Orc::SetEvent(int clip, Event event, float timeRatio)
{
    if (totalEvent[clip].count(timeRatio) > 0) return; // 선행 예약된 이벤트가 있으면 종료
    totalEvent[clip][timeRatio] = event;
}

void Orc::ExecuteEvent()
{
    int index = curState; //현재 상태 받아오기
    if (totalEvent[index].empty()) return;
    if (eventIters[index] == totalEvent[index].end()) return;

    float ratio = motion->runningTime / motion->duration; //진행된 시간 나누기 전체 진행시간

    if (eventIters[index]->first > ratio) return; // 진행 시간이 정해진 기준에 못 미치면 종료(재시작)

    eventIters[index]->second(); //등록된 이벤트 수행
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
                //발각
            //    for (UINT i = 0; i < RobotManager::Get()->GetCollider().size(); ++i) {
            //        if (RobotManager::Get()->GetCollider()[i]->IsRayCollision(ray))
            //        {
            //            return;
            //        }
            //
            //    }
                // 추후 오크매니저에서 씬에 깔린 모든 벽들 체크해서 ray충돌페크
                // behaviorstate = NPC_BehaviorState::DETECT;
                bDetection = true;
            }
            else {
                if (Enemytothisangle > 0) {
                    Enemytothisangle += 360;
                }

                if (leftdir1 <= Enemytothisangle && rightdir1 >= Enemytothisangle) {
                    //발각
                    //    for (UINT i = 0; i < RobotManager::Get()->GetCollider().size(); ++i) {
            //        if (RobotManager::Get()->GetCollider()[i]->IsRayCollision(ray))
            //        {
            //            return;
            //        }
            //
            //    }
                // 추후 오크매니저에서 씬에 깔린 모든 벽들 체크해서 ray충돌페크
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
    // 웅크리고 걷는 소리 ,암살소리  제외
    // 플레이어 소리와 주위 시선 끄는 소리 추가




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