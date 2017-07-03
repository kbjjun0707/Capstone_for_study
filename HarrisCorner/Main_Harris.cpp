#include <opencv2\opencv.hpp>
#include <opencvLibs.h>

#include <iostream>

using namespace std;

namespace iMiner {
	class HarrisDetector {
	private:
		cv::Mat cornerStrength;
		cv::Mat cornerTh;		// 경계화된 코너
		cv::Mat localMax;
		int neighbourhood;
		int aperture;
		double k;
		double maxStrength;
		double threshold;
		int nonMaxSize;
		cv::Mat kernel;
		
		cv::Mat getCornerMap(double qualityLevel) {
			cv::Mat cornerMap;
			threshold = qualityLevel*maxStrength;
			cv::threshold(cornerStrength, cornerTh, threshold, 255, cv::THRESH_BINARY);
			cornerTh.convertTo(cornerMap, CV_8U);
			cv::bitwise_and(cornerMap, localMax, cornerMap);
			return cornerMap;
		}

		void getCorners(std::vector<cv::Point> &points, cv::Mat &cornerMap) {
			for (int y = 0; y < cornerMap.rows; y++) {
				const uchar *rowPtr = cornerMap.ptr<uchar>(y);
				for (int x = 0; x < cornerMap.cols; x++) {
					if (rowPtr[x]) {
						points.push_back(cv::Point(x, y));
					}
				}
			}
		}
	public:
		HarrisDetector() : neighbourhood(3), aperture(3), k(0.01), maxStrength(0.0), threshold(0.01), nonMaxSize(3) {
			//setLocalMaxWindowSize(nonMaxSize);
		}

		void detect(const cv::Mat &image) {
			cv::cornerHarris(image, cornerStrength, neighbourhood, aperture, k);
			double minStrength;
			cv::minMaxLoc(cornerStrength, &minStrength, &maxStrength);
			cv::Mat dilated;
			cv::dilate(cornerStrength, dilated, cv::Mat());
			cv::compare(cornerStrength, dilated, localMax, cv::CMP_EQ);
		}

		void getCorners(std::vector<cv::Point> &points, double qualityLevel) {
			cv::Mat cornerMap = getCornerMap(qualityLevel);
			getCorners(points, cornerMap);
		}

		void drawOnImage(cv::Mat &image, const std::vector<cv::Point> &points, 
			cv::Scalar color = cv::Scalar(255, 255, 255), 
			int radius = 3, int thickness = 2) {
			std::vector<cv::Point>::const_iterator it = points.begin();

			while (it != points.end()) {
				cv::circle(image, *it, radius, color, thickness);
				++it;
			}
		}
	};
}

char *g_ImgName = "D:/workspace/stereo001R.jpg";

int main() {
	cv::Mat image = cv::imread(g_ImgName);
	cv::Mat gray;
	cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
	cv::namedWindow("1");
	cv::namedWindow("2");
	cv::moveWindow("1", 10, 10);
	cv::moveWindow("2", 10 + image.cols, 10);
	cv::imshow("1", image);

	iMiner::HarrisDetector harris;
	harris.detect(gray);
	std::vector<cv::Point> pts;
	harris.getCorners(pts, 0.01);
	harris.drawOnImage(image, pts, cv::Scalar(200, 0, 0));

	cv::imshow("2", image);

	cv::waitKey(0);

	return 0;
}