#include "Framework.h"
#include "Player.h"

Player::Player()
    : ModelAnimator("akai")
{
    ArrowManager::Get();

    targetTransform = new Transform();
    //straightRay = Ray(Pos(), Back());

    //Scale() *= 0.1f;
    // ?????? ????? ??????(?? ?????? ???)?? ?? ??° ????????? ????
    ClientToScreen(hWnd, &clientCenterPos);
    SetCursorPos(clientCenterPos.x, clientCenterPos.y);

    collider = new CapsuleCollider(25.0f, 140);
    collider->SetParent(this);
    collider->Pos().y += 20;

    rightHand = new Transform();
    leftHand = new Transform();

    dagger = new Dagger(rightHand);

    weaponColliders.push_back(dagger->GetCollider());

    bow = new Model("Elven Long Bow");
    bow->SetParent(leftHand);
    bow->Scale() *= 6.f;
    bow->Pos() = { 0, 6.300, -2.400 };
    bow->Rot() = { 3.000, 0, 0 };


    //left foot : 57
    leftFoot = new Transform();
    leftFootCollider = new CapsuleCollider(10, 50);
    leftFootCollider->SetParent(leftFoot);
    //weaponColliders.push_back(leftFootCollider);

    //right foot : 62
    rightFoot = new Transform();
    rightFootCollider = new CapsuleCollider(10, 50);
    rightFootCollider->SetParent(rightFoot);
    //weaponColliders.push_back(rightFootCollider);

    // 무기 콜라이더 공격할때만 활성화
    leftFootCollider->SetActive(false);
    rightFootCollider->SetActive(false);
    dagger->GetCollider()->SetActive(false);

    computeShader = Shader::AddCS(L"Compute/ComputePicking.hlsl");
    rayBuffer = new RayBuffer();

    hpBar = new ProgressBar(L"Textures/UI/hp_bar2.png", L"Textures/UI/hp_bar2_BG.png");
    hpBar->Scale().y *= 0.3f;
    hpBar->Scale().x *= 0.2f;
    hpBar->Pos() = { 403, WIN_HEIGHT - 30, 0 };
    hpBar->SetAmount(curHP / maxHp);
    hpBar->UpdateWorld();

    string temp;    //???? ????

    ReadClip("Idle"); // ?????? 0??? 1???? ????

    ReadClip("Medium Run");
    ReadClip("Running Backward");
    ReadClip("Left Strafe");
    ReadClip("Right Strafe");

    ReadClip("Jog Forward Diagonal Left");
    ReadClip("Jog Forward Diagonal Right");

    ReadClip("Jumping"); // ?????? 0??? 1???? ????
    ReadClip("IntheSky"); // ?????? 0??? 1???? ????
    ReadClip("Falling To Landing"); // ?????? 0??? 1???? ????

    ReadClip("Stand To Cover"); // ?????? 0??? 1???? ????
    ReadClip("Cover Idle"); // ?????? 0??? 1???? ????
    ReadClip("Crouched Sneaking Right"); // ?????? 0??? 1???? ????
    ReadClip("Crouched Sneaking Left"); // ?????? 0??? 1???? ????
    ReadClip("Crouch Turn To Stand"); // ?????? 0??? 1???? ????

    ReadClip("Head Hit");

    ReadClip("Climbing1");
    ReadClip("Climbing2");
    ReadClip("Climbing3");

    temp = "Attack/";

    //NONE
    ReadClip(temp + "Side Kick");
    ReadClip(temp + "Assassination1");
    ReadClip(temp + "Assassination2");

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
    ReadClip(temp + "Standing Aim Idle");
    ReadClip(temp + "Standing Aim Recoil");

    temp = "Bow/Crouch/";

    ReadClip(temp + "Standing To Crouch");
    ReadClip(temp + "Crouch To Standing");
    ReadClip(temp + "Crouch Idle");
    ReadClip(temp + "Crouch Walk Forward");
    ReadClip(temp + "Crouch Walk Back");
    ReadClip(temp + "Crouch Walk Left");
    ReadClip(temp + "Crouch Walk Right");



    GetClip(JUMP1)->SetEvent(bind(&Player::Jump, this), 0.1f);  //????????
    GetClip(JUMP1)->SetEvent(bind(&Player::AfterJumpAnimation, this), 0.20001f);   //???

    GetClip(JUMP3)->SetEvent(bind(&Player::SetIdle, this), 0.0001f);   //????

    GetClip(TO_COVER)->SetEvent(bind(&Player::SetIdle, this), 0.05f);   //????

    GetClip(HIT)->SetEvent(bind(&Player::EndHit, this), 0.35f); // ????? ??????
    GetClip(CLIMBING1)->SetEvent(bind(&Player::EndClimbing, this), 0.1f);
    GetClip(CLIMBING2)->SetEvent(bind(&Player::EndClimbing, this), 0.32f);
    GetClip(CLIMBING3)->SetEvent(bind(&Player::EndClimbing, this), 0.95f);

    GetClip(ASSASSINATION1)->SetEvent(bind(&Player::EndAssassination, this, 1), 0.9f);
    GetClip(ASSASSINATION2)->SetEvent(bind(&Player::EndAssassination, this, 2), 0.9f);

    GetClip(DAGGER1)->SetEvent(bind(&Player::SetIdle, this), 0.6f);
    GetClip(DAGGER2)->SetEvent(bind(&Player::SetIdle, this), 0.6f);
    GetClip(DAGGER3)->SetEvent(bind(&Player::SetIdle, this), 0.6f);

    GetClip(DAGGER1)->SetEvent(bind(&Collider::SetActive, dagger->GetCollider(), true), 0.2f); //콜라이더 켜는 시점 설정
    GetClip(DAGGER2)->SetEvent(bind(&Collider::SetActive, dagger->GetCollider(), true), 0.3f); //콜라이더 켜는 시점 설정
    GetClip(DAGGER3)->SetEvent(bind(&Collider::SetActive, dagger->GetCollider(), true), 0.15f); //콜라이더 켜는 시점 설정

    GetClip(DAGGER1)->SetEvent(bind(&Collider::SetActive, dagger->GetCollider(), false), 0.3f); //콜라이더 꺼지는 시점 설정
    GetClip(DAGGER2)->SetEvent(bind(&Collider::SetActive, dagger->GetCollider(), false), 0.45f); //콜라이더 꺼지는 시점 설정
    GetClip(DAGGER3)->SetEvent(bind(&Collider::SetActive, dagger->GetCollider(), false), 0.35f); //콜라이더 꺼지는 시점 설정



    //GetClip(B_DRAW)->SetEvent(bind(&Player::SetBowAnim, this), 0.2f);
    //GetClip(B_ODRAW)->SetEvent(bind(&Player::SetBowAnim, this), 0.6f);
    GetClip(B_DRAW)->SetEvent(bind(&Player::SetBowAnim, this), 0.4f);
    GetClip(B_ODRAW)->SetEvent(bind(&Player::SetBowAnim, this), 0.4f);
    GetClip(B_RECOIL)->SetEvent(bind(&Player::ShootArrow, this), 0.1f);

    //GetClip(DAGGER1)->SetEvent(bind(&Player::CanCombo, this), 0.4f);

    //GetClip(HIT)->SetEvent(bind(&Player::EndHit, this), 0.35f);

    {
        portrait = new Quad(Vector2(200, 150));
        portrait->GetMaterial()->SetShader(L"Basic/Texture.hlsl");
        portrait->GetMaterial()->SetDiffuseMap(L"Textures/UI/portrait.png");
        portrait->Pos() = { 100, WIN_HEIGHT - 75, 0 };
        portrait->UpdateWorld();
    }

    {
        form = new Quad(Vector2(70, 70));
        form->GetMaterial()->SetShader(L"Basic/Texture.hlsl");
        form->GetMaterial()->SetDiffuseMap(L"Textures/UI/rpg/Bow_icon.png");
        form->Pos() = { 245, WIN_HEIGHT - 100, 0 };
        form->UpdateWorld();
    }

    aimT = new Transform();
    aimT->SetParent(this);
    aimT->Pos().x += -40.0f;
    aimT->Pos().y += -15.0f;

    crosshair = new Quad(L"Textures/UI/cursor.png");
    crosshair->Pos() = { CENTER_X, CENTER_Y, 0 };
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
    delete leftFoot;
    delete rightFoot;

    delete leftFootCollider;
    delete rightFootCollider;
    delete dagger;
    delete bow;

    delete ArrowManager::Get();

    delete aimT;
    delete crosshair;
}

