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
	void ExecuteSpecialKey();
	void SetActiveSpecialKey(bool active){ specialKeyUI["getItem"].active = active; }


	ModelInstancing* GetInstancing() { return arrowInstancing; }

	int GetCount() { return count; }
	int GetPlayerArrowCount() { return playerArrowCount; }

private:
	// ���� ��¿� ��
	ModelInstancing* arrowInstancing; // ���� ��
	vector<Arrow*> arrows;
	vector<Collider*> wallColiders;
	int count = 0; // �ε����� ����

	int playerArrowCount = 1; // ó���� 3�� ������ ����

	map<string, SpecialKeyUI> specialKeyUI;
};

