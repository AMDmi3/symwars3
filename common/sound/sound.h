#ifndef SOUND_H
#define SOUND_H

#include "../platform.h"

class Sound
{
public:
	Sound() {}
	Sound(const char* fp);

	char filepath[MAX_PATH+1];
	void play();
};

extern std::vector<Sound> g_ordersnd;

void SoundPath(const char* from, char* to);

void Sound_Order();

#endif
