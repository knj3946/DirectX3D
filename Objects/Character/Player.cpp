#include "Framework.h"
#include "Player.h"

Player::Player()
    : ModelAnimator("akai")
{
    targetTransform = new Transform();

    ClientToScreen(hWnd, &clientCenterPos);
    SetCursorPos(clientCenterPos.x, clientCenterPos.y);

    collider = new CapsuleCollider(25.0f, 140);
    collider->SetParent(this);
    collider->Pos().y += 20;

    rightHand = new Transform();
    leftHand = new Transform();

    dagger = new Dagger(rightHand);

    bow = new Model("hungarian_bow");
    bow->SetParent(leftHand);
    bow->Scale() *= 0.3f;
    bow->Pos() = { 1.700, -1.6, 0 };
    bow->Rot() = { 0, -0.200, 2.5 };

    bowCol = new CapsuleCollider(25, 130);
    bowCol->SetParent(bow);


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

    string temp;

    ReadClip("Idle");

    ReadClip("Medium Run");
    ReadClip("Running Backward");
    ReadClip("Left Strafe");
    ReadClip("Right Strafe");

    ReadClip("Jog Forward Diagonal Left");
    ReadClip("Jog Forward Diagonal Right");

    ReadClip("Jumping"); 
    ReadClip("IntheSky");
    ReadClip("Falling To Landing"); 

    ReadClip("Stand To Cover"); 
    ReadClip("Cover Idle"); 
    ReadClip("Crouched Sneaking Right"); 
    ReadClip("Crouched Sneaking Left");
    ReadClip("Crouch Turn To Stand");

    ReadClip("Turning Right");

    ReadClip("Head Hit");

    temp = "Attack/";
    //NONE
    ReadClip(temp + "Side Kick");

    //DAGGER
    ReadClip(temp + "Stable Sword Outward Slash");
    ReadClip(temp + "Stable Sword Inward Slash");
    ReadClip(temp + "Upward Thrust");

    temp = "Bow/Walk/";

    ReadClip(temp + "Standing Idle");
    ReadClip(temp + "Standing Sprint Forward");
    ReadClip(temp + "Standing Walk Back");
    ReadClip(temp + "Standing Run Left");
    ReadClip(temp + "Standing Run Right");

    temp = "Bow/Attack/";

    ReadClip(temp + "Standing Draw Arrow");
    ReadClip(temp + "Standing Aim Overdraw");
    ReadClip(temp + "Standing Aim Recoil");


    GetClip(JUMP1)->SetEvent(bind(&Player::Jump, this), 0.1f);  //????????
    GetClip(JUMP1)->SetEvent(bind(&Player::AfterJumpAnimation, this), 0.20001f);   //???

    GetClip(JUMP3)->SetEvent(bind(&Player::SetIdle, this), 0.0001f);   //????

    GetClip(TO_COVER)->SetEvent(bind(&Player::SetIdle, this), 0.05f);   //????

    //GetClip(TO_ASSASIN)->SetEvent(bind(&Player::Assasination, this), 0.01f);   //???

    GetClip(HIT)->SetEvent(bind(&Player::EndHit, this), 0.35f); // ????? ??????

    GetClip(DAGGER1)->SetEvent(bind(&Player::SetIdle, this), 0.6f);
    GetClip(DAGGER2)->SetEvent(bind(&Player::SetIdle, this), 0.6f);
    GetClip(DAGGER3)->SetEvent(bind(&Player::SetIdle, this), 0.6f);

    //GetClip(B_DRAW)->SetEvent(bind(&Player::))

    //GetClip(DAGGER1)->SetEvent(bind(&Player::CanCombo, this), 0.4f);

    //GetClip(HIT)->SetEvent(bind(&Player::EndHit, this), 0.35f);
}

Player::~Player()
{
    delete collider;

    delete hpBar;

    delete rayBuffer;
    delete structuredBuffer;

    delete targetTransform;
    delete targetObject;

    delete rightHand;
    delete leftHand;
    delete dagger;
    delete bow;
    delete bowCol;

    delete leftFoot;
    delete leftFootCollider;
    delete rightFoot;
    delete rightFootCollider;
}

