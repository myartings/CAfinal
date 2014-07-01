//#pragma once
#ifndef _GRIDDATA_H_
#define _GRIDDATA_H_
#include"particle.h"
#include <vector>
struct GridData
{
	float DT;
	float T;
	Point2f pos;
	std::vector<Particle *> pv;
	bool flagOfData;
	void updateParticles()
	{
	}

};
#endif