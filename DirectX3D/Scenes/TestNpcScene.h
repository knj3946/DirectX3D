#pragma once

class TestNpcScene : public Scene
{
	// NPC�� �����ϰ� �׽�Ʈ�� �����ϱ� ���� Scene ��ũ��Ʈ

	// �ۼ��� : �ڼ���
	// 2�� 15�� �ۼ�, VS2022
	// ����Ʈ + ��Ŭ���� fox �̵�
	// �ʹ��־����� ���ڸ��� ����, �����ϸ� attack
	// �ͷ��ο����͸� ������ ���� -> Fox, Robot, RobotA, AStar���� Terrain��� TerrainEditor�� �ٲ���

public:
	TestNpcScene();
	~TestNpcScene();

	// Scene��(��) ���� ��ӵ�
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

	// �׸��� ����
	//Shadow* shadow;
	//LightBuffer::Light* light;
};
