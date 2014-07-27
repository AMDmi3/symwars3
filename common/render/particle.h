#ifndef _PARTICLE_H
#define _PARTICLE_H

#pragma warning(disable: 4018)

#include "../math/3dmath.h"
#include "../math/vec3f.h"
#include "../utils.h"

class Particle;
class Billboard;

class ParticleT
{
public:
	int billbT;
	int delay;
	float decay;
	Vec3f minvelocity;
	Vec3f velvariation;
	Vec3f minacceleration;
	Vec3f accelvariation;
	float minsize;
	float sizevariation;
	void (*collision)(Particle* part, Billboard* billb, Vec3f trace, Vec3f normal);
};

#define PARTICLE_EXHAUST		0
#define PARTICLE_EXHAUST2		1
#define PARTICLE_EXHAUSTBIG		2
#define PARTICLE_FIREBALL		3
#define PARTICLE_FIREBALL2		4
#define PARTICLE_SMOKE			5
#define PARTICLE_SMOKE2			6
#define PARTICLE_DEBRIS			7
#define PARTICLE_FLAME			8
#define PARTICLE_PLUME			9
#define PARTICLE_TYPES			10
extern ParticleT g_particleT[PARTICLE_TYPES];

class EmitterCounter
{
public:
	long long last;

	EmitterCounter()
	{
		last = GetTickCount();
	}
	bool EmitNext(int delay)
	{
		if(GetTickCount64()-last > (long long)delay)
		{
			last = GetTickCount64();
			return true;
		}
		else
			return false;
	}
};

class Billboard;

class Particle
{
public:
	bool on;
	int type;
	float life;
	Vec3f vel;
	float dist;

	Particle()
	{
		on = false;
	}
	Particle(Vec3f p, Vec3f v)
	{
		on = true;
		vel = v;
	}

	void update(Billboard* billb);
};

#define PARTICLES 512
extern Particle g_particle[PARTICLES];

class EmitterPlace
{
public:
	EmitterPlace()
	{
		on = false;
	}
	EmitterPlace(int t, Vec3f off)
	{
		on = true;
		type=t;
		offset=off;
	}
	bool on;
	Vec3f offset;
	int type;
};

void LoadParticles();
void Particles();
void EmitParticle(int type, Vec3f pos);
void UpdateParticles();

#endif
