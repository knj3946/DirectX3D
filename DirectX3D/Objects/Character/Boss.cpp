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


	exclamationMark = new Quad(L"Textures/UI/Exclamationmark.png");
	questionMark = new Quad(L"Textures/UI/QuestionMark.png");
	exclamationMark->Scale() *= 0.1f;
	questionMark->Scale() *= 0.1f;
}

Boss::~Boss()
{
	delete leftHand;
	delete leftCollider;
	delete collider;
	delete exclamationMark;
	delete questionMark;
	delete rangeBar;
	delete hpBar;
}

void Boss::Render()
{
	ModelAnimator::Render();
	leftCollider->Render();
	collider->Render();
}

void Boss::Update()
{

	IDLE();
	Direction();
	Move();
	Attack();
	Die();
	Control();

	


	ModelAnimator::Update();
	leftHand->SetWorld(GetTransformByNode(14));
	leftCollider->UpdateWorld();
	collider->UpdateWorld();
}

void Boss::GUIRender() {

}

void Boss::CalculateEyeSight()
{
}

void Boss::CalculateEarSight()
{
}

void Boss::AddObstacleObj(Collider* collider)
{
}

void Boss::IDLE()
{
}

void Boss::Direction()
{
}

void Boss::Move()
{
}

void Boss::Attack()
{
}

void Boss::JumpAttack()
{
}

void Boss::Die()
{
}

void Boss::Control()
{
}

void Boss::UpdateUI()
{
}

void Boss::SetPath(Vector3 targetPos)
{
}

void Boss::ExecuteEvent()
{
}

void Boss::EndAttack()
{
}

void Boss::EndHit()
{
}

void Boss::EndDying()
{
}

void Boss::EndJumpAttack()
{
}
