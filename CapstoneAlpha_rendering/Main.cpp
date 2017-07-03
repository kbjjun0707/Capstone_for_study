#pragma warning(disable:4996)
#include <gl/glut.h>
#include <gl/GL.H>
#include <gl/GLU.H>

#include "Camera.h"

#include "glCamera.h"
#include "TestPoint.h"
//#include "Heightmap.h"

#include "Light.h"

#include <iostream>
#include <fstream>
#include <string>

//HeightMap map[4];
iMiner::Cus_PointCloud map[4];

double CameraFar = 1300.0;
Camera Main_Camera(0, 0, 0,
	0, 0, 0, CameraFar);
glCamera glcamera;

Light Main_light(0);

int type = 1, pSize = 1;

int		Win_width = 1024, Win_height = 768;
int		NewMouseX, NewMouseY, OldMouseX, OldMouseY, OldMouseXm, OldMouseYm;
int		moveX, moveY; //∞¥√º ¿Ãµø 
bool	G_Mouse_State[3];

int count, index;
char fileName[100];
int fw, fh;

int G_Tra[4] = { 0 };

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Main_light.run();

	glPushMatrix(); {
		glTranslatef(-moveX, -moveY, 0);
		glPushMatrix(); {
			glcamera.SetPrespective();

			glRotatef(90, 1, 0, 0);
			glTranslated(-fw / 2.0, 0, -fh / 2.0);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);p
			//glutSolidTeapot(10);
			map[0].RenderHeighMap(type, pSize);
			//glCallList(G_Tra[0]);

		}glPopMatrix();
	}glPopMatrix();

	glutSwapBuffers();
}
void MyMouseClick(int click, int state, int x, int y) {

	if (click == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		G_Mouse_State[0] = true;
		OldMouseX = x; OldMouseY = y;
	}
	else if (click == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		G_Mouse_State[0] = false;
	}
	if (click == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
		G_Mouse_State[1] = true;
	}
	else if (click == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
		G_Mouse_State[1] = false;
	}
	if (click == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		G_Mouse_State[2] = true;
		OldMouseXm = x; OldMouseYm = y;
	}
	else if (click == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
		G_Mouse_State[2] = false;
	}

}
void MyMouseMotion(int x, int y) {
	GLfloat DeltaMouse;

	if (G_Mouse_State[0]) {
		NewMouseX = x; NewMouseY = y;
		if (NewMouseX < OldMouseX) {
			DeltaMouse = GLfloat(OldMouseX - NewMouseX);
			glcamera.ChangeHeading(-0.2f * DeltaMouse);
		}
		else if (NewMouseX > OldMouseX) {
			DeltaMouse = GLfloat(NewMouseX - OldMouseX);
			glcamera.ChangeHeading(0.2f * DeltaMouse);
		}
		if (NewMouseY < OldMouseY) {
			DeltaMouse = GLfloat(OldMouseY - NewMouseY);
			glcamera.ChangePitch(-0.2f * DeltaMouse);
		}
		else if (NewMouseY > OldMouseY) {
			DeltaMouse = GLfloat(NewMouseY - OldMouseY);
			glcamera.ChangePitch(0.2f * DeltaMouse);
		}
		OldMouseX = x;
		OldMouseY = y;
	}
	if (G_Mouse_State[2]) {

	}

}
void MyReshape(int x, int y) {
	Win_width = x; Win_height = y;
	glViewport(0, 0, x, y);

	Main_Camera.run(x, y);
}
void SkeyFunc(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_RIGHT:
		moveX += 10;
		break;
	case GLUT_KEY_LEFT:
		moveX -= 10;
		break;
	case GLUT_KEY_UP:
		moveY += 10;
		break;
	case GLUT_KEY_DOWN:
		moveY -= 10;
		break;
	case GLUT_KEY_PAGE_UP:
		glcamera.ChangeVelocity(0.1f);
		break;
	case GLUT_KEY_PAGE_DOWN:
		glcamera.ChangeVelocity(-0.1f);
		break;
	case GLUT_KEY_F1:
		exit(0);
		break;

	}

}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(Win_width, Win_height);
	glutInitWindowPosition(200, 130);
	glutCreateWindow("Project");
	glClearColor(0.5, 0.0, 0.0, 0.0);

	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_ALPHA);
	//glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	/*Camera Setup*/
	glcamera.m_MaxForwardVelocity = 5.0f;
	glcamera.m_MaxPitchRate = 5.0f;
	glcamera.m_MaxHeadingRate = 5.0f;
	glcamera.m_PitchDegrees = 0.0f;
	glcamera.m_HeadingDegrees = 0.0f;

	//std::ifstream input;
	//input.open("test.txt");
	//input >> count;
	//index = count;

	//while (index >= count)
	//	std::cin >> index;

	//for (int i = 0; i <= index; i++) {
	//	input >> fileName >> fw >> fh;
	//};
	//input.close();

	//map[0].Mapset("E:/Workspace/testDense.jpg", fw, fh, 1.0, NULL, 1);
	map[0].set("E:/Workspace/testDense.jpg", 1.0, "E:/Workspace/stereo001L.jpg", 1);
	//map[0].set("E:/Workspace/testDisparity.jpg", 1.0, NULL, 1);

	std::cout << "Type : ";	std::cin >> type;
	if (!type) {
		std::cout << "Size : "; std::cin >> pSize;
	}

	//G_Tra[0] = glGenLists(1);
	//glNewList(G_Tra[0], GL_COMPILE);
	//map[0].RenderHeighMap(type, pSize);
	//glEndList();


	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutSpecialFunc(SkeyFunc);
	glutReshapeFunc(MyReshape);
	glutMouseFunc(MyMouseClick);
	glutMotionFunc(MyMouseMotion);

	glutMainLoop();

	return 0;
}