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


	Vector3 v1 = player->GlobalPos();
	Vector3 v2 = col->GlobalPos();

	Vector3 v2m1 = v2 - v1;

    int maxIndex = -1;
    float maxValue = -99999.0f;

    for (int i = 0; i < 3; ++i)
    {
        Vector3 halfSize = col->GetHalfSize();

        float Val = 0;

        switch (i)
        {
            case 0:
            {
                Vector3 tempv = col->Right();
                Val = Dot(v2m1, tempv);
                break;
            }
            case 1:
            {
                Vector3 tempv = col->Up();
                Val = Dot(v2m1, tempv);
                break;
            }
            case 2:
            {
                Vector3 tempv = col->Forward();
                Val = Dot(v2m1, tempv);
                break;
            }
        }

        if (i != 1)
        {
            if (abs(Val) - abs(halfSize[i]) > maxValue)
            {
                maxIndex = i;
                maxValue = abs(Val) - abs(halfSize[i]);
            }
        }
    }
    if (maxIndex > -1)
    {
        float rotY = asin(maxValue/v2m1.Length());
        player->Rot().y = rotY;
    }
}