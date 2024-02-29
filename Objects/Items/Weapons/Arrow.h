#pragma once

class Arrow
{
	// ����ü �ϳ��� �Ѱ��ϴ� Ŭ����

private:
	// ������ �ʿ��� �ֿ� ������
	float LIFE_SPAN = 2.f; // life span : ���� �� ��Ȱ��ȭ���� �ɸ��� �ð�



public:
	Arrow(Transform* transform,int id,bool isDropItem=false); // �ν��Ͻ��� ������ Ʈ������ �Ű� ����
	~Arrow();

	void Update();
	void Render();

	void Throw(Vector3 pos, Vector3 dir);

	SphereCollider* GetCollider() { return collider; }
	Transform* GetTransform() { return transform; }

	float GetDamage() { return damage; }

	class Trail* GetTrail() { return trail; }

	void SetOutLine(bool flag);
	bool IsOutLine() { return outLine; };
private:



private:
	Transform* transform; // �ν��Ͻ��� ������ �ϱ� ������ transform�� ������ ����
	SphereCollider* collider;

	float speed = 30;
	float time = 0; // ������ �ð�
	float damage = 30; // �ӽ� �������� ����

	Vector3 direction;

	class Trail* trail; // ���� ��ƼŬ
	Transform* startEdge; // ������ ���۵� ��
	Transform* endEdge;

	bool outLine = false;
	int index;
	bool isDropItem;
};

