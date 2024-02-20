#include "Framework.h"

Naruto::Naruto()
    : ModelAnimator("Naruto")
{
    // ������ �ڵ鷯�� ������(�� ������ ũ��)�� �� ��° �Ű������� ����
    ClientToScreen(hWnd, &clientCenterPos);
    SetCursorPos(clientCenterPos.x, clientCenterPos.y);

    collider = new CapsuleCollider(50, 200); // <- ���� ���� 160
    collider->SetParent(this);
    collider->SetActive(true); //�浹ü ���̱� ���� ���� �� �κ� false

    mainHand = new Transform();
    tmpCollider = new SphereCollider();
    tmpCollider->Scale() *= 0.1; // �ӽ÷� 10% ũ��
    tmpCollider->SetParent(mainHand); // �ӽ÷� ���� �浹ü�� "��" Ʈ�������� �ֱ�

    crosshair = new Quad(L"Textures/UI/cursor.png");
    crosshair->Pos() = { CENTER_X, CENTER_Y, 0 };
    crosshair->UpdateWorld();
    Audio::Get()->Add("PlayerWalk", "Sounds/footstep.wav",false,false,true);

    ReadClip("Idle");
    ReadClip("Run");
    ReadClip("RunBack");  
    ReadClip("RunLeft");
    ReadClip("RunRight");
    ReadClip("Throw");
    ReadClip("Punching");

    //�� Ŭ�� �ȿ� �ִ� �̺�Ʈ ������ ȣ��

    //GetClip(THROW)->SetEvent(bind(&Naruto::Throw, this), 0.0f); //<-�� �ð��� ������ �����ߴٰ�
    //<-�� ���� �ð��� ������ ���� ���� ������ ���� ����
    //GetClip(THROW)->SetEvent(bind(&Naruto::Throw, this), 1.0f / 33.0f * ��ų���);
    GetClip(THROW)->SetEvent(bind(&Naruto::Throw, this), 0.7f);
    GetClip(THROW)->SetEvent(bind(&Naruto::EndThrow, this), 0.9f);
}

Naruto::~Naruto()
{
    delete mainHand;
    delete kunai;
}

void Naruto::Update()
{
    Control();
    SetAnimation();

    mainHand->SetWorld(GetTransformByNode(38));
    //SetWorld : Ʈ�������� ������ �ٸ� ������ ���Խ�Ű��
    //GetTransformByNode : ���� ��忡�� Ʈ������ ���� ���
    // 38 : ������ ���� ���� ��� Ʈ���� ��� ���� �������� �޽� ��Ͽ��� �����ϴ� ����
    //      -> �ٸ� ���̳� �ٸ� ��带 ������ ���� �ش� ����� �ε����� �˸� ����!

    ModelAnimator::Update(); //�� ������Ʈ

   // UpdateRay();
    tmpCollider->UpdateWorld(); //�������� ���󰡴� ���� �浹ü ������Ʈ
                                //�𵨺��� ������Ʈ�� �صθ� ���� �ޱ⿡ ����

    ///-----------------------------

    //TODO : ���̰� ���� �߿��� ������ ���ư���
    //       (���� ���� �ƴ� ���� �������)
    collider->UpdateWorld(); //�浹ü ������Ʈ
}

void Naruto::Render()
{
    ModelAnimator::Render();
    tmpCollider->Render();
    collider->Render();
}

void Naruto::PostRender()
{
    crosshair->Render();
}

void Naruto::GUIRender()
{
    Model::GUIRender(); // �𵨷μ� GUI ����
                        // (������ �ȿ��� �ִϸ����Ͱ�, �ִϸ����� �ȿ��� ���� �����ϱ�)
    ImGui::DragFloat("ray", &value,1.f,0.f,200.f);



}

void Naruto::Blocking(Collider* collider)
{

    if (collider->IsCollision(this->GetCollider()))
    {
        Vector3 dir = this->GetCollider()->GlobalPos() - collider->GlobalPos();

        int maxIndex = 0;
        float maxValue = -99999.0f;

        for (int i = 0; i < 3; ++i)
        {
            Vector3 halfSize = ((BoxCollider*)collider)->GetHalfSize();

            if (i != 1)
            {
                if (abs(dir[i]) - abs(halfSize[i]) > maxValue)
                {
                    maxIndex = i;
                    maxValue = abs(dir[i]) - abs(halfSize[i]);
                }
            }
        }

        switch (maxIndex)
        {
        case 0: // x
            dir.x = dir.x > 0 ? 1.0f : -1.0f;
            dir.y = 0;
            dir.z = 0;
            break;

        case 1: // y
            dir.x = 0;
            dir.y = dir.y > 0 ? 1.0f : -1.0f;;
            dir.z = 0;
            break;

        case 2: // z
            dir.x = 0;
            dir.y = 0;
            dir.z = dir.z > 0 ? 1.0f : -1.0f;;
            break;
        }

        dir.y = 0;

        if (NearlyEqual(dir.y, 1.0f)) return; 

        this->Pos() += dir * 50.0f * DELTA;

    }
}

void Naruto::Control()
{
    Rotate();
    MouseDirection();
    Move();
    Attack();
    
}

