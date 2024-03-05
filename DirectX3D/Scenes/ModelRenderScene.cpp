#include "Framework.h"
#include "ModelRenderScene.h"

ModelRenderScene::ModelRenderScene()
{
    model = new Model("arrow");
}

ModelRenderScene::~ModelRenderScene()
{
    delete model;
}

void ModelRenderScene::Update()
{
    model->UpdateWorld();
}

void ModelRenderScene::PreRender()
{
}

void ModelRenderScene::Render()
{
    model->Render();
}

void ModelRenderScene::PostRender()
{
}

void ModelRenderScene::GUIRender()
{
    model->GUIRender();

    ImGui::DragFloat3("model", (float*)&model->Rot(), 0.1f);
}
