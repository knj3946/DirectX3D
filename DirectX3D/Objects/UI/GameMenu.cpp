#include "Framework.h"
#include "GameMenu.h"

GameMenu::GameMenu(Vector3 scale, Vector3 pos)
{
	Pos() = pos;
	Scale() = scale;
	UpdateWorld();
}

GameMenu::~GameMenu()
{
	for (Quad* quad : menuList)
	{
		delete quad;
	}
}

void GameMenu::Update()
{
	for (Quad* quad : menuList)
	{
		quad->UpdateWorld();
	}
}

void GameMenu::Render()
{
	menuBackGround->Render();

	for (Quad* quad : menuList)
	{
		quad->Render();
	}
}

void GameMenu::AddMenu(Quad* quad)
{
	quad->SetParent(this);
	quad->Pos() = { 0,((UINT)menuList.size())*(-80.f),0};
	quad->UpdateWorld();
	menuList.push_back(quad);
}

void GameMenu::SetMenuBackGround(Quad* quad)
{
	menuBackGround = quad;
	menuBackGround->SetParent(this);
	menuBackGround->UpdateWorld();
}

void GameMenu::SetMenuBackGroundPos(UINT menuIdx)
{
	menuBackGround->Pos() = menuList[menuIdx]->Pos();
	menuBackGround->UpdateWorld();
}
