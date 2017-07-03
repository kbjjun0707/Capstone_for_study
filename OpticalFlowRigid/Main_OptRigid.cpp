#include <opencv2\opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <OpenCVInit.h>

using namespace cv;
using namespace std;


class Tracker {
	vector<Point2f> trackedFeatures;
	Mat             m_PrevGray;
public:
	bool            m_isAffine;
	Mat_<float>     m_RigidTransform;

	Tracker() :m_isAffine(true) {
		m_RigidTransform = Mat::eye(3, 3, CV_32FC1); //affine 2x3 in a 3x3 matrix
	}

	void processImage(Mat& img) {
		Mat gray;
		cvtColor(img, gray, CV_BGR2GRAY);
		vector<Point2f> corners;
		goodFeaturesToTrack(gray, corners, 300, 0.1, 10);
		cout << "found " << corners.size() << " features\n";

		if (!m_PrevGray.empty()) {
			vector<uchar> status; vector<float> errors;
			calcOpticalFlowPyrLK(m_PrevGray, gray, trackedFeatures, corners, status, errors);

			if (countNonZero(status) < status.size() * 0.7) {
				cout << "cataclysmic error \n";
				m_RigidTransform = Mat::eye(3, 3, CV_32FC1);
				trackedFeatures.clear();
				m_PrevGray.release();
				m_isAffine = true;
				return;
			} else
				m_isAffine = false;

			Mat_<float> newRigidTransform = estimateRigidTransform(trackedFeatures, corners, false);
			Mat_<float> nrt33 = Mat_<float>::eye(3, 3);
			newRigidTransform.copyTo(nrt33.rowRange(0, 2));
			m_RigidTransform *= nrt33;

			trackedFeatures.clear();
			for (int i = 0; i < status.size(); ++i) {
				if (status[i]) {
					trackedFeatures.push_back(corners[i]);
				}
			}
		}

		if (m_isAffine) {
			for (int i = 0; i < corners.size(); ++i) {
				trackedFeatures.push_back(corners[i]);
			}
		}

		for (int i = 0; i < trackedFeatures.size(); ++i) {
			circle(img, trackedFeatures[i], 3, Scalar(0, 0, 255), CV_FILLED);
		}

		gray.copyTo(m_PrevGray);
	}
};


int main() {
	VideoCapture vc(0);


	Mat frame, orig, orig_warped, tmp;

	Tracker tracker;

	if (!vc.isOpened())
		return 0;
	vc >> frame;
	resize(frame, frame, Size(480, 360));
	frame.copyTo(orig);
	tracker.processImage(orig);
	while (vc.isOpened()) {
		vc >> frame;
		if (frame.empty()) break;
		resize(frame, frame, Size(480, 360));

		frame.copyTo(orig);

		tracker.processImage(orig);

		Mat invTrans = tracker.m_RigidTransform.inv(DECOMP_SVD);

		//warpAffine(orig, orig_warped, invTrans.rowRange(0, 2), Size());
		warpPerspective(orig, orig_warped, invTrans, Size());

		imshow("orig", orig_warped);

		if (waitKey(20) == 27) break;
	}
	vc.release();

}