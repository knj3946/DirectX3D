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

	void SetTrailActive(bool val) { trail->Init();  trail->SetActive(val); }

	void SetInteraction(bool val) { collider->SetActive(val);  trail->Init();  trail->SetActive(val); }

private:
	CapsuleCollider* collider;
	float damage = 20.0f;

	class Trail* trail; // 궤적 파티클
	Transform* startEdge; // 궤적이 시작될 곳
	Transform* endEdge;
};