void Player::Update()
{
    leftFootCollider->SetActive(false);
    rightFootCollider->SetActive(false);

    ColliderManager::Get()->SetHeight();
    if (!isCeiling)
        ColliderManager::Get()->PushPlayer();

    collider->Pos().y = collider->Height() * 0.5f * 33.3f + collider->Radius() * 33.3f;
    collider->UpdateWorld();

    UpdateUI();

    Control();
    Searching();
    Targeting();

    SetAnimation();

    ModelAnimator::Update();

    if (comboHolding > 0.0f)
        comboHolding -= DELTA;  //???? ???, ????��? ?? ?��? ???? ??? ?????
    else
    {
        comboHolding = 0.0f;
        comboStack = 0;
    }

    rightHand->SetWorld(this->GetTransformByNode(rightHandNode));
    leftHand->SetWorld(this->GetTransformByNode(leftHandNode));
    if(weaponState == DAGGER)
        dagger->Update();
    bow->UpdateWorld();
    bowCol->UpdateWorld();

    leftFoot->SetWorld(this->GetTransformByNode(leftFootNode));
    leftFootCollider->UpdateWorld();

    rightFoot->SetWorld(this->GetTransformByNode(rightFootNode));
    rightFootCollider->UpdateWorld();

}

void Player::Render()
{
    ModelAnimator::Render();
    collider->Render();

    leftFootCollider->Render();
    rightFootCollider->Render();

    dagger->Render();
    bow->Render();
    bowCol->Render();
}

void Player::PostRender()
{
    hpBar->Render();
}

void Player::GUIRender()
{
    Model::GUIRender();

    ImGui::DragFloat3("Velocity", (float*)&velocity, 0.5f);
    //???
    ImGui::SliderFloat("moveSpeed", &moveSpeed, 10, 1000);
    ImGui::SliderFloat("rotSpeed", &rotSpeed, 1, 10);
    ImGui::SliderFloat("deceleration", &deceleration, 1, 10);
    ImGui::Text("isPushed : %d", isPushed);
    ImGui::Text("feedBackPosY : %f", feedBackPos.y);
    ImGui::Text("Pos.y : %f", Pos().y);
    ImGui::Text("heightLevel : %f", heightLevel);

    //???? ??
    ImGui::SliderFloat("force1", &force1, 1, 500);
    ImGui::SliderFloat("force2", &force2, 1, 500);
    ImGui::SliderFloat("force3", &force3, 1, 500);

    ImGui::SliderFloat("jumpSpeed", &jumpSpeed, 0.01f, 5.0f);
    //???
    ImGui::SliderFloat("gravityMult", &gravityMult, 1, 100);

    //3?? ???? ?????? ???? ????
    ImGui::InputFloat("JumpVel", (float*)&jumpVel);
    ImGui::InputFloat("jumpN", (float*)&jumpVel);
    ImGui::InputFloat("nextJump", (float*)&nextJump);

    //?????? ?��??��?
    ImGui::InputFloat("landingT", (float*)&landingT);
    ImGui::InputFloat("landing", (float*)&landing);

    ImGui::InputInt("rightHandNode", &rightHandNode);
    ImGui::InputInt("leftHandNode", &leftHandNode);
    ImGui::InputInt("leftFootNode", &leftFootNode);
    ImGui::InputInt("rightFootNode", &rightFootNode);

    ImGui::DragFloat3("bowPos", (float*)&bow->Pos(), 0.1f);
    ImGui::DragFloat3("bowRot", (float*)&bow->Rot(), 0.1f);
    ImGui::DragFloat3("bowScale", (float*)&bow->Scale(), 0.1f);

    dagger->GUIRender();

    ColliderManager::Get()->GuiRender();
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

void Player::Control()  //??????? ?????, ???�J ??? ???
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

    if (KEY_PRESS(VK_LBUTTON))
<<<<<<< HEAD
    {
        if (weaponState == DAGGER)
        {
            if (curState != DAGGER1 && curState != DAGGER2 && curState != DAGGER3)
                ComboAttack();
        }
        else if (weaponState == BOW)
        {
            if (curState == B_IDLE)
                SetState(B_DRAW);
        }
    }
=======
        if (curState != DAGGER1 && curState != DAGGER2 && curState != DAGGER3)
            ComboAttack();
>>>>>>> a5c054ed44e6c11db8a44248124781545ef28c0f

    if (isHit) 
    {
        return;
    }

    if (KEY_DOWN(VK_SPACE) && !InTheAir())
    {
        SetState(JUMP1);
    }

    Move();
    Jumping();

    //if (targetObject != nullptr && KEY_DOWN('F'))     ////����
    //{
    //    velocity = 0;
    //    SetState(TO_ASSASIN);
    //}
}

