#pragma once
class Bow :public GameObject
{
private:
	struct SpecialKeyUI
	{
		string name;
		Quad* quad;
		bool active;
	};
	Model* bow;
	map<string, SpecialKeyUI> specialKeyUI;
	Transform* transform;

public:
	Model* GetModel() { return bow; }

	Vector3 GlobalPosition() { return bow->GlobalPos(); }
	Bow();
	~Bow();
	void SetTarget(Transform* _transform) { transform = _transform; }
	void Update();
	void Render();
	void GUIRender();
	
};

