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
    bow->SetActive(true);
    
    {
        bow->SetParent(leftHand);
        bow->Scale() *= 6.f;
        bow->Pos() = { 0, 6.300, -2.400 };
        bow->Rot() = { 3.000, 0, 0 };

    }
    aimT = new Transform();
    aimT->SetParent(this);
    aimT->Pos() = aimStartPos;
    temp = new SphereCollider(5);
    temp->SetParent(aimT);

    crosshair = new Quad(L"Textures/UI/cursor.png");
    crosshair->Pos() = { CENTER_X, CENTER_Y, 0 };


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

    string temp;  

    ReadClip("Idle");

    ReadClip("Medium Run");
    ReadClip("Running Backward");
    ReadClip("Left Strafe");
    ReadClip("Right Strafe");

    ReadClip("Jog Forward Diagonal Left");
    ReadClip("Jog Forward Diagonal Right");

    ReadClip("Jumping Up");
    ReadClip("IntheSky");
    ReadClip("Falling To Landing"); 
    ReadClip("Hard Landing"); 

    ReadClip("Stand To Cover"); 
    ReadClip("Cover Idle");
    ReadClip("Crouched Sneaking Right");
    ReadClip("Crouched Sneaking Left");
    ReadClip("Crouch Turn To Stand");

    ReadClip("Head Hit");

    ReadClip("Climbing1");
    ReadClip("Climbing2");
    ReadClip("Climbing3");
    ReadClip("Braced Hang Hop Left");
    ReadClip("Braced Hang Hop Right");
    ReadClip("Climbing Down Wall");
    ReadClip("Braced Hang Drop");
    //ReadClip("temp");

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
    GetClip(JUMP3)->SetEvent(bind(&Player::SetIdle, this), 0.1f);   //????
    GetClip(JUMP4)->SetEvent(bind(&Player::JumpSetting, this), 0.78f);   //????

    GetClip(TO_COVER)->SetEvent(bind(&Player::SetIdle, this), 0.05f);   //????

    GetClip(HIT)->SetEvent(bind(&Player::EndHit, this), 0.34f); // ????? ??????
    //GetClip(CLIMBING1)->SetEvent(bind(&Player::EndClimbing, this), 0.1f);
    //GetClip(CLIMBING2)->SetEvent(bind(&Player::EndClimbing, this), 0.32f);
    //GetClip(CLIMBING3)->SetEvent(bind(&Player::EndClimbing, this), 0.95f);

    GetClip(CLIMBING1)->SetEvent(bind(&Player::SetClimbAnim, this), 0.1f);
    GetClip(CLIMBING2)->SetEvent(bind(&Player::SetClimbAnim, this), 0.85f);
    GetClip(CLIMBING3)->SetEvent(bind(&Player::SetClimbAnim, this), 0.9f);
    GetClip(CLIMBING_JUMP_R)->SetEvent(bind(&Player::SetClimbAnim, this), 0.8f);
    GetClip(CLIMBING_JUMP_L)->SetEvent(bind(&Player::SetClimbAnim, this), 0.8f);
    GetClip(CLIMBING_DOWN)->SetEvent(bind(&Player::SetClimbAnim, this), 0.9f);
    //GetClip(CLIMBING_JUMP_D)->SetEvent(bind(&Player::SetClimbAnim, this), 0.5f);

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

    tempCam = new Transform();

    hiteffect = new Sprite(L"Textures/Effect/HitEffect.png", 15, 15, 5, 2, false);
    jumpparticle=new ParticleSystem("TextData/Particles/JumpSmoke.fx");

    FOR(2) blendState[i] = new BlendState();
    blendState[1]->Additive(); //투명색 적용 + 배경색 처리가 있으면 역시 적용

    stateInfo = new StateInfo();
}

Player::~Player()
{
    FOR(2)
        delete blendState[i];
    delete stateInfo;
    delete collider;
    delete portrait;
    delete form;
   
    delete hpBar;
    delete hiteffect;
    delete jumpparticle;
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

    ArrowManager::Delete();

    delete aimT;
    delete crosshair;
    delete tempCam;

    delete temp;
}

