#include "sfm.h"
#include "sfmStereoUtilities.h"
#include "sfmBundleAdjustmentUtils.h"

#include <iostream>
#include <algorithm>
#include <thread>
#include <mutex>

using namespace std;
using namespace cv;

namespace sfmlib {
	const float MERGE_CLOUD_POINT_MIN_MATCH_DISTANCE = 0.01f;
	const float MERGE_CLOUD_FEATURE_MIN_MATCH_DISTANCE = 20.0f;
	const int   MIN_POINT_COUNT_FOR_HOMOGRAPHY = 100;

	SfM::SfM(const float downscale) :
		mVisualDebugLevel(LOG_INFO),
		mConsoleDebugLevel(LOG_INFO),
		mDownscaleFactor(downscale) {


		//// 1. 보정된 카메라 매트릭스 입력
		//double K_Data[] = {
		//	759.738,            0,	251.326,
		//	0,      758.256,	191.065,
		//	0,            0,                 1
		//};
		//mIntrinsics.K = cv::Mat(3, 3, CV_64FC1, K_Data).clone();
		//mIntrinsics.Kinv = mIntrinsics.K.inv();
		//double dC_Data[] = { 0.0295856678258018, -0.4875803173620251, -0.008405564255234423, -0.01679550574929698, 1.40807775361489 };
		//mIntrinsics.distortion = cv::Mat(1, 5, CV_64FC1, dC_Data).clone();
		
	}

	SfM::~SfM() {
	}

	ErrorCode SfM::runSfM() {
		if (mImages.size() <= 0) {
			cerr << "No images to work on." << endl;
			return ErrorCode::ERROR;
		}

		//initialize intrinsics
		mIntrinsics.K = cv::Mat(cv::Matx33d(
			2500.0, 0, mImages[0].cols / 2.0,
			0, 2500.0, mImages[0].rows / 2.0,
			0, 0, 1));
		mIntrinsics.Kinv = mIntrinsics.K.inv();
		mIntrinsics.distortion = Mat_<double>::zeros(1, 4);
		
		mCameraPoses.resize(mImages.size());

		// 1. 특징점 추출
		extractFeatures();

		// 2. 특징점 매칭 정보
		createFeatureMatchMatrix();

		// 3. Triangulation
		findBaselineTriangulation();

		// 4. 더 많은 뷰를 3차원 지도에 추가
		addMoreViewsToReconstruction();

		if (mConsoleDebugLevel <= LOG_INFO) {
			cout << "----------------------- Done -----------------------" << endl;
		}

		return OKAY;
	}

	// 1. 특징점 추출
	void SfM::extractFeatures() {
		if (mConsoleDebugLevel <= LOG_INFO) {
			cout << "----------------- Extract Features -----------------" << endl;
		}

		// vector<Features> = mImageFeatures
		// Features -> 키포인트, 2D 점, descriptor 의 struct
		mImageFeatures.resize(mImages.size());
		for (size_t i = 0; i < mImages.size(); i++) {
			// 특징점 추출 및 KeyPoint to 2D Point
			mImageFeatures[i] = mFeatureUtil.extractFeatures(mImages[i]);

			if (mConsoleDebugLevel <= LOG_DEBUG) {
				cout << "Image " << i << ": " << mImageFeatures[i].keyPoints.size() << " keypoints" << endl;
			}
		}
	}

