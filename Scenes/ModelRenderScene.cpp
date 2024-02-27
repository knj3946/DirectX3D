#include "Framework.h"
#include "ModelRenderScene.h"

ModelRenderScene::ModelRenderScene()
{
    model = new Model("akai");
    model->Scale() *= 0.3f;
    bow = new Model("hungarian_bow");
    bow->Scale() *= 0.08f;

    leftHand = new Transform();
    bow->SetParent(leftHand);
}

ModelRenderScene::~ModelRenderScene()
{
    delete model;
    delete bow;
}

void ModelRenderScene::Update()
{
    model->UpdateWorld();

    //leftHand->SetWorld(model->GetNodeMatrix(leftHandNode));
    bow->UpdateWorld();
    bow->Pos() = 0;
}

void ModelRenderScene::PreRender()
{
}

void ModelRenderScene::Render()
{
    model->Render();
    bow->Render();
}

void ModelRenderScene::PostRender()
{
}

void ModelRenderScene::GUIRender()
{
    model->GUIRender();
    bow->GUIRender();

    ImGui::InputInt("leftHandNode", &leftHandNode);
}
