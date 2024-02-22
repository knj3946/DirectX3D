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

    footRay = new Ray();
    footRay->pos = Pos();
    footRay->dir = Pos().Down();

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

    GetClip(JUMP1)->SetEvent(bind(&Player::Jump, this), 0.1f);  //점프시작
    GetClip(JUMP1)->SetEvent(bind(&Player::AfterJumpAnimation, this), 0.20001f);   //하강

    GetClip(JUMP3)->SetEvent(bind(&Player::SetIdle, this), 0.0001f);   //착지

    GetClip(TO_COVER)->SetEvent(bind(&Player::SetIdle, this), 0.05f);   //엄폐

    //GetClip(TO_ASSASIN)->SetEvent(bind(&Player::Assasination, this), 0.01f);   //암살

    computeShader = Shader::AddCS(L"Compute/ComputePicking.hlsl");
    rayBuffer = new RayBuffer();
}

Player::~Player()
{
    delete collider;

    delete targetTransform;
    delete targetObject;
}

void Player::Update()
{
    if (KEY_DOWN(VK_TAB))
        camera = !camera;

    collider->Pos().y = collider->Height() * 0.5f * 33.3f + collider->Radius() * 33.3f;
    collider->UpdateWorld();

    footRay->pos = collider->GlobalPos();
    footRay->dir = Down();

    Control();
    Searching();

    SetAnimation();

    ModelAnimator::Update();
}

void Player::Render()
{
    ModelAnimator::Render();
    collider->Render();
}

void Player::PostRender()
{
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

    ImGui::InputFloat("temp", (float*)&temp);
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

void Player::Control()
{
    if (KEY_PRESS(VK_RBUTTON))
        return;

    Rotate();
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

    if (curState == JUMP3 && landing > 0.0f)    //착지 애니메이션 동안 부동 이동 제한 and 제한 시간 감소
    {
        landing -= DELTA;
        return;
    }

    if (curState == TO_COVER)    //엄폐하러 이동할 경우
    {  
        if (Distance(Pos(), targetTransform->Pos()) < teleport)
        {
            SetState(TO_COVER);
            return;
        }
        Pos() += (targetTransform->Pos() - Pos()).GetNormalized() * DELTA * 400;
        SetState(RUN_F);
        return;
    }

    if (KEY_DOWN(VK_SPACE) && !InTheAir())
    {
        SetState(JUMP1);
    }

    Walking();
}

void Player::Rotate()
{
    Vector3 delta = mousePos - Vector3(CENTER_X, CENTER_Y);
    
    if(camera)
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
        && (radianHeightAngle < XMConvertToRadians(60) || destFeedBackPos.y <= feedBackPos.y)) //각이 70도보다 작아야 이동, 혹은 목적지 높이가 더 낮아야함
        Pos() += direction * moveSpeed * DELTA * -1; // 이동 수행

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

<<<<<<< HEAD
=======
    float heightLevel = feedBackPos.y;

>>>>>>> c1ecc0ed25aaa49c606e267100ed98a2b707dcbf
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

void Player::SetState(State state)
{
    if (state == curState) return;

    curState = state;
    PlayClip((int)state);
}

void Player::SetAnimation()
{
    if (curState == JUMP1 || curState == JUMP3 || Pos().y > 0.0f) return;   //높이가 바뀌는 경우가 생기기 때문에 Pos().y의 조건 값을 변수로 바꾸기
 /*   if (toCover)
        return;*/

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
    else
        SetState(IDLE);
}