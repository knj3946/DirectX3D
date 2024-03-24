#include "Framework.h"
#include "TerrainScene.h"

TerrainScene::TerrainScene()
{
	terrain = new TerrainEditor(256, 256);
	terrain->GetMaterial()->SetDiffuseMap(L"Textures/Landscape/Sand.png");
	terrain->GetMaterial()->SetSpecularMap(L"Textures/Color/Black.png");
	terrain->GetMaterial()->SetNormalMap(L"Textures/Landscape/Sand_Normal.png");
	terrain->SetHeightMap(L"Textures/HeightMaps/SamepleHeightMap04.png");
	terrain->GetMaterial()->SetShader(L"Light/Shadow.hlsl");

	FOR(6)
		HeightCollider[i] = new BoxCollider;

	HeightCollider[0]->Pos() = { 128.f,25.f,18.f };
	HeightCollider[0]->Scale() = { 256.f,50.f,30.f };
	HeightCollider[1]->Pos() = { 128.f,25.f, 246.5f };
	HeightCollider[1]->Scale() = { 256.f,50.f,30.f };
	HeightCollider[2]->Pos() = { 239.f,25.f,128.f };
	HeightCollider[2]->Scale() = { 30.f,50.f,256.f };
	HeightCollider[3]->Pos() = { 13.7f,25.f, 128.f };
	HeightCollider[3]->Scale() = { 30.f,50.f,256.f };
	HeightCollider[4]->Pos() = { 128.f,15.f,129.f };
	HeightCollider[4]->Rot() = { 0.f,XMConvertToRadians(21),0.f};
	HeightCollider[4]->Scale() = { 14.7f,22.4f,59.4f };
	HeightCollider[5]->Pos() = { 183.f,15.f, 152.7f };
	HeightCollider[5]->Rot() = { 0.f,0.f,0.f };
	HeightCollider[5]->Scale() = { 110.f,30.f,14.1f };

	FOR(6)
		HeightCollider[i]->UpdateWorld();

	

}

TerrainScene::~TerrainScene()
{
	delete terrain;
}

void TerrainScene::Update()
{
	terrain->Update();
}

void TerrainScene::PreRender()
{
}

void TerrainScene::Render()
{
	terrain->Render();
	FOR(6)
		HeightCollider[i]->Render();
}

void TerrainScene::PostRender()
{
}

void TerrainScene::GUIRender()
{
	terrain->GUIRender();
}
