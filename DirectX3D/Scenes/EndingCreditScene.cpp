#include "Framework.h"
#include "EndingCreditScene.h"

EndingCreditScene::EndingCreditScene()
{
	endingCredit = new Quad(Vector2(1280,720*6));
	endingCredit->GetMaterial()->SetDiffuseMap(L"Textures/Etc/endingCredit.png");
	endingCredit->Pos() = {CENTER_X,-720.f*2.f,0.f};
	endingCredit->UpdateWorld();

	videoPlayer = new VideoPlayer(192*2,108*2);
	videoPlayer->Pos() = { CENTER_X-300 ,CENTER_Y+100,0};
	videoPlayer->UpdateWorld();
}

EndingCreditScene::~EndingCreditScene()
{
	delete endingCredit;
}

void EndingCreditScene::Update()
{
	endingCredit->Pos().y += DELTA*50;
	endingCredit->UpdateWorld();

	videoPlayer->Update();
	
}

void EndingCreditScene::PreRender()
{
	videoPlayer->PreRender();
}

void EndingCreditScene::Render()
{
	
}

void EndingCreditScene::PostRender()
{
	endingCredit->Render();
	videoPlayer->PostRender();
}

void EndingCreditScene::GUIRender()
{
}
