#include "mainheader.h"


int main() {
	iMiner::Tracker test(0.25);
	iMiner::DisparityMaker DM(0.25);

	////cv::Mat img1 = cv::imread("E:/Workspace/stereo002L.jpg");
	//cv::Mat img1 = cv::imread("E:/Workspace/sample_box04.jpg");
	//cv::resize(img1, img1, cv::Size(), 0.4, 0.4);
	//test.setStdImg(img1);
	////cv::Mat img2 = cv::imread("E:/Workspace/stereo002R.jpg");
	//cv::Mat img2 = cv::imread("E:/Workspace/sample_box03.jpg");
	//cv::resize(img2, img2, cv::Size(), 0.4, 0.4);
	//test.processImage(img2);
	////test.drawPoints(img2, img2);
	//test.warpImage(img2, img2);
	//cv::imshow("1", img1);
	//cv::imshow("2", img2);

	//DM.setting(63, 3, (int)test.getDisparity(), 0, 3, 8000.0, 1.0, cv::StereoSGBM::MODE_SGBM_3WAY);
	//DM.compute(img1, img2);

	//cv::Mat res1 = DM.getVisDisparity();
	//cv::Mat res2 = DM.getVisFiltedDisparity();

	//cv::imshow("3", res1);
	//cv::imshow("4", res2);

	//cv::imwrite("test01.jpg", img1);
	//cv::imwrite("test02.jpg", img2);
	//
	//std::cout << test.m_Homography << endl;

	
	cv::VideoCapture Cam(0);
	if (!Cam.isOpened()) {
		cout << "Cam open fail" << endl;
		return 0;
	}
	cv::Mat Frame, tMat, stdMat;
	Cam >> Frame;

	cv::namedWindow("Cam");
	cv::moveWindow("Cam", 10, 10);

	int Key = 0;
	bool Capture = true;
	while (Capture) {
		cv::imshow("Cam", Frame);

		switch (Key = cv::waitKey(40)) {
		case 13:		// enter key
			stdMat = Frame.clone();
			test.setStdImg(Frame);
			break;
		case 27:
			Capture = false;
			break;
		default:
			//cout << Key << endl;
			break;
		}

		Cam >> Frame;
		test.processImage(Frame);
		tMat = Frame;
		test.drawPoints(Frame, tMat);
		test.warpImage(tMat, tMat);
		cv::imshow("warp", tMat);
		if (!stdMat.empty()) {
			cv::imshow("std", stdMat);
			DM.compute(stdMat, tMat);
			tMat = DM.getVisDisparity();
			cv::imshow("dis", tMat);
			tMat = DM.getVisFiltedDisparity();
			cv::imshow("fdis", tMat);
		}
	}

	cv::waitKey(0);
	

	return 0;
}