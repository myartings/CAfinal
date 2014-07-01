#include"util.h"

float linearInterpolation(float x1, float f_x1, float x2, float f_x2, float x)
{
	float result = (x - x1)/(x2-x1)*f_x2  + (x2-x)/(x2-x1)*f_x1;
	return result;
}

float bilinearInterpolation(float x1, float f_x1, float g_x1, float x2, float f_x2,
	float g_x2, float x, float y1, float y2, float y)
{
	float R1 = linearInterpolation(x1, f_x1, x2, f_x2, x);
	float R2 = linearInterpolation(x1, g_x1, x2, g_x2, x);
	float result = linearInterpolation(y1, R1, y2, R2, y);;
	return result;
}