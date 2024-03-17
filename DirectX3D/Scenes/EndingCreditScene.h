#pragma once
class EndingCreditScene : public Scene
{
public:
	EndingCreditScene();
	~EndingCreditScene();

	// Scene��(��) ���� ��ӵ�
	void Update() override;
	void PreRender() override;
	void Render() override;
	void PostRender() override;
	void GUIRender() override;

private:
	Quad* endingCredit;
	VideoPlayer* videoPlayer;
};

