#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <iostream>
#include <cmath>
#include <Windows.h>
#include <cstdlib>

int WIDTH = 1280;
int HEIGHT = 720;

GLuint tex;
char title[] = "3D Model Loader Sample";
#define DEG2RAD(a) (a * 0.0174532925)


// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 100;

class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	//================================================================================================//
	// Operator Overloading; In C++ you can override the behavior of operators for you class objects. //
	// Here we are overloading the += operator to add a given value to all vector coordinates.        //
	//================================================================================================//
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
};
class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f& v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f& v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};
class Camera {
public:
	Vector3f eye, center, up;

	Camera(float eyeX = 26.0f, float eyeY = 5.0f, float eyeZ = 26.0f, float centerX = 0.25f, float centerY = 0.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 0.7f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}
	void forwardMove(float d) {
		eye.z += d;
		center.z += d;
	}
	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}
};

Camera camera;
Camera cameraF;

Vector Eye(30, 5, 30);
Vector At(0, 0, 0);
Vector Up(0, 1, 0);

int cameraZoom = 0;

// Model Variables
Model_3DS model_house;
Model_3DS model_tree;
Model_3DS model_player;
Model_3DS model_rock;
Model_3DS model_silvercoin;
Model_3DS model_goldcoin;
Model_3DS model_cone;
Model_3DS model_pole;
// Global variables
void drawCoinBar();
bool touchRockX();
bool touchRockX2();
bool touchRockZ();
bool touchRockZ2();
void printLevelOneTime();
void printLevelTwoTime();
void printLevelOneCoins();
void printLevelTwoCoins();
void printLevelOneFinished();
void printLevel();
void printCoinsTarget();
int playerZ = 0;
float playerY = 0.05;
int originalPlayerZ = 0;
int playerX = 0;
int originalPlayerX = 0;
float playerRot = 0;
int coinAngle = 0;
int coinBar = 0;
int coinBar2 = 0;
int playerRotationControl = 0;
int levelOneTime = 61;
int levelTwoTime = 41;
int level = 1;
float policeLightY = 10;
float sunZ = -40;
float sunY = 15;
float sunAmbient = 0;
bool rotateTheplayer = false;
bool isTouchedRock = false;
bool playerWinL1 = false;
bool playerWinL2 = false;
bool gameOver = false;
bool gameOver2 = false;
bool proceedToLevelTwo = false;
double r = 1;
int rd = 1;
int trigger = 0;
double rw = 1;
double rl = 1;
bool w = true;
bool rwb = false;
bool rlb = false;
bool s = true;
bool l = true;
bool firstPersonCamera = false;
bool thirdPersonCamera = false;
int treeCoordinates[][2] = { {-4,0},{10,25},{10,10},{5,-15},{22,15},{-20,5},{-10,-15},{20,-20} };
int coinCoordinates[][2] = { {-9,0},{18,25},{10,0},{0,-15},{5,6},{-20,-3},{-4,-15},{12,-20},{9,6},{17,3},{-20,8},{-4,-20},{-20,20},{-1,11},{-9,15} };
int coinCoordinates2[][2] = { {-9,0},{18,25},{10,0},{0,-15},{5,6},{-20,-3},{-4,-15},{12,-20},{9,6},{17,3},{-20,8},{-4,-20},{-20,20},{-1,11},{-9,15} };
int conesCoordinates[][2] = { {7,14}, {-15,-6}, {18,-16}, {-19,16}, {-7,-9}, {5,5} };

GLfloat texCoords[] = {
  0.0, 0.0,  // bottom-left corner
  1.0, 0.0,  // bottom-right corner
  1.0, 1.0,  // top-right corner
  0.0, 1.0   // top-left corner
};
// Textures
GLTexture tex_ground;
GLTexture tex_ground_sand;
GLTexture tex_hole;
GLTexture tex_sun;

//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources


	//// Define Light source 0 ambient light
	//GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	//glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	//// Define Light source 0 diffuse light
	//GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	//// Define Light source 0 Specular light
	//GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	//// Finally, define light source 0 position in World Space
	//GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	//glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	if (!proceedToLevelTwo)
		glClearColor(0.0, 0.75, 1.0, 1.0);
	if (proceedToLevelTwo)
		glClearColor(0.0, 0.0, 0.4, 1.0);



	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	//gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);

	//*******************************************************************************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************************************************************************//

	InitLightSource();

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

//=======================================================================
// Render Ground Function
//=======================================================================
void RenderGround()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	if (!proceedToLevelTwo)
		glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture
	if (proceedToLevelTwo)
		glBindTexture(GL_TEXTURE_2D, tex_ground_sand.texture[0]);	// Bind the ground texture


	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-30, 0, -30);
	glTexCoord2f(5, 0);
	glVertex3f(30, 0, -30);
	glTexCoord2f(5, 5);
	glVertex3f(30, 0, 30);
	glTexCoord2f(0, 5);
	glVertex3f(-30, 0, 30);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

