#pragma once
class ArrowManager : public Singleton<ArrowManager>
{
private:
	friend class Singleton;
	UINT SIZE = 10; // ���ÿ� ��� ������ ���� �ִ� ����

	struct SpecialKeyUI
	{
		string name;
		Quad* quad;
		bool active;
	};

public:
	ArrowManager();
	~ArrowManager();

	void Update();
	void Render();
	void PostRender();

	void Throw(Vector3 pos, Vector3 dir); // ���� ��ü���� �����ϱ� ���� �Ű� �Լ�

	bool IsCollision(); //�� ���̰� �ٸ� ��ü�� �浹�ߴ��� �Ǻ��ϱ� ����

	void OnOutLineByRay(Ray ray);
	void ActiveSpecialKey(Vector3 playPos, Vector3 offset);


	ModelInstancing* GetInstancing() { return arrowInstancing; }

private:
	// ���� ��¿� ��
	ModelInstancing* arrowInstancing; // ���� ��
	vector<Arrow*> arrows;
	vector<Collider*> wallColiders;
	int count = 0;

	map<string, SpecialKeyUI> specialKeyUI;
};

