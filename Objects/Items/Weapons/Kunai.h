#pragma once

class Kunai
{
	// ����ü �ϳ��� �Ѱ��ϴ� Ŭ����

private:
	// ������ �ʿ��� �ֿ� ������
	float LIFE_SPAN = 2.f; // life span : ���� �� ��Ȱ��ȭ���� �ɸ��� �ð�



public:
	Kunai(Transform* transform); // �ν��Ͻ��� ������ Ʈ������ �Ű� ����
	~Kunai();

	void Update();
	void Render();

	void Throw(Vector3 pos, Vector3 dir);

	SphereCollider* GetCollider() { return collider; }
	Transform* GetTransform() { return transform; }

	float GetDamage() { return damage; }

	class Trail* GetTrail() { return trail; }
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
};