void PoliceLight() {
	GLfloat lmodel_ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat material_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat material_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat material_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat material_shininess[] = { 50.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	GLfloat l1Diffuse[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	GLfloat l1Ambient[] = { 0.9f, 0.0f, 0.0f, 1.0f };
	GLfloat l1Position[] = { 1.0f, policeLightY, 0.0f, s };
	GLfloat l1Direction[] = { 0.0f, 0.0f, 10.0f };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, l1Diffuse);
	glLightfv(GL_LIGHT1, GL_AMBIENT, l1Ambient);
	glLightfv(GL_LIGHT1, GL_POSITION, l1Position);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 90.0);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, l1Direction);

	GLfloat l2Diffuse[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	GLfloat l2Ambient[] = { 0.0f, 0.0f, 0.9f, 1.0f };
	GLfloat l2Position[] = { 0.0f, policeLightY, 1.0f, s };
	GLfloat l2Direction[] = { 0.0f, 0.0f, -10.0f };
	glLightfv(GL_LIGHT2, GL_DIFFUSE, l2Diffuse);
	glLightfv(GL_LIGHT2, GL_AMBIENT, l2Ambient);
	glLightfv(GL_LIGHT2, GL_POSITION, l2Position);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 90.0);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, l2Direction);



}

void anim()
{
	r += 0.001 * rd;
	if (r < 0.05 || r>6)
		rd *= -1;

	rw += 0.1;
	rl += 0.1;

	glutPostRedisplay();
}

void poleLights() {
	glEnable(GL_LIGHTING);
	GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat material_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat material_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat material_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat material_shininess[] = { 50.0 };
	GLfloat l2Direction[] = { 1.0, 1.0, 1.0 };



	GLfloat light_position3[] = { -30.0, 10.0, -30.0, 0.0 };
	glLightfv(GL_LIGHT3, GL_POSITION, light_position3);
	glLightfv(GL_LIGHT3, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT3, GL_SPECULAR, light_specular);

	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);
	glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, l2Direction);
	glEnable(GL_LIGHT3);


	GLfloat light_position4[] = { 30.0, 10.0, 30.0, 0.0 };
	glLightfv(GL_LIGHT4, GL_POSITION, light_position4);
	glLightfv(GL_LIGHT4, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT4, GL_SPECULAR, light_specular);

	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);
	glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, l2Direction);

	glEnable(GL_LIGHT4);


	GLfloat light_position5[] = { -30.0, 10.0, 30.0, 0.0 };
	glLightfv(GL_LIGHT5, GL_POSITION, light_position5);
	glLightfv(GL_LIGHT5, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT5, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT5, GL_SPECULAR, light_specular);

	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);
	glLightfv(GL_LIGHT5, GL_SPOT_DIRECTION, l2Direction);

	glEnable(GL_LIGHT5);


	GLfloat light_position6[] = { 30.0, 10.0, -30.0, 0.0 };
	glLightfv(GL_LIGHT4, GL_POSITION, light_position6);
	glLightfv(GL_LIGHT4, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT4, GL_SPECULAR, light_specular);

	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);
	glLightfv(GL_LIGHT6, GL_SPOT_DIRECTION, l2Direction);

	glEnable(GL_LIGHT6);

}

void cameraSetup() {
	if (firstPersonCamera) {

	}
	else if (thirdPersonCamera) {

	}
	else {
	}
	camera.look();

}

//=======================================================================
// Display Function
//=======================================================================


