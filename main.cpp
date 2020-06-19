#include <GL/glut.h>
#include "RgbImage.h"
//#include "sweettoothtruck.h"

#include <ctime>
#include <iostream>
#include <unistd.h>

//plantita
#include <vector>
#include <string>
#include <math.h>

using namespace std;

float camaraX = 0;
float camaraY = 10;
float camaraZ = 140;

float posLuzX = 0;
float posLuzY = 80;//70
float posLuzZ = 100;

float posLuz1X = 0;
float posLuz1Y = 80;
float posLuz1Z = 0;

float angulo = 0;

GLuint texture[3];

/*para el terreno*/
int cols,rows;
int scl=20;
float terrain[40][40];
float angle=160/4;

/*para las plantitas*/
const float PI = 3.14, DEPTH = 6;
// Start and end of camera movement
const int ENDX = 10, STARTX = -500;
// Angle of branches, and depth of tree
float ANGLE = 20, depth = 0;
vector<string> *trees = new vector<string>();


double lastTime = 0, elapsedTime = 0, lastElapsedTime = 0;

bool cam = false;

float eyeX, eyeY, eyeZ, fieldOfView, length = 0.001, num = 0,
	incr = 0.1;

float lineWidth = 5;
// L-System
string str = "X";


void loadTextureFromFile(const char* filename, int index){
	glShadeModel(GL_FLAT);
	RgbImage theTexMap(filename);
	glGenTextures(1, &texture[index]);
	glBindTexture(GL_TEXTURE_2D, texture[index]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, theTexMap.GetNumCols(), theTexMap.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE, theTexMap.ImageData());
	
}

void cargarImagenes() {
	loadTextureFromFile("texturas/lapida.bmp", 0);
	loadTextureFromFile("texturas/tnt.bmp", 1);
	loadTextureFromFile("texturas/piedra.bmp", 2);
}

void iniciarVentana(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80.0, (float)w / (float)h, 1, 100);
	
	/*para el terreno*/
	cols=w/scl;
	rows=h/scl;
	
	/*srand(getpid());
	for(int y=0;y<rows;y++){	
		for(int x=0;x<cols;x++){
			terrain[cols][rows]=rand () % (10+10+1) -10;
		}
	}*/
}

void iniciarLuces() {
	GLfloat light_ambient[] = { 0.2,0.2,0.2,1 };
	GLfloat light_diffuse[] = { 0.8,0.8,0.8,1 };
	GLfloat light_specular[] = { 0.2,0.2,0.2,1 };
	
	float reflejo[] = { 0,0,0,1 };
	float posicionLuz[] = { posLuzX,posLuzY,posLuzZ,1 };
	int sombra = 128;
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, reflejo);
	glMateriali(GL_FRONT, GL_SHININESS, sombra);
	glLightfv(GL_LIGHT0, GL_POSITION, posicionLuz);		
	
	//Luz 1
	GLfloat light_ambient1[] = { 0.6,0,0,1 };
	GLfloat light_diffuse1[] = { 0.8,0.8,0.8,1 };
	GLfloat light_specular1[] = { 0.5,0.5,0.5,1 };
	float posicionLuz1[] = { posLuz1X,posLuz1Y,posLuz1Z,1 };
	
	//glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);
	glLightfv(GL_LIGHT1, GL_POSITION, posicionLuz1);
	
	//Luz 2 - Spot
	GLfloat colorSpot[] = { 1,1,1,1 };
	float posicionLuz2[] = { 0,0,0,1 };
	float spot_direccion[] = {0,0,-1};
	//glEnable(GL_LIGHT2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, colorSpot);
	glLightfv(GL_LIGHT2, GL_SPECULAR, colorSpot);
	glLightfv(GL_LIGHT2, GL_POSITION, posicionLuz2);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 50);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direccion);
	/*glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 1);
	glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1);*/
	
}

void timer(int t){
	glutPostRedisplay();
	glutTimerFunc(20, timer, 0);
}


