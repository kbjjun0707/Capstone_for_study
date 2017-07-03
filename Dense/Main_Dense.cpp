#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ximgproc/disparity_filter.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/stereo/stereo.hpp>
#include <vector>
#include <opencvInit.h>

void main() {
	double baseLine = 300.0;
	double covergence = 0.00285;
	double FX = 3272.6083;
	double FY = 3272.6083;
	double CX =  250;
	double CY =  200;
	double K1 =   -0.15;
	double K2 =    0.0;
	double P1 =    0.0;
	double P2 =    0.0;

	// load IMG
	//cv::Mat img_1 = cv::imread("E:/Workspace/stereo001L.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	//cv::Mat img_2 = cv::imread("E:/Workspace/stereo001R.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat img_1 = cv::imread("test01.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat img_2 = cv::imread("test02.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat tmp1 = img_1.clone();
	cv::Mat tmp2 = img_2.clone();
	
	// Detect the keypoints using SURF Detector
	int minHessian = 1000;
	cv::Ptr<cv::xfeatures2d::SurfFeatureDetector> detector = cv::xfeatures2d::SurfFeatureDetector::create(minHessian);
	std::vector<cv::KeyPoint> keypoints_1, keypoints_2;
	detector->detect(img_1, keypoints_1);
	detector->detect(img_2, keypoints_2);
	
	// Calculate descriptors (feature vectors)
	cv::Ptr<cv::xfeatures2d::SurfDescriptorExtractor> extractor = cv::xfeatures2d::SurfDescriptorExtractor::create();
	cv::Mat descriptors_1, descriptors_2;
	extractor->compute(img_1, keypoints_1, descriptors_1);
	extractor->compute(img_2, keypoints_2, descriptors_2);

	// Matching descriptor vectors with a brute force matcher
	cv::BFMatcher matcher(cv::NORM_L1, true);
	std::vector< cv::DMatch > matches;
	matcher.match(descriptors_1, descriptors_2, matches);

	// calculate Fundamental Matrix
	std::vector<cv::Point2f> imgpts1, imgpts2;
	for (unsigned int i = 0; i<matches.size(); i++) {
		// queryIdx is the "left" image
		imgpts1.push_back(keypoints_1[matches[i].queryIdx].pt);
		// trainIdx is the "right" image
		imgpts2.push_back(keypoints_2[matches[i].trainIdx].pt);
	}
	cv::Mat F = findFundamentalMat(imgpts1, imgpts2, CV_FM_RANSAC, 0.1, 0.99);
	
	double datas[] = {	FX,  0,  CX,
						 0, FY,  CY,
						 0,  0, 1.0 };

	cv::Mat K = cv::Mat(3, 3, CV_64F, datas);
	cv::Matx41d distCoeffs = cv::Matx41d(K1, K2, P1, P2);
	cv::Matx44d Q;
	//= cv::Matx44d(
	//	1.0, 0.0, 0.0, -CX,
	//	0.0, 1.0, 0.0, -CY,
	//	0.0, 0.0, 0.0, FX,
	//	0.0, 0.0, -1.0 / baseLine, (CX - CX) / baseLine
	//);

	cv::Mat_<double> E = K.t() * F * K;
	//cv::Mat E = cv::findEssentialMat(imgpts1, imgpts2);

	// calculate Rotation Matrix and Translation Vector
	cv::Matx34d tP;
	cv::Matx34d tP1;
	//decompose E to P' , HZ (9.19)
	cv::SVD svd(E);
	cv::Mat svd_u = svd.u;
	cv::Mat svd_vt = svd.vt;
	cv::Mat svd_w = svd.w;
	cv::Matx33d W(	0, -1, 0,
					1,  0, 0, 
					0,  0, 1);			//HZ 9.13
	cv::Mat_<double> R = svd_u * cv::Mat(W) * svd_vt;	//HZ 9.19
	cv::Mat_<double> t = svd_u.col(2); //u3

	tP1 = cv::Matx34d(	R(0, 0), R(0, 1), R(0, 2), t(0),
						R(1, 0), R(1, 1), R(1, 2), t(1),
						R(2, 0), R(2, 1), R(2, 2), t(2));


	//cv::Mat R, t;
	//cv::recoverPose(E, imgpts1, imgpts2, K, R, t);



	// Reprojection Matrix and rectification data
	cv::Mat R1, R2, P1_, P2_, H_1, H_2;
	cv::Rect validRoi[2];
	double dist[] = { -0.03432, 0.05332, -0.00347, 0.00106, 0.00000 };
	cv::Mat D(1, 5, CV_64F, dist);

	// Point 1 *************************************************************************************************************
	stereoRectify(K, distCoeffs, K, distCoeffs,
		img_1.size(), R, t, 
		R1, R2, P1_, P2_, Q, CV_CALIB_ZERO_DISPARITY, -1.0, 
		img_1.size(), &validRoi[0], &validRoi[1]);

	// param
	int sgbmWinSize = 5;
	int numberOfDisparities = 16 * 4;
	int cn = 3;

	// SGBM
	cv::Ptr<cv::StereoSGBM> sgbm = cv::StereoSGBM::create(-32, numberOfDisparities, sgbmWinSize);

	// filter
	cv::Ptr<cv::ximgproc::DisparityWLSFilter> wls_filter;
	wls_filter = cv::ximgproc::createDisparityWLSFilter(sgbm);
	cv::Ptr<cv::StereoMatcher> sm = cv::ximgproc::createRightMatcher(sgbm);

	// param
	double lambda = 8000.0;
	double sigma = 1.5;
	double vis_multi = 1.0;

	// init
	sgbm->setPreFilterCap(63);
	sgbm->setBlockSize(sgbmWinSize);
	sgbm->setNumDisparities(numberOfDisparities);
	sgbm->setP1(8 * cn*sgbmWinSize*sgbmWinSize);
	sgbm->setP2(32 * cn*sgbmWinSize*sgbmWinSize);
	sgbm->setMode(cv::StereoSGBM::MODE_SGBM_3WAY);

	cv::Mat imgDisparity16S = cv::Mat(img_1.rows, img_1.cols, CV_16S);
	cv::Mat imgDisparity16Sr = cv::Mat(img_1.rows, img_1.cols, CV_16S);
	cv::Mat imgDisparity8U = cv::Mat(img_1.rows, img_1.cols, CV_8UC1);

	cv::Mat imgFilteredDisparity16S = cv::Mat(img_1.rows, img_1.cols, CV_16S);
	cv::Mat imgFilteredDisparity8U = cv::Mat(img_1.rows, img_1.cols, CV_8UC1);

	sgbm->compute(img_1, img_2, imgDisparity16S);
	sm->compute(tmp2, tmp1, imgDisparity16Sr);

	//CvStereoBMState * state = cvCreateStereoBMState();
	//CvMat* imgL = cvCreateMat()
	//cvFindStereoCorrespondenceBM(img_1, img_2, imgDisparity16S, state);

	// Check its extreme values
	double minVal; double maxVal;

	minMaxLoc(imgDisparity16S, &minVal, &maxVal);

	//imgDisparity16S.convertTo(imgDisparity8U, CV_8UC1, 255 / (numberOfDisparities*16.));
	imgDisparity16S.convertTo(imgDisparity8U, CV_8UC1, 255 / (maxVal - minVal));

	wls_filter->setLambda(lambda);
	wls_filter->setSigmaColor(sigma);
	wls_filter->filter(imgDisparity16S, tmp1, imgFilteredDisparity16S, imgDisparity16Sr);

	//imgFilteredDisparity16S.convertTo(imgFilteredDisparity8U, CV_8U, 255 / (numberOfDisparities*16.));
	imgFilteredDisparity16S.convertTo(imgFilteredDisparity8U, CV_8UC1, 255 / (maxVal - minVal));

	// Point 2 *************************************************************************************************************
	cv::Mat xyz;
	// output : 3-channel floating-point image of the same size as disparity
	cv::reprojectImageTo3D(imgFilteredDisparity8U, xyz, Q);

	cv::Mat showXYZ;
	//xyz.convertTo(showXYZ, CV_8UC3, 255 / (numberOfDisparities*16.));
	xyz.convertTo(showXYZ, CV_8UC3, 255 / (maxVal - minVal));

	//cv::Mat grayXYZ;
	//cvtColor(showXYZ, grayXYZ, CV_BGR2GRAY);


	//// test
	//cv::Ptr<cv::Mat> testdata = new cv::Mat[4];
	//cv::split(showXYZ, testdata);
	//cv::Mat tmpR[] = { cv::Mat::zeros(testdata[2].size(), testdata[2].type()), cv::Mat::zeros(testdata[2].size(), testdata[2].type()), testdata[2] };
	//cv::Mat tmpG[] = { cv::Mat::zeros(testdata[2].size(), testdata[2].type()), testdata[1], cv::Mat::zeros(testdata[2].size(), testdata[2].type()) };
	//cv::Mat tmpB[] = { testdata[0], cv::Mat::zeros(testdata[2].size(), testdata[2].type()), cv::Mat::zeros(testdata[2].size(), testdata[2].type()) };
	//cv::Mat testR;
	//cv::merge(tmpR, 3, testR);
	//cv::Mat testG;
	//cv::merge(tmpG, 3, testG);
	//cv::Mat testB;
	//cv::merge(tmpB, 3, testB);
	////test


	cv::namedWindow("Limg");
	cv::moveWindow("Limg", 10, 10);
	cv::namedWindow("Rimg");
	cv::moveWindow("Rimg", 10 + img_1.cols, 10);
	cv::namedWindow("Disparity");
	cv::moveWindow("Disparity", 10, 10 + img_1.rows);
	cv::namedWindow("Filtered");
	cv::moveWindow("Filtered", 10 + img_1.cols, 10 + img_1.rows);
	cv::namedWindow("XYZ");
	cv::moveWindow("XYZ", 10, 10 + img_1.rows + imgFilteredDisparity8U.rows);
	cv::namedWindow("showXYZ");
	cv::moveWindow("showXYZ", 10 + img_1.cols, 10 + img_1.rows + imgFilteredDisparity8U.rows);

	cv::imshow("Limg", img_1);
	cv::imshow("Rimg", img_2);
	cv::imshow("Disparity", imgDisparity8U);
	cv::imshow("Filtered", imgFilteredDisparity8U);
	cv::imwrite("testDisparity.jpg", imgFilteredDisparity8U);
	cv::imshow("XYZ", xyz);
	cv::imshow("showXYZ", showXYZ);
	cv::imwrite("testDense.jpg", showXYZ);
	
	//cv::imshow("grayXYZ", grayXYZ);

	//cv::imshow("tX", testdata[0]);
	//cv::imshow("tY", testdata[1]);
	//cv::imshow("tZ", testdata[2]);

	//cv::imshow("tcX", testB);
	//cv::imshow("tcY", testG);
	//cv::imshow("tcZ", testR);
	cv::waitKey(0);

}