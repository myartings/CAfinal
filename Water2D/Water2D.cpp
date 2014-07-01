#include "Water2D.h"
#pragma comment (lib, "glew32.lib")

void initGL(void)
{
	printf("Vendor:   %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version:  %s\n", glGetString(GL_VERSION));
	printf("--------------------------------------------------\n");

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenTextures(1, &waterTex);
	glBindTexture(GL_TEXTURE_2D, waterTex); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, RENDER_SAMPLE, RENDER_SAMPLE, 0, GL_RGBA, GL_FLOAT, 0);

	//
	glGenTextures(1, &iceTex);
	glBindTexture(GL_TEXTURE_2D, iceTex); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, RENDER_SAMPLE, RENDER_SAMPLE, 0, GL_RGBA, GL_FLOAT, 0);

	screenData = new unsigned long[windowWidth * windowHeight];
	frameNum = 0;

	glewInit();
	if (!glewIsSupported("GL_VERSION_2_0 GL_VERSION_1_5 GL_ARB_multitexture GL_ARB_vertex_buffer_object")) {
		fprintf(stderr, "Required OpenGL extensions missing.");
		exit(-1);
	}

	setUpShader();
}

//set up shader
void setUpShader()
{
	GLchar *vertexData;
	GLchar *fragmentData;
	int size;
	size = ShaderSize(vertexFile);
	vertexData = ReadShaderSource(vertexFile, size);
	size = ShaderSize(fragmentFile);
	fragmentData = ReadShaderSource(fragmentFile, size);

	InstallShaders(programObject, vertexData, fragmentData);

}



void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	windowWidth = width; windowHeight = height;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 1.0, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
}

void initSim(void)
{
	ps.init(32);
}

void iteration(void)
{
	if (ps.pNum < PARTICLE_NUM) ps.generateParticle();
	ps.update();
}

void idle(void)
{
	fps.update(120.0);
	if (systemRunning) {
		iteration();
	}
	glutPostRedisplay();
}

