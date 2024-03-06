#pragma once

class KunaiManager : public Singleton<KunaiManager>
{
private:
	friend class Singleton;
	UINT SIZE = 10; // ���ÿ� ��� ������ ������ �ִ� ����

public:
	KunaiManager();
	~KunaiManager();

	void Update();
	void Render();

	void Throw(Vector3 pos, Vector3 dir); // ���� ��ü���� �����ϱ� ���� �Ű� �Լ�

    bool IsCollision(); //�� �����̰� �ٸ� ��ü�� �浹�ߴ��� �Ǻ��ϱ� ����

private:
	// ������ ��¿� ��
	ModelInstancing* kunaiInstancing; // ������ ��
	vector<Kunai*> kunaies;
	vector<Collider*> wallColiders;
};
