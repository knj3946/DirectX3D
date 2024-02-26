#pragma once
class EyesRaySearchScene : public Scene
{
public:
	EyesRaySearchScene();
	~EyesRaySearchScene();

	// Scene��(��) ���� ��ӵ�
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual void GUIRender() override;

private:
	TerrainEditor* terrain;

	BoxCollider* box1;
	Player* player;
};