void Player::Move() //??? ????(?? ???, ??? ???, ???? ?? ???????, ??? ???? ???? ???????? ?? ??? ??)
{
    //�÷��̾��� ������ ���̸� ���� ���� terrain ��ġ�� ���̸� ����
<<<<<<< HEAD

    //if (!OnColliderFloor(feedBackPos))
    //{
        Vector3 PlayerSkyPos = Pos();
        PlayerSkyPos.y += 1000;
        Ray groundRay = Ray(PlayerSkyPos, Vector3(Down()));
        TerainComputePicking(feedBackPos, groundRay);
    //}
=======
    /*
    if (!OnColliderFloor(feedBackPos))
    {

    }
    */
    Vector3 PlayerSkyPos = Pos();
    PlayerSkyPos.y += 1000;
    Ray groundRay = Ray(PlayerSkyPos, Vector3(Down()));
    TerainComputePicking(feedBackPos, groundRay);
>>>>>>> a5c054ed44e6c11db8a44248124781545ef28c0f

    //if (curState == JUMP3 && landing > 0.0f)    //???? ??????? ???? ?��? ??? ???? and ???? ?��? ????
    //{
    //    landing -= DELTA;
    //    return;
    //}

    //if (curState == TO_COVER)    //??????? ????? ???
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
    bool isMoveZ = false; // ???? ??? ?? : ???? "???"
    bool isMoveX = false; // ?��? ??? ?? : ???? "???"

    if (KEY_PRESS('W'))
    {
        if (velocity.z + DELTA * 4.0f < 0.0f)
            velocity.z += DELTA * 4.0f;
        else
            velocity.z += DELTA; //??? ????? ?��? ?????? ?????? ???

        isMoveZ = true; //???? ??? ????
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
            velocity.x += DELTA; //??? ????? ?��? ?????? ?????? ???

        isMoveX = true;
    }

    if (velocity.Length() > 1) velocity.Normalize();

    if (!isMoveZ)
        velocity.z = Lerp(velocity.z, 0, deceleration * DELTA); //???????? ????

    if (!isMoveX)
        velocity.x = Lerp(velocity.x, 0, deceleration * DELTA);

    Matrix rotY = XMMatrixRotationY(Rot().y);
    Vector3 direction = XMVector3TransformCoord(velocity, rotY);


    Vector3 destFeedBackPos;
    Vector3 destPos = Pos() + direction * moveSpeed * DELTA * -1;
    Vector3 PlayerSkyPos = destPos;
    PlayerSkyPos.y += 1000;
    Ray groundRay = Ray(PlayerSkyPos, Vector3(Down()));
    /*
    if (!OnColliderFloor(destFeedBackPos))
    {
        
    }*/

    TerainComputePicking(destFeedBackPos, groundRay);

    //destFeedBackPos : ������ �ͷ���Pos
    //feedBackPos : ���� �ͷ���Pos

    //???????? ???? ?????
    Vector3 destDir = destFeedBackPos - feedBackPos;
    Vector3 destDirXZ = destDir;
    destDirXZ.y = 0;

    //????
    float radianHeightAngle = acos(abs(destDirXZ.Length()) / abs(destDir.Length()));


    if (/*ColliderManager::Get()->ControlPlayer(&direction)*/ !isPushed
        && (radianHeightAngle < XMConvertToRadians(60) || destFeedBackPos.y <= feedBackPos.y)) //???? 60?????? ???? ???, ??? ?????? ????? ?? ???????
    {
        Pos() += direction * moveSpeed * DELTA * -1; // ??? ????
        feedBackPos.y = destFeedBackPos.y;
    }

    //???????��? ????? ???? ???? ????? ???? ????
    if (curState != JUMP1 && curState != JUMP2 && curState != JUMP3)
        Pos().y = feedBackPos.y;
}

void Player::Jump()
{
    jumpVel = force3;
}

void Player::AfterJumpAnimation()
{
    SetState(JUMP2);
}

