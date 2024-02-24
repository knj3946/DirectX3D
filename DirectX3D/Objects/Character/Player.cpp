//    if (KEY_DOWN(VK_SPACE) && canJump) {
//        if (curState != JUMP1 && KEY_DOWN(VK_SPACE))
//        {
//            SetState(JUMP1);
//        }
//
//        if (jumpN == 2)
//        {
//            jumpVel = force3;
//            jumpN = 0;
//        }
//        else {
//            if (jumpN == 0)
//                jumpVel = force1;
//            else if (jumpN == 1)
//                jumpVel = force2;
//            jumpN++;
//        }
//    }
//}
//
//void Player::Jump()
//{
//    jumpVel -= 9.8f * gravityMult * DELTA;
//    Pos().y += jumpVel * DELTA;
//
//    //float heightLevel = 0; // 기준 높이 (현재는 0)
//    float heightLevel = BlockManager::Get()->GetHeight(Pos());
//
//    if (Pos().y < heightLevel) {
//        canJump = true;
//
//        Pos().y = heightLevel;
//        fall = false;
//        jumpVel = 0;
//
//        if (nextJump > 0.0f) {
//            nextJump -= DELTA;
//            if (nextJump <= 0.0f)
//            {
//                nextJump = 0.0f;
//                jumpN = 0;
//            }
//            return;
//        }
//
//        if (jumpN != 0)
//            nextJump = 1.5f;
//    }
//    else
//        canJump = false;
//}
//
//void Player::SetAnimation()
//{
//    /*if (jumpVel > 0.0f)
//        SetState(JUMP1);
//    //else if (jumpVel < 0.0f) {
//    //    SetState(JUMP2);
//    //    if (!fall) {
//    //        if(jumpN == 2)
//    //            Pos().y += 70.0f;
//    //        if(jumpN == 0)
//    //            Pos().y += 100.0f;
//    //        fall = true;
//    //    }
//    //}

#include "Framework.h"
#include "Player.h"

Player::Player()
    : ModelAnimator("akai")
{
    targetTransform = new Transform();
    //straightRay = Ray(Pos(), Back());

    //Scale() *= 0.1f;
    // 윈도우 핸들러의 정보값(중 윈도우 크기)을 두 번째 매개변수에 저장
    ClientToScreen(hWnd, &clientCenterPos);
    SetCursorPos(clientCenterPos.x, clientCenterPos.y);

    collider = new CapsuleCollider(25.0f, 140);
    collider->SetParent(this);
    collider->Pos().y += 20;

    footRay = new Ray();
    footRay->pos = Pos();
    footRay->dir = Pos().Down();

    weapon = new Model("dagger");
    weapon->Scale() *= 100;
    //weapon->Rot();

    rightHand = new Transform();
    weaponCollider = new CapsuleCollider(10, 50);
    weaponCollider->Pos().y += 20;
    weaponCollider->SetParent(rightHand); // 임시로 만든 충돌체를 "손" 트랜스폼에 주기
    weapon->SetParent(rightHand); // 임시로 만든 충돌체를 "손" 트랜스폼에 주기
    
    //left foot : 57
    leftFoot = new Transform();
    leftFootCollider = new CapsuleCollider(10, 50);
    leftFootCollider->SetParent(leftFoot);

    //right foot : 62
    rightFoot = new Transform();
    rightFootCollider = new CapsuleCollider(10, 50);
    rightFootCollider->SetParent(rightFoot);

    computeShader = Shader::AddCS(L"Compute/ComputePicking.hlsl");
    rayBuffer = new RayBuffer();

    hpBar = new ProgressBar(L"Textures/UI/hp_bar.png", L"Textures/UI/hp_bar_BG.png");
    hpBar->Scale() *= 0.6f;
    hpBar->SetAmount(curHP / maxHp);

    string temp;    //하위 파일

    ReadClip("Idle"); // 동작이 0뒤에 1까지 있음

    ReadClip("Medium Run");
    ReadClip("Running Backward");
    ReadClip("Left Strafe");
    ReadClip("Right Strafe");

    ReadClip("Jog Forward Diagonal Left");
    ReadClip("Jog Forward Diagonal Right");

    ReadClip("Jumping"); // 동작이 0뒤에 1까지 있음
    ReadClip("IntheSky"); // 동작이 0뒤에 1까지 있음
    ReadClip("Falling To Landing"); // 동작이 0뒤에 1까지 있음

    ReadClip("Stand To Cover"); // 동작이 0뒤에 1까지 있음
    ReadClip("Cover Idle"); // 동작이 0뒤에 1까지 있음
    ReadClip("Crouched Sneaking Right"); // 동작이 0뒤에 1까지 있음
    ReadClip("Crouched Sneaking Left"); // 동작이 0뒤에 1까지 있음
    ReadClip("Crouch Turn To Stand"); // 동작이 0뒤에 1까지 있음

    ReadClip("Turning Right"); // TO_ASSASIN //뭔지 몰라서 아무거나 넣어둠

    temp = "Attack/";
    //NONE
    ReadClip(temp + "Side Kick");

    //DAGGER
    ReadClip(temp + "Stable Sword Outward Slash");
    ReadClip(temp + "Stable Sword Inward Slash");
    ReadClip(temp + "Upward Thrust");

    // HIT
    ReadClip("Head Hit"); 

    GetClip(JUMP1)->SetEvent(bind(&Player::Jump, this), 0.1f);  //점프시작
    GetClip(JUMP1)->SetEvent(bind(&Player::AfterJumpAnimation, this), 0.20001f);   //하강

    GetClip(JUMP3)->SetEvent(bind(&Player::SetIdle, this), 0.0001f);   //착지

    GetClip(TO_COVER)->SetEvent(bind(&Player::SetIdle, this), 0.05f);   //엄폐

    //GetClip(TO_ASSASIN)->SetEvent(bind(&Player::Assasination, this), 0.01f);   //암살

    GetClip(HIT)->SetEvent(bind(&Player::EndHit, this), 0.35f); // 히트가 끝나고

    GetClip(DAGGER1)->SetEvent(bind(&Player::SetIdle, this), 0.6f);
    GetClip(DAGGER2)->SetEvent(bind(&Player::SetIdle, this), 0.6f);
    GetClip(DAGGER3)->SetEvent(bind(&Player::SetIdle, this), 0.6f);

    GetClip(DAGGER1)->SetEvent(bind(&Player::CanCombo, this), 0.4f);

    //GetClip(HIT)->SetEvent(bind(&Player::EndHit, this), 0.35f);
}

