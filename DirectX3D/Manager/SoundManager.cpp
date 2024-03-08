#include "Framework.h"

SoundManager::SoundManager()
{
	// key는 오브젝트명_행동 으로 정의
	// 각 단어의 첫 문자만 대문자로


	// 여기서 모든 오디오소스 추가

	Audio::Get();

	// 배경음
	Audio::Get()->Add("bgm1", "Sounds/BGM/dramatic-choir.wav", true, true, false);
	Audio::Get()->Play("bgm1",0.6f);


	// 플레이어 사운드
	Audio::Get()->Add("Player_Move","Sounds/Player/footstep.wav", false, true, true);	// 걷기
	Audio::Get()->Add("Player_Assassination","Sounds/Player/Assassination.mp3", false, false, false); // 암살
	Audio::Get()->Add("Player_Attack","Sounds/Player/PlayerAttack.mp3", false, false, false);	// 대거 공격
	Audio::Get()->Add("Player_BowLoading","Sounds/Player/BowLoading.mp3", false, false, false);	// 시위 당기기
	Audio::Get()->Add("Player_ShootArrow","Sounds/Player/ShootArrow.mp3", false, false, false);	// 활 쏘기



	// 오크 사운드
	Audio::Get()->Add("Orc_Hit","Sounds/Orc/hit.wav", false, false, true);
	Audio::Get()->Add("Orc_Test","Sounds/move.wav", false, true, true);

}

SoundManager::~SoundManager()
{
	//delete soundKey;
}

void SoundManager::Add(string key)
{
	
}
