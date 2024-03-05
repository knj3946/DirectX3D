#pragma once
class Rain : public Particle
{
public:
	Rain();
	~Rain();

	void Update();
	void Render();
	void GUIRender();

private:
	void Create();

private:

	//FloatValueBuffer* buffer; //����� ǥ��
	//VertexBuffer* vertexBuffer; // ����� ��ġ
	//->�� ������ ��� �����Ϸ��� �� �������� �� �ְ�
	// �ܼ��ϰ� ������� ������ �� �ܼ��ϰԵ� ���� �� �ִ�
	WeatherBuffer* buffer; // ����+�Ҽ� ����, ���� ȿ�� �����
						   // ���� ���۴� �� ���� �ٸ� ���� ��ƼŬ�� �� �� �־ �������� �ۼ���

	vector<VertexUV> vertices;

};

