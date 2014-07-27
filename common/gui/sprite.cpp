#include "sprite.h"
#include "../utils.h"
#include "../platform.h"
#include "../texture.h"

Sprite::Sprite()
{
	texindex = 0;
}

void DefS(const char* relative, Sprite* s, int offx, int offy)
{
	CreateTexture(s->texindex, relative, true, false);
	s->offset[0] = offx;
	s->offset[1] = offy;
}
