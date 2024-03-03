#include "Framework.h"

InteractManager::InteractManager()
{
}

InteractManager::~InteractManager()
{
}

void InteractManager::ActiveSkill(string activeSkill,char key, Event event)
{
	Skill s;
	s.skill = activeSkill;
	s.key = key;
	s.event = event;
	activeSkills.push_back(s);
}

void InteractManager::ClearSkill()
{
	activeSkills.clear();
}

void InteractManager::Assassination(Orc* orc)
{
	player->Assassination();

	Vector3 pos = orc->GetTransform()->Pos();
	pos.y += 5.0f;
	orc->Assassinated(pos,player);
}

void InteractManager::Climb(Collider* col)
{
	player->Climb(col,col->GetPickContact().hitPoint);
}