	// 2. 특징점 매칭 정보
	void SfM::createFeatureMatchMatrix() {
		if (mConsoleDebugLevel <= LOG_INFO) {
			cout << "----------- Create Feature Match Matrix ------------" << endl;
		}

		const size_t numImages = mImages.size();
		// Matching -> vector<DMatch> 키포인트들의 매치 정보
		// mFeatureMatchMatrix = MatchMatrix -> vector<Matching> 매치 정보들의 배열 (여러 장의 사진 매치 정보)
		mFeatureMatchMatrix.resize(numImages, vector<Matching>(numImages));

		// 모든 이미지들을 2개의 쌍으로 잇는다
		vector<ImagePair> pairs;
		for (size_t i = 0; i < numImages; i++) {
			for (size_t j = i + 1; j < numImages; j++) {
				pairs.push_back({ i, j });
			}
		}

		// 여러 쓰레드 생성 관리 배열
		vector<thread> threads;

		//find out how many threads are supported, and how many pairs each thread will work on
		const int numThreads = std::thread::hardware_concurrency() - 1;
		const int numPairsForThread = (numThreads > pairs.size()) ? 1 : (int)ceilf((float)(pairs.size()) / numThreads);

		mutex writeMutex;

		if (mConsoleDebugLevel <= LOG_DEBUG) {
			cout << "Launch " << numThreads << " threads with " << numPairsForThread << " pairs per thread" << endl;
		}

		//invoke each thread with its pairs to process (if less pairs than threads, invoke only #pairs threads with 1 pair each)
		for (size_t threadId = 0; threadId < MIN(numThreads, pairs.size()); threadId++) {
			threads.push_back(thread([&, threadId] {
				const int startingPair = numPairsForThread * threadId;

				for (int j = 0; j < numPairsForThread; j++) {
					const int pairId = startingPair + j;
					if (pairId >= pairs.size()) { //make sure threads don't overflow the pairs
						break;
					}
					const ImagePair& pair = pairs[pairId];

					// 왼쪽 이미지와 다른 이미지들의 Matching값 저장, 두장의 Features(Keypoint...)들을 입력
					mFeatureMatchMatrix[pair.left][pair.right] = SfM2DFeatureUtilities::matchFeatures(mImageFeatures[pair.left], mImageFeatures[pair.right]);

					if (mConsoleDebugLevel <= LOG_DEBUG) {
						writeMutex.lock();
						cout << "Thread " << threadId << ": Match (pair " << pairId << ") " << pair.left << ", " << pair.right << ": " << mFeatureMatchMatrix[pair.left][pair.right].size() << " matched features" << endl;
						writeMutex.unlock();
					}
				}
			}));
		}

		//wait for threads to complete
		for (auto& t : threads) {
			t.join();
		}
	}


	// 3. Triangulation
	void SfM::findBaselineTriangulation() {
		if (mConsoleDebugLevel <= LOG_INFO) {
			cout << "----------- Find Baseline Triangulation ------------" << endl;
		}

		if (mConsoleDebugLevel <= LOG_DEBUG) {
			cout << "--- Sort views by homography inliers" << endl;
		}

		// maps are sorted, so the best pair is at the beginnning
		map<float, ImagePair> pairsHomographyInliers = sortViewsForBaseline();

		// P = K[R|t] (3X4)
		Matx34f Pleft = Matx34f::eye();
		Matx34f Pright = Matx34f::eye();
		PointCloud pointCloud;

		if (mConsoleDebugLevel <= LOG_DEBUG) {
			cout << "--- Try views in triangulation" << endl;
		}

		// try to find the best pair, stating at the beginning
		for (auto& imagePair : pairsHomographyInliers) {
			if (mConsoleDebugLevel <= LOG_DEBUG) {
				cout << "Trying " << imagePair.second << " ratio: " << imagePair.first << endl << flush;
			}
			size_t i = imagePair.second.left;
			size_t j = imagePair.second.right;

			if (mConsoleDebugLevel <= LOG_TRACE) {
				cout << "---- Find camera matrices" << endl;
			}

			Matching prunedMatching;
			// recover camera matrices (poses) from the point matching
			bool success = SfMStereoUtilities::findCameraMatricesFromMatch(
				mIntrinsics,
				mFeatureMatchMatrix[i][j],
				mImageFeatures[i],
				mImageFeatures[j],
				prunedMatching,
				Pleft, Pright
			);

			if (!success) {
				if (mConsoleDebugLevel <= LOG_WARN) {
					cerr << "stereo view could not be obtained " << imagePair.second << ", go to next pair" << endl << flush;
				}
				continue;
			}

			// (i, j의 이미지끼리 걸러낸 매치 정보의 수)/(i, j의 이미지끼리 매치 정보의 전체 수)
			float poseInliersRatio = (float)prunedMatching.size() / (float)mFeatureMatchMatrix[i][j].size();

			if (mConsoleDebugLevel <= LOG_TRACE) {
				cout << "pose inliers ratio " << poseInliersRatio << endl;
			}

			// POSE_INLIERS_MINIMAL_RATIO=0.5
			if (poseInliersRatio < POSE_INLIERS_MINIMAL_RATIO) {
				if (mConsoleDebugLevel <= LOG_TRACE) {
					cout << "insufficient pose inliers. skip." << endl;
				}
				continue;
			}

			if (mVisualDebugLevel <= LOG_INFO) {
				Mat outImage;
				drawMatches(mImages[i], mImageFeatures[i].keyPoints,
					mImages[j], mImageFeatures[j].keyPoints,
					prunedMatching,
					outImage);
				resize(outImage, outImage, Size(), 0.5, 0.5);
				cv::namedWindow("outimage");
				cv::moveWindow("outimage", 10, 10);
				imshow("outimage", outImage);
				waitKey(40);
			}

			// poseInliersRatio >= 0.5이면 i, j의 매치 정보를 걸러진 매치 정보로 수정
			mFeatureMatchMatrix[i][j] = prunedMatching;

			if (mConsoleDebugLevel <= LOG_DEBUG) {
				cout << "---- Triangulate from stereo views: " << imagePair.second << endl;
			}

			// i, j의 걸러진 매치 정보와 각각의 키포인트들로부터...
			success = SfMStereoUtilities::triangulateViews(
				mIntrinsics,
				imagePair.second,
				mFeatureMatchMatrix[i][j],
				mImageFeatures[i], mImageFeatures[j],
				Pleft, Pright,
				pointCloud
			);

			if (!success) {
				if (mConsoleDebugLevel <= LOG_WARN) {
					cerr << "could not triangulate: " << imagePair.second << endl << flush;
				}
				continue;
			}

			mReconstructionCloud = pointCloud;
			mCameraPoses[i] = Pleft;
			mCameraPoses[j] = Pright;
			mDoneViews.insert(i);
			mDoneViews.insert(j);
			mGoodViews.insert(i);
			mGoodViews.insert(j);

			adjustCurrentBundle();

			break;
		}
	}

