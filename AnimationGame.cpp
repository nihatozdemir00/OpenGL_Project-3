
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "vec.h"

#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 600

#define  TIMER_PERIOD  16 // Period for the timer.
#define  TIMER_ON     1     // 0:disable timer, 1:enable timer

#define D2R 0.0174532
#define FIRE_SPEED 3

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height


bool activeFire = false;
bool activeFire2 = false;
bool activeSun = true;
bool activeSun2 = true;


/* User defined Types and variables for the application */

typedef struct {
	float r, g, b;
} color_t;

typedef struct {
	vec_t pos;
	float angle;
	color_t color;
} sun_t;

typedef struct {
	vec_t pos;
	float angle;
	vec_t vel;  //velocity
} fire_t;

typedef struct {
	vec_t pos;
	vec_t N;
} vertex_t;

int mousex, mousey;

sun_t S = { { -240, 0 }, 270, {1, 1, 0} };

fire_t  F;
fire_t F2;


//
// to draw circle, center at (x,y)
//  radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void print(int x, int y, const char* string, void* font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}


// display text with variables.
void vprint(int x, int y, void* font, const char* string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

void vprint2(int x, int y, float size, const char* string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

color_t mulColor(float k, color_t c) {
	color_t tmp = { k * c.r, k * c.g, k * c.b };
	return tmp;
}

color_t addColor(color_t c1, color_t c2) {
	color_t tmp = { c1.r + c2.r, c1.g + c2.g, c1.b + c2.b };
	return tmp;
}

double distanceImpact(double d) {
	return (-1.0 / 350.0) * d + 1.0;
}

color_t calculateColor(sun_t source, vertex_t v) {
	vec_t L = subV(source.pos, v.pos);
	vec_t uL = unitV(L);
	float factor = dotP(uL, v.N) * distanceImpact(magV(L));
	return mulColor(factor, source.color);
}

void backGround()
{
	// blue backround
	glColor3f(0, 0.6, 0.9);
	glBegin(GL_POLYGON);
	glVertex2f(-300, -300);
	glVertex2f(-300, 300);
	glVertex2f(300, 300);
	glVertex2f(300, -300);
	glEnd();

	// the earthplane which have different parts and slopes
	double y = -200.0;
	for (int x = -300; x <= -200; x++)
	{
		vertex_t P = { { x, -100 }, { 0, 1 } };
		color_t res = { 0.6, 0.4, 0 };

		if (activeSun2)
			res = addColor(res, calculateColor(S, P));

		glBegin(GL_LINES);
		glColor3f(res.r, res.g, res.b);
		glVertex2f(x, y);
		glVertex2f(x, -300);
		glEnd();

		if (y <= -150)
			y += 0.5;
	}
	double y1 = -150.0;
	for (int x1 = -200; x1 <= -100; x1++)
	{
		vertex_t P = { { x1, -100 }, { 0, 1 } };
		color_t res = { 0.6, 0.4, 0 };

		if (activeSun2)
			res = addColor(res, calculateColor(S, P));

		glBegin(GL_LINES);
		glColor3f(res.r, res.g, res.b);
		glVertex2f(x1, y1);
		glVertex2f(x1, -300);
		glEnd();

		if (y1 >= -200)
			y1 -= 0.5;
	}
	double y2 = -200.0;
	for (int x2 = -100; x2 <= 0; x2++)
	{
		vertex_t P = { { x2, -100 }, { 0, 1 } };
		color_t res = { 0.6, 0.4, 0 };

		if (activeSun2)
			res = addColor(res, calculateColor(S, P));

		glBegin(GL_LINES);
		glColor3f(res.r, res.g, res.b);
		glVertex2f(x2, y2);
		glVertex2f(x2, -300);
		glEnd();

		if (y2 <= -150)
			y2 += 0.5;
	}
	double y3 = -150.0;
	for (int x3 = 0; x3 <= 100; x3++)
	{
		vertex_t P = { { x3, -100 }, { 0, 1 } };
		color_t res = { 0.6, 0.4, 0 };

		if (activeSun2)
			res = addColor(res, calculateColor(S, P));

		glBegin(GL_LINES);
		glColor3f(res.r, res.g, res.b);
		glVertex2f(x3, y3);
		glVertex2f(x3, -300);
		glEnd();

		if (y3 >= -200)
			y3 -= 0.5;
	}
	double y4 = -200.0;
	for (int x4 = 100; x4 <= 200; x4++)
	{
		vertex_t P = { { x4, -100 }, { 0, 1 } };
		color_t res = { 0.6, 0.4, 0 };

		if (activeSun2)
			res = addColor(res, calculateColor(S, P));

		glBegin(GL_LINES);
		glColor3f(res.r, res.g, res.b);
		glVertex2f(x4, y4);
		glVertex2f(x4, -300);
		glEnd();

		if (y4 <= -150)
			y4 += 0.5;
	}
	double y5 = -150.0;
	for (int x5 = 200; x5 <= 300; x5++)
	{
		vertex_t P = { { x5, -100 }, { 0, 1 } };
		color_t res = { 0.6, 0.4, 0 };

		if (activeSun2)
			res = addColor(res, calculateColor(S, P));

		glBegin(GL_LINES);
		glColor3f(res.r, res.g, res.b);
		glVertex2f(x5, y5);
		glVertex2f(x5, -300);
		glEnd();

		if (y5 >= -200)
			y5 -= 0.5;
	}

	// the bottom bar which contains information about how to play
	glColor3f(0, 0, 0);
	glBegin(GL_QUADS);
	glVertex2f(-300, -300);
	glVertex2f(-300, -250);
	glVertex2f(300, -250);
	glVertex2f(300, -300);
	glEnd();

	// the frame for current location of mouse
	glLineWidth(3);
	glColor3f(1, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(190, -295);
	glVertex2f(300, -295);
	glVertex2f(300, -278);
	glVertex2f(190, -278);
	glEnd();

	// information about how to play and current location of mouse
	glColor3f(1, 1, 1);
	vprint(205, -290, GLUT_BITMAP_8_BY_13, "[%d,%d]", mousex, mousey);
	vprint(-280, -270, GLUT_BITMAP_8_BY_13, "[Click left to create red bullet, Click right to create green bullet]");
	vprint(-280, -290, GLUT_BITMAP_8_BY_13, "[Press <F1> to start again after the sun was dissappeared]");
}

// light source
void drawSun(sun_t p)
{
	if (activeSun2)
	{
		glColor3f(S.color.r, S.color.g, S.color.b);
		circle(p.pos.x, p.pos.y, 30);
	}
}

// red flying object
void drawFire(fire_t e)
{
	if (activeFire && activeSun)
	{
		glColor3f(1, 0, 0);
		circle(e.pos.x, e.pos.y, 10);

		glColor3f(0, 0, 0);
		circle_wire(e.pos.x, e.pos.y, 11);
		circle_wire(e.pos.x, e.pos.y, 7);
		circle_wire(e.pos.x, e.pos.y, 3);
	}
}

// green flying object
void drawFire2(fire_t e)
{
	if (activeFire2 && activeSun2)
	{
		glColor3f(0.1, 1, 0);
		circle(e.pos.x, e.pos.y, 10);

		glColor3f(0, 0, 0);
		circle_wire(e.pos.x, e.pos.y, 11);
		circle_wire(e.pos.x, e.pos.y, 7);
		circle_wire(e.pos.x, e.pos.y, 3);
	}
}

void display()
{
	//
	// clear window to black
	//
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	backGround();

	drawSun(S);
	drawFire(F);
	drawFire2(F2);

	glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}
//
// Special Key like GLUT_KEY_F1, F2, F3,...https://i.scdn.co/image/ab67616600001e01ad61f5a3393cb55511eda886
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	// Write your codes here.

	// to start again
	if (key == GLUT_KEY_F1)
		activeSun2 = true;


	switch (key) {
	case GLUT_KEY_UP: up = true; break;
	case GLUT_KEY_DOWN: down = true; break;
	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;

	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}


//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;

	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}


//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
	// Write your codes here.

	// to create red flying object
	if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN)
	{
		activeFire = !activeFire;
		F.pos.x = mousex;
		F.pos.y = mousey;
		activeSun = true;
	}

	// to create green flying object
	if (button == GLUT_RIGHT_BUTTON && stat == GLUT_DOWN)
	{
		activeFire2 = !activeFire2;
		F2.pos.x = mousex;
		F2.pos.y = mousey;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown(int x, int y) {
	// Write your codes here.



	 // to refresh the window it calls display() function	
	glutPostRedisplay();
}

void onMove(int x, int y) {
	// Write your codes here.

	// to get current location of mouse
	mousex = x - winWidth / 2;
	mousey = winHeight / 2 - y;


	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.

	// to move sun from left side to right side of the screen
	S.pos.y = 150;

	if (S.pos.x + 40 < 300)
		S.pos.x += 1.5;
	else
		S.pos.x = -300;


	
	// velocity vector from fire to sun
	F.vel = mulV(FIRE_SPEED, unitV(subV(S.pos, F.pos)));
	F2.vel = mulV(FIRE_SPEED, unitV(subV(S.pos, F2.pos)));

	vec_t prevPos = F.pos;
	vec_t prevPos2 = F2.pos;

	//chase
	if (activeFire)
	{
		F.pos = addV(F.pos, F.vel); 
	}

	if (activeFire2)
	{
		F2.pos = addV(F2.pos, F2.vel);
	}

	// to keep fire in the borders
	if (F.pos.x > 310 || F.pos.x < -310 || F.pos.y > 310 || F.pos.y < -310)
	{
		F.pos = prevPos;

	}

	if (F2.pos.x > 310 || F2.pos.x < -310 || F2.pos.y > 310 || F2.pos.y < -310)
	{
		F2.pos = prevPos2;
	}

	// to check whether the bullet hits to sun or not
	if (F.pos.x >= S.pos.x - 15 && F.pos.x <= S.pos.x + 15 && F.pos.y <= 180 && F.pos.y >= 120)
	{
		activeSun = false;
		activeFire = false;
		F.pos = { 400, 400 };
	}

	if (F2.pos.x >= S.pos.x - 15 && F2.pos.x <= S.pos.x + 15 && F2.pos.y <= 180 && F2.pos.y >= 120)
	{
		activeSun = false;
		activeFire2 = false;
		F2.pos = { 400, 400 };
		activeSun2 = false;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}


void main(int argc, char* argv[])
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Bullets Chasing The Sun by Nihat Özdemir");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);
	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	glutMainLoop();
}
