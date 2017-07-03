#ifndef CAMERA_H
#define CAMERA_H

#ifndef POINT_H
#include "Point.h"
#endif // !POINT_H

#include <math.h>
//
//#include <math.h>
//
//enum keystate { NONE_ROTATE = 0, UP, DOWN, LEFT, RIGHT, LEFTDOWN, RIGHTDOWN } RotateState;
//
//enum cameramovement { CM_NON = 0, CM_FRONT, CM_BACK, CM_LEFT, CM_RIGHT, CM_UP, CM_DOWN } CameraMovement;
//
//GLdouble CM_zoom = 1, CameraSpeed = 1, AngleValue = 2;

class Camera {
	void set() {
		// 카메라의 실제 위치
		eye.x = (Far)*cos(ang.x*3.141592653 / 180.0)*sin((ang.y)*3.141592653 / 180) + look.x;
		eye.y = (Far)*sin(ang.x*3.141592653 / 180.0) + look.y;
		eye.z = (Far)*cos(ang.x*3.141592653 / 180.0)*cos((ang.y)*3.141592653 / 180) + look.z;
	}
public:
	iMiner::Point3<float> look, eye, ang;
	float Far;

	Camera(float lookx, float looky, float lookz, float angx, float angy, float angz, float far1) {
		look.set(lookx, looky, lookz); 
		ang.set(angx, angy, angz);
		this->Far = far1;
	}

