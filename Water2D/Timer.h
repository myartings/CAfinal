// File		Timer.h
// Author	Ruoguan Huang
// Version	1.5
// Date		April 20, 2009

#ifndef _TIMER_H_
#define _TIMER_H_

#include <windows.h>

class Timer
{
private:
	int count, current;
	double initial;
	double *times;
	double frequency;

public:
	Timer() {}
	
	~Timer()
	{
		if (times != NULL) {
			delete []times;
			times = NULL;
		}
	}

	void init(void)
	{
		__int64 temp;

		count = 101;
		if (count < 2) count = 2;
		times = new double[count];
		QueryPerformanceCounter((LARGE_INTEGER*)&temp);
		initial = (double)temp;
		QueryPerformanceFrequency((LARGE_INTEGER*)&temp);
		frequency = 1.0 / (double)temp;
		for (int i = 0; i < count; i++)
			times[i] = initial;
		current = 0;
	}

	void init(int smooth)
	{
		__int64 temp;

		count = smooth + 1;
		if (count < 2) count = 2;
		times = new double[count];
		QueryPerformanceCounter((LARGE_INTEGER*)&temp);
		initial = (double)temp;
		QueryPerformanceFrequency((LARGE_INTEGER*)&temp);
		frequency = 1.0 / (double)temp;
		for (int i = 0; i < count; i++)
			times[i] = initial;
		current = 0;
	}

	void update(void)
	{
		__int64 temp;
		QueryPerformanceCounter((LARGE_INTEGER*)&temp);
		current = (current + 1) % count;
		times[current] = (double)temp;
	}

	void update(double Hz)
	{
		__int64 temp;
		double dt =  1.0 / Hz / frequency;
		QueryPerformanceCounter((LARGE_INTEGER*)&temp);
		while ((double)temp - times[current] < dt)
			QueryPerformanceCounter((LARGE_INTEGER*)&temp);
		current = (current + 1) % count;
		times[current] = (double)temp;
	}

	int update(double Hz, void (*func)(void))
	{
		__int64 temp;
		int n = 0;
		double dt =  1.0 / Hz / frequency;
		if (func != NULL) {
			func();
			n++;
			QueryPerformanceCounter((LARGE_INTEGER*)&temp);
			while ((double)temp - times[current] < dt) {
				func();
				n++;
				QueryPerformanceCounter((LARGE_INTEGER*)&temp);
			}
		}
		else {
			QueryPerformanceCounter((LARGE_INTEGER*)&temp);
			while ((double)temp - times[current] < dt)
				QueryPerformanceCounter((LARGE_INTEGER*)&temp);
		}
		current = (current + 1) % count;
		times[current] = (double)temp;
		return n;
	}

	double avgTime(void)
	{
		double dt = times[current] - times[(current + 1) % count];

		return dt * frequency / (double)(count - 1);
	}

	double avgHz(void)
	{
		double dt = times[current] - times[(current + 1) % count];

		return 1.0 / (dt * frequency / (double)(count - 1));
	}

	double lastTime(void)
	{
		double dt = times[current] - times[(current + count - 1) % count];

		return dt * frequency;
	}

	double totalTime(void)
	{
		__int64 temp;
		QueryPerformanceCounter((LARGE_INTEGER*)&temp);
		return ((double)temp - initial) * frequency;
	}
};

#endif
