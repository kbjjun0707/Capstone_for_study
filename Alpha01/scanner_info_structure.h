#pragma once

#include <opencv2\opencv.hpp>

namespace iMiner {
	class CameraIntrinsics {
	public:
		CameraIntrinsics(const cv::Mat &K = cv::Mat::eye(3, 3, CV_64FC1), const cv::Mat &distCoeff = cv::Mat::zeros(1, 5, CV_64FC1)) {
			this->K = K;
			this->Kinv = K.inv();
			this->distortion = distCoeff;
		}
		cv::Mat		K;
		cv::Mat		Kinv;
		cv::Mat		distortion;
		CameraIntrinsics *clonePtr() {
			return (new CameraIntrinsics(this->K, this->distortion));
		}
	};

	typedef cv::Matx34d Pose;

	class Perspective {
	public:
		Perspective(CameraIntrinsics *k = nullptr) {
			this->K = k;
		}
		~Perspective() {
		}
		CameraIntrinsics*	K;
		cv::Mat				R;
		cv::Mat				t;
		Pose				P;
		void setP(const cv::Matx34d &p) {
			cv::Matx33d test = p.get_minor<3, 3>(0, 0);
			R = cv::Mat(p.get_minor<3, 3>(0, 0));
			t = cv::Mat(p.get_minor<3, 1>(0, 3));
			P = p;
		}
		void setP(const cv::Mat &R, const cv::Mat &t) {
			this->R = R;
			this->t = t;
			P = Pose(
				R.at<double>(0, 0), R.at<double>(0, 1), R.at<double>(0, 2), t.at<double>(0),
				R.at<double>(1, 0), R.at<double>(1, 1), R.at<double>(1, 2), t.at<double>(1),
				R.at<double>(2, 0), R.at<double>(2, 1), R.at<double>(2, 2), t.at<double>(2)
			);
		}
		void setP() {
			P = Pose(
				R.at<double>(0, 0), R.at<double>(0, 1), R.at<double>(0, 2), t.at<double>(0),
				R.at<double>(1, 0), R.at<double>(1, 1), R.at<double>(1, 2), t.at<double>(1),
				R.at<double>(2, 0), R.at<double>(2, 1), R.at<double>(2, 2), t.at<double>(2)
			);
		}
	};

	class Features {
	public:
		std::vector<cv::KeyPoint>	kpt;
		std::vector<cv::Point2f>	pt;
		cv::Mat						desciptor;
	};

	typedef std::vector<cv::DMatch>	Match;

	typedef std::vector<Match>		Matches;

}