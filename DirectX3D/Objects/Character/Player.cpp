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
    targetTransform = new Transform();
    //straightRay = Ray(Pos(), Back());

    //Scale() *= 0.1f;
    // ������ �ڵ鷯�� ������(�� ������ ũ��)�� �� ��° �Ű������� ����
    ClientToScreen(hWnd, &clientCenterPos);
    SetCursorPos(clientCenterPos.x, clientCenterPos.y);

    collider = new CapsuleCollider(25.0f, 140);
    collider->SetParent(this);

    footRay = new Ray();
    footRay->pos = Pos();
    footRay->dir = Pos().Down();

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
    //�ӷ�
    ImGui::SliderFloat("moveSpeed", &moveSpeed, 10, 1000);
    ImGui::SliderFloat("rotSpeed", &rotSpeed, 1, 10);
    ImGui::SliderFloat("deceleration", &deceleration, 1, 10);

    //���� ��
    ImGui::SliderFloat("force1", &force1, 1, 500);
    ImGui::SliderFloat("force2", &force2, 1, 500);
    ImGui::SliderFloat("force3", &force3, 1, 500);

    ImGui::SliderFloat("jumpSpeed", &jumpSpeed, 0.01f, 5.0f);
    //�߷�
    ImGui::SliderFloat("gravityMult", &gravityMult, 1, 100);

    //3�� ���� ������ ���� ����
    ImGui::InputFloat("JumpVel", (float*)&jumpVel);
    ImGui::InputFloat("jumpN", (float*)&jumpVel);
    ImGui::InputFloat("nextJump", (float*)&nextJump);

    //������ �ε��ð�
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

    if (targetObject != nullptr && KEY_DOWN('F'))
    {
        velocity = 0;
        SetState(TO_ASSASIN);
    }
}

void Player::Move() //�̵� ����(�⺻ �̵�, �ϻ� �̵�, ���� �� �̵�����, Ư�� �ൿ�� ���� �������� �� �̵� ��)
{
    //�÷��̾��� ������ ���̸� ���� ���� terrain ��ġ�� ���̸� ����
    Vector3 PlayerSkyPos = Pos();
    PlayerSkyPos.y += 100;
    Ray groundRay = Ray(PlayerSkyPos, Vector3(Down()));
    TerainComputePicking(feedBackPos, groundRay);

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

    if (camera)
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
        if (velocity.z - DELTA * 4.0f > 0.0f)
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





    Vector3 destFeedBackPos;
    Vector3 destPos = Pos() + direction * moveSpeed * DELTA * -1;
    Vector3 PlayerSkyPos = destPos;
    PlayerSkyPos.y += 100;
    Ray groundRay = Ray(PlayerSkyPos, Vector3(Down()));
    TerainComputePicking(destFeedBackPos, groundRay);

    //destFeedBackPos : ������ �ͷ���Pos
    //feedBackPos : ���� �ͷ���Pos

    //�������� ���� ���ϱ�
    Vector3 destDir = destFeedBackPos - feedBackPos;
    Vector3 destDirXZ = destDir;
    destDirXZ.y = 0;

    //����
    float radianHeightAngle = acos(abs(destDirXZ.Length()) / abs(destDir.Length()));


    if (ColliderManager::Get()->ControlPlayer(&direction)
        && (radianHeightAngle < XMConvertToRadians(60) || destFeedBackPos.y <= feedBackPos.y)) //���� 70������ �۾ƾ� �̵�, Ȥ�� ������ ���̰� �� ���ƾ���
        Pos() += direction * moveSpeed * DELTA * -1; // �̵� ����

    //�������°� �ƴ϶�� ���� ���� ���̷� ���� ����
    if (curState != JUMP1 && curState != JUMP2 && curState != JUMP3)
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

    float heightLevel = feedBackPos.y;

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