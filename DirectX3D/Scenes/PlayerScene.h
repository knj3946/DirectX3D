#pragma once
class PlayerScene : public Scene
{
public:
	PlayerScene();
	~PlayerScene();

	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual void GUIRender() override;

private:
	Player* player;

	BoxCollider* box1;
	BoxCollider* box2;

	vector<Collider*> colliders;

	int onColliderNum = -1;
};