void Player::Update()
{
    ColliderManager::Get()->SetHeight();
    ColliderManager::Get()->PushPlayer();

    SetCameraPos();

    if (collider->GetParent() == this)
    {
        collider->Pos().y = collider->Height() * 0.5f * 33.3f + collider->Radius() * 33.3f;
        collider->UpdateWorld();
    }

    UpdateUI();

    if (isDying) return;

    if(curState != JUMP4)
        Control();
    if (!isClimb)
        Jumping();
    Rotate();
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

    hiteffect->Update();
    jumpparticle->Update();
    crosshair->UpdateWorld();

    CoolDown();

    temp->Pos() = { 0, 0, 0 };
    temp->UpdateWorld();
}

void Player::PreRender()
{

}

void Player::Render()
{
    if(stateInfo->isCloaking)
        blendState[1]->SetState();
    ModelAnimator::Render();
    switch (weaponState)
    {
    case DAGGER:
        dagger->Render();
        break;
    case BOW:
        bow->Render();
        break;
    }
    blendState[0]->SetState();

    collider->Render();
    hiteffect->Render();
    jumpparticle->Render();
    
    //leftFootCollider->Render();
    //rightFootCollider->Render();

    ArrowManager::Get()->Render();

    temp->Render();
}

void Player::PostRender()
{
    hpBar->Render();
    
    portrait->Render();
    if (bow->Active())
           form->Render();
    string str = to_string(ArrowManager::Get()->GetPlayerArrowCount());

    Vector3 tmp = form->Pos() + Vector3(60, 10, 0);
    Font::Get()->RenderText("X ", { tmp.x, tmp.y }, Float2(12, 12));
    Font::Get()->RenderText(str, { tmp.x + 30, tmp.y }, Float2(12, 12));

    if (crosshair->Active())
        crosshair->Render();
}

void Player::GUIRender()
{
    ImGui::Text("playerArrowCount : %d", ArrowManager::Get()->GetPlayerArrowCount());

    CAM->GUIRender();

    ImGui::Text("curState : %d", curState);

    ColliderManager::Get()->GuiRender();
}

void Player::TextRender()
{
    Font::Get()->RenderText(L"남은 투명화 시간 : " + residualCloakingTime, { 700,600 }, { 600,100 });
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
    CAM->temp = false;

    Pos() = { climbPos.x, Pos().y, climbPos.z };
    UpdateWorld();

    //remainClimbDis = col->GetHalfSize().z * 2; // 콜라이더의 로컬좌표계는 z가 높이인 좌표계임
    isClimb = true;

    canClimbControl = true;
    SetState(CLIMBING1);

    tempCam->Pos() = Pos();
    tempCam->Rot() = Rot();
}

void Player::SetClimbAnim()
{
    if (curState == CLIMBING_JUMP_D)
    {
        Pos().y = climbJ_y;
        Pos() += Back() * 3.0f;
        velocity = Back();
        this->UpdateWorld();
        SetState(JUMP2);

        isClimb = false;
    }
    else if (curState == CLIMBING_DOWN || curState == CLIMBING2 || curState == CLIMBING_JUMP_R || curState == CLIMBING_JUMP_L)
    {
        SetState(CLIMBING1);
        canClimbControl = true;
    }
    else if (curState == CLIMBING3)
    {
        Pos() = climbArrivePos;
        this->UpdateWorld();
        collider->UpdateWorld();

        SetState(IDLE);
        isClimb = false;
    }
}

