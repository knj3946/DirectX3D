#include "Framework.h"
 
Orc::Orc(Transform* transform, ModelAnimatorInstancing* instancing, UINT index)
    :transform(transform), instancing(instancing), index(index)
{
    //클립 생성해두기 
    string modelName = "Orc";

    //히트 충돌체
    collider = new CapsuleCollider(40, 120);
    collider->SetParent(transform);
    //collider->Rot().z = XM_PIDIV2 - 0.2f;
    collider->Pos() = { -15, 80, 0 };
    collider->SetActive(false); //spawn 할때 활성화

    //이동 충돌체
    moveCollider = new CapsuleCollider(5, 120);
    moveCollider->SetParent(transform);
    //moveCollider->Rot().z = XM_PIDIV2 - 0.2f;
    moveCollider->Pos() = { -15, 80, 0 };
    moveCollider->SetActive(false); //spawn 할때 활성화

    // 무기 충돌체
    leftHand = new Transform();
    leftWeaponCollider = new CapsuleCollider(8, 70);
    leftWeaponCollider->Pos().y += 20;
    leftWeaponCollider->SetParent(leftHand); // 임시로 만든 충돌체를 "손" 트랜스폼에 주기
    leftWeaponCollider->SetActive(false); //attack 할때 활성화

    weaponColliders.push_back(leftWeaponCollider);

    rightHand = new Transform();
    rightWeaponCollider = new CapsuleCollider(6, 60);
    rightWeaponCollider->Pos().y += 20;
    rightWeaponCollider->SetParent(rightHand); // 임시로 만든 충돌체를 "손" 트랜스폼에 주기
    rightWeaponCollider->SetActive(false); //attack 할때 활성화

    weaponColliders.push_back(rightWeaponCollider);

 
    motion = instancing->GetMotion(index);
    totalEvent.resize(instancing->GetClipSize()); //모델이 가진 동작 숫자만큼 이벤트 리사이징
    eventIters.resize(instancing->GetClipSize());

    //이벤트 세팅
#pragma region 공격 애니메이션 이벤트 설정
    SetEvent(ATTACK1, bind([=]() {ORCSOUND(index)->AllStop(); isAnim = true; }), 0.01f);
    SetEvent(ATTACK2, bind([=]() {ORCSOUND(index)->AllStop(); isAnim = true; }), 0.01f);
    SetEvent(ATTACK3, bind([=]() {ORCSOUND(index)->AllStop(); isAnim = true; }), 0.01f);
    
    // 공격별 타이밍 맞추기
    SetEvent(ATTACK1, bind(&Orc::StartAttack, this), 0.3f);
    SetEvent(ATTACK2, bind(&Orc::StartAttack, this), 0.3f);
    SetEvent(ATTACK3, bind(&Orc::StartAttack, this), 0.3f);

    SetEvent(ATTACK2, bind(&Collider::SetActive, rightWeaponCollider, true), 0.6f); // 오른손으로 두번 때리는 동작임
    SetEvent(ATTACK3, bind(&Collider::SetActive, leftWeaponCollider, true), 0.7f); //왼손으로 두번 때리는 동작임

    SetEvent(ATTACK1, bind(&Orc::EndAttack, this), 0.99f);
    SetEvent(ATTACK2, bind(&Orc::EndAttack, this), 0.99f);
    SetEvent(ATTACK3, bind(&Orc::EndAttack, this), 0.99f);
    

    // 공격별 사운드 타이밍
    SetEvent(ATTACK1, bind([=]() {ORCSOUND(index)->Play("Orc_Attack",attackVolume*VOLUME); }), 0.31f);
    SetEvent(ATTACK2, bind([=]() {ORCSOUND(index)->Play("Orc_Attack", attackVolume* VOLUME); }), 0.25f);
    SetEvent(ATTACK2, bind([=]() {ORCSOUND(index)->Play("Orc_Attack", attackVolume*VOLUME); }), 0.66f);
    SetEvent(ATTACK3, bind([=]() {ORCSOUND(index)->Play("Orc_Attack2", attackVolume* VOLUME); }), 0.31f);
    SetEvent(ATTACK3, bind([=]() {ORCSOUND(index)->Play("Orc_Attack2", attackVolume*VOLUME); }), 0.70f);


#pragma endregion

   
    
    SetEvent(THROW, bind([=]() {ORCSOUND(index)->Stop("Orc_Run"); isAnim = true; }), 0.01f);
    //SetEvent(ATTACK, bind(&Orc::StartAttack, this), 0.3f);
    //SetEvent(ATTACK, bind(&Orc::EndAttack, this), 0.99f);
    SetEvent(THROW, bind(&Orc::Throw, this), 0.59f);
    SetEvent(THROW, bind([=]() {isAnim = false; }), 0.99f);
    SetEvent(HIT, bind([=]() {ORCSOUND(index)->Stop("Orc_Run"); isAnim = true; }), 0.01f);
    SetEvent(HIT, bind(&Orc::EndHit, this), 0.95f);
    SetEvent(ASSASSINATED, bind(&Orc::EndAssassinated, this), 0.9f);
    SetEvent(DYING, bind(&Orc::EndDying, this), 0.9f);

    

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
    rangeBar->SetAlpha(0.5f);

    rangeBar->Rot() = { XMConvertToRadians(90.f),0,XMConvertToRadians(-90.f) };
    rangeBar->Pos() = { -15.f,2.f,-650.f };

    particleHit = new Sprite(L"Textures/Effect/HitEffect.png", 15, 15, 5, 2, false);
    particleHit->Stop();
    

    startEdgeTrailL = new Transform();
    startEdgeTrailL->SetParent(leftWeaponCollider);
    startEdgeTrailL->Pos() = leftWeaponCollider->Pos() + leftWeaponCollider->Pos().Up() * 0.5f / 0.03f; // 20.0f :모델크기반영

    endEdgeTrailL = new Transform();
    endEdgeTrailL->SetParent(leftWeaponCollider);
    endEdgeTrailL->Pos() = leftWeaponCollider->Pos() + leftWeaponCollider->Pos().Down() * 0.5f / 0.03f; // 20.0f :모델크기반영

    weaponTrailL = new Trail(L"Textures/Effect/wind.jpg", startEdgeTrailL, endEdgeTrailL, 100, 95);
    weaponTrailL->SetActive(true);


    startEdgeTrailR = new Transform();
    startEdgeTrailR->SetParent(rightWeaponCollider);
    startEdgeTrailR->Pos() = leftWeaponCollider->Pos() + leftWeaponCollider->Pos().Up() * 0.5f / 0.03f; // 20.0f :모델크기반영

    endEdgeTrailR = new Transform();
    endEdgeTrailR->SetParent(rightWeaponCollider);
    endEdgeTrailR->Pos() = rightWeaponCollider->Pos() + rightWeaponCollider->Pos().Down() * 0.5f / 0.03f; // 20.0f :모델크기반영

    weaponTrailR = new Trail(L"Textures/Effect/wind.jpg", startEdgeTrailR, endEdgeTrailR, 100, 95);
    weaponTrailR->SetActive(true);
   // particleHit->Stop();
}

