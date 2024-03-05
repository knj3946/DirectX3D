#pragma once

class Trail : public GameObject
{
	// 궤적파티클 

public:
	Trail(wstring imageFile, Transform* start, Transform* end, UINT width, float speed);
	~Trail();

	void Init();

	void Update();
	void Render();


private:
	void CreateMesh(); //만들어진 이미지를 위한 메쉬가 필요해서

private:

	Mesh<VertexUV>* mesh; // 궤적의 (기본) 형태

	Transform* start; // 궤적이 시작되는 곳
	Transform* end;
	//Transform* offset, * origin; 궤적의 중간 경유 연산지가 들어갈 수 있지만 여기서는 생략

	UINT width; // 궤적에 들어가는 단위 파티클의 그림 내 이미지 가로 크기
	float speed; // 궤적의 진행, 재생 속도

	// 이미지 출력을 위한 상태 설정
	RasterizerState* rasterizerState[2];
	BlendState* blendState[2];

};

