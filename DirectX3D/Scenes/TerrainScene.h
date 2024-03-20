#pragma once
class TerrainScene : public Scene
{
public:
	TerrainScene();
	~TerrainScene();

	// Scene��(��) ���� ��ӵ�
	void Update() override;
	void PreRender() override;
	void Render() override;
	void PostRender() override;
	void GUIRender() override;
private:
	TerrainEditor* terrain;

	BoxCollider* HeightCollider[6];
};

