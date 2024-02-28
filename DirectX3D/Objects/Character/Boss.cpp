#include "Framework.h"

Boss::Boss():ModelAnimator("Warrok W Kurniawan")
{
	collider = new CapsuleCollider(50, 100);
	collider->SetParent(this);
	collider->Pos().y += 100;
	leftHand = new Transform;
	leftCollider = new CapsuleCollider(10, 10);
	leftCollider->Scale() *= 2.2f;
	

	leftCollider->SetParent(leftHand);
	ReadClip("Idle");
	ReadClip("Run Forward");
	ReadClip("Mutant Swiping");
	ReadClip("Mutant Roaring");
	ReadClip("Jump Attack");
	ReadClip("Falling Forward Death");
	Scale() *= 0.03f;

}

Boss::~Boss()
{
	delete leftHand;
	delete leftCollider;
	delete collider;
}

void Boss::Render()
{
	ModelAnimator::Render();
	leftCollider->Render();
	collider->Render();
}

void Boss::Update()
{
	if (KEY_DOWN('I')) {
		count++;	
	}
	ModelAnimator::Update();
	leftHand->SetWorld(GetTransformByNode(14));
	leftCollider->UpdateWorld();
	collider->UpdateWorld();
}

void Boss::GUIRender() {

}
