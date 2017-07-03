#include <opencv2\opencv.hpp>
#include <opencv2\xfeatures2d.hpp>
#include <opencv2\stitching\detail\motion_estimators.hpp>
#include "opencv2\stitching/detail\matchers.hpp"
#include <opencv2\stitching\detail\camera.hpp>
#include <opencvLibs.h>

#include <gl/glut.h>
#include <gl/GLU.h>
#include <gl/GL.h>

#include <iostream>

using namespace std;

// CV
cv::Mat g_K, g_dC, g_R, g_t;
cv::Mat g_LImg, g_RImg;
vector<cv::Mat> tR, tt, tn;
vector<cv::detail::CameraParams> cameras;

// GL
int		Win_width = 1024, Win_height = 768;
float	CameraFar = 3000.0f, gRotateY = 0, gRotateX = 0;
int		NewMouseX, NewMouseY, OldMouseX, OldMouseY, OldMouseXm, OldMouseYm;
int		moveX, moveY; //객체 이동 
bool	G_Mouse_State[3];
float	ChessBoardPoints[4][7];

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix(); {
		gluLookAt(0, 0, CameraFar, 0, 0, 0, 0, 1, 0);
		glRotatef(gRotateY, 0, 1, 0);
		glRotatef(gRotateX, 1, 0, 0);

		glPushMatrix(); {
			int rows = g_LImg.rows, cols = g_LImg.cols;
			glTranslated(-cols / 2.0, -rows / 2.0, 0);
			glPointSize(3);
			glBegin(GL_POINTS); {
				for (int r = 0; r < rows; r++) {
					for (int c = 0; c < cols; c++) {
						cv::Vec3d color = g_LImg.at<cv::Vec3d>(r, c);
						glColor3d(color[2] / 255.0, color[1] / 255.0, color[0] / 255.0);
						glVertex3i(c, rows - 1 - r, 0);
					}
				}
			}glEnd();

		}glPopMatrix();

		glPushMatrix(); {
			int rows = g_RImg.rows, cols = g_RImg.cols;
			glTranslated(-cols / 2.0, -rows / 2.0, 0.0);
			glPointSize(3);
			glBegin(GL_POINTS); {
				for (int r = 0; r < rows; r++) {
					for (int c = 0; c < cols; c++) {
						cv::Vec3d color = g_RImg.at<cv::Vec3d>(r, c);
						glColor3d(color[2] / 255.0, color[1] / 255.0, color[0] / 255.0);
						//glColor3d(255 / 255.0, 255 / 255.0, 0 / 255.0);
						float points[] = {
							c,
							rows - 1 - r,
							0
						};
						cv::Mat temp = cv::Mat(3, 1, CV_32FC1, points);
						temp = cameras[0].R*temp;
						glVertex3f(temp.at<float>(0, 0), temp.at<float>(1, 0), temp.at<float>(2, 0));
					}
				}
			}glEnd();

		}glPopMatrix();

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

char *g_LImgName = "D:/workspace/stereo001L.jpg";
char *g_RImgName = "D:/workspace/stereo001R.jpg";

bool checkCoherentRotation(cv::Mat_<double> &R) {
	if (fabsf(cv::determinant(R)) - 1.0 > 1e-07) {
		cerr << "det(R) != +-1.0, this is not a rotation matrix" << endl;
		return false;
	}
	return true;
}

//void findCameraMatrices(const cv::Mat &K, const cv::Mat &Kinv, const vector<cv::KeyPoint> &imgpts1, const vector<cv::KeyPoint> &imgpts2, cv::Matx34d &P, cv::Matx34d &P1);

int main(int argc, char **argv) {

	double K_Data[] = {
		722.4746521224624,    0,			   270.9539526130284,
		0,					722.1833532044184, 183.6054680066445,
		0,					  0,				 1
	};
	g_K = cv::Mat(3, 3, CV_64FC1, K_Data).clone();
	double dC_Data[] = { 0.03706626099164666, 0.05982037436156284, -0.004219657019491862, -0.01081838283093384, -0.96046615296765 };
	g_dC = cv::Mat(1, 5, CV_64FC1, dC_Data).clone();

	g_LImg = cv::imread(g_LImgName);
	g_RImg = cv::imread(g_RImgName);
	
	cv::namedWindow("left");
	cv::namedWindow("right");
	cv::namedWindow("result01");
	cv::namedWindow("result02");
	cv::namedWindow("result03");
	cv::moveWindow("left", 10, 10);
	cv::moveWindow("right", 10 + g_LImg.cols, 10);
	cv::moveWindow("result01", 10, 10 + g_LImg.rows);
	cv::destroyAllWindows();
	cv::imshow("left", g_LImg);
	cv::imshow("right", g_RImg);

	cv::Mat m_Prevgray, m_Gray;
	if (g_LImg.channels() == 3) {
		cv::cvtColor(g_LImg, m_Prevgray, CV_RGB2GRAY);
		cv::cvtColor(g_RImg, m_Gray, CV_RGB2GRAY);
	} else {
		m_Prevgray = g_LImg;
		m_Gray = g_RImg;
	}

	{

		//vector<cv::KeyPoint> m_Left_Keypoints, m_Right_Keypoints;

		////cv::Ptr<cv::xfeatures2d::SIFT> sift = cv::xfeatures2d::SIFT::create(1000);
		//cv::Ptr<cv::xfeatures2d::SURF> surf = cv::xfeatures2d::SURF::create();
		//surf->detect(m_Prevgray, m_Left_Keypoints);
		//surf->detect(m_Gray, m_Right_Keypoints);

		//cv::Mat m_LDescript, m_RDescript;
		//surf->compute(m_Prevgray, m_Left_Keypoints, m_LDescript);
		//surf->compute(m_Gray, m_Right_Keypoints, m_RDescript);

		//{
		//	//vector<cv::Point2f> m_Left_Points;
		//	//for (cv::KeyPoint kp : m_Left_Keypoints)
		//	//	m_Left_Points.push_back(kp.pt);

		//	//vector<cv::Point2f> m_Right_Points(m_Left_Points.size());

		//	//vector<uchar> vstatus;
		//	//vector<float> verror;
		//	//cv::calcOpticalFlowPyrLK(m_Prevgray, m_Gray, m_Left_Points, m_Right_Points, vstatus, verror);

		//	//vector<cv::Point2f> m_Right_Points_to_Find;
		//	//vector<int> m_Right_Points_to_Find_Back_Index;

		//	//for (unsigned int i = 0; i < vstatus.size(); i++) {
		//	//	if ((vstatus[i]) && (verror[i] < 12.0)) {
		//	//		m_Right_Points_to_Find_Back_Index.push_back(i);
		//	//		m_Right_Points_to_Find.push_back(m_Right_Points[i]);
		//	//	} else {
		//	//		vstatus[i] = 0;
		//	//	}
		//	//}

		//	//cv::Mat m_Right_Points_to_Find_Flat;// = cv::Mat(m_Right_Points_to_Find).reshape(1, m_Right_Points_to_Find.size());


		//	//vector<cv::Point2f> m_Right_Features;
		//	//for (cv::KeyPoint kp : m_Right_Keypoints)
		//	//	m_Left_Points.push_back(kp.pt);

		//	//cv::Mat m_Right_Features_Flat = cv::Mat(m_Right_Features).reshape(1, m_Right_Features.size());
		//}

		//cv::BFMatcher matcher(CV_L2);
		//vector<vector<cv::DMatch>> nearest_neighbors;
		//vector<cv::DMatch> matches;

		//matcher.radiusMatch(
		//	m_LDescript,
		//	m_RDescript,
		//	nearest_neighbors,
		//	2.0f
		//);
		//
		//std::set<int> found_in_right_points;
		//for (int i = 0; i < nearest_neighbors.size(); i++) {
		//	cv::DMatch _m;
		//	if (nearest_neighbors[i].size() == 1) {
		//		_m = nearest_neighbors[i][0];
		//	} else if (nearest_neighbors[i].size() > 1) {
		//		double ratio = nearest_neighbors[i][0].distance / nearest_neighbors[i][1].distance;
		//		if (ratio < 0.7) {
		//			_m = nearest_neighbors[i][0];
		//		} else {
		//			continue;
		//		}
		//	} else {
		//		continue;
		//	}
		//	if (found_in_right_points.find(_m.trainIdx) == found_in_right_points.end()) {		// find 없으면 end 반환  중복제거
		//		matches.push_back(_m);
		//		found_in_right_points.insert(_m.trainIdx);
		//	}
		//}


		//vector<cv::Point2f> Limgpts, Rimgpts;
		//for (unsigned int i = 0; i < matches.size(); i++) {
		//	Limgpts.push_back(m_Left_Keypoints[matches[i].queryIdx].pt);
		//	Rimgpts.push_back(m_Right_Keypoints[matches[i].trainIdx].pt);
		//}

		//cv::Mat F = cv::findFundamentalMat(Limgpts, Rimgpts, CV_RANSAC);
		//cv::Mat_<double> E = g_K.t() * F *g_K;

		//cv::SVD svd(E);
		//cv::Matx33d W(
		//	0, -1, 0,
		//	1,  0, 0,
		//	0,  0, 1
		//);
		//cv::Mat_<double> R = svd.u * cv::Mat(W) * svd.vt;
		//cv::Mat_<double> t = svd.u.col(2);
		//cv::Matx34d P1(
		//	R(0, 0), R(0, 1), R(0, 2), t(0),
		//	R(1, 0), R(1, 1), R(1, 2), t(1),
		//	R(2, 0), R(2, 1), R(2, 2), t(2)
		//);


		//cv::Mat H1, H2;
		//cv::stereoRectifyUncalibrated(Limgpts, Rimgpts, F, g_LImg.size(), H1, H2);

		//cout << H1 << endl << endl << endl << H2 << endl << endl;
		//cv::Mat testH = cv::findHomography(Rimgpts, Limgpts);
	}

	cv::Ptr<cv::detail::FeaturesFinder> finder;

	finder = cv::makePtr<cv::detail::SurfFeaturesFinder>();

	cv::Mat full_img, img;
	int imgnum = 2;
	vector<cv::detail::ImageFeatures> features(imgnum);
	vector<cv::Mat> images(imgnum);
	vector<cv::Size> full_img_sizes(imgnum);

	full_img = g_LImg;
	full_img_sizes[0] = full_img.size();
	img = full_img.clone();
	(*finder)(img, features[0]);
	features[0].img_idx = 0;
	images[0] = img.clone();
	full_img = g_RImg;
	full_img_sizes[1] = full_img.size();
	img = full_img.clone();
	(*finder)(img, features[1]);
	features[1].img_idx = 1;
	images[1] = img.clone();

	cout << features[0].keypoints.size() << endl;
	cout << features[1].keypoints.size() << endl;

	finder->collectGarbage();
	full_img.release();
	img.release();

	vector<cv::detail::MatchesInfo> pairwise_matches;
	cv::Ptr<cv::detail::FeaturesMatcher> matcher;

	matcher = cv::makePtr<cv::detail::BestOf2NearestMatcher>(false, 0.3f);

	(*matcher)(features, pairwise_matches);
	matcher->collectGarbage();

	cout << pairwise_matches.size() << endl;
	cv::waitKey(0);

	vector<int> indices = cv::detail::leaveBiggestComponent(
		features, pairwise_matches, 1.0f
	);
	vector<cv::Mat> img_subset;
	vector<cv::String> img_name_subset;
	vector<cv::Size> full_img_sizes_subset;
	for (size_t i = 0; i < indices.size(); i++) {
		img_subset.push_back(images[indices[i]]);
		full_img_sizes_subset.push_back(full_img_sizes[indices[i]]);
	}
	img_name_subset.push_back(g_LImgName);
	img_name_subset.push_back(g_RImgName);

	cv::Ptr<cv::detail::Estimator> estimator;
	estimator = cv::makePtr<cv::detail::HomographyBasedEstimator>();

	if (!(*estimator)(features, pairwise_matches, cameras)) //
		return -1;

	for (size_t i = 0; i < cameras.size(); i++) {
		cv::Mat R;
		cameras[i].R.convertTo(R, CV_32F);
		cameras[i].R = R;
	}

	cv::Ptr<cv::detail::BundleAdjusterBase> adjuster;
	adjuster = cv::makePtr<cv::detail::BundleAdjusterReproj>();

	adjuster->setConfThresh(1.0f);
	cv::Mat_<uchar> refine_mask = cv::Mat::zeros(3, 3, CV_8U);
	refine_mask(0, 0) = 1;
	refine_mask(0, 1) = 1;
	refine_mask(0, 2) = 1;
	refine_mask(1, 1) = 1;
	refine_mask(1, 2) = 1;
	adjuster->setRefinementMask(refine_mask);


	for (int i = 0; i < cameras.size(); i++) {
		cout << i << " :" << endl << cameras[i].K() << endl << cameras[i].R << endl << cameras[i].t << endl << endl;
	}

	vector<double> focals;
	for (size_t i = 0; i < cameras.size(); i++) {
		focals.push_back(cameras[i].focal);
	}

	sort(focals.begin(), focals.end());
	float warped_image_scale;
	if (focals.size() % 2 == 1) {
		warped_image_scale = static_cast<float>(focals[focals.size() / 2]);
	} else {
		warped_image_scale = static_cast<float>(focals[focals.size() / 2 - 1] + focals[focals.size() / 2]) *0.5f;
	}

	for (int i = 0; i < pairwise_matches.size(); i++) {
		cout << pairwise_matches[i].H << endl;
	}

	cv::Mat rst;
	//cv::drawMatches(g_LImg, features[0].keypoints, g_RImg, features[1].keypoints, pairwise_matches[0].matches, rst);
	cv::warpPerspective(g_RImg, rst, pairwise_matches[2].H, g_LImg.size());

	vector<cv::Point2f> lp, rp;
	for (int i = 0; i < pairwise_matches[1].matches.size(); i++) {
		cv::DMatch _m = pairwise_matches[1].matches[i];
		lp.push_back(features[0].keypoints[_m.queryIdx].pt);
		rp.push_back(features[1].keypoints[_m.trainIdx].pt);
	}
	cv::Mat F = cv::findFundamentalMat(lp, rp, CV_FM_RANSAC, 0.1, 0.99);

	cv::Mat Q;
	Q.at<double>(0, 0) = 1.0;
	Q.at<double>(0, 1) = 0.0;
	Q.at<double>(0, 2) = 0.0;
	Q.at<double>(0, 3) = -cameras[0].ppx; //cx
	Q.at<double>(1, 0) = 0.0;
	Q.at<double>(1, 1) = 1.0;
	Q.at<double>(1, 2) = 0.0;
	Q.at<double>(1, 3) = -cameras[0].ppy;  //cy
	Q.at<double>(2, 0) = 0.0;
	Q.at<double>(2, 1) = 0.0;
	Q.at<double>(2, 2) = 0.0;
	Q.at<double>(2, 3) = cameras[0].focal;  //Focal
	Q.at<double>(3, 0) = 0.0;
	Q.at<double>(3, 1) = 0.0;
	Q.at<double>(3, 2) = 1.0 / 8;    //1.0/BaseLine
	Q.at<double>(3, 3) = 0.0;    //cx - cx'



	//cv::reprojectImageTo3D()

	cv::imshow("resutl01", rst);



	cv::waitKey(0);

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
		gluPerspective(45, (double)Win_width / Win_height, 0.0001, 10000);

		g_LImg.convertTo(g_LImg, CV_64FC3);
		g_RImg.convertTo(g_RImg, CV_64FC3);

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
//#include <iostream>
//#include <fstream>
//#include <string>
//#include <cmath>
//#include <ctime>
//#include <random>
//
//#include <opencv2/core/core.hpp>
//#include <opencv2/features2d/features2d.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/calib3d/calib3d.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2\xfeatures2d\nonfree.hpp>
//
//#include <opencv2/stitching/detail/matchers.hpp>
//#include <opencv2/stitching/detail/motion_estimators.hpp>
//#include <opencv2/stitching/detail/camera.hpp>
//#include <opencv2/stitching/detail/util.hpp>
//
//using namespace std;
//using namespace cv;
//using namespace cv::detail;
//using namespace cv::xfeatures2d;
//
//void readImage();
//void featureDetect(vector<vector<KeyPoint>> &keypoint, vector<Mat> &descriptors, vector< vector<DMatch> > &goodMatches);
//void cameraCalibration(vector<Mat> & images, vector<CameraParams> & cameras);
//void projectedImageSize(vector<Mat> & images, vector<float> & f, float s, int & width, int & height);
//
//vector<Mat> images;
//int imageNumber = 0;
//
//string features_type = "surf";
//string ba_cost_func = "ray";
//string ba_refine_mask = "xxxxx";
//float match_conf = 0.6f;
//float conf_thresh = 1.f;
//bool try_gpu = false;
//
//void readImage() {
//	//images.push_back(imread("cglab2.jpg"));
//	images.push_back(imread("left.jpg"));
//	images.push_back(imread("right.jpg"));
//	imageNumber = images.size();
//}
//void featureDetect(vector<vector<KeyPoint>> &keypoints, vector<Mat> &descriptors, vector< vector<DMatch> > &goodMatches) {
//	//-- Detect the keypoints using SURF Detector
//	int minHessian = 400;
//
//	Ptr< SurfFeatureDetector> detector = SurfFeatureDetector::create(minHessian);
//	detector->detect(images, keypoints);
//
//	//-- Calculate descriptors (feature vectors)
//	Ptr<SurfDescriptorExtractor> extractor = SurfDescriptorExtractor::create();
//	extractor->compute(images, keypoints, descriptors);
//
//	//-- Matching descriptor vectors using FLANN matcher
//	FlannBasedMatcher matcher;
//	vector< vector< vector<DMatch> > > matches(imageNumber - 1, vector< vector<DMatch> >(0));
//	for (int i = 0; i < imageNumber - 1; i++) {
//		//matcher.match(descriptors[i], descriptors[i + 1], matches[i]);
//		matcher.knnMatch(descriptors[i], descriptors[i + 1], matches[i], 2);
//	}
//
//	const float ratio = 0.8; // 0.8 in Lowe's paper; can be tuned
//	for (int i = 0; i < imageNumber - 1; i++) {
//		for (int j = 0; j < static_cast<int>(matches[i].size()); j++) {
//			if (matches[i][j][0].distance < ratio * matches[i][j][1].distance) {
//				goodMatches[i].push_back(matches[i][j][0]);
//			}
//		}
//	}
//
//}
//void cameraCalibration(vector<Mat> & images, vector<CameraParams> & cameras) {
//	int num_images = static_cast<int>(images.size());
//
//	Ptr<FeaturesFinder> finder;
//	if (features_type == "surf") {
//		finder = new SurfFeaturesFinder();
//	} else if (features_type == "orb") {
//		finder = new OrbFeaturesFinder();
//	} else {
//		cout << "Unknown 2D features type: '" << features_type << "'.\n";
//	}
//
//	vector<ImageFeatures> features(num_images);
//
//	for (int i = 0; i < num_images; ++i) {
//		(*finder)(images[i], features[i]);
//		features[i].img_idx = i;
//	}
//	finder->collectGarbage();
//
//	vector<MatchesInfo> pairwise_matches;
//	BestOf2NearestMatcher matcher(try_gpu, match_conf);
//	matcher(features, pairwise_matches);
//	matcher.collectGarbage();
//
//	HomographyBasedEstimator estimator;
//	estimator(features, pairwise_matches, cameras);
//
//	for (size_t i = 0; i < cameras.size(); ++i) {
//		Mat R;
//		cameras[i].R.convertTo(R, CV_32F);
//		cameras[i].R = R;
//	}
//
//	Ptr<detail::BundleAdjusterBase> adjuster;
//	if (ba_cost_func == "reproj") adjuster = new detail::BundleAdjusterReproj();
//	else if (ba_cost_func == "ray") adjuster = new detail::BundleAdjusterRay();
//	else {
//		cout << "Unknown bundle adjustment cost function: '" << ba_cost_func << "'.\n";
//	}
//	adjuster->setConfThresh(conf_thresh);
//	Mat_<uchar> refine_mask = Mat::zeros(3, 3, CV_8U);
//	if (ba_refine_mask[0] == 'x') refine_mask(0, 0) = 1;
//	if (ba_refine_mask[1] == 'x') refine_mask(0, 1) = 1;
//	if (ba_refine_mask[2] == 'x') refine_mask(0, 2) = 1;
//	if (ba_refine_mask[3] == 'x') refine_mask(1, 1) = 1;
//	if (ba_refine_mask[4] == 'x') refine_mask(1, 2) = 1;
//	adjuster->setRefinementMask(refine_mask);
//	(*adjuster)(features, pairwise_matches, cameras);
//
//	//매트릭스 출력
//	for (int i = 0; i < imageNumber; i++) {
//		cout << "cameras[" << i << "].R matrix :" << endl;
//		cout << cameras[i].t << endl;
//	}
//
//	Mat warpimge;
//
//	warpPerspective(images.at(0), warpimge, cameras[1].R, images[0].size() + images[1].size(), INTER_LINEAR + WARP_INVERSE_MAP);
//	imshow("dd", warpimge);
//}
//void projectedImageSize(vector<Mat> & images, vector<float> & f, float s, int & width, int & height) {
//	int imageNumber = static_cast<int>(images.size());
//
//	for (int i = 0; i < imageNumber; i++) {
//		if (width < ceil(images[i].cols / f[i] * s)) {
//			width = ceil(images[i].cols / f[i] * s);
//		}
//		if (height < ceil(images[i].rows / f[i] * s)) {
//			height = ceil(images[i].rows / f[i] * s);
//		}
//	}
//}
//
//int main() {
//
//	//read Images
//	readImage();
//
//	//feature detection
//	vector< vector<KeyPoint> > keypoints;
//	vector<Mat> descriptors(imageNumber);
//	vector< vector<DMatch> > goodMatches(imageNumber - 1, vector<DMatch>(0));
//	featureDetect(keypoints, descriptors, goodMatches);
//
//	//Camera calibration (calculate focal length)
//	vector<CameraParams> cameras(imageNumber);
//	cameraCalibration(images, cameras);
//
//	//Set the median focal length as the scale of the warped image
//	vector<float> f(imageNumber);
//	for (int i = 0; i < imageNumber; i++) {
//		f[i] = cameras[i].focal;
//	}
//	sort(f.begin(), f.end());
//	float warped_image_scale;
//	if (f.size() % 2 == 1) {
//		warped_image_scale = static_cast<float>(f[f.size() / 2]);
//	} else {
//		warped_image_scale = static_cast<float>(f[f.size() / 2 - 1] + f[f.size() / 2]) * 0.5;
//	}
//
//
//	//Project to cylindrical or spherical surface
//	int ind = 0; // 0 -> cylindrical; 1 -> spherical
//	int projWidth = 0, projHeight = 0;
//	vector<Mat> projImages(imageNumber);
//
//	projectedImageSize(images, f, warped_image_scale, projWidth, projHeight); // all the images have the same sizes
//
//	for (int i = 0; i < imageNumber; i++) {
//		projImages[i] = Mat::zeros(projHeight, projWidth, CV_8U);
//		//imshow(to_string(i), projImages[i]);
//		//imwrite(to_string(i)+".jpg", projImages[i]);
//	}
//
//	waitKey(0);
//	return 0;
//}
