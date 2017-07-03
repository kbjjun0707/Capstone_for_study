#include <opencv2\opencv.hpp>
#include <opencv2\xfeatures2d.hpp>
#include <opencvLibs.h>

#include <time.h>

#include <iostream>
using namespace std;

int main() {
	cv::Mat Img1 = cv::imread("E:/Workspace/stereo001L.jpg");
	cv::Mat Img2 = cv::imread("E:/Workspace/stereo001R.jpg");
	cv::Mat Gray1, Gray2;

	cv::cvtColor(Img1, Gray1, CV_BGR2GRAY);
	cv::cvtColor(Img2, Gray2, CV_BGR2GRAY);

	cv::Ptr<cv::xfeatures2d::SURF> surf = cv::xfeatures2d::SURF::create();

	std::vector<cv::Point2f>	corners1, corners2;
	std::vector<cv::KeyPoint>	keypoints1, keypoints2;
	cv::Mat descriptor1, descriptor2;

	clock_t start = clock();
	cv::goodFeaturesToTrack(Gray1, corners1, 1000, 0.1, 5);
	cv::goodFeaturesToTrack(Gray2, corners2, 1000, 0.1, 5);
	surf->detectAndCompute(Gray1, cv::Mat(), keypoints1, descriptor1);
	surf->detectAndCompute(Gray2, cv::Mat(), keypoints2, descriptor2);
	clock_t end = clock();


	cout << "time : " << (double)((end - start) / 1000.0) << endl;
	cout << "found " << corners1.size() << " corners." << endl;
	cout << "found " << keypoints1.size() << " features." << endl;

	cout << "found " << corners2.size() << " corners." << endl;
	cout << "found " << keypoints2.size() << " features." << endl;
	

	for (cv::Point2f p : corners1) {
		cv::circle(Img1, p, 3, cv::Scalar(0, 200, 0));
	}
	cv::drawKeypoints(Img1, keypoints1, Img1, cv::Scalar(200, 0, 0));

	
	cv::imshow("", Img1);





	cv::waitKey(0);

	return 0;
}