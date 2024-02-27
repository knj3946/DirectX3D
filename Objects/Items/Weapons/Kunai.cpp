#include "Framework.h"

Kunai::Kunai(Transform* transform) : transform(transform)
{
    transform->Scale() = { 3,3,3 }; // ũ�� �⺻���� 1
                                      // ���߿� ũ�Ⱑ �ٲ��� �ϸ� �ͼ� �����ϰ�

    collider = new SphereCollider();
    collider->SetParent(transform);

    collider->Scale() = { 2,2,2 }; //ũ�� �⺻���� 1.0
    collider->Pos() = {};            //��ġ �⺻�� : �θ� ��ġ
}

Kunai::~Kunai()
{
    delete collider;
}

void Kunai::Update()
{
    //��Ȱ��ȭ �߿��� �� ������
    if (!transform->Active()) return;

    time += DELTA; // �ð� ����� ���� ������ ����

    if (time > LIFE_SPAN)
    {
        transform->SetActive(false);
        transform->Pos() = {0,0,0};
        trail->GetStartEdge()->Pos() = { 0,0,0 };
        trail->GetEndEdge()->Pos() = { 0,0,0 };
        trail->ResetVertices();
        trail->UpdateWorld();
    }
        

    transform->Pos() += direction * speed * DELTA;
    transform->UpdateWorld();
    collider->UpdateWorld();

    if (trail)
        trail->Update();
}

void Kunai::Render()
{
    collider->Render();
}

void Kunai::GUIRender()
{
    ImGui::Text("x : %f, y: %f, z : %f", transform->Pos().x, transform->Pos().y, transform->Pos().z);
}

void Kunai::Throw(Vector3 pos, Vector3 dir)
{
    //Ȱ��ȭ
    transform->SetActive(true);

    transform->Pos() = pos;
    direction = dir;

    startEdge->Pos() = transform->Pos();
    endEdge->Pos() = transform->Pos();
    startEdge->UpdateWorld();
    endEdge->UpdateWorld();
    trail->Init();
    trail->SetActive(true);
    //���⿡ �°� ��(=Ʈ������) ȸ�� ����
    transform->Rot().y = atan2(dir.x, dir.z) - XM_PIDIV2; //���� ���� + �� ���鿡 ���� ����
                                                          //���� ���� 90�� ���ư� �־���
    //transform->Rot().y = atan2(dir.x, dir.z) - XMConvertToRadians(90);
    //                                           �� ������ ȣ���� �ٲ��ִ� �Լ�

    time = 0; //����ð� ����
}