	// 4. 보정
	void SfM::adjustCurrentBundle() {
		SfMBundleAdjustmentUtils::adjustBundle(
			mReconstructionCloud,
			mCameraPoses,
			mIntrinsics,
			mImageFeatures);

		for (auto pc : mReconstructionCloud) {
			sfmlib::Point3DInMapRGB tmp;
			tmp.p = pc;
			tmp.rgb = cv::Scalar(0, 0, 0);
			int cnt = 0;
			for (auto i : pc.originatingViews) {
				cv::Point tmppt = mImageFeatures[i.first].points[i.second];
				cv::Vec3b tmpbgr = mImages[i.first].at<cv::Vec3b>(tmppt);
				tmp.rgb += cv::Scalar(tmpbgr[0], tmpbgr[1], tmpbgr[2]);
				cnt++;
			}
			tmp.rgb /= cnt;
			mReconstructionCloudRGB.push_back(tmp);
		}
	}


	map<float, ImagePair> SfM::sortViewsForBaseline() {
		if (mConsoleDebugLevel <= LOG_INFO) {
			cout << "---------- Find Views Homography Inliers -----------" << endl;
		}

		// sort pairwise matches to find the lowest Homography inliers [Snavely07 4.2]
		map<float, ImagePair> matchesSizes;

		const size_t numImages = mImages.size();
		for (size_t i = 0; i < numImages - 1; i++) {
			for (size_t j = i + 1; j < numImages; j++) {
				// i번째와 j번째의 Matching(vector<cv::DMatch>)이 MIN_POINT_COUNT_FOR_HOMOGRAPHY(=100) 보다 작을 때
				// 지나감
				if (mFeatureMatchMatrix[i][j].size() < MIN_POINT_COUNT_FOR_HOMOGRAPHY) {
					//Not enough points in matching
					//matchesSizes[1.0] = { i, j };
					continue;
				}

				// Find number of homography inliers
				const int numInliers = SfMStereoUtilities::findHomographyInliers(
					mImageFeatures[i],
					mImageFeatures[j],
					mFeatureMatchMatrix[i][j]);
				// 매칭정보와 호모그르피 행렬 결과의 관계에따라 
				const float inliersRatio = (float)numInliers / (float)(mFeatureMatchMatrix[i][j].size());
				matchesSizes[inliersRatio] = { i, j };

				if (mConsoleDebugLevel <= LOG_DEBUG) {
					cout << "Homography inliers ratio: [" << i << ", " << j << "]  " << inliersRatio << endl;
				}
			}
		}

		return matchesSizes;
	}