void Player::Jumping()
{
    if (weaponState == DAGGER)
    {
        if (heightLevel < feedBackPos.y)
            heightLevel = feedBackPos.y;

<<<<<<< HEAD
        if (isCeiling) {
            jumpVel = -20;
            isCeiling = false;
=======
        tempJumpVel = jumpVel - 9.8f * gravityMult * DELTA;
        tempY = preHeight + jumpVel * DELTA * jumpSpeed;
    }
    else
    {
        tempJumpVel = jumpVel - 9.8f * gravityMult * DELTA;
        tempY = Pos().y + jumpVel * DELTA * jumpSpeed;
    }

    //heightLevel = feedBackPos.y;

    if (tempY <= heightLevel)
    {
        tempY = heightLevel;
        tempJumpVel = 0.0f;

        if (curState == JUMP2) {
            //landing = landingT;   //?????? ????? ???? ????? ????? ??????
            SetState(JUMP3);
>>>>>>> a5c054ed44e6c11db8a44248124781545ef28c0f
        }

        float tempJumpVel;
        float tempY;
        if (preHeight - heightLevel > 5.0f)
        {
            jumpVel = -1;

            tempJumpVel = jumpVel - 9.8f * gravityMult * DELTA;
            tempY = preHeight + jumpVel * DELTA * jumpSpeed;
        }
        else
        {
            tempJumpVel = jumpVel - 9.8f * gravityMult * DELTA;
            tempY = Pos().y + jumpVel * DELTA * jumpSpeed;
        }

        //heightLevel = feedBackPos.y;

        if (tempY <= heightLevel)
        {
            tempY = heightLevel;
            tempJumpVel = 0.0f;

            if (curState == JUMP2) {
                //landing = landingT;   //?????? ????? ???? ????? ????? ??????
                SetState(JUMP3);
            }
        }

        Pos().y = tempY;
        jumpVel = tempJumpVel;

        if (jumpVel < 0.0f)
            SetState(JUMP2);

        preHeight = heightLevel;
    }
    else if (weaponState == BOW)
    {

    }
}

void Player::Searching()
{
    //straightRay.dir = Back;
    //straightRay.pos = Pos();

    //diagnolLRay.dir = Back;

    //block manager?? ???????? ??? ??? ????
}

void Player::Targeting()
{
    Ray mouseRay = CAM->ScreenPointToRay(mousePos);

    MonsterManager::Get()->OnOutLineByRay(mouseRay);
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
/*
bool Player::OnColliderFloor(Vector3& feedback)
{
    Vector3 PlayerSkyPos = Pos();
    PlayerSkyPos.y += 5;
    Ray groundRay = Ray(PlayerSkyPos, Vector3(Down()));
    Contact con;
    if (ColliderManager::Get()->CloseRayCollisionColliderContact(groundRay, con))
    {
        feedback = con.hitPoint;
        feedback.y += 0.1f; //��¦ ������μ� �浹 ����
        return true;
    }
    
    return false;
}
*/
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
            //???????
            return;
        }

        SetState(HIT, 0.8f);

        isHit = true;
    }

}

void Player::ComboAttack()
{
    switch (weaponState)
    {
        //case NONE:

        //    break;
    case DAGGER:
        SetState(static_cast<State>(DAGGER1 + comboStack));
        break;
    case BOW:
        //SetState(static_cast<State>(DAGGER1 + comboStack));
        break;
    }

    comboHolding = 1.5f;
    comboStack++;
    if (comboStack == 3)
        comboStack = 0;
}

void Player::SetState(State state, float scale, float takeTime)
{
    if (state == curState) return;

    curState = state;
    PlayClip((int)state, scale, takeTime);
}

void Player::SetAnimation()
{
    if (weaponState == DAGGER)
    {
        if (curState == JUMP1 || curState == JUMP3 || Pos().y > 0.0f) return;   //????? ???? ??�� ????? ?????? Pos().y?? ???? ???? ?????? ????
        /*   if (toCover)
               return;*/

        if (curState == HIT || curState == KICK || curState == DAGGER1 || curState == DAGGER2 || curState == DAGGER3)
            return;

        if (velocity.z > 0.01f && velocity.x < -0.1f)
            SetState(RUN_DL);
        else if (velocity.z > 0.01f && velocity.x > 0.1f)
            SetState(RUN_DR);
        else if (velocity.z > 0.1f)
            SetState(RUN_F);
        else if (velocity.z < -0.1f)
            SetState(RUN_B);
        else if (velocity.x > 0.1f)
            SetState(RUN_R);
        else if (velocity.x < -0.1f)
            SetState(RUN_L);
        else
            SetState(IDLE);
    }
    else if (weaponState == BOW)
    {
        if (velocity.z > 0.1f)
            SetState(B_RUN_F);
        else if (velocity.z < -0.1f)
            SetState(B_RUN_B);
        else if (velocity.x > 0.1f)
            SetState(B_RUN_R);
        else if (velocity.x < -0.1f)
            SetState(B_RUN_L);
        else
            SetState(B_IDLE);
    }
}

void Player::SetIdle()
{
    if (weaponState == DAGGER)
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
    else if (weaponState == BOW)
    {
        SetState(B_IDLE);
    }
}