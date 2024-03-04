#include "Framework.h"
#include "AStarScene.h"

AStarScene::AStarScene()
{
    terrain = new TerrainEditor();
    aStar = new AStar(128, 128);
    aStar->SetNode(terrain); //지형에 맞추어서 노드 설치
    aStar2 = new AStar(128, 128);
    aStar2->SetNode(terrain);
    fox = new Fox();
    fox->SetTerrain(terrain);
    fox->SetAStar(aStar);

   
    robot = new RobotA();
    //robot->SetTerrain(terrain);
    robot->SetAStar(aStar2);
    robot->SetTarget(fox);

    FOR(3) {
        cube[i] = new Cube;
        cube[i]->SetTag(to_string(i));
        ColliderManager::Get()->PushCollision(ColliderManager::WALL, cube[i]->GetCollider());


    }
    cube[0]->Pos() = { 90.f,0.f,72.f };
    cube[0]->Scale() = { 20.f,20.f,1.f };
    cube[1]->Pos() = { 100.f,0.f,62.f };
    cube[1]->Scale() = { 1.f,20.f,20.f };
    cube[2]->Scale() = { 1.f,20.f,20.f };
    cube[2]->Pos() = { 80.f,0.f,62.f };

    FOR(3)
    {
        cube[i]->Update();

        aStar->AddObstacleObj(cube[i]->GetCollider());
        aStar2->AddObstacleObj(cube[i]->GetCollider());
    }

}

AStarScene::~AStarScene()
{
    delete terrain;
    delete aStar;
    delete aStar2;
    FOR(3)
        delete cube[i];
    delete fox;
    delete robot;
}

void AStarScene::Update()
{
    FOR(3)
        cube[i]->Update();
    aStar->Update();
    aStar2->Update();;
    fox->Update();
    robot->Update();
}

void AStarScene::PreRender()
{
}

void AStarScene::Render()
{
    terrain->Render();
  
   
    fox->Render();
    robot->Render();
    FOR(3)
        cube[i]->Render();

}

void AStarScene::PostRender()
{
}

void AStarScene::GUIRender()
{
}
