#include "Framework.h"
#include "MenuManager.h"

MenuManager::MenuManager()
{
	//���� �ε�
	gameStartScreen = new Quad(Vector2(1280, 720));
	gameStartScreen->GetMaterial()->SetDiffuseMap(L"Textures/Etc/introImage.png");//�ӽ�
	gameStartScreen->Pos() = { CENTER_X, CENTER_Y, 0 };
	gameStartScreen->UpdateWorld();

	gameFailScreen = new Quad(Vector2(1280, 720));
	gameFailScreen->GetMaterial()->SetDiffuseMap(L"Textures/Etc/failScreen.png");//�ӽ�
	gameFailScreen->Pos() = { CENTER_X, CENTER_Y, 0 };
	gameFailScreen->UpdateWorld();

	gameEndScreen = new Quad(Vector2(1280, 720));
	gameEndScreen->GetMaterial()->SetDiffuseMap(L"Textures/Etc/sun.jpg");//�ӽ�
	gameEndScreen->Pos() = { CENTER_X, CENTER_Y, 0 };
	gameEndScreen->UpdateWorld();


	loadingBar = new ProgressBar(L"Textures/UI/hp_bar2.png", L"Textures/UI/hp_bar2_BG.png"); //�ӽ�
	loadingBar->Scale() = { 0.2,0.1,0.1 };
	loadingBar->Pos() = { CENTER_X,CENTER_Y - 200,0 };
	loadingBar->UpdateWorld();

	gameMenu = new GameMenu(Vector3(1,1,1),Vector3(CENTER_X, CENTER_Y - 200, 0));

	Quad* menu1 = new Quad(Vector2(340, 80));
	menu1->GetMaterial()->SetDiffuseMap(L"Textures/UI/menu_1.png");
	gameMenu->AddMenu(menu1);
	

	Quad* menu2 = new Quad(Vector2(340, 80));
	menu2->GetMaterial()->SetDiffuseMap(L"Textures/UI/menu_exit.png");
	gameMenu->AddMenu(menu2);

	Quad* menuBackGround = new Quad(Vector2(340, 80));
	menuBackGround->GetMaterial()->SetDiffuseMap(L"Textures/UI/menu_back.png");
	menuBackGround->GetMaterial()->GetData().diffuse.w = 1.9f;
	gameMenu->SetMenuBackGround(menuBackGround);

	failMenu = new GameMenu(Vector3(1, 1, 1), Vector3(CENTER_X, CENTER_Y, 0));

	Quad* failMenu1 = new Quad(Vector2(340, 80));
	failMenu1->GetMaterial()->SetDiffuseMap(L"Textures/UI/fail_1.png");
	failMenu->AddMenu(failMenu1);

	Quad* failMenu2 = new Quad(Vector2(340, 80));
	failMenu2->GetMaterial()->SetDiffuseMap(L"Textures/UI/menu_exit.png");
	failMenu->AddMenu(failMenu2);

	Quad* failMenuBackGround = new Quad(Vector2(340, 80));
	failMenuBackGround->GetMaterial()->SetDiffuseMap(L"Textures/UI/menu_back.png");
	failMenuBackGround->GetMaterial()->GetData().diffuse.w = 1.9f;
	failMenu->SetMenuBackGround(failMenuBackGround);

}

MenuManager::~MenuManager()
{
	delete gameStartScreen;
	delete gameFailScreen;
	delete gameEndScreen;
	delete loadingBar;
	delete gameMenu;
	delete failMenu;
	
}

void MenuManager::Render()
{
}

void MenuManager::PostRender()
{
	if (game_select == 0)
	{
		gameStartScreen->Render();
		loadingBar->Render();
	}
	if (loadingRate < 100)
		return;

	if (game_select == 0 && loadingRate >= 100)
	{
		gameStartScreen->Render();
		gameMenu->Render();
	}

	if (failFlag)
	{
		gameFailScreen->Render();
		failMenu->Render();
	}
	else if (endFlag)
	{
		gameEndScreen->Render();
	}
}

void MenuManager::GUIRender()
{
	if (loadingRate < 100)
		return;
}

void MenuManager::FirstLoading(Event event)
{
	event();
}

void MenuManager::GameMenuUpdate()
{

	if (KEY_DOWN(VK_UP))
	{
		menu_cursor--;
		if (menu_cursor < 1)
			menu_cursor = 1;
	}

	if (KEY_DOWN(VK_DOWN))
	{
		menu_cursor++;
		if (menu_cursor > 2)
			menu_cursor = 2;
	}
	if (KEY_DOWN(VK_RETURN))
	{
		game_select = menu_cursor;
		return;
	}

	gameMenu->SetMenuBackGroundPos(menu_cursor - 1);
	/*
	FOR(gameMenu->GetMenuSize())
	{
		if (menu_cursor == i + 1)
			gameMenu->GetQuad(i)->GetMaterial()->SetDiffuseMap(L"Textures/Color/Black.png");//�ӽ�
		else
			gameMenu->GetQuad(i)->GetMaterial()->SetDiffuseMap(L"Textures/Color/Green.png");//�ӽ�
	}
	*/
}

void MenuManager::FailMenuUpdate()
{
	if (KEY_DOWN(VK_UP))
	{
		fail_cursor--;
		if (fail_cursor < 1)
			fail_cursor = 1;
	}

	if (KEY_DOWN(VK_DOWN))
	{
		fail_cursor++;
		if (fail_cursor > 2)
			fail_cursor = 2;
	}
	if (KEY_DOWN(VK_RETURN))
	{
		fail_select = fail_cursor;
		return;
	}

	failMenu->SetMenuBackGroundPos(fail_cursor - 1);
	
}

bool MenuManager::IsPickGameExit()
{
	if (game_select == 2)
		return true;
	
	return false;
}

void MenuManager::SetLoadingRate(float rate)
{
	this->loadingRate = rate;
	loadingBar->SetAmount(loadingRate / 100.f);

	if(rate >= 100)
		loadingFlag = true;
}