void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cameraSetup();

	//GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
	//GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
	//glLightfv(GL_LIGHT3, GL_POSITION, lightPosition);
	//glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);







	// Draw Ground
	RenderGround();

	// Draw Tree Model
	//glPushMatrix();
	//glTranslatef(10, 0, 0);
	//glScalef(0.7, 0.7, 0.7);
	//model_tree.Draw();
	//glPopMatrix();

	if (!proceedToLevelTwo) {

		//draw rocks
		glPushMatrix();
		glTranslated(-4, 0, 0);
		glScaled(0.05, 0.04, 0.05);
		model_rock.Draw();
		glPopMatrix();
		glPushMatrix();
		glTranslated(10, 1, 25);
		glScaled(0.05, 0.03, 0.05);
		model_rock.Draw();
		glPopMatrix();
		glPushMatrix();
		glTranslated(10, 0, 10);
		glScaled(0.05, 0.03, 0.05);
		model_rock.Draw();
		glPopMatrix();
		glPushMatrix();
		glTranslated(5, 0, -15);
		glScaled(0.05, 0.03, 0.05);
		model_rock.Draw();
		glPopMatrix();
		glPushMatrix();
		glTranslated(22, 1, 15);
		glScaled(0.05, 0.03, 0.05);
		model_rock.Draw();
		glPopMatrix();
		glPushMatrix();
		glTranslated(-20, 0, 5);
		glScaled(0.05, 0.03, 0.05);
		model_rock.Draw();
		glPopMatrix();
		glPushMatrix();
		glTranslated(-10, 1, -15);
		glScaled(0.05, 0.03, 0.05);
		model_rock.Draw();
		glPopMatrix();
		glPushMatrix();
		glTranslated(20, 0, -20);
		glScaled(0.05, 0.03, 0.05);
		model_rock.Draw();
		glPopMatrix();
		for (int i = 0; i < 6; i++) {
			glPushMatrix();
			glTranslated(conesCoordinates[i][0], 0, conesCoordinates[i][1]);
			glScaled(1.5, 1.5, 1.5);
			model_cone.Draw();
			glPopMatrix();
		}
	}



	//draw silvercoin
	if (!proceedToLevelTwo) {
		GLfloat l0Diffuse[] = { 0.5f, 0.5f, 0.5f, 0.9f };
		GLfloat l0Spec[] = { 0.0f, 0.1f, 0.1f, 5.0f };
		GLfloat l0Ambient[] = { 0.7f - sunAmbient, 0.8f - sunAmbient, 0.7f - sunAmbient, 1.0f };
		GLfloat l0Position[] = { -5.0f, 10.0f, -17.0f, 1.0f };
		GLfloat l0Direction[] = { 1.0, 1.0, 1.0 };
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, l0Direction);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, l0Diffuse);
		glLightfv(GL_LIGHT0, GL_AMBIENT, l0Ambient);
		glLightfv(GL_LIGHT0, GL_POSITION, l0Position);
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.0);
		glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 90.0);
		glEnable(GL_LIGHT0);


		//draw sun $$#$#
		glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tex_sun.texture[0]);	// Bind the ground texture
		glTranslatef(-5, sunY, sunZ);
		GLUquadricObj* sphere = gluNewQuadric();
		gluQuadricNormals(sphere, GLU_SMOOTH);
		gluSphere(sphere, 3, 50, 50);
		gluDeleteQuadric(sphere);
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();

		for (int i = 0; i < 15; i++) {
			glPushMatrix();
			glTranslated(coinCoordinates[i][0], 1, coinCoordinates[i][1]);
			glRotated(coinAngle, 0, 1, 0);
			model_silvercoin.Draw();
			glPopMatrix();
		}
	}
	if (proceedToLevelTwo) {
		glDisable(GL_LIGHT0);
		for (int i = 0; i < 15; i++) {
			glPushMatrix();
			glTranslated(coinCoordinates2[i][0], 1, coinCoordinates2[i][1]);
			glRotated(coinAngle, 0, 1, 0);
			model_goldcoin.Draw();
			glPopMatrix();
		}
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tex_hole.texture[0]);	// Bind the ground texture
		for (int i = 0; i < 8; i++) {
			glPushMatrix();
			glColor3f(1.0, 1.0, 1.0);
			glTranslated(treeCoordinates[i][0], 0.15, treeCoordinates[i][1]);
			glScaled(2, 2, 2);
			glBegin(GL_QUADS);
			glTexCoord2f(texCoords[0], texCoords[1]); glVertex3f(-1, 0, -1);
			glTexCoord2f(texCoords[2], texCoords[3]); glVertex3f(1, 0, -1);
			glTexCoord2f(texCoords[4], texCoords[5]); glVertex3f(1, 0, 1);
			glTexCoord2f(texCoords[6], texCoords[7]); glVertex3f(-1, 0, 1);
			glEnd();
			glPopMatrix();
		}
		glDisable(GL_TEXTURE_2D);
		//draw poleelle
		glPushMatrix();
		glTranslated(-30, -1, -30);
		glScaled(1, 0.5, 1);
		glRotated(-135, 0, 1, 0);
		model_pole.Draw();
		glPopMatrix();
		//draw poleelle
		glPushMatrix();
		glTranslated(30, -1, 30);
		glScaled(1, 0.5, 1);
		glRotated(45, 0, 1, 0);
		model_pole.Draw();
		glPopMatrix();
		//draw poleelle
		glPushMatrix();
		glTranslated(30, -1, -30);
		glScaled(1, 0.5, 1);
		glRotated(90, 0, 1, 0);
		model_pole.Draw();
		glPopMatrix();
		//draw poleelle
		glPushMatrix();
		glTranslated(-30, -1, 30);
		glScaled(1, 0.5, 1);
		glRotated(-45, 0, 1, 0);
		model_pole.Draw();
		glPopMatrix();

		//glPushMatrix();
		//glTranslated(0, 5, 0);
		//glRotatef(-90, 1, 0, 0);
		//GLUquadricObj* sphere = gluNewQuadric();
		//gluQuadricNormals(sphere, GLU_SMOOTH);
		//gluSphere(sphere, 0.5, 50, 50);
		//gluDeleteQuadric(sphere);
		//glPopMatrix();

		glPushMatrix();
		glRotated(rl, 1, 1, 1);
		PoliceLight();
		glPopMatrix();
		poleLights();

	}


	//draw player
	glPushMatrix();
	glTranslated(playerX, playerY, playerZ);
	glScaled(0.07, 0.05, 0.07);
	model_player.Draw();
	glPopMatrix();





	drawCoinBar();
	//sky box
	glPushMatrix();
	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	glTranslated(50, 0, 0);
	glRotated(90, 1, 0, 1);
	glBindTexture(GL_TEXTURE_2D, tex);
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 300, 100, 100); //old radius value was 100
	gluDeleteQuadric(qobj);
	glPopMatrix();

	//Player Lose


	//Player win the game
	if (playerWinL2) {
		LPCTSTR str1 = TEXT("Congratulations You Win! ");
		LPCTSTR str2 = TEXT("End of Game. ");
		MessageBox(0, str1, str2, MB_OK);
		exit(0);
	}

	if (gameOver) {
		LPCTSTR str1 = TEXT("Time is Up!! Goodluck next time. ");
		LPCTSTR str2 = TEXT("End of Game ");
		MessageBox(0, str1, str2, MB_OK);
		exit(0);
	}


	glutSwapBuffers();

	if (gameOver2) {
		LPCTSTR str1 = TEXT("Game Over! ");
		LPCTSTR str2 = TEXT("End of Game ");
		MessageBox(0, str1, str2, MB_OK);
		exit(0);
	}
}