void renderText(void)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, windowWidth, windowHeight, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(0.75f, 0.75f, 0.75f);

	char buffer[256];

	sprintf_s(buffer, 256, "FPS: %.1f", fps.avgHz());
	glRasterPos2i(5, 20);
	for (char *s = buffer; *s != '\0'; s++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *s);

	//sprintf_s(buffer, 256, "Frame Number: %d", frameNum);
	sprintf_s(buffer, 256, "Press 'B' to begin freeze(when water is stable), 'F' to change mode \n");
	glRasterPos2i(5, 40);
	for (char *s = buffer; *s != '\0'; s++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *s);

	if (!systemRunning) {
		sprintf_s(buffer, 256, "PAUSED");
		glRasterPos2i(5, 60);
		for (char *s = buffer; *s != '\0'; s++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *s);
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void screenShot(int n)
{
	Bitmap b;
	char filename[] = "Screen_Shots/000000.bmp";
	int i = 18;

	while (n > 0) {
		filename[i--] += n % 10;
		n /= 10;
	}

	glReadPixels(0, 0, windowWidth, windowHeight, GL_RGBA, GL_UNSIGNED_BYTE, screenData);
	b.makeBMP(screenData, windowWidth, windowHeight);
	b.Save(filename);
}

void display(void)
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(1.0f, 1.0f, 1.0f);

	if (ps.renderMode == 0) {
		int i;

		glColor3f(0.3f, 0.8f, 1.0f);
		glEnable(GL_BLEND);
		glEnable(GL_POINT_SMOOTH);
		glPointSize(5.0f);
		glBegin(GL_POINTS);
		for (i = 0; i < ps.pNum; i++)
		{
			//
			if(ps.p[i].phase == bubble)
				glColor3f(1.0f, 0.3f, 0.8f);
			else if(ps.p[i].phase == water)
				glColor3f(0.5f, 0.8f, 1.0f);
			else
				glColor3f(0.3f, 1.0f, 0.8f);
			glVertex2fv(&ps.p[i].pos.x);
		}
		glEnd();
		glPointSize(1.0f);
		glDisable(GL_POINT_SMOOTH);
		glDisable(GL_BLEND);
	}
	else if (ps.renderMode > 0) {
		//glBindTexture(GL_TEXTURE_2D, waterTex);
		//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, RENDER_SAMPLE, RENDER_SAMPLE, GL_RGBA, GL_FLOAT, ps.texture);
		//glEnable(GL_TEXTURE_2D);
		//glBegin(GL_QUADS);
		//glTexCoord2f(0.0f, 0.0f);
		//glVertex2f(0.0f, 0.0f);
		//glTexCoord2f(1.0f, 0.0f);
		//glVertex2f(1.0f, 0.0f);
		//glTexCoord2f(1.0f, 1.0f);
		//glVertex2f(1.0f, 1.0f);
		//glTexCoord2f(0.0f, 1.0f);
		//glVertex2f(0.0f, 1.0f);
		//glEnd();
		//glDisable(GL_TEXTURE_2D);

		if (ps.renderMode == 2) {
			int i;

			glColor3f(0.3f, 0.8f, 1.0f);
			glEnable(GL_BLEND);
			glEnable(GL_LINE_SMOOTH);
			glLineWidth(1.5f);
			glBegin(GL_LINES);
			for (i = 0; i < ps.nLine0; i++) {
				glVertex2fv(&ps.line0[i].x);
				glVertex2fv(&ps.line1[i].x);
			}
			glEnd();
			glColor3f(1.0f, 0.8f, 0.3f);
			glBegin(GL_LINES);
			for (i = 0; i < ps.nLine1; i++) {
				glVertex2fv(&ps.line2[i].x);
				glVertex2fv(&ps.line3[i].x);
			}
			glEnd();
			glLineWidth(1.0f);
			glDisable(GL_LINE_SMOOTH);
			glDisable(GL_BLEND);
		}

		//
		if (ps.renderMode == 1) {
			//int i;

			//glColor3f(0.3f, 0.8f, 1.0f);
			//glEnable(GL_BLEND);
			//glEnable(GL_POINT_SMOOTH);
			//glPointSize(5.0f);
			//glBegin(GL_POINTS);
			//for (i = 0; i < ps.pNum; i++)
			//	glVertex2fv(&ps.p[i].pos.x);
			//glEnd();
			//glPointSize(1.0f);
			//glDisable(GL_POINT_SMOOTH);

			////draw contour
			//glLineWidth(1.5f);
			//glBegin(GL_LINES);
			//for (i = 0; i < ps.nLine; i++) {
			//	glVertex2fv(&ps.line0[i].x);
			//	glVertex2fv(&ps.line1[i].x);
			//}
			//glEnd();
			//glLineWidth(1.0f);
			//glDisable(GL_LINE_SMOOTH);

			//glDisable(GL_BLEND);
			//glEnable(GL_TEXTURE_2D);
			glUseProgram(programObject);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, waterTex);	
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, RENDER_SAMPLE, RENDER_SAMPLE, GL_RGBA, GL_FLOAT, ps.textureWater);
			waterLoc = glGetUniformLocation(programObject, "tex_water");
			if(waterLoc >= 0)
				glUniform1i(waterLoc, 0);
			else
				printf("Uniform variable tex_water not found!\n");
			
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, iceTex);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, RENDER_SAMPLE, RENDER_SAMPLE, GL_RGBA, GL_FLOAT, ps.textureIce);
			iceLoc = glGetUniformLocation(programObject, "tex_ice");
			if(iceLoc >= 0)
				glUniform1i(iceLoc, 1);
			else
				printf("Uniform variable tex_ice not found!\n");

			glEnable(GL_TEXTURE_2D);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(1.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(1.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(0.0f, 1.0f);
			glEnd();
			//glDisable(GL_TEXTURE_2D);
			glDisable(GL_TEXTURE_2D);
			glActiveTexture(GL_TEXTURE0);
			glUseProgram(0);

			/*glBindTexture(GL_TEXTURE_2D, iceTex);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, RENDER_SAMPLE, RENDER_SAMPLE, GL_RGBA, GL_FLOAT, ps.textureIce);
			glEnable(GL_TEXTURE_2D);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(1.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(1.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(0.0f, 1.0f);
			glEnd();
			glDisable(GL_TEXTURE_2D);*/

			//glColor3f(0.3f, 0.8f, 1.0f);
			glEnable(GL_BLEND);
			glEnable(GL_POINT_SMOOTH);
			glPointSize(10.0f);
			//glBegin(GL_POINTS);
			int i;
			for (i = 0; i < ps.pNum; i++)
			{
				//
				if(ps.p[i].phase == bubble)
				{
					//glPointSize(10.0f * ps.p[i].volume);
					glColor3f(1.0f, 1.0f, 1.0f);
					DrawCircle(ps.p[i].pos.x, ps.p[i].pos.y, 0.005 * ps.p[i].volume, 10);
					//glVertex2fv(&ps.p[i].pos.x);
				}
			}
			//glEnd();
			glPointSize(1.0f);
			glDisable(GL_POINT_SMOOTH);
			glDisable(GL_BLEND);
	}

	}

	if (systemRunning) {
		//if (frameNum % 10 == 0)
			//screenShot(frameNum / 10);
		frameNum++;
	}

	renderText();

	glutSwapBuffers();
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
		}
		else if (state == GLUT_UP) {
		}
	}	
	else if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
		}
		else if (state == GLUT_UP) {
		}
	}
	else if (button == GLUT_MIDDLE_BUTTON) {
		if (state == GLUT_DOWN) {
		}
		else if (state == GLUT_UP) {
		}
	}
}

