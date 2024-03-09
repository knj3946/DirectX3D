#include "Framework.h"

SoundManager::SoundManager()
{
	// key는 오브젝트명_행동 으로 정의
	// 각 단어의 첫 문자만 대문자로

	// 여기서 모든 오디오소스 추가
}

SoundManager::~SoundManager()
{
	for (auto item : orcAudios)
		delete item.second;
	orcAudios.clear();

	delete playerAudio;
	delete bossAudio;
}

void SoundManager::Update()
{
	playerAudio->Update();
	//bossAudio->Update(); // 보스 생성하면 주석 해제하기

	for (pair<int, Audio*> item : orcAudios)
		item.second->Update();
}

void SoundManager::GUIRender()
{
}

void SoundManager::OrcCreate(int id, Transform* t)
{
	Audio* audio = AudioCreate(t);

	// 오크 사운드
	audio->Add("Orc_Hit", "Sounds/Orc/hit.wav", false, false, true);
	//audio->Add("Orc_Test", "Sounds/move.wav", false, true, true);
	audio->Add("Orc_Walk", "Sounds/Orc/Orc_WALK.wav", false, true, true);

	orcAudios.insert({ id,audio });
}

void SoundManager::PlayerCreate(Transform* t)
{
	Audio* audio = AudioCreate(t);

	// 배경음
	audio->Add("bgm1", "Sounds/BGM/dramatic-choir.wav", true, true, false);
	audio->Play("bgm1", 0.6f);

	// 플레이어 사운드
	audio->Add("Player_Move", "Sounds/Player/footstep.wav", false, true, true);	// 걷기
	audio->Add("Player_Assassination", "Sounds/Player/Assassination.mp3", false, false, false); // 암살
	audio->Add("Player_Attack", "Sounds/Player/PlayerAttack.mp3", false, false, false);	// 대거 공격
	audio->Add("Player_BowLoading", "Sounds/Player/BowLoading.mp3", false, false, false);	// 시위 당기기
	audio->Add("Player_ShootArrow", "Sounds/Player/ShootArrow.mp3", false, false, false);	// 활 쏘기

	playerAudio = audio;
}

void SoundManager::BossCreate(Transform* t)
{
	Audio* audio = AudioCreate(t);
	audio->Add("Boss_Run", "Sounds/Boss/Bossfootstep.mp3", false, false, true); // 보스 이동
	audio->Add("Boss_Splash", "Sounds/Boss/BossSplash.mp3", false, false, true); // 보스 스플래시?
	audio->Add("Boss_Walk", "Sounds/Boss/Bosswalk.mp3", false, false, true); // 보스 걷기
	audio->Add("Boss_Roar", "Sounds/Boss/Roar.mp3", false, false, true); // 보스 포효

	bossAudio = audio;
}

Audio* SoundManager::AudioCreate(Transform* t)
{
	Audio* audio = new Audio();

	audio->SetTarget(t);

	return audio;
}
