#ifndef _SPH_H_
#define _SPH_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "const.h"
#include "Point.h"
#include "particle.h"
#include "GridData.h"
#include "util.h"

typedef Particle *Ptr;

class SPH
{
private:
	int tLen, tSize;
	Particle **table;
	Point2f pos0, vel0;
	GridData Grid[50][50];

	void buildTable(void)
	{
		int i, x, y, index;

		for (i = 0; i < tSize; i++) table[i] = NULL;
		for (i = 0; i < pNum; i++) {
			x = (int)(p[i].pos.x / KR);
			y = (int)(p[i].pos.y / KR);
			index = x + y * tLen;
			if (table[index] == NULL) {
				table[index] = &p[i];
				p[i].next = NULL;
			} 
			else {
				p[i].next = table[index];
				table[index] = &p[i];
			}
		}
	}
	
	void buildGrid(void)
	{
		int i, j, x, y, index;

		//clean
		for(i = 0; i < 50; i++)
			for(j = 0; j < 50; j++)
			{
				Grid[i][j].T = Twater;
				Grid[i][j].DT = 0.0f;
				Grid[i][j].pv.clear();
				Grid[i][j].flagOfData = false;
			}

		for (i = 0; i < pNum; i++) {
			x = (int)(p[i].pos.x/gridSize);
			y = (int)(p[i].pos.y/gridSize);
			//printf("px: %f, py: %f\n", p[i].pos.x, p[i].pos.y);
			//printf("x: %d, y: %d\n", x, y);
			if(p[i].pos.x != 1.0f && p[i].pos.y != 1.0f)
				Grid[x][y].pv.push_back(&p[i]);
			else 
				p[i].phase = ice;

		}

		for(i = 0; i < 50; i++)
			for(j = 0; j < 50; j++)
				if(Grid[i][j].pv.size() != 0)
					Grid[i][j].flagOfData = true;
	}

	inline float WPoly6(const Point2f &r)
	{
		float r2 = r.LengthSquared();
		if (r2 > KR2) return 0.0f;
		float a = KR2 - r2;
		return WPoly6Scale * CUBE(a);
	}

	inline Point2f WPoly6Grad(const Point2f &r)
	{
		float r2 = r.LengthSquared();
		if (r2 > KR2) return Point2f(0.0f, 0.0f);
		float a = KR2 - r2;
		float b = -6.0f * WPoly6Scale * SQ(a);
		return Point2f(b * r.x, b * r.y); 
	}

	inline float WSpiky(const Point2f &R)
	{
		float r2 = R.LengthSquared();
		if (r2 > KR2) return 0.0f;
		float a = KR - sqrt(r2);
		return WSpikyScale * CUBE(a);
	}

	inline Point2f WSpikyGrad(const Point2f &R)
	{
		float r2 = R.LengthSquared();
		if (r2 > KR2) return Point2f(0.0f, 0.0f);
		if (r2 < EPS) r2 = EPS;
		float r = sqrt(r2);
		float a = -3.0f * WSpikyScale * (KR - r) * (KR - r) / r;
		return Point2f(a * R.x, a * R.y);
	}

	inline float WViscosity(const Point2f &R)
	{
		float r2 = R.LengthSquared();
		if (r2 > KR2) return 0.0f;
		if (r2 < EPS) r2 = EPS;
		float r = sqrt(r2);
		float r3 = r * r * r;
		return WViscosityScale * (((-r3 / (2.0f * KR3)) + (r2 / KR2) + KR / (2.0f * r)) - 1.0f);
	}

	inline float WViscosityLap(const Point2f &R)
	{
		float r2 = R.LengthSquared();
		if (r2 > KR2) return 0.0f;
		float r = sqrt(r2);
		return WViscosityScale * (6.0f / KR3) * (KR - r);
	}

	inline float WLucy(const Point2f &R)
	{
		float r2 = R.LengthSquared();
		if (r2 > KR2) return 0.0f;
		float r = sqrt(r2);
		float a = (1.0f + 3.0f * r / KR) * CUBE(1 - r / KR);
		return WLucyScale * a;
	}

