#pragma once
class GameControlManager : public Singleton<GameControlManager>
{
private:
    friend class Singleton;

    GameControlManager();
    ~GameControlManager();

public:

    void Render();
    void PostRender();
    void GUIRender();

    bool PauseGame() { return isPauseGame; }
    void SetPauseGame(bool b) { isPauseGame = b;}

private:
    bool isPauseGame = false;
};

