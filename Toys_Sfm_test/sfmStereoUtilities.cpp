#include "sfmStereoUtilities.h"

#include <iostream>
using namespace cv;
using namespace std;

namespace sfmlib {
	const double RANSAC_THRESHOLD = 9.0f; // RANSAC inlier threshold
	const float MIN_REPROJECTION_ERROR = 10.0; // Maximum 10-pixel allowed re-projection error

	SfMStereoUtilities::SfMStereoUtilities() {

	}

	SfMStereoUtilities::~SfMStereoUtilities() {
	}

	// Find number of homography inliers
	int SfMStereoUtilities::findHomographyInliers(
		const Features& left,
		const Features& right,
		const Matching& matches) {

		Features alignedLeft;
		Features alignedRight;
		// ���� ������ �������� ���� ��ġ�Ǵ� Ű����Ʈ ���� ������ aligned Features�� ����
		GetAlignedPointsFromMatch(left, right, matches, alignedLeft, alignedRight);

		Mat inlierMask;
		Mat homography;
		// ��Ī ���� 4�� �̻��� �� ȣ��׷��� ����� ���Ѵ�. (RANSAC_THRESHOLD = 10)
		// Mask�� ����.
		if (matches.size() >= 4) {
			homography = findHomography(alignedLeft.points, alignedRight.points,
				cv::RANSAC, RANSAC_THRESHOLD, inlierMask);
		}

		if (matches.size() < 4 || homography.empty()) {
			return 0;
		}

		return countNonZero(inlierMask);
	}

	bool SfMStereoUtilities::findCameraMatricesFromMatch(
		const Intrinsics&   intrinsics,
		const Matching&     matches,
		const Features&     featuresLeft,
		const Features&     featuresRight,
		Matching&     		prunedMatches,
		cv::Matx34f&        Pleft,
		cv::Matx34f&        Pright) {

		if (intrinsics.K.empty()) {
			cerr << "Intrinsics matrix (K) must be initialized." << endl;
			return false;
		}

		// fx, fy�� �ƴϰ� focal length
		double focal = intrinsics.K.at<double>(0, 0); // Note: assuming fx = fy
		// ī�޶��� ���� (Principal point)
		cv::Point2d pp(intrinsics.K.at<double>(0, 2), intrinsics.K.at<double>(1, 2));

		Features alignedLeft;
		Features alignedRight;
		// ���� ������ �������� ���� ��ġ�Ǵ� Ű����Ʈ ���� ������ aligned Features�� ����
		GetAlignedPointsFromMatch(featuresLeft, featuresRight, matches, alignedLeft, alignedRight);

		// Essential Matrix ���ϰ�
		Mat E, R, t;
		Mat mask;
		// mask�� Inliers�� ��Ÿ��. �� �� x, x`�� ���Ͽ� ((x`)^T)E(x) = 0 �� Ư���� �̿��Ͽ� Essential matrix�� ����
		E = findEssentialMat(alignedLeft.points, alignedRight.points, intrinsics.K, RANSAC, 0.998999999999999, 1.0, mask);
		// Essential Matrix�� �� ����� ù��° ī�޶� ������ R(3X3), t(3X1)�� ���Ѵ�.
		recoverPose(E, alignedLeft.points, alignedRight.points, intrinsics.K, R, t, mask);

		// R, t -> [R|t]
		Pleft = Matx34f::eye();
		Pright = Matx34f(
			R.at<double>(0, 0), R.at<double>(0, 1), R.at<double>(0, 2), t.at<double>(0),
			R.at<double>(1, 0), R.at<double>(1, 1), R.at<double>(1, 2), t.at<double>(1),
			R.at<double>(2, 0), R.at<double>(2, 1), R.at<double>(2, 2), t.at<double>(2));

		// ��ġ ������ �ɷ��� prunedMatches�� �ٽ� ��´�
		prunedMatches.clear();
		for (size_t i = 0; i < mask.rows; i++) {
			if (mask.at<uchar>(i)>0) {
				prunedMatches.push_back(matches[i]);
			}
		}

		return true;
	}

