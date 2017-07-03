#include <opencv2\opencv.hpp>
#include <opencv2\xfeatures2d.hpp>
#include <opencvInit.h>

#include <Windows.h>

#include <gl\glut.h>
#include <gl\GLU.h>
#include <gl\GL.h>
#include "Quaternion.h"

#include <iostream>
#include <thread>
using namespace std;

namespace iMiner {
	// std::vector
	// opencv
	class CameraCalibrator {
		std::vector<std::vector<cv::Point3f>> objectPoints;
		std::vector<std::vector<cv::Point2f>> imagePoints;
		int flag;
		cv::Mat map1, map2;
		bool mustInitUndistort;
		int CellSize;
	public:
		cv::Mat cameraMatrix;
		cv::Mat distCoeffs;
		CameraCalibrator(int chessBoardCellSizemm) :flag(0), mustInitUndistort(true) {
			this->CellSize = chessBoardCellSizemm;
		};
		int addChessboardPoints(const std::vector<std::string>& filelist, cv::Size &boardSize) {
			std::vector<cv::Point2f> imageCorners;
			std::vector<cv::Point3f> objectCorners;
			for (int i = 0; i < boardSize.height; i++)
				for (int j = 0; j < boardSize.width; j++)
					objectCorners.push_back(cv::Point3f(i, j, 0.0f));

			cv::Mat image;
			int successes = 0;
			for (int i = 0; i < filelist.size(); i++) {
				image = cv::imread(filelist[i], 0);
				bool found = cv::findChessboardCorners(image, boardSize, imageCorners);
				if (found) {
					cv::cornerSubPix(image, imageCorners, cv::Size(CellSize, CellSize), cv::Size(-1, -1),
						cv::TermCriteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 30, 0.001));
				}
				if (imageCorners.size() == boardSize.area()) {
					imagePoints.push_back(imageCorners);
					objectPoints.push_back(objectCorners);
					//cv::imwrite(cv::format("E:/workspace/calibrationTest/img%02d.jpg", successes), image);
					successes++;
				}
			}
			return successes;
		}
		int addChessboardPoints(const std::vector<cv::Mat>& imagelist, cv::Size &boardSize) {
			std::vector<cv::Point2f> imageCorners;
			std::vector<cv::Point3f> objectCorners;
			for (int i = 0; i < boardSize.height; i++)
				for (int j = 0; j < boardSize.width; j++)
					objectCorners.push_back(cv::Point3f((float)i, (float)j, 0.0f));

			cv::Mat image;
			int successes = 0;
			for (int i = 0; i < imagelist.size(); i++) {
				image = imagelist[i];
				cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
				bool found = cv::findChessboardCorners(image, boardSize, imageCorners);
				cv::cornerSubPix(image, imageCorners, cv::Size(CellSize, CellSize), cv::Size(-1, -1),
					cv::TermCriteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 30, 0.1));
				if (imageCorners.size() == boardSize.area()) {
					imagePoints.push_back(imageCorners);
					objectPoints.push_back(objectCorners);
					//cv::imwrite(cv::format("E:/workspace/calibrationTest/img%02d.jpg", successes), image);
					successes++;
				}
			}
			return successes;
		}

		double calibrate(cv::Size &imageSize) {
			mustInitUndistort = true;
			std::vector<cv::Mat> rvecs, tvecs;

			return cv::calibrateCamera(
				objectPoints, imagePoints, imageSize,
				cameraMatrix, distCoeffs,
				rvecs, tvecs, flag);
		}
		cv::Mat remap(const cv::Mat &image) {
			cv::Mat remapMat;
			if (mustInitUndistort) {
				cv::initUndistortRectifyMap(
					cameraMatrix, distCoeffs,
					cv::Mat(), cv::Mat(),
					image.size(), CV_32FC1,
					map1, map2);
				mustInitUndistort = false;
			}
			cv::remap(image, remapMat, map1, map2, cv::INTER_LINEAR);
			return remapMat;
		}
		void outputFile(const char* fileName) {
			ofstream output(fileName);
			output << "# CameraMatrix 3X3, distCoeffs 1X5" << endl << endl;
			output << cameraMatrix << endl << endl
				<< distCoeffs << endl << endl;

			output.close();
		}
		cv::Mat undistort(const cv::Mat &image) {
			cv::Mat undistortMat;
			cv::undistort(image, undistortMat, this->cameraMatrix, this->distCoeffs);
			return undistortMat;
		}
	};

	class Feature {
	public:
		cv::Ptr<cv::xfeatures2d::SURF> mDetector;
		Feature() {
			mDetector = cv::xfeatures2d::SURF::create();
		}
		~Feature() {}
		void detectNcompute(vector<cv::Mat> &pImgs, vector<vector<cv::KeyPoint>> &pKpnts, vector<cv::Mat> &pDescriptors) {
			mDetector->detect(pImgs, pKpnts);
			mDetector->compute(pImgs, pKpnts, pDescriptors);
		}
	};
}

