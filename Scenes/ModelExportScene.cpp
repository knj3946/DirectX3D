#include "Framework.h"
#include "ModelExportScene.h"

ModelExportScene::ModelExportScene()
{
    string name = "akai";
    string file = "Models/FBX/" + name + ".fbx";
    ModelExporter* exporter;
    string clipName;

    //exporter = new ModelExporter(name, file);
    //exporter->ExportMaterial();
    //exporter->ExportMesh();
    //delete exporter;

    clipName = "temp";
    file = "Models/Animations/" + name + "/" + clipName + ".fbx";
    exporter = new ModelExporter(name, file);
    exporter->ExportClip(clipName);
    delete exporter;

    //string floderN1 = "Bow/Crouch";

    //clipName = "Standing To Crouch";
    //file = "Models/Animations/" + name + "/" + floderN1 + "/" + clipName + ".fbx";
    //exporter = new ModelExporter(name, file);
    //exporter->ExportClip(clipName);
    //delete exporter;
}

ModelExportScene::~ModelExportScene()
{
}

void ModelExportScene::Update()
{
    exit(1);
}

void ModelExportScene::PreRender()
{
}

void ModelExportScene::Render()
{
}

void ModelExportScene::PostRender()
{
}

void ModelExportScene::GUIRender()
{
}
