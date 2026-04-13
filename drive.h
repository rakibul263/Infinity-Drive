#ifndef DRIVE_H
#define DRIVE_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
  float yf, r, g, b;
} Stop;

extern int W, H;
extern int scene, N_SCENES;
extern float sceneTimer, SCENE_DUR;
extern float fadeA;
extern int fadIn, fadOut;
extern float T;
extern float c1a, c1b, c1p, c2p, c2sx, c2sy, c3m, c3p, c4p, c4sw;
extern float bx[3], by2[3];
extern float c5w, c5b1, c5b2, c5p, c5beam;
extern float stX[120], stY[120], stS[120], stTw[120];
extern int stInit;

void initStars(void);
void dda(float x1, float y1, float x2, float y2);
void mca(float cx, float cy, float r);
void fc(float cx, float cy, float r);
void col3(float r, float g, float b);
float cl(float v);
float lp(float a, float b, float t);
void gradBand(float x, float y, float w, float h,
              float r0, float g0, float b0, float r1, float g1, float b1);
void skyGrad(Stop *stops, int n);
void glow(float cx, float cy, float r, float R, float gr, float gg, float gb);
void fillRect(float x, float y, float w, float h);
void drawStars(float brightness);
void drawMoon(float x, float y, float skyr, float skyg, float skyb);
void drawPerson(float x, float y, float shR, float shG, float shB);

void scene1(void);
void scene2(void);
void scene3(void);
void scene4(void);
void scene5(void);

void initSmoke(void);

void switchScene(int n);
void display(void);
void timerCB(int v);
void keyboard(unsigned char key, int x, int y);
void reshape(int w, int h);

/* Add this to your drive.h file */
void drawCar(float x, float y);

#endif