	inline Point2f WLucyGrad(const Point2f &R)
	{
		float r2 = R.LengthSquared();
		if (r2 > KR2) return Point2f(0.0f, 0.0f);
		float r = sqrt(r2);
		float a = -12.0f * (KR2 - 2.0f * r * KR + r2) / KR4;
		return R * a;
	}

	void normalizeKernels()
	{
		int i, j, n = 1000;

		float xmin = -KR;
		float ymin = -KR;
		float xmax = KR;
		float ymax = KR;
		float dx = (xmax - xmin) / n;
		float dy = (ymax - ymin) / n;
		float dA = dx * dy;
		float totalPoly6 = 0.0f;
		float totalSpiky = 0.0f;
		float totalViscosity = 0.0f;
		float totalLucy = 0.0f;

		WPoly6Scale = 1.0f;
		WSpikyScale = 1.0f;
		WViscosityScale = 1.0f;
		WLucyScale = 1.0f;

		for (i = 0 ; i < n ; i++) {
			for (j = 0 ; j < n ; j++) {
				Point2f r(xmin + i * dx, ymin + j * dy);
				totalPoly6 += WPoly6(r) * dA;
				totalSpiky += WSpiky(r) * dA;
				totalViscosity += WViscosity(r) * dA;
				totalLucy += WLucy(r) * dA;
			}
		}

		WPoly6Scale = 1.0f / totalPoly6;
		WSpikyScale = 1.0f / totalSpiky;
		WViscosityScale = 1.0f / totalViscosity;
		WLucyScale = 1.0f / totalLucy;
	}

