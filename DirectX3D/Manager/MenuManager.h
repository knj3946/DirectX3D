#pragma once
class MenuManager : public Singleton<MenuManager>
{
private:
    friend class Singleton;

    MenuManager();
    ~MenuManager();
public:

    void Render();
    void PostRender();
    void GUIRender();

    void FirstLoading(Event event);

    bool IsPickGameMenu() { return game_select == 0 ? false : true; }
    UINT GetSelectGameMenu() { return game_select; }

    void GameMenuUpdate();
    bool IsPickGameExit();

    UINT GetLoadingSequence() { return loadingSequence; }
    void IncreaseLoadingSequence() { loadingSequence++; }
    void SetLoadingRate(float rate);
private:
    GameMenu* gameMenu;

    UINT game_select = 0;
    UINT menu_cursor = 1;

    Quad* gameStartScreen;
    Quad* gameEndScreen;

    bool loadingFlag = false;
    float loadingRate = 0.f;
    UINT loadingSequence = 0;
    ProgressBar* loadingBar;
};