//if (gameOver2) {
//	LPCTSTR str1 = TEXT("You fell!! Goodluck next time. ");
//	LPCTSTR str2 = TEXT("End of Game ");
//	MessageBox(0, str1, str2, MB_OK);
//	exit(0);
//}

//=======================================================================
// My Functions
//=======================================================================

void drawCoinBar()
{
	// Set up a 2D projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

	// Disable depth testing
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);
	// Set up a model-view matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();



	//draw coinBar
	//glPushMatrix();
	//glTranslated(0.85, -0.8, 0);
	//glScaled(0.2, 3, 0);
	//glLineWidth(1);
	//glBegin(GL_LINE_LOOP);
	//glVertex2f(0, 0.5);
	//glVertex2f(0, 0);
	//glVertex2f(0.2, 0);
	//glVertex2f(0.2, 0.5);
	//glEnd();
	//glLineWidth(25);
	//glBegin(GL_LINES);
	//glVertex2f(0.1f, coinBar);
	//glVertex2f(0.1f, 0);
	//glEnd();
	//glPopMatrix();

	glPushMatrix();
	glTranslated(-0.7, 0.9, 0);
	printLevel();
	glPopMatrix();

	if (!proceedToLevelTwo) {
		glPushMatrix();
		glTranslated(0, 0.9, 0);
		printLevelOneTime();
		glPopMatrix();
		glPushMatrix();
		glTranslated(0.7, 0.9, 0);
		printLevelOneCoins();
		glPopMatrix();

	}


	if (proceedToLevelTwo) {
		glPushMatrix();
		glTranslated(0, 0.9, 0);
		printLevelTwoTime();
		glPopMatrix();
		glPushMatrix();
		glTranslated(0.7, 0.9, 0);
		printLevelTwoCoins();
		glPopMatrix();
	}


	//Player Win
	if (playerWinL1) {
		glPushMatrix();
		glColor3f(0.0f, 0.0f, 0.0f);

		// Draw the rectangle
		//glBegin(GL_QUADS);
		//glVertex2f( -0.5, 0.2);
		//glVertex2f(-0.5, 0);
		//glVertex2f(0.5, 0);
		//glVertex2f(0.5,0.2);
		//glEnd();
		glColor3f(1.0f, 1.0f, 1.0f);

		glTranslated(-0.2, 0, 0);
		glScaled(1, 4, 1);
		printLevelOneFinished();
		glPopMatrix();
	}

	myInit();
}

void printCoinsTarget() {
	glColor3f(1, 1, 1);
	char* p0s[20];
	sprintf((char*)p0s, "Coins Target");
	int len, i;
	glRasterPos2f(0, 0);
	len = (int)strlen((char*)p0s);
	char* string = (char*)p0s;

	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
	}
}

void printLevelOneFinished() {
	glColor3f(1, 1, 1);
	char* p0s[20];
	sprintf((char*)p0s, "You Won! Press X to proceed to next level.");
	int len, i;
	glRasterPos2f(0, 0);
	len = (int)strlen((char*)p0s);
	char* string = (char*)p0s;

	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
	}

}