Player::~Player()
{
    delete collider;
    delete footRay;

    delete hpBar;

    delete rayBuffer;
    delete structuredBuffer;

    delete targetTransform;
    delete targetObject;

    delete rightHand;
    delete weaponCollider;
    delete weapon;
}

void Player::Update()
{
    weaponCollider->SetActive(false);
    collider->Pos().y = collider->Height() * 0.5f * 33.3f + collider->Radius() * 33.3f;
    collider->UpdateWorld();

    footRay->pos = collider->GlobalPos();
    footRay->dir = Down();

    UpdateUI();

    Control();
    Searching();

    SetAnimation();

    ModelAnimator::Update();

    if (comboHolding > 0.0f)
        comboHolding -= DELTA;  //공격 콤보, 스턴시간 등 시간 감소 함수 만들기
    else
    {
        comboHolding = 0.0f;
        comboStack = 0;
    }

    rightHand->SetWorld(this->GetTransformByNode(rightHandNode));
    weaponCollider->UpdateWorld();

    leftFoot->SetWorld(this->GetTransformByNode(leftFootNode));
    leftFootCollider->UpdateWorld();

    rightFoot->SetWorld(this->GetTransformByNode(rightFootNode));
    rightFootCollider->UpdateWorld();

    weapon->Pos() = { 0, 10, 0 };
    weapon->Rot().x = x;
    weapon->Rot().y = y;
    weapon->Rot().z = z;
    weapon->UpdateWorld();
}

void Player::Render()
{
    ModelAnimator::Render();
    collider->Render();

    weaponCollider->Render();
    leftFootCollider->Render();
    rightFootCollider->Render();

    weapon->Render();
}

void Player::PostRender()
{
    hpBar->Render();
}