void Player::Update()
{
    
    ColliderManager::Get()->SetHeight();
    ColliderManager::Get()->PushPlayer();

    SetCameraPos();

    collider->Pos().y = collider->Height() * 0.5f * 33.3f + collider->Radius() * 33.3f;
    collider->UpdateWorld();

    UpdateUI();

    if (isDying)return;

    Control();
    Searching();
    Targeting();
    UseSkill();

    //이 함수 내에서 조건으로 애니메이션 세팅을 중단하지 말고,
    //특정 애니메이션이 동작하면 canSettingAnim같은 bool 변수를 false로 만들어서
    //이 함수를 캔슬하는 방식 고려

    ModelAnimator::Update();

    if (comboHolding > 0.0f)
        comboHolding -= DELTA;  //???? ???, ????ð? ?? ?ð? ???? ??? ?????
    else
    {
        comboHolding = 0.0f;
        comboStack = 0;
    }

    rightHand->SetWorld(this->GetTransformByNode(rightHandNode));
    leftHand->SetWorld(this->GetTransformByNode(leftHandNode));
    if (weaponState == DAGGER)
        dagger->Update();
    else
        bow->UpdateWorld();

    //leftFoot->SetWorld(this->GetTransformByNode(leftFootNode));
    //leftFootCollider->UpdateWorld();

    //rightFoot->SetWorld(this->GetTransformByNode(rightFootNode));
    //rightFootCollider->UpdateWorld();

    SetAnimation();

    ArrowManager::Get()->Update();
    ArrowManager::Get()->IsCollision();

    
    crosshair->UpdateWorld();
}

