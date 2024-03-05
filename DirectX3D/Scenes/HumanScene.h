#pragma once
class HumanScene : public Scene
{
public:
	HumanScene();
	~HumanScene();

	// Scene을(를) 통해 상속됨
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual void GUIRender() override;

private:
	Human* human;
};

