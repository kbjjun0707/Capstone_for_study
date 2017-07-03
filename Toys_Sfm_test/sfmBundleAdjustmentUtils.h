#pragma once

#include "sfmCommon.h"

namespace sfmlib {
	class SfMBundleAdjustmentUtils {
	public:
		/**
		*
		* @param pointCloud
		* @param cameraPoses
		* @param intrinsics
		* @param image2dFeatures
		*/
		static void adjustBundle(
			PointCloud&                     pointCloud,
			std::vector<cv::Matx34f>&       cameraPoses,
			Intrinsics&                     intrinsics,
			const std::vector<Features>&    image2dFeatures
		);
	};
}