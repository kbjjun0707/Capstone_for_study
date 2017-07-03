// test 3D reconstruction from two images
// start 2017-03-08 iMiner
// Pipeline :
// 1. two-view reconstruction
//   1.1 Keypoints Detection -> Descriptor -> Point Match
//   1.2 find Fundamental Matrix, verify it
//   1.3 get Essential Matrix from F, verify it
//   1.4 E -> [R|t]
//   1.5 Triangulation
// 2. repeat step1 process for the other two views
// 3. The two point cloud is merged through the calibration of the Bundle adjustment

#pragma warning (disable:4996)

#include <opencv2\opencv.hpp>
#include <opencv2\xfeatures2d.hpp>
#include <opencv2\sfm.hpp>
#include <opencvLibs.h>

#include <gl/glut.h>
#include <gl/GLU.h>
#include <gl/GL.h>

#include <iostream>

using namespace std;

// CV
cv::Mat g_K, g_dC, g_R, g_t;
cv::Mat g_LImg, g_RImg, g_LtmpRst, g_RtmpRst;
vector<cv::Mat> tR, tt, tn;
vector<cv::detail::CameraParams> cameras;
cv::Mat_<double> LinearLSTriangulation(cv::Point3d u, cv::Matx34d P, cv::Point3d u1, cv::Matx34d P1);
vector<cv::Mat> g_X;
vector<cv::Vec3b> g_Color;

// GL
int		Win_width = 1024, Win_height = 768;
float	CameraFar = 3000.0f, gRotateY = 0, gRotateX = 0;
int		NewMouseX, NewMouseY, OldMouseX, OldMouseY, OldMouseXm, OldMouseYm;
bool	G_Mouse_State[3];
GLuint	Textures[2];

// file names of images
char *g_LImgName = "E:/workspace/stereo001L.jpg";
char *g_RImgName = "E:/workspace/stereo001R.jpg";

