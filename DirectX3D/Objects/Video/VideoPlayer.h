#pragma once
class VideoPlayer : public Transform
{
public:
	VideoPlayer(UINT width, UINT height);
	~VideoPlayer();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void GUIRender();

private:
	Quad* quad;

	UINT picCount = 0;
	UINT picMaxCount = 3;

	float curCutTime = 1.0f;
	float cutCool = 5.f;

	float alpha = 1.f;

	BlendState* blendState[2];
};

