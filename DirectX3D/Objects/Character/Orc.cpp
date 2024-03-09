#include "Framework.h"
Orc::Orc(Transform* transform, ModelAnimatorInstancing* instancing, UINT index)
    :transform(transform), instancing(instancing), index(index)
{
    //클립 생성해두기 
    string modelName = "Orc";

    //충돌체
    collider = new CapsuleCollider(50, 120);
    collider->SetParent(transform);
    //collider->Rot().z = XM_PIDIV2 - 0.2f;
    collider->Pos() = { -15, 80, 0 };
    collider->SetActive(false); //spawn 할때 활성화

    // 무기 충돌체
    leftHand = new Transform();
    leftWeaponCollider = new CapsuleCollider(8, 50);
    leftWeaponCollider->Pos().y += 20;
    leftWeaponCollider->SetParent(leftHand); // 임시로 만든 충돌체를 "손" 트랜스폼에 주기
    leftWeaponCollider->SetActive(false); //attack 할때 활성화

    weaponColliders.push_back(leftWeaponCollider);

    rightHand = new Transform();
    rightWeaponCollider = new CapsuleCollider(6, 30);
    rightWeaponCollider->Pos().y += 20;
    rightWeaponCollider->SetParent(rightHand); // 임시로 만든 충돌체를 "손" 트랜스폼에 주기
    rightWeaponCollider->SetActive(false); //attack 할때 활성화

    weaponColliders.push_back(rightWeaponCollider);

 
    motion = instancing->GetMotion(index);
    totalEvent.resize(instancing->GetClipSize()); //모델이 가진 동작 숫자만큼 이벤트 리사이징
    eventIters.resize(instancing->GetClipSize());

    //이벤트 세팅
    SetEvent(ATTACK, bind(&Orc::EndAttack, this), 0.99f);
    SetEvent(THROW, bind(&Orc::Throw, this), 0.59f);
    SetEvent(HIT, bind(&Orc::EndHit, this), 0.99f);
    SetEvent(ASSASSINATED, bind(&Orc::EndAssassinated, this), 0.9f);
    SetEvent(DYING, bind(&Orc::EndDying, this), 0.9f);

    SetEvent(ATTACK,bind(&Collider::SetActive, leftWeaponCollider, true), 0.11f); //콜라이더 켜는 시점 설정
    SetEvent(ATTACK,bind(&Collider::SetActive, rightWeaponCollider, true), 0.12f); //콜라이더 켜는 시점 설정

    SetEvent(ATTACK,bind(&Collider::SetActive, leftWeaponCollider, false), 0.98f); //콜라이더 꺼지는 시점 설정
    SetEvent(ATTACK,bind(&Collider::SetActive, rightWeaponCollider, false), 0.99f); //콜라이더 꺼지는 시점 설정

    FOR(totalEvent.size())
    {
        eventIters[i] = totalEvent[i].begin(); // 등록되어 있을 이벤트의 시작지점으로 반복자 설정
    }

    // hp UI
    hpBar = new ProgressBar(L"Textures/UI/hp_bar.png", L"Textures/UI/hp_bar_BG.png");
    
    hpBar->SetAmount(curHP / maxHp);
    hpBar->Pos() = transform->Pos() + Vector3(0, 6, 0);
    Vector3 dir = hpBar->Pos() - CAM->Pos();
    hpBar->Rot().y = atan2(dir.x, dir.z);
    hpBar->UpdateWorld();
    hpBar->Render();

    exclamationMark = new Quad(L"Textures/UI/Exclamationmark.png");
    questionMark = new Quad(L"Textures/UI/QuestionMark.png");
    exclamationMark->Scale() *= 0.1f;
    questionMark->Scale() *= 0.1f;

    questionMark->SetActive(false);
    exclamationMark->SetActive(false);
    //aStar = new AStar(512, 512);
    
    //aStar->SetNode();

    computeShader = Shader::AddCS(L"Compute/ComputePicking.hlsl");
    rayBuffer = new RayBuffer();

 //   particleHit = new ParticleSystem("TextData/Particles/SlowHit.fx");

    rangeBar = new ProgressBar(L"Textures/UI/Range_bar.png", L"Textures/UI/Range_bar_BG.png");
    rangeBar->SetAmount(DetectionStartTime / DetectionEndTime);
    rangeBar->SetParent(transform);

    rangeBar->Rot() = { XMConvertToRadians(90.f),0,XMConvertToRadians(-90.f) };
    rangeBar->Pos() = { -15.f,2.f,-650.f };

    particleHit = new Sprite(L"Textures/Effect/HitEffect.png", 50, 50, 5, 2, false);
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
    delete exclamationMark;
    delete questionMark;
    delete rangeBar;
    delete hpBar;
    for (Collider* wcollider : weaponColliders)
        delete wcollider;
    
    delete particleHit;
    delete transform;
}

