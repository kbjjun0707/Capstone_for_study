#pragma once

#include "sfmCommon.h"

namespace sfmlib {
	class SfM2DFeatureUtilities {
	public:
		SfM2DFeatureUtilities();
		virtual ~SfM2DFeatureUtilities();

		Features extractFeatures(const cv::Mat& image);

		static Matching matchFeatures(
			const Features& featuresLeft,
			const Features& featuresRight);

	private:
		cv::Ptr<cv::Feature2D>         mDetector;
		cv::Ptr<cv::DescriptorMatcher> mMatcher;

	};
}