/*ESCENARIO INICIO*/
void cielo(){
	srand(getpid());
	int random;
	glLineWidth(1);
	for(int y=0;y<rows+100;y++){	
		for(int x=0;x<cols+50;x++){
			random=rand () % (10+10+1) -10;
			glBegin(GL_LINES);
			glVertex3f(x*scl, y*scl, random);
			glVertex3f(x*scl, (y+1)*scl, random);
			glEnd();
		}
	}
}
	
void mar_sangre(){
	srand(getpid());
	int random;
	//glColor3ub(160,6,11);
	glLineWidth(1);
	glBegin(GL_TRIANGLE_STRIP);
	for(int y=0;y<40;y++){	
		for(int x=0;x<3;x++){
			random=rand () % (20+20+1) -20;
			glVertex3f(x*scl, y*scl, terrain[x][y]);
			glVertex3f(x*scl, (y+1)*scl, terrain[x][y]);
		}
	}
	glEnd();
}
	
void rio(){
	srand(getpid());
	int random;
	//glColor3ub(160,6,11);
	glLineWidth(1);
	glBegin(GL_TRIANGLE_STRIP);
	for(int y=0;y<40;y++){	
		for(int x=0;x<8;x++){
			random=rand () % (20+20+1) -20;
			glVertex3f(x*scl, y*scl, terrain[x][y]);
			glVertex3f(x*scl, (y+1)*scl, terrain[x][y]);
		}
	}
	glEnd();
}
	
void arbol2D(int len, int width){
	glLineWidth(width);
	glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3f(0, len, 0);
	glEnd();
	glTranslatef(0,len,0);
	if(len>2){
		glPushMatrix();
		glRotated(angle,0,0,1);
		arbol2D(len*0.67,width);
		glPopMatrix();
		glPushMatrix();
		glRotated(-angle,0,0,1);
		arbol2D(len*0.67,width);
		glPopMatrix();
	}
}

void arbol(int width){
	glPushMatrix();
		glTranslatef(-5,-10,5);
		arbol2D(20,width);
	glPopMatrix();
	for(int i=0;i<6;i++){
		glPushMatrix();
			glTranslatef(-5,-10,5);
			glRotated(30*i,0,1,0);
			arbol2D(20,width);
		glPopMatrix();
	}
}

void bosque(){
	/*-----------atras*/
	for(int i=1;i<16;i++){
		glPushMatrix();
		glColor3ub(124,132,151);
		glTranslatef(-40*i,-60,-1500);
		arbol(3);
		glPopMatrix();
		glPushMatrix();
		glColor3ub(124,132,151);
		glTranslatef(40*i,-65,-1500);
		glRotatef(180,0,1,0);
		arbol(3);
		glPopMatrix();
	}
	
	glColor3ub(109,105,75);
	for(int i=1;i<16;i++){
		glPushMatrix();
		glTranslatef(-40*i,-60,-1100);
		arbol(3);
		glPopMatrix();
		
		glPushMatrix();
		glTranslatef(40*i,-65,-1100);
		glRotatef(180,0,1,0);
		arbol(3);
		glPopMatrix();
		
		glPushMatrix();
		glTranslatef(-40*i,-60,-900);
		arbol(3);
		glPopMatrix();
		
		glPushMatrix();
		glTranslatef(40*i,-65,-900);
		glRotatef(180,0,1,0);
		arbol(3);
		glPopMatrix();
	}
	
	for(int i=1;i<16;i++){
		glPushMatrix();
		glTranslatef(-40*i,-60,-700);
		arbol(2);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(40*i,-65,-700);
		glRotatef(180,0,1,0);
		arbol(2);
		glPopMatrix();
	}
	
	/*------------------adelante*/
	glColor3ub(21,49,89);
	
	glPushMatrix();
	glTranslatef(-100,-50,30);
	glScalef(2,2,2);
	arbol(20);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(110,-50,25);
	glScalef(2,2.5,2);
	arbol(20);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-180,-50,-60);
	glScalef(2,2,2);
	arbol(20);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(300,-50,-120);
	glScalef(2,2,2);
	arbol(20);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-180,-50,-200);
	glScalef(1.5,1.5,1.5);
	arbol(20);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(180,-50,-170);
	glScalef(1.5,1.5,1.5);
	arbol(20);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-200,-50,-300);
	glScalef(1,1,1);
	arbol(20);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(200,-50,-270);
	glScalef(1,1,1);
	arbol(20);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-200,-50,-400);
	glScalef(0.8,0.8,0.8);
	arbol(20);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(220,-50,-390);
	glScalef(0.7,0.7,0.7);
	arbol(20);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(120,-50,-280);
	glScalef(0.8,0.8,0.8);
	arbol(20);
	glPopMatrix();
}


