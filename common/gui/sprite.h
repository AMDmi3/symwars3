#ifndef SPRITE_H
#define SPRITE_H

class Sprite
{
public:

	Sprite();

	unsigned int texindex;
	float offset[2];
};

void DefS(const char* relative, Sprite* s, int offx, int offy);

#endif
