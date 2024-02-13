#pragma once
class ModelAnimationScene : public Scene
{
public:
	ModelAnimationScene();
	~ModelAnimationScene();

	// Scene��(��) ���� ��ӵ�
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual void GUIRender() override;

private:
	ModelAnimator* model;
	ModelAnimator* model2;
	ModelAnimator* model3;
	Model* only_model;
	TerrainEditor* terrainEditor;

	Vector3 prevMousePos;

	bool idleFlag = true;

	float jumpCoolDown = 0.0;
	float moveCoolDown = 0.0;

	bool jumpFlag = false;

	bool oneFlag = true;

	int clip = 0;
	int clipSize = 0;

	Model* background;
	SkyBox* skyBox;

	BlendState* blendState[2];
};