void caja(){
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_QUADS);
	//tapa frontal
	glTexCoord2f(0, 0); glVertex3f(-2, 2, 2);
	glTexCoord2f(1, 0); glVertex3f(2, 2, 2);
	glTexCoord2f(1, 1); glVertex3f(2, -2, 2);
	glTexCoord2f(0, 1); glVertex3f(-2, -2, 2);
	
	//tapa trasera
	glTexCoord2f(0, 0); glVertex3f(-2, 2, -2);
	glTexCoord2f(1, 0); glVertex3f(2, 2, -2);
	glTexCoord2f(1, 1); glVertex3f(2, -2, -2);
	glTexCoord2f(0, 1); glVertex3f(-2, -2, -2);
	
	//tapa derecha
	glTexCoord2f(0, 0); glVertex3f(2, 2, 2);
	glTexCoord2f(1, 0); glVertex3f(2, -2, 2);
	glTexCoord2f(1, 1); glVertex3f(2, -2, -2);
	glTexCoord2f(0, 1); glVertex3f(2, 2, -2);
	
	//tapa izquierda
	glTexCoord2f(0, 0); glVertex3f(-2, 2, 2);
	glTexCoord2f(1, 0); glVertex3f(-2, -2, 2);
	glTexCoord2f(1, 1); glVertex3f(-2, -2, -2);
	glTexCoord2f(0, 1); glVertex3f(-2, 2, -2);
	
	//tapa arriba
	glTexCoord2f(0, 0); glVertex3f(-2, 2, 2);
	glTexCoord2f(1, 0); glVertex3f(2, 2, 2);
	glTexCoord2f(1, 1); glVertex3f(2, 2, -2);
	glTexCoord2f(0, 1); glVertex3f(-2, 2, -2);
	
	//tapa abajo
	glTexCoord2f(0, 0); glVertex3f(-2, -2, 2);
	glTexCoord2f(1, 0); glVertex3f(2, -2, 2);
	glTexCoord2f(1, 1); glVertex3f(2, -2, -2);
	glTexCoord2f(0, 1); glVertex3f(-2, -2, -2);
	
	glEnd();
	glPopMatrix();
	
	glDisable(GL_TEXTURE_2D);
	glFlush();
}
	
void arsenalTNT(int tamano){
	//base
	glPushMatrix();
	glScaled(tamano,tamano,tamano);
	glRotated(90,1,0,0);
	caja();
	glPopMatrix();
	
	glPushMatrix();
	glScaled(tamano,tamano,tamano);
	glTranslatef(6.5,0,0);
	glRotated(90,1,0,0);
	caja();
	glPopMatrix();
	
	glPushMatrix();
	glScaled(tamano,tamano,tamano);
	glTranslatef(-6.5,0,0);
	glRotated(90,1,0,0);
	caja();
	glPopMatrix();
	
	//segundo nivel
	glPushMatrix();
	glScaled(tamano,tamano,tamano);
	glTranslatef(3,4,0);
	glRotated(90,1,0,0);
	caja();
	glPopMatrix();
	
	glPushMatrix();
	glScaled(tamano,tamano,tamano);
	glTranslatef(-3,4,0);
	glRotated(90,1,0,0);
	caja();
	glPopMatrix();
}
	
void push(){
	glPushMatrix();
	if (lineWidth > 0)
		lineWidth -= 1;
}
		
