#pragma once
class ParticleScene : public Scene
{
public:
	ParticleScene();
	~ParticleScene();

	// Scene을(를) 통해 상속됨
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual void GUIRender() override;

private:
	//Particle* particle;
	ParticleSystem* particle;

	SphereCollider* collider;

};

