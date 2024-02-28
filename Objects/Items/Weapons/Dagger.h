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
private:
	CapsuleCollider* collider;
	float damage = 60.0f;
};

