#include "Framework.h"

#include "Scenes/CubeScene.h"
#include "Scenes/GridScene.h"
#include "Scenes/SphereScene.h"
#include "Scenes/TerrainEditorScene.h"
#include "Scenes/CollisionScene.h"
#include "Scenes/ModelExportScene.h"
#include "Scenes/ModelRenderScene.h"
#include "Scenes/ModelAnimationScene.h"
#include "Scenes/IntancingScene.h"
#include "Scenes/GameScene.h"
#include "Scenes/BillboardScene.h"
#include "Scenes/RenderTargetScene.h"
#include "Scenes/ShadowScene.h"
#include "Scenes/DijkstraScene.h"
#include "Scenes/AStarScene.h"
#include "Scenes/ParticleScene.h"
#include "Scenes/ParticleConfigScene.h"
#include "Scenes/HumanScene.h"
#include "Scenes/QuadTreeScene.h"
#include "Scenes/GameMapScene.h"
#include "Scenes/TestNpcScene.h"
#include "Scenes/PlayerScene.h"
#include "Scenes/PlayerMapScene.h"
#include "Scenes/EyesRaySearchScene.h"
#include "Scenes/BuildingScene.h"

GameManager::GameManager()
{
    Create();

    SceneManager::Get()->Create("Grid", new GridScene());

    //SceneManager::Get()->Create("ModelExport", new ModelExportScene());
    //SceneManager::Get()->Create("Cube", new CubeScene());
    //SceneManager::Get()->Create("Sphere", new SphereScene());
  //  SceneManager::Get()->Create("Terrain", new TerrainEditorScene());
    //SceneManager::Get()->Create("Collision", new CollisionScene());
    //SceneManager::Get()->Create("ModelRender", new ModelRenderScene());
    //SceneManager::Get()->Create("ModelAnimation", new ModelAnimationScene());
    //SceneManager::Get()->Create("Instancing", new IntancingScene());
    //SceneManager::Get()->Create("Game", new GameScene());
    ///SceneManager::Get()->Create("BillboardScene", new BillboardScene());
    //SceneManager::Get()->Create("RenderTarget", new RenderTargetScene());
    //SceneManager::Get()->Create("ShadowScene", new ShadowScene());
    //SceneManager::Get()->Create("Dijkstra", new DijkstraScene());
    //SceneManager::Get()->Create("AStar", new AStarScene());
    //SceneManager::Get()->Create("Particle", new ParticleScene());
    //SceneManager::Get()->Create("ParticleConfig", new ParticleConfigScene());
    //SceneManager::Get()->Create("Human", new HumanScene());
   // SceneManager::Get()->Create("QuadTree", new QuadTreeScene());
//    SceneManager::Get()->Create("GameMap", new GameMapScene());
   // SceneManager::Get()->Create("TestNpcScene", new TestNpcScene());
    //SceneManager::Get()->Create("Player", new PlayerScene());
    SceneManager::Get()->Create("PlayerMap", new PlayerMapScene());
    //SceneManager::Get()->Create("EyesRaySearch", new EyesRaySearchScene());
    //SceneManager::Get()->Create("Building", new BuildingScene());

    SceneManager::Get()->Add("Grid");

    //SceneManager::Get()->Add("ModelExport");
    //SceneManager::Get()->Add("Cube");
    //SceneManager::Get()->Add("Sphere");
   //  SceneManager::Get()->Add("Terrain");
    //SceneManager::Get()->Add("Collision");
    //SceneManager::Get()->Add("ModelRender");
    //SceneManager::Get()->Add("ModelAnimation");
    //SceneManager::Get()->Add("Instancing");
    //SceneManager::Get()->Add("Game");
    //SceneManager::Get()->Add("BillboardScene");
    //SceneManager::Get()->Add("RenderTarget");
    //SceneManager::Get()->Add("ShadowScene");
    //SceneManager::Get()->Add("Dijkstra");
    //SceneManager::Get()->Add("AStar");
    //SceneManager::Get()->Add("Particle");
    //SceneManager::Get()->Add("ParticleConfig");
    //SceneManager::Get()->Add("Human");
    //SceneManager::Get()->Add("QuadTree");
  //  SceneManager::Get()->Add("GameMap");
   // SceneManager::Get()->Add("TestNpcScene");
  //  SceneManager::Get()->Add("Player");
    SceneManager::Get()->Add("PlayerMap");
    //SceneManager::Get()->Add("EyesRaySearch");
    //SceneManager::Get()->Add("Building");
}

GameManager::~GameManager()
{
    Delete();
}

void GameManager::Update()
{
    Keyboard::Get()->Update();
    Timer::Get()->Update();
    Audio::Get()->Update();

    SceneManager::Get()->Update();
    Environment::Get()->Update();
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
}

void GameManager::Create()
{
    Keyboard::Get();
    Timer::Get();
    Audio::Get();
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
}

void GameManager::Delete()
{
    Keyboard::Delete();
    Timer::Delete();
    Device::Delete();
    Shader::Delete();
    Texture::Delete();
    Environment::Delete();
    Observer::Delete();
    Font::Delete();
    Audio::Delete();

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();
}
