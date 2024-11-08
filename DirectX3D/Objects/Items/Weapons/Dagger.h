#pragma once
class Dagger : public Model
{
public:
	Dagger(Transform* parent);
	~Dagger();

	void Update();
	void Render();
	void PostRender();
	void GUIRender();

	CapsuleCollider* GetCollider() { return collider; }
	float GetDamaged() { return damage; }

	void SetInteraction(bool val) { collider->SetActive(val);  if(val) trail->Init();  trail->SetActive(val); }

private:
	CapsuleCollider* collider;

#if _DEBUG
	float damage = 100.0f;
#else
	float damage = 20.0f;
#endif

	class Trail* trail; // ���� ��ƼŬ
	Transform* startEdge; // ������ ���۵� ��
	Transform* endEdge;
};


