/**	포함시켜야 할 것
*	Ceres solver
*	glog
*	gflag
*	Eigen
**/
#include "sfm.h"
#include <opencv2\viz.hpp>

#include <gl\glut.h>
#include <gl\GLU.h>
#include <gl\GL.h>

#include <iostream>
using namespace std;

void onMouseFuncCv(int evt, int x, int y, int flags, void *param);

sfmlib::SfM sfmtest(1.0);

//// GL
//int			Win_width = 1024, Win_height = 768;
//float		CameraFar = 300.0f, CameraX = 0, CameraY = 0, gRotateY = 0, gRotateX = 0;
//bool		StdVisiable = true;
//int			NewMouseX, NewMouseY, OldMouseX, OldMouseY, OldMouseXm, OldMouseYm;
//int			moveX, moveY;
//bool		G_Mouse_State[3];
//float		ChessBoardPoints[4][7];
//
//void display() {
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	glPushMatrix(); {
//		gluPerspective(45, (double)Win_width / Win_height, 0.00001, 10000);
//		gluLookAt(CameraX, CameraY, -CameraFar, CameraX, CameraY, 0, 0, -1, 0);
//		glRotated(gRotateY, 0, 1, 0);
//		glRotated(gRotateX, 1, 0, 0);
//
//		glPushMatrix(); {
//			glScaled(2, 2, 2);
//			glPointSize(2);
//			glBegin(GL_POINTS); {
//				for (const sfmlib::Point3DInMap& p : sfmtest.mReconstructionCloud) {
//					auto originatingView = p.originatingViews.begin();
//					const int viewIdx = originatingView->first;
//					cv::Point2f p2d = sfmtest.mImageFeatures[viewIdx].points[originatingView->second];
//					cv::Vec3b pointColor = sfmtest.mImages[viewIdx].at<cv::Vec3b>(p2d);
//					glColor3b((int)pointColor(2), (int)pointColor(1), (int)pointColor(0));
//					glVertex3f(p.p.x, p.p.y, p.p.z);
//					
//				}
//			}glEnd();
//
//			glScaled(0.2, 0.2, 0.2);
//			for (cv::Matx34f cm : sfmtest.mCameraPoses) {
//				glPushMatrix(); {
//					glTranslatef(-cm(0, 3), -cm(1, 3), -cm(2, 3));
//					float Rm[16] = {
//						cm(0,0), cm(0,1), cm(0,2), 0,
//						cm(1,0), cm(1,1), cm(1,2), 0,
//						cm(2,0), cm(2,1), cm(2,2), 0,
//							  0,       0,       0, 1
//					};
//					//float Rm[16] = {
//					//	cm(0,0), cm(0,1), cm(2,0), 0,
//					//	cm(0,1), cm(1,1), cm(2,1), 0,
//					//	cm(0,2), cm(1,2), cm(2,2), 0,
//					//	0,       0,       0, 1
//					//};
//					glMultMatrixf(Rm);
//					glTranslated(0, 0, -sfmtest.mIntrinsics.K.at<double>(0, 0));
//					glColor3f(120, 0, 120);
//					glBegin(GL_LINE_LOOP);
//					glVertex3f(-0.5, -0.5, 2);
//					glVertex3f(-0.5,  0.5, 2);
//					glVertex3f( 0.5,  0.5, 2);
//					glVertex3f( 0.5, -0.5, 2);
//					glEnd();
//
//					glBegin(GL_LINE_LOOP);
//					glVertex3f(0, 0, 0);
//					glVertex3f(0, 0, 2);
//					glEnd();
//
//					glColor3f(255, 255, 255);
//					glutSolidCube(0.3);
//				}glPopMatrix();
//			}
//			
//		}glPopMatrix();
//		
//		glBegin(GL_LINES); {
//			glColor3f(255, 0, 0);
//			glVertex3i(0, 0, 0);
//			glVertex3i(10000, 0, 0);
//
//			glColor3f(0, 255, 0);
//			glVertex3i(0, 0, 0);
//			glVertex3i(0, 10000, 0);
//
//			glColor3f(0, 0, 255);
//			glVertex3i(0, 0, 0);
//			glVertex3i(0, 0, 10000);
//		}glEnd();
//
//	}glPopMatrix();
//
//	glutSwapBuffers();
//}
//void MyMouseClick(int click, int state, int x, int y) {
//
//	if (click == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
//		G_Mouse_State[0] = true;
//		OldMouseX = x; OldMouseY = y;
//	} else if (click == GLUT_LEFT_BUTTON && state == GLUT_UP) {
//		G_Mouse_State[0] = false;
//	}
//	if (click == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
//		G_Mouse_State[1] = true;
//	} else if (click == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
//		G_Mouse_State[1] = false;
//	}
//	if (click == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
//		G_Mouse_State[2] = true;
//		OldMouseXm = x; OldMouseYm = y;
//	} else if (click == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
//		G_Mouse_State[2] = false;
//	}
//
//}
//void MyMouseMotion(int x, int y) {
//	GLfloat DeltaMouse;
//
//	if (G_Mouse_State[0]) {
//		NewMouseX = x; NewMouseY = y;
//		if (NewMouseX < OldMouseX) {
//			DeltaMouse = GLfloat(OldMouseX - NewMouseX);
//		} else if (NewMouseX > OldMouseX) {
//			DeltaMouse = GLfloat(NewMouseX - OldMouseX);
//		}
//		if (NewMouseY < OldMouseY) {
//			DeltaMouse = GLfloat(OldMouseY - NewMouseY);
//		} else if (NewMouseY > OldMouseY) {
//			DeltaMouse = GLfloat(NewMouseY - OldMouseY);
//		}
//		OldMouseX = x;
//		OldMouseY = y;
//	}
//	if (G_Mouse_State[2]) {
//
//	}
//
//}
//void MyReshape(int x, int y) {
//	Win_width = x; Win_height = y;
//	glViewport(0, 0, x, y);
//
//}
//void SkeyFunc(int key, int x, int y) {
//	switch (key) {
//	case GLUT_KEY_RIGHT:
//		gRotateY += 5;
//		break;
//	case GLUT_KEY_LEFT:
//		gRotateY -= 5;
//		break;
//	case GLUT_KEY_UP:
//		gRotateX += 5;
//		break;
//	case GLUT_KEY_DOWN:
//		gRotateX -= 5;
//		break;
//	case GLUT_KEY_PAGE_UP:
//		CameraFar *= 0.9;
//		break;
//	case GLUT_KEY_PAGE_DOWN:
//		CameraFar *= 1.1;
//		break;
//	case GLUT_KEY_F1:
//		exit(0);
//		break;
//	}
//
//}
//void KeyFunc(unsigned char key, int x, int y) {
//	switch (key) {
//	case 'w':
//		CameraY += 3;
//		break;
//	case 's':
//		CameraY -= 3;
//		break;
//	case 'a':
//		CameraX -= 3;
//		break;
//	case 'd':
//		CameraX += 3;
//		break;
//	case 't':
//		StdVisiable = !StdVisiable;
//		break;
//	case 27:
//		exit(0);
//		break;
//	default:
//		break;
//	}
//}

int main() {
	//cv::VideoCapture ml_Cam(0);
	//if (!ml_Cam.isOpened()) {
	//	cout << "Cam Open Fail" << endl;
	//	return 0;
	//}
	//cv::Mat ml_Frame, ml_UndistortedFrame;
	//ml_Cam >> ml_Frame;
	////cv::undistort(ml_Frame, ml_UndistortedFrame, sfmtest.mIntrinsics.K, sfmtest.mIntrinsics.distortion);
	//cv::namedWindow("Cam", cv::WINDOW_AUTOSIZE);
	//cv::moveWindow("Cam", 10, 10);
	////cv::imshow("Cam", ml_UndistortedFrame);
	//cv::imshow("Cam", ml_Frame);
	////cv::setMouseCallback("Cam", onMouseFuncCv, &ml_UndistortedFrame);
	//cv::setMouseCallback("Cam", onMouseFuncCv, &ml_Frame);
	//while (1) {
	//	if (cv::waitKey(40) == 27) break;
	//	ml_Cam >> ml_Frame;
	//	//cv::undistort(ml_Frame, ml_UndistortedFrame, sfmtest.mIntrinsics.K, sfmtest.mIntrinsics.distortion);
	//	//cv::imshow("Cam", ml_UndistortedFrame);
	//	cv::imshow("Cam", ml_Frame);
	//}

	sfmtest.mImages.push_back(cv::imread("P1000965.JPG"));
	sfmtest.mImages.push_back(cv::imread("P1000966.JPG"));
	sfmtest.mImages.push_back(cv::imread("P1000967.JPG"));
	sfmtest.mImages.push_back(cv::imread("P1000968.JPG"));
	//sfmtest.mImages.push_back(cv::imread("P1000969.JPG"));
	//sfmtest.mImages.push_back(cv::imread("P1000970.JPG"));
	//sfmtest.mImages.push_back(cv::imread("P1000971.JPG"));
	
	//sfmtest.mImages.push_back(cv::imread("kermit000.jpg"));
	//sfmtest.mImages.push_back(cv::imread("kermit001.jpg"));
	//sfmtest.mImages.push_back(cv::imread("kermit002.jpg"));
	//sfmtest.mImages.push_back(cv::imread("kermit003.jpg"));
	//sfmtest.mImages.push_back(cv::imread("kermit004.jpg"));
	//sfmtest.mImages.push_back(cv::imread("kermit005.jpg"));
	//sfmtest.mImages.push_back(cv::imread("kermit006.jpg"));
	//sfmtest.mImages.push_back(cv::imread("kermit007.jpg"));
	//sfmtest.mImages.push_back(cv::imread("kermit008.jpg"));
	//sfmtest.mImages.push_back(cv::imread("kermit009.jpg"));
	//sfmtest.mImages.push_back(cv::imread("kermit010.jpg"));
	
	if (sfmtest.mDownscaleFactor != 1.0) {
		cv::resize(sfmtest.mImages.back(), sfmtest.mImages.back(), cv::Size(), sfmtest.mDownscaleFactor, sfmtest.mDownscaleFactor);
	}

	if (sfmtest.mImages.back().empty()) {
		cerr << "Unable to read image from file: " << endl;
		return false;
	}

	sfmtest.runSfM();

	//sfmtest.saveCloudAndCamerasToPLY("test");

	cv::destroyAllWindows();

	cv::viz::Viz3d window("Coordinate Frame");
	window.setWindowSize(cv::Size(500, 500));
	window.setWindowPosition(cv::Point(150, 150));
	window.setBackgroundColor(); // black by default
	
	// Create the pointcloud
	cout << "Recovering points  ... ";

	// recover estimated points3d
	vector<cv::Vec3f> point_cloud_est;
	for (const sfmlib::Point3DInMap& p : sfmtest.mReconstructionCloud)
		point_cloud_est.push_back(cv::Vec3f(p.p.x, p.p.y, p.p.z));

	cout << "[DONE]" << endl;


	// Recovering cameras
	cout << "Recovering cameras ... ";

	vector<cv::Affine3d> path;
	for (size_t i = 0; i < sfmtest.mCameraPoses.size(); ++i) {
		cv::Mat tR = cv::Mat(sfmtest.mCameraPoses[i]).colRange(0, 3);
		cv::Mat tt = cv::Mat(sfmtest.mCameraPoses[i].col(3));
		tR.convertTo(tR, CV_64FC1);
		tt.convertTo(tt, CV_64FC1);

		cout << tR << endl << tt << endl << endl;

		path.push_back(cv::Affine3d(tR, tt));
	}

	cout << "[DONE]" << endl;


	// Add the pointcloud
	if (point_cloud_est.size() > 0) {
		cout << "Rendering points   ... ";

		cv::viz::WCloud cloud_widget(point_cloud_est, cv::viz::Color::green());
		window.showWidget("point_cloud", cloud_widget);

		cout << "[DONE]" << endl;
	} else {
		cout << "Cannot render points: Empty pointcloud" << endl;
	}

	cv::Matx33d K = cv::Matx33d(sfmtest.mIntrinsics.K.at<double>(0,0), 0, sfmtest.mIntrinsics.K.at<double>(0, 2),
		0, sfmtest.mIntrinsics.K.at<double>(1, 1), sfmtest.mIntrinsics.K.at<double>(1, 2),
		0, 0, 1);

	/// Add cameras
	if (path.size() > 0) {
		cout << "Rendering Cameras  ... ";

		window.showWidget("cameras_frames_and_lines", cv::viz::WTrajectory(path, cv::viz::WTrajectory::BOTH, 0.1, cv::viz::Color::green()));
		window.showWidget("cameras_frustums", cv::viz::WTrajectoryFrustums(path, K, 0.1, cv::viz::Color::yellow()));

		window.setViewerPose(path[0]);

		cout << "[DONE]" << endl;
	} else {
		cout << "Cannot render the cameras: Empty path" << endl;
	}

	//// Wait for key 'q' to close the window
	cout << endl << "Press 'q' to close each windows ... " << endl;

	window.spin();


	cv::waitKey(0);

	return 0;
}

void onMouseFuncCv(int evt, int x, int y, int flags, void *param) {
	cv::Mat l_clickedImg = *(cv::Mat *)param;
	switch (evt) {
	case::CV_EVENT_FLAG_LBUTTON:
		sfmtest.mImages.push_back(l_clickedImg.clone());

		break;
	default:
		break;
	}
}
