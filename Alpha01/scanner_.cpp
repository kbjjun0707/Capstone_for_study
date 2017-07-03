#include "scanner_.h"

#include <opencv2\sfm.hpp>

iMiner::Scanner::Scanner(const iMiner::CameraIntrinsics &intrinsics) {
	m_Intrinsics = intrinsics;
	m_RecoverDone = 0;
	m_MatchDone = 0;
	m_FeatureDone = 0;
	m_ImgNum = 0;
	m_Scanning = false;
	surf = cv::xfeatures2d::SURF::create();
}

iMiner::Scanner::Scanner(const cv::Mat &K, const cv::Mat &distCoeff) {
	m_Intrinsics.K = K;
	m_Intrinsics.Kinv = K.inv();
	m_Intrinsics.distortion = distCoeff;
	m_RecoverDone = 0;
	m_MatchDone = 0;
	m_FeatureDone = 0;
	m_ImgNum = 0;
	m_Scanning = false;
	surf = cv::xfeatures2d::SURF::create();
}

iMiner::Scanner::~Scanner() {
	stopThread();
	m_RecoverDone = 0;
	m_MatchDone = 0;
	m_FeatureDone = 0;
	m_ImgNum = 0;
		
	surf.release();

	//if (m_FeatureThread.joinable()) m_FeatureThread.join();
}

void iMiner::Scanner::featureProc() {
	if (m_Scanning && (m_FeatureDone < m_ImgNum)) {
		cv::Mat color = cv::imread(m_ImgNames[m_FeatureDone]);
		cv::Mat gray;
		cv::cvtColor(color, gray, cv::COLOR_BGR2GRAY);

		Features features;
		surf->detect(gray, features.kpt);
		surf->compute(gray, features.kpt, features.desciptor);
		cv::KeyPoint::convert(features.kpt, features.pt);

		m_ImgFeatures.push_back(features);
		m_FeatureDone++;
	}
}

void iMiner::Scanner::recoverProc() {
	if (!m_Scanning) return;
	featureMatch();
	recoverPose();
}

void iMiner::Scanner::featureMatch() {
	cv::BFMatcher l_matcher(CV_L2);
	std::vector<std::vector<cv::DMatch>> l_nearest_neighbors;
	std::vector<cv::DMatch> prunedMatching;

	if (!m_Scanning) return;
	if ((m_MatchDone == 0) && (m_FeatureDone > 0)) {
		m_MatchInfo.push_back(prunedMatching);
		m_MatchDone++;
	} else if ((m_FeatureDone > 1) && (m_MatchDone < m_FeatureDone)) {
		Features a = m_ImgFeatures[0], b = m_ImgFeatures[m_MatchDone];

		l_matcher.radiusMatch(a.desciptor, b.desciptor, l_nearest_neighbors, 2.0f);
		for (unsigned i = 0; i < l_nearest_neighbors.size(); i++) {
			if (l_nearest_neighbors[i][0].distance < NN_MATCH_RATIO * l_nearest_neighbors[i][1].distance) {
				prunedMatching.push_back(l_nearest_neighbors[i][0]);
			}
		}
		m_MatchInfo.push_back(prunedMatching);

		m_MatchDone++;
	}
}

void iMiner::Scanner::recoverPose() {

	if (!m_Scanning) return;
	if ((m_RecoverDone == 0) && (m_MatchDone > 0)) {
		Perspective P1(&m_Intrinsics);
		cv::Matx34d Pa = cv::Matx34d::eye();

		P1.setP(Pa);
		m_Cameras.push_back(P1);
		m_RecoverDone++;
	} else if ((m_MatchDone > 0) && (m_RecoverDone < m_MatchDone)) {
		Perspective P2(&m_Intrinsics);

		Features a = m_ImgFeatures[0];
		Features b = m_ImgFeatures[m_RecoverDone];
		Features AlignedA, AlignedB;
		Match _m = m_MatchInfo[m_RecoverDone];
		for (int i = 0; i < _m.size(); i++) {
			AlignedA.kpt.push_back(a.kpt[_m[i].queryIdx]);
			AlignedA.pt.push_back(a.pt[_m[i].queryIdx]);
			AlignedA.desciptor.push_back(a.desciptor.row(_m[i].queryIdx));

			AlignedB.kpt.push_back(b.kpt[_m[i].trainIdx]);
			AlignedB.pt.push_back(b.pt[_m[i].trainIdx]);
			AlignedB.desciptor.push_back(b.desciptor.row(_m[i].trainIdx));
		}
		cv::Mat E, R, t, F;
		cv::Mat mask;
		
		E = cv::findEssentialMat(AlignedA.pt, AlignedB.pt, m_Intrinsics.K, CV_RANSAC, 0.999, 1.0, mask);
		cv::recoverPose(E, AlignedA.pt, AlignedB.pt, m_Intrinsics.K, R, t);

		cv::Mat normalizedLeftPts;
		cv::Mat normalizedRightPts;
		cv::undistortPoints(AlignedA.pt, normalizedLeftPts, m_Intrinsics.K, m_Intrinsics.distortion);
		cv::undistortPoints(AlignedB.pt, normalizedRightPts, m_Intrinsics.K, m_Intrinsics.distortion);

		P2.setP(R, t);
		m_Cameras.push_back(P2);

		cv::Mat points3dHomogeneous;
		triangulatePoints(m_Cameras[0].P, m_Cameras[m_RecoverDone].P, normalizedLeftPts, normalizedRightPts, points3dHomogeneous);

		cv::Mat points3d;
		cv::convertPointsFromHomogeneous(points3dHomogeneous.t(), points3d);

		testData.push_back(points3d);

		m_RecoverDone++;
	}

}

void iMiner::Scanner::setBaseCameraIntrinsics(const cv::Mat &K, const cv::Mat &distCoeff) {
	m_Intrinsics.K = K;
	m_Intrinsics.Kinv = K.inv();
	m_Intrinsics.distortion = distCoeff;
}

void iMiner::Scanner::clearStorage() {
	stopThread();
	m_RecoverDone = 0;
	m_MatchDone = 0;
	m_FeatureDone = 0;
	m_ImgNum = 0; 

	m_ImgNames.clear();
	m_Cameras.clear();
	m_ImgFeatures.clear();
	testData.clear();

	runThread();
}

void iMiner::Scanner::addImg(const char * imgName, bool otherCamera) {
	if (otherCamera) {

	} else {
		m_ImgNames.push_back(imgName);
		m_ImgNum++;
	}
}

void iMiner::Scanner::processing() {
	while (m_Scanning) {
		featureProc();
		recoverProc();
	}
}

void iMiner::Scanner::runThread() {
	if (!m_Scanning) {
		m_Scanning = true;
		m_SfmThread = std::thread([&]() {
			processing();
		});
	}
}

void iMiner::Scanner::stopThread() {
	m_Scanning = false;
	if (m_SfmThread.joinable())
		m_SfmThread.join();
}

float * iMiner::Scanner::getAffineCameraPose(int idx) {
	Perspective p = m_Cameras[idx];
	cv::Affine3d resAf(p.R, p.t);
	
	static cv::Mat res;
	
	res = cv::Mat(resAf.matrix);
	res.convertTo(res, CV_32FC1);
	res = res.t();

	return ((float *)res.data);
}
