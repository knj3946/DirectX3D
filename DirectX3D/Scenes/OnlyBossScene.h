#pragma once
class OnlyBossScene : public Scene
{
public:
	OnlyBossScene();
	~OnlyBossScene();

	// Scene을(를) 통해 상속됨
	void Update() override;
	void PreRender() override;
	void Render() override;
	void PostRender() override;
	void GUIRender() override;

	void FirstLoading();

private:
	vector<ColliderModel*> colliderModels;
	ColliderModel* model;

	TerrainEditor* terrain;

	SkyBox* skyBox;
	AStar* aStar;
	BlendState* blendState[2];

	Player* player;
	Boss* boss;
	Bow* bow;

	Shadow* shadow;

	float waitSettingTime = 0.0f;
};