Orc::~Orc()
{

    delete leftHand;
    delete rightHand;
    delete leftWeaponCollider;
    delete rightWeaponCollider;
    delete collider;
    delete moveCollider;
    //delete terrain;
   // delete aStar;
   
 
    //delete root;
    delete exclamationMark;
    delete questionMark;
    delete rangeBar;
    delete hpBar;
  
    delete particleHit;
    delete rayBuffer;
    delete structuredBuffer;

    delete weaponTrailL;
    delete startEdgeTrailL;
    delete endEdgeTrailL;

    delete weaponTrailR;
    delete startEdgeTrailR;
    delete endEdgeTrailR;
}

void Orc::SetType(NPC_TYPE _type) {

    switch (_type)
    {
    case Orc::NPC_TYPE::ATTACK:// 오크 타입에 따라 탐지 범위 지정 (현재 임시)
        informrange = 40;
        type = NPC_TYPE::ATTACK;
        break;
    case Orc::NPC_TYPE::INFORM:
        informrange = 70;
        type = NPC_TYPE::INFORM;
        break;
    default:
        break;
    }
}

void Orc::Update()
{
    if (!transform->Active()) return; //활성화 객체가 아니면 리턴
   
    if (curState != DYING && curState != ASSASSINATED)
    {
        Direction();// 방향지정 함수
        CalculateEyeSight(); //시야에 발각됬는지 확인하는 함수 (bDetection 설정)
        CalculateEarSight(); //소리가 들렸는지 확인하는 함수
        Detection(); //플레이어를 인지했는지 확인하는 함수
        RangeCheck(); //발견되었다가 사라진 플레이어 탐지
    }

    // 천장으로 올라갔을때 이 3가지 상태에서 고정되는거 강제로 해제
    /*if (bFind && curState == RUN && path.empty())
    {
        bFind = false;
    }*/

    // 사운드 테스트
    SoundControl();
    //-------------------------

    TimeCalculator(); //공격 간격을 두기 위한 설정
    //ParticleUpdate(); //파티클이펙트 업데이트
    UpdateUI(); //UI 업데이트
    ExecuteEvent(); //이벤트 있으면 실행
    ParticleUpdate(); //파티클이펙트 업데이트

    if (curState == DYING || curState == ASSASSINATED)
        return;

    PartsUpdate(); //모델 각 파츠 업데이트
    StateRevision(); //애니메이션 중간에 끊겨서 변경안된 값들 보정
    
    CoolDown();
    //====================== 이동관련==============================
    if (CalculateHit()) return; //맞는 중이면 리턴 (이 아래는 이동과 관련된 것인데 맞는중에는 필요없음)
    if (!GetDutyFlag()) //해야할일(움직임)이 생겼는지 확인
    {
        //해야할일이 없을때
        //SetState(IDLE);
        IdleAIMove();
        return;
    }

    SetParameter(); //필요한 변수들 세팅
    Control(); // 경로설정등 오크가 움직이기 위한 정보들 계산
    Move(); //실제 움직임
    RotationRestore();
}

