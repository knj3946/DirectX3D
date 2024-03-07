#pragma once

class SoundManager : public Singleton<SoundManager>
{
	// key�� ������Ʈ��_�ൿ ���� ����
	// �� �ܾ��� ù ���ڸ� �빮�ڷ�
	// �������ϵ� key�� �����ϰ� ���� ��

	enum KeyType
	{
		Player,
		Orc,
		Boss,
		Bgm,
		None
	};

	enum BeHaviorType
	{
		Move,
		//None,
	};

	friend class Singleton;

public:
	SoundManager();
	~SoundManager();

	//string GetKey(KeyType type1, BeHaviorType type2) { return soundKey[type1][type2]; }

private:
	void Add(string key);

private:
	//string soundKey[KeyType::None][BeHaviorType::None];
};