	void SfM::addMoreViewsToReconstruction() {
		if (mConsoleDebugLevel <= LOG_INFO) {
			cout << "------------------ Add More Views ------------------" << endl;
		}

		while (mDoneViews.size() != mImages.size()) {
			// Find the best view to add, according to the largest number of 2D-3D corresponding points
			Images2D3DMatches matches2D3D = find2D3DMatches();

			size_t bestView;
			size_t bestNumMatches = 0;
			for (const auto& match2D3D : matches2D3D) {
				const size_t numMatches = match2D3D.second.points2D.size();
				if (numMatches > bestNumMatches) {
					bestView = match2D3D.first;
					bestNumMatches = numMatches;
				}
			}
			if (mConsoleDebugLevel <= LOG_DEBUG) {
				cout << "Best view " << bestView << " has " << bestNumMatches << " matches" << endl;
				cout << "Adding " << bestView << " to existing " << Mat(vector<int>(mGoodViews.begin(), mGoodViews.end())).t() << endl;
			}

			mDoneViews.insert(bestView);

			// recover the new view camera pose
			Matx34f newCameraPose;
			bool success = SfMStereoUtilities::findCameraPoseFrom2D3DMatch(
				mIntrinsics,
				matches2D3D[bestView],
				newCameraPose);

			if (!success) {
				if (mConsoleDebugLevel <= LOG_WARN) {
					cerr << "Cannot recover camera pose for view " << bestView << endl;
				}
				continue;
			}

			mCameraPoses[bestView] = newCameraPose;

			if (mConsoleDebugLevel <= LOG_DEBUG) {
				cout << "New view " << bestView << " pose " << endl << newCameraPose << endl;
			}

			// triangulate more points from new view to all existing good views
			bool anyViewSuccess = false;
			for (const int goodView : mGoodViews) {
				//since match matrix is upper-triangular (non symmetric) - use lower index as left
				size_t leftViewIdx = (goodView < bestView) ? goodView : bestView;
				size_t rightViewIdx = (goodView < bestView) ? bestView : goodView;

				Matching prunedMatching;
				Matx34f Pleft = Matx34f::eye();
				Matx34f Pright = Matx34f::eye();

				//use the essential matrix recovery to prune the matches
				bool success = SfMStereoUtilities::findCameraMatricesFromMatch(
					mIntrinsics,
					mFeatureMatchMatrix[leftViewIdx][rightViewIdx],
					mImageFeatures[leftViewIdx],
					mImageFeatures[rightViewIdx],
					prunedMatching,
					Pleft, Pright
				);
				mFeatureMatchMatrix[leftViewIdx][rightViewIdx] = prunedMatching;

				//triangulate the matching points
				PointCloud pointCloud;
				success = SfMStereoUtilities::triangulateViews(
					mIntrinsics,
					{ leftViewIdx, rightViewIdx },
					mFeatureMatchMatrix[leftViewIdx][rightViewIdx],
					mImageFeatures[leftViewIdx],
					mImageFeatures[rightViewIdx],
					mCameraPoses[leftViewIdx],
					mCameraPoses[rightViewIdx],
					pointCloud
				);

				if (success) {
					if (mConsoleDebugLevel <= LOG_DEBUG) {
						cout << "Merge triangulation between " << leftViewIdx << " and " << rightViewIdx <<
							" (# matching pts = " << (mFeatureMatchMatrix[leftViewIdx][rightViewIdx].size()) << ") ";
					}

					//add new points to the reconstruction
					mergeNewPointCloud(pointCloud);

					anyViewSuccess = true;
				} else {
					if (mConsoleDebugLevel <= LOG_WARN) {
						cerr << "Failed to triangulate " << leftViewIdx << " and " << rightViewIdx << endl;
					}
				}
			}

			//Adjust bundle if any additional view was added
			if (anyViewSuccess) {
				adjustCurrentBundle();
			}
			mGoodViews.insert(bestView);
		}
	}

