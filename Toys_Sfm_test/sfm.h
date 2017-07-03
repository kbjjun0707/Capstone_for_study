#pragma once

#include "sfmCommon.h"
#include "sfm2DFeatureUtilities.h"

#include <string>
#include <vector>
#include <map>
#include <set>

namespace sfmlib {
	enum ErrorCode {
		OKAY = 0,
		ERROR
	};

	class SfM {
		typedef std::vector<std::vector<Matching> > MatchMatrix;

		typedef std::map<int, Image2D3DMatch> Images2D3DMatches;

	public:
		SfM(const float downscale = 1.0);
		virtual ~SfM();

		ErrorCode runSfM();

		void saveCloudAndCamerasToPLY(const std::string& filename);

		void setConsoleDebugLevel(unsigned int consoleDebugLevel) {
			mConsoleDebugLevel = MIN(LOG_ERROR, consoleDebugLevel);
		}

		void setVisualDebugLevel(unsigned int visualDebugLevel) {
			mVisualDebugLevel = MIN(LOG_ERROR, visualDebugLevel);
		}

		std::vector<Features>     mImageFeatures;
		std::vector<cv::Matx34f>  mCameraPoses;
		std::vector<cv::Mat>      mImages;
		Intrinsics                mIntrinsics;
		PointCloud                mReconstructionCloud;

		float                     mDownscaleFactor;

	private:
		
		void extractFeatures();

		void createFeatureMatchMatrix();

		void findBaselineTriangulation();

		void adjustCurrentBundle();

		std::map<float, ImagePair> sortViewsForBaseline();

		void addMoreViewsToReconstruction();

		Images2D3DMatches find2D3DMatches();
		
		void mergeNewPointCloud(const PointCloud& cloud);

		std::vector<std::string>  mImageFilenames;
		//std::vector<Features>     mImageFeatures;
		//std::vector<cv::Matx34f>  mCameraPoses;
		std::set<int>             mDoneViews;
		std::set<int>             mGoodViews;
		MatchMatrix               mFeatureMatchMatrix;
		SfM2DFeatureUtilities     mFeatureUtil;
		//std::vector<cv::Mat>      mImages;
		//Intrinsics                mIntrinsics;
		//PointCloud                mReconstructionCloud;
		unsigned int              mConsoleDebugLevel;
		unsigned int              mVisualDebugLevel;
		//float                     mDownscaleFactor;
	};
}