void Player::GUIRender()
{
    Model::GUIRender();

    ImGui::DragFloat3("Velocity", (float*)&velocity, 0.5f);
    //속력
    ImGui::SliderFloat("moveSpeed", &moveSpeed, 10, 1000);
    ImGui::SliderFloat("rotSpeed", &rotSpeed, 1, 10);
    ImGui::SliderFloat("deceleration", &deceleration, 1, 10);

    //점프 힘
    ImGui::SliderFloat("force1", &force1, 1, 500);
    ImGui::SliderFloat("force2", &force2, 1, 500);
    ImGui::SliderFloat("force3", &force3, 1, 500);

    ImGui::SliderFloat("jumpSpeed", &jumpSpeed, 0.01f, 5.0f);
    //중력
    ImGui::SliderFloat("gravityMult", &gravityMult, 1, 100);

    //3단 점프 구현시 조건 변수
    ImGui::InputFloat("JumpVel", (float*)&jumpVel);
    ImGui::InputFloat("jumpN", (float*)&jumpVel);
    ImGui::InputFloat("nextJump", (float*)&nextJump);

    //착지후 부동시간
    ImGui::InputFloat("landingT", (float*)&landingT);
    ImGui::InputFloat("landing", (float*)&landing);

    ImGui::InputInt("rightHandNode", &rightHandNode);
    ImGui::InputInt("leftFootNode", &leftFootNode);
    ImGui::InputInt("rightFootNode", &rightFootNode);

    ImGui::SliderFloat("x", &x, 0.001f, 360.0f);
    ImGui::SliderFloat("y", &y, 0.001f, 360.0f);
    ImGui::InputFloat("z", &z, 0.001f, 360.0f);
    ImGui::SliderFloat("s", &s, 0.001f, 360.0f);
}

void Player::SetTerrain(LevelData* terrain)
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

void Player::Control()  //사용자의 키보드, 마우스 입력 처리
{
    if (KEY_DOWN(VK_TAB))
    {
        camera = !camera;
    }
    if (camera)
        CAM->SetTarget(this);
    else
        CAM->SetTarget(nullptr);


    if (KEY_PRESS(VK_RBUTTON))
        return;

    Rotate();

    switch (curState)
    {
    case KICK:
    {
        //leftWeaponCollider->SetActive(true);
        //rightWeaponCollider->SetActive(true);
        break;
    }
    default:
    {
        //leftWeaponCollider->SetActive(false);
        //rightWeaponCollider->SetActive(false);
    }
    }

    if (curState == DAGGER1 || curState == DAGGER2 || curState == DAGGER3) 
    {
        if (combo && KEY_PRESS(VK_LBUTTON))
        {
            AttackCombo();
            return;
        }

        return;
    }

    if (KEY_PRESS('G'))
    {
        AttackCombo();
        return;
    }

    if (isHit)   //맞는게 활성화됐을때 Jumping함수가 동작을 안하면 공중에 떠있는 시간이 늘어남
    {
        return;
    }

    Move();
    Jumping();


    if(targetObject != nullptr && KEY_DOWN('F'))
    {
        velocity = 0;
        SetState(TO_ASSASIN);
    }
}

void Player::Move() //이동 관련(기본 이동, 암살 이동, 착지 후 이동제한, 특정 행동을 위한 목적지로 의 이동 등)
{
    //플레이어의 위에서 레이를 쏴서 현재 terrain 위치와 높이를 구함
    Vector3 PlayerSkyPos = Pos();
    PlayerSkyPos.y += 100;
    Ray groundRay = Ray(PlayerSkyPos, Vector3(Down()));
    TerainComputePicking(feedBackPos, groundRay);

    //if (curState == JUMP3 && landing > 0.0f)    //착지 애니메이션 동안 부동 이동 제한 and 제한 시간 감소
    //{
    //    landing -= DELTA;
    //    return;
    //}

    //if (curState == TO_COVER)    //엄폐하러 이동할 경우
    //{  
    //    if (Distance(Pos(), targetTransform->Pos()) < teleport)
    //    {
    //        SetState(TO_COVER);
    //        return;
    //    }
    //    Pos() += (targetTransform->Pos() - Pos()).GetNormalized() * DELTA * 400;
    //    SetState(RUN_F);
    //    return;
    //}

    if (KEY_DOWN(VK_SPACE) && !InTheAir())
    {
        SetState(JUMP1);
    }

    Walking();
}