	SfM::Images2D3DMatches SfM::find2D3DMatches() {
		Images2D3DMatches matches;

		//scan all not-done views
		for (size_t viewIdx = 0; viewIdx < mImages.size(); viewIdx++) {
			if (mDoneViews.find(viewIdx) != mDoneViews.end()) {
				continue; //skip done views
			}

			Image2D3DMatch match2D3D;

			//scan all cloud 3D points
			for (const Point3DInMap& cloudPoint : mReconstructionCloud) {
				bool found2DPoint = false;

				//scan all originating views for that 3D point
				for (const auto& origViewAndPoint : cloudPoint.originatingViews) {
					//check for 2D-2D matching via the match matrix
					const int originatingViewIndex = origViewAndPoint.first;
					const int originatingViewFeatureIndex = origViewAndPoint.second;

					//match matrix is upper-triangular (not symmetric) so the left index must be the smaller one
					const int leftViewIdx = (originatingViewIndex < viewIdx) ? originatingViewIndex : viewIdx;
					const int rightViewIdx = (originatingViewIndex < viewIdx) ? viewIdx : originatingViewIndex;

					//scan all 2D-2D matches between originating view and new view
					for (const DMatch& m : mFeatureMatchMatrix[leftViewIdx][rightViewIdx]) {
						int matched2DPointInNewView = -1;
						if (originatingViewIndex < viewIdx) { //originating view is 'left'
							if (m.queryIdx == originatingViewFeatureIndex) {
								matched2DPointInNewView = m.trainIdx;
							}
						} else {                              //originating view is 'right'
							if (m.trainIdx == originatingViewFeatureIndex) {
								matched2DPointInNewView = m.queryIdx;
							}
						}
						if (matched2DPointInNewView >= 0) {
							//This point is matched in the new view
							const Features& newViewFeatures = mImageFeatures[viewIdx];
							match2D3D.points2D.push_back(newViewFeatures.points[matched2DPointInNewView]);
							match2D3D.points3D.push_back(cloudPoint.p);
							found2DPoint = true;
							break;
						}
					}

					if (found2DPoint) {
						break;
					}
				}
			}

			matches[viewIdx] = match2D3D;
		}

		return matches;
	}

	void SfM::mergeNewPointCloud(const PointCloud& cloud) {
		const size_t numImages = mImages.size();
		MatchMatrix mergeMatchMatrix;
		mergeMatchMatrix.resize(numImages, vector<Matching>(numImages));

		size_t newPoints = 0;
		size_t mergedPoints = 0;

		for (const Point3DInMap& p : cloud) {
			const Point3f newPoint = p.p; //new 3D point

			bool foundAnyMatchingExistingViews = false;
			bool foundMatching3DPoint = false;
			for (Point3DInMap& existingPoint : mReconstructionCloud) {
				if (norm(existingPoint.p - newPoint) < MERGE_CLOUD_POINT_MIN_MATCH_DISTANCE) {
					//This point is very close to an existing 3D cloud point
					foundMatching3DPoint = true;

					//Look for common 2D features to confirm match
					for (const auto& newKv : p.originatingViews) {
						//kv.first = new point's originating view
						//kv.second = new point's view 2D feature index

						for (const auto& existingKv : existingPoint.originatingViews) {
							//existingKv.first = existing point's originating view
							//existingKv.second = existing point's view 2D feature index

							bool foundMatchingFeature = false;

							const bool newIsLeft = newKv.first < existingKv.first;
							const int leftViewIdx = (newIsLeft) ? newKv.first : existingKv.first;
							const int leftViewFeatureIdx = (newIsLeft) ? newKv.second : existingKv.second;
							const int rightViewIdx = (newIsLeft) ? existingKv.first : newKv.first;
							const int rightViewFeatureIdx = (newIsLeft) ? existingKv.second : newKv.second;

							const Matching& matching = mFeatureMatchMatrix[leftViewIdx][rightViewIdx];
							for (const DMatch& match : matching) {
								if ((match.queryIdx == leftViewFeatureIdx)
									&& (match.trainIdx == rightViewFeatureIdx)
									&& (match.distance < MERGE_CLOUD_FEATURE_MIN_MATCH_DISTANCE)) {

									mergeMatchMatrix[leftViewIdx][rightViewIdx].push_back(match);

									//Found a 2D feature match for the two 3D points - merge
									foundMatchingFeature = true;
									break;
								}
							}

							if (foundMatchingFeature) {
								//Add the new originating view, and feature index
								existingPoint.originatingViews[newKv.first] = newKv.second;

								foundAnyMatchingExistingViews = true;

							}
						}
					}
				}
				if (foundAnyMatchingExistingViews) {
					mergedPoints++;
					break; //Stop looking for more matching cloud points
				}
			}

			if ((!foundAnyMatchingExistingViews) && (!foundMatching3DPoint)) {
				//This point did not match any existing cloud points - add it as new.
				mReconstructionCloud.push_back(p);
				newPoints++;
			}
		}

		if (mVisualDebugLevel <= LOG_DEBUG) {
			//debug: show new matching points in the cloud
			for (size_t i = 0; i < numImages - 1; i++) {
				for (size_t j = i; j < numImages; j++) {
					const Matching& matching = mergeMatchMatrix[i][j];
					if (matching.empty()) {
						continue;
					}

					Mat outImage;
					drawMatches(mImages[i], mImageFeatures[i].keyPoints,
						mImages[j], mImageFeatures[j].keyPoints,
						matching, outImage);
					//write the images index...
					putText(outImage, "Image " + to_string(i), Point(10, 50), CV_FONT_NORMAL, 3.0, Colors::GREEN, 3);
					putText(outImage, "Image " + to_string(j), Point(10 + outImage.cols / 2, 50), CV_FONT_NORMAL, 3.0, Colors::GREEN, 3);
					resize(outImage, outImage, Size(), 0.25, 0.25);
					imshow("Merge Match", outImage);
					waitKey(0);
				}
			}
			destroyWindow("Merge Match");
		}

		if (mConsoleDebugLevel <= LOG_DEBUG) {
			cout << " adding: " << cloud.size() << " (new: " << newPoints << ", merged: " << mergedPoints << ")" << endl;
		}
	}

