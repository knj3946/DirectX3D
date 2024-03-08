#include "Framework.h"

SoundManager::SoundManager()
{
	// key�� ������Ʈ��_�ൿ ���� ����
	// �� �ܾ��� ù ���ڸ� �빮�ڷ�


	// ���⼭ ��� ������ҽ� �߰�

	Audio::Get();

	// �����
	Audio::Get()->Add("bgm1", "Sounds/BGM/dramatic-choir.wav", true, true, false);
	Audio::Get()->Play("bgm1",0.6f);


	// �÷��̾� ����
	Audio::Get()->Add("Player_Move","Sounds/Player/footstep.wav", false, true, true);	// �ȱ�
	Audio::Get()->Add("Player_Assassination","Sounds/Player/Assassination.mp3", false, false, false); // �ϻ�
	Audio::Get()->Add("Player_Attack","Sounds/Player/PlayerAttack.mp3", false, false, false);	// ��� ����
	Audio::Get()->Add("Player_BowLoading","Sounds/Player/BowLoading.mp3", false, false, false);	// ���� ����
	Audio::Get()->Add("Player_ShootArrow","Sounds/Player/ShootArrow.mp3", false, false, false);	// Ȱ ���



	// ��ũ ����
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
