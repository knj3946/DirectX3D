#pragma once
class EndingCreditScene : public Scene
{
public:
	EndingCreditScene();
	~EndingCreditScene();

	// Scene을(를) 통해 상속됨
	void Update() override;
	void PreRender() override;
	void Render() override;
	void PostRender() override;
	void GUIRender() override;

private:
	Quad* endingCredit;
	VideoPlayer* videoPlayer;
};