void printLevelOneTime() {

	glColor3f(1, 1, 1);
	char* p0s[20];
	sprintf((char*)p0s, "Time Left: %d", levelOneTime);
	int len, i;
	glRasterPos2f(0, 0);
	len = (int)strlen((char*)p0s);
	char* string = (char*)p0s;

	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
	}
}

void printLevelTwoTime() {

	glColor3f(1, 1, 1);
	char* p0s[20];
	sprintf((char*)p0s, "Time Left: %d", levelTwoTime);
	int len, i;
	glRasterPos2f(0, 0);
	len = (int)strlen((char*)p0s);
	char* string = (char*)p0s;

	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
	}
}

void printLevelOneCoins() {

	glColor3f(1, 1, 1);
	char* p0s[20];
	sprintf((char*)p0s, "Coins Collected: %d/6", coinBar);
	int len, i;
	glRasterPos2f(0, 0);
	len = (int)strlen((char*)p0s);
	char* string = (char*)p0s;

	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
	}
}

void printLevelTwoCoins() {

	glColor3f(1, 1, 1);
	char* p0s[20];
	sprintf((char*)p0s, "Coins Collected: %d/12", coinBar2);
	int len, i;
	glRasterPos2f(0, 0);
	len = (int)strlen((char*)p0s);
	char* string = (char*)p0s;

	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
	}
}

void printLevel() {

	glColor3f(1, 1, 1);
	char* p0s[20];
	sprintf((char*)p0s, "Level: %d/2", level);
	int len, i;
	glRasterPos2f(0, 0);
	len = (int)strlen((char*)p0s);
	char* string = (char*)p0s;

	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
	}
}

void playerRotationController1() {
	if (playerRotationControl % 4 == 0) {
		if (!proceedToLevelTwo) {
			if (!touchRockZ()) {
				playerZ += 1;
				if (firstPersonCamera || thirdPersonCamera) {
					camera.eye.z++;
					camera.center.z++;

				}
			}

		}
		else {
			if (touchRockZ()) {
				playerZ += 2;
				playerY -= 3;
				gameOver2 = true;
			}

			if (firstPersonCamera || thirdPersonCamera) {

				camera.center.z++;
				camera.eye.z++;
			}
			playerZ += 1;

		}
	}
	if (playerRotationControl % 4 == 1) {
		if (!proceedToLevelTwo) {
			if (!touchRockX2()) {
				playerX -= 1;
				if (firstPersonCamera || thirdPersonCamera) {
					camera.eye.x--;
					camera.center.x--;

				}
			}

		}
		else {
			if (touchRockX2()) {
				playerX -= 2;
				playerY -= 3;
				gameOver2 = true;
			}
			if (firstPersonCamera || thirdPersonCamera) {

				camera.eye.x--;
				camera.center.x--;
			}
			playerX -= 1;
		}

	}
	if (playerRotationControl % 4 == 2) {
		if (!proceedToLevelTwo) {
			if (!touchRockZ2()) {
				playerZ -= 1;
				if (firstPersonCamera || thirdPersonCamera) {
					camera.eye.z--;
					camera.center.z--;

				}
			}
		}
		else {
			if (touchRockZ2()) {
				playerZ -= 2;
				playerY -= 3;
				gameOver2 = true;
			}
			if (firstPersonCamera || thirdPersonCamera) {
				camera.eye.z--;
				camera.center.z--;

			}
			playerZ -= 1;
		}

	}
	if (playerRotationControl % 4 == 3) {
		if (!proceedToLevelTwo) {
			if (!touchRockX()) {
				playerX += 1;
				if (firstPersonCamera || thirdPersonCamera) {

					camera.eye.x++;
					camera.center.x++;
				}
			}

		}
		else {
			if (touchRockX()) {
				playerX += 2;
				playerY -= 3;
				gameOver2 = true;
			}
			if (firstPersonCamera || thirdPersonCamera) {

				camera.eye.x++;
				camera.center.x++;
			}
			playerX += 1;
		}

	}
}

