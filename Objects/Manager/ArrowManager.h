#pragma once
class ArrowManager : public Singleton<ArrowManager>
{
private:
	friend class Singleton;
	UINT SIZE = 10; // ���ÿ� ��� ������ ���� �ִ� ����

public:
	ArrowManager();
	~ArrowManager();

	void Update();
	void Render();

	void Throw(Vector3 pos, Vector3 dir); // ���� ��ü���� �����ϱ� ���� �Ű� �Լ�

	bool IsCollision(); //�� ���̰� �ٸ� ��ü�� �浹�ߴ��� �Ǻ��ϱ� ����

	void OnOutLineByRay(Ray ray);

	ModelInstancing* GetInstancing() { return arrowInstancing; }

private:
	// ���� ��¿� ��
	ModelInstancing* arrowInstancing; // ���� ��
	vector<Arrow*> arrows;
	vector<Collider*> wallColiders;
	int count = 0;
};