void Player::UpdateUI()
{
    if (isHit)
    {
        curHP -= DELTA * 10 * 2;
        State aa = curState;

        if (curHP <= destHP)
        {
            curHP = destHP;
        }
        hpBar->SetAmount(curHP / maxHp);
    }
    
    Vector3 barPos = Pos() + Vector3(0, 6, 0);

    hpBar->UpdateWorld();

    if (!CAM->ContainPoint(barPos))
    {
        hpBar->SetActive(false);
        return;
    }

    if (!hpBar->Active()) hpBar->SetActive(true);
    hpBar->Pos() = CAM->WorldToScreen(barPos);
}

void Player::Rotate()
{
    Vector3 delta = mousePos - Vector3(CENTER_X, CENTER_Y);
    
    if (!camera)
        return;
    
    SetCursorPos(clientCenterPos.x, clientCenterPos.y);

    Rot().y += delta.x * rotSpeed * DELTA;
    CAM->Rot().x -= delta.y * rotSpeed * DELTA;
    
    if (KEY_PRESS('Q'))
        Rot().y -= DELTA * 2;
    if (KEY_PRESS('E'))
        Rot().y += DELTA * 2;
}

void Player::Walking()
{
    bool isMoveZ = false; // 전후 이동 중 : 기본값 "아님"
    bool isMoveX = false; // 좌우 이동 중 : 기본값 "아님"

    if (KEY_PRESS('W'))
    {
        if (velocity.z + DELTA * 4.0f < 0.0f)
            velocity.z += DELTA * 4.0f;
        else
            velocity.z += DELTA; //속력 기준에 시간 경과만큼 누적값 주기

        isMoveZ = true; //전후 이동 중임
    }

    if (KEY_PRESS('S'))
    {
        if(velocity.z - DELTA * 4.0f > 0.0f)
            velocity.z -= DELTA * 4.0f;
        else
            velocity.z -= DELTA;

        isMoveZ = true;
    }

    if (KEY_PRESS('A'))
    {
        if (velocity.x - DELTA * 4.0f > 0.0f)
            velocity.x -= DELTA * 4.0f;
        else
            velocity.x -= DELTA;

        isMoveX = true;
    }

    if (KEY_PRESS('D'))
    {
        if (velocity.x + DELTA * 4.0f < 0.0f)
            velocity.x += DELTA * 4.0f;
        else
            velocity.x += DELTA; //속력 기준에 시간 경과만큼 누적값 주기

        isMoveX = true;
    }

    if (velocity.Length() > 1) velocity.Normalize();

    if (!isMoveZ)
        velocity.z = Lerp(velocity.z, 0, deceleration * DELTA); //보간으로 감속

    if (!isMoveX)
        velocity.x = Lerp(velocity.x, 0, deceleration * DELTA);

    Matrix rotY = XMMatrixRotationY(Rot().y);
    Vector3 direction = XMVector3TransformCoord(velocity, rotY);

        
    Vector3 destFeedBackPos;
    Vector3 destPos = Pos() + direction * moveSpeed * DELTA * -1;
    Vector3 PlayerSkyPos = destPos;
    PlayerSkyPos.y += 100;
    Ray groundRay = Ray(PlayerSkyPos, Vector3(Down()));
    TerainComputePicking(destFeedBackPos, groundRay);
    
    //destFeedBackPos : 목적지 터레인Pos
    //feedBackPos : 현재 터레인Pos

    //방향으로 각도 구하기
    Vector3 destDir = destFeedBackPos - feedBackPos;
    Vector3 destDirXZ = destDir;
    destDirXZ.y = 0;
    
    //각도
    float radianHeightAngle = acos(abs(destDirXZ.Length()) / abs(destDir.Length()));


    if (ColliderManager::Get()->ControlPlayer(&direction) 
        && (radianHeightAngle < XMConvertToRadians(60) || destFeedBackPos.y <= feedBackPos.y)) //각이 60도보다 작아야 이동, 혹은 목적지 높이가 더 낮아야함
    {
        Pos() += direction * moveSpeed * DELTA * -1; // 이동 수행
        feedBackPos.y = destFeedBackPos.y;
    }
        
    //점프상태가 아니라면 현재 지면 높이로 높이 수정
    if(curState != JUMP1 && curState != JUMP2 && curState != JUMP3)
        Pos().y = feedBackPos.y;
}

void Player::Jump()
{
    jumpVel = force1;
}

void Player::AfterJumpAnimation()
{
    SetState(JUMP2);
}

