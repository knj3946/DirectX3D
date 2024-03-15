#include "Framework.h"
#include "Bow.h"

Bow::Bow()
{
	bow = new Model("Elven Long Bow");
	{
		SpecialKeyUI sk;
		Quad* quad = new Quad(Vector2(100, 50));
		quad->GetMaterial()->SetShader(L"Basic/Texture.hlsl");
		quad->GetMaterial()->SetDiffuseMap(L"Textures/UI/SpecialKeyUI_dropItem.png");
		sk.name = "getItem";
		sk.quad = quad;
		sk.active = false;
		specialKeyUI.insert(make_pair(sk.name, sk));
	}
	bow->Pos() = { 130.f,1.f,190.f };
	//bow->Scale() *= 6.f;
	bow->Scale() *= 0.15f;
	bow->Rot().x = XMConvertToDegrees(90);
	bow->UpdateWorld();
}

Bow::~Bow()
{
	delete bow;
	for (pair<string, SpecialKeyUI> key : specialKeyUI)
	{
		delete key.second.quad;
	}
}

void Bow::Update()
{
	if (bow->Active())
	bow->UpdateWorld();
}

void Bow::Render()
{
	if(bow->Active())
		bow->Render();
}

void Bow::GUIRender()
{
	if (bow->Active())
	bow->GUIRender();
}