void onMouse(int evt, int x, int y, int flags, void *param);

class CamInfo {
public:
	cv::Mat m_Imgs[2];
	cv::Mat m_R;
	cv::Mat m_t;
	vector<cv::KeyPoint> m_Kpnt;
	cv::Mat m_Descriptor;
};

// CV
cv::Mat g_K, g_dC, g_R, g_t;
vector<CamInfo> gCamInfos;
bool gFresh = true;
int gTurn = 0, g_CapSize = 0;


// GL
int			Win_width = 1024, Win_height = 768;
float		CameraFar = 1000.0f, CameraX=0, CameraY=0, gRotateY = 0, gRotateX = 0;
bool		StdVisiable = true;
int			NewMouseX, NewMouseY, OldMouseX, OldMouseY, OldMouseXm, OldMouseYm;
int			moveX, moveY;
bool		G_Mouse_State[3];
float		ChessBoardPoints[4][7];

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix(); {
		gluPerspective(45, (double)Win_width / Win_height, 0.0001, 10000);
		gluLookAt(CameraX, CameraY, -CameraFar, CameraX, CameraY, -CameraFar + 1000, 0, -1, 0);
		glRotated(gRotateY, 0, 1, 0);
		glRotated(gRotateX, 1, 0, 0);

		glPushMatrix(); {
			if (!gFresh && StdVisiable) {
				double *stdR = (double *)gCamInfos[0].m_R.data;
				double stdR44[16] = {
					stdR[0], stdR[1], stdR[2], 0,
					stdR[3], stdR[4], stdR[5], 0,
					stdR[6], stdR[7], stdR[8], 0,
						  0,       0,       0, 1
				};
				glMultMatrixd(stdR44);
				glColor3f(0.6, 0, 0.6);
				glutSolidCube(5.2);

				glBegin(GL_LINES); {
					glColor3f(1, 1, 1);
					glVertex3i(0, 0, 0);
					glVertex3i(0, 0, -10000);
				}glEnd();
			}
		}glPopMatrix();

		for (int i = 1; i < gCamInfos.size(); i++) {
			glPushMatrix(); {
				cv::Mat Rtran = gCamInfos[i].m_R.t();
				double *tR = (double *)Rtran.data;
				double *tt = (double *)gCamInfos[i].m_t.data;
				//double *tR = (double *)gCamInfos[i].m_R.data;
				double tR44[16] = {
					tR[0], tR[1], tR[2],  0,
					tR[3], tR[4], tR[5],  0,
					tR[6], tR[7], tR[8],  0,
					    0,     0,     0,  1
				};
				//double tt44[16] = {
				//	1, 0, 0, tt[0],
				//	0, 1, 0, tt[1],
				//	0, 0, 1, tt[2],
				//	0, 0, 0,     0
				//};
				//glMultMatrixd(tt44);
				glTranslated(tt[0], tt[1], tt[2]);
				glMultMatrixd(tR44);
				glColor3f(0.0, 0.7, 0.0);
				glutSolidCube(5);
				glBegin(GL_LINES); {
					glColor3f(1, 1, 1);
					glVertex3i(0, 0, 0);
					glVertex3i(0, 0, -10000);
				}glEnd();
			}glPopMatrix();
		}


		glBegin(GL_LINES); {
			glColor3f(255, 0, 0);
			glVertex3i(0, 0, 0);
			glVertex3i(10000, 0, 0);

			glColor3f(0, 255, 0);
			glVertex3i(0, 0, 0);
			glVertex3i(0, 10000, 0);

			glColor3f(0, 0, 255);
			glVertex3i(0, 0, 0);
			glVertex3i(0, 0, 10000);
		}glEnd();
		
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
		CameraFar -= 10;
		break;
	case GLUT_KEY_PAGE_DOWN:
		CameraFar += 10;
		break;
	case GLUT_KEY_F1:
		exit(0);
		break;
	}

}
void KeyFunc(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
		CameraY += 10;
		break;
	case 's':
		CameraY -= 10;
		break;
	case 'a':
		CameraX -= 10;
		break;
	case 'd':
		CameraX += 10;
		break;
	case 't':
		StdVisiable = !StdVisiable;
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}
}

