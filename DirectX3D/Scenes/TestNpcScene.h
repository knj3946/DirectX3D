#pragma once

class TestNpcScene : public Scene
{
	// NPC를 구현하고 테스트를 진행하기 위한 Scene 스크립트

	// 작성자 : 박성진
	// 2월 15일 작성, VS2022
	// 시프트 + 좌클릭시 fox 이동
	// 너무멀어지면 제자리에 멈춤, 근접하면 attack
	// 터레인에디터를 맵으로 적용 -> Fox, Robot, RobotA, AStar에서 Terrain대신 TerrainEditor로 바꿨음

public:
	TestNpcScene();
	~TestNpcScene();

	// Scene을(를) 통해 상속됨
	void Update() override;
	void PreRender() override;
	void Render() override;
	void PostRender() override;
	void GUIRender() override;

private:
	//Terrain* terrain;
	TerrainEditor* terrain;

	AStar* aStar;
	AStar* aStar2;

	Fox* fox;
	Naruto* naruto;
	RobotA* robot;
	Orc* orc;

	UINT orcCount = 0;

	ModelAnimatorInstancing* instancing;

	// 그림자 관련
	//Shadow* shadow;
	//LightBuffer::Light* light;
};