	bool SfMStereoUtilities::triangulateViews(
		const Intrinsics&  intrinsics,
		const ImagePair    imagePair,
		const Matching&    matches,
		const Features&    featuresLeft,
		const Features&    featuresRight,
		const cv::Matx34f& Pleft,
		const cv::Matx34f& Pright,
		PointCloud&        pointCloud) {

		//get aligned features left-right, with back reference to original indexing
		vector<int>	leftBackReference;
		vector<int>	rightBackReference;
		Features	alignedLeft;
		Features	alignedRight;

		// ���� ������ �������� ���� ��ġ�Ǵ� Ű����Ʈ ���� ������ aligned Features�� ����
		GetAlignedPointsFromMatch(
			featuresLeft,
			featuresRight,
			matches,
			alignedLeft,
			alignedRight,
			leftBackReference,
			rightBackReference);

		// normalized point�� K�� ���� P�� ������ point
		Mat normalizedLeftPts;
		Mat normalizedRightPts;
		undistortPoints(alignedLeft.points, normalizedLeftPts, intrinsics.K, intrinsics.distortion);
		undistortPoints(alignedRight.points, normalizedRightPts, intrinsics.K, intrinsics.distortion);

		// Homogeneous point�� p = (x, y, 1)^T
		Mat points3dHomogeneous;
		// �� normalized ���� normalized ī�޶�[R|t]��� ���� ���� �� Homogeneous point�� ��ȯ
		triangulatePoints(Pleft, Pright, normalizedLeftPts, normalizedRightPts, points3dHomogeneous);

		Mat points3d;
		convertPointsFromHomogeneous(points3dHomogeneous.t(), points3d);

		Mat rvecLeft;
		// 3X3 R -> 3X1 or 1X3 rvec
		Rodrigues(Pleft.get_minor<3, 3>(0, 0), rvecLeft);
		// 1X3 tvec
		Mat tvecLeft(Pleft.get_minor<3, 1>(0, 3).t());

		vector<Point2f> projectedOnLeft(alignedLeft.points.size());
		projectPoints(points3d, rvecLeft, tvecLeft, intrinsics.K, intrinsics.distortion, projectedOnLeft);

		Mat rvecRight;
		Rodrigues(Pright.get_minor<3, 3>(0, 0), rvecRight);
		Mat tvecRight(Pright.get_minor<3, 1>(0, 3).t());

		vector<Point2f> projectedOnRight(alignedRight.points.size());
		projectPoints(points3d, rvecRight, tvecRight, intrinsics.K, intrinsics.distortion, projectedOnRight);

		//{
		//	Mat outLeft(2048, 3072, CV_8UC3, Colors::BLACK);
		//	drawKeypoints(outLeft, alignedLeft.keyPoints, outLeft, Colors::RED);
		//	drawKeypoints(outLeft, PointsToKeyPoints(projectedOnLeft), outLeft, Colors::GREEN);
		//	Mat outRight(2048, 3072, CV_8UC3, Colors::BLACK);
		//	drawKeypoints(outRight, alignedRight.keyPoints, outRight, Colors::RED);
		//	drawKeypoints(outRight, PointsToKeyPoints(projectedOnRight), outRight, Colors::GREEN);
		//	Mat tmp;
		//	hconcat(outLeft, outRight, tmp);
		//	imshow("features", tmp, 0.25);
		//	waitKey(0);
		//}
		//Note: cheirality check (all points z > 0) was already performed at camera pose calculation

		for (size_t i = 0; i < points3d.rows; i++) {
			// check if point reprojection error is small enough
			// MIN_REPROJECTION_ERROR = 10.0, norm�� ���� �Ÿ� (normalize �ƴ�)
			if ((norm(projectedOnLeft [i] - alignedLeft.points [i]) > MIN_REPROJECTION_ERROR) ||
				(norm(projectedOnRight[i] - alignedRight.points[i]) > MIN_REPROJECTION_ERROR)) {
				continue;
			}

			Point3DInMap p;
			p.p = Point3f(points3d.at<float>(i, 0),
				points3d.at<float>(i, 1),
				points3d.at<float>(i, 2)
			);

			//use back reference to point to original features in images
			p.originatingViews[imagePair.left]  = leftBackReference [i];
			p.originatingViews[imagePair.right] = rightBackReference[i];

			pointCloud.push_back(p);
		}

		return true;
	}

	bool SfMStereoUtilities::findCameraPoseFrom2D3DMatch(
		const Intrinsics&     intrinsics,
		const Image2D3DMatch& match,
		cv::Matx34f&          cameraPose) {

		//Recover camera pose using 2D-3D correspondence
		Mat rvec, tvec;
		Mat inliers;
		solvePnPRansac(
			match.points3D,
			match.points2D,
			intrinsics.K,
			intrinsics.distortion,
			rvec,
			tvec,
			false,
			100,
			RANSAC_THRESHOLD,
			0.99,
			inliers
		);

		//check inliers ratio and reject if too small
		if (((float)countNonZero(inliers) / (float)match.points2D.size()) < POSE_INLIERS_MINIMAL_RATIO) {
			cerr << "Inliers ratio is too small: " << countNonZero(inliers) << " / " << match.points2D.size() << endl;
			return false;
		}

		Mat rotMat;
		Rodrigues(rvec, rotMat); //convert to a rotation matrix

		rotMat.copyTo(Mat(3, 4, CV_32FC1, cameraPose.val)(ROT));
		tvec.copyTo(Mat(3, 4, CV_32FC1, cameraPose.val)(TRA));

		return true;
	}
}