// GL function
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix(); {
		gluLookAt(0, 0, CameraFar, 0, 0, 0, 0, 1, 0);
		glRotatef(gRotateY, 0, 1, 0);
		glRotatef(gRotateX, 1, 0, 0);

		glPushMatrix(); {
			//int rows = g_LImg.rows, cols = g_LImg.cols;
			//glTranslated(-cols / 2.0, -rows / 2.0, 0);
			////glPointSize(3);
			////glBegin(GL_POINTS); {
			////	for (int r = 0; r < rows; r++) {
			////		for (int c = 0; c < cols; c++) {
			////			cv::Vec3d color = g_LImg.at<cv::Vec3d>(r, c);
			////			glColor3d(color[0] / 255.0, color[1] / 255.0, color[2] / 255.0);
			////			glVertex3i(c, rows - 1 - r, 1);
			////		}
			////	}
			////}glEnd();

			//glEnable(GL_TEXTURE_2D);
			//glBindTexture(GL_TEXTURE_2D, Textures[0]);
			//glBegin(GL_QUADS);
			//glTexCoord2f(0, 0); glVertex3d(0, 0, 0);
			//glTexCoord2f(1, 0); glVertex3d(cols, 0, 0);
			//glTexCoord2f(1, 1); glVertex3d(cols, rows, 0);
			//glTexCoord2f(0, 1); glVertex3d(0, rows, 0);
			//glEnd();
			//glDisable(GL_TEXTURE_2D);

			glScaled(3, 3, 3);
			glPointSize(2);
			glBegin(GL_POINTS); {
				for (int i = 0; i < g_X.size(); i++) {
					cv::Point3d point = g_X[i].at<cv::Point3d>(0, 0);
					cv::Vec3b color = g_Color[i];
					glColor3b(color[2], color[1], color[0]);
					glVertex3d(point.x, -point.y, point.z);
				}
			}glEnd();




		}glPopMatrix();

		//glPushMatrix(); {
		//	cv::Mat invR = g_R.inv();
		//	int rows = g_RImg.rows, cols = g_RImg.cols;
		//	glMultMatrixd((double *)g_R.data);
		//	glTranslated(-cols / 2.0, -rows / 2.0, 0.0);
		//	//glPointSize(3);
		//	//glBegin(GL_POINTS); {
		//	//	for (int r = 0; r < rows; r++) {
		//	//		for (int c = 0; c < cols; c++) {
		//	//			cv::Vec3d color = g_RImg.at<cv::Vec3d>(r, c);
		//	//			glColor3d(color[0] / 255.0, color[1] / 255.0, color[2] / 255.0);
		//	//			//glColor3d(255 / 255.0, 255 / 255.0, 0 / 255.0);
		//	//			double points[] = {
		//	//				c,
		//	//				rows - 1 - r,
		//	//				1
		//	//			};
		//	//			cv::Mat temp = cv::Mat(3, 1, CV_64FC1, points);
		//	//			temp = g_R*temp;
		//	//			temp = g_t+temp;
		//	//			glVertex3d(temp.at<double>(0, 0)+ 300, temp.at<double>(1, 0), temp.at<double>(2, 0));
		//	//		}
		//	//	}
		//	//}glEnd();

		//	glEnable(GL_TEXTURE_2D);
		//	glEnable(GL_TEXTURE_GEN_S);
		//	glEnable(GL_TEXTURE_GEN_T);
		//	glBindTexture(GL_TEXTURE_2D, Textures[1]);
		//	glBegin(GL_QUADS);
		//	glTexCoord2f(0, 0); glVertex3d(0, 0, 0);
		//	glTexCoord2f(1, 0); glVertex3d(cols, 0, 0);
		//	glTexCoord2f(1, 1); glVertex3d(cols, rows, 0);
		//	glTexCoord2f(0, 1); glVertex3d(0, rows, 0);
		//	glEnd();
		//	glDisable(GL_TEXTURE_2D);

		//}glPopMatrix();

		glPushMatrix(); {

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
	// step1
	// step1.1 Keypoints Detection -> Descriptor -> Point Match
	g_LImg = cv::imread(g_LImgName);
	g_RImg = cv::imread(g_RImgName);
	//cv::resize(g_LImg, g_LImg, cv::Size(), 0.12, 0.12);
	//cv::resize(g_RImg, g_RImg, cv::Size(), 0.12, 0.12);

	//double K_Data[] = {
	//	285.1070862006333,	               0,	g_LImg.cols/2.0,
	//	                0,	285.1070862006333,	g_LImg.rows/2.0,
	//	                0,	               0,                 1
	//};
	double K_Data[] = {
		759.738,            0,	251.326,
		      0,      758.256,	191.065,
		      0,            0,                 1
	};
	//double K_Data[] = {
	//	1,	               0,	g_LImg.cols / 2.0,
	//	0,	               1,	g_LImg.rows / 2.0,
	//	0,	               0,                 1
	//};
	g_K = cv::Mat(3, 3, CV_64FC1, K_Data);
	double dC_Data[] = { 0.0370662609916466,6, 0.05982037436156284, -0.004219657019491862, -0.01081838283093384, -0.96046615296765 };
	g_dC = cv::Mat(1, 5, CV_64FC1, dC_Data);
	cv::Mat tmpKm = cv::Mat::eye(3, 3, CV_64FC1);

	// cv Windows
	cv::namedWindow("left");
	cv::namedWindow("right");
	cv::namedWindow("result01");
	cv::namedWindow("result02");
	cv::namedWindow("result03");
	cv::moveWindow("left", 10, 10);
	cv::moveWindow("right", 10 + g_LImg.cols, 10);
	cv::destroyAllWindows();
	cv::imshow("left", g_LImg);
	cv::imshow("right", g_RImg);

	if (cv::waitKey(0) == 27)return 0;

	cv::Mat m_LGray, m_RGray;
	if (g_LImg.channels() == 3) {
		cv::cvtColor(g_LImg, m_LGray, CV_RGB2GRAY);
		cv::cvtColor(g_RImg, m_RGray, CV_RGB2GRAY);
	} else {
		m_LGray = g_LImg;
		m_RGray = g_RImg;
	}

	vector<cv::KeyPoint> m_Left_Keypoints, m_Right_Keypoints;

	cv::Ptr<cv::xfeatures2d::SURF> surf = cv::xfeatures2d::SURF::create();
	surf->detect(m_LGray, m_Left_Keypoints);
	surf->detect(m_RGray, m_Right_Keypoints);

	cv::Mat m_LDescript, m_RDescript;
	surf->compute(m_LGray, m_Left_Keypoints, m_LDescript);
	surf->compute(m_RGray, m_Right_Keypoints, m_RDescript);

	cout << "Number of SURF points (1): " << m_Left_Keypoints.size() << endl;
	cout << "Number of SURF points (2): " << m_Right_Keypoints.size() << endl;

	cv::drawKeypoints(g_LImg, m_Left_Keypoints, g_LtmpRst);
	cv::drawKeypoints(g_RImg, m_Right_Keypoints, g_RtmpRst);

	cv::moveWindow("result01", 10, 10 + g_LImg.rows);
	cv::imshow("result01", g_LtmpRst);
	cv::moveWindow("result02", 10 + g_LImg.cols, 10 + g_LImg.rows);
	cv::imshow("result02", g_RtmpRst);

	if (cv::waitKey(0) == 27)return 0;

	{
		//vector<cv::Point2f> m_Left_Points;
		//for (cv::KeyPoint kp : m_Left_Keypoints)
		//	m_Left_Points.push_back(kp.pt);

		//vector<cv::Point2f> m_Right_Points(m_Left_Points.size());

		//vector<uchar> vstatus;
		//vector<float> verror;
		//cv::calcOpticalFlowPyrLK(m_Prevgray, m_Gray, m_Left_Points, m_Right_Points, vstatus, verror);

		//vector<cv::Point2f> m_Right_Points_to_Find;
		//vector<int> m_Right_Points_to_Find_Back_Index;

		//for (unsigned int i = 0; i < vstatus.size(); i++) {
		//	if ((vstatus[i]) && (verror[i] < 12.0)) {
		//		m_Right_Points_to_Find_Back_Index.push_back(i);
		//		m_Right_Points_to_Find.push_back(m_Right_Points[i]);
		//	} else {
		//		vstatus[i] = 0;
		//	}
		//}

		//cv::Mat m_Right_Points_to_Find_Flat;// = cv::Mat(m_Right_Points_to_Find).reshape(1, m_Right_Points_to_Find.size());


		//vector<cv::Point2f> m_Right_Features;
		//for (cv::KeyPoint kp : m_Right_Keypoints)
		//	m_Left_Points.push_back(kp.pt);

		//cv::Mat m_Right_Features_Flat = cv::Mat(m_Right_Features).reshape(1, m_Right_Features.size());
	}
	
	cv::BFMatcher matcher(CV_L2);
	vector<vector<cv::DMatch>> nearest_neighbors;
	vector<cv::DMatch> matches;

	matcher.radiusMatch(
		m_LDescript,
		m_RDescript,
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


	cv::drawMatches(g_LImg, m_Left_Keypoints, g_RImg, m_Right_Keypoints, matches, g_LtmpRst);

	//g_LtmpRst = cv::Mat(cv::Size(g_LImg.cols * 2, g_LImg.rows), g_LImg.type());
	//g_LImg.copyTo(g_LtmpRst(cv::Rect(0, 0, g_LImg.cols, g_LImg.rows)));
	//g_RImg.copyTo(g_LtmpRst(cv::Rect(g_LImg.cols, 0, g_LImg.cols, g_LImg.rows)));
	//for (int i = 0; i < matches.size(); i++) {
	//	cv::Point a = cv::Point(m_Left_Keypoints[matches[i].queryIdx].pt);
	//	cv::Point b = cv::Point(m_Right_Keypoints[matches[i].trainIdx].pt + cv::Point2f(g_LImg.cols, 0));
	//	cv::circle(g_LtmpRst, a, 2, cv::Scalar(0, 0, 255));
	//	cv::circle(g_LtmpRst, b, 2, cv::Scalar(0, 0, 255));
	//	if( a.y== b.y)
	//		cv::line(g_LtmpRst, cv::Point(m_Left_Keypoints[matches[i].queryIdx].pt), cv::Point(m_Right_Keypoints[matches[i].trainIdx].pt + cv::Point2f(g_LImg.cols, 0)),
	//			cv::Scalar(0, 255, 0));
	//}
	
	cv::destroyWindow("result02");
	cv::moveWindow("result01", 10, 10 + g_LImg.rows);
	cv::imshow("result01", g_LtmpRst);
	
	if (cv::waitKey(0) == 27)return 0;

	// step1.2 find Fundamental Matrix, verify it
	vector<cv::Point2f> Limgpts, Rimgpts;
	for (unsigned int i = 0; i < matches.size(); i++) {
		Limgpts.push_back(m_Left_Keypoints[matches[i].queryIdx].pt);
		Rimgpts.push_back(m_Right_Keypoints[matches[i].trainIdx].pt);
	}
	vector<cv::Point3d> HomoLimgpts, HomoRimgpts;
	for (int i = 0; i < Limgpts.size(); i++) {
		HomoLimgpts.push_back(cv::Point3d(Limgpts[i].x, Limgpts[i].y, 1.0));
		HomoRimgpts.push_back(cv::Point3d(Rimgpts[i].x, Rimgpts[i].y, 1.0));
	}

	cv::Mat F = cv::findFundamentalMat(Limgpts, Rimgpts, CV_RANSAC);

	cout << "Fundamental1" << endl << F << endl
		<< "LptsNum : " << Limgpts.size() << "   "
		<< "RptsNum : " << Rimgpts.size() << endl << endl;
	cv::correctMatches(F, Limgpts, Rimgpts, Limgpts, Rimgpts);

	{
		//F = cv::findFundamentalMat(Limgpts, Rimgpts, CV_RANSAC);
		//cout << "Fundamental2" << endl << F << endl
		//	<< "LptsNum : " << Limgpts.size() << "   "
		//	<< "RptsNum : " << Rimgpts.size() << endl << endl;

		//F = cv::findFundamentalMat(Limgpts, Rimgpts, CV_FM_8POINT);
		//cout << "Fundamental3" << endl << F << endl
		//	<< "LptsNum : " << Limgpts.size() << "   "
		//	<< "RptsNum : " << Rimgpts.size() << endl << endl;
	}
	
	// test
	{
		double pnt1Data[] = {
			Limgpts[0].x,
			Limgpts[0].y,
			1
		};
		double pnt2Data[] = {
			Rimgpts[0].x,
			Rimgpts[0].y,
			1
		};


		cv::Mat tPoint1 = cv::Mat(3, 1, CV_64F, pnt1Data);
		cv::Mat tPoint2 = cv::Mat(3, 1, CV_64F, pnt2Data);
		
		cout << "Fundametal result : " << endl
			<< tPoint2.t()*F*tPoint1 << endl;

		//int count = 0;
		//for (int i = 0; i < Limgpts.size(); i++) {
		//	double tpnt1Data[] = {
		//		Limgpts[0].x,
		//		Limgpts[0].y,
		//		1
		//	};
		//	double tpnt2Data[] = {
		//		Rimgpts[0].x,
		//		Rimgpts[0].y,
		//		1
		//	};
		//	cv::Mat ttPoint1 = cv::Mat(3, 1, CV_64F, tpnt1Data);
		//	cv::Mat ttPoint2 = cv::Mat(1, 3, CV_64F, tpnt2Data);
		//	cv::Mat tmp = ttPoint2*F*ttPoint1;
		//	cout << tmp << endl;
		//	_sleep(10);
		//	if (tmp.at<double>(0, 0) == 0)
		//		count++;
		//}
		//cout << count << endl;
		cout << endl;
	}
	
	// step1.2 find Fundamental Matrix, verify it
	// x`.t()*F*x = 0 이 정상 0에 근접한 값이 출력 됨
	if (cv::waitKey(0) == 27)return 0;
	

	// step1.3 get Essential Matrix from F, verify it
	cv::Mat E = tmpKm.t() * F *tmpKm;
	cout << "Essential test 1 : " << endl
		<< E << endl << endl;

	cv::Mat tmpE = cv::findEssentialMat(Limgpts, Rimgpts, tmpKm, CV_RANSAC);
	cout << "Essential test 2 : " << endl
		<< tmpE << endl << endl;

	cv::Mat tmpE2;
	cv::sfm::essentialFromFundamental(F, g_K, g_K, tmpE2);
	cout << "Essential test 3 : " << endl
		<< tmpE2 << endl << endl;
	
	// step1.3 get Essential Matrix from F, verify it
	// 검증 불가...
	if (cv::waitKey(0) == 27)return 0;
	
	// step1.4 E -> [R|t]
	cv::SVD svd(E);
	cv::Matx33d W(
		0, -1, 0,
		1,  0, 0,
		0,  0, 1
	);
	cv::Matx33d Z(
		 0, 1, 0,
		-1, 0, 0,
		 0, 0, 0
	);
	cv::Mat S = svd.u * cv::Mat(Z) * svd.u.t();
	cv::Mat R = svd.u * cv::Mat(W) * svd.vt;
	cv::Mat_<double> t = svd.u.col(2);
	//t = -R.inv()*t;
	cv::Matx34d Pt1(
		R.at<double>(0, 0), R.at<double>(0, 1), R.at<double>(0, 2), t(0),
		R.at<double>(1, 0), R.at<double>(1, 1), R.at<double>(1, 2), t(1),
		R.at<double>(2, 0), R.at<double>(2, 1), R.at<double>(2, 2), t(2)
	);
	cv::Mat P1(Pt1);

	cout << "P1 : " << endl
		<< P1 << endl << endl;

	cv::SVD svd2(tmpE);
	cv::Matx33d W2(
		0, -1, 0,
		1, 0, 0,
		0, 0, 1
	);
	cv::Matx33d Z2(
		0, 1, 0,
		-1, 0, 0,
		0, 0, 0
	);
	cv::Mat S2 = svd2.u * cv::Mat(Z2) * svd2.u.t();
	cv::Mat R2 = svd2.u * cv::Mat(W2) * svd2.vt;
	cv::Mat_<double> t2 = svd2.u.col(2);
	//t2 = -R2.inv()*t2;
	cv::Matx34d Pt2(
		R2.at<double>(0, 0), R2.at<double>(0, 1), R2.at<double>(0, 2), t2(0),
		R2.at<double>(1, 0), R2.at<double>(1, 1), R2.at<double>(1, 2), t2(1),
		R2.at<double>(2, 0), R2.at<double>(2, 1), R2.at<double>(2, 2), t2(2)
	);
	cv::Mat P2(Pt2);

	cout << "P2 : " << endl
		<< P2 << endl << endl;

	cv::Mat tmpP1, tmpP2;
	cv::SVD svd3(tmpE2);
	cv::Matx33d W3(
		0, -1, 0,
		1, 0, 0,
		0, 0, 1
	);
	cv::Matx33d Z3(
		0, 1, 0,
		-1, 0, 0,
		0, 0, 0
	);
	cv::Mat S3 = svd3.u * cv::Mat(Z3) * svd2.u.t();
	cv::Mat R3 = svd3.u * cv::Mat(W3) * svd2.vt;
	cv::Mat_<double> t3 = svd3.u.col(2);
	//t2 = -R2.inv()*t2;
	cv::Matx34d Pt3(
		R3.at<double>(0, 0), R3.at<double>(0, 1), R3.at<double>(0, 2), t3(0),
		R3.at<double>(1, 0), R3.at<double>(1, 1), R3.at<double>(1, 2), t3(1),
		R3.at<double>(2, 0), R3.at<double>(2, 1), R3.at<double>(2, 2), t3(2)
	);
	cv::Mat P3(Pt3);

	cout << "P3 : " << endl
		<< P3 << endl << endl;


	//cv::Mat tmpCam, tmpR, tmpt;
	//cv::sfm::KRtFromProjection(tmpP2, tmpCam, tmpR, tmpt);
	cv::Mat LPointMat(Limgpts), RPointMat(Rimgpts);
	cout << "Size test : " << LPointMat.size() << endl << endl;
	//cv::Mat tmpR(3, 3, CV_64FC1), tmpt(3, 1, CV_64FC1);
	//cv::sfm::computeOrientation(Limgpts, Rimgpts, tmpR, tmpt, 20);
	//cout 
	//	//<< "tmpCam:" << endl
	//	//<< tmpCam << endl
	//	<< "tmpR:" << endl
	//	<< tmpR << endl
	//	<< "tmpt" << endl
	//	<< tmpt << endl << endl;

	cv::Mat R4, t4;
	cv::recoverPose(tmpE, Limgpts, Rimgpts, tmpKm, R4, t4);
	//t3 = -R3.inv()*t3;
	cout << R4 << endl << t4 << endl << endl;

	cv::Matx34d Pt4(
		R4.at<double>(0, 0), R4.at<double>(0, 1), R4.at<double>(0, 2), t4.at<double>(0, 0),
		R4.at<double>(1, 0), R4.at<double>(1, 1), R4.at<double>(1, 2), t4.at<double>(1, 0),
		R4.at<double>(2, 0), R4.at<double>(2, 1), R4.at<double>(2, 2), t4.at<double>(2, 0)
	);
	cv::Mat P4(Pt4);

	cv::Mat R5, t5;
	cv::recoverPose(tmpE2, Limgpts, Rimgpts, tmpKm, R5, t5);
	//t3 = -R3.inv()*t3;
	cout << R5 << endl << t5 << endl << endl;

	cv::Matx34d Pt5(
		R5.at<double>(0, 0), R5.at<double>(0, 1), R5.at<double>(0, 2), t5.at<double>(0, 0),
		R5.at<double>(1, 0), R5.at<double>(1, 1), R5.at<double>(1, 2), t5.at<double>(1, 0),
		R5.at<double>(2, 0), R5.at<double>(2, 1), R5.at<double>(2, 2), t5.at<double>(2, 0)
	);
	cv::Mat P5(Pt5);


	//double txdata[] = {
	//	                   0, -t3.at<double>(2, 0),  t3.at<double>(1, 0),
	//	 t3.at<double>(2, 0),                    0, -t3.at<double>(0, 0),
	//	-t3.at<double>(1, 0),  t3.at<double>(0, 0),                    0
	//};
	//cv::Mat tx(3, 3, CV_64FC1, txdata);

	//cout << (g_K.t().inv())*R3*tx*(g_K.inv()) << endl << endl;

	// step1.4 E -> [R|t]
	// E Mat가 맞는 쪽이 맞을 것이라는 추측
	if (cv::waitKey(0) == 27)return 0;

	// step1.5 Triangulation
	{
		double stddata1[] = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0
		};
		cv::Mat tmpdata1 = g_K*cv::Mat(3,4,CV_64FC1, stddata1);
		cv::Mat tmpdata2 = g_K*P5;
		cv::Matx34d tmpP1((double *)tmpdata1.data), tmpP2((double *)tmpdata2.data);
		cv::Point3d u, u1;
		for (int i = 0; i < HomoLimgpts.size(); i++) {
			 u = HomoLimgpts[i];
			u1 = HomoRimgpts[i];

			//cv::Mat_<double> um = g_K.inv()*cv::Mat_<double>(u);
			//u = um.at<cv::Point3d>(0, 0);
			//cv::Mat_<double> um1 = g_K.inv()*cv::Mat_<double>(u1);
			//u1 = um1.at<cv::Point3d>(0, 0);

			cv::Mat_<double> X = LinearLSTriangulation(u, tmpP1, u1, tmpP2);
			g_X.push_back(cv::Mat(X));
			g_Color.push_back(g_LImg.at<cv::Vec3b>(u.y, u.x));
			cout << X << endl;
		}
		//cv::triangulatePoints()
	}




	// step1.5 Triangulation
	if (cv::waitKey(0) == 27)return 0;


	// GL
	{
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
		glutInitWindowSize(Win_width, Win_height);
		glutInitWindowPosition(200, 130);
		glutCreateWindow("test");
		glClearColor(0.5, 0.0, 0.0, 0.0);

		glMatrixMode(GL_MODELVIEW);
		glEnable(GL_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glShadeModel(GL_SMOOTH);
		gluPerspective(45, (double)Win_width / Win_height, 0.0001, 100000);

		//cv::cvtColor(g_LImg, g_LImg, cv::COLOR_BGR2RGB);
		//
		//glGenTextures(1, &Textures[0]);
		//glBindTexture(GL_TEXTURE_2D, Textures[0]);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		////glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_DECAL);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_LImg.cols, g_LImg.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, g_LImg.data);
		//GLenum error;
		//while ((error = glGetError()) != GL_NO_ERROR);

		//glBindTexture(GL_TEXTURE_2D, 0);

		//좌측
		//glGenTextures(1, &Textures[0]);
		//glBindTexture(GL_TEXTURE_2D, Textures[0]);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_DECAL);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_LImg.cols, g_LImg.rows, 0, GL_RGB, GL_DOUBLE, g_LImg.data);

		//우측
		//glGenTextures(1, &Textures[1]);
		//glBindTexture(GL_TEXTURE_2D, Textures[1]);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_DECAL);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, g_RImg.cols, g_RImg.rows, 0, GL_RGB, GL_DOUBLE, g_RImg.data);


		glutDisplayFunc(display);
		glutIdleFunc(display);
		glutSpecialFunc(SkeyFunc);
		glutReshapeFunc(MyReshape);
		glutMouseFunc(MyMouseClick);
		glutMotionFunc(MyMouseMotion);

		glutMainLoop();
	}

	



	return 0;
}
//
////Triagulate points
//void TriangulatePoints(const vector<cv::Point3f>& pt_set1,
//	const vector<cv::Point3f>& pt_set2,
//	const cv::Mat& Kinv,
//	const cv::Matx34d& P,
//	const cv::Matx34d& P1,
//	vector<cv::Point3f>& pointcloud,
//	vector<cv::Point3f>& correspImg1Pt) {
//#ifdef __SFM__DEBUG__
//	vector depths;
//#endif
//
//	pointcloud.clear();
//	correspImg1Pt.clear();
//
//	cout << "Triangulating...";
//	double t = getTickCount();
//	unsigned int pts_size = pt_set1.size();
//#pragma omp parallel for
//	for (unsigned int i = 0; i        Point2f kp = pt_set1[i];
//		Point3d u(kp.x, kp.y, 1.0);
//		Mat_ um = Kinv * Mat_(u);
//		u = um.at(0);
//		Point2f kp1 = pt_set2[i];
//		Point3d u1(kp1.x, kp1.y, 1.0);
//		Mat_ um1 = Kinv * Mat_(u1);
//		u1 = um1.at(0);
//
//		Mat_ X = IterativeLinearLSTriangulation(u, P, u1, P1);
//
//		//      if(X(2) > 6 || X(2) < 0) continue;
//
//#pragma omp critical
//	{
//		pointcloud.push_back(Point3d(X(0), X(1), X(2)));
//		correspImg1Pt.push_back(pt_set1[i]);
//#ifdef __SFM__DEBUG__
//		depths.push_back(X(2));
//#endif
//	}
//}
//t = ((double)getTickCount() - t) / getTickFrequency();
//cout << "Done. ("<
//	//show "range image"
//#ifdef __SFM__DEBUG__
//{
//	double minVal, maxVal;
//	minMaxLoc(depths, &minVal, &maxVal);
//	Mat tmp(240, 320, CV_8UC3); //cvtColor(img_1_orig, tmp, CV_BGR2HSV);
//	for (unsigned int i = 0; i            double _d = MAX(MIN((pointcloud[i].z - minVal) / (maxVal - minVal), 1.0), 0.0);
//		circle(tmp, correspImg1Pt[i], 1, Scalar(255 * (1.0 - (_d)), 255, 255), CV_FILLED);
//}
//cvtColor(tmp, tmp, CV_HSV2BGR);
//imshow("out", tmp);
//waitKey(0);
//destroyWindow("out");
//	}
//#endif
//}



cv::Mat_<double> LinearLSTriangulation(cv::Point3d u,       //homogenous image point (u,v,1)
	cv::Matx34d P,       //camera 1 matrix
	cv::Point3d u1,      //homogenous image point in 2nd camera
	cv::Matx34d P1       //camera 2 matrix
) {
	//build matrix A for homogenous equation system Ax = 0
	//assume X = (x,y,z,1), for Linear-LS method
	//which turns it into a AX = B system, where A is 4x3, X is 3x1 and B is 4x1
	cv::Matx43d A(
		 u.x*P(2, 0)  - P(0, 0),    u.x*P(2, 1) -  P(0, 1),  u.x*P(2, 2)   - P(0, 2),
		 u.y*P(2, 0)  - P(1, 0),    u.y*P(2, 1) -  P(1, 1),  u.y*P(2, 2)   - P(1, 2),
		u1.x*P1(2, 0) - P1(0, 0), u1.x*P1(2, 1) - P1(0, 1), u1.x*P1(2, 2) - P1(0, 2),
		u1.y*P1(2, 0) - P1(1, 0), u1.y*P1(2, 1) - P1(1, 1), u1.y*P1(2, 2) - P1(1, 2)
	);
	cv::Mat_<double> B = (cv::Mat_<double>(4, 1) << -(u.x*P(2, 3) - P(0, 3)),
		-( u.y* P(2, 3) -  P(1, 3)),
		-(u1.x*P1(2, 3) - P1(0, 3)),
		-(u1.y*P1(2, 3) - P1(1, 3)));

	cv::Mat_<double> X;
	cv::solve(A, B, X, cv::DECOMP_SVD);

	return X;
}
