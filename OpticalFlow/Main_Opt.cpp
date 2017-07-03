// opencv
#include <opencv2\opencv.hpp>
#include <opencv2\optflow.hpp>
#include <opencvLibs.h>

// std
#include <iostream>

using namespace std;
using namespace cv;

char *g_workspace = "E:/Workspace";
char *g_VideoWin = "Video";
char *g_OptWin = "Optical";
std::vector<cv::Point2f> g_PrePts, g_PosPts;
Mat g_K;

void drawOptFlowMap(const cv::Mat& flow, cv::Mat& cflowmap, int step, const cv::Scalar& color) {
	g_PrePts.clear(); g_PosPts.clear();
	for (int y = 0; y < cflowmap.rows; y += step)
		for (int x = 0; x < cflowmap.cols; x += step) {
			const cv::Point2f& fxy = flow.at< cv::Point2f>(y, x);
			line(cflowmap, cv::Point(x, y), cv::Point(cvRound(x + fxy.x), cvRound(y + fxy.y)),
				color);
			circle(cflowmap, cv::Point(cvRound(x + fxy.x), cvRound(y + fxy.y)), 1, color, 1);
			g_PrePts.push_back(cv::Point(x, y));
			g_PosPts.push_back(cv::Point(x + fxy.x, y + fxy.y));
		}
}

int main() {
	double K_Data[] = {
		761.788,        0, 258.556,
		0,        760.536, 196.138,
		0,              0,       1
	};
	g_K = cv::Mat(3, 3, CV_64FC1, K_Data);
	cv::VideoCapture m_Cam(0);
	cv::Mat m_PreImg, m_PreImgGray;
	cv::Mat m_NxtImg, m_NxtImgGray;

	m_Cam >> m_PreImg;
	cv::Size m_ImgSize = cv::Size(m_PreImg.cols * 2 / 3, m_PreImg.rows * 2 / 3);
	cv::resize(m_PreImg, m_PreImg, m_ImgSize);
	cv::cvtColor(m_PreImg, m_PreImgGray, CV_BGR2GRAY);

	cv::namedWindow(g_VideoWin);
	cv::namedWindow(g_OptWin);
	cv::moveWindow(g_VideoWin, 0, 0);
	cv::moveWindow(g_OptWin, 0 + m_ImgSize.width, 0);

	cv::Mat flow, cflow;

	bool m_IsCap = true;
	while (m_IsCap) {
		m_Cam >> m_NxtImg;
		cv::resize(m_NxtImg, m_NxtImg, m_ImgSize);
		cv::cvtColor(m_NxtImg, m_NxtImgGray, CV_BGR2GRAY);

		cv::calcOpticalFlowFarneback(m_PreImgGray, m_NxtImgGray, flow, 0.5, 2, 15, 3, 5, 1.2, 0);

		drawOptFlowMap(flow, m_PreImg, 50, CV_RGB(0, 255, 0));
		//Mat R, t;
		//Mat E = findEssentialMat(g_PrePts, g_PosPts, g_K);
		//recoverPose(E, g_PrePts, g_PosPts, g_K, R, t);
		//cout << R << endl
		//	<< -R.inv()*t << endl << endl;

		cv::imshow(g_VideoWin, m_NxtImg);
		cv::imshow(g_OptWin, m_PreImg);
		m_PreImg = m_NxtImg.clone();
		m_PreImgGray = m_NxtImgGray.clone();

		if (cv::waitKey(25) == 27)
			m_IsCap = false;
	}

	cv::Mat m_Img1, m_Img2;
	cv::Mat m_Gray1, m_Gray2;

	m_Img1 = cv::imread("d:/workspace/stereo001L.jpg");
	m_Img2 = cv::imread("d:/workspace/stereo001R.jpg");

	cv::cvtColor(m_Img1, m_Gray1, CV_BGR2GRAY);
	cv::cvtColor(m_Img2, m_Gray2, CV_BGR2GRAY);


	cv::calcOpticalFlowFarneback(m_Gray1, m_Gray2, flow, 0.5, 2, 15, 3, 5, 1.2, 0);

	drawOptFlowMap(flow, m_Img2, 20, CV_RGB(0, 255, 0));

	cv::imshow("", m_Img2);
	cv::waitKey();

	

	return 0;
}