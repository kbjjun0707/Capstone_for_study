#include <opencv2\opencv.hpp>
#include <opencvInit.h>

#include <gl\glut.h>
#include <gl\GL.H>

#include <iostream>
#include <thread>

using namespace std;

// CV
void onMouse(int evt, int x, int y, int flags, void *param);

cv::VideoCapture g_Cam;
int g_CorHor = 7, g_CorVer = 4, g_CorCellSz = 26;
cv::Size g_BoardSz(g_CorHor, g_CorVer);

cv::Mat g_K, g_dC, g_R, g_t;
int g_State = 0;

vector<vector<cv::Point3f>> g_ObjPoints;
vector<vector<cv::Point2f>> g_ImgPoints;
vector<cv::Point3f> g_ObjCorners;

char *g_CamWin = "Cam", *g_CapturedWin = "Captured";


// GL
int		Win_width = 1024, Win_height = 768;
float	CameraFar = 1000.0f, gRotateY = 0, gRotateX = 0;
int		NewMouseX, NewMouseY, OldMouseX, OldMouseY, OldMouseXm, OldMouseYm;
int		moveX, moveY; //∞¥√º ¿Ãµø 
bool	G_Mouse_State[3];
float	ChessBoardPoints[4][7];

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix(); {
		gluLookAt(0, -CameraFar, 0, 0, 0, 0, 0, 0, 1);
		glRotatef(gRotateY, 0, 0, 1);
		glRotatef(gRotateX, 1, 0, 0);
		glPushMatrix(); {
			glBegin(GL_QUADS);
			for (int r = 0; r < g_CorVer - 1; r++) {
				for (int c = 0; c < g_CorHor - 1; c++) {
					if (r % 2 == 0 && c % 2 == 0)
						glColor3f(0, 0, 0);
					else if (r % 2 == 0 && c % 2 == 1)
						glColor3f(1, 1, 1);
					else if (r % 2 == 1 && c % 2 == 0)
						glColor3f(1, 1, 1);
					else if (r % 2 == 1 && c % 2 == 1)
						glColor3f(0, 0, 0);
					glVertex3f(          0 + (c*g_CorCellSz),           0 + (r*g_CorCellSz), 0);
					glVertex3f(g_CorCellSz + (c*g_CorCellSz),           0 + (r*g_CorCellSz), 0);
					glVertex3f(g_CorCellSz + (c*g_CorCellSz), g_CorCellSz + (r*g_CorCellSz), 0);
					glVertex3f(          0 + (c*g_CorCellSz), g_CorCellSz + (r*g_CorCellSz), 0);
				}
			}
			glEnd();

			glBegin(GL_LINES); {
				glColor3f(1, 0, 0);
				glVertex3f(0, 0, 0);
				glVertex3f(200, 0, 0);
				glColor3f(0, 1, 0);
				glVertex3f(0, 0, 0);
				glVertex3f(0, 200, 0);
				glColor3f(0, 0, 1);
				glVertex3f(0, 0, 0);
				glVertex3f(0, 0, 200);
			}glEnd();
		}glPopMatrix();

		glPushMatrix(); {
			for (int i = 0; i < g_State; i++) {
				cv::Mat rvec, tvec;
				cv::solvePnP(g_ObjPoints[i], g_ImgPoints[i], g_K, g_dC, rvec, tvec);
				cv::Rodrigues(rvec, g_R);
				cout << tvec << endl << endl;

				cv::Mat R_inv = g_R.inv();
				cv::Mat Cam_Pos = -R_inv*tvec;
				double *p = (double *)Cam_Pos.data;

				double unit_z[] = { 0, 0, 1 };
				cv::Mat Zc(3, 1, CV_64FC1, unit_z);
				cv::Mat Zw = -R_inv*Zc;
				double *zw = (double *)Zw.data;

				double pan = (atan2(zw[1], zw[0]) -CV_PI / 2);
				double tilt = (atan2(zw[2], sqrt(zw[0] * zw[0] + zw[1] * zw[1])));

				double unit_x[] = { 1,0,0 };
				cv::Mat Xc(3, 1, CV_64FC1, unit_x);
				cv::Mat Xw = -R_inv*Xc;
				double *xw = (double *)Xw.data;
				double xpan[] = { cos(pan), sin(pan), 0 };

				double roll = (acos(xw[0] * xpan[0] + xw[1]*xpan[1] + xw[2] * xpan[2]));
				if (xw[2] < 0)roll = -roll;			

				glPushMatrix(); {
					glColor3f(0.6, 0, 0.6);
					//glRotatef(180, 1, 0, 0);
					glTranslated(p[0], p[1], p[2]);
					//glRotated(tilt*180.0 / CV_PI, 1, 0, 0);
					//glRotated(pan*180.0 / CV_PI, 0, 1, 0);
					//glRotated(roll*180.0/CV_PI, 0, 0, 1);
					glScalef(30, 60, 10);
					//glRotatef(90, 1, 0, 0);
					glutSolidCube(1);
				}glPopMatrix();
			}
			//cout << g_R << endl;
		}glPopMatrix();

	}glPopMatrix();
	
	glutSwapBuffers();
}
void MyMouseClick(int click, int state, int x, int y) {

	if (click == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		G_Mouse_State[0] = true;
		OldMouseX = x; OldMouseY = y;
	} else if (click == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		G_Mouse_State[0] = false;
	}
	if (click == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
		G_Mouse_State[1] = true;
	} else if (click == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
		G_Mouse_State[1] = false;
	}
	if (click == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		G_Mouse_State[2] = true;
		OldMouseXm = x; OldMouseYm = y;
	} else if (click == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
		G_Mouse_State[2] = false;
	}

}
void MyMouseMotion(int x, int y) {
	GLfloat DeltaMouse;

	if (G_Mouse_State[0]) {
		NewMouseX = x; NewMouseY = y;
		if (NewMouseX < OldMouseX) {
			DeltaMouse = GLfloat(OldMouseX - NewMouseX);
		} else if (NewMouseX > OldMouseX) {
			DeltaMouse = GLfloat(NewMouseX - OldMouseX);
		}
		if (NewMouseY < OldMouseY) {
			DeltaMouse = GLfloat(OldMouseY - NewMouseY);
		} else if (NewMouseY > OldMouseY) {
			DeltaMouse = GLfloat(NewMouseY - OldMouseY);
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

}
void SkeyFunc(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_RIGHT:
		gRotateY += 10;
		break;
	case GLUT_KEY_LEFT:
		gRotateY -= 10;
		break;
	case GLUT_KEY_UP:
		gRotateX += 10;
		break;
	case GLUT_KEY_DOWN:
		gRotateX -= 10;
		moveY -= 10;
		break;
	case GLUT_KEY_PAGE_UP:
		CameraFar *= 1.1;
		break;
	case GLUT_KEY_PAGE_DOWN:
		CameraFar *= 0.9;
		break;
	case GLUT_KEY_F1:
		exit(0);
		break;
	}

}

int main(int argc, char **argv) {
	g_Cam.open(0);

	cv::namedWindow(g_CamWin);
	cv::namedWindow(g_CapturedWin);
	cv::moveWindow(g_CamWin, 10, 10);
	cv::destroyWindow(g_CapturedWin);

	double K_Data[] = {
		761.788,        0, 258.556,
		0,        760.536, 196.138,
		0,              0,       1
	};
	g_K = cv::Mat(3, 3, CV_64FC1, K_Data);
	double dC_Data[] = { 0, 0, 0, 0, 0 };
	g_dC = cv::Mat(1, 5, CV_64FC1, dC_Data);

	if (!g_Cam.isOpened()) {
		cout << "Failed camera open" << endl;
		return 0;
	}

	std::thread m_GlThread([&]() {
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
		glutInitWindowSize(Win_width, Win_height);
		glutInitWindowPosition(200, 130);
		glutCreateWindow("Project");
		glClearColor(0.5, 0.0, 0.0, 0.0);
		
		glMatrixMode(GL_MODELVIEW);
		glEnable(GL_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glShadeModel(GL_SMOOTH);
		gluPerspective(45, (double)Win_width / Win_height, 0.0001, 10000);

		glutDisplayFunc(display);
		glutIdleFunc(display);
		glutSpecialFunc(SkeyFunc);
		glutReshapeFunc(MyReshape);
		glutMouseFunc(MyMouseClick);
		glutMotionFunc(MyMouseMotion);

		glutMainLoop();
	});

	cv::Mat m_Frame, m_Img, m_GrayImg;
	cv::setMouseCallback(g_CamWin, onMouse, (void *)&m_Frame);

	for (int r = 0; r < g_BoardSz.height; r++)
		for (int c = 0; c < g_BoardSz.width; c++)
			g_ObjCorners.push_back(cv::Point3f(c*g_CorCellSz, r*g_CorCellSz, 0.0f));
	
	bool m_CamRun = true;
	while (m_CamRun) {
		g_Cam >> m_Frame;
		if (m_Frame.empty())
			continue;
		cv::imshow(g_CamWin, m_Frame);
		

		switch (cv::waitKey(40)) {
		case 2424832:      // left
			break;
		case 2555904:      // right
			break;
		case 2490368:      // up
			break;
		case 2621440:      // down
			break;
		case 'r':
		case 'R':
			break;
		case 'd':
		case 'D':
			break;
		case 13:                     // Enter key
		case 27:
			m_CamRun = false;
			break;
		default:
			break;
		}
	}

	m_GlThread.join();
	g_Cam.release();

	return 0;
}


void onMouse(int evt, int x, int y, int flags, void *param) {
	cv::Mat tmpImg = *(cv::Mat *)param;
	cv::Mat tmpGrayImg;
	cv::cvtColor(tmpImg, tmpGrayImg, cv::COLOR_BGR2GRAY);

	vector<cv::Point2f> l_ImgCorners;
	bool found = false;
	switch (evt) {
	case::CV_EVENT_FLAG_LBUTTON:
		found = cv::findChessboardCorners(tmpGrayImg, g_BoardSz, l_ImgCorners);
		if (!found)
			break;
		cv::cornerSubPix(tmpGrayImg, l_ImgCorners, cv::Size(5, 5), cv::Size(-1, -1),
			cv::TermCriteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 30, 0.001));
		cv::drawChessboardCorners(tmpImg, g_BoardSz, l_ImgCorners, found);
		if (l_ImgCorners.size() == g_BoardSz.area()) {
			g_ImgPoints.push_back(l_ImgCorners);
			g_ObjPoints.push_back(g_ObjCorners);
		}

		g_State++;

		cv::moveWindow(g_CapturedWin, 12 + tmpImg.cols, 10);
		cv::imshow(g_CapturedWin, tmpImg);
		break;
	default:
		break;
	}
}