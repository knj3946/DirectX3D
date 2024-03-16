#include "Framework.h"
#include "Sprite.h"

Sprite::Sprite(wstring imageFile, float width, float height, UINT frameCol, UINT frameRow, bool isAdditive)
	:size(width, height)
{
	material->SetShader(L"Geometry/Sprite.hlsl");
	material->SetDiffuseMap(imageFile);

	geometryShader = Shader::AddGS(L"Geometry/Sprite.hlsl");

	buffer = new SpriteBuffer();
	buffer->Get().maxFrame = { (float)frameCol,(float)frameRow }; // 프레임 최대 개수
										 //윗줄은 버퍼에서 설정 (+ 최대 프레임 범위)
	maxFrameCount = frameCol * frameRow; //프레임 최대 개수 (클래스 내부 계산에 쓰기 위함)

	if (isAdditive) blendState[1]->Additive();

	Create();

}

Sprite::~Sprite()
{
	SAFE_DELETE(buffer);

}


// 1 2 3 4 5
// 6 7 8 9 10
// 11 12 13 14 15
void Sprite::Update()
{
	if (!isActive) return; // 비활성화시 종료

	time += speed * DELTA; // 경과시간 * 재생속도만큼 누적재생시간 증가

	if (time > interval) //0.1f : 여기서 정한 임의의 프레임 간격. 필요하면 수정이나 변수 할당
	{
		curFrameCount++; // 프레임+1
		buffer->Get().curFrame.x = curFrameCount % (UINT)buffer->Get().maxFrame.x;
		buffer->Get().curFrame.y = curFrameCount / (UINT)buffer->Get().maxFrame.x;
		time = 0; //재생시간 리셋
	}

	if (curFrameCount >= maxFrameCount) Stop(); // 정지
}

void Sprite::Render()
{

	buffer->SetPS(10);
	Particle::Render();
}

void Sprite::GUIRender()
{
}

void Sprite::Play(Vector3 pos)
{
	isActive = true; //재생 시작

	time = 0; //재생 시간 리셋
	curFrameCount = 0; // 현재 진행된 프레임 리셋

	vertex.pos = pos; //위치를 받은 후 
	vertex.uv = size; //텍스처의 크기 = 클래스가 받은 대로

	vertexBuffer->Update(&vertex, particleCount);
}

void Sprite::Create()
{
	//스프라이트 파티클은 텍스처 준비만 해주면 끝
	particleCount = 1;	//파티클 재생 개수 = 1
	vertexBuffer = new VertexBuffer(&vertex,sizeof(VertexUV),1); // 버퍼 용량도 1
}