void pop(){
	glPopMatrix();
	lineWidth += 1;
}
			
void rotL(){
	glRotatef(ANGLE, 1, 0, 0);
	glRotatef(ANGLE*4, 0, 1, 0);
	glRotatef(ANGLE, 0, 0, 1);
}
	
void rotR(){
	glRotatef(-ANGLE, 1, 0, 0);
	glRotatef(ANGLE*4, 0, 1, 0);
	glRotatef(-ANGLE, 0, 0, 1);
}
	
void leaf(){
	glPushAttrib(GL_LIGHTING_BIT);//saves current lighting stuff

	glBegin(GL_TRIANGLES);
	glVertex3f(0, 0, 0);
	glVertex3f(0.2, 0, 0.3);
	glVertex3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(-0.2, 0, -0.3);
	glVertex3f(0, 1, 0);
	glEnd();
	glPopAttrib();
}

void drawLine(){
	glPushAttrib(GL_LIGHTING_BIT);//saves current lighting stuff
	
	//glColor3f(0.55, 0.27, 0.07);
	GLfloat ambient[4] = {0.55, 0.27, 0.07};    // ambient reflection
	GLfloat specular[4] = {0.55, 0.27, 0.07};   // specular reflection
	GLfloat diffuse[4] = {0.55, 0.27, 0.07};   // diffuse reflection
	
	// set the ambient reflection for the object
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	// set the diffuse reflection for the object
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	// set the specular reflection for the object      
	//glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glLineWidth(lineWidth);
	
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, length, 0);
	glEnd();
	
	glTranslatef(0, length, 0);
	glPopAttrib();
}
	
void draw(){
	string ch = "";
	string LSystem = trees->at(depth);
	for (int i = 0; i < LSystem.length(); i++){
		ch = LSystem.at(i);
		
		if (ch.compare("D") == 0 || ch.compare("X") == 0){
			drawLine();
		} else if (ch.compare("[") == 0){
			push();
		} else if (ch.compare("]") == 0){
			pop();
		} else if (ch.compare("V") == 0){
			leaf();
		} else if (ch.compare("R") == 0){
			rotR();
		} else if (ch.compare("L") == 0){
			rotL();
		}
	}
}

void expand(float num){
	string ch = "";
	
	for (int i = 0; i < str.length(); i++){
		ch = str.at(i);
		
		if (ch.compare("D") == 0){
			str.replace(i, 1, "DD");
			i = i + 1;
		} else if (ch.compare("X") == 0){
			
			if (num < 0.4){
				//LSystem.replace(i, 1, "D[LX]D[RX]LX");
				str.replace(i, 1, "D[LXV]D[RXV]LX");
				
			} else {
				//LSystem.replace(i, 1, "D[RX]D[LX]RX");
				str.replace(i, 1, "D[RXV]D[LXV]RX");
			}
			i = i + 13;	//11
		} 
		
	} 
	trees->push_back(str);
}
	
