#pragma once

#include "scanner_info_structure.h"

#include <qdir.h>

#include <opencv2\xfeatures2d.hpp>
#include <string>
#include <thread>
#include <set>

namespace iMiner {

	class Scanner {
		const double NN_MATCH_RATIO = 0.8f; // Nearest-neighbour matching ratio

		iMiner::CameraIntrinsics m_Intrinsics;
		cv::Ptr<cv::xfeatures2d::SURF> surf;

		cv::Mat	m_StdColor, m_StdGray;

		std::vector<std::string>	m_ImgNames;
		std::vector<Perspective>	m_Cameras;
		std::vector<Features>		m_ImgFeatures;
		std::vector<Match>			m_MatchInfo;

		bool		m_Scanning;
		//std::thread	m_FeatureThread;
		std::thread	m_SfmThread;
		
		void featureProc();

		void recoverProc();

		void featureMatch();

		void recoverPose();
	public:
		int	m_FeatureDone, m_MatchDone, m_RecoverDone, m_ImgNum;

		Scanner(const iMiner::CameraIntrinsics &intrinsic);

		Scanner(const cv::Mat &K = cv::Mat::eye(3, 3, CV_64FC1), const cv::Mat &distCoeff = cv::Mat::zeros(1, 5, CV_64FC1));

		~Scanner();
		
		void setBaseCameraIntrinsics(const cv::Mat &K, const cv::Mat &distCoeff = cv::Mat::zeros(1, 5, CV_64FC1));

		void clearStorage();

		void addImg(const char *imgName, bool otherCamera = false);

		void processing();		

		void runThread();

		void stopThread();

		float *getAffineCameraPose(int idx);

		std::vector<cv::Mat> testData;
	};

}
