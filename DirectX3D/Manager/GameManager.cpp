#include "Framework.h"

#include "Scenes/GameMapScene.h"
#include "Scenes/GameSmallMapScene.h"
#include "Scenes/TestNpcScene.h"
#include "Scenes/ModelAnimationScene.h"
#include "Scenes/BuildingScene.h"
#include "Scenes/ShadowScene.h"
#include "Scenes/OnlyBossScene.h"
#include "Scenes/EndingCreditScene.h"
#include "Scenes/TerrainScene.h"

bool GameManager::restartFlag = false;

GameManager::GameManager()
{
    Create();
    Start();
}

GameManager::~GameManager()
{
    Delete();
}

void GameManager::Update()
{
    Keyboard::Get()->Update();
    //Audio::Get()->Update();

    SceneManager::Get()->Update();
    Environment::Get()->Update();
    Timer::Get()->Update();
}

void GameManager::Render()
{
    SceneManager::Get()->PreRender();

    Device::Get()->Clear();
    Font::Get()->GetDC()->BeginDraw();

    Environment::Get()->Set();
    SceneManager::Get()->Render();

    Environment::Get()->PostSet();
    SceneManager::Get()->PostRender();

   ImGui_ImplDX11_NewFrame();
   ImGui_ImplWin32_NewFrame();
   ImGui::NewFrame();

    string fps = "FPS : " + to_string(Timer::Get()->GetFPS());
    Font::Get()->RenderText(fps, { 100, WIN_HEIGHT - 10 });

    static bool isActive = true;

    if (isActive)
    {
        ImGui::Begin("Inspector", &isActive);
        Environment::Get()->GUIRender();
        SceneManager::Get()->GUIRender();
        ImGui::End();
    }
 
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    Font::Get()->GetDC()->EndDraw();

    Device::Get()->Present();

    if (GameManager::restartFlag)
    {
        SceneManager::Delete();
        Environment::Delete();
        SceneManager::Get();
        Environment::Get();
        Start();
        GameManager::restartFlag = false;
    }
}

void GameManager::Start()
{
    //SceneManager::Get()->Create("ModelAnimation", new ModelAnimationScene());
    SceneManager::Get()->Create("GameMap", new GameMapScene());
    //SceneManager::Get()->Create("GameSmallMap", new GameSmallMapScene());
    //SceneManager::Get()->Create("TestNpcScene", new TestNpcScene());
    //SceneManager::Get()->Create("Building", new BuildingScene());
    //SceneManager::Get()->Create("Shadow", new ShadowScene());
    //SceneManager::Get()->Create("OnlyBoss", new OnlyBossScene());
//    SceneManager::Get()->Create("EndingCredit", new EndingCreditScene());
    //SceneManager::Get()->Create("Terrain", new TerrainScene());

    //SceneManager::Get()->Add("Grid");

    // 활 위치 130 0.5 190

    //SceneManager::Get()->Add("ModelAnimation");
    SceneManager::Get()->Add("GameMap");
    //SceneManager::Get()->Add("GameSmallMap");
    //SceneManager::Get()->Add("TestNpcScene");
    //SceneManager::Get()->Add("Building");
    //SceneManager::Get()->Add("Shadow");
    //SceneManager::Get()->Add("OnlyBoss");
   // SceneManager::Get()->Add("EndingCredit");
    //SceneManager::Get()->Add("Terrain");
}

void GameManager::Create()
{
    Keyboard::Get();
    Timer::Get();
    //Audio::Get();
    SoundManager::Get();
    Device::Get();
    Environment::Get();
    Observer::Get();

    Font::Get()->AddColor("White", 1, 1, 1);
    Font::Get()->AddStyle("Default", L"배달의민족 주아");

    Font::Get()->SetColor("White");
    Font::Get()->SetStyle("Default");

    Texture::Add(L"Textures/Color/White.png");

   ImGui::CreateContext();
   ImGui::StyleColorsDark();
 
   ImGui_ImplWin32_Init(hWnd);
   ImGui_ImplDX11_Init(DEVICE, DC);

    srand((unsigned int)time(NULL));

    VideoTexture::createAPI();
}

void GameManager::Delete()
{
    Keyboard::Delete();
    Timer::Delete();
    SceneManager::Delete();
    Device::Delete();
    Shader::Delete();
    SoundManager::Delete();
    Texture::Delete();
    Environment::Delete();
    Observer::Delete();
    Font::Delete();
    //Audio::Delete();

   ImGui_ImplDX11_Shutdown();
   ImGui_ImplWin32_Shutdown();
  

   ImGui::DestroyContext();
   VideoTexture::destroyAPI();
}
