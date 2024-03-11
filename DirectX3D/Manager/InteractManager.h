#pragma once
class InteractManager : public Singleton<InteractManager>
{
private:
    friend class Singleton;

    InteractManager();
    ~InteractManager();
public:
    struct Skill
    {
        string skill;
        char key;
        Event event;
    };
public:
    void SetPlayer(Player* player) { this->player = player; }

    void ActiveSkill(string activeSkill,char key, Event event);
    void ClearSkill();

    vector<Skill> GetActiveSkills() { return activeSkills; }


    void Assassination(Orc* orc);
    void AssassinationBoss(Boss* boss);
    void Climb(Collider* col);
private:
    Player* player;
    vector<Skill> activeSkills;
};

