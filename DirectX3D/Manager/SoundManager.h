#pragma once

class SoundManager : public Singleton<SoundManager>
{
	friend class Singleton;

public:
	SoundManager();
	~SoundManager();


};