void playerRotationController2() {
	if (playerRotationControl % 4 == 0) {
		if (!proceedToLevelTwo) {
			if (!touchRockZ2()) {
				playerZ -= 1;
				if (firstPersonCamera || thirdPersonCamera) {
					camera.eye.z--;
					camera.center.z--;

				}
			}
		}
		else {
			if (touchRockZ2()) {
				playerZ -= 2;
				playerY -= 3;
				gameOver2 = true;
			}
			if (firstPersonCamera || thirdPersonCamera) {

				camera.eye.z--;
				camera.center.z--;
			}
			playerZ -= 1;
		}

	}
	if (playerRotationControl % 4 == 1) {
		if (!proceedToLevelTwo) {
			if (!touchRockX()) {
				playerX += 1;
				if (firstPersonCamera || thirdPersonCamera) {

					camera.eye.x++;
					camera.center.x++;
				}
			}
			else {
				if (touchRockX()) {
					playerX += 2;
					playerY -= 3;
					gameOver2 = true;
				}
				playerX += 1;
				if (firstPersonCamera || thirdPersonCamera) {
					camera.eye.x++;
					camera.center.x++;

				}
			}

		}
	}
	if (playerRotationControl % 4 == 2) {
		if (!proceedToLevelTwo) {
			if (!touchRockZ()) {
				playerZ += 1;
				if (firstPersonCamera || thirdPersonCamera) {
					camera.eye.z++;
					camera.center.z++;

				}
			}

		}
		else {
			if (touchRockZ()) {
				playerZ += 2;
				playerY -= 3;
				gameOver2 = true;
			}
			playerZ += 1;
			if (firstPersonCamera || thirdPersonCamera) {
				camera.eye.z++;
				camera.center.z++;

			}
		}

	}
	if (playerRotationControl % 4 == 3) {
		if (!proceedToLevelTwo) {
			if (!touchRockX2())
			{
				playerX -= 1;
				if (firstPersonCamera || thirdPersonCamera) {
					camera.eye.x--;
					camera.center.x--;

				}
			}
			else {
				if (touchRockX2()) {
					playerX -= 2;
					playerY -= 3;
					gameOver2 = true;
				}
				playerX -= 1;
				if (firstPersonCamera || thirdPersonCamera) {
					camera.eye.x--;
					camera.center.x--;

				}
			}

		}
	}
}

bool touchRockX() {

	for (int i = 0; i < 8; i++) {

		int minX = treeCoordinates[i][0] - 2;
		int maxX = treeCoordinates[i][0] + 2;
		int minZ = treeCoordinates[i][1] - 2;
		int maxZ = treeCoordinates[i][1] + 2;

		if (playerZ >= minZ && playerZ <= maxZ) {
			if (playerX + 1 >= minX && playerX < maxX) {
				PlaySound("sounds/crash.wav", NULL, SND_ASYNC);

				return true;
			}
		}

	}
	return false;
}

bool touchRockX2() {

	for (int i = 0; i < 8; i++) {

		int minX = treeCoordinates[i][0] - 2;
		int maxX = treeCoordinates[i][0] + 2;
		int minZ = treeCoordinates[i][1] - 2;
		int maxZ = treeCoordinates[i][1] + 2;

		if (playerZ >= minZ && playerZ <= maxZ) {
			if (playerX > minX && playerX - 1 <= maxX) {
				PlaySound("sounds/crash.wav", NULL, SND_ASYNC);

				return true;
			}
		}

	}
	return false;
}

bool touchRockZ() {

	for (int i = 0; i < 8; i++) {

		int minX = treeCoordinates[i][0] - 2;
		int maxX = treeCoordinates[i][0] + 2;
		int minZ = treeCoordinates[i][1] - 2;
		int maxZ = treeCoordinates[i][1] + 2;

		if (playerX >= minX && playerX <= maxX) {
			if (playerZ + 1 >= minZ && playerZ < maxZ) {
				PlaySound("sounds/crash.wav", NULL, SND_ASYNC);

				return true;
			}
		}

	}
	return false;
}

bool touchRockZ2() {

	for (int i = 0; i < 8; i++) {

		int minX = treeCoordinates[i][0] - 2;
		int maxX = treeCoordinates[i][0] + 2;
		int minZ = treeCoordinates[i][1] - 2;
		int maxZ = treeCoordinates[i][1] + 2;
		if (playerX >= minX && playerX <= maxX) {
			if (playerZ > minZ && playerZ - 1 <= maxZ) {
				PlaySound("sounds/crash.wav", NULL, SND_ASYNC);

				return true;
			}
		}

	}
	return false;
}

void rotateCamera(float angle) {
	// Angle of rotation in radians

	// Create a 3D rotation matrix
	float rotationMatrix[3][3] = {
		{cos(angle), 0, sin(angle)},
		{0, 1, 0},
		{-sin(angle), 0, cos(angle)}
	};

	// Rotate the eye position of the camera
	camera.eye.x = rotationMatrix[0][0] * camera.eye.x + rotationMatrix[0][1] * camera.eye.y + rotationMatrix[0][2] * camera.eye.z;
	camera.eye.y = rotationMatrix[1][0] * camera.eye.x + rotationMatrix[1][1] * camera.eye.y + rotationMatrix[1][2] * camera.eye.z;
	camera.eye.z = rotationMatrix[2][0] * camera.eye.x + rotationMatrix[2][1] * camera.eye.y + rotationMatrix[2][2] * camera.eye.z;

	// Rotate the center position of the camera
	camera.center.x = rotationMatrix[0][0] * camera.center.x + rotationMatrix[0][1] * camera.center.y + rotationMatrix[0][2] * camera.center.z;
	camera.center.y = rotationMatrix[1][0] * camera.center.x + rotationMatrix[1][1] * camera.center.y + rotationMatrix[1][2] * camera.center.z;
	camera.center.z = rotationMatrix[2][0] * camera.center.x + rotationMatrix[2][1] * camera.center.y + rotationMatrix[2][2] * camera.center.z;

	// Rotate the up vector of the camera
	camera.up.x = rotationMatrix[0][0] * camera.up.x + rotationMatrix[0][1] * camera.up.y + rotationMatrix[0][2] * camera.up.z;
	camera.up.y = rotationMatrix[1][0] * camera.up.x + rotationMatrix[1][1] * camera.up.y + rotationMatrix[1][2] * camera.up.z;
	camera.up.z = rotationMatrix[2][0] * camera.up.x + rotationMatrix[2][1] * camera.up.y + rotationMatrix[2][2] * camera.up.z;

}

