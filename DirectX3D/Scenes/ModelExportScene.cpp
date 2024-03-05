#include "Framework.h"
#include "ModelExportScene.h"

ModelExportScene::ModelExportScene()
{
    string name = "Warrok W Kurniawan";
    string file = "Models/FBX/" + name + ".fbx";
    ModelExporter* exporter;
    string clipName;

    exporter = new ModelExporter(name, file);
    exporter->ExportMaterial();
    exporter->ExportMesh();
    delete exporter;

    string floderN1 = "Bow/Crouch";

    //clipName = "Standing To Crouch";
    //file = "Models/Animations/" + name + "/" + floderN1 + "/" + clipName + ".fbx";
    //exporter = new ModelExporter(name, file);
    //exporter->ExportClip(clipName);
    //delete exporter;


    //clipName = "Crouch To Standing";
    //file = "Models/Animations/" + name + "/" + floderN1 + "/" + clipName + ".fbx";
    //exporter = new ModelExporter(name, file);
    //exporter->ExportClip(clipName);
    //delete exporter;

    //clipName = "Crouch Idle";
    //file = "Models/Animations/" + name + "/" + floderN1 + "/" + clipName + ".fbx";
    //exporter = new ModelExporter(name, file);
    //exporter->ExportClip(clipName);
    //delete exporter;

    //clipName = "Crouch Walk Forward";
    //file = "Models/Animations/" + name + "/" + floderN1 + "/" + clipName + ".fbx";
    //exporter = new ModelExporter(name, file);
    //exporter->ExportClip(clipName);
    //delete exporter;


    //clipName = "Crouch Walk Back";
    //file = "Models/Animations/" + name + "/" + floderN1 + "/" + clipName + ".fbx";
    //exporter = new ModelExporter(name, file);
    //exporter->ExportClip(clipName);
    //delete exporter;

    clipName = "Crouch Walk Right";
    file = "Models/Animations/" + name + "/" + floderN1 + "/" + clipName + ".fbx";
    exporter = new ModelExporter(name, file);
    exporter->ExportClip(clipName);
    delete exporter;

    //clipName = "Crouch Walk Left";
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
