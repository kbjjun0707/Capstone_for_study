#pragma warning(disable:4996)

#include <opencv2\opencv.hpp>
#include <opencv2\stereo\stereo.hpp>
#include <opencv2\features2d\features2d.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\ximgproc.hpp>
#include <opencv2\xfeatures2d.hpp>
#include <opencv\cvaux.h>
#include <opencvInit.h>

#include <iostream>

using namespace std;
using namespace cv::xfeatures2d;

char *g_Kparams = "E:/Workspace/CameraParams.txt";
char *g_LeftImg = "E:/Workspace/stereo001L.jpg";
char *g_RightImg = "E:/Workspace/stereo001R.jpg";

double g_fx;
double g_fy;
double g_cx;
double g_cy;
double g_distCoeffs[5];

cv::Size g_ReSize(660, 480);

int main() {
	// input CameraMatrix from file
	ifstream input(g_Kparams);
	if (!input.is_open()) {
		cout << "ERROR : No Camera Params" << endl;
		return 0;
	}
	char strbuf[50];
	float floatbuf;
	input.getline(strbuf, 49);
	input >> strbuf[0] >> g_fx >> strbuf[0] >> floatbuf >> strbuf[0] >> g_cx >> strbuf[0]
		>> floatbuf >> strbuf[0] >> g_fy >> strbuf[0] >> g_cy >> strbuf[0]
		>> floatbuf >> strbuf[0] >> floatbuf >> strbuf[0] >> floatbuf >> strbuf[0];

	input >> strbuf[0] >> g_distCoeffs[0] >> strbuf[0] >> g_distCoeffs[1] >> strbuf[0] >> g_distCoeffs[2] >> strbuf[0]
		>> g_distCoeffs[3] >> strbuf[0] >> g_distCoeffs[4] >> strbuf[0];

	cout << endl << g_fx << " " << g_fy << " " << g_cx << " " << g_cy << endl;
	cout << endl << g_distCoeffs[0] << " " << g_distCoeffs[1] << " " << g_distCoeffs[2] << " " << g_distCoeffs[3] << " " << g_distCoeffs[4] << endl;
	
	input.close();
	fflush(stdin);
	
	cv::Mat img_1 = cv::imread(g_LeftImg, CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat img_2 = cv::imread(g_RightImg, CV_LOAD_IMAGE_GRAYSCALE);
	cv::resize(img_1, img_1, g_ReSize);
	cv::resize(img_2, img_2, g_ReSize);

	if (!img_1.data || !img_2.data)
	{
		return -1;
	}
	
	//-- Step 1: Detect the keypoints using SURF Detector
	int minHessian = 700;
	cv::Ptr<SiftFeatureDetector> detector = SiftFeatureDetector::create(minHessian);
	std::vector<cv::KeyPoint> keypoints_1, keypoints_2;
	detector->detect(img_1, keypoints_1);
	detector->detect(img_2, keypoints_2);

	//-- Step 2: Calculate descriptors (feature vectors)
	cv::Ptr<SiftDescriptorExtractor> extractor = SiftDescriptorExtractor::create();
	cv::Mat descriptors_1, descriptors_2;
	extractor->compute(img_1, keypoints_1, descriptors_1);
	extractor->compute(img_2, keypoints_2, descriptors_2);

	//-- Step 3: Matching descriptor vectors with a brute force matcher
	cv::BFMatcher matcher(cv::NORM_L2, true);
	std::vector< cv::DMatch > matches;
	matcher.match(descriptors_1, descriptors_2, matches);

	//-- Draw matches
	cv::Mat img_matches;
	drawMatches(img_1, keypoints_1, img_2, keypoints_2, matches, img_matches);
	//-- Show detected matches
	cv::namedWindow("Matches", CV_WINDOW_AUTOSIZE);
	cv::moveWindow("Matches", 10, 10);
	imshow("Matches", img_matches);
	cv::waitKey(0);

	//-- Step : stereoBM
	int ndisparities = 16 * 1;
	int SADWindowSize = 9;

	cv::Ptr<cv::StereoSGBM> sgbm = cv::StereoSGBM::create(0, ndisparities, SADWindowSize);

	// init
	sgbm->setPreFilterCap(63);
	sgbm->setBlockSize(SADWindowSize);
	sgbm->setNumDisparities(ndisparities);
	sgbm->setP1(8 * 3* SADWindowSize*SADWindowSize);
	sgbm->setP2(32 * 3*SADWindowSize*SADWindowSize);
	sgbm->setMode(cv::StereoSGBM::MODE_SGBM_3WAY);

	// filter
	cv::Ptr<cv::ximgproc::DisparityWLSFilter> wls_filter;
	wls_filter = cv::ximgproc::createDisparityWLSFilter(sgbm);
	cv::Ptr<cv::StereoMatcher> sm = cv::ximgproc::createRightMatcher(sgbm);

	// create the image in which we will save our disparities
	cv::Mat imgDisparity16S = cv::Mat(img_1.rows, img_1.cols, CV_16S);
	cv::Mat imgDisparity16Sr = cv::Mat(img_1.rows, img_1.cols, CV_16S);
	cv::Mat imgDisparity8U =  cv::Mat(img_1.rows, img_1.cols, CV_8UC1);
	cv::Mat imgFilteredDisparity16S = cv::Mat(img_1.rows, img_1.cols, CV_16S);
	cv::Mat imgFilteredDisparity8U = cv::Mat(img_1.rows, img_1.cols, CV_8UC1);

	sgbm->compute(img_1, img_2, imgDisparity16S);
	sm->compute(img_2, img_1, imgDisparity16Sr);

	wls_filter->setLambda(7000.0);
	wls_filter->setSigmaColor(1.25);
	wls_filter->filter(imgDisparity16S, img_1, imgFilteredDisparity16S, imgDisparity16Sr);

	//-- Check its extreme values
	double minVal; double maxVal;

	minMaxLoc(imgDisparity16S, &minVal, &maxVal);

	//-- Display it as a CV_8UC1 image
	imgDisparity16S.convertTo(imgDisparity8U, CV_8UC1, 255 / (maxVal - minVal));
	imgFilteredDisparity16S.convertTo(imgFilteredDisparity8U, CV_8UC1, 255 / (maxVal - minVal));

	cv::namedWindow("Disparity", CV_WINDOW_AUTOSIZE);
	cv::moveWindow("Disparity", 10, img_matches.rows + 15);
	cv::imshow("Disparity", imgFilteredDisparity8U);
	cv::waitKey(0);

	//-- Step 4: calculate Fundamental Matrix
	vector<cv::Point2f>imgpts1, imgpts2;
	for (unsigned int i = 0; i<matches.size(); i++) {
		// queryIdx is the "left" image
		imgpts1.push_back(keypoints_1[matches[i].queryIdx].pt);
		// trainIdx is the "right" image
		imgpts2.push_back(keypoints_2[matches[i].trainIdx].pt);
	}
	cv::Mat F = findFundamentalMat(imgpts1, imgpts2, CV_FM_8POINT);

	cout << "Fundamental : " << endl << F << endl << endl;

	//-- Step 5: calculate Essential Matrix
	double data[] = {	g_fx,	0.0,	g_cx,
						0.0,	g_fy,	g_cy,
						0.0,	0.0,	1.0 };//Camera Matrix
	cv::Mat K(3, 3, CV_64F, data);
	cv::Mat_<double> E = //K.t() * F * K; //according to HZ (9.12)
	cv::findEssentialMat(imgpts1, imgpts2, K);

	//-- Step 6: calculate Rotation Matrix and Translation Vector
	cv::Matx34d P;
	cv::Matx34d P1;
	//decompose E to P' , HZ (9.19)
	cv::SVD svd(E, cv::SVD::MODIFY_A);
	cv::Mat svd_u = svd.u;
	cv::Mat svd_vt = svd.vt;
	cv::Mat svd_w = svd.w;
	cv::Matx33d W(0, -1, 0, 1, 0, 0, 0, 0, 1);//HZ 9.13
	cv::Mat_<double> R = svd_u * cv::Mat(W) * svd_vt; //HZ 9.19
	cv::Mat_<double> t = svd_u.col(2); //u3

	//if (!CheckCoherentRotation(R)) {
	//	std::cout << "resulting rotation is not coherent\n";
	//	P1 = 0;
	//	return 0;
	//}

	P1 = cv::Matx34d(
		R(0, 0), R(0, 1), R(0, 2), t(0),
		R(1, 0), R(1, 1), R(1, 2), t(1),
		R(2, 0), R(2, 1), R(2, 2), t(2));

	//-- Step 7: Reprojection Matrix and rectification data
	cv::Mat R1, R2, P1_, P2_, Q;
	cv::Rect validRoi[2];
	double *dist = g_distCoeffs;
	cv::Mat D(1, 5, CV_64F, dist);

	stereoRectify(K, D, K, D, img_1.size(), R, t, R1, R2, P1_, P2_, Q, CV_CALIB_ZERO_DISPARITY);

	cv::Mat xyz;
	reprojectImageTo3D(imgDisparity16S, xyz, Q, true);
	
	cv::Mat showXYZ;
	//xyz.convertTo(showXYZ, CV_8UC3, 255 / (numberOfDisparities*8.));
	xyz.convertTo(showXYZ, CV_8UC3, 255 / (maxVal - minVal));


	cv::namedWindow("To3D", CV_WINDOW_AUTOSIZE);
	cv::moveWindow("To3D", imgDisparity8U.cols + 10, img_matches.rows + 15);
	cv::imshow("To3D", showXYZ);
	
	cv::waitKey(0);

	return 0;
}


//#include <opencv2\opencv.hpp>
//#include <opencvInit.h>
//
//#include <iostream>
//
//using namespace std;
//
//
//int main() {
//
//
//
//	return 0;
//}