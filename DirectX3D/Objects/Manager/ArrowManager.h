#pragma once
class ArrowManager : public Singleton<ArrowManager>
{
private:
	friend class Singleton;
	UINT SIZE = 10; // 동시에 출력 가능한 쿠나이 최대 개수

	struct SpecialKeyUI
	{
		string name;
		Quad* quad;
		bool active;
	};

public:
	ArrowManager();
	~ArrowManager();

	void Update();
	void Render();
	void PostRender();

	void GUIRender();

	void Throw(Vector3 pos, Vector3 dir); // 개별 객체에게 전달하기 위한 매개 함수
	void Throw(Vector3 pos, Vector3 dir, float charingT);

	bool IsCollision(); //각 쿠나이가 다른 물체와 충돌했는지 판별하기 위함

	void OnOutLineByRay(Ray ray);
	void ActiveSpecialKey(Vector3 playPos, Vector3 offset,bool _btrue);
	void ExecuteSpecialKey();
	void SetActiveSpecialKey(bool active){ specialKeyUI["getItem"].active = active; }


	ModelInstancing* GetInstancing() { return arrowInstancing; }

	int GetCount() { return count; }
	int GetPlayerArrowCount() { return playerArrowCount; }
	void SetBowTransform(Transform* _transform) { bow = _transform; }
	void SetStart() { bStart = !bStart; }
private:
	bool bStart = false;
	// 쿠나이 출력용 모델
	ModelInstancing* arrowInstancing; // 쿠나이 모델
	vector<Arrow*> arrows;
	vector<Collider*> wallColiders;
	int count = 0; // 인덱스를 위함
	Transform* bow;
	int playerArrowCount = 10; // 처음에 3개 가지고 시작

	map<string, SpecialKeyUI> specialKeyUI;
};