void pasto(){
	
	glColor3ub(187,149,134);
	//izquierda
	glPushMatrix();
	glScaled(100,7,100);
	glTranslatef(-1.5,-11,0);
	draw();
	glPopMatrix();
	
	glPushMatrix();
	glScaled(60,5,50);
	glTranslatef(-1.5,-15,-2);
	draw();
	glPopMatrix();
	
	glPushMatrix();
	glScaled(60,5,50);
	glTranslatef(-2,-15,-3);
	draw();
	glPopMatrix();
	
	glPushMatrix();
	glScaled(100,6,100);
	glTranslatef(-1.5,-15,-3.5);
	draw();
	glPopMatrix();
	
	glPushMatrix();
	glScaled(50,5,50);
	glTranslatef(-1.5,-15,-7);
	draw();
	glPopMatrix();
	
	glPushMatrix();
	glScaled(50,5,50);
	glTranslatef(-1.1,-15,-9);
	draw();
	glPopMatrix();
	
	glPushMatrix();
	glScaled(50,5,50);
	glTranslatef(-1.6,-15,-11);
	draw();
	glPopMatrix();
	
	glPushMatrix();
	glScaled(50,5,50);
	glTranslatef(-1.6,-15,-13);
	draw();
	glPopMatrix();
	
	//despues de bordear
	
	glPushMatrix();
	glScaled(100,6,100);
	glTranslatef(-2,-15,-6);
	draw();
	glPopMatrix();
	
	
	//derecha
	glPushMatrix();
	glScaled(60,5,50);
	glTranslatef(1.8,-15,1);
	draw();
	glPopMatrix();
	
	glPushMatrix();
	glScaled(60,5,50);
	glTranslatef(1.8,-15,-1);
	draw();
	glPopMatrix();
	
	glPushMatrix();
	glScaled(60,5,50);
	glTranslatef(1.5,-15,-3);
	draw();
	glPopMatrix();
	
	glPushMatrix();
	glScaled(50,5,50);
	glTranslatef(1.5,-15,-5);
	draw();
	glPopMatrix();
	
	glPushMatrix();
	glScaled(50,5,50);
	glTranslatef(2.5,-15,-4);
	draw();
	glPopMatrix();
	
	glPushMatrix();
	glScaled(56,5,50);
	glTranslatef(1,-15,-7);
	draw();
	glPopMatrix();
	
	glPushMatrix();
	glScaled(56,5,50);
	glTranslatef(3,-15,-9);
	draw();
	glPopMatrix();
	
	glPushMatrix();
	glScaled(56,5,50);
	glTranslatef(1.2,-15,-11);
	draw();
	glPopMatrix();
	
	glPushMatrix();
	glScaled(56,5,50);
	glTranslatef(1.15,-15,-13);
	draw();
	glPopMatrix();
	
	//despues de bordear
	
	glPushMatrix();
	glScaled(100,6,100);
	glTranslatef(1.5,-15,-6);
	draw();
	glPopMatrix();
	
}
	

	
void lapida(){
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_QUADS);
	//tapa frontal
	glTexCoord2f(0, 0); glVertex3f(-2, 2, 2);
	glTexCoord2f(1, 0); glVertex3f(2, 2, 2);
	glTexCoord2f(1, 1); glVertex3f(2, -2, 2);
	glTexCoord2f(0, 1); glVertex3f(-2, -2, 2);
	
	glEnd();
	glPopMatrix();
	
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_QUADS);
	//tapa trasera
	glTexCoord2f(0, 0); glVertex3f(-2, 2, -2);
	glTexCoord2f(1, 0); glVertex3f(2, 2, -2);
	glTexCoord2f(1, 1); glVertex3f(2, -2, -2);
	glTexCoord2f(0, 1); glVertex3f(-2, -2, -2);
	
	glEnd();
	glPopMatrix();
	
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_QUADS);
	//tapa derecha
	glTexCoord2f(0, 0); glVertex3f(2, 2, 2);
	glTexCoord2f(1, 0); glVertex3f(2, -2, 2);
	glTexCoord2f(1, 1); glVertex3f(2, -2, -2);
	glTexCoord2f(0, 1); glVertex3f(2, 2, -2);
	
	glEnd();
	glPopMatrix();
	
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_QUADS);
	//tapa izquierda
	glTexCoord2f(0, 0); glVertex3f(-2, 2, 2);
	glTexCoord2f(1, 0); glVertex3f(-2, -2, 2);
	glTexCoord2f(1, 1); glVertex3f(-2, -2, -2);
	glTexCoord2f(0, 1); glVertex3f(-2, 2, -2);
	
	glEnd();
	glPopMatrix();
	
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glBegin(GL_QUADS);
	//tapa arriba
	glTexCoord2f(0, 0); glVertex3f(-2, 2, 2);
	glTexCoord2f(1, 0); glVertex3f(2, 2, 2);
	glTexCoord2f(1, 1); glVertex3f(2, 2, -2);
	glTexCoord2f(0, 1); glVertex3f(-2, 2, -2);
	
	glEnd();
	glPopMatrix();
	
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_QUADS);
	//tapa abajo
	glTexCoord2f(0, 0); glVertex3f(-2, -2, 2);
	glTexCoord2f(1, 0); glVertex3f(2, -2, 2);
	glTexCoord2f(1, 1); glVertex3f(2, -2, -2);
	glTexCoord2f(0, 1); glVertex3f(-2, -2, -2);
	
	glEnd();
	glPopMatrix();
	
	glDisable(GL_TEXTURE_2D);
	glFlush();
}

