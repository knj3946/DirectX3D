#include "Framework.h"

VideoPlayer::VideoPlayer(UINT width,UINT height)
{
	FOR(2) blendState[i] = new BlendState();
	blendState[1]->Alpha(true); //이미지 배경색 투명화 적용

	quad = new Quad(Vector2(width, height));
	quad->SetParent(this);
	quad->GetMaterial()->SetDiffuseMap(L"Textures/Color/White.png");
	quad->UpdateWorld();

	alpha = 0;
}

VideoPlayer::~VideoPlayer()
{
	delete quad;
}

void VideoPlayer::Update()
{
	UpdateWorld();
	quad->UpdateWorld();

	if (curCutTime <= 0)
	{
		picCount++;
		quad->GetMaterial()->SetDiffuseMap(L"Videos/pic" + to_wstring(picCount) + L".png");
		curCutTime = cutCool;

		if (picCount == picMaxCount)
			picCount = 0;
	}
	else
	{
		if (cutCool -1.f < curCutTime)
		{
			alpha += DELTA;

			if (alpha > 1.f)
				alpha = 1.f;
		}

		if (curCutTime < 1)
		{
			alpha -= DELTA;

			if (alpha < 0)
				alpha = 0;
		}
		curCutTime -= DELTA;
	}
	quad->GetMaterial()->GetData().diffuse.w = alpha;
}

void VideoPlayer::PreRender()
{

}

void VideoPlayer::Render()
{

}

void VideoPlayer::PostRender()
{
	blendState[1]->SetState();
	quad->Render();
	blendState[0]->SetState();
}

void VideoPlayer::GUIRender()
{
}
