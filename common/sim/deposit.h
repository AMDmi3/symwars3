#ifndef DEPOSIT_H
#define DEPOSIT_H

#include "../math/vec2i.h"
#include "../math/matrix.h"
#include "../math/vec3f.h"

class Deposit
{
public:
	bool on;
	bool occupied;
	int restype;
	int amount;
	Vec2i tilepos;
	Vec3f drawpos;

	Deposit();
};

#define DEPOSITS	128

extern Deposit g_deposit[DEPOSITS];

void FreeDeposits();
void DrawDeposits(const Matrix projection, const Matrix viewmat);

#endif