	void computeDP(void)
	{
		int i, j, x0, y0, x, y;
		int dx[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
		int dy[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
		Point2f r;
		Particle *iter;

		for (i = 0; i < pNum; i++) {
			p[i].dens = 0.0f;
			x0 = (int)(p[i].pos.x / KR);
			y0 = (int)(p[i].pos.y / KR);
			for (j = 0; j < 9; j++) {
				x = x0 + dx[j];
				y = y0 + dy[j];
				if (x < 0 || x > tLen - 1 || y < 0 || y > tLen - 1) continue;
				iter = table[x + y * tLen];
				while (iter != NULL) {
					if (iter != &p[i]) {
						r = p[i].pos - iter->pos;
						p[i].dens += WPoly6(r);
					}
					iter = iter->next;
				}
			}
			if (p[i].dens < EPS) p[i].dens = DEFAULT_DENSITY;
			p[i].dens *= MASS;
			p[i].pressure = GAS_CONSTANT * (CUBE(p[i].dens / DEFAULT_DENSITY) - 1.0f);
		}
	}

	void computeForce(void)
	{
		int i, j, x0, y0, x, y;
		int dx[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
		int dy[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
		Point2f r, ap, av, g(0.0f, -GRAVITY);
		Particle *iter;

		for (i = 0; i < pNum; i++) {
			ap.Zero();
			av.Zero();
			x0 = (int)(p[i].pos.x / KR);
			y0 = (int)(p[i].pos.y / KR);
			for (j = 0; j < 9; j++) {
				x = x0 + dx[j];
				y = y0 + dy[j];
				if (x < 0 || x > tLen - 1 || y < 0 || y > tLen - 1) continue;
				iter = table[x + y * tLen];
				while (iter != NULL) {
					if (iter != &p[i]) {
						r = p[i].pos - iter->pos;
						ap -= ((p[i].pressure + iter->pressure) / iter->dens) * WSpikyGrad(r);
						av += (WViscosityLap(r) / iter->dens) * (iter->vel - p[i].vel);
					}
					iter = iter->next;
				}
			}
			p[i].acc = 0.5f * ap + VISCOSITY * av + g;
		}
	}
	
	//heat transfer
	void transferHeat(void)
	{
		/*int i, j, x0, y0, x, y;
		int dx[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
		int dy[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
		Point2f r, ap, av, g(0.0f, -GRAVITY);
		//
		float detaHeat(0.0f);
		float cThermal(0.0f);

		Particle *iter;

		for (i = 0; i < pNum; i++) {
			ap.Zero();
			av.Zero();
			x0 = (int)(p[i].pos.x / KR);
			y0 = (int)(p[i].pos.y / KR);

			if(p[i].phase == water)
				cThermal = cThermalWater;
			else
				cThermal = cThermalIce;

			for (j = 0; j < 9; j++) {
				x = x0 + dx[j];
				y = y0 + dy[j];
				if (x < 0 || x > tLen - 1 || y < 0 || y > tLen - 1) continue;
				iter = table[x + y * tLen];
				while (iter != NULL) {
					if (iter != &p[i]) {
						r = p[i].pos - iter->pos;
						//ap -= ((p[i].pressure + iter->pressure) / iter->dens) * WSpikyGrad(r);
						detaHeat += cThermal * (WViscosityLap(r) / iter->dens) * (iter->T - p[i].T);
						if(detaHeat!= 0.0f)
						printf("detaHeat: %f\n",detaHeat);
					}
					iter = iter->next;
				}
			}
			p[i].T = p[i].T + detaHeat;
			if(p[i].phase == water && p[i].T <= Tfreeze)
				p[i].phase = ice;
		}*/

		//
		for(int i = 0; i < 50; i++)
			for(int j = 0; j < 50; j++)
			{
				Grid[i][j].T += Grid[i][j].DT;
				//if(Grid[i][j].T > 0)
				//printf("T of (%d,%d): %f\n", i, j, Grid[i][j].T);
				for (std::vector<Particle *>::iterator it = Grid[i][j].pv.begin() ; it != Grid[i][j].pv.end(); ++it)
				{
					(*it)->T = Grid[i][j].T;
					if((*it)->phase == water && (*it)->T <= Tfreeze)
						(*it)->phase = ice;
				}
			}

		//bilinear interpolation
		float x1,  f_x1,  g_x1,  x2,  f_x2,
	 g_x2,  x,  y1,  y2,  y;

		for(int i = 0; i < 50; i++)
			for(int j = 0; j < 50; j++)
				if(Grid[i][j].flagOfData)
			{
				x1 = Grid[i][j].pos.x;
				x2 = Grid[i][j].pos.y;

				//bilinearInterpolation();
			}

	}


	//
	void updateDissolvedAir(void)
	{
		int i, j, x0, y0, x, y;
		int dx[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
		int dy[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
		Point2f r;

		Particle *iter, *nearest;
		
		float deltaDpi = 0.0f, deltaDpj = 0.0f, temp = 0.0f;
		float W = 0.0f;
		float sumOfW = 0.0f;
		float rMin = 10.0f;
		for (i = 0; i < pNum; i++) {
			
			if(p[i].phase != ice || p[i].S == 0.0f) continue;

			sumOfW = 0.0f;
			temp = 0.0f;
			x0 = (int)(p[i].pos.x / KR);
			y0 = (int)(p[i].pos.y / KR);

			/*for (j = 0; j < 9; j++) {
				x = x0 + dx[j];
				y = y0 + dy[j];
				if (x < 0 || x > tLen - 1 || y < 0 || y > tLen - 1) continue;
				iter = table[x + y * tLen];
				
				while (iter != NULL) {
					if(iter->phase == water)
					if (iter != &p[i]) {
						r = p[i].pos - iter->pos;
						printf("r.length: %f\n",r.Length());
						W = 1/r.Length();
						sumOfW += W;
						temp += W * p[i].S;
						//if(detaAir == 0.0)
						//printf("detaAir: %f\n",detaAir);
						//printf("iter->S: %f, p[i].S: %f\n",iter->S,p[i].S);
					}
					iter = iter->next;
				}
				temp = temp/sumOfW;
				if(temp > p[i].S)
					deltaDpi = p[i].S;
				else
					deltaDpi = temp;
				//printf("deltaDpi: %f\n", deltaDpi);
			}
			//
			for (j = 0; j < 9; j++) {
				x = x0 + dx[j];
				y = y0 + dy[j];
				if (x < 0 || x > tLen - 1 || y < 0 || y > tLen - 1) continue;
				iter = table[x + y * tLen];
				
				while (iter != NULL) {
					if(iter->phase == water)
					if (iter != &p[i]) {
						r = p[i].pos - iter->pos;
						W = 1/r.Length();
						deltaDpj = W * deltaDpi / sumOfW;
						iter->S += deltaDpj;
						p[i].S -= deltaDpj;
						//if(detaAir == 0.0)
						//printf("detaAir: %f\n",detaAir);
						//printf("iter->S: %f, p[i].S: %f\n",iter->S,p[i].S);
					}
					iter = iter->next;
				}

			}*/

			//only consider nearest point
			nearest = NULL;
			for (j = 0; j < 9; j++) {
				x = x0 + dx[j];
				y = y0 + dy[j];
				if (x < 0 || x > tLen - 1 || y < 0 || y > tLen - 1) continue;
				iter = table[x + y * tLen];
				
				while (iter != NULL) {
					if(iter->phase == water)
					if (iter != &p[i]) {
						r = p[i].pos - iter->pos;
						//printf("r.length: %f\n",r.Length());
						if(r.Length() < rMin)
						{
							rMin = r.Length();
							nearest = iter;
						}
					}
					iter = iter->next;
				}
			}
			if(nearest)
			{
				nearest->S += p[i].S;
				p[i].S = 0.0f;

				if(nearest->S > 1.3)
				{
					nearest->phase = bubble;
					nearest->volume = nearest->S;
				}
			}
		}
	}
	void checkAir()
	{
		int i;

		for (i = 0; i < pNum; i++) {
		if (p[i].pos.x < 0.01f) {
			p[i].phase = bubble;
			p[i].T = Tair;
		}
		else if (p[i].pos.x > 0.99f) {
			p[i].phase = bubble;
			p[i].T = Tair;
		}
		if (p[i].pos.y < 0.01f) {
			p[i].phase = bubble;
			p[i].T = Tair;
		}
		else if (p[i].pos.y > 0.99f) {
			p[i].phase = bubble;
			p[i].T = Tair;
		}
		}

	}

	void fixIce()
	{
		int i;
		for (i = 0; i < pNum; i++) {
			if(p[i].phase == ice)
			{
				p[i].vel.x = 0;
				p[i].vel.y = 0;
			}
		}
	}

	void integrate(void)
	{
		int i;

		for (i = 0; i < pNum; i++) {
			p[i].vel += p[i].acc * h;
			p[i].pos += p[i].vel * h;
		}
	}

	void fixBoundary(void)
	{
		int i;
		float bedding = 0.0f;
		for (i = 0; i < pNum; i++) {
			if (p[i].pos.x < 0.0f + bedding) {
				p[i].pos.x = EPS;
				p[i].vel.x = -p[i].vel.x * ELASTICITY;
			}
			else if (p[i].pos.x > 1.0f - bedding) {
				p[i].pos.x = 1.0f - EPS;
				p[i].vel.x = -p[i].vel.x * ELASTICITY;
			}
			if (p[i].pos.y < 0.0f + bedding) {
				p[i].pos.y = EPS;
				p[i].vel.y = -p[i].vel.y * ELASTICITY;
			}
			else if (p[i].pos.y > 1.0f - bedding) {
				p[i].pos.y = 1.0f - EPS;
				p[i].vel.y = -p[i].vel.y * ELASTICITY;
			}
		}
	}

	void generateTexture(float * texture, float phase, float * color)
	{
		int i, j, k, x0, y0, x, y;
		int dx[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
		int dy[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
		float delta = 1.0f / RENDER_SAMPLE;
		float intensity, dens, *data;
		Point2f pos;
		Particle *iter;

		float max = 0.0f, min = 1e6f;
		for (i = 0; i < RENDER_SAMPLE; i++) {
			for (j = 0; j < RENDER_SAMPLE; j++) {
				dens = 0.0f;
				pos.x = delta * i;
				pos.y = delta * j;
				x0 = (int)(pos.x / KR);
				y0 = (int)(pos.y / KR);
				for (k = 0; k < 9; k++) {
					x = x0 + dx[k];
					y = y0 + dy[k];
					if (x < 0 || x > tLen - 1 || y < 0 || y > tLen - 1) continue;
					iter = table[x + y * tLen];
					while (iter != NULL) {
						if(iter->phase == phase)
						dens += WPoly6(pos - iter->pos);
						iter = iter->next;
					}
				}
				intensity = MASS * dens / 500.0f;
				if (intensity > 1.0f) intensity = 1.0f;
				data = &texture[4 * (i + (j * RENDER_SAMPLE))];
				data[0] = color[0] * intensity;
				data[1] = color[1] * intensity;
				data[2] = color[2] * intensity;
				data[3] = color[3] * intensity;
			}
		}
	}

	void marchingSquares(float * texture, Point2f * lineA, Point2f * lineB, int &nLine)
	{
		int i, j, key, s;
		float isolevel = 0.5f;
		float c[4];
		Point2f v[4], p[4];

		s = 0;
		for (i = 0; i < RENDER_SAMPLE - 1; i++) {
			for (j = 0; j < RENDER_SAMPLE - 1; j++) {
				c[0] = texture[4 * (i + j * RENDER_SAMPLE) + 3] - isolevel;
				c[1] = texture[4 * (i + 1 + j * RENDER_SAMPLE) + 3] - isolevel;
				c[2] = texture[4 * (i + 1 + (j + 1) * RENDER_SAMPLE) + 3] - isolevel;
				c[3] = texture[4 * (i + (j + 1) * RENDER_SAMPLE) + 3] - isolevel;
				v[0] = Point2f((float)i / RENDER_SAMPLE, (float)j / RENDER_SAMPLE);
				v[1] = Point2f((float)(i + 1) / RENDER_SAMPLE, (float)j / RENDER_SAMPLE);
				v[2] = Point2f((float)(i + 1) / RENDER_SAMPLE, (float)(j + 1) / RENDER_SAMPLE);
				v[3] = Point2f((float)i / RENDER_SAMPLE, (float)(j + 1) / RENDER_SAMPLE);
				p[0] = (c[0] * v[0] - c[1] * v[1]) / (c[0] - c[1]);
				p[1] = (c[1] * v[1] - c[2] * v[2]) / (c[1] - c[2]);
				p[2] = (c[2] * v[2] - c[3] * v[3]) / (c[2] - c[3]);
				p[3] = (c[3] * v[3] - c[0] * v[0]) / (c[3] - c[0]);
				key = (c[0] > 0 ? 1 : 0) + (c[1] > 0 ? 2 : 0) + (c[2] > 0 ? 4 : 0) + (c[3] > 0 ? 8 : 0);
				switch (key) {
					case 0:
					case 15:
						break;
					case 1:
					case 14:
						lineA[s] = p[3];
						lineB[s] = p[0];
						s++;
						break;
					case 2:
					case 13:
						lineA[s] = p[0];
						lineB[s] = p[1];
						s++;
						break;
					case 4:
					case 11:
						lineA[s] = p[1];
						lineB[s] = p[2];
						s++;
						break;
					case 8:
					case 7:
						lineA[s] = p[2];
						lineB[s] = p[3];
						s++;
						break;
					case 3:
					case 12:
						lineA[s] = p[3];
						lineB[s] = p[1];
						s++;
						break;
					case 6:
					case 9:
						lineA[s] = p[0];
						lineB[s] = p[2];
						s++;
						break;
					case 5:
						lineA[s] = p[3];
						lineB[s] = p[0];
						s++;
						lineA[s] = p[1];
						lineB[s] = p[2];
						s++;
						break;
					case 10:
						lineA[s] = p[0];
						lineB[s] = p[1];
						s++;
						lineA[s] = p[2];
						lineB[s] = p[3];
						s++;
						break;
					default:
						break;
				}
			}
		}
		
		nLine = s;
	}

public:
	int pNum;
	Particle *p;
	float h;
	float *textureWater, *textureIce;
	int nLine0, nLine1;
	Point2f *line0, *line1;
	Point2f *line2, *line3;
	int renderMode;

	SPH()
	{
		h = TIME_STEP;
		pNum = 0;
		p = NULL;
		tLen = tSize = 0;
		table = NULL;
		textureWater = new float[RENDER_SAMPLE * RENDER_SAMPLE * 4];
		textureIce = new float[RENDER_SAMPLE * RENDER_SAMPLE * 4];
		nLine0 = 0;
		nLine1 = 0;
		line0 = new Point2f[RENDER_SAMPLE * RENDER_SAMPLE * 2];
		line1 = new Point2f[RENDER_SAMPLE * RENDER_SAMPLE * 2];
		line2 = new Point2f[RENDER_SAMPLE * RENDER_SAMPLE * 2];
		line3 = new Point2f[RENDER_SAMPLE * RENDER_SAMPLE * 2];
		renderMode = 0;
	}

	~SPH()
	{
		if (p != NULL) delete []p;
		if (table != NULL) delete []table;
		if (textureWater != NULL) delete []textureWater;
		if (textureIce != NULL) delete []textureIce;
		if (line0 != NULL) delete []line0;
		if (line1 != NULL) delete []line1;
		if (line2 != NULL) delete []line2;
		if (line3 != NULL) delete []line3;
	}

	void init(int n)
	{
		pNum = 0;
		p = new Particle[PARTICLE_NUM];

		tLen = (int)(1.0f / KR) + 1;
		tSize = SQ(tLen);
		table = new Ptr[tSize];

		pos0.Set(0.2f, 0.8f);
		vel0.Set(0.8f, 0.6f);

		normalizeKernels();
		//
		for(int i = 0; i < 50; i++)
			for(int j = 0; j < 50; j++)
			{
				Grid[i][j].pos.x = i * gridSize + gridSize / 2.0;
				Grid[i][j].pos.y = j * gridSize + gridSize / 2.0;
			}
	}

	//
	void computeDT()
	{
		//float L1, L2, L3, L4;
		float up;
		float DT;
		int t = 0;
		float alpha , x, y;
		int Pice;
		for(int i = 0; i < 50; i++)
		{
			for(int j = 0; j < 50; j++)
			{
				if(!Grid[i][j].flagOfData)
				{
					t = 1;
					up = Grid[i][j].pos.y + gridSize / 2.0f ;
					break;
				}
			}
			if(t == 1)break;
		}
		
		//DT for every grid
		for(int i = 0; i < 50; i++)
		{
			for(int j = 0; j < 50; j++)
			{
				if(Grid[i][j].flagOfData)
				{
					//L1 = Grid[i][j].pos.x;
					//L4 = 1 - Grid[i][j].pos.y;
					//alpha
					Pice = 0;
					for (std::vector<Particle *>::iterator it = Grid[i][j].pv.begin() ; it != Grid[i][j].pv.end(); ++it)
						if((*it)->phase == ice)
							Pice++;
					float W = Pice / Grid[i][j].pv.size();
					alpha = (1 - W) * cThermalWater + W * cThermalIce;
					x = Grid[i][j].pos.x;
					y = Grid[i][j].pos.y;
					DT = (-1) * alpha * (Tfreeze - Tair) * ( 1/pow(x,2) + 1/pow(1-x, 2) + 1/pow(y, 2) + 1/pow(up - y, 2));
					Grid[i][j].DT = DT/ 10000.0f;
					//printf("Pice: %d\n",Pice); 
					//printf("alpha: %f\n", alpha);
					//printf("DT: %f\n", Grid[i][j].DT);
				}
			}
		}

	}

	void generateParticle(void)
	{
		int i, n = 4;
		float r = 0.015625f;

		for (i = 0; i < n; i++) {
			p[pNum].pos = pos0 + r * Point2f(cos((float)i / n * D360), sin((float)i / n * D360));
			p[pNum].vel = vel0;
			p[pNum].acc.Zero();
			p[pNum].next = NULL;
			//
			p[pNum].phase = water;
			p[pNum].T = Twater;
			p[pNum].S = 0.15f;
			p[pNum].volume = 0.0f;
			pNum++;
		}
	}

	void update(void)
	{
		buildTable();
		computeDP();
		computeForce();
		integrate();
		fixBoundary();

		//generateTexture(textureWater, water, colorWater);
		//marchingSquares(textureWater, line0, line1, nLine0);

		float sumS = 0.0f;
		for (int i = 0; i < pNum; i++) {
			sumS += p[i].S;
		}
		
		if(freeze)
		{
			if(!gridBuilt)
			{
				buildGrid();
				gridBuilt = true;
			}
			//checkAir();
			computeDT();
			transferHeat();
			updateDissolvedAir();
			fixIce();

			generateTexture(textureWater, water, colorWater);
			marchingSquares(textureWater, line0, line1, nLine0);

			generateTexture(textureIce, ice, colorIce);
			marchingSquares(textureIce, line2, line3, nLine1);
		}
	}
};

#endif