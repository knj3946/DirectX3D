#pragma once
class Trail : public GameObject
{
	//���� ��ƼŬ : ��ƼŬ�� ����, �����δ� �ٸ� ��ƼŬ�� ���� ���α׷� ���ֿ� ���� �ʴ´�
					//�ٸ� ��ƼŬ�� �׸��� ������ �����Ϳ� ���ؼ� ���� ����ϴ� �Ͱ� �ٸ���
	//				������ ������ ������(�̹���)�� ������ ��Ģ�� ���� �κ� ���� ����� �ϱ� ����

public:
	Trail(wstring imageFile,Transform* start, Transform* end, UINT width,float speed);
	~Trail();

	void Init();

	void Update();
	void Render();

	Transform* GetStartEdge() { return start; }
	Transform* GetEndEdge() { return end; }

	void ResetVertices();

private:
	void CreateMesh(); // ������� �̹����� ���� �޽��� �ʿ��ؼ�

private:
	Mesh<VertexUV>* mesh; // ������ (�⺻) ����)
	Transform* start; //������ ���۵Ǵ� ��
	Transform* end; //������ ������ ��
	Transform* offset, * origin; // ������ �߰� ���� �����ڰ� �� �� ������ ���⼭�� ����

	UINT width; // ������ ���� ���� ��ƼŬ�� �׷��� �̹��� ���� ũ��
	float speed; //������ ����, ��� �ӵ�

	//�̹��� ����� ���� ���� ����
	RasterizerState* rasterizerState[2];
	BlendState* blendState[2];
};

