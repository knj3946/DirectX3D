#pragma once
class GameSmallMapScene : public Scene
{
public:
	GameSmallMapScene();
	~GameSmallMapScene();

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
	Bow* bow;

	Shadow* shadow;

	float waitSettingTime = 0.0f;
};

