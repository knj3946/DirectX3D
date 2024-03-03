#include "Framework.h"

DepthStencilState::DepthStencilState()
{
    // ����(����), ���ٽ� ������Ʈ : �̹����� �켱����(Z�� ��ǥ Ȥ�� ������¼���) ���� �� (����)
    //                             �̹����� ������ �� ���̰� �ϴ� ���� ���ο� ���� ����(���ٽ�)
    //                             -> �� ���� ���� �̹����� ������ ����� ����ϴ� ������Ʈ

    SetMode(ALL);

    desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;       //���ٽ� ��� ��� (�� �� ��)
    desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;  
    desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    Changed();
}

DepthStencilState::~DepthStencilState()
{
    state->Release();
}

void DepthStencilState::SetState()
{
    DC->OMSetDepthStencilState(state, 1);
}

void DepthStencilState::DepthEnable(bool value)
{
    desc.DepthEnable = value;

    Changed();
}

void DepthStencilState::DepthWriteMask(D3D11_DEPTH_WRITE_MASK value)
{
    desc.DepthWriteMask = value;

    Changed();
}

void DepthStencilState::Changed()
{
    if (state != nullptr)
        state->Release();

    DEVICE->CreateDepthStencilState(&desc, &state);
}

void DepthStencilState::SetMode(Mode mode)
{
    desc = CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT{} };
    //
    switch (mode)
    {
    case DepthStencilState::ALL:
    {
        desc.DepthEnable = true;                            //���� ��뿩��
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;   //���̿� ���� �켱���� ���� : ALL = ���� ������
        desc.DepthFunc = D3D11_COMPARISON_LESS;             //���� �Ǵ� ���� : (ī�޶���� �Ÿ�����) Z�� ���� ���� ������� = ����� ���� ������ �� ���δ�

        desc.StencilEnable = true;      //���ٽ� ��뿩��
        desc.StencilReadMask = 0xff;    //���ٽ��� �����ؼ� �̹����� ���� ��, ������ �� ����ũ�� �����Ͱ�
        desc.StencilWriteMask = 0xff;   //����� ��, ������ �� ����ũ�� �����Ͱ�

        // ���� ���ٽ��� ���⼭ �ɼ��� ������ �����ϱ⺸��
        // �ʿ��� �� ���� Ŭ������ ����ų�, �ƴϸ� ���ٽ��� ������ ����ũ�� ������ ��
        // �ٸ� ��ü�� ����� ȣ���ϴ� ��찡 �� ����

        // * Ư�� ���� ���ٽ��� Ư��(����)�� �츮�� -> �� ������Ʈ�� ���ؼ��� (������ �̻�����)
        //   �̹����� ������ ��� ��Ҹ� �и���ų �� �ִ�
    }

    case DepthStencilState::WRITE:
    {
        desc.StencilEnable = TRUE;
        desc.StencilWriteMask = 0xFF;
        //
        desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    }
    break;
    case DepthStencilState::MASK:
    {
        desc.DepthEnable = FALSE;
        //
        desc.StencilEnable = TRUE;
        desc.StencilReadMask = 0xFF;
        //
        desc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
        desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    }
    break;
    }
    //
    Changed();
}