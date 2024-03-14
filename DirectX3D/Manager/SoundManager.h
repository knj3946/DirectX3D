#pragma once

class SoundManager : public Singleton<SoundManager>
{
	// key�� ������Ʈ��_�ൿ ���� ����
	// �� �ܾ��� ù ���ڸ� �빮�ڷ�
	// �������ϵ� key�� �����ϰ� ���� ��

	friend class Singleton;

	const int MAX_CHANNEL = 20;

public:
	SoundManager();
	~SoundManager();

	void Update();
	void GUIRender(); // ����׸� ���� �뵵

	void OrcCreate(int id, Transform* t);
	void PlayerCreate(Transform* t);
	void BossCreate(Transform* t);

	// ��ũ�η� ���ϰ� �����ϰ� �ϱ�
	Audio* GetOrcAudio(int id) { return orcAudios[id]; }
	Audio* GetBossAudio() { return bossAudio; }
	Audio* GetPlayerAudio() { return playerAudio; }

	System* GetSoundSystem() { return soundSystem; }

	float GetVolume() { return volume; }
private:

	Audio* AudioCreate(Transform* t); // ����� ���� �� ���⼭ ���� �� �����Ұ� �ִٸ� �� �Լ���

private:
	System* soundSystem;

	FMOD_VECTOR listenerPos;

	map<int, Audio*> orcAudios; // �� ��ũ���� ������ý����� ������ �ִ´�.
	Audio* playerAudio;
	Audio* bossAudio;

	float volume = 10.0f; // �ѹ��� ���� �����ϱ� ���� ���� -> 10�� �⺻�����ΰ� ������

};

