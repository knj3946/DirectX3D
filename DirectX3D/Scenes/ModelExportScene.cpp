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

        clipName = "Jump Attack";
        file = "Models/Animations/" + name + "/" + clipName + ".fbx";
        exporter = new ModelExporter(name, file);
        exporter->ExportClip(clipName);
        delete exporter;
        clipName = "Falling Forward Death";
        file = "Models/Animations/" + name + "/" + clipName + ".fbx";
        exporter = new ModelExporter(name, file);
        exporter->ExportClip(clipName);
        delete exporter;
        clipName = "Idle";
        file = "Models/Animations/" + name + "/" + clipName + ".fbx";
        exporter = new ModelExporter(name, file);
        exporter->ExportClip(clipName);
        delete exporter;
        clipName = "Mutant Roaring";
        file = "Models/Animations/" + name + "/" + clipName + ".fbx";
        exporter = new ModelExporter(name, file);
        exporter->ExportClip(clipName);
        delete exporter;
        clipName = "Run Forward";
        file = "Models/Animations/" + name + "/" + clipName + ".fbx";
        exporter = new ModelExporter(name, file);
        exporter->ExportClip(clipName);
        delete exporter;
        clipName = "Mutant Swiping";
        file = "Models/Animations/" + name + "/" + clipName + ".fbx";
        exporter = new ModelExporter(name, file);
        exporter->ExportClip(clipName);
        delete exporter;
        clipName = "Walking";
        file = "Models/Animations/" + name + "/" + clipName + ".fbx";
        exporter = new ModelExporter(name, file);
        exporter->ExportClip(clipName);
        delete exporter;

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