void escenario(){
	glColor3ub(124,132,151);

	glPushMatrix();
	glTranslatef(-1000,100,-1000);
	glRotated(90,1,0,0);
	cielo();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-1000,80,-3000);
	glRotated(90,1,0,0);
	cielo();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-1000,50,-3200);
	glRotated(90,1,0,0);
	cielo();
	glPopMatrix();
	
	glColor3ub(254,231,200);
	
	glPushMatrix();
	glTranslatef(-1000,30,-3500);
	glRotated(90,1,0,0);
	cielo();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-1000,10,-3700);
	glRotated(90,1,0,0);
	cielo();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-1000,-10,-3900);
	glRotated(90,1,0,0);
	cielo();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-1000,-30,-4100);
	glRotated(90,1,0,0);
	cielo();
	glPopMatrix();
	
	glColor3ub(124,132,151);
	
	glPushMatrix();
	glTranslatef(-1000,-50,-4300);
	glRotated(90,1,0,0);
	cielo();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-1000,-70,-4600);
	glRotated(90,1,0,0);
	cielo();
	glPopMatrix();
	
	/*--------------------LUNA*/
	glPushMatrix();
	glColor3ub(255,255,255);
	glTranslatef(0,10,-2800);
	glScalef(1,0.6,0.2);
	glutSolidSphere(60,100,100);
	glPopMatrix();
	
	/*--------------------PISO*/
	glColor3ub(107,135,147);
	
	glPushMatrix();
	glTranslatef(-70,-70,-700);
	glRotated(90,1,0,0);
	rio();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-20,-70,-1400);
	glRotated(90,1,0,0);
	mar_sangre();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-20,-70,-2100);
	glRotated(90,1,0,0);
	mar_sangre();
	glPopMatrix();
	
	/*------------------BOSQUE*/
	glPushMatrix();
	bosque();
	glPopMatrix();
	
	glPushMatrix();
	pasto();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-90,-60,-100);
	glRotated(-45,0,1,0);
	arsenalTNT(4);
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(-100,-60,-220);
		glRotated(45,0,1,0);
		arsenalTNT(4);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-200,-60,-500);
	glRotated(45,0,1,0);
	arsenalTNT(4);
	glPopMatrix();
	
	glPushMatrix();
	glScaled(4,4,4);
	glTranslatef(-15,-15,-35);
	glRotated(90,1,0,0);
	glRotated(45,0,0,1);
	caja();
	glPopMatrix();
	
	glPushMatrix();
	glScaled(4,4,4);
	glTranslatef(-45,-15,-90);
	glRotated(90,1,0,0);
	glRotated(45,0,0,1);
	caja();
	glPopMatrix();
	
	glPushMatrix();
	glScaled(4,4,4);
	glTranslatef(-15,-15,-10);
	glRotated(90,1,0,0);
	glRotated(45,0,0,1);
	caja();
	glPopMatrix();
	
	glPushMatrix();
	glScaled(4,4,4);
	glTranslatef(-35,-14,-12);
	glRotated(90,1,0,0);
	glRotated(45,0,0,1);
	caja();
	glPopMatrix();
	
	glPushMatrix();
	glScaled(4,4,4);
	glTranslatef(-45,-15,-38);
	glRotated(90,1,0,0);
	glRotated(45,0,0,1);
	caja();
	glPopMatrix();
	
	/*--------------LAPIDAS*/
	glPushMatrix();
	glTranslatef(90,-55,-70);
	glScaled(5,7,1.5);
	glRotated(90,1,0,0);
	glRotated(20,0,0,1);
	lapida();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(80,-55,-600);
	glScaled(4,6,1.5);
	glRotated(90,1,0,0);
	glRotated(20,0,0,1);
	lapida();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-100,-55,30);
	glScaled(5,7,1.5);
	glRotated(90,1,0,0);
	glRotated(-40,0,0,1);
	lapida();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-80,-55,-400);
	glScaled(5,7,1.5);
	glRotated(90,1,0,0);
	glRotated(-20,0,0,1);
	lapida();
	glPopMatrix();
}
/*ESCENARIO FIN*/