void Naruto::Move()
{
    if (curState == THROW) return; //������ ���� �߿��� �������� �ʵ���

    bool isMoveZ = false; // ���� �̵� �� : �⺻�� "�ƴ�"
    bool isMoveX = false; // �¿� �̵� �� : �⺻�� "�ƴ�"

    if (KEY_PRESS('W'))
    {
        velocity.z += DELTA; //�ӷ� ���ؿ� �ð� �����ŭ ������ �ֱ�
        isMoveZ = true; //���� �̵� ����
    }

    if (KEY_PRESS('S'))
    {
        velocity.z -= DELTA;
        isMoveZ = true;
    }

    if (KEY_PRESS('A'))
    {
        velocity.x -= DELTA;
        isMoveX = true;
    }

    if (KEY_PRESS('D'))
    {
        velocity.x += DELTA;
        isMoveX = true;
    }





    // �ӷ� ���� ���� x,z (������ xy) ���� �Ǵ��ؼ� ������ ���Ѵ�
    if (velocity.Length() > 1) velocity.Normalize(); // ����ȭ�� Ǯ��? -> �ӷ��� �������� �ͻӸ� �ƴ϶�
                                                     // �밢�� �Է¿��� ū ������ �Ͼ�� �ȴ�
    if (velocity.x != 0 || velocity.z != 0) {
        if (!Audio::Get()->IsPlaySound("PlayerWalk")) {
            Audio::Get()->Play("PlayerWalk",GlobalPos());
        }
    }


    if (!isMoveZ) //���� �̵� ���� �ƴ� ���� �ӷ� ������ �ٽ� ������
        velocity.z = Lerp(velocity.z, 0, deceleration * DELTA); //�������� ����

    if (!isMoveX)
        velocity.x = Lerp(velocity.x, 0, deceleration * DELTA);

    //Ʈ�������� ���⿡ ���� ȸ�������� �������� �ٲٱ�
    Matrix rotY = XMMatrixRotationY(Rot().y);
    //����(=�� �������� ���� ������)���� �� ȸ�� ������ �ӷ� ������ ���ļ� ���� ����
    //=���� ȸ�� ��Ȳ���� �ӷ� �������� ���ϱ� ���� ������ ����ϴ� ���� ����
    Vector3 direction = XMVector3TransformCoord(velocity, rotY);

    //���� ������� ����ȭ�� �ӷ� ����(=����)
    // + ������ ���� (���� �ӷ�)
    // + ������ ȸ�� ����
    //....�� ���ļ� �����ǿ� �ֵ��� �Ѵ�
    Pos() += direction * moveSpeed * DELTA * -1; // �̵� ����
                                                 // -1 : "�ڷ� ����" -> ���� ������ �����ϱ�.

    // -> ��Ÿ ��� ��� ������ �̿��ؼ� �̵� ����
}

void Naruto::Rotate()
{
    //�Ʒ� �ڵ� ������ �߰����� ������ Ŀ���� �ٽ� �����̸鼭 ��Ÿ ����
    Vector3 delta = mousePos - Vector3(CENTER_X, CENTER_Y);
    // ���콺�� ������ ������ ��ġ�� �߰����� ����
    SetCursorPos(clientCenterPos.x, clientCenterPos.y);
    // -> Ȥ�� �� �� ���� ���� �ÿ� �� �� �ȴٸ� CENTER_XY�� clinetCenterPos ǥ�ø� �ٲ㺸�� ������

    //��Ÿ�� ���� ĳ���Ϳ� ī�޶� ������ ��� ȸ��
    Rot().y += delta.x * rotSpeed * DELTA; // ĳ���� �¿�ȸ�� (���� ���̶� ī�޶� ���� ��)
    CAM->Rot().x -= delta.y * rotSpeed * DELTA; // ī�޶� ����ȸ��
}

void Naruto::Attack()
{
    if (curState == THROW) return; // �̹� ������ ���̸� ���� (�޺��Ұ�)

    if (KEY_DOWN(VK_LBUTTON))
    {
        SetState(THROW);

        //���̰� ������
    }
}

void Naruto::SetAnimation()
{
    if (curState == THROW) return;

    if (velocity.z > 0.1f) // �ӷ� ������ ���� ������ +��
        SetState(RUN_F);
    else if (velocity.z < -0.1f) // �� ���� -��
        SetState(RUN_B);
    else if (velocity.x > 0.1f) // �¿� ���� +��
        SetState(RUN_R);
    else if (velocity.x < -0.1f) //�¿� ���� -��
        SetState(RUN_L);
    else SetState(IDLE); // ������ ������
}

void Naruto::SetState(State state)
{
    if (state == curState) return;

    curState = state;
    PlayClip((int)state);
}

void Naruto::Throw()
{
    //���̰� �������� ������ ���ư���
    KunaiManager::Get()->Throw(tmpCollider->GlobalPos(), Back()); // Back : �������� ������ �� �ݿ�
}

void Naruto::EndThrow()
{
    SetState(IDLE); //�� �������Ƿ� ���� ���� ���ư���
}

void Naruto::MouseDirection()
{
    Ray ray;
    ray.pos = GlobalPos();
    ray.dir = Back();
    Contact contact;
    value=200;
    for (UINT i = 0; i < 2; ++i) {
        if (cubes[i]->GetCollider()->IsRayCollision(ray, &contact)) {
            if (contact.distance < value) {
                value = contact.distance;
            }

        }
    }


}
