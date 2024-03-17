#pragma once

using namespace FMOD;

class Audio// : public Singleton<Audio>
{
private:
    //friend class Singleton;

    //const int MAX_CHANNEL = 20;

    struct SoundInfo
    {
        Sound* sound = nullptr;
        Channel* channel = nullptr;

        ~SoundInfo()
        {
            sound->release();
        }
    };

    

public:
    Audio();
    ~Audio();

    void SetTarget(class Transform* t) { target = t; }

    void Update();

    void Add(string key, string file,
        bool bgm = false, bool loop = false, bool is3D = false);

    void Play(string key, float valume = 1.0f);
    void Play(string key, Float3 position, float valume = 1.0f);
    void Stop(string key);
    void AllStop();
    void Pause(string key);
    void Resume(string key);

    bool IsPlaySound(string key);
    SoundInfo* GetSounds(string key) { return sounds[key]; }
    float GetVolume(string key) { float volume; sounds[key]->channel->getVolume(&volume); return volume; }
    void SetVolume(string key, float volume) { sounds[key]->channel->setVolume(volume); }
    FMOD_VECTOR GetSoundPos(string key) {
        FMOD_VECTOR pos; FMOD_VECTOR vel; sounds[key]->channel->get3DAttributes(&pos, &vel); return pos;
    }
    FMOD_VECTOR GetSoundvelocity(string key) {
        FMOD_VECTOR pos; FMOD_VECTOR vel; sounds[key]->channel->get3DAttributes(&pos, &vel); return vel;
    }
private:
    //System* soundSystem;

    unordered_map<string, SoundInfo*> sounds;

    FMOD_VECTOR listenerPos;

    class Transform* target;
};