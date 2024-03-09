#pragma once

class SoundManager : public Singleton<SoundManager>
{
	// key는 오브젝트명_행동 으로 정의
	// 각 단어의 첫 문자만 대문자로
	// 사운드파일도 key와 동일하게 맞출 것

	friend class Singleton;

public:
	SoundManager();
	~SoundManager();

	void Update();
	void GUIRender(); // 디버그를 위한 용도

	void OrcCreate(int id, Transform* t);
	void PlayerCreate(Transform* t);
	void BossCreate(Transform* t);

	// 매크로로 편하게 접근하게 하기
	Audio* GetOrcAudio(int id) { return orcAudios[id]; }
	Audio* GetBossAudio() { return bossAudio; }
	Audio* GetPlayerAudio() { return playerAudio; }
private:

	Audio* AudioCreate(Transform* t); // 오디오 만들 때 여기서 뭔가 더 세팅할게 있다면 이 함수로

private:
	map<int, Audio*> orcAudios; // 각 오크마다 오디오시스템을 가지고 있는다.
	Audio* playerAudio;
	Audio* bossAudio;
};

