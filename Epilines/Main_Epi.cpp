#pragma warning(disable:4996)

#include <opencv2\opencv.hpp>
#include <opencv2\stereo\stereo.hpp>
#include <opencv2\features2d\features2d.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\calib3d\calib3d.hpp>
#include <opencv2\xfeatures2d.hpp>
#include <opencvInit.h>

#include <iostream>

using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;

int main() {
	Mat imgL = imread("E:/Workspace/stereo003L.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	Mat imgR = imread("E:/Workspace/stereo003R.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	//resize(imgL, imgL, Size(), 0.2, 0.2);
	//resize(imgR, imgR, Size(), 0.2, 0.2);

	if (!imgL.data || !imgR.data)
		return -1;

	vector<KeyPoint> keypointsL;
	vector<KeyPoint> keypointsR;

	Ptr<SURF> surf = SURF::create();

	surf->detect(imgL, keypointsL);
	surf->detect(imgR, keypointsR);

	cout << "Number of SURF points (1): " << keypointsL.size() << endl;
	cout << "Number of SURF points (2): " << keypointsR.size() << endl;

	//Ptr<SiftDescriptorExtractor> siftDesc = SiftDescriptorExtractor::create();

	Mat descriptorsL, descriptorsR;
	surf->compute(imgL, keypointsL, descriptorsL);
	surf->compute(imgR, keypointsR, descriptorsR);
	
	//BFMatcher matcher(NORM_L2);

	//vector<DMatch> matches;
	////vector<vector<DMatch>> namtches;
	//matcher.match(descriptorsL, descriptorsR, matches);

	cv::BFMatcher matcher(CV_L2);
	vector<vector<cv::DMatch>> nearest_neighbors;
	vector<cv::DMatch> matches;

	matcher.radiusMatch(
		descriptorsL,
		descriptorsR,
		nearest_neighbors,
		2.0f
	);

	cout << "Number of nearest neighbors points: " << nearest_neighbors.size() << endl;
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
		}
	}

	cout << "Number of matched points: " << matches.size() << endl;

	vector<int> pointIndexesL;
	vector<int> pointIndexesR;

	for (vector<DMatch>::const_iterator it = matches.begin(); it != matches.end(); ++it) {
		pointIndexesL.push_back(it->queryIdx);
		pointIndexesR.push_back(it->trainIdx);
	}

	vector<cv::Point2f> selPointsL, selPointsR;
	KeyPoint::convert(keypointsL, selPointsL, pointIndexesL);
	KeyPoint::convert(keypointsR, selPointsR, pointIndexesR);

	Mat matchedImg;
	drawMatches(imgL, keypointsL, imgR, keypointsR, matches, matchedImg);
	imshow("matches", matchedImg);
	waitKey(0);

	//vector<cv::Point2f>::const_iterator it = selPointsL.begin();
	//while (it != selPointsL.end()) {
	//	circle(imgL, *it, 3, cv::Scalar(255, 255, 255), 2);
	//	++it;
	//}

	Mat fundemental = findFundamentalMat(
		selPointsL, // points in first image
		selPointsR, // points in second image
		//CV_FM_8POINT
		CV_RANSAC
	);
	cout << "Fundamental Matrix:" << endl << fundemental << endl << endl;
		
	vector<cv::Vec3f> linesL;
	computeCorrespondEpilines(
		selPointsL,				// image points
		1,                      // in image 1 (can also be 2)
		fundemental,            // F matrix
		linesL);				// vector of epipolar lines
	for (vector<Vec3f>::const_iterator it = linesL.begin(); it != linesL.end(); ++it) {
		line(imgR, cv::Point(0, -(*it)[2] / (*it)[1]), Point(imgR.cols, -((*it)[2] + (*it)[0] * imgR.cols) / (*it)[1]), Scalar(255, 255, 255));
	}
	vector<cv::Vec3f> linesR;
	computeCorrespondEpilines(
		selPointsR,
		2, 
		fundemental, 
		linesR);
	for (vector<Vec3f>::const_iterator it = linesR.begin(); it != linesR.end(); ++it) {
		line(imgL, cv::Point(0, -(*it)[2] / (*it)[1]), Point(imgL.cols, -((*it)[2] + (*it)[0] * imgL.cols) / (*it)[1]), Scalar(255, 255, 255));
	}

	//for (int i = 0; i < selPointsL.size(); i++) {
	//	//double dist1 = (linesL[i][0] * linesL[i][0]) + (linesL[i][1] * linesL[i][1]) + (linesL[i][2] * linesL[i][2]);
	//	//double dist2 = (linesR[i][0] * linesR[i][0]) + (linesR[i][1] * linesR[i][1]) + (linesR[i][2] * linesR[i][2]);
	//	//dist1 = sqrt(dist1);
	//	//dist2 = sqrt(dist2);
	//	//cout << linesL[i].mul(1.0 / dist1) << "   " << linesR[i].mul(1.0 / dist2) << "   " << endl << linesL[i].dot(linesR[i]) << endl;
	//	Mat tmp1 = Mat::eye(3, 1, CV_64F);
	//	tmp1.at<double>(0, 0) = selPointsL[i].x;
	//	tmp1.at<double>(1, 0) = selPointsL[i].y;
	//	Mat tmp2 = Mat::eye(3, 1, CV_64F);
	//	tmp2.at<double>(0, 0) = selPointsL[i].x;
	//	tmp2.at<double>(1, 0) = selPointsL[i].y;
	//	Mat tmp3 = Mat(3, 1, CV_64F);
	//	tmp3.at<double>(0, 0) = linesR[i][0];
	//	tmp3.at<double>(1, 0) = linesR[i][1];
	//	tmp3.at<double>(2, 0) = linesR[i][2];

	//	cout << tmp2.t().dot(fundemental) << endl;

	//	//cout << (fundemental*tmp1).dot(tmp2.t()) << endl;
	//}

	namedWindow("Right Image Epilines");
	moveWindow("Right Image Epilines", 10+ imgR.cols, 10);
	imshow("Right Image Epilines", imgR);
	namedWindow("Left Image Epilines");
	moveWindow("Left Image Epilines", 10, 10);
	imshow("Left Image Epilines", imgL);

	waitKey(0);



	return 0;
}