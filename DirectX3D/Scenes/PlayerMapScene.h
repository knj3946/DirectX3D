#pragma once
class PlayerMapScene : public Scene
{
public:
	PlayerMapScene();
	~PlayerMapScene();

	// Scene��(��) ���� ��ӵ�
	void Update() override;
	void PreRender() override;
	void Render() override;
	void PostRender() override;
	void GUIRender() override;

	void CreateColliderModel(string mName, string mTag, Vector3 mScale, Vector3 mRot, Vector3 mPos);

private:
	vector<ColliderModel*> colliderModels;
	ColliderModel* model;

	TerrainEditor* terrain;

	SkyBox* skyBox;

	BlendState* blendState[2];

	AStar* astar;
	ModelAnimator* player;
};
