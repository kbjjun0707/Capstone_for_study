#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\ximgproc\disparity_filter.hpp>
#include <opencvLibs.h>

void main() {
	double baseLine = 16;
	double covergence	=    0.00285;
	double FX			= 164.947925023;
	double FY			= 164.947925023;
	double CX			=  250;
	double CY			=  200;
	double K1			=    0.0;
	double K2			=    0.0;
	double P1			=    0.0;
	double P2			=    0.0;
	
	cv::Matx33d K = cv::Matx33d(
		FX,  0, CX, 
		 0, FY, CY, 
		 0,  0,  1);
	cv::Matx41d distCoeffs = cv::Matx41d(K1, K2, P1, P2);
	cv::Matx44d Q = cv::Matx44d(
		1.0,			0.0,				0.0,					-CX,
		0.0,			1.0,				0.0,					-CY,
		0.0,			0.0,				0.0,					 FX,
		0.0,			0.0,	-1.0 / baseLine,		(CX - CX) / baseLine
	);
	
	// param
	int sgbmWinSize = 5;
	int numberOfDisparities = 16 * 1;
	int cn = 3;

	//// SGBM
	cv::Ptr<cv::StereoSGBM> sgbm = cv::StereoSGBM::create(0, numberOfDisparities, sgbmWinSize);

	// filter
	cv::Ptr<cv::ximgproc::DisparityWLSFilter> wls_filter;
	wls_filter = cv::ximgproc::createDisparityWLSFilter(sgbm);
	cv::Ptr<cv::StereoMatcher> sm = cv::ximgproc::createRightMatcher(sgbm);

	// param
	double lambda = 7000.0;
	double sigma = 1.25;
	double vis_multi = 1.0;

	// init
	sgbm->setPreFilterCap(63);
	sgbm->setBlockSize(sgbmWinSize);
	sgbm->setNumDisparities(numberOfDisparities);
	sgbm->setP1(8 * cn*sgbmWinSize*sgbmWinSize);
	sgbm->setP2(32 * cn*sgbmWinSize*sgbmWinSize);
	sgbm->setMode(cv::StereoSGBM::MODE_SGBM_3WAY);
	
	// viz
	//cv::viz::Viz3d window("Coordinate Frame");
	//window.showWidget("Coordinate Widget", cv::viz::WCoordinateSystem());
	//main loop
	//while (!window.wasStopped()) {
	//	cv::Mat tmpImg;
	//	cap.read(tmpImg);
	//	leftImg = tmpImg(cv::Rect(0, 0, tmpImg.cols / 2, tmpImg.rows));
	//	rightImg = tmpImg(cv::Rect(tmpImg.cols / 2, 0, tmpImg.cols / 2, tmpImg.rows));
	//	cv::Mat tmp1 = leftImg.clone();
	//	cv::Mat tmp2 = rightImg.clone();
	//	sgbm->compute(tmp1, tmp2, disparity16S);
	//	sm->compute(tmp2, tmp1, img16Sr);
	//	cv::Mat showDisparity;
	//	disparity16S.convertTo(showDisparity, CV_8UC1, 255 / (numberOfDisparities*16.));
	//	printf("disparity16S: %s %d x %d\n", type2str(disparity16S.type()).c_str(), disparity16S.rows, disparity16S.cols);
	//	cv::imshow("disparity", showDisparity);
	//	wls_filter->setLambda(lambda);
	//	wls_filter->setSigmaColor(sigma);
	//	wls_filter->filter(disparity16S, tmp1, filteredDisparity, img16Sr);
	//	cv::Mat showFilteredDisparity;
	//	filteredDisparity.convertTo(showFilteredDisparity, CV_8U, 255 / (numberOfDisparities*16.));
	//	printf("filteredDisparity: %s %d x %d\n", type2str(filteredDisparity.type()).c_str(), filteredDisparity.rows, filteredDisparity.cols);
	//	cv::imshow("Filtered Disparity", showFilteredDisparity);
	//	cv::Mat xyz, xyzt;
	//	// output : 3-channel floating-point image of the same size as disparity
	//	cv::reprojectImageTo3D(filteredDisparity, xyz, Q, true);
	//	printf("xyz: %s %d x %d\n", type2str(xyz.type()).c_str(), xyz.rows, xyz.cols);
	//	cv::Mat showXYZ;
	//	xyz.convertTo(showXYZ, CV_8UC3, 255 / (numberOfDisparities*8.));
	//	cv::imshow("XYZ", showXYZ);
	//	viz::WCloud cw(xyz, viz::Color::white());
	//	cw.setRenderingProperty(cv::viz::POINT_SIZE, 2);
	//	window.showWidget("Cloud Widget", cw);
	//	window.spinOnce(30, true);
	//}

	// load IMG
	cv::Mat img_1 = cv::imread("D:/Workspace/stereo001L.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat img_2 = cv::imread("D:/Workspace/stereo001R.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat tmp1 = img_1.clone();
	cv::Mat tmp2 = img_2.clone();

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
	
	//-- Check its extreme values
	double minVal; double maxVal;

	minMaxLoc(imgDisparity16S, &minVal, &maxVal);

	imgDisparity16S.convertTo(imgDisparity8U, CV_8UC1, 255 / (numberOfDisparities*16.));
	//imgDisparity16S.convertTo(imgDisparity8U, CV_8UC1, 255 / (maxVal - minVal));

	wls_filter->setLambda(lambda);
	wls_filter->setSigmaColor(sigma);
	wls_filter->filter(imgDisparity16S, tmp1, imgFilteredDisparity16S, imgDisparity16Sr);

	imgFilteredDisparity16S.convertTo(imgFilteredDisparity8U, CV_8U, 255 / (numberOfDisparities*16.));
	//imgFilteredDisparity16S.convertTo(imgFilteredDisparity8U, CV_8UC1, 255 / (maxVal - minVal));

	
	cv::Mat xyz;
	// output : 3-channel floating-point image of the same size as disparity
	cv::reprojectImageTo3D(imgFilteredDisparity16S, xyz, Q);
	
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
	cv::moveWindow("XYZ", 10 , 10 + img_1.rows + imgFilteredDisparity8U.rows);
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