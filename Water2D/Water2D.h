#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gl\glew.h>
#include <gl\glut.h>
#include "const.h"
#include "SPH.h"
#include "Point.h"
#include "Timer.h"
#include "bitmap.h"
#include <gl\GLAux.h>


int windowWidth = 512, windowHeight = 512;

int systemRunning = 0;
int testSwitch = 0;
Timer fps;

SPH ps;
GLuint waterTex, iceTex, finalTex;

unsigned long *screenData;
int frameNum;

//bool fboUsed = true;
//	
//GLuint rboId;
//GLuint fbo;

const char *vertexFile = "final.vert";
const char *fragmentFile = "final.frag"; 
GLuint vertexShaderObject;
GLuint fragementShaderObject;
GLuint programObject;
GLint waterLoc;
GLint iceLoc;

int ShaderSize(const char *fileName );
GLchar* ReadShaderSource(const char *fileName, int len);
int InstallShaders( GLuint &programObj,GLchar *Vertex, GLchar *Fragement );
void PrintShaderCompileInfo();
void setUpShader();
void DrawCircle(float cx, float cy, float r, int num_segments);