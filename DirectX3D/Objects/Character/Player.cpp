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
//    //float heightLevel = 0; // ���� ���� (����� 0)
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
    //straightRay = Ray(Pos(), Back());

    //Scale() *= 0.1f;
    // ������ �ڵ鷯�� ������(�� ������ ũ��)�� �� ��° �Ű������� ����
    //ClientToScreen(hWnd, &clientCenterPos);
    //SetCursorPos(clientCenterPos.x, clientCenterPos.y);

    collider = new CapsuleCollider(25.0f, 160);
    collider->SetParent(this);

    ReadClip("Idle"); // ������ 0�ڿ� 1���� ����

    ReadClip("Medium Run");
    ReadClip("Running Backward");
    ReadClip("Left Strafe");
    ReadClip("Right Strafe");

    ReadClip("Jog Forward Diagonal Left");
    ReadClip("Jog Forward Diagonal Right");

    ReadClip("Jumping"); // ������ 0�ڿ� 1���� ����
    ReadClip("IntheSky"); // ������ 0�ڿ� 1���� ����
    ReadClip("Falling To Landing"); // ������ 0�ڿ� 1���� ����

    ReadClip("Stand To Cover"); // ������ 0�ڿ� 1���� ����
    ReadClip("Cover Idle"); // ������ 0�ڿ� 1���� ����
    ReadClip("Crouched Sneaking Right"); // ������ 0�ڿ� 1���� ����
    ReadClip("Crouched Sneaking Left"); // ������ 0�ڿ� 1���� ����
    ReadClip("Crouch Turn To Stand"); // ������ 0�ڿ� 1���� ����

    GetClip(JUMP1)->SetEvent(bind(&Player::Jump, this), 0.1f);  //��������
    GetClip(JUMP1)->SetEvent(bind(&Player::AfterJumpAnimation, this), 0.20001f);   //�ϰ�

    GetClip(JUMP3)->SetEvent(bind(&Player::SetIdle, this), 0.0001f);   //����

    GetClip(TO_COVER)->SetEvent(bind(&Player::SetIdle, this), 0.05f);   //����

    //GetClip(TO_ASSASIN)->SetEvent(bind(&Player::Assasination, this), 0.01f);   //�ϻ�
}

Player::~Player()
{
    delete collider;

    delete targetTransform;
    delete targetObject;
}

void Player::Update()
{
    collider->Pos().y = collider->Height() / 2.0f + collider->Radius();
    collider->UpdateWorld();

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
    //�ӷ�
    ImGui::SliderFloat("moveSpeed", &moveSpeed, 10, 1000);
    ImGui::SliderFloat("rotSpeed", &rotSpeed, 1, 10);
    ImGui::SliderFloat("deceleration", &deceleration, 1, 10);

    //���� ��
    ImGui::SliderFloat("force1", &force1, 100, 1000);
    ImGui::SliderFloat("force2", &force2, 100, 1000);
    ImGui::SliderFloat("force3", &force3, 100, 1000);

    ImGui::SliderFloat("jumpSpeed", &jumpSpeed, 1.0f, 3.0f);
    //�߷�
    ImGui::SliderFloat("gravityMult", &gravityMult, 100, 1000);

    //3�� ���� ������ ���� ����
    ImGui::InputFloat("JumpVel", (float*)&jumpVel);
    ImGui::InputFloat("jumpN", (float*)&jumpVel);
    ImGui::InputFloat("nextJump", (float*)&nextJump);

    //������ �ε��ð�
    ImGui::InputFloat("landingT", (float*)&landingT);
    ImGui::InputFloat("landing", (float*)&landing);

    ImGui::InputFloat("teleport", (float*)&teleport);
}

void Player::Control()
{
    if (KEY_PRESS(VK_RBUTTON))
        return;

    //Rotate();
    Move();
    Jumping();

    if(targetObject != nullptr && KEY_DOWN('F'))
    {
        velocity = 0;
        SetState(TO_ASSASIN);
    }
}

