#include "Framework.h"
#include "AStarScene.h"

AStarScene::AStarScene()
{
    terrain = new Terrain();
    aStar = new AStar(20, 20);
    aStar->SetNode(terrain); //지형에 맞추어서 노드 설치

    fox = new Fox();
    fox->SetTerrain(terrain);
    fox->SetAStar(aStar);

    aStar2 = new AStar(20, 20);
    aStar2->SetNode(terrain);

    robot = new RobotA();
    robot->SetTerrain(terrain);
    robot->SetAStar(aStar2);
    robot->SetTarget(fox);
}

AStarScene::~AStarScene()
{
    delete terrain;
    delete aStar;
    delete aStar2;
    delete fox;
    delete robot;
}

void AStarScene::Update()
{
    aStar2->Update();
    aStar->Update();
    fox->Update();
    robot->Update();
}

void AStarScene::PreRender()
{
}

void AStarScene::Render()
{
    terrain->Render();
    //aStar->Render();
    aStar2->Render();
    fox->Render();
    robot->Render();
}

void AStarScene::PostRender()
{
}

void AStarScene::GUIRender()
{
}
