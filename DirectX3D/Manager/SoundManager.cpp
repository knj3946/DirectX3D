#include "Framework.h"

SoundManager::SoundManager()
{
	// key�� ������Ʈ��_�ൿ ���� ����
	// �� �ܾ��� ù ���ڸ� �빮�ڷ�

	// ���⼭ ��� ������ҽ� �߰�

	System_Create(&soundSystem);
	soundSystem->init(MAX_CHANNEL, FMOD_INIT_NORMAL, nullptr);

	soundSystem->set3DSettings(1.0f, 300.0f, 0.01f);
}

SoundManager::~SoundManager()
{
	for (auto item : orcAudios)
		delete item.second;
	orcAudios.clear();

	delete playerAudio;
	delete bossAudio;
	soundSystem->release();
}

void SoundManager::Update()
{
	//if (target == nullptr)return;
	//listenerPos = { target->Pos().x, target->Pos().y, target->Pos().z };
	listenerPos = { CAM->Pos().x, CAM->Pos().y, CAM->Pos().z };
	soundSystem->set3DListenerAttributes(0, &listenerPos, nullptr, nullptr, nullptr);

	soundSystem->update();

	//playerAudio->Update();
	//bossAudio->Update(); // ���� �����ϸ� �ּ� �����ϱ�

	//for (pair<int, Audio*> item : orcAudios)
		//item.second->Update();
}

void SoundManager::GUIRender()
{
}

void SoundManager::OrcCreate(int id, Transform* t)
{
	Audio* audio = AudioCreate(t);

	// ��ũ ����
	audio->Add("Orc_Hit", "Sounds/Orc/hit.wav", false, false, true);
	//audio->Add("Orc_Test", "Sounds/move.wav", false, true, true);
	audio->Add("Orc_Walk", "Sounds/Orc/Orc_WALK.wav", false, true, true);

	orcAudios.insert({ id,audio });
}

void SoundManager::PlayerCreate(Transform* t)
{
	Audio* audio = AudioCreate(t);

	// �����
	audio->Add("bgm1", "Sounds/BGM/dramatic-choir.wav", true, true, false);
	audio->Play("bgm1", 0.6f);

	// �÷��̾� ����
	audio->Add("Player_Move", "Sounds/Player/footstep.wav", false, true, true);	// �ȱ�
	audio->Add("Player_Assassination", "Sounds/Player/Assassination.mp3", false, false, false); // �ϻ�
	audio->Add("Player_Attack", "Sounds/Player/PlayerAttack.mp3", false, false, false);	// ��� ����
	audio->Add("Player_BowLoading", "Sounds/Player/BowLoading.mp3", false, false, false);	// ���� ����
	audio->Add("Player_ShootArrow", "Sounds/Player/ShootArrow.mp3", false, false, false);	// Ȱ ���

	playerAudio = audio;
}

void SoundManager::BossCreate(Transform* t)
{
	Audio* audio = AudioCreate(t);
	audio->Add("Boss_Run", "Sounds/Boss/Bossfootstep.mp3", false, false, true); // ���� �̵�
	audio->Add("Boss_Splash", "Sounds/Boss/BossSplash.mp3", false, false, true); // ���� ���÷���?
	audio->Add("Boss_Walk", "Sounds/Boss/Bosswalk.mp3", false, false, true); // ���� �ȱ�
	audio->Add("Boss_Roar", "Sounds/Boss/Roar.mp3", false, false, true); // ���� ��ȿ

	bossAudio = audio;
}

Audio* SoundManager::AudioCreate(Transform* t)
{
	Audio* audio = new Audio();

	audio->SetTarget(t);

	return audio;
}