void motion(int x, int y)
{
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case 27:
			exit(0);
			break;
		case 32:
			systemRunning = !systemRunning;
			break;
		case 'q':
		case 'Q':
			testSwitch = !testSwitch;
			glutPostRedisplay();
			break;
		case 'f':
		case 'F':
			ps.renderMode = (ps.renderMode + 1) % 3;
			glutPostRedisplay();
			break;
		case 'b':
		case 'B':
			freeze = true;
		default:
			break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow("Water Simulation 2D");

	initGL();
	initSim();

	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(keyboard);

	fps.init();

	glutMainLoop();

	return 0;
}


int ShaderSize(const char *fileName )
{
	FILE *fp = fopen(fileName,"rt");
	int count;
	if (!fp)
		MessageBox(NULL,"open shader file error","´íÎó",MB_OK);
	rewind(fp);
	fseek(fp,0,SEEK_END);
	count = ftell(fp);
	rewind(fp);
	fclose(fp);
	return count;
}

GLchar* ReadShaderSource(const char *fileName, int len)
{
	FILE *fp = fopen(fileName,"rt");
	if (!fp)
		MessageBox(NULL,"read shader file error","´íÎó",MB_OK);
	GLchar *shader;
	shader = (GLchar *)malloc(sizeof(char)*(len+1));
	memset(shader,0,len+1);
	fseek(fp,0,SEEK_SET);
	fread(shader,sizeof(char),len,fp);
	fclose(fp);
	return shader;
}

int InstallShaders( GLuint &programObj,GLchar *Vertex, GLchar *Fragement )
{
	GLint vertCompiled,fragCompiled;
	GLint linked;

	//create
	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	fragementShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	
	//load
	glShaderSource(vertexShaderObject,1,(const char**)&Vertex,NULL);
	glShaderSource(fragementShaderObject,1,(const char**)&Fragement,NULL);
	
	//compile
	glCompileShader(vertexShaderObject);
	glGetShaderiv(vertexShaderObject,GL_COMPILE_STATUS,&vertCompiled);
	glCompileShader(fragementShaderObject);
	glGetShaderiv(fragementShaderObject,GL_COMPILE_STATUS,&fragCompiled);

	if (!vertCompiled || !fragCompiled)
		MessageBox(NULL,"shader compile failed","´íÎó",MB_OK);
    
	//create a program
	programObj = glCreateProgram();
	glAttachShader(programObj,vertexShaderObject);
	glAttachShader(programObj,fragementShaderObject);

	//link program
	glLinkProgram(programObj);

	glDeleteShader(vertexShaderObject);
	glDeleteShader(fragementShaderObject);  

	glGetProgramiv(programObj,GL_LINK_STATUS,&linked);
	if (!linked)
		MessageBox(NULL,"link program failed","´íÎó",MB_OK);

	//get the compile fail info
	PrintShaderCompileInfo();
	return TRUE;
}

void PrintShaderCompileInfo()
{
	int infoLogLength;
	int charswritten;
	char *infolog;

	glGetShaderiv(vertexShaderObject,GL_INFO_LOG_LENGTH,&infoLogLength);
	infolog = (char *)malloc(infoLogLength);
	glGetShaderInfoLog(vertexShaderObject,infoLogLength,&charswritten,infolog);

	//printf("%s\n",infolog);

	glGetShaderiv(fragementShaderObject,GL_INFO_LOG_LENGTH,&infoLogLength);
	infolog = (char *)malloc(infoLogLength);
	glGetShaderInfoLog(fragementShaderObject,infoLogLength,&charswritten,infolog);
	//printf("%s\n",infolog);
}

void DrawCircle(float x1, float y1, float r, int num_segments)
{
	//filled circle
	float x2,y2;
	float angle;

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(x1,y1);
 
	for (angle=1.0f;angle<361.0f;angle+=0.2)
	{
		x2 = x1+sin(angle)*r;
		y2 = y1+cos(angle)*r;
		glVertex2f(x2,y2);
	}
 
	glEnd();
}