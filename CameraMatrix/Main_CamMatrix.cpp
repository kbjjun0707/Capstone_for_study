// openCV
#include <opencv2\opencv.hpp>
#include <opencvInit.h>

// std
#include <iostream>
#include <vector>

using namespace std;

namespace iMiner {
	// std::vector
	// opencv
	class CameraCalibrator {
		std::vector<std::vector<cv::Point3f>> objectPoints;
		std::vector<std::vector<cv::Point2f>> imagePoints;
		int flag;
		cv::Mat map1, map2;
		bool mustInitUndistort;
		int CellSize;
	public:
		cv::Mat cameraMatrix;
		cv::Mat distCoeffs;
		CameraCalibrator(int chessBoardCellSizemm) :flag(0), mustInitUndistort(true) {
			this->CellSize = chessBoardCellSizemm;
		};
		int addChessboardPoints(const std::vector<std::string>& filelist, cv::Size &boardSize) {
			std::vector<cv::Point2f> imageCorners;
			std::vector<cv::Point3f> objectCorners;
			for (int i = 0; i < boardSize.height; i++)
				for (int j = 0; j < boardSize.width; j++)
					objectCorners.push_back(cv::Point3f(i, j, 0.0f));

			cv::Mat image;
			int successes = 0;
			for (int i = 0; i < filelist.size(); i++) {
				image = cv::imread(filelist[i], 0);
				bool found = cv::findChessboardCorners(image, boardSize, imageCorners);
				if(found) {
					cv::cornerSubPix(image, imageCorners, cv::Size(CellSize, CellSize), cv::Size(-1, -1),
						cv::TermCriteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 30, 0.001));
				}
				if (imageCorners.size() == boardSize.area()) {
					imagePoints.push_back(imageCorners);
					objectPoints.push_back(objectCorners);
					//cv::imwrite(cv::format("E:/workspace/calibrationTest/img%02d.jpg", successes), image);
					successes++;
				}
			}
			return successes;
		}
		int addChessboardPoints(const std::vector<cv::Mat>& imagelist, cv::Size &boardSize) {
			std::vector<cv::Point2f> imageCorners;
			std::vector<cv::Point3f> objectCorners;
			for (int i = 0; i < boardSize.height; i++)
				for (int j = 0; j < boardSize.width; j++)
					objectCorners.push_back(cv::Point3f((float)i, (float)j, 0.0f));

			cv::Mat image;
			int successes = 0;
			for (int i = 0; i < imagelist.size(); i++) {
				image = imagelist[i];
				cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
				bool found = cv::findChessboardCorners(image, boardSize, imageCorners);
				cv::cornerSubPix(image, imageCorners, cv::Size(CellSize, CellSize), cv::Size(-1, -1),
					cv::TermCriteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 30, 0.1));
				if (imageCorners.size() == boardSize.area()) {
					imagePoints.push_back(imageCorners);
					objectPoints.push_back(objectCorners);
					//cv::imwrite(cv::format("E:/workspace/calibrationTest/img%02d.jpg", successes), image);
					successes++;
				}
			}
			return successes;
		}

		double calibrate(cv::Size &imageSize) {
			mustInitUndistort = true;
			std::vector<cv::Mat> rvecs, tvecs;

			return cv::calibrateCamera(
				objectPoints, imagePoints, imageSize,
				cameraMatrix, distCoeffs,
				rvecs, tvecs, flag);
		}
		cv::Mat remap(const cv::Mat &image) {
			cv::Mat remapMat;
			if (mustInitUndistort) {
				cv::initUndistortRectifyMap(
					cameraMatrix, distCoeffs,
					cv::Mat(), cv::Mat(),
					image.size(), CV_32FC1,
					map1, map2);
				mustInitUndistort = false;
			}
			cv::remap(image, remapMat, map1, map2, cv::INTER_LINEAR);
			return remapMat;
		}
		void outputFile(const char* fileName) {
			ofstream output(fileName);
			output << "# CameraMatrix 3X3, distCoeffs 1X5" << endl << endl;
			output << cameraMatrix << endl << endl
				<< distCoeffs << endl << endl;

			output.close();
		}
		cv::Mat undistort(const cv::Mat &image) {
			cv::Mat undistortMat;
			cv::undistort(image, undistortMat, this->cameraMatrix, this->distCoeffs);
			return undistortMat;
		}
	};
}

void onMouse(int evt, int x, int y, int flags, void *param);

char *g_CamWin = "Camera";
char *g_ImgsWin = "SubWindow";
char *g_testWin = "TestWindow";
char *g_CParamsOut = "E:/workspace/CameraParams.txt";
cv::Mat g_Frame, g_SubFrame, g_ReSzFrame;
cv::Size g_Resize(640 * 4 / 5, 480 * 4 / 5);

vector<cv::Mat> g_ImgsStorage;
int g_ImgsIdx, g_ImgsStorageSz;

