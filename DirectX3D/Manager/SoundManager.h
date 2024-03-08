#pragma once

class SoundManager : public Singleton<SoundManager>
{
	// key는 오브젝트명_행동 으로 정의
	// 각 단어의 첫 문자만 대문자로
	// 사운드파일도 key와 동일하게 맞출 것

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

