#pragma once

class Arrow
{
	// 투사체 하나를 총괄하는 클래스

private:
	// 열람시 필요한 주요 데이터
	float LIFE_SPAN = 2.f; // life span : 생성 후 비활성화까지 걸리는 시간



public:
	Arrow(Transform* transform,int id,bool isDropItem=false); // 인스턴싱을 전제한 트랜스폼 매개 생성
	~Arrow();

	void Update();
	void Render();

	void Throw(Vector3 pos, Vector3 dir);

	SphereCollider* GetCollider() { return collider; }
	Transform* GetTransform() { return transform; }

	float GetDamage() { return damage; }

	class Trail* GetTrail() { return trail; }

	void SetOutLine(bool flag);
	bool IsOutLine() { return outLine; };
private:



private:
	Transform* transform; // 인스턴싱을 전제로 하기 때문에 transform을 변수로 받음
	SphereCollider* collider;

	float speed = 30;
	float time = 0; // 생성된 시간
	float damage = 30; // 임시 데미지값 설정

	Vector3 direction;

	class Trail* trail; // 궤적 파티클
	Transform* startEdge; // 궤적이 시작될 곳
	Transform* endEdge;

	bool outLine = false;
	int index;
	bool isDropItem;
};