//=======================================================================
// Keyboard Function
//=======================================================================
void myKeyboard(unsigned char button, int x, int y)
{
	switch (button)
	{
	case 'w':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'r':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 'q':
		exit(0);
		break;
	case 'x':
		if (playerWinL1) {
			proceedToLevelTwo = true;
			playerWinL1 = false;
			model_player.rot.y = 0;
			playerX = 0;
			playerZ = 0;
			coinBar = 0;
			level++;
		}
		break;
	case 't':
		thirdPersonCamera = true;
		firstPersonCamera = false;
		camera.eye.x = playerX;
		camera.eye.y = 5;
		camera.eye.z = playerZ - 8;
		camera.center.x = playerX;
		camera.center.y = 2;
		camera.center.z = playerZ + 6;
		camera.up.x = 0;
		camera.up.y = 0.968069;
		camera.up.z = 0;
		break;
	case 'f':
		thirdPersonCamera = false;
		firstPersonCamera = true;
		camera.eye.x = playerX;
		camera.eye.y = 5;
		camera.eye.z = playerZ;
		camera.center.x = playerX;
		camera.center.y = 3;
		camera.center.z = playerZ + 7;
		camera.up.x = 0;
		camera.up.y = 0.968069;
		camera.up.z = 0;
		break;
	case 'g':
		thirdPersonCamera = false;
		firstPersonCamera = false;
		camera.eye.x = 26;
		camera.eye.y = 5;
		camera.eye.z = 26;
		camera.center.x = 0;
		camera.center.y = 2;
		camera.center.z = 0;
		camera.up.x = 0;
		camera.up.y = 0.968069;
		camera.up.z = 0;
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void cameraFPRotation1() {
	if (playerX == camera.center.x) {
		if (camera.center.z > playerZ) {
			camera.center.x += 7;
			camera.center.z -= 7;
		}
		else {
			camera.center.x -= 7;
			camera.center.z += 7;
		}
	}
	else {
		if (camera.center.x > playerX) {
			camera.center.x -= 7;
			camera.center.z -= 7;
		}
		else {
			camera.center.x += 7;
			camera.center.z += 7;
		}
	}
}
void cameraFPRotation2() {
	if (playerX == camera.center.x) {
		if (camera.center.z > playerZ) {
			camera.center.x -= 7;
			camera.center.z -= 7;
		}
		else {
			camera.center.x += 7;
			camera.center.z += 7;
		}
	}
	else {
		if (camera.center.x > playerX) {
			camera.center.x -= 7;
			camera.center.z += 7;
		}
		else {
			camera.center.x += 7;
			camera.center.z -= 7;
		}
	}
}

void SpecialKey(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		playerRot += 90;
		model_player.rot.y = playerRot;
		playerRotationControl--;
		if (firstPersonCamera) {
			cameraFPRotation1();
		}


		break;
	case GLUT_KEY_RIGHT:
		playerRot -= 90;
		model_player.rot.y = playerRot;
		playerRotationControl++;
		if (firstPersonCamera) {
			cameraFPRotation2();
		}
		break;
	case GLUT_KEY_UP:
		playerRotationController1();
		break;
	case GLUT_KEY_DOWN:
		playerRotationController2();
		break;
	}
	std::cout << playerX << " " << playerZ << "\n";

	glutPostRedisplay();
}

//=======================================================================
// Motion Function
//=======================================================================
void myMotion(int x, int y)
{
	y = HEIGHT - y;

	if (cameraZoom - y > 0)
	{
		Eye.x += -0.1;
		Eye.z += -0.1;
	}
	else
	{
		Eye.x += 0.1;
		Eye.z += 0.1;
	}

	cameraZoom = y;

	glLoadIdentity();	//Clear Model_View Matrix

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters

	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glutPostRedisplay();	//Re-draw scene 
}

//=======================================================================
// Mouse Function
//=======================================================================
void myMouse(int button, int state, int x, int y)
{
	y = HEIGHT - y;

	if (state == GLUT_DOWN)
	{
		cameraZoom = y;
	}
}

//=======================================================================
// Reshape Function
//=======================================================================
void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}

