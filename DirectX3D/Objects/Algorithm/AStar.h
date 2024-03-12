#pragma once

class AStar
{
    // A* �˰��� : ���� �� ã�⿡ Ȱ��� �� ����-�˻� �˰���
private:
    class RayBuffer : public ConstBuffer
    {
    private:
        struct Data
        {
            Float3 pos;
            Float3 dir;

            float distance;
            UINT obstacleSize;
        };

    public:
        RayBuffer() : ConstBuffer(&data, sizeof(Data))
        {
        }

        Data& Get() { return data; }

    private:
        Data data;
    };

    struct InputDesc
    {
        Float3 min;
        Float3 max;

        Float3 boxPos;

        Float3 axisX;
        Float3 axisY;
        Float3 axisZ;
    };

    struct OutputDesc
    {
        bool flag;
        float distance;
        Float3 hitPoint;
    };

    //typedef Terrain LevelData;
    typedef TerrainEditor LevelData;
public:

    AStar(UINT width = 10, UINT height = 10); //�԰��� �����鼭 ����
    ~AStar();

    void Update();
    void Render();

    void SetNode() {} // �⺻�� (������ ���� �ʴ� ��� ��ġ) - ����α�
    void SetNode(LevelData* terrain); //������ ��� �޾� ��� ��ġ (���� ���� �Լ�) - �ۼ����
    //�� ������ ������ �ϴ���? -> ���ӿ��� �� �����̴ϱ�
    //�� class��? -> ������ ������ ��ã�⸦ ��� �ϴϱ�
    //���ɼ��� ������� ��ȣ������ ��������.

    int FindCloseNode(Vector3 pos); //�������� ��ǥ�� ���� ��Ȯ�� ��� ���
    int FindRandomPos(Vector3 pos, float range); //������� ���

    void GetPath(IN int start, IN int end, OUT vector<Vector3>& path); //��� ���� �� path�� �����ϱ�
    void MakeDirectionPath(IN Vector3 start, IN Vector3 end, OUT vector<Vector3>& path);
    // ������� ����
    // (�� ��Ȯ���� ��ֹ� ������ ���� ��������)
    // 3D������ �ʼ� ����

    bool IsCollisionObstacle(Vector3 start, Vector3 end);
    bool IsCollisionObstacleCompute(Vector3 start, Vector3 end);
    //���ۿ��� ������ �������� �̾��� ��, ��ֹ��� �ε�ġ�°�?

    void AddObstacle(Collider* collider); //��ֹ� �����߰� (Ŭ���� �� ȣ�⵵ ����)
    void AddObstacleObj(Collider* collider);

    Vector3 FindPos(Vector3 _pos,float _range);

    bool IsObstacle() {
        for (auto p : nodes) {
            if (p->state == Node::OBSTACLE)
                return true;
     } 
        return false;
    }
    

private:
    void SetEdge(); // ������ �߰�
    int GetMinNode(); //�ּҺ�� ��� ���ϱ�
    void Extend(int center, int end); //������ ��ǥ���� �ֺ� ��ǥ ��ΰ���� �����ϱ�

    void Reset();

    float GetManhattanDistance(int start, int end);         // ����ư �Ÿ�
    float GetDiagonalManhattanDistance(int start, int end); // �缱�� ������ ����ư �Ÿ�

    // ����ư �Ÿ� (or �ý� �Ÿ�)
    // : �԰��� �ִ� �������� �Ÿ��� ����� ��, ����/�������� �Ÿ��� �����ϰ�(�����Ÿ� ��� �� ��)
    //   ��� ���� �԰ݿ� ���߾ ���ο� ���� (��쿡 ���� �缱 ����) �������� �Ÿ��� ���� ��
    //   Ȥ�� �׷��� �� �Ÿ� ��ġ�� ��Ī�Ѵ�

private:

    vector<Node*> nodes;
    Heap* heap;

    vector<Collider*> obstacles;

    UINT width, height;
    Vector2 interval; //��� ����. Vector3���� ������ ������ 2���� ��� ��ġ�� �� �Ŷ� Vector2

    RayBuffer* rayBuffer;
    StructuredBuffer* structuredBuffer;

    vector<InputDesc> inputs;
    vector<OutputDesc> outputs;

    ComputeShader* computeShader;
};

