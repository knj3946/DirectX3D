#pragma once
class GameMenu : public Transform
{
public:
	GameMenu(Vector3 scale, Vector3 pos);
	~GameMenu();

	void Update();
	void Render();

	void AddMenu(Quad* quad);

	Quad* GetQuad(UINT idx) { return menuList[idx]; }

	UINT GetMenuSize() { return menuList.size(); }

	void SetMenuBackGround(Quad* quad);
	void SetMenuBackGroundPos(UINT menuIdx);
private:
	vector<Quad*> menuList;
	Quad* menuBackGround;
};

