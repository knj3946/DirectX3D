#pragma once

class GameManager
{
public:
    GameManager();
    ~GameManager();

    void Update();
    void Render();
    void Start();

    static void ReserveRestart() { restartFlag = true; }
private:
    void Create();
    void Delete();

    static bool restartFlag;
};