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
    CAM->Pos() = { -309.720, 742.889, -247.274 };
    CAM->Rot() = { 75, 90, 0 };

    targetTransform = new Transform();
    ray = Ray(Pos(), Back());
    //Scale() *= 0.1f;
    // 윈도우 핸들러의 정보값(중 윈도우 크기)을 두 번째 매개변수에 저장
    //ClientToScreen(hWnd, &clientCenterPos);
    //SetCursorPos(clientCenterPos.x, clientCenterPos.y);
    collider = new CapsuleCollider(25.0f, 160);
    collider->SetParent(this);
    //collider->SetPivot({ 0, Pos().y + collider->Height() / 2.0f + collider->Radius(), 0 });

    ReadClip("Standing Idle"); // 동작이 0뒤에 1까지 있음

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

    GetClip(JUMP1)->SetEvent(bind(&Player::Jump, this), 0.00001f);  //점프시작
    GetClip(JUMP1)->SetEvent(bind(&Player::EndJump, this), 0.20001f);   //하강

    GetClip(JUMP3)->SetEvent(bind(&Player::SetIdle, this), 0.0001f);   //착지

    GetClip(TO_COVER)->SetEvent(bind(&Player::SetIdle, this), 0.05f);   //엄폐
    //GetClip(TO_COVER)->SetEvent(bind(&Player::Cover, this), 0.0001f);   //착지
}

Player::~Player()
{
    delete collider;

    delete targetTransform;
    delete targetObject;
}

void Player::Update()
{

    //if (Pos().y > highestY)
    //    highestY = Pos().y;
    collider->Pos().y = collider->Height()/2.0f + collider->Radius();
    collider->UpdateWorld();
    //collider->SetPivot({ 0, Pos().y + collider->Height() / 2.0f + collider->Radius(), 0 });

    if(curState != TO_COVER || curState != C_IDLE)
        Control();

    SetAnimation();

    Searching();

    ModelAnimator::Update(); //모델 업데이트

    if(targetObject != nullptr)
        targetObject->SetColor({ 1, 0, 0, 0 });
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
    ImGui::SliderFloat("force1", &force1, 100, 1000);
    ImGui::SliderFloat("force2", &force2, 100, 1000);
    ImGui::SliderFloat("force3", &force3, 100, 1000);

    ImGui::SliderFloat("jumpSpeed", &jumpSpeed, 1.0f, 3.0f);
    //중력
    ImGui::SliderFloat("gravityMult", &gravityMult, 100, 1000);

    //3단 점프 구현시 조건 변수
    ImGui::InputFloat("JumpVel", (float*)&jumpVel);
    ImGui::InputFloat("jumpN", (float*)&jumpVel);
    ImGui::InputFloat("nextJump", (float*)&nextJump);

    //착지후 부동시간
    ImGui::InputFloat("landingT", (float*)&landingT);
    ImGui::InputFloat("landing", (float*)&landing);

    ImGui::InputFloat("teleport", (float*)&teleport);
}

void Player::Control()
{
    Rotate();
    if (!KEY_PRESS(VK_RBUTTON)) {
        Move();
        Attack();
        Jumping();
    }

    if(targetObject != nullptr && KEY_DOWN('F'))
    {
        velocity = 0;
        Cover();
    }
}

void Player::Move()
{
    if (toCover) {
        if (Distance(Pos(), targetTransform->Pos()) < teleport)
        {
            SetState(TO_COVER);
            return;
        }
        Pos() += (targetTransform->Pos() - Pos()).GetNormalized() * DELTA * 400;
        SetState(RUN_F);
        return;
    }

    if (curState == JUMP3 && landing > 0.0f)    //착지후 부동 시간 감소
    {
        landing -= DELTA;
        return;
    }

    if (KEY_DOWN(VK_SPACE) && !InTheAir())
    {
        SetState(JUMP1);
        //Jump();
    }

    Walking();
}

void Player::Rotate()
{
    ////아래 코드 때문에 중간으로 고정된 커서가 다시 움직이면서 델타 생성
    //Vector3 delta = mousePos - Vector3(CENTER_X, CENTER_Y);
    //// 마우스가 움직일 때마다 위치를 중간으로 고정
    //SetCursorPos(clientCenterPos.x, clientCenterPos.y);
    //// -> 혹시 위 두 줄이 조작 시에 잘 안 된다면 CENTER_XY와 clinetCenterPos 표시를 바꿔보면 될지도

    ////델타에 의한 캐릭터와 카메라 양쪽을 모두 회전
    //Rot().y += delta.x * rotSpeed * DELTA; // 캐릭터 좌우회전 (추적 중이라 카메라도 따라갈 것)
    //CAM->Rot().x -= delta.y * rotSpeed * DELTA; // 카메라 상하회전
    
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

    //트랜스폼의 방향에 의한 회전정보를 면적으로 바꾸기
    Matrix rotY = XMMatrixRotationY(Rot().y);
    //면적(=그 면적에서 나온 수직선)으로 된 회전 정보와 속력 기준을 합쳐서 만든 방향
    //=현재 회전 상황에서 속력 기준으로 향하기 위한 방향을 계산하는 벡터 보간
    Vector3 direction = XMVector3TransformCoord(velocity, rotY);

    ColliderManager::Get()->ControlPlayer(&direction);

    Pos() += direction * moveSpeed * DELTA * -1; // 이동 수행
}

void Player::Attack()
{
}

void Player::Jump()
{
    jumpVel = force1;
}

void Player::Jumping()
{
    //    jumpVel -= 9.8f * gravityMult * DELTA;
    //    Pos().y += jumpVel * DELTA;

    float tempJumpVel = jumpVel - 9.8f * gravityMult * DELTA;
    float tempY = Pos().y + jumpVel * DELTA * jumpSpeed;

    //float heightLevel = BlockManager::Get()->GetHeight(Pos());
    float heightLevel = 0.0f;

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
        EndJump();
}

void Player::SetAnimation()
{
    //if (InTheAir()) return;
    if (curState == JUMP1 || curState == JUMP3 || Pos().y > 0.0f) return;   //높이가 바뀌는 경우가 생기기 때문에 Pos().y의 조건 값을 변수로 바꾸기
    if (toCover) 
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

void Player::SetState(State state)
{
    if (state == curState) return;

    curState = state;
    PlayClip((int)state);
}

void Player::EndJump()
{
    //Pos().y += jumpOffset;
    SetState(JUMP2);
}

void Player::Cover()
{    
    toCover = true;

    targetTransform->Pos() = { contact.hitPoint.x, Pos().y, contact.hitPoint.z };

    Vector3 objectPos = { targetObject->Pos().x, 0, targetObject->Pos().z };
    Vector3 objectDir = Pos() - objectPos;
    objectDir = objectDir.GetNormalized();

    targetTransform->Rot().y = atan2(objectDir.x, objectDir.z) /*+ XM_PI*/;

    targetTransform->Pos() -= Back() * 30;
}

bool Player::InTheAir() {
    return ((curState == JUMP1 || curState == JUMP2 || curState == JUMP3) && Pos().y > 0.0f);
}

void Player::SetIdle()
{
    if (curState == TO_COVER) {
        Pos() = targetTransform->Pos();
        Rot() = targetTransform->Rot();

        SetState(C_IDLE);

        toCover = false;
    }
    else
        SetState(IDLE);
}

void Player::Searching()
{
    ray.dir = Back();
    ray.pos = Pos();

    //block manager로 가져오는 코드 작성 가능
}

void Player::Wall(BoxCollider* other)
{
    if(collider->IsBoxCollision(other))
    {
    }
}