void Orc::Render()
{
    //collider->Render();
    //leftWeaponCollider->Render();
    //rightWeaponCollider->Render();
    hpBar->Render();
    if(behaviorstate != NPC_BehaviorState::DETECT || !isDying) // 죽으면 바로 렌더안하게 
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

void Orc::WeaponTrailRender()
{
    if (trailToggle)
    {
        weaponTrailL->Render();
        weaponTrailR->Render();
    }
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
    ImGui::Text("curState : %d", curState);

    /*ImGui::Text("bFind : %d", bFind);
    ImGui::Text("bDetection : %d", bDetection);
    ImGui::Text("isTracking : %d", isTracking);
    ImGui::Text("path.empty() : %d", path.empty());*/
    //ImGui::Text("missTargetTrigger : %d", missTargetTrigger);
    //ImGui::Text("NPC_BehaviorState : %d", behaviorstate);
    ImGui::Text("OrcIsWalk : %d", ORCSOUND(index)->IsPlaySound("Orc_Walk"));
    ImGui::Text("earCal : %d", bSound);
    ImGui::Text("OrcRunVolume : %f", lastRunVolume);
    ImGui::Text("OrcWalkVolume : %f", lastWalkVolume);
    ImGui::Text("OrcHitVolume : %f",lastVolume);
    ImGui::Text("lastDist : %f", lastDist);

    //ImGui::Text("curState : %d", curState);


    //ImGui::SliderFloat("OrcMoveSound", &volume, 0, 10);

    //ImGui::SliderFloat("OrcWalkSetVolume", &walkVolumeS, 0, 100);

    /*ImGui::Text("y : %f", transform->GlobalPos().y);
    ImGui::Text("bFind : %d", bFind);
    ImGui::Text("bDetection : %d", bDetection);
    ImGui::Text("isTracking : %d", isTracking);
    ImGui::Text("path.empty() : %d", path.empty());
    ImGui::Text("missTargetTrigger : %d", missTargetTrigger);
    ImGui::Text("NPC_BehaviorState : %d", behaviorstate);
    ImGui::Text("curState : %d", curState);*/

    //ImGui::Text("FeedbackPosY : %f", feedBackPos.y);
    //ImGui::Text("eyeSightRange : %f", eyeSightRange);
    //ImGui::Text("curhp : %f", curHP);
    //ImGui::Text("desthp : %f", destHP);

    //rangeBar->GUIRender();

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
    
    // 거리에 따라 이것도 사운드 조절
    ORCSOUND(index)->Play("ORC_Throw", throwVolume * VOLUME);

    SetState(IDLE);
}

void Orc::StartAttack()
{
    trailToggle = true;

    switch (curState)
    {
    case ATTACK1:
        rightWeaponCollider->SetActive(true);
        break;

    case ATTACK2:
        rightWeaponCollider->SetActive(true);
        break;

    case ATTACK3:
        leftWeaponCollider->SetActive(true);
        break;
    
    default:
        leftWeaponCollider->SetActive(true);
        rightWeaponCollider->SetActive(true);
    }
    
    leftWeaponCollider->UpdateWorld();
    weaponTrailL->Init(leftWeaponCollider->GlobalPos());

    
    rightWeaponCollider->UpdateWorld();
    weaponTrailR->Init(rightWeaponCollider->GlobalPos());
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

    transform->Pos().y = feedBackPos.y;
}

bool Orc::CalculateHit()
{
    if (isHit)
    {
        //SetState(HIT);

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
            //isHit = false;
            //collider->SetActive(true);
            //leftWeaponCollider->SetActive(true);
            //rightWeaponCollider->SetActive(true);
            //SetState(IDLE);
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
    moveCollider->UpdateWorld();
    transform->UpdateWorld();

    leftHand->SetWorld(instancing->GetTransformByNode(index, 170));//170
    leftWeaponCollider->UpdateWorld();

    rightHand->SetWorld(instancing->GetTransformByNode(index, 187));//187
    rightWeaponCollider->UpdateWorld();
}

void Orc::StateRevision()
{
    if (curState != ATTACK1 && curState!=ATTACK2 && curState!=ATTACK3)
    {
        leftWeaponCollider->SetActive(false);
        rightWeaponCollider->SetActive(false);
        trailToggle = false;
    }
}

void Orc::ParticleUpdate()
{
    particleHit->Update();

    if (trailToggle)
    {                
        startEdgeTrailL->Pos() = leftWeaponCollider->Pos() + leftWeaponCollider->Pos().Up() * 0.5f / 0.03f; // 20.0f :모델크기반영
        endEdgeTrailL->Pos() = leftWeaponCollider->Pos() + leftWeaponCollider->Pos().Down() * 0.5f / 0.03f; // 20.0f :모델크기반영

        startEdgeTrailL->Pos().y -= 12.0f;
        endEdgeTrailL->Pos().y -= 12.0f;

        // 찾아낸 꼭지점 위치를 업데이트 
        startEdgeTrailL->UpdateWorld();
        endEdgeTrailL->UpdateWorld(); // 이러면 트랜스폼에 위치가 담긴다
        weaponTrailL->Update();

        startEdgeTrailR->Pos() = rightWeaponCollider->Pos() + rightWeaponCollider->Pos().Up() * 0.5f / 0.03f; // 20.0f :모델크기반영
        endEdgeTrailR->Pos() = rightWeaponCollider->Pos() + rightWeaponCollider->Pos().Down() * 0.5f / 0.03f; // 20.0f :모델크기반영

        startEdgeTrailR->Pos().y -= 12.0f;
        endEdgeTrailR->Pos().y -= 12.0f;

        // 찾아낸 꼭지점 위치를 업데이트 
        startEdgeTrailR->UpdateWorld();
        endEdgeTrailR->UpdateWorld(); // 이러면 트랜스폼에 위치가 담긴다
        weaponTrailR->Update();
    }
}

void Orc::SoundControl()
{
    if (!isAnim)
    {
        // 오크가 걷거나 달릴 때 사운드 틀고 볼륨 조절
        // 멈추면 사운드 다 멈추기

        if (curState == RUN)
        {
            if (curState == RUN && !ORCSOUND(index)->IsPlaySound("Orc_Run"))
            {
                ORCSOUND(index)->Play("Orc_Run");
            }
            if (ORCSOUND(index)->IsPlaySound("Orc_Run"))
            {
                float distance = Distance(target->Pos(), transform->Pos());
                distance = (distance > 50) ? 50 : distance;
                ORCSOUND(index)->SetVolume("Orc_Run", (50 - distance) / 10.0f * VOLUME);
                lastRunVolume = ORCSOUND(index)->GetVolume("Orc_Run");
                lastDist = distance;
            }
        }
        else if (curState == WALK)
        {
            if (curState == WALK && !ORCSOUND(index)->IsPlaySound("Orc_Walk"))
            {
                ORCSOUND(index)->Play("Orc_Walk");
            }
            if (ORCSOUND(index)->IsPlaySound("Orc_Walk"))
            {
                float distance = Distance(target->Pos(), transform->Pos());
                distance = (distance > 40) ? 40 : distance;
                ORCSOUND(index)->SetVolume("Orc_Walk", (40 - distance) / 20.0f * VOLUME);
                lastWalkVolume = ORCSOUND(index)->GetVolume("Orc_Walk");
                lastDist = distance;
            }
        }
        else
        {
            if(ORCSOUND(index)->IsPlaySound("Orc_Run"))
                ORCSOUND(index)->Stop("Orc_Run");
            if (ORCSOUND(index)->IsPlaySound("Orc_Walk"))
                ORCSOUND(index)->Stop("Orc_Walk");
        }
    }
}

void Orc::SetAttackState()
{
    if (curState == ATTACK1 || curState == ATTACK2 || curState == ATTACK3)return;
    int randNum = GameMath::Random(0, 3);
    switch (randNum)
    {
    case 0:
        SetState(ATTACK1,0.7);
        break;
    case 1:
        SetState(ATTACK2,0.8);
        break;
    case 2:
        SetState(ATTACK3,0.7);
        break;
    default:
        break;
    }
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
    if (curState == ATTACK1||curState==ATTACK2||curState==ATTACK3)
    {
        r = 10.0f;// 임시로 바꿈 10 -> 60
    }
    return r;
}

void Orc::Hit(float damage,Vector3 collisionPos, bool _btrue)
{
    if (!isHit)
    {
        //ORCSOUND(index)->AllStop();

        destHP = (curHP - damage > 0) ? curHP - damage : 0;

        collider->SetActive(false);
        leftWeaponCollider->SetActive(false);
        rightWeaponCollider->SetActive(false);
        trailToggle = false;
        if (destHP <= 0)
        {
            
            isDying = true;
            SetState(DYING);
            return;
        }

        if (!ORCSOUND(index)->IsPlaySound("Orc_Hit"))
        {
            float distance = Distance(target->Pos(), transform->Pos());
            distance = (distance > 40) ? 40 : distance;
            ORCSOUND(index)->Play("Orc_Hit",Clamp(0,100,40-distance)/ 10.0f * VOLUME); // 크기조절 가까울수록 사운드 커지게
            lastVolume = ORCSOUND(index)->GetVolume("Orc_Hit");
        }

        SetState(HIT);

        isHit = true;
       
      
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
    moveCollider->SetActive(true);
    questionMark->SetActive(false);
    exclamationMark->SetActive(false);
    bFind = false;
    bDetection = false;
    DetectionStartTime = 0.f;
    rangeBar->SetAmount(DetectionStartTime / DetectionEndTime);
    hpBar->SetAmount(curHP / maxHp);


    //leftWeaponCollider->SetActive(true);
    //rightWeaponCollider->SetActive(true);
}

void Orc::AttackTarget()
{
    if (curState == DYING) return;
    //return;//??
    if (!bFind)
    {
        bFind = true;
        isTracking = true;
        //if (curState == IDLE)
            SetState(RUN);
    }
}


void Orc::Findrange(float startCool)
{
    if (curState == DYING) return;

    if (curState == ATTACK1 || curState == ATTACK2 || curState == ATTACK3)return;
    // 탐지시 범위에 닿은 애에게 설정
    bFind = true; bDetection = true;
    DetectionStartTime = DetectionEndTime;
    isTracking = true;
    SetState(RUN);

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

    behaviorstate = NPC_BehaviorState::DETECT;

    searchStartCoolDown = startCool;
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
    if (searchCoolDown > 1 && searchStartCoolDown <= 0)//
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
                    
                    // 세팅할 때는 1을 주고 함수안에서 랜덤으로 정한다.
                    SetAttackState();
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
                /*
                if (aStar->IsCollisionObstacle(transform->GlobalPos(), restorePos))// 중간에 장애물이 있으면
                {
                    SetPath(restorePos); // 구체적인 경로 내어서 가기
                }
                else
                {
                    path.clear(); // 굳이 장애물없는데 길찾기 필요 x
                    path.push_back(restorePos); // 가야할 곳만 경로에 집어넣기
                }
                */
                if (target->Pos().y > 9) // 플레이어가 위로 올라가서 안보이면
                {
                    path.clear();
                    //SetPath(restorePos);
                    return; // 던질 수 있는데 굳이 앞으로 달려가는거 방지
                }
                //직선레이에 장애물이 탐지되지 않아도 몸통 콜라이더가 걸려서 못갈수도 있기 때문에 항상 경로 내기
                 // 구체적인 경로 내어서 가기
                path.clear();
                SetPath(restorePos);
            }
            else
            {
         


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

    if (searchStartCoolDown > 0.f)
        searchStartCoolDown -= DELTA;
    else
        searchStartCoolDown = 0.f;
}

void Orc::Move()
{   
    //if (path.empty())SetState(IDLE);
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
            SetAttackState();
        }
    }
    
    
    float f=velocity.Length();
    if (curState == IDLE) return; 
    if (curState == ATTACK1) return;
    if (curState == ATTACK2) return;
    if (curState == ATTACK3) return;
    if (curState == THROW) return;
    if (curState == HIT) return;
    if (curState == DYING) return;
    if (behaviorstate == NPC_BehaviorState::CHECK)return;
    if (velocity.Length() < 0.1f&&!missTargetTrigger) return;
    if (velocity.Length() < 0.1f && missTargetTrigger) return;

    if (!path.empty())
    {
        // 가야할 곳이 있다
     
        if(behaviorstate!=NPC_BehaviorState::SOUNDCHECK
            //&&!bSound
            )
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
    else
    {
        SetState(IDLE);
    }
}


void Orc::IdleAIMove()
{
    // WALK애니메이션 해결 -> Orc_Run0.clip 대신 character1@walk30.clip 사용할 것

    if (behaviorstate == NPC_BehaviorState::CHECK)return;
  
    if (returntoPatrol) {
        if (IsStartPos()) {
            returntoPatrol = false;
       }

        if (curState == WALK)
        {
            transform->Rot().y = atan2(velocity.x, velocity.z) + XM_PI;
            transform->Pos() += DELTA * walkSpeed * transform->Back();
        }
        return;
    }
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
        if (targetStateInfo->isCloaking)return;
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

    // 공격을 할때는 우선 SetAttackState()로 설정하고 이 함수 안에서 1,2,3 중 한개 모션으로 실행한다.

    if (state != HIT && isHit)return;

    // 공격 속도 조절 -> Attack이지만 attackSpeed만큼 시간이 지나지 않았다면 공격 x
    if (state == ATTACK1 && !isAttackable)
    {
        state = IDLE;
        //return;
    }
    if (state == ATTACK1 || state == THROW)
    {
        // 방향도 바꾸기
        velocity = target->Pos() - transform->Pos();
        transform->Rot().y = atan2(velocity.x, velocity.z) + XM_PI;
    }
    curState = state; //매개변수에 따라 상태 변화

    if (curState == DYING)
    {
        ORCSOUND(index)->AllStop();
        ORCSOUND(index)->Play("Orc_Die", dieVolume * VOLUME);
        if (isAssassinated)
        {
            instancing->PlayClip(index, (int)state + 1, 0.8);
        }
        else
            instancing->PlayClip(index, (int)state + 0, 0.8);
    }
    else
        instancing->PlayClip(index, (int)state, scale); //인스턴싱 내 자기 트랜스폼에서 동작 수행 시작
    
    
    eventIters[state] = totalEvent[state].begin(); //이벤트 반복자도 등록된 이벤트 시작시점으로
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
    isAnim = false;
    battacktarget = false;
    //SetState(ATTACK);
}

void Orc::EndHit()
{
    collider->SetActive(true);
    isAnim = false;
    isHit = false;
    SetState(IDLE);
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
    //ORCSOUND(index)->AllStop();

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

    trailToggle = false;

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
    if (bDetection) {
        if (targetStateInfo->isCloaking)
        {
            bDetection = false;
            SetState(IDLE);
            DetectionStartTime = DELTA * 0.1f;
            return;
        }
    }

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
     DetectionRange =(Distance(target->GlobalPos(), transform->GlobalPos()));
    if (DetectionRange < eyeSightRange)
    {
        SetEyePos(); //눈 위치만 설정

        if (!EyesRayToDetectTarget(targetCollider, eyesPos)) //리턴 값 false면 가려서 안보이는 것
        {
            if (bDetection) {
                restorePos = target->GlobalPos();
            }
             bDetection = false;
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
                if (bDetection) {
                    restorePos = target->GlobalPos();
                }
                bDetection = false;
                return;
            }
        }
        else {
            if (!(leftdir1 <= Enemytothisangle && rightdir1 >= Enemytothisangle))
            {
                if (bDetection) {
                    restorePos = target->GlobalPos();
                }
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
    SoundPositionCheck(); //소리가 들렸다면 소리의 위치 확인하는 함수
    if (behaviorstate != NPC_BehaviorState::IDLE)return;
    Vector3 pos;
    float volume = -1.f;
    float distance = -1.f;

    // 현재 시프트 누르면 이동속도를 낮춘다. 
    // 나중에 느리게 걷기 사운드를 추가한다면 변경하기
    if ((PLAYERSOUND()->IsPlaySound("Player_Move") && ColliderManager::Get()->GetPlayer()->GetMoveSpeed() > 20)
        || (PLAYERSOUND()->IsPlaySound("Player_Land"))) {
        
        pos.x = target->Pos().x;
        /*if (PLAYERSOUND()->IsPlaySound("Player_Land"))
            pos.y = 0;
        else
            pos.y = target->Pos().y;*/
        pos.y = target->Pos().y;
        pos.z = target->Pos().z;
        distance = Distance(transform->Pos(), pos);
        
        // GetVolume() 값은 0.0 ~ 1.0 임
        /*if (PLAYERSOUND()->IsPlaySound("Player_Move"))
            volume = PLAYERSOUND()->GetVolume("Player_Move");
        else
            volume = PLAYERSOUND()->GetVolume("Player_Land");*/

    }
    // 웅크리고 걷는 소리 ,암살소리  제외
    // 플레이어 소리와 주위 시선 끄는 소리 추가



    if (distance == -1.f)return;
    if (distance < earRange ) {
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
    if (bDetection)
    {
        if (!bFind)
        {
            if (behaviorstate == NPC_BehaviorState::SOUNDCHECK) {
                DetectionStartTime = DetectionEndTime;
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
                    returntoPatrol = false;
                }
            }
            float value = eyeSightRange / DetectionRange;

            //DetectionStartTime += DELTA * value;
            DetectionStartTime += DELTA;
        }
    }
    else {
        if (ErrorCheckTime >= 6.f) {
            DetectionStartTime = DELTA * 0.5f;
        }
        if (DetectionStartTime > 0.f) {
            DetectionStartTime -= DELTA * 0.5f;
            if (bFind) {
                if (DetectionStartTime <= DELTA * 0.5f) {
                    DetectionStartTime = 0.f;
                    bFind = false;
                }

            }
            if (DetectionStartTime <= 0.f) {
                    DetectionStartTime = 0.f;
                    path.clear();
                    bFind = false;
                    bSound = false; // 추가
                    //isTracking = false;
                    if (behaviorstate != NPC_BehaviorState::CHECK)
                        rangeDegree = XMConvertToDegrees(transform->Rot().y);
                    behaviorstate = NPC_BehaviorState::CHECK;
                    //behaviorstate = NPC_BehaviorState::IDLE;
                    SetState(IDLE);
                    missTargetTrigger = false;
                    restorePos = {};
                }
        }
     
 
        if (bFind&&!bDetection) {
            ErrorCheckTime += DELTA;
        }
        else {
            ErrorCheckTime = 0.f;
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
        returntoPatrol = false;
    }
    rangeBar->SetAmount(DetectionStartTime / DetectionEndTime);

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

bool Orc::IsStartPos()
{
   
    if (PatrolPos[nextPatrol].x + 2.0f > transform->Pos().x && PatrolPos[nextPatrol].x - 2.0f < transform->Pos().x &&
        PatrolPos[nextPatrol].z + 2.0f > transform->Pos().z && PatrolPos[nextPatrol].z - 2.0f < transform->Pos().z)
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
     
        if (Basedegree >=45.f) {// 플레이어를 놓친 후 각도에서 왼쪽으로 45 넘을시
            m_uiRangeCheck++;
            Basedegree = 0.f;
        }
    }
    else if (1 == m_uiRangeCheck)
    {

       

        if (Basedegree <= -90.f) {// 플레이어를 놓친 후 각도에서 왼쪽으로 45 넘을시
            m_uiRangeCheck++;
            Basedegree = 0.f;
        }
    }
    else if (2 == m_uiRangeCheck) //원래보고있던 각도로 복귀
    {
  
        if (Basedegree >=45.f) {
            m_uiRangeCheck++;
            Basedegree = 0.f;
        }
    
    }


    if (m_uiRangeCheck % 2 == 0) {
        transform->Rot().y += DELTA*1.5f;
        Basedegree +=XMConvertToDegrees( DELTA * 1.5f);
       
    }
    else {
        transform->Rot().y -= DELTA * 1.5f;
        Basedegree -= XMConvertToDegrees(DELTA * 1.5f);

    }

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
        returntoPatrol = true;

        if (aStar->IsCollisionObstacle(transform->GlobalPos(), PatrolPos[nextPatrol]))// 중간에 장애물이 있으면
        {
            SetPath(PatrolPos[nextPatrol]); // 구체적인 경로 내어서 가기
        }
        else
        {
            path.clear(); // 굳이 장애물없는데 길찾기 필요 x
            path.push_back(PatrolPos[nextPatrol]); // 가야할 곳만 경로에 집어넣기
        }
    }
}

void Orc::SoundPositionCheck()
{
    // 소리가 나면 해당위치로 setPath 설정 -> 해당 위치에 근접했다면 거기서 부터 탐지 시작
    float d = 2.f;
    if (behaviorstate == NPC_BehaviorState::SOUNDCHECK) {
        if (CheckPoint.x + d > transform->GlobalPos().x && CheckPoint.x - d < transform->GlobalPos().x &&
            CheckPoint.z + d > transform->GlobalPos().z && CheckPoint.z - d < transform->GlobalPos().z)
        {
            behaviorstate = NPC_BehaviorState::CHECK;
            rangeDegree = XMConvertToDegrees(transform->Rot().y);
            path.clear();
            SetPath(StorePos);
            SetState(IDLE);
            bSound = false;
            //bDetection = false;
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
        UINT w = terrain->GetSizeWidth();

        float minx = floor(ray.pos.x);
        float maxx = ceil(ray.pos.x);

        if (maxx == w - 1 && minx == maxx)
            minx--;
        else if(minx == maxx)
            maxx++;
        
        float minz = floor(ray.pos.z);
        float maxz = ceil(ray.pos.z);

        if (maxz == w - 1 && minz == maxz)
            minz--;
        else if (minz == maxz)
            maxz++;

        for (UINT z = w - maxz - 1; z < w - minz - 1; z++)
        {
            for (UINT x = minx; x < maxx; x++)
            {
                UINT index[4];
                index[0] = w * z + x;
                index[1] = w * z + x + 1;
                index[2] = w * (z + 1) + x;
                index[3] = w * (z + 1) + x + 1;

                vector<VertexType>& vertices = terrain->GetMesh()->GetVertices();

                Vector3 p[4];
                for (UINT i = 0; i < 4; i++)
                    p[i] = vertices[index[i]].pos;

                float distance = 0.0f;
                if (Intersects(ray.pos, ray.dir, p[0], p[1], p[2], distance))
                {
                    feedback = ray.pos + ray.dir * distance;
                    return true;
                }

                if (Intersects(ray.pos, ray.dir, p[3], p[1], p[2], distance))
                {
                    feedback = ray.pos + ray.dir * distance;
                    return true;
                }
            }
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