void Player::Move() //�̵� ����(�⺻ �̵�, �ϻ� �̵�, ���� �� �̵�����, Ư�� �ൿ�� ���� �������� �� �̵� ��)
{
    if (curState == JUMP3 && landing > 0.0f)    //���� �ִϸ��̼� ���� �ε� �̵� ���� and ���� �ð� ����
    {
        landing -= DELTA;
        return;
    }

    if (curState == TO_COVER)    //�����Ϸ� �̵��� ���
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
    //// ���콺�� ������ ������ ��ġ�� �߰����� ����
    SetCursorPos(CENTER_X, CENTER_Y);
    //// -> Ȥ�� �� �� ���� ���� �ÿ� �� �� �ȴٸ� CENTER_XY�� clinetCenterPos ǥ�ø� �ٲ㺸�� ������

    ////��Ÿ�� ���� ĳ���Ϳ� ī�޶� ������ ��� ȸ��
    Rot().y += delta.x * rotSpeed * DELTA; // ĳ���� �¿�ȸ�� (���� ���̶� ī�޶� ���� ��)
    CAM->Rot().x -= delta.y * rotSpeed * DELTA; // ī�޶� ����ȸ��
    
    if (KEY_PRESS('Q'))
        Rot().y -= DELTA * 2;
    if (KEY_PRESS('E'))
        Rot().y += DELTA * 2;
}

void Player::Walking()
{
    bool isMoveZ = false; // ���� �̵� �� : �⺻�� "�ƴ�"
    bool isMoveX = false; // �¿� �̵� �� : �⺻�� "�ƴ�"

    if (KEY_PRESS('W'))
    {
        if (velocity.z + DELTA * 4.0f < 0.0f)
            velocity.z += DELTA * 4.0f;
        else
            velocity.z += DELTA; //�ӷ� ���ؿ� �ð� �����ŭ ������ �ֱ�

        isMoveZ = true; //���� �̵� ����
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
            velocity.x += DELTA; //�ӷ� ���ؿ� �ð� �����ŭ ������ �ֱ�

        isMoveX = true;
    }

    if (velocity.Length() > 1) velocity.Normalize();

    if (!isMoveZ)
        velocity.z = Lerp(velocity.z, 0, deceleration * DELTA); //�������� ����

    if (!isMoveX)
        velocity.x = Lerp(velocity.x, 0, deceleration * DELTA);

    Matrix rotY = XMMatrixRotationY(Rot().y);
    Vector3 direction = XMVector3TransformCoord(velocity, rotY);

    if (ColliderManager::Get()->ControlPlayer(&direction))
        Pos() += direction * moveSpeed * DELTA * -1; // �̵� ����
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

    float heightLevel = 0.0f;

    if (tempY <= heightLevel)
    {
        tempY = heightLevel;
        tempJumpVel = 0.0f;

        if (curState == JUMP2) {
            //landing = landingT;   //������ ���Ƽ� ���� �ڼ��� �ٸ��� ȣ���ϱ�
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

    //block manager�� �������� �ڵ� �ۼ� ����
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
    return ((curState == JUMP1 || curState == JUMP2 || curState == JUMP3) && Pos().y > 0.0f);
}

void Player::SetState(State state)
{
    if (state == curState) return;

    curState = state;
    PlayClip((int)state);
}

void Player::SetAnimation()
{
    if (curState == JUMP1 || curState == JUMP3 || Pos().y > 0.0f) return;   //���̰� �ٲ�� ��찡 ����� ������ Pos().y�� ���� ���� ������ �ٲٱ�
 /*   if (toCover)
        return;*/

    if (velocity.z > 0.01f && velocity.x < -0.1f)
        SetState(RUN_DL);
    else if (velocity.z > 0.01f && velocity.x > 0.1f)
        SetState(RUN_DR);
    else if (velocity.z > 0.1f) // �ӷ� ������ ���� ������ +��
        SetState(RUN_F);
    else if (velocity.z < -0.1f) // �� ���� -��
        SetState(RUN_B);
    else if (velocity.x > 0.1f) // �¿� ���� +��
        SetState(RUN_R);
    else if (velocity.x < -0.1f) //�¿� ���� -��
        SetState(RUN_L);
    else
        SetState(IDLE); // ������ ������
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