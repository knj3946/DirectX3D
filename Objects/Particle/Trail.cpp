#include "Framework.h"
#include "Trail.h"

Trail::Trail(wstring imageFile, Transform* start, Transform* end, UINT width, float speed)
	:start(start), end(end), width(width), speed(speed)
{
	material->SetShader(L"Basic/Texture.hlsl"); // �⺻�� �о �ؽ�ó�� ���ư�
												// ->������ �ɼ� �� Ȱ�� �Ұ�
												// 

	material->SetDiffuseMap(imageFile); // 

	// �޽������
	CreateMesh();

	//��»��¼���
	FOR(2) rasterizerState[i] = new RasterizerState();
	FOR(2) blendState[i] = new BlendState();

	rasterizerState[1]->CullMode(D3D11_CULL_NONE); // ���̵�, �ڵ� ��� ����ϱ�
	//blendState[1]->Alpha(true); // ����� ����
	blendState[1]->Additive();
}

Trail::~Trail()
{
	delete mesh;
	FOR(2) delete rasterizerState[i];
	FOR(2) delete blendState[i];

	// start, end�� �ٸ� ������ ���� ���� �����Ƿ�, ���⼭�� ���� �� ��
}

void Trail::Init()
{
	vector<VertexUV>& vertices = mesh->GetVertices();
	for (UINT i = 0; i <= width; i++)
	{
		vertices[i * 2 + 0].pos = start->Pos();
		vertices[i * 2 + 1].pos = end->Pos();
	}
}

void Trail::Update()
{
	if (!Active())return;
	// �� �ڵ带 �ּ��ϸ� ������ �Ⱥ��̴� ���̴��� �׻� ���� ������ ������
	// ����ȭ ������ �������� ������ �׻� �𵨰� ��ġ�ϴ� ȿ���� ��´�.

	// ���� ���� ( �޽����� ���� �Լ��� )
	vector<VertexUV>& vertices = mesh->GetVertices();


	// �� ������ ��ġ(pos) �籸��
	for (UINT i = 0; i <= width; i++)
	{
		// ����, �� ������ ���� �� ���ϱ�
		Vector3 startPos = vertices[i * 2 + 0].pos;
		Vector3 endPos = vertices[i * 2 + 1].pos;

		//�� ������ ���� �� �� �غ��ϱ�
		Vector3 startDestPos;
		Vector3 endDestPos;

		if (i == 0)//�̹��� ���� ������ ��
		{
			startDestPos = start->GlobalPos(); // ������ ��ġ �� ��ü
			endDestPos = end->GlobalPos();
		}
		else //�̹����� ���� ���� �� ����� ��
		{
			startDestPos = vertices[(i - 1) * 2 + 0].pos; // �� ���� ������ ��ġ
															// ������ ���� ���� ����
															// ��ݱ��� �־��� ��

			endDestPos = vertices[(i - 1) * 2 + 1].pos;
		}

		// ����� �������� ���� �� ��ġ�� �ð� ���(+�ӵ�)�� ���� ���� ����
		startPos = Lerp(startPos, startDestPos, speed * DELTA);
		endPos = Lerp(endPos, endDestPos, speed * DELTA);

		// ����� ��� ������ �ǵ��
		vertices[i * 2 + 0].pos = startPos;
		vertices[i * 2 + 1].pos = endPos;
	}
	// �̷��� �ݺ����� ������
	// 1. ���� ���� �������� ������ ����, �� Ʈ�������� ���� ���̴�
	// 2. �� ���� �������� ��� �ڱ⺸�� �� ĭ ���� ������ ��ݱ��� �־��� ���� ���� ���̴�.

	// -> �� ������ �ݺ��Ǹ� ������ �̾����� ������ �׷��� ���̴�. 

	mesh->UpdateVertex(); // �޽��� �ٲ� ���� �����͸� ������Ʈ
}

void Trail::Render()
{
	if (!Active()) return;

	// �޽��� �־ ������ �޽� ���� Ŭ����(=ť��, ���� ��)�� ���
	// �ش� ���� ��ü�� ���� ������ �ٽ� ���� ���ľ� �Ѵ�

	worldBuffer->SetVS(0); // ������ �������� (Ʈ�������� ��������) ������ ������ 
							// ���� ���̴����� �����Ѵ� (���� �⺻ ������)

	// ������ ���� �ȿ��� ������ �� �̹��� �����͸� ����
	material->Set();

	// ����� �غ�(���� ����)
	rasterizerState[1]->SetState();
	blendState[1]->SetState();

	//�޽��� �׸���(����)
	mesh->Draw(); // Draw �ؿ� DrawInstanced�� �ܺ� �����͸� �����ͼ� �׸� �� ���


	// ���� ���󺹱�
	rasterizerState[0]->SetState();
	blendState[0]->SetState();

	// �� �������� Ʈ���������� ������ ���� ���� ��, ������ ���� �̹��� �����Ͱ� ���
}

void Trail::CreateMesh()
{
	mesh = new Mesh<VertexUV>(); // ���� ���°� ���� �޽� �غ��ϱ�

	vector<VertexUV>& vertices = mesh->GetVertices(); // ���� �޾ƿ���

	vertices.reserve((width + 1) * 2); // +1 : �̹��� �ȼ��� ���۰� ���� ��� �����ϱ� ����
										// +2 : �� ����Ʈ ������ �������� ������ �ֱ�����

	for (UINT i = 0; i <= width; i++)//
	{
		// ���� �غ� �� ���Ϳ� �ֱ�
		VertexUV vertex;

		vertex.uv = { (float)i / width,0 }; // �߸� ������ u, ���� v�� �ϴ� UV���� ��
		vertices.push_back(vertex);			// ������ ���Ϳ� �߰�

		vertex.uv = { (float)i / width,1 };	// �߸� ������ u, �Ʒ��� v�� �ϴ� UV�� ����
		vertices.push_back(vertex);			// �ٲ� ������ ������ ���Ϳ� �� �߰�

		// -> �̶��� �ݺ��ϸ� �ȼ��� ���� 0���� ������ ������������ �ݺ��ϸ鼭 
		// �ؽ�ó�� UV���� ���Ϳ� ���� �� �ְ� �ȴ� (�ݺ����� ������ �׸� �� ��)
	}

	// �ε��� ���� �ޱ�
	vector<UINT>& indices = mesh->GetIndices();
	indices.reserve(width * 6);

	for (int i = 0; i < width; i++) // ���� ������ �Ȱ��� ����ϰ� 
	{
		indices.push_back((i * 2) + 0); // 0 : ���� �ε��� �ֱ�
		indices.push_back((i * 2) + 2); // 2
		indices.push_back((i * 2) + 1); // 1

		indices.push_back((i * 2) + 1); // 1
		indices.push_back((i * 2) + 2); // 2
		indices.push_back((i * 2) + 3); // 3

		// 0 - 2 - 1, 1 - 2 - 3 : �޸� �ؽ�ó �׸��� ����
		// * ������ ���� �ȵ����� ������ �ø���� �� �յ� ���о��� ��� ����̴ϱ� ����

	}
	//������� ���� ������, ���� ��Ͽ� ���� ���� �غ� �Ϸ�

	// �����͸� �޽��� ����
	mesh->CreateMesh();

}