void Orc::SetType(NPC_TYPE _type) {

    switch (_type)
    {
    case Orc::NPC_TYPE::ATTACK:// 오크 타입에 따라 탐지 범위 지정 (현재 임시)
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
    if (!transform->Active()) return; //활성화 객체가 아니면 리턴

    
    if (curState != DYING || curState != ASSASSINATED)
    {
        Direction();// 방향지정 함수
        CalculateEyeSight(); //시야에 발각됬는지 확인하는 함수 (bDetection 설정)
        CalculateEarSight(); //소리가 들렸는지 확인하는 함수
        Detection(); //플레이어를 인지했는지 확인하는 함수
        RangeCheck(); //발견되었다가 사라진 플레이어 탐지
    }
    
    TimeCalculator(); //공격 간격을 두기 위한 설정
    ParticleUpdate(); //파티클이펙트 업데이트
    UpdateUI(); //UI 업데이트
    ExecuteEvent(); //이벤트 있으면 실행

    if (curState == DYING || curState == ASSASSINATED)
        return;

    PartsUpdate(); //모델 각 파츠 업데이트
    StateRevision(); //애니메이션 중간에 끊겨서 변경안된 값들 보정
    

    //====================== 이동관련==============================
    if (CalculateHit()) return; //맞는 중이면 리턴 (이 아래는 이동과 관련된 것인데 맞는중에는 필요없음)
    if (!GetDutyFlag()) //해야할일(움직임)이 생겼는지 확인
    {
        //해야할일이 없을때
        SetState(IDLE);
        //IdleAIMove();
        return;
    }

    SetParameter(); //필요한 변수들 세팅
    Control(); // 경로설정등 오크가 움직이기 위한 정보들 계산
    Move(); //실제 움직임
    RotationRestore();

    CoolDown();
    
}

void Orc::Render()
{
    collider->Render();
    leftWeaponCollider->Render();
    rightWeaponCollider->Render();
    hpBar->Render();
    if(behaviorstate != NPC_BehaviorState::DETECT)
        rangeBar->Render();
    //aStar->Render();

    particleHit->Render();
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

    vector<VertexType> vertices = terrain->GetMesh()->GetVertices();
    vector<UINT> indices = terrain->GetMesh()->GetIndices();

    terrainTriangleSize = indices.size() / 3;

    inputs.resize(terrainTriangleSize);
    outputs.resize(terrainTriangleSize);

    for (UINT i = 0; i < terrainTriangleSize; i++)
    {
        UINT index0 = indices[i * 3 + 0];
        UINT index1 = indices[i * 3 + 1];
        UINT index2 = indices[i * 3 + 2];

        inputs[i].v0 = vertices[index0].pos;
        inputs[i].v1 = vertices[index1].pos;
        inputs[i].v2 = vertices[index2].pos;
    }

    structuredBuffer = new StructuredBuffer(
        inputs.data(), sizeof(InputDesc), terrainTriangleSize,
        sizeof(OutputDesc), terrainTriangleSize);
   
}


void Orc::SetSRT(Vector3 scale, Vector3 rot, Vector3 pos)
{
    transform->Scale() = scale;
    transform->Rot() = rot;
    transform->Pos() = pos;

    transform->UpdateWorld();

    rangeBar->Scale() = { 1.f / scale.x,1.f / scale.y,1.f / scale.z };
    rangeBar->Scale() *= (eyeSightRange / 100);
}

void Orc::GUIRender()
{
    ImGui::Text("OrcWalkVolume : %f", ORCSOUND(index)->GetVolume("Orc_Walk"));
    ImGui::Text("earCal : %d", bSound);

    ImGui::Text("bFind : %d", bFind);
    ImGui::Text("bDetection : %d", bDetection);
    ImGui::Text("isTracking : %d", isTracking);
    ImGui::Text("path.empty() : %d", path.empty());
    ImGui::Text("missTargetTrigger : %d", missTargetTrigger);
    ImGui::Text("NPC_BehaviorState : %d", behaviorstate);
    ImGui::Text("curState : %d", curState);

    ImGui::Text("FeedbackPosY : %f", feedBackPos.y);
    ImGui::Text("eyeSightRange : %f", eyeSightRange);
    //ImGui::Text("curhp : %f", curHP);
    //ImGui::Text("desthp : %f", destHP);

    rangeBar->GUIRender();

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

    // 방향지정
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

void Orc::Throw()
{
    Vector3 dir = target->Pos() - transform->Pos();
    dir = dir.GetNormalized();
    KunaiManager::Get()->Throw(transform->Pos()+Vector3(0,3,0), dir);
    SetState(IDLE);
}

bool Orc::GetDutyFlag()
{
    if (isTracking == false && path.empty())
        return false;

    return true;
}

void Orc::SetParameter()
{
    SetGroundPos(); //현재 서있는 땅 위치 계산
}

void Orc::SetGroundPos()
{
    if (!OnColliderFloor(feedBackPos)) // 문턱올라가기 때문
    {
        if (curRayCoolTime <= 0.f)
        {
            Vector3 OrcSkyPos = transform->Pos();
            OrcSkyPos.y += 100;
            Ray groundRay = Ray(OrcSkyPos, Vector3(transform->Down()));
            TerainComputePicking(feedBackPos, groundRay);
        }
    }
}

bool Orc::CalculateHit()
{
    if (isHit)
    {
        SetState(HIT);

        if (!bFind)
        {
            // 탐지 안된 상태에서 맞았을경우 바로 찾기
            bDetection = true;
            isTracking = true;
            bFind = true;
            DetectionStartTime = DetectionEndTime;
            behaviorstate = NPC_BehaviorState::DETECT;

            Vector3 dir = (target->GlobalPos() - transform->GlobalPos()).GetNormalized();
            float rotY = atan2(dir.x, dir.z)+XM_PI;
            transform->Rot().y = rotY;
            transform->UpdateWorld();
        }

        if (curHP <= destHP)
        {
            isHit = false;
            collider->SetActive(true);
            leftWeaponCollider->SetActive(true);
            rightWeaponCollider->SetActive(true);
            SetState(IDLE);
        }

        return true; 
    }
    else
        return false;
}

void Orc::PartsUpdate()
{
    if (transform->Active()==false || isDying == true)return;
    transform->SetWorld(instancing->GetTransformByNode(index, 5));
    collider->UpdateWorld();
    transform->UpdateWorld();

    leftHand->SetWorld(instancing->GetTransformByNode(index, 170));//170
    leftWeaponCollider->UpdateWorld();

    rightHand->SetWorld(instancing->GetTransformByNode(index, 187));//187
    rightWeaponCollider->UpdateWorld();
}

void Orc::StateRevision()
{
    if (curState != ATTACK)
    {
        leftWeaponCollider->SetActive(false);
        rightWeaponCollider->SetActive(false);
    }
}

void Orc::ParticleUpdate()
{
    particleHit->Update();
}

void Orc::RotationRestore()
{
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

float Orc::GetDamage()
{
    float r = 0.0f;
    if (curState == ATTACK)
    {
        r = 10.0f;// 임시로 바꿈 10 -> 60
    }
    return r;
}

void Orc::Hit(float damage,Vector3 collisionPos)
{
    if (!isHit)
    {
        if (!ORCSOUND(index)->IsPlaySound("Orc_Hit"))
        {
            float distance = Distance(target->Pos(), transform->Pos());
            distance = (distance < 30) ? distance : 0;
            ORCSOUND(index)->Play("Orc_Hit", 30 - distance); // 크기조절 가까울수록 사운드 커지게
            
        }
        destHP = (curHP - damage > 0) ? curHP - damage : 0;

        collider->SetActive(false);
        leftWeaponCollider->SetActive(false);
        rightWeaponCollider->SetActive(false);
        if (destHP <= 0)
        {
            isDying = true;
            SetState(DYING);
            return;
        }

        SetState(HIT);

        isHit = true;

    

        particleHit->Play(collider->GetCollisionPoint()); // 해당위치에서 파티클 재생
    }

}

void Orc::Spawn(Vector3 pos)
{
    SetState(IDLE); // 가볍게 산책

    curHP = maxHp;
    hpBar->SetAmount(curHP / maxHp);

    transform->Pos() = pos;

    transform->SetActive(true); //비활성화였다면 활성화 시작
    collider->SetActive(true);
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
    // 탐지시 범위에 닿은 애에게 설정
    bFind = true; bDetection = true;
    DetectionStartTime = DetectionEndTime;
    isTracking = true;
    SetState(RUN);
    if (aStar->IsCollisionObstacle(transform->GlobalPos(), target->GlobalPos()))// 중간에 장애물이 있으면
    {
        SetPath(target->GlobalPos()); // 구체적인 경로 내어서 가기
    }
    else
    {
        path.clear(); // 굳이 장애물없는데 길찾기 필요 x
        path.push_back(target->GlobalPos()); // 가야할 곳만 경로에 집어넣기
    }
    behaviorstate = NPC_BehaviorState::DETECT;
}

void Orc::Assassinated(Vector3 collisionPos,Transform* attackerTrf)
{
    transform->Rot() = attackerTrf->Rot();
    transform->UpdateWorld();
   
    float dis = Distance(transform->GlobalPos(), attackerTrf->GlobalPos());

    if (dis < 4.f)
    {
        transform->Pos() += transform->Back() * (4 - dis);
    }
    transform->UpdateWorld();

    instancing->SetOutLine(index, false);
    //MonsterManager::Get()->specialKeyUI["assassination"].active = false;
    //MonsterManager::Get()->specialKeyUI["assassination"].quad->UpdateWorld();

    //암살 애니메이션 재생
    SetState(ASSASSINATED, 0.3f);
    isAssassinated = true; // SetState에서 암살로 죽는 애니메이션 실행

    Vector3 pos = transform->GlobalPos();
    pos.y += 5;
    Hit(120, pos);

    PartsUpdate();
}

void Orc::CoolDown()
{
    if (curRayCoolTime <= 0.0f)
    {
        curRayCoolTime = rayCoolTime;
    }
    else
        curRayCoolTime -= DELTA;
}

void Orc::Control()
{
    if (behaviorstate == NPC_BehaviorState::CHECK)return;
    if (behaviorstate == NPC_BehaviorState::SOUNDCHECK)return;
    if (curState == DYING)return;
    if (searchCoolDown > 1)
    {
        Vector3 dist = target->Pos() - transform->GlobalPos();

        // 시야에 들어왔다면
        if (bDetection )
        {
            // 발견한지 2초가 됐나
            if (bFind)
            {
                // 공격 사정거리안에 들지 못하면
                if (dist.Length() > 4.0f)
                {
                    // 범위 15안에 들고 플레이어가 일정 높이 이상이면 쿠나이 던지기
                    /*건물올라갔을때높이가 9.4라 9로설정 */
                    if (dist.Length() < 35.f && target->Pos().y > 9) // THROW 범위 : 5~15 임시 설정
                    {
                        SetState(THROW);
                        return; // 던질 수 있는데 굳이 앞으로 달려가는거 방지
                    }
                    //else
                    {
                        /*
                        if (aStar->IsCollisionObstacle(transform->GlobalPos(), target->GlobalPos()))// 중간에 장애물이 있으면
                        {
                            SetPath(target->GlobalPos()); // 구체적인 경로 내어서 가기
                        }
                        else
                        {
                            path.clear(); // 굳이 장애물없는데 길찾기 필요 x
                            path.push_back(target->GlobalPos()); // 가야할 곳만 경로에 집어넣기
                        }
                        */

                        //직선레이에 장애물이 탐지되지 않아도 몸통 콜라이더가 걸려서 못갈수도 있기 때문에 항상 경로 내기
                        SetPath(target->GlobalPos()); // 구체적인 경로 내어서 가기
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
            }
            else
            {
                //if (!missTargetTrigger)
             //   {
             //       missTargetTrigger = true;
             ////       DetectionStartTime = 2.0f;
             //       //path.clear();
             //   }


                //직선레이에 장애물이 탐지되지 않아도 몸통 콜라이더가 걸려서 못갈수도 있기 때문에 항상 경로 내기
                SetPath(PatrolPos[nextPatrol]); // 구체적인 경로 내어서 가기

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

    if (behaviorstate == NPC_BehaviorState::DETECT)
    {
        if (Distance(target->GlobalPos(),transform->GlobalPos()) >= 4) //공격텀일때는 IDLE 상태임
        {
            if(curState == IDLE)
                SetState(RUN);
        }
        else
        {
            SetState(ATTACK);
        }
    }
    
    
    float f=velocity.Length();
    if (curState == IDLE) return; 
    if (curState == ATTACK) return;
    if (curState == THROW) return;
    if (curState == HIT) return;
    if (curState == DYING) return;
    if (behaviorstate == NPC_BehaviorState::CHECK)return;
    if (velocity.Length() < 0.1f&&!missTargetTrigger) return;
    if (velocity.Length() < 0.1f && missTargetTrigger) return;

    if (!path.empty())
    {
        // 가야할 곳이 있다
     
        if(behaviorstate!=NPC_BehaviorState::SOUNDCHECK&&!bSound)
            SetState(RUN);  // 움직이기 + 달리는 동작 실행
        else
            SetState(WALK);
        //벡터로 들어온 경로를 하나씩 찾아가면서 움직이기

        Vector3 dest = path.back(); // 나에게 이르는 경로의 마지막
        // = 목적지로 가기 위해 가는 최초의 목적지
        // 경로가 왜 거꾸로냐? -> 길찾기 알고리즘에서 
        // 경로를 위한 데이터 작성은 처음부터, 
        // 검산과 경로 추가는 뒤에서부터 했기 때문

        Vector3 direction = dest - transform->Pos(); // 내 위치에서 목적지로 가기위한 벡터

        direction.y = 0; // 필요하면 지형의높이 반영을 해줄 수도 있고,
        // 역시 필요하면 그냥 좌우회전만 하는걸로 (y방향 일부러 주지 않음)

        if (direction.Length() < 0.1f)  // 대충 다 왔으면 (업데이트 호출 과정에서 계속 갱신)
        {
            path.pop_back(); // 다 온 목적지를 벡터에서 빼기
        }

        // 목적지로 가기 위한 실제 이동
        velocity = direction.GetNormalized(); // 속력기준 (방향의 정규화)

        //transform->Pos() += velocity.GetNormalized() * speed * DELTA;
        transform->Rot().y = atan2(velocity.x, velocity.z) + XM_PI; // XY좌표 방향 + 전후반전(문워크 방지)
      

        //지형 오르기
        
        Vector3 destFeedBackPos;
        Vector3 destPos = transform->GlobalPos() + velocity * moveSpeed * DELTA;
        Vector3 OrcSkyPos = destPos;
        OrcSkyPos.y += 100;
        Ray groundRay = Ray(OrcSkyPos, Vector3(transform->Down()));
        if (!OnColliderFloor(destFeedBackPos)) // 문턱올라가기 때문
        {
            if (curRayCoolTime <= 0.0f)
            {
                TerainComputePicking(destFeedBackPos, groundRay);
            }
        }

        //destFeedBackPos : 목적지 터레인Pos
        //feedBackPos : 현재 터레인Pos

        //방향으로 각도 구하기
        Vector3 destDir = destFeedBackPos - feedBackPos;
        Vector3 destDirXZ = destDir;
        destDirXZ.y = 0;

        //각도
        float radianHeightAngle = acos(abs(destDirXZ.Length()) / abs(destDir.Length()));

        transform->Pos() += velocity * moveSpeed * DELTA; // 이동 수행
        feedBackPos.y = destFeedBackPos.y;
        transform->Pos().y = feedBackPos.y;
    }
}


void Orc::IdleAIMove()
{
    // WALK애니메이션 해결 -> Orc_Walk0.clip 대신 character1@walk30.clip 사용할 것

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

    if (isHit||isDying)
    {
        if (isDying)
            curHP -= DELTA * 30 * 7;
        else 
            curHP -= DELTA * 30 * 2;

        if (curHP <= destHP)
        {
            curHP = destHP;
        }
        hpBar->SetAmount(curHP / maxHp);
    }

    hpBar->Pos() = transform->Pos() + Vector3(0, 6, 0);
    Vector3 dir = hpBar->Pos() - CAM->Pos();
    hpBar->Rot().y = atan2(dir.x, dir.z);

    float scale = 1 / (target->GlobalPos() - transform->GlobalPos()).Length();
    scale = Clamp(0.01f, 0.02f, scale);
    //scale *= 0.03f;
    hpBar->Scale() = { scale, scale, scale };
    
    hpBar->UpdateWorld();

    rangeBar->UpdateWorld();
}

void Orc::TimeCalculator()
{
    // false 됐을 때 동작 시작
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
    if (state == curState) return; // 이미 그 상태라면 굳이 변환 필요 없음

    // 테스트
    if (state == WALK || state == RUN)
    {
        float distance = Distance(target->Pos(), transform->Pos());
        distance = (distance < 30) ? distance : 0;
        if (!ORCSOUND(index)->IsPlaySound("Orc_Walk"))
        {
            ORCSOUND(index)->Play("Orc_Walk", 30 - distance);
        }
        else
        {
            ORCSOUND(index)->SetVolume("Orc_Walk", 30 - distance);
        }
    }
    else
        ORCSOUND(index)->Stop("Orc_Walk");

    // 공격 속도 조절 -> Attack이지만 attackSpeed만큼 시간이 지나지 않았다면 공격 x
    if (state == ATTACK && !isAttackable)
    {
        state = IDLE;
        //return;
    }
    if (state == ATTACK || state == THROW)
    {
        // 방향도 바꾸기
        velocity = target->Pos() - transform->Pos();
        transform->Rot().y = atan2(velocity.x, velocity.z) + XM_PI;
    }
    curState = state; //매개변수에 따라 상태 변화
    if (state == ATTACK)
    {
        int randNum = GameMath::Random(0, 3);
        //인스턴싱 내 자기 트랜스폼에서 동작 수행 시작
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
    else if (curState == DYING)
    {
        if(isAssassinated)
            instancing->PlayClip(index, (int)state + 3, 0.8);
        else
            instancing->PlayClip(index, (int)state + 2, 0.8);
    }
    else
        instancing->PlayClip(index, (int)state, scale); //인스턴싱 내 자기 트랜스폼에서 동작 수행 시작
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
    path.insert(path.begin(), targetPos);


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

void Orc::EndAssassinated()
{
    hpBar->SetActive(false);
    /*Vector3 pos = transform->GlobalPos();
    pos.y += 5;
    Hit(120, pos);*/

    /*instancing->SetOutLine(index, false);
    MonsterManager::Get()->specialKeyUI["assassination"].active = false;
    instancing->Remove(index);*/
}

void Orc::EndDying()
{
    instancing->SetOutLine(index, false);
    MonsterManager::Get()->specialKeyUI["assassination"].active = false;
    ColliderManager::Get()->PopCollision(ColliderManager::Collision_Type::ORC, collider);
    collider->SetActive(false);

    transform->SetActive(false);
    hpBar->SetActive(false);

    rangeBar->SetActive(false);
    collider->SetActive(false);

    leftHand->SetActive(false);
    leftWeaponCollider->SetActive(false);
    questionMark->SetActive(false);
    exclamationMark->SetActive(false);

   // particleHit->Stop();

    /*instancing->SetOutLine(index, false);
    MonsterManager::Get()->specialKeyUI["assassination"].active = false;*/
    instancing->Remove(index);
    //instancing->Remove(index);
    // 삭제 전에 아이템 떨굴거면 여기서
    
    //isDelete = true;
    MonsterManager::Get()->DieOrc(index);
}



void Orc::CalculateEyeSight()
{
    //bDetection = true;
    //return;

    float rad = XMConvertToRadians(eyeSightangle);
    Vector3 front = Vector3(transform->Forward().x, 0, transform->Forward().z).GetNormalized();

    Vector3 eyevector = Vector3(sinf(rad) + transform->GlobalPos().x, 0, cos(rad) + transform->GlobalPos().z);
    Vector3 rightdir = eyevector * eyeSightRange;
    Vector3 leftdir = -eyevector * eyeSightRange;

    Vector3 direction = target->GlobalPos() - transform->GlobalPos();
    direction.Normalize();

    float degree = XMConvertToDegrees(transform->Rot().y);
    degree = degree < 0.0f ? degree + 360.0f : degree;
    while (degree > 360.0f)
        degree -= 360.0f;

    float dirz = transform->Forward().z;
    float rightdir1 = -(180.f - eyeSightangle) + degree + 360;

    bool breverse = false;
    float leftdir1 = (180.f - eyeSightangle) + degree;

    while (rightdir1 > 360.0f)
        rightdir1 -= 360.0f;
    while (leftdir1 > 360.0f)
        leftdir1 -= 360.0f;

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

    if ((Distance(target->GlobalPos(), transform->GlobalPos()) < eyeSightRange)) 
    {
        SetEyePos(); //눈 위치만 설정

        if (!EyesRayToDetectTarget(targetCollider, eyesPos)) //리턴 값 false면 가려서 안보이는 것
        {
            if (bDetection) {
                bDetection = false;
                //DetectionStartTime = 0.001f;
                //missTargetTrigger = true;
            }
            return;
        }
        // 가려서 안보이는건 아니니까 발견상태유지
        if (bFind && behaviorstate == NPC_BehaviorState::DETECT)
        {
            bDetection = true;
            return;
        }

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

        // 추후 오크매니저에서 씬에 깔린 모든 벽들 체크해서 ray충돌페크
        // behaviorstate = NPC_BehaviorState::DETECT;
        bDetection = true;
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
    if (ORCSOUND(index)->IsPlaySound("Player_Move")) {
        
        pos.x = ORCSOUND(index)->GetSoundPos("Player_Move").x;
        pos.y = ORCSOUND(index)->GetSoundPos("Player_Move").y;
        pos.z = ORCSOUND(index)->GetSoundPos("Player_Move").z;
        volume = ORCSOUND(index)->GetVolume("Player_Move");
        distance = Distance(transform->Pos(), pos);
        volume = ORCSOUND(index)->GetVolume("Player_Move");
    }
    // 웅크리고 걷는 소리 ,암살소리  제외
    // 플레이어 소리와 주위 시선 끄는 소리 추가



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
    

    SoundPositionCheck(); //소리가 들렸다면 소리의 위치 확인하는 함수
}

void Orc::Detection()
{

    if (bDetection) {
        if (!bFind)
            DetectionStartTime += DELTA;
    }
    else {
        if (DetectionStartTime > 0.f && path.empty()) {
            DetectionStartTime -= DELTA * 2;
            
        
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
    rangeBar->SetAmount(DetectionStartTime / DetectionEndTime);

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

void Orc::SetEyePos()
{
    eyesPos = transform->GlobalPos();
    eyesPos.y += 5.f;
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

bool Orc::OnColliderFloor(Vector3& feedback)
{
    Vector3 PlayerSkyPos = transform->GlobalPos();
    PlayerSkyPos.y += 3;
    Ray groundRay = Ray(PlayerSkyPos, Vector3(transform->Down()));
    Contact con;
    if (ColliderManager::Get()->CloseRayCollisionColliderContact(groundRay, con))
    {
        feedback = con.hitPoint;
        //feedback.y += 0.1f; //살짝 띄움으로서 충돌 방지
        return true;
    }

    return false;
}

void Orc::RangeCheck()
{
    if (behaviorstate != NPC_BehaviorState::CHECK)return;


    float curdegree = XMConvertToDegrees(transform->Rot().y);//
    if (0 == m_uiRangeCheck)
    {
        float vlaue = rangeDegree + 45.f;
        if (vlaue > 180.f) {
            vlaue -= 180.f;
            vlaue = -180.f + vlaue;

        }
        if (vlaue < curdegree) {// 플레이어를 놓친 후 각도에서 왼쪽으로 45 넘을시
            m_uiRangeCheck++;
        }
    }
    else if (1 == m_uiRangeCheck)
    {

        float vlaue = rangeDegree - 45.f;

        if (vlaue < -180.f) {
            vlaue += 180.f;
            vlaue = 180.f + vlaue;

        }


        if (vlaue > curdegree) {// 플레이어를 놓친 후 각도에서 왼쪽으로 45 넘을시
            m_uiRangeCheck++;
        }
    }
    else if (2 == m_uiRangeCheck) //원래보고있던 각도로 복귀
    {
        float vlaue = rangeDegree;
        if (vlaue > 180.f) {
            vlaue -= 180.f;
            vlaue = -180.f + vlaue;

        }
        if (vlaue < curdegree) {
            m_uiRangeCheck++;
        }
    }

    

    if (m_uiRangeCheck % 2 == 0) {
        transform->Rot().y += DELTA*1.5f;
    }
    else
        transform->Rot().y -= DELTA*1.5f;
    

    if (m_uiRangeCheck == 3)
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

        UINT x = ceil((float)terrainTriangleSize / 1.0f);

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

bool Orc::EyesRayToDetectTarget(Collider* targetCol,Vector3 orcEyesPos)
{
    // 목표 콜라이더 맨위 맨 아래 맨 왼쪽 맨 오른쪽 중앙 5군데만 레이를 쏴서 확인 (0.8을 곱해서 좀 안쪽으로 쏘기)

    if (curRayCoolTime <= 0.f)
    {
        switch (targetCol->GetType())
        {
            case Collider::Type::CAPSULE:
            {
                CapsuleCollider* targetCapsuleCol = static_cast<CapsuleCollider*>(targetCol);

                Vector3 top = targetCapsuleCol->GlobalPos() + (transform->Up() * targetCapsuleCol->Height() * 0.4 + transform->Up() * targetCapsuleCol->Radius() * 0.8);
                Vector3 bottom = targetCapsuleCol->GlobalPos() + (transform->Down() * targetCapsuleCol->Height() * 0.4 + transform->Down() * targetCapsuleCol->Radius() * 0.8);
                Vector3 left = targetCapsuleCol->GlobalPos() + (transform->Right() * targetCapsuleCol->Radius() * 0.8);
                Vector3 right = targetCapsuleCol->GlobalPos() + (transform->Left() * targetCapsuleCol->Radius() * 0.8);
                Vector3 center = targetCapsuleCol->GlobalPos();

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
                    //모든 부분에서 엄폐되었다
                    isRayToDetectTarget = false;
                }
                else
                {
                    isRayToDetectTarget = true;
                }

            }
            default:
            {
            }
        }
    }

    return isRayToDetectTarget;
}

void Orc::SetOutLine(bool flag)
{
    if (transform->Active() == false)return;
    outLine = flag;
    instancing->SetOutLine(index,flag);
}