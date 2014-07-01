#ifndef _PARTICLE_H
#define _PARTICLE_H
#include"const.h"
#include"Point.h"

struct Particle {
	Point2f pos, vel, acc;
	float dens, pressure;
	Particle *next;
	//status
	status phase;
	//temperature
	float T;
	//dissolved air
	float S;

	float volume;

};

#endif