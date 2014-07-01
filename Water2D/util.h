#ifndef _UTIL_H
#define _UTIL_H

double linearInterpolation(double x1, double f_x1, double x2, double f_x2, double x);

double bilinearInterpolation(double x1, double f_x1, double g_x1, double x2, double f_x2, 
	double g_x2, double x, double y1, double y2, double y);

#endif