//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	// Loading Model files
	model_house.Load("Models/house/house.3DS");
	model_tree.Load("Models/tree/Tree1.3ds");
	model_player.Load("Models/stick/StickFigurea.3ds");
	model_rock.Load("Models/rock/CavePlatform1_3ds.3ds");
	model_silvercoin.Load("Models/silvercoin/GoldCoinBlank.3ds");
	model_silvercoin.rot.z = -25;
	model_goldcoin.Load("Models/goldcoin/GoldCoinBlank.3ds");
	model_goldcoin.rot.z = -25;
	model_cone.Load("models/cone/cone.3DS");
	model_pole.Load("models/pole/untitled.3DS");

	// Loading texture files
	tex_ground.Load("Textures/ground.bmp");
	tex_ground_sand.Load("Textures/sand.bmp");
	tex_hole.Load("Textures/hole.bmp");
	tex_sun.Load("Models/goldcoin/goldcoin.bmp");
	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
}

//=======================================================================
// Timer Functions
//=======================================================================

void timer(int) {
	glutPostRedisplay();
	glutTimerFunc(1000 / 30, timer, 0);

	if (!proceedToLevelTwo) {
		for (int i = 0; i < 15; i++)
		{
			if (playerX == coinCoordinates[i][0] && playerZ == coinCoordinates[i][1])
			{
				PlaySound("sounds/coin.wav", NULL, SND_ASYNC);
				coinCoordinates[i][0] = -100;
				coinCoordinates[i][1] = -100;
				if (coinBar <= 5)
					coinBar += 1;
				if (coinBar == 6)
					playerWinL1 = true;
			}
		}
		for (int i = 0; i < 6; i++) {
			if (playerX == conesCoordinates[i][0] && playerZ == conesCoordinates[i][1]) {
				//prevent player from moving
				playerX = playerX - 1;
				playerZ = playerZ - 1;
			}

		}
	}
	if (proceedToLevelTwo) {
		for (int i = 0; i < 15; i++)
		{
			if (playerX == coinCoordinates2[i][0] && playerZ == coinCoordinates2[i][1])
			{
				PlaySound("sounds/coin.wav", NULL, SND_ASYNC);

				coinCoordinates2[i][0] = -100;
				coinCoordinates2[i][1] = -100;
				if (coinBar2 <= 11)
					coinBar2 += 1;
				if (coinBar2 == 12)
					playerWinL2 = true;
			}
		}
	}



}

void coinRotation(int) {
	glutPostRedisplay();
	glutTimerFunc(1000 / 30, coinRotation, 0);
	coinAngle += 10;

}

void levelOneTimer(int) {
	glutPostRedisplay();
	glutTimerFunc(1000, levelOneTimer, 0);
	if (!proceedToLevelTwo) {

		if (levelOneTime > 0 && !playerWinL1)
			levelOneTime--;
		else {
			if (playerWinL1)
				std::cout << "PLAYER WIN";
			else
				gameOver = true;
		}
	}
}

void levelTwoTimer(int) {
	glutPostRedisplay();
	glutTimerFunc(1000, levelTwoTimer, 0);
	if (proceedToLevelTwo) {
		if (levelTwoTime > 0)
			levelTwoTime--;
		else {
			if (playerWinL2 && !playerWinL2)
				std::cout << "PLAYER WIN";
			else
				gameOver = true;
		}

		policeLightY -= 0.5;
		if (trigger % 2 == 0) {
			glDisable(GL_LIGHT2);
			glEnable(GL_LIGHT1);
		}
		else {
			glDisable(GL_LIGHT1);
			glEnable(GL_LIGHT2);
		}
		trigger++;

		if (levelTwoTime == 5)
			glDisable(GL_LIGHT3);
		glDisable(GL_LIGHT4);
		glDisable(GL_LIGHT5);
		glDisable(GL_LIGHT6);

	}

}

void sunSetTimer(int) {
	glutPostRedisplay();
	glutTimerFunc(1000 / 2, sunSetTimer, 0);
	sunZ -= 0.1;
	sunY -= 0.1;
	sunAmbient += 0.004;

}



//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(20, 20);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);

	glutKeyboardFunc(myKeyboard);

	glutSpecialFunc(SpecialKey);

	glutTimerFunc(0, coinRotation, 0);

	glutTimerFunc(0, timer, 0);

	glutTimerFunc(0, levelOneTimer, 0);

	glutTimerFunc(0, levelTwoTimer, 0);

	glutTimerFunc(0, sunSetTimer, 0);

	//glutIdleFunc(anim);

	glutMotionFunc(myMotion);

	glutMouseFunc(myMouse);

	glutReshapeFunc(myReshape);

	myInit();

	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}