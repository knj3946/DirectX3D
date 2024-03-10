#pragma once
class ProgressBar : public Quad
{
    // ���α׷��� �� (�����Ȳ ǥ�ñ�) : Ư�� ������ ����, �޼�ġ ���� �׸��� ������ ��Ÿ���� UI
    //                         ������� Ư���� (������ ä������ �����) ü�¹� ��� ���� ����

public:
    ProgressBar(wstring frontImageFile, wstring backImageFile);
    ~ProgressBar();

    void Render();

    void SetAmount(float value); //amount : (��ü �翡 ����) ���� ����ġ
    void SetAlpha(float value) { valueBuffer2->Get()[0] = value; }
private:
    Texture* backImage; // ��� �̹���.
                        // �� Ŭ������ �����̱� ������ ��������� �̹��� 2���� ���� ��

    float fillAmount = 1; // �⺻ ����ġ = 1 = "��ü, �� á��"
    float Alpha = 1.f;

    // �� ������ �־ ��� ��������... ���� �ӵ��� ���̱� ���ؼ�
    // �����͸� ������ ����(Ȥ�� �ٸ� ����)���� �ű���� �Ѵ�

    FloatValueBuffer* valueBuffer; // �Ҽ� ����� ���� : �����̵��� ���۷� ���� �� �ִ�!
                                   // ���۰� �������� �ش� �����ʹ� ������ ��������,
                                   // ��ü ���� ��������, �޸� �δ��� Ŀ����.
    FloatValueBuffer* valueBuffer2;
};

