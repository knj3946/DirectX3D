#pragma once
class GameMapScene : public Scene
{
public:
	GameMapScene();
	~GameMapScene();

	// Scene을(를) 통해 상속됨
	void Update() override;
	void PreRender() override;
	void Render() override;
	void PostRender() override;
	void GUIRender() override;

	void FirstLoading();

	

private:

	// 임시로 여기서
	bool isPauseGame = false;

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

	// 몬스터 위치 세팅 데이터
	vector<vector<Vector3>> srt =
	{
		{Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(150.f, 0.f, 175.f)},
		{Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(100, 0, 185)},
		//{Vector3(0.03f, 0.03f, 0.03f), Vector3(0, 0, 0), Vector3(85, 0, 120)},

	};

	vector<Vector3> patrolPos =
	{
		Vector3(150.f, 0.f, 210.f),
		Vector3(50.f, 0.f, 185.f),
		//Vector3(40.f, 0.f, 120.f),

	};

};

