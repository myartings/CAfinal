#ifndef _CONST_H_
#define _CONST_H_

#define EPS 1e-12f
#define PI 3.14159265358979323846
#define D360 6.28318530717958647693
#define PI_180 0.01745329251994329577

#define TIME_STEP 0.01f
#define GRAVITY 10.0f
#define MASS 1.0f
#define KR 0.078125f
#define DEFAULT_DENSITY 1024.0f
#define PARTICLE_NUM 1024
#define GAS_CONSTANT 2.0f
#define VISCOSITY 0.02f
#define ELASTICITY 0.618f
#define RENDER_SAMPLE 100//128

#define SQ(x) ((x) * (x))
#define CUBE(x) ((x) * (x) * (x))

const float KR2 = SQ(KR);
const float KR3 = KR2 * KR;
const float KR4 = SQ(KR2);
const float KR6 = SQ(KR3);
const float KR9 = KR6 * KR3;

float WPoly6Scale;
float WSpikyScale;
float WViscosityScale;
float WLucyScale;

enum status {water, ice, bubble};

//thermal diffusion constant
const float cThermalWater = 0.143;
const float cThermalIce = 1.235;

const float Tair = -20.0f;
const float Tfreeze = 0.0f;
const float Twater = 20.0f;

bool print;
bool freeze;
bool gridBuilt;
const float gridSize = 0.02f;

float colorWater[4] = { 0.3, 0.5, 0.6, 1.0};
float colorIce[4] = { 0.3, 0.5, 0.9, 1.0};
#endif