	void run(int winx, int winy) {
		set();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0, (GLdouble)winx / (GLdouble)winy, 1, 8000.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(eye.x, eye.y, eye.z,
			look.x, look.y, look.z,
			0, 1, 0);
	}

	
	//	Point3<float> eye;
//	float tx, ty, tz,	// 임시 저장소 
//		vx, vy, vz;			// 카메라의 y축 벡터
//							// 방향에 따른 움직임
//	void Cameramove(int cameramove, int speed = 1) {
//		double dx = 0, dy = 0, dz = 0;
//		switch (cameramove) {
//		case CM_FRONT:
//			dx = -sin(AngX*3.141592653 / 180)*cos(AngZ*3.141592653 / 180);
//			dy = -sin(AngY*3.141592653 / 180)*cos(AngZ*3.141592653 / 180);
//			dz = -cos(AngY*3.141592653 / 180)*cos(AngX*3.141592653 / 180);
//			break;
//		case CM_BACK:
//			dx = sin(AngX*3.141592653 / 180)*cos(AngZ*3.141592653 / 180);
//			dy = sin(AngY*3.141592653 / 180)*cos(AngZ*3.141592653 / 180);
//			dz = cos(AngY*3.141592653 / 180)*cos(AngX*3.141592653 / 180);
//			break;
//		case CM_LEFT:
//			dx = -cos(AngX*3.141592653 / 180)*cos(AngZ*3.141592653 / 180);
//			dy = -cos(AngY*3.141592653 / 180)*sin(AngZ*3.141592653 / 180);
//			dz = cos(AngZ*3.141592653 / 180)*sin(AngX*3.141592653 / 180);
//			break;
//		case CM_RIGHT:
//			dx = cos(AngX*3.141592653 / 180)*cos(AngZ*3.141592653 / 180);
//			dy = cos(AngY*3.141592653 / 180)*sin(AngZ*3.141592653 / 180);
//			dz = -cos(AngZ*3.141592653 / 180)*sin(AngX*3.141592653 / 180);
//			break;
//		case CM_UP:
//			dx = cos(AngX*3.141592653 / 180)*sin(AngZ*3.141592653 / 180);
//			dy = cos(AngY*3.141592653 / 180)*cos(AngZ*3.141592653 / 180);
//			dz = -sin(AngY*3.141592653 / 180)*cos(AngX*3.141592653 / 180);
//			break;
//		case CM_DOWN:
//			dx = -cos(AngX*3.141592653 / 180)*sin(AngZ*3.141592653 / 180);
//			dy = -cos(AngY*3.141592653 / 180)*cos(AngZ*3.141592653 / 180);
//			dz = sin(AngY*3.141592653 / 180)*cos(AngX*3.141592653 / 180);
//			break;
//		default:
//			break;
//		}
//		look.x += (speed*dx); look.y += (speed*dy); look.z += (speed*dz);
//	}
//	// 입력받은 키에 따른 각도조절
//	void moveAngle(int Key, double value) {
//		switch (Key) {
//		case UP:
//			AngY += value;
//			break;
//		case DOWN:
//			AngY -= value;
//			break;
//		case LEFT:
//			AngX -= value;
//			break;
//		case RIGHT:
//			AngX += value;
//			break;
//		case LEFTDOWN:
//			AngZ += value;
//			break;
//		case RIGHTDOWN:
//			AngZ -= value;
//			break;
//
//		}
//	}
//	// 정보들로부터 카메라의 위치 세팅
//	void set(GLdouble zo, GLdouble speed, GLdouble turnvalue) {
//		moveAngle(RotateState, turnvalue);
//		RotateState = NONE_ROTATE;
//		Cameramove(CameraMovement, speed);
//		CameraMovement = CM_NON;
//		Zoom *= zo;
//		// 카메라의 실제 위치
//		eye.x = (Far / Zoom)*cos(AngY*3.141592653 / 180.0)*sin((AngX)*3.141592653 / 180) + look.x;
//		eye.y = (Far / Zoom)*sin(AngY*3.141592653 / 180.0) + look.y;
//		eye.z = (Far / Zoom)*cos(AngY*3.141592653 / 180.0)*cos((AngX)*3.141592653 / 180) + look.z;
//		// 카메라의 y벡터
//		vx = sin((AngX + 180)*3.141592653 / 180)*sin(AngY*3.141592653 / 180)*cos(AngZ*3.141592653 / 180);
//		vy = cos(AngY*3.141592653 / 180)*cos(AngZ*3.141592653 / 180)*cos(AngZ*3.141592653 / 180);
//		vz = cos((AngX + 180)*3.141592653 / 180)*sin(AngY*3.141592653 / 180)*cos(AngZ*3.141592653 / 180);
//
//		vx -= (cos(AngX*3.141592653 / 180)*sin(AngZ*3.141592653 / 180));
//		vz += (sin(AngX*3.141592653 / 180)*sin(AngZ*3.141592653 / 180));
//	}
//public:
//	// 원점(참조점)좌표
//	Point3<float> look;
//	// degree 각도
//	GLdouble AngX, AngY, AngZ;
//	GLdouble Far, Zoom;
//	//     원점 x, y, z                      각도는 degree값  Ax = 좌우 각도   Ay = 높이 각도   Az = y축각도     원점과의 거리    확대비율
//	Camera(GLdouble x1 = 0, GLdouble y1 = 0, GLdouble z1 = 0, GLdouble Ax = 0, GLdouble Ay = 0, GLdouble Az = 0, GLdouble F = 10, GLdouble zo = 1) {
//		look.set(x1, y1, z1); AngX = Ax; AngY = Ay; AngZ = Az; Far = F; Zoom = zo;
//	}
//	void Position(float ix, float iy, float iz) {
//		look.set(ix, iy, iz);
//	}
//	void Position(float pos[3]) {
//		look.set(pos);
//	}
//	void Run() {
//		set(CM_zoom, CameraSpeed, AngleValue);
//		CM_zoom = 1; CameraSpeed = 1;
//		glMatrixMode(GL_PROJECTION);
//		glLoadIdentity();
//		//glRotated(-AngY, 1, 0, 0);
//		//glOrtho(-100, 100, -100, 100, -100, 100);
//		//glFrustum(-1, 1, -1, 1, 1, 1000);
//
//		gluPerspective(45.0, (GLdouble)1024.0/ (GLdouble)768.0, 0.1, 100000.0);
//		glMatrixMode(GL_MODELVIEW);
//		glLoadIdentity();
//		gluLookAt(eye.x, eye.y, eye.z,
//			look.x, look.y, look.z,
//			vx, vy, vz);
//	}
};

#endif // !CAMERA_H