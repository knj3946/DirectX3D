#pragma once
class ArrowManager : public Singleton<ArrowManager>
{
private:
	friend class Singleton;
	UINT SIZE = 10; // 동시에 출력 가능한 쿠나이 최대 개수

public:
	ArrowManager();
	~ArrowManager();

	void Update();
	void Render();

	void Throw(Vector3 pos, Vector3 dir); // 개별 객체에게 전달하기 위한 매개 함수

	bool IsCollision(); //각 쿠나이가 다른 물체와 충돌했는지 판별하기 위함

	void OnOutLineByRay(Ray ray);

	ModelInstancing* GetInstancing() { return arrowInstancing; }

private:
	// 쿠나이 출력용 모델
	ModelInstancing* arrowInstancing; // 쿠나이 모델
	vector<Arrow*> arrows;
	vector<Collider*> wallColiders;
	int count = 0;
};