int main() {
	int m_CornerHorNum = 7, m_CornerVerNum = 4, m_CellSize = 26;      // inside corners count
	//cout << "Enter the number of inside corners along width : ";
	//cin >> m_CornerHorNum;
	//cout << "Enter the number of inside corners along height : ";
	//cin >> m_CornerVerNum;
	//cout << "Enter the size of Cell : ";
	//cin >> m_CellSize;

	int m_NumSquares = m_CornerHorNum*m_CornerVerNum;
	cv::Size m_BoardSz = cv::Size(m_CornerHorNum, m_CornerVerNum);

	cv::VideoCapture m_Cam(0);

	if (!m_Cam.isOpened()) {
		cout << "Camera Opend faild" << endl;
		return 0;
	}

	m_Cam >> g_Frame;
	cout << "Cam Size : " << g_Frame.size() << endl;
	cv::resize(g_Frame, g_ReSzFrame, g_Resize);

	cv::namedWindow(g_CamWin);
	cv::moveWindow(g_CamWin, 0, 10);
	cv::imshow(g_CamWin, g_ReSzFrame);
	cv::namedWindow(g_ImgsWin);
	cv::moveWindow(g_ImgsWin, g_Resize.width, 10);
	cv::destroyWindow(g_ImgsWin);
	cv::namedWindow(g_testWin);
	cv::moveWindow(g_testWin, g_Resize.width, 10 + g_Resize.height);
	cv::destroyWindow(g_testWin);

	cv::setMouseCallback(g_CamWin, onMouse, &g_Frame);

	bool m_Capturing = true, m_Calibration = false;
	while (m_Capturing) {
		m_Cam >> g_Frame;
		cv::resize(g_Frame, g_ReSzFrame, g_Resize);
		cv::imshow(g_CamWin, g_ReSzFrame);

		switch (cv::waitKey(40)) {
		case 2424832:      // left
			if (g_ImgsIdx > 0) {
				cv::resize(g_ImgsStorage[--g_ImgsIdx], g_SubFrame, g_Resize);
				cv::imshow(g_ImgsWin, g_SubFrame);
			}
			break;
		case 2555904:      // right
			if (g_ImgsIdx < (g_ImgsStorageSz - 1)) {
				cv::resize(g_ImgsStorage[++g_ImgsIdx], g_SubFrame, g_Resize);
				cv::imshow(g_ImgsWin, g_SubFrame);
			}
			break;
		case 2490368:      // up
			break;
		case 2621440:      // down
			break;
		case 'r':
		case 'R':
			g_ImgsStorage.clear();
			g_ImgsIdx = g_ImgsStorageSz = 0;
			break;
		case 'd':
		case 'D':
			if (g_ImgsStorageSz <= 0) break;
			else if (g_ImgsStorageSz <= 1) {
				g_ImgsStorage.clear();
				g_ImgsIdx = 0;
				cv::destroyWindow(g_ImgsWin);
			} else if (g_ImgsIdx == 0) {
				vector<cv::Mat>::iterator iter = g_ImgsStorage.begin() + g_ImgsIdx;
				g_ImgsStorage.erase(iter);
				cv::resize(g_ImgsStorage[g_ImgsIdx], g_SubFrame, g_Resize);
				cv::imshow(g_ImgsWin, g_SubFrame);
			} else {
				vector<cv::Mat>::iterator iter = g_ImgsStorage.begin() + (g_ImgsIdx--);
				g_ImgsStorage.erase(iter);
				cv::resize(g_ImgsStorage[g_ImgsIdx], g_SubFrame, g_Resize);
				cv::imshow(g_ImgsWin, g_SubFrame);
			}
			g_ImgsStorageSz--;
			break;
		case 13:                     // Enter key
			if (g_ImgsStorageSz == 0) break;
			m_Calibration = true;
		case 27:
			cv::destroyWindow(g_CamWin);
			m_Capturing = false;
			break;
		default:
			break;
		}
	}

	if (m_Calibration) {
		iMiner::CameraCalibrator l_Calibrator(m_CellSize);
		std::vector<std::string> test;
		test.push_back("E:/Library/Document/Coding Projects/Visual Studio 2015/Projects/CapstoneAlpha/AlphaTest/captures/img00.jpg");
		test.push_back("E:/Library/Document/Coding Projects/Visual Studio 2015/Projects/CapstoneAlpha/AlphaTest/captures/img01.jpg");
		test.push_back("E:/Library/Document/Coding Projects/Visual Studio 2015/Projects/CapstoneAlpha/AlphaTest/captures/img02.jpg");
		test.push_back("E:/Library/Document/Coding Projects/Visual Studio 2015/Projects/CapstoneAlpha/AlphaTest/captures/img03.jpg");
		l_Calibrator.addChessboardPoints(test, m_BoardSz);
		l_Calibrator.calibrate(g_Frame.size());
		l_Calibrator.outputFile(g_CParamsOut);
		while (1) {
			m_Cam >> g_Frame;
			cv::resize(g_Frame, g_ReSzFrame, g_Resize);
			cv::imshow(g_CamWin, g_ReSzFrame);

			cv::Mat imageUndistorted = l_Calibrator.remap(g_Frame);
			cv::resize(imageUndistorted, g_ReSzFrame, g_Resize);
			cv::imshow(g_ImgsWin, g_ReSzFrame);

			cv::Mat test;
			//cv::undistort(g_Frame, test, l_Calibrator.cameraMatrix, l_Calibrator.distCoeffs);
			test = l_Calibrator.undistort(g_Frame);
			cv::resize(test, g_ReSzFrame, g_Resize);
			cv::imshow(g_testWin, g_ReSzFrame);

			if (cv::waitKey(40) == 27)break;
		}
	}
	m_Cam.release();

	return 0;
}

void onMouse(int evt, int x, int y, int flags, void *param) {
	cv::Mat tmpImg = *(cv::Mat *)param;

	switch (evt) {
	case::CV_EVENT_FLAG_LBUTTON:
		g_ImgsStorage.push_back(tmpImg.clone());
		g_ImgsStorageSz = g_ImgsStorage.size();
		g_ImgsIdx = g_ImgsStorageSz - 1;
		cv::resize(g_ImgsStorage[g_ImgsIdx], g_SubFrame, g_Resize);
		cv::imshow(g_ImgsWin, g_SubFrame);
		break;
	default:
		break;
	}
}