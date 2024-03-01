#include "Framework.h"
#include "Sprite.h"

Sprite::Sprite(wstring imageFile, float width, float height, UINT frameCol, UINT frameRow, bool isAdditive)
	:size(width, height)
{
	material->SetShader(L"Geometry/Sprite.hlsl");
	material->SetDiffuseMap(imageFile);

	geometryShader = Shader::AddGS(L"Geometry/Sprite.hlsl");

	buffer = new SpriteBuffer();
	buffer->Get().maxFrame = { (float)frameCol,(float)frameRow }; // ������ �ִ� ����
										 //������ ���ۿ��� ���� (+ �ִ� ������ ����)
	maxFrameCount = frameCol * frameRow; //������ �ִ� ���� (Ŭ���� ���� ��꿡 ���� ����)

	if (isAdditive) blendState[1]->Additive();

	Create();

}

Sprite::~Sprite()
{
	delete buffer;
}


// 1 2 3 4 5
// 6 7 8 9 10
// 11 12 13 14 15
void Sprite::Update()
{
	if (!isActive) return; // ��Ȱ��ȭ�� ����

	time += speed * DELTA; // ����ð� * ����ӵ���ŭ ��������ð� ����

	if (time > interval) //0.1f : ���⼭ ���� ������ ������ ����. �ʿ��ϸ� �����̳� ���� �Ҵ�
	{
		curFrameCount++; // ������+1
		buffer->Get().curFrame.x = curFrameCount % (UINT)buffer->Get().maxFrame.x;
		buffer->Get().curFrame.y = curFrameCount / (UINT)buffer->Get().maxFrame.x;
		time = 0; //����ð� ����
	}

	if (curFrameCount >= maxFrameCount) Stop(); // ����
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
	isActive = true; //��� ����

	time = 0; //��� �ð� ����
	curFrameCount = 0; // ���� ����� ������ ����

	vertex.pos = pos; //��ġ�� ���� �� 
	vertex.uv = size; //�ؽ�ó�� ũ�� = Ŭ������ ���� ���

	vertexBuffer->Update(&vertex, particleCount);
}

void Sprite::Create()
{
	//��������Ʈ ��ƼŬ�� �ؽ�ó �غ� ���ָ� ��
	particleCount = 1;	//��ƼŬ ��� ���� = 1
	vertexBuffer = new VertexBuffer(&vertex,sizeof(VertexUV),1); // ���� �뷮�� 1
}