void Player::Jumping()
{
    float tempJumpVel = jumpVel - 9.8f * gravityMult * DELTA;
    float tempY = Pos().y + jumpVel * DELTA * jumpSpeed;

    if(heightLevel < feedBackPos.y)
        heightLevel = feedBackPos.y;

    if (tempY <= heightLevel)
    {
        tempY = heightLevel;
        tempJumpVel = 0.0f;

        if (curState == JUMP2) {
            //landing = landingT;   //점프가 높아서 착지 자세가 다를때 호출하기
            SetState(JUMP3);
        }
    }
    
    Pos().y = tempY;
    jumpVel = tempJumpVel;

    if (jumpVel < 0.0f)
        SetState(JUMP2);
}

void Player::Searching()
{
    //straightRay.dir = Back;
    //straightRay.pos = Pos();

    //diagnolLRay.dir = Back;

    //block manager로 가져오는 코드 작성 가능
}

void Player::Cover()
{    
    targetTransform->Pos() = { contact.hitPoint.x, Pos().y, contact.hitPoint.z };

    Vector3 objectPos = { targetObject->Pos().x, 0, targetObject->Pos().z };
    Vector3 objectDir = Pos() - objectPos;
    objectDir = objectDir.GetNormalized();

    targetTransform->Rot().y = atan2(objectDir.x, objectDir.z) /*+ XM_PI*/;

    targetTransform->Pos() -= BACK * 30;
}

void Player::Assasination()
{
    //targetTransform->Pos() = { contact.x, Pos().y, contact.hitPoint.z };

}

bool Player::InTheAir() {
    return ((curState == JUMP1 || curState == JUMP2 || curState == JUMP3) && Pos().y > feedBackPos.y);
}

void Player::EndHit()
{
    isHit = false;
    collider->SetActive(true);
    SetState(IDLE);
}

bool Player::TerainComputePicking(Vector3& feedback, Ray ray)
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

float Player::GetDamage()
{
    float r = 0.0f;
    if (curState == KICK)
    {
        r = 10.0f;
    }
    return r;
}

void Player::Hit(float damage)
{
    if (!isHit)
    {
        destHP = (curHP - damage > 0) ? curHP - damage : 0;

        collider->SetActive(false);
        if (destHP <= 0)
        {
            //SetState(DYING);
            //게임오버
            return;
        }

        SetState(HIT,0.8f);

        isHit = true;
    }

}

void Player::AttackCombo()
{
    switch (weaponState)
    {
    //case NONE:

    //    break;
    case DAGGER:
        SetState(static_cast<State>(DAGGER1 + comboStack));
        break;
    }

    combo = false;

    comboHolding = 1.5f;
    comboStack++;
    if (comboStack == 3)
        comboStack = 0;
}

void Player::CanCombo()
{
    combo = true;
}

void Player::SetState(State state, float scale, float takeTime)
{
    if (state == curState) return;
    
    curState = state;
    PlayClip((int)state,scale,takeTime);
}

void Player::SetAnimation()
{
    if (curState == JUMP1 || curState == JUMP3 || Pos().y > 0.0f) return;   //높이가 바뀌는 경우가 생기기 때문에 Pos().y의 조건 값을 변수로 바꾸기
 /*   if (toCover)
        return;*/

    if (curState == HIT || curState == KICK || curState == DAGGER1 || curState == DAGGER2 || curState == DAGGER3)
        return;

    if (velocity.z > 0.01f && velocity.x < -0.1f)
        SetState(RUN_DL);
    else if (velocity.z > 0.01f && velocity.x > 0.1f)
        SetState(RUN_DR);
    else if (velocity.z > 0.1f) // 속력 기준이 현재 앞으로 +면
        SetState(RUN_F);
    else if (velocity.z < -0.1f) // 앞 기준 -면
        SetState(RUN_B);
    else if (velocity.x > 0.1f) // 좌우 기준 +면
        SetState(RUN_R);
    else if (velocity.x < -0.1f) //좌우 기준 -면
        SetState(RUN_L);
    else
        SetState(IDLE); // 가만히 있으면
}

void Player::SetIdle()
{

    if (curState == TO_COVER) {
        Pos() = targetTransform->Pos();
        Rot() = targetTransform->Rot();

        SetState(C_IDLE);

        //toCover = false;
    }
    //else if(curState == DAGGER1 && KEY_PRESS(VK_LBUTTON))
    //    SetState(IDLE);
    else
        SetState(IDLE);

}