void dibujar() {
	iniciarLuces();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glClearColor(0.0509803921569, 0.0862745098039, 0.145098039216, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();
	
	gluPerspective(60,2,10,3000);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(camaraX, camaraY, camaraZ, 0, 0, 0, 0, 1, 0);
	
	//Representacion Luz 0
	glPushMatrix();
		glTranslatef(posLuzX, posLuzY, posLuzZ);
		glColor3ub(255, 255, 255);
		glutSolidSphere(2,50,50);
	glPopMatrix();
	
	//Representacion Luz 1
	glPushMatrix();
		glTranslatef(posLuz1X, posLuz1Y, posLuz1Z);
		glColor3ub(255, 255, 255);
		glutSolidSphere(1,50,50);
	glPopMatrix();
	
	glPushMatrix();
		glRotated(angulo, 0, 1, 0);
		
		glPushMatrix();
			escenario();
		glPopMatrix();
		

	
	glPopMatrix();
	
	glutSwapBuffers();
}

void animate(){
	if (lastTime == 0)
		lastTime = clock();
	
	elapsedTime = clock()-lastTime;
	
	// Change the angle to make it blow in the wind
	float numR = (float) rand()/RAND_MAX;
	
	
	if (ANGLE > 21.5){
		if (numR < 0.5){
			incr = -0.15;
		} else {
			incr = -0.1;
		}
	} else if (ANGLE < 18.5){ 
		if (numR > 0.5){ 
			incr = 0.15;
		} else {
			incr = 0.1;
		}
	}
	ANGLE += incr;
	
	if (depth < DEPTH)
		length += 0.001;
	
	if(elapsedTime-lastElapsedTime > 2000 && depth < DEPTH){
		depth++;
		lastElapsedTime = elapsedTime;
		cout <<"a ";
		
	}
	elapsedTime = elapsedTime/5000;
	float t = (sin((elapsedTime*PI-PI/2))+1)/2;
	float p = (1-t)*STARTX + t*ENDX;
	
	if (cam)
		eyeX = p;
	glutPostRedisplay();
	
}

void manejarTeclado(unsigned char key, int x, int y){
	switch (key){
	case 'a':
		posLuzX -= 1;
		break;
	case 'd':
		posLuzX += 1;
		break;
	case 'w':
		posLuzY += 1;
		break;
	case 's':
		posLuzY -= 1;
		break;
	case 'z':
		posLuzZ -= 1;
		break;
	case 'x':
		posLuzZ += 1;
		break;
	case 'o': //cerca
		camaraZ +=5;
		break;
	case 'p': //lejo
		camaraZ -=5;
		break;
	}
	glutPostRedisplay();
}

void teclado_especial(int tecla, int x, int y) {
	switch (tecla)
	{
	case 100: //izquierda
		angulo -= 1;
		break;
	case 101: //arriba
		camaraY += 0.2;
		break;
	case 102: //derecha
		angulo += 1;
		break;
	case 103: //abajo
		camaraY -= 0.2;
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1200, 800);
	glutCreateWindow("Sweet Tooth World");
	cargarImagenes();
	glutReshapeFunc(iniciarVentana);
	glutDisplayFunc(dibujar);
	glutKeyboardFunc(manejarTeclado);
	glutSpecialFunc(teclado_especial);
	
	glutIdleFunc(animate);
	trees = new vector<string>();
	for(int i = 0; i <= DEPTH; i++){
		expand(num);
	}
	//glutTimerFunc(0, timer, 0);
	glutMainLoop();
	return 0;
}
