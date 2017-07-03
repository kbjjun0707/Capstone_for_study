#pragma warning(disable:4996)
#include <iostream>
#include <opencv2\opencv.hpp>
#include <opencv2\xfeatures2d.hpp>
#include <opencvInit.h>

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

char *Lw = "Left KeyPoins", *Rw = "Right KeyPoins", *Mw = "Match";
char	*g_LImgName = "E:/workspace/stereo001L.jpg",
		*g_RImgName = "E:/workspace/stereo001R.jpg";

int main() {
	namedWindow(Lw);
	namedWindow(Rw);
	namedWindow(Mw);
	moveWindow(Lw, 50, 50);

	cv::Mat m_LImg = cv::imread(g_LImgName);
	cv::Mat m_RImg = cv::imread(g_RImgName);
	cv::namedWindow("left");
	cv::namedWindow("right");
	cv::namedWindow("result01");
	cv::namedWindow("result02");
	cv::moveWindow("left", 10, 10);
	cv::moveWindow("right", 10 + m_LImg.cols, 10);
	cv::destroyAllWindows();
	cv::imshow("left", m_LImg);
	cv::imshow("right", m_RImg);

	vector<cv::KeyPoint> m_Left_Keypoints, m_Right_Keypoints;

	cv::Ptr<cv::xfeatures2d::SURF> surf = cv::xfeatures2d::SURF::create();
	//cv::Ptr<cv::FastFeatureDetector> ffd = cv::FastFeatureDetector::create();
	surf->detect(m_LImg, m_Left_Keypoints);
	surf->detect(m_RImg, m_Right_Keypoints);

	vector<cv::Point2f> m_Left_Points;
	for (cv::KeyPoint kp : m_Left_Keypoints)
		m_Left_Points.push_back(kp.pt);

	vector<cv::Point2f> m_Right_Points(m_Left_Points.size());

	cv::Mat m_Prevgray, m_Gray;
	if (m_LImg.channels() == 3) {
		cv::cvtColor(m_LImg, m_Prevgray, CV_RGB2GRAY);
		cv::cvtColor(m_RImg, m_Gray, CV_RGB2GRAY);
	} else {
		m_Prevgray = m_LImg;
		m_Gray = m_RImg;
	}

	//vector<uchar> vstatus;
	//vector<float> verror;
	//cv::calcOpticalFlowPyrLK(m_Prevgray, m_Gray, m_Left_Points, m_Right_Points, vstatus, verror);

	//vector<cv::Point2f> m_Right_Points_to_Find;
	//vector<int> m_Right_Points_to_Find_Back_Index;

	//for (unsigned int i = 0; i < vstatus.size(); i++) {
	//	if ((vstatus[i]) && (verror[i] < 12.0)) {
	//		m_Right_Points_to_Find_Back_Index.push_back(i);
	//		m_Right_Points_to_Find.push_back(m_Left_Points[i]);
	//	} else {
	//		vstatus[i] = 0;
	//	}
	//}

	//cv::Mat m_Right_Points_to_Find_Flat = cv::Mat(m_Right_Points_to_Find).reshape(1, m_Right_Points_to_Find.size());

	cv::Mat m_LDescript, m_RDescript;
	surf->compute(m_Prevgray, m_Left_Keypoints, m_LDescript);
	surf->compute(m_Gray, m_Right_Keypoints, m_RDescript);


	//vector<cv::Point2f> m_Right_Features;
	//for (int idx : m_Right_Points_to_Find_Back_Index)
	//	m_Right_Features.push_back(m_Right_Points[idx]);

	//cv::Mat m_Right_Features_Flat = cv::Mat(m_Right_Features).reshape(1, m_Right_Features.size());

	cv::BFMatcher matcher(CV_L2);
	vector<vector<cv::DMatch>> nearest_neighbors;
	vector<cv::DMatch> matches;

	//matcher.radiusMatch(
	//	m_Right_Points_to_Find_Flat,
	//	m_Right_Features_Flat,
	//	nearest_neighbors,
	//	2.0f
	//);

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
			//_m.queryIdx = m_Right_Points_to_Find_Back_Index[_m.queryIdx];
			matches.push_back(_m);
			found_in_right_points.insert(_m.trainIdx);
			//cout << "  차이 : " << m_Left_Keypoints[_m.queryIdx].pt << "     " << m_Right_Keypoints[_m.trainIdx].pt << endl;
		}
	}
	cout << "pruned " << matches.size() << " / " << nearest_neighbors.size() << " matches" << endl;
	
	cv::Mat rst1;
	cv::drawMatches(m_LImg, m_Left_Keypoints, m_RImg, m_Right_Keypoints, matches, rst1);
	cv::moveWindow("result01", 10, 10 + m_LImg.rows);
	cv::imshow("resutl01", rst1);


	//cv::Mat rst2 = m_LImg.clone();
	//for (int i = 0; i < m_Left_Points.size(); i++)
	//	cv::arrowedLine(rst2, m_Left_Points[i], m_Right_Points[i], cv::Scalar(0, 255, 0));

	//cv::moveWindow("result02", 10+rst1.cols, 10 + m_LImg.rows);
	//cv::imshow("resutl02", rst2);

	waitKey(0);


	return 0;
}