void Player::Climbing()
{
    if (curState == CLIMBING3 || curState == CLIMBING_JUMP_D)
        return;

    //현재는 건물이 직각이기 때문에 가능한 코드, 굴곡이 있다면 Ray로 바꾸고 distance 체크하면서 이동하기로 구현
    Transform rayT = *this;
    rayT.Pos().y += 7.2f;

    Ray headForwardRay = Ray(rayT.Pos(), rayT.Back());

    bool arriveTop = false;
    Contact con;

    if (curState == CLIMBING2)
    {
        arriveTop = true;
        for (Collider* obstacle : ColliderManager::Get()->GetObstacles())
        {
            if (obstacle->IsRayCollision(headForwardRay, &con)/* && con.distance < 10.0f*/)
            {
                if (con.distance < 1.f)
                {
                    arriveTop = false;
                    climbArrivePos = con.hitPoint;
                    break;
                }
            }
        }
    }

    if (arriveTop)    ////꼭대기 올라가는 코드, 테스트 위해서 주석처리 함
    {
        //climbArrivePos.y += 0.1f;
        climbArrivePos += Back() * 2.0f;
        SetState(CLIMBING3);
        return;
    }

    if (canClimbControl)
    {
        if (KEY_DOWN(VK_SPACE))
        {
            canClimbControl = false;
            SetState(CLIMBING_JUMP_D);
            climbJ_y = Pos().y; 

            velocity = Back();

            isClimb = false;

            CAM->temp = false;

            return;
        }

        climbVel = 0.0f;

        if (KEY_PRESS('W'))
        {
            climbVel.y = 1.0f;
            SetState(CLIMBING2);
            canClimbControl = false;
        }
        else if (KEY_PRESS('S'))
        {   //1.3f정도 위에서 애니메이션이 재생되는듯
            climbVel = Down();
            SetState(CLIMBING_DOWN);
            canClimbControl = false;
        }

        if (KEY_PRESS('A'))
        {
            climbVel = Right();
            climbVel.y = 0.0f;
            SetState(CLIMBING_JUMP_L);
            canClimbControl = false;
        }
        else if (KEY_PRESS('D'))
        {
            climbVel = Left();
            climbVel.y = 0.0f;
            SetState(CLIMBING_JUMP_R);
            canClimbControl = false;
        }
    }

    if (curState == CLIMBING_DOWN && tempCam->Pos().y <= heightLevel)
    {
        climbVel = 0;

        Pos() = tempCam->Pos();
        Pos().y = heightLevel + 0.01f;   //heightLevel이 갱신되는지 확인해야 함, 현재는 건물 밑에 바닥밖에 없어서 상관x

        if (Pos().y < 0.0f)
            Pos().y = 0.0f;

        collider->SetParent(this);
        CAM->SetTarget(this);
        CAM->temp = false;

        isClimb = false;

        return;
    }

    Pos() += climbVel * DELTA * 4.0f;
    tempCam->Pos() = Pos();
    tempCam->Pos().y += 8.0f;

    tempCam->UpdateWorld();
}

void Player::CameraMove()
{
    Ray cameraRay = CAM->ScreenPointToRay(mousePos);
}

void Player::Control()  //??????? ?????, ???콺 ??? ???
{
    if (KEY_DOWN(VK_ESCAPE))
        camera = !camera;

    if (curState == ASSASSINATION1 || curState == ASSASSINATION2) return;

    if (curState != CLIMBING2 && curState != CLIMBING3
        && curState != CLIMBING_JUMP_L && curState != CLIMBING_JUMP_R && curState != CLIMBING_DOWN)
    {

        if (KEY_DOWN(VK_TAB)) {
            if (static_cast<WeaponState>(weaponState + 1) >= 3)
                weaponState = DAGGER;
            else
            {
                if (!bow->Active())
                    return;
                weaponState = static_cast<WeaponState>(weaponState + 1);

            }
        }

        if (KEY_DOWN(VK_LBUTTON))
        {
            if (weaponState == BOW)
            {
                // 보유한 화살이 있는가
                if (ArrowManager::Get()->GetPlayerArrowCount() <= 0) return;
                SetState(B_DRAW);
                aimT->Pos() = aimStartPos;
                return;
            }
        }
        else if (KEY_PRESS(VK_LBUTTON))
        {
            if (weaponState == DAGGER)
            {
                if (curState != DAGGER1 && curState != DAGGER2 && curState != DAGGER3)
                {
                    ComboAttack();
                    //dagger->GetCollider()->SetActive(true); //콜라이더 켜지는 시점은 이벤트로 설정
                }
            }
            else if (weaponState == BOW)
            {
                if (curState == B_DRAW || curState == B_ODRAW || curState == B_AIM)
                    if (chargingT < maxSpeed) 
                    {
                        Vector3 dir = { 0.1, 0, -1 };
                        aimT->Pos() += DELTA * dir * 35.0f;
                        chargingT += DELTA * chargetVal;
                    }
                    else
                        chargingT = maxSpeed;
            }
        }
        else if (KEY_UP(VK_LBUTTON))
        {
            if (!bow->Active())return;

            if (weaponState == BOW)
            {
                if (curState == B_DRAW || curState == B_ODRAW || curState == B_AIM)
                {
                    ArrowManager::Get()->Throw(bow->GlobalPos(), CAM->ScreenPointToRayDir(mousePos), chargingT);
                    chargingT = initSpeed;
                    SetState(B_RECOIL);
                }
                return;
            }
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
            bow->SetActive(true);
        }

        if (KEY_DOWN('R'))
        {
            if (!stateInfo->isCloaking)
                stateInfo->isCloaking = true;
            else 
                stateInfo->isCloaking = false;
        }
    }

    Cloaking();
    Move();

    //if (targetObject != nullptr && KEY_DOWN('F'))     ////보류
    //{
    //    velocity = 0;
    //    SetState(TO_ASSASIN);
    //}
}