	void SfM::saveCloudAndCamerasToPLY(const std::string& prefix) {
		if (mConsoleDebugLevel <= LOG_INFO) {
			cout << "Saving result reconstruction with prefix " << prefix << endl;
		}

		ofstream ofs(prefix + "_points.ply");

		//write PLY header
		ofs << "ply                 " << endl <<
			"format ascii 1.0    " << endl <<
			"element vertex " << mReconstructionCloud.size() << endl <<
			"property float x    " << endl <<
			"property float y    " << endl <<
			"property float z    " << endl <<
			"property uchar red  " << endl <<
			"property uchar green" << endl <<
			"property uchar blue " << endl <<
			"end_header          " << endl;

		for (const Point3DInMap& p : mReconstructionCloud) {
			//get color from first originating view
			auto originatingView = p.originatingViews.begin();
			const int viewIdx = originatingView->first;
			Point2f p2d = mImageFeatures[viewIdx].points[originatingView->second];
			Vec3b pointColor = mImages[viewIdx].at<Vec3b>(p2d);

			//write vertex
			ofs << p.p.x << " " <<
				p.p.y << " " <<
				p.p.z << " " <<
				(int)pointColor(2) << " " <<
				(int)pointColor(1) << " " <<
				(int)pointColor(0) << " " << endl;
		}

		ofs.close();

		ofstream ofsc(prefix + "_cameras.ply");

		//write PLY header
		ofsc << "ply                 " << endl <<
			"format ascii 1.0    " << endl <<
			"element vertex " << (mCameraPoses.size() * 4) << endl <<
			"property float x    " << endl <<
			"property float y    " << endl <<
			"property float z    " << endl <<
			"element edge " << (mCameraPoses.size() * 3) << endl <<
			"property int vertex1" << endl <<
			"property int vertex2" << endl <<
			"property uchar red  " << endl <<
			"property uchar green" << endl <<
			"property uchar blue " << endl <<
			"end_header          " << endl;

		//save cameras polygons..
		for (const auto& pose : mCameraPoses) {
			Point3d c(pose(0, 3), pose(1, 3), pose(2, 3));
			Point3d cx = c + Point3d(pose(0, 0), pose(1, 0), pose(2, 0)) * 0.2;
			Point3d cy = c + Point3d(pose(0, 1), pose(1, 1), pose(2, 1)) * 0.2;
			Point3d cz = c + Point3d(pose(0, 2), pose(1, 2), pose(2, 2)) * 0.2;

			ofsc << c.x << " " << c.y << " " << c.z << endl;
			ofsc << cx.x << " " << cx.y << " " << cx.z << endl;
			ofsc << cy.x << " " << cy.y << " " << cy.z << endl;
			ofsc << cz.x << " " << cz.y << " " << cz.z << endl;
		}

		const int camVertexStartIndex = mReconstructionCloud.size();

		for (size_t i = 0; i < mCameraPoses.size(); i++) {
			ofsc << (i * 4 + 0) << " " <<
				(i * 4 + 1) << " " <<
				"255 0 0" << endl;
			ofsc << (i * 4 + 0) << " " <<
				(i * 4 + 2) << " " <<
				"0 255 0" << endl;
			ofsc << (i * 4 + 0) << " " <<
				(i * 4 + 3) << " " <<
				"0 0 255" << endl;
		}
	}
}