void Player::Render()
{
    ModelAnimator::Render();
    //collider->Render();

    switch (weaponState)
    {
    case DAGGER:
        dagger->Render();
        break;
    case BOW:
        bow->Render();
        break;
    }

    //leftFootCollider->Render();
    //rightFootCollider->Render();

    ArrowManager::Get()->Render();

}

void Player::PostRender()
{
    hpBar->Render();
    portrait->Render();
    form->Render();
    string str = to_string(ArrowManager::Get()->GetPlayerArrowCount());
    
    Vector3 tmp = form->Pos() + Vector3(60, 10, 0);
    Font::Get()->RenderText("X ", { tmp.x, tmp.y }, Float2(12, 12));
    Font::Get()->RenderText(str, { tmp.x + 30, tmp.y }, Float2(12, 12));

    if(crosshair->Active())
        crosshair->Render();
}

void Player::GUIRender()
{
    return;
    Model::GUIRender();

    ImGui::Text("playerArrowCount : %d", ArrowManager::Get()->GetPlayerArrowCount());

    //ImGui::DragFloat3("Velocity", (float*)&velocity, 0.5f);
    ////???
    //ImGui::SliderFloat("moveSpeed", &moveSpeed1, 10, 1000);
    //ImGui::SliderFloat("moveSpeed", &moveSpeed2, 10, 1000);
    //ImGui::SliderFloat("rotSpeed", &rotSpeed, 1, 10);
    //ImGui::SliderFloat("deceleration", &deceleration, 1, 10);
    //ImGui::Text("isPushed : %d", isPushed);
    //ImGui::Text("feedBackPosY : %f", feedBackPos.y);
    //ImGui::Text("Pos.x : %f", Pos().x);
    //ImGui::Text("Pos.y : %f", Pos().y);
    //ImGui::Text("Pos.z : %f", Pos().z);
    //ImGui::Text("heightLevel : %f", heightLevel);

    ImGui::Text("curState : %d", curState);

    //???? ??
    //ImGui::SliderFloat("force1", &force1, 1, 500);
    //ImGui::SliderFloat("force2", &force2, 1, 500);
    //ImGui::SliderFloat("force3", &force3, 1, 500);

    //ImGui::SliderFloat("jumpSpeed", &jumpSpeed, 0.01f, 5.0f);
    ////???
    //ImGui::SliderFloat("gravityMult", &gravityMult, 1, 100);

    ////3?? ???? ?????? ???? ????
    //ImGui::InputFloat("JumpVel", (float*)&jumpVel);
    //ImGui::InputFloat("jumpN", (float*)&jumpVel);
    //ImGui::InputFloat("nextJump", (float*)&nextJump);

    //?????? ?ε??ð?
    /*ImGui::InputFloat("landingT", (float*)&landingT);
    ImGui::InputFloat("landing", (float*)&landing);

    ImGui::InputInt("rightHandNode", &rightHandNode);
    ImGui::InputInt("leftHandNode", &leftHandNode);
    ImGui::InputInt("leftFootNode", &leftFootNode);
    ImGui::InputInt("rightFootNode", &rightFootNode);

    ImGui::DragFloat3("bowPos", (float*)&bow->Pos(), 0.1f);
    ImGui::DragFloat3("bowRot", (float*)&bow->Rot(), 0.1f);
    ImGui::DragFloat3("bowScale", (float*)&bow->Scale(), 0.1f);*/

    //dagger->GUIRender();

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

void Player::Assassination()
{
    SetState(ASSASSINATION1, 2.0f);
}

void Player::Climb(Collider* col, Vector3 climbPos)
{
    Pos() = { climbPos.x,heightLevel,climbPos.z };
    UpdateWorld();

    remainClimbDis = col->GetHalfSize().z * 2; // 콜라이더의 로컬좌표계는 z가 높이인 좌표계임
    isClimb = true;

    SetState(CLIMBING1);
}

void Player::CameraMove()
{
    Ray cameraRay = CAM->ScreenPointToRay(mousePos);
}

void Player::Control()  //??????? ?????, ???콺 ??? ???
{
    if (KEY_DOWN(VK_TAB)) {
        if (static_cast<WeaponState>(weaponState + 1) >= 3)
            weaponState = DAGGER;
        else
            weaponState = static_cast<WeaponState>(weaponState + 1);
    }

    if (KEY_DOWN(VK_ESCAPE))
        camera = !camera;

    //if (camera)
    //    CAM->SetTarget(this);
    //else
    //    CAM->SetTarget(nullptr);


    if (KEY_PRESS(VK_RBUTTON))
        return;

    if(curState != CLIMBING1 && curState != CLIMBING2 && curState != CLIMBING3)
        Rotate();

    //switch (curState)
    //{
    //case KICK:
    //{
    //    //leftWeaponCollider->SetActive(true);
    //    //rightWeaponCollider->SetActive(true);
    //    break;
    //}
    //default:
    //{
    //    //leftWeaponCollider->SetActive(false);
    //    //rightWeaponCollider->SetActive(false);
    //}

    if (KEY_UP(VK_LBUTTON))
    {
        if (weaponState == BOW)
        {
            if (curState == B_DRAW || curState == B_ODRAW || curState == B_AIM)
            {
                ArrowManager::Get()->Throw(bow->GlobalPos(), CAM->ScreenPointToRayDir(mousePos));
                SetState(B_RECOIL);
            }
            return;
        }
    }

    if (KEY_DOWN(VK_LBUTTON))
    {
        if (weaponState == BOW)
        {
            // 보유한 화살이 있는가
            if (ArrowManager::Get()->GetPlayerArrowCount() <= 0)return;
                SetState(B_DRAW);
            return;
        }
    }

    if (KEY_PRESS(VK_LBUTTON))
    {
        if (weaponState == DAGGER)
        {
            if (curState != DAGGER1 && curState != DAGGER2 && curState != DAGGER3)
            {
                ComboAttack();
                //dagger->GetCollider()->SetActive(true); //콜라이더 켜지는 시점은 이벤트로 설정
            }
        }
        //else if (weaponState == BOW)
        //{
        //    if (curState == B_IDLE)
        //        SetState(B_DRAW);
        //}
    }
    if (KEY_UP(VK_LBUTTON))
    {
        //dagger->GetCollider()->SetActive(false); //콜라이더 꺼지는 시점은 이벤트로 설정
    }

    if (isHit)   //?´°? ?????????? Jumping????? ?????? ????? ????? ????? ?ð??? ?þ?
    {
        return;
    }

    if (KEY_DOWN(VK_SPACE) && !InTheAir())
    {
        SetState(JUMP1);
    }

    // 스페셜 키
    if (KEY_DOWN('C'))
    {
        // 화살 줍기
        ArrowManager::Get()->ExecuteSpecialKey();
    }

    if (curState != CLIMBING1 && curState != CLIMBING2 && curState != CLIMBING3)
    {
        Move();
        Jumping();
    }

    if (curState == CLIMBING2)
    {
        Pos().y += DELTA*3.0f;
        UpdateWorld();
    }

    //if (targetObject != nullptr && KEY_DOWN('F'))     ////보류
    //{
    //    velocity = 0;
    //    SetState(TO_ASSASIN);
    //}
}

void Player::Move() //??? ????(?? ???, ??? ???, ???? ?? ???????, ??? ???? ???? ???????? ?? ??? ??)
{
    //플레이어의 위에서 레이를 쏴서 현재 terrain 위치와 높이를 구함

    if (!OnColliderFloor(feedBackPos)) // 문턱올라가기 때문에 다시 살림
    {
        Vector3 PlayerSkyPos = Pos();
        PlayerSkyPos.y += 1000;
        Ray groundRay = Ray(PlayerSkyPos, Vector3(Down()));
        TerainComputePicking(feedBackPos, groundRay);
    }

    //if (curState == JUMP3 && landing > 0.0f)    //???? ??????? ???? ?ε? ??? ???? and ???? ?ð? ????
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
    if (isHit || isDying)
    {
        if (isHit)
            curHP -= DELTA * 10 * 4; // 2 -> 4
        else
            curHP -= DELTA * 10 * 6;
        State aa = curState;

        if (curHP <= destHP)
        {
            curHP = destHP;
            isHit = false;
        }

        hpBar->SetAmount(curHP / maxHp);
    }

    /*
    Vector3 barPos = Pos() + Vector3(0, 6, 0);

    hpBar->UpdateWorld();

    if (!CAM->ContainPoint(barPos))
    {
        hpBar->SetActive(false);
        return;
    }

    if (!hpBar->Active()) hpBar->SetActive(true);
    hpBar->Pos() = CAM->WorldToScreen(barPos);

    */
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
    bool isMoveX = false; // ?¿? ??? ?? : ???? "???"

    if (KEY_PRESS('W'))
    {
        if (velocity.z + DELTA * 4.0f < 0.0f)
            velocity.z += DELTA * 4.0f;
        else
            velocity.z += DELTA; //??? ????? ?ð? ?????? ?????? ???

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
            velocity.x += DELTA; //??? ????? ?ð? ?????? ?????? ???

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
    Vector3 destPos;
    if (!KEY_PRESS(VK_LSHIFT))
    {
        /*if (curState == B_AIM || curState == B_DRAW || curState == B_ODRAW)
            destPos = Pos() + direction * aimMoveSpeed * DELTA * -1;
        else*/
        destPos = Pos() + direction * moveSpeed1 * DELTA * -1;
    }
        
    else
        destPos = Pos() + direction * moveSpeed2 * DELTA * -1;
    Vector3 PlayerSkyPos = destPos;
    PlayerSkyPos.y += 1000;
    Ray groundRay = Ray(PlayerSkyPos, Vector3(Down()));
    if (!OnColliderFloor(destFeedBackPos)) // 문턱올라가기 때문에 다시 살림
    {
        TerainComputePicking(destFeedBackPos, groundRay);
    }

    //destFeedBackPos : 목적지 터레인Pos
    //feedBackPos : 현재 터레인Pos

    //???????? ???? ?????
    Vector3 destDir = destFeedBackPos - feedBackPos;
    Vector3 destDirXZ = destDir;
    destDirXZ.y = 0;
 
    //????
    float radianHeightAngle = acos(abs(destDirXZ.Length()) / abs(destDir.Length()));


    if (/*ColliderManager::Get()->ControlPlayer(&direction)*/ !isPushed
        && (radianHeightAngle < XMConvertToRadians(60) || destFeedBackPos.y <= feedBackPos.y
            || destFeedBackPos.y - feedBackPos.y < 0.5f) // 바닥 올라가게 하기위해 추가함
        ) //???? 60?????? ???? ???, ??? ?????? ????? ?? ???????
    {
        if (!KEY_PRESS(VK_LSHIFT))
            Pos() += direction * moveSpeed1 * DELTA * -1; // ??? ????
        else
            Pos() += direction * moveSpeed2 * DELTA * -1; // ??? ????
        feedBackPos.y = destFeedBackPos.y;
    }

    //???????°? ????? ???? ???? ????? ???? ????
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

        if (heightLevel < feedBackPos.y)
            heightLevel = feedBackPos.y;

        if (isCeiling)
        {
            jumpVel = -20;
            isCeiling = false;
        }

        float tempJumpVel;
        float tempY;

        if (curState != JUMP1 && curState != JUMP2 && preHeight - heightLevel > 5.0f)
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

void Player::Searching()
{
    //straightRay.dir = Back;
    //straightRay.pos = Pos();

    //diagnolLRay.dir = Back;

    //block manager?? ???????? ??? ??? ????
}

void Player::Targeting()
{
    InteractManager::Get()->ClearSkill();

    Vector3 offset;
    Ray mouseRay = CAM->ScreenPointToRay(mousePos);

    {
        MonsterManager::Get()->OnOutLineByRay(mouseRay);
        offset = (CAM->Right() * 2.f) + (CAM->Up() * 6.f);
        MonsterManager::Get()->ActiveSpecialKey(Pos(), offset);
    }
    
    {
        ArrowManager::Get()->OnOutLineByRay(mouseRay);
        offset = (CAM->Right() * 1.5f) + (CAM->Up() * 3.f);
        ArrowManager::Get()->ActiveSpecialKey(Pos(), offset);
    }

    {
        ColliderManager::Get()->PickFlagByRay(mouseRay);
        offset = (CAM->Right() * 2.f) + (CAM->Up() * 2.f);
        ColliderManager::Get()->ActiveSpecialKey(GlobalPos(), offset);
        if (curState == CLIMBING1 || curState == CLIMBING2 || curState == CLIMBING3)
            ColliderManager::Get()->ClearSpecialKey();
    }
    
}

void Player::UseSkill()
{
    for (InteractManager::Skill s : InteractManager::Get()->GetActiveSkills())
    {
        if (KEY_DOWN(s.key)) // 등록되어있는 키를 누르면
        {
            s.event(); //등록되어있는 함수 실행
        }
    }
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

bool Player::InTheAir() {
    return ((curState == JUMP1 || curState == JUMP2 || curState == JUMP3) && Pos().y > feedBackPos.y);
}

void Player::EndAssassination(UINT num)
{
    if (num == 1)
        SetState(ASSASSINATION2);
    else
    {
        if (weaponState == DAGGER)
            SetState(IDLE);
        else if (weaponState == BOW)
            SetState(B_IDLE);
        else
            SetState(IDLE);
    }

}

void Player::EndHit()
{
    isHit = false;
    collider->SetActive(true);
    SetState(IDLE);
}

void Player::EndClimbing()
{
    if (curState == CLIMBING1)
    {
        SetState(CLIMBING2, 1.5f); 
        return;
    }
     
    if (curState == CLIMBING2)
    {
        //벽높이 만큼 애니메이션 재생하기

        remainClimbDis -= 10;
        if (remainClimbDis <= 0.0f)
        {
            SetState(CLIMBING3);
            remainClimbDis = 0.0f;
            return;
        }

        SetState(CLIMBING2);
        return;
    }

    Pos() += Back() * 1.5f; 
    Pos() += Up() * 7.f;
    
    SetState(IDLE);
}

bool Player::OnColliderFloor(Vector3& feedback)
{
    Vector3 PlayerSkyPos = GlobalPos();
    PlayerSkyPos.y += 3;
    Ray groundRay = Ray(PlayerSkyPos, Vector3(Down()));
    Contact con;
    if (ColliderManager::Get()->CloseRayCollisionColliderContact(groundRay, con))
    {
        feedback = con.hitPoint;
        //feedback.y += 0.1f; //살짝 띄움으로서 충돌 방지
        return true;
    }

    return false;
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
    else if (curState == DAGGER1 || curState == DAGGER2 || curState == DAGGER3)
        r = dagger->GetDamaged();
    // 모션마다 동작마다 여기서 데미지 추가할 것. 
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
            isDying = true;
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
    }

    comboHolding = 1.5f;
    comboStack++;
    if (comboStack == 3)
        comboStack = 0;
}

void Player::ShootArrow()
{
    //ArrowManager::Get()->Throw(bow->GlobalPos(), CAM->ScreenPointToRayDir(mousePos));
    SetState(B_IDLE);
    
}

void Player::SetState(State state, float scale, float takeTime)
{
    if (state == curState) return;

    curState = state;
    PlayClip((int)state, scale, takeTime);
}

void Player::SetAnimation()     //bind로 매개변수 넣어줄수 있으면 매개변수로 값을 받아올 경우엔 바로 그 state로 변경하게 만들기
{
    if (curState == ASSASSINATION1 || curState == ASSASSINATION2) return;
    if (curState == CLIMBING1 || curState == CLIMBING2 || curState == CLIMBING3) return;
        
    if (weaponState == DAGGER)
    {
        if (curState == JUMP1 || curState == JUMP3 || Pos().y > heightLevel) return;

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
        if (curState == JUMP1 || curState == JUMP3 || Pos().y > heightLevel) return;


        if (curState == B_DRAW || curState == B_ODRAW || curState == B_AIM || curState == B_RECOIL)
        {
            if (KEY_DOWN(VK_RBUTTON))
            {
                SetState(B_IDLE);
            }
            return;
        }

        if (KEY_PRESS(VK_LSHIFT)) {
            if (velocity.z > 0.1f)
                SetState(B_C_F);
            else if (velocity.z < -0.1f)
                SetState(B_C_B);
            else if (velocity.x > 0.1f)
                SetState(B_C_R);
            else if (velocity.x < -0.1f)
                SetState(B_C_L);
            else
                SetState(B_C_IDLE);
        }
        else
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
}

void Player::SetBowAnim()
{
    if (curState == B_DRAW)
    {
        SetState(B_ODRAW);
    }
    else if (curState == B_ODRAW)
    {
        SetState(B_AIM);
    }
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

void Player::SetCameraPos()
{
    if (curState == B_AIM || curState == B_DRAW || curState == B_ODRAW)
    {
        CAM->SetTarget(aimT);
        CAM->SetDistance(1.0f);

        aimT->Rot() = Rot();
        aimT->UpdateWorld();

        crosshair->SetActive(true);
        return;
    }

    crosshair->SetActive(false);

    CAM->SetTarget(this);

    Ray playerBackRay = Ray(Pos(), Forward());
    Contact temp;

    float distance = 13.f;

    for (Collider* obstacle : ColliderManager::Get()->GetObstacles())
    {
        if (obstacle->Role() == Collider::BLOCK)
        {
            if (obstacle->IsRayCollision(playerBackRay, &temp) && temp.distance < distance)
            {
                distance = temp.distance;
            }
        }
    }

    CAM->SetDistance(distance);
}