void Player::Move() //??? ????(?? ???, ??? ???, ???? ?? ???????, ??? ???? ???? ???????? ?? ??? ??)
{
    if (isClimb)
    {
        Climbing();

        //if (curRayCoolTime <= 0.f)
        //{
        //    Vector3 PlayerSkyPos = Pos();
        //    PlayerSkyPos.y += 100;
        //    Ray groundRay = Ray(PlayerSkyPos, Vector3(0.f,-1.f,0.f));
        //    TerainComputePicking(feedBackPos, groundRay);
        //}   
    }
    else
    {
        //플레이어의 위에서 레이를 쏴서 현재 terrain 위치와 높이를 구함

        if (!OnColliderFloor(feedBackPos)) // 문턱올라가기 때문에 다시 살림
        {
            Vector3 PlayerSkyPos = Pos();
            PlayerSkyPos.y += 1000;
            Ray groundRay = Ray(PlayerSkyPos, Vector3(0.f, -1.f, 0.f));
            TerainComputePicking(feedBackPos, groundRay);
        }

        Walking();
    }
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

    wstring s = L"%.2f";
    residualCloakingTime = format(s, stateInfo->possibleCloakingTime - stateInfo->curCloakingTime);
}

void Player::Cloaking()
{
    if (stateInfo->isCloaking)
    {
        if (stateInfo->possibleCloakingTime <= stateInfo->curCloakingTime)
        {
            stateInfo->isCloaking = false;
            return;
        }
        stateInfo->curCloakingTime += DELTA;
    }
    else
    {
        if (stateInfo->curCloakingTime <= 0)
        {
            stateInfo->curCloakingTime = 0.f;
            return;
        }
        stateInfo->curCloakingTime -= DELTA;
    }
}

void Player::Rotate()
{
    Vector3 delta = mousePos - Vector3(CENTER_X, CENTER_Y);

    if (!camera)
        return;

    SetCursorPos(clientCenterPos.x, clientCenterPos.y);

    if (!isClimb)
        Rot().y += delta.x * rotSpeed * DELTA;
    else
        tempCam->Rot().y += delta.x * rotSpeed * DELTA;
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
    Ray groundRay = Ray(PlayerSkyPos, Vector3(0.f,-1.f,0.f));
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

    if (!isPushed && curState != JUMP1
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
    if (curState != JUMP1 && curState != JUMP2 && curState != JUMP3 && curState != CLIMBING_JUMP_D)
        Pos().y = feedBackPos.y;    //여기 체크
}

void Player::Jump()
{
    jumpVel = force1;
}

void Player::AfterJumpAnimation()
{
    SetState(JUMP2);
}

void Player::JumpSetting()
{
    if (curState == JUMP4)
    {
        SetState(IDLE);
    }
}

void Player::Jumping()
{
    if (heightLevel < feedBackPos.y)
        heightLevel = feedBackPos.y;

    if (headCrash)
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

        if (climbJ_y >= 0)
        {
            Pos().y = climbJ_y;

            climbJ_y = -1.0f;
        }
        tempY = Pos().y + jumpVel * DELTA * jumpSpeed;
    }

    if (tempY <= heightLevel)
    {
        tempY = heightLevel;
        tempJumpVel = 0.0f;

        if (curState == JUMP2) {
            if (fallingT >= 0.45f)
            {
                SetState(JUMP4);
                curHP -= 10.0f;
            }
            else
                SetState(JUMP3);

            jumpparticle->Play(Pos());
        }
    }

    Pos().y = tempY;
    jumpVel = tempJumpVel;

    if (jumpVel < 0.0f)
    {
        SetState(JUMP2);
        fallingT += DELTA;
    }
    else
        fallingT = 0.0f;

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
    if(boss){
        boss->OnOutLineByRay(mouseRay);
        offset = (CAM->Right() * 2.f) + (CAM->Up() * 6.f);
        boss->ActiveSpecialKey(Pos(), offset);
    }




    {
        ArrowManager::Get()->OnOutLineByRay(mouseRay);
        offset = (CAM->Right() * 1.5f) + (CAM->Up() * 3.f);
        bool bTRUE = false;
        if (BowInstallation)
        {
            if (!BowInstallation->Active())return;
            if (Distance(BowInstallation->GlobalPos(), GlobalPos()) < 6.f) {
                bTRUE = true;
            }
        }

        ArrowManager::Get()->ActiveSpecialKey(Pos(), offset, bTRUE);
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

    if (isClimb)
        isClimb = false;

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
        UINT w = terrain->GetSizeWidth();

        float minx = floor(ray.pos.x);
        float maxx = ceil(ray.pos.x);

        if (maxx == w - 1 && minx == maxx)
            minx--;
        else if (minx == maxx)
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

        hiteffect->Play(particlepos);

        if (destHP <= 0)
        {
            //SetState(DYING);
            //???????
            isDying = true;
            return;
        }
        preState = curState;
        SetState(HIT, 0.8f);

        isHit = true;
    }

}

void Player::Hit(float damage, bool camerashake)
{
    if (!isHit)
    {
        destHP = (curHP - damage > 0) ? curHP - damage : 0;

        CAM->DoShake();
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
    if (curState == CLIMBING1 || curState == CLIMBING2 || curState == CLIMBING3 || curState == CLIMBING_JUMP_R || curState == CLIMBING_JUMP_L || curState == CLIMBING_DOWN) return;

    if (weaponState == DAGGER)
    {
        if (curState == JUMP1 || curState == JUMP3 || curState == JUMP4 || Pos().y > heightLevel) return;

        if (curState == HIT || curState == KICK || curState == DAGGER1 || curState == DAGGER2 || curState == DAGGER3)
            return;


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
        else if (velocity.z > 0.01f && velocity.x < -0.1f)
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
    SetState(IDLE); 
}

void Player::SetCameraPos()
{
    if (isClimb)
    {
        CAM->SetTarget(tempCam);
    }
    else if (curState == CLIMBING3)
    {
        CAM->SetTarget(nullptr);
        CAM->Pos().y += 2.0f * DELTA;
    }
    else if (curState == B_AIM || curState == B_DRAW || curState == B_ODRAW)
    {
        CAM->SetTarget(aimT);
        CAM->SetDistance(1.0f);

        aimT->Rot() = Rot();
        aimT->UpdateWorld();

        crosshair->SetActive(true);
    }
    else
    {
        crosshair->SetActive(false);

        CAM->SetTarget(this);

        Ray playerBackRay = Ray(Pos(), Forward());
        playerBackRay.pos.y += 3.5f;
        Contact temp;

        float distance = 13.f;

        for (Collider* obstacle : ColliderManager::Get()->GetObstacles())
        {
            if (obstacle->IsRayCollision(playerBackRay, &temp) && temp.distance < distance)
            {
                distance = temp.distance;
            }
        }
        CAM->SetDistance(distance);
    }
}

void Player::CoolDown()
{
    if (curRayCoolTime <= 0.0f)
    {
        curRayCoolTime = rayCoolTime;
    }
    else
        curRayCoolTime -= DELTA;
}