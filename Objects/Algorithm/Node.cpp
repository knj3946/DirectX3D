#include "Framework.h"

Node::Node(Vector3 pos, int index)
    : index(index)
{
    Pos() = pos; //(����°� �԰� ����)�浹ü�� ��ġ
    UpdateWorld();
}

Node::~Node()
{
    for (Edge* edge : edges)
        delete edge;
}

void Node::Render()
{
    //���� ����� ���º��� �浹ü ���� �ٸ��� ǥ��
    switch (state)
    {
    case Node::NONE:
        SetColor({ 0, 1, 1, 1 }); // û�ϻ�
        break;
    case Node::OPEN:
        SetColor({ 0, 0, 1, 1 }); // �Ķ���
        break;
    case Node::CLOSED:
        SetColor({ 0, 0, 0, 1 }); // ���
        break;
    case Node::USING:
        SetColor({ 0, 1, 0, 1 }); // �ʷϻ�
        break;
    case Node::OBSTACLE:
        SetColor({ 1, 0, 0, 1 }); // ������
        break;
    }

    BoxCollider::Render();
}

void Node::AddEdge(Node* node)
{
    Edge* edge = new Edge();
    edge->index = node->index;
    edge->cost = Distance(node->GlobalPos(), GlobalPos()); // ���� ��Ģ : ������ �Ÿ��� �� �̵����

    edges.push_back(edge);
}