int main() {
	std::thread glThread([&]() {
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
		glutInitWindowSize(Win_width, Win_height);
		glutInitWindowPosition(200, 130);
		glutCreateWindow("Simulation");
		glClearColor(0.1, 0.1, 0.1, 1.0);

		glMatrixMode(GL_MODELVIEW);
		glEnable(GL_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glShadeModel(GL_SMOOTH);


		glutDisplayFunc(display);
		glutIdleFunc(display);
		glutSpecialFunc(SkeyFunc);
		glutKeyboardFunc(KeyFunc);
		glutReshapeFunc(MyReshape);
		glutMouseFunc(MyMouseClick);
		glutMotionFunc(MyMouseMotion);

		glutMainLoop();
	});

	double K_Data[] = {
		759.738,            0,	251.326,
		0,      758.256,	191.065,
		0,            0,                 1
	};
	g_K = cv::Mat(3, 3, CV_64FC1, K_Data);
	double dC_Data[] = { 0.0295856678258018, -0.4875803173620251, -0.008405564255234423, -0.01679550574929698, 1.40807775361489 };
	g_dC = cv::Mat(1, 5, CV_64FC1, dC_Data);

	cv::namedWindow("Cam");
	cv::moveWindow("Cam", 10, 10);
	cv::namedWindow("std");
	cv::namedWindow("test");
	cv::VideoCapture mlCam(0);
	bool mlCapturing = mlCam.isOpened();
	cv::Mat mlFrame, mlUndistorFrame;
	mlCam >> mlFrame;
	cv::undistort(mlFrame, mlUndistorFrame, g_K, g_dC);
	cv::setMouseCallback("Cam", onMouse, &mlUndistorFrame);
	cv::moveWindow("std", 10, 10+mlFrame.rows);
	cv::moveWindow("test", 10+mlFrame.cols, 10 + mlFrame.rows);
	cv::destroyWindow("std");
	cv::destroyWindow("test");
	cv::imshow("Cam", mlUndistorFrame);

	cv::Ptr<cv::xfeatures2d::SURF> mlDetector = cv::xfeatures2d::SURF::create();

	while (mlCapturing) {
		mlCam >> mlFrame;
		cv::undistort(mlFrame, mlUndistorFrame, g_K, g_dC);
		cv::imshow("Cam", mlUndistorFrame);
		if (gFresh) {
			cv::destroyWindow("std");
			cv::destroyWindow("test");
		} else {
			cv::imshow("std", gCamInfos[0].m_Imgs[gTurn]);
			if (g_CapSize > 1)
				cv::imshow("test", gCamInfos[g_CapSize - 1].m_Imgs[gTurn]);
		}

		CamInfo stdci;
		switch (cv::waitKey(40)) {
		case 'R':
		case 'r':
			gCamInfos.clear();
			gFresh = true;
			StdVisiable = true;
			break;
		case 'T':
		case 't':
			gTurn = (gTurn + 1) % 2;
			break;
		case 27:
			mlCapturing = false;
			break;
		default:
			break;
		}
	}


	glThread.join();

	return 0;
}

void onMouse(int evt, int x, int y, int flags, void *param) {
	cv::Mat tmpImg = *(cv::Mat *)param;
	CamInfo tcaminfo;
	switch (evt) {
	case::CV_EVENT_FLAG_LBUTTON:
		if (gFresh) {
			tcaminfo.m_Imgs[0] = tmpImg.clone();
			tcaminfo.m_R = cv::Mat::eye(3, 3, CV_64FC1);
			tcaminfo.m_t = cv::Mat::zeros(3, 1, CV_64FC1);
			cv::Ptr<cv::xfeatures2d::SURF> Surf = cv::xfeatures2d::SURF::create();
			Surf->detect(tcaminfo.m_Imgs[0], tcaminfo.m_Kpnt);
			cv::drawKeypoints(tcaminfo.m_Imgs[0], tcaminfo.m_Kpnt, tcaminfo.m_Imgs[1]);
			Surf->compute(tcaminfo.m_Imgs[0], tcaminfo.m_Kpnt, tcaminfo.m_Descriptor);
			Surf.release();
			gCamInfos.push_back(tcaminfo);
			gFresh = false;
		} else {
			tcaminfo.m_Imgs[0] = tmpImg.clone();
			tcaminfo.m_R = cv::Mat::eye(3, 3, CV_64FC1);
			tcaminfo.m_t = cv::Mat::zeros(3, 1, CV_64FC1);
			cv::Ptr<cv::xfeatures2d::SURF> Surf = cv::xfeatures2d::SURF::create();
			Surf->detect(tcaminfo.m_Imgs[0], tcaminfo.m_Kpnt);
			cv::drawKeypoints(tcaminfo.m_Imgs[0], tcaminfo.m_Kpnt, tcaminfo.m_Imgs[1]);
			Surf->compute(tcaminfo.m_Imgs[0], tcaminfo.m_Kpnt, tcaminfo.m_Descriptor);

			cv::BFMatcher matcher(CV_L2);
			vector<vector<cv::DMatch>> nearest_neighbors;
			vector<cv::DMatch> matches;
			matcher.radiusMatch(
				gCamInfos[0].m_Descriptor,
				tcaminfo.m_Descriptor,
				nearest_neighbors,
				2.0f
			);			
			std::set<int> found_in_right_points;
			for (int i = 0; i < nearest_neighbors.size(); i++) {
				cv::DMatch _m;
				if (nearest_neighbors[i].size() == 1) {
					_m = nearest_neighbors[i][0];
				} else if (nearest_neighbors[i].size() > 1) {
					double ratio = nearest_neighbors[i][0].distance / nearest_neighbors[i][1].distance;
					if (ratio < 0.7) {
						_m = nearest_neighbors[i][0];
					} else {
						continue;
					}
				} else {
					continue;
				}
				if (found_in_right_points.find(_m.trainIdx) == found_in_right_points.end()) {		// find 없으면 end 반환  중복제거
					matches.push_back(_m);
					found_in_right_points.insert(_m.trainIdx);
				}
			}
			vector<cv::Point2f> Limgpts, Rimgpts;
			for (unsigned int i = 0; i < matches.size(); i++) {
				Limgpts.push_back(gCamInfos[0].m_Kpnt[matches[i].queryIdx].pt);
				Rimgpts.push_back(tcaminfo.m_Kpnt[matches[i].trainIdx].pt);
			}			
			//cv::Mat H = cv::findHomography(Rimgpts, Limgpts, cv::RANSAC);
			////cv::warpPerspective(tmpImg, tmpImg, H, tmpImg.size());
			////cout << H << endl << endl;
			//tcaminfo.m_R = g_K.inv()*H*g_K;
			////cout << tcaminfo.m_R << endl << endl;
			cv::Mat F = cv::findFundamentalMat(Limgpts, Rimgpts);
			cv::Mat E = g_K.t()*F*g_K;
			cv::SVD svd(E);
			cv::Mat W = cv::Mat::zeros(3, 3, CV_64FC1);
			W.at<double>(0, 1) = -1;
			W.at<double>(1, 0) =  1;
			W.at<double>(2, 2) =  1;
			cout //<< svd. << endl
				<< svd.u << endl
				<< svd.u.col(2) << endl << endl;
			tcaminfo.m_R = svd.u*W*svd.vt;
			tcaminfo.m_t = -svd.u.col(2);
			//tcaminfo.m_t = tcaminfo.m_t.mul(200);
			//cout << tcaminfo.m_t.mul << endl << endl;
			Surf.release();
			gCamInfos.push_back(tcaminfo);
		}
		g_CapSize++;
		break;
	default:
		break;
	}
}