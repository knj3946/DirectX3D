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
    UINT GetSelectFailMenu() { return fail_select; }
    void SetSelectFailMenu(UINT select) { this->fail_select = select; }

    void GameMenuUpdate();
    void FailMenuUpdate();
    bool IsPickGameExit();

    UINT GetLoadingSequence() { return loadingSequence; }
    void IncreaseLoadingSequence() { loadingSequence++; }
    void SetLoadingRate(float rate);
    UINT Getsequence() { return loadingSequence; }

    void SetFailFlag(bool flag) { this->failFlag = flag; }
    void SetEndFlag(bool flag) { this->endFlag = flag; }

    bool GetFailFlag() { return failFlag; }
    bool GetEndFlag() { return endFlag; }
private:
    GameMenu* gameMenu;
    GameMenu* failMenu;

    UINT game_select = 0;
    UINT menu_cursor = 1;

    UINT fail_select = 0;
    UINT fail_cursor = 1;

    Quad* gameStartScreen;
    Quad* gameFailScreen;
    Quad* gameEndScreen;

    bool loadingFlag = false;
    float loadingRate = 0.f;
    UINT loadingSequence = 0;
    ProgressBar* loadingBar;

    bool failFlag = false;
    bool endFlag = false;
};

