#pragma once

class Heap
{
    // �� : �޸��� �� ������ �䳻����, �ڷᰡ ������ �װų� �����ϰų� ����� ���� ���
    //      A* �˰����򿡼��� ��ã�⿡ ���� ����� ������ ���� �� �ִ�
    //      �ϴ� �ൿ�� ����ϰ� ���� �Ŷ� ���̶� �̸��� ���� ��. ��¥ ������ �������

public:
    Heap();
    ~Heap();

    void Insert(Node* node); // "��"�� ���ο� ��� �߰��ϱ�
    Node* DeleteRoot();      // "��"�� �����ϴ� ��� �� ���� �ؿ� �ִ� ��Ʈ ��Ҹ� ������,
                             // ���� ����� ��ȯ�ޱ�

    void UpdateUpper(int index); // ������ ������ "Ʈ���� ����"�� ������Ʈ
    void UpdateLower(int index); // ������ ������ "Ʈ���� �Ʒ�"�� ������Ʈ
                                 // "Ʈ����" Ȥ�� "Ʈ���� �ɷ� ���� ��" :
                                 // �ڷ�� ���ͷ� ���� ������, ���� ������ (�ε��� ���)
                                 // ��ġ ���� Ʈ���� ��ó�� ����� �Ŷ� (= �� �䳻)

    void Swap(int n1, int n2); // �Ű������� ���� �� ����� ���� ��ȯ

    void Clear() { heap.clear(); } //���� ��� ����
    bool Empty() { return heap.empty(); } //������ �Ǻ��Լ� ����

private:
    vector<Node*> heap; // ��带 Ȱ���� ����(=�ڷ� ���� ����)
                        // �� Ŭ������ ����
};
