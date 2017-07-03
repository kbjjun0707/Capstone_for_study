#pragma once

#include <opencv2\opencv.hpp>
#include <opencvLibs.h>

#include <fstream>
#include <vector>

namespace iMiner {
	// std::vector
	// std::fstream
	// opencv
	class CameraCalibrator {
		std::vector<std::vector<cv::Point3f>> objectPoints;
		std::vector<std::vector<cv::Point2f>> imagePoints;
		cv::Mat cameraMatrix;
		cv::Mat distCoeffs;
		int flag;
		cv::Mat map1, map2;
		cv::Size imgSize;
		bool mustInitUndistort;
		int CellSize;
	public:
		CameraCalibrator(int chessBoardCellSizemm) :flag(0), mustInitUndistort(true) {
			this->CellSize = chessBoardCellSizemm;
		};

		int addChessboardPoints(const std::vector<std::string>& filelist, cv::Size &boardSize) {
			std::vector<cv::Point2f> imageCorners;
			std::vector<cv::Point3f> objectCorners;
			for (int i = 0; i < boardSize.height; i++)
				for (int j = 0; j < boardSize.width; j++)
					objectCorners.push_back(cv::Point3f(i*CellSize, j*CellSize, 0.0f));

			cv::Mat image, grayImg;
			int successes = 0;
			for (int i = 0; i < filelist.size(); i++) {
				image = cv::imread(filelist[i]);
				cv::cvtColor(image, grayImg, cv::COLOR_BGR2GRAY);
				bool found = cv::findChessboardCorners(grayImg, boardSize, imageCorners);
				if (found) {
					cv::cornerSubPix(grayImg, imageCorners, cv::Size(5, 5), cv::Size(-1, -1),
						cv::TermCriteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 30, 0.001));
					cv::drawChessboardCorners(image, boardSize, imageCorners, found);
				}
				if (imageCorners.size() == boardSize.area()) {
					imagePoints.push_back(imageCorners);
					objectPoints.push_back(objectCorners);
					cv::imwrite(filelist[i], image);
					successes++;
				}
			}
			this->imgSize = image.size();
			
			return successes;
		}

		int addChessboardPoints(const std::vector<char *>& filelist, cv::Size &boardSize) {
			std::vector<cv::Point2f> imageCorners;
			std::vector<cv::Point3f> objectCorners;
			for (int i = 0; i < boardSize.height; i++)
				for (int j = 0; j < boardSize.width; j++)
					objectCorners.push_back(cv::Point3f(i*CellSize, j*CellSize, 0.0f));

			cv::Mat image, grayImg;
			int successes = 0;
			for (int i = 0; i < filelist.size(); i++) {
				image = cv::imread(filelist[i]);
				cv::cvtColor(image, grayImg, cv::COLOR_BGR2GRAY);
				bool found = cv::findChessboardCorners(grayImg, boardSize, imageCorners);
				if (found) {
					cv::cornerSubPix(grayImg, imageCorners, cv::Size(5, 5), cv::Size(-1, -1),
						cv::TermCriteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 30, 0.001));
					cv::drawChessboardCorners(image, boardSize, imageCorners, found);
				}
				if (imageCorners.size() == boardSize.area()) {
					imagePoints.push_back(imageCorners);
					objectPoints.push_back(objectCorners);
					cv::imwrite(filelist[i], image);
					successes++;
				}
			}
			this->imgSize = image.size();

			return successes;
		}

		int addChessboardPoints(const std::vector<cv::Mat>& imagelist, cv::Size &boardSize) {
			std::vector<cv::Point2f> imageCorners;
			std::vector<cv::Point3f> objectCorners;
			for (int i = 0; i < boardSize.height; i++)
				for (int j = 0; j < boardSize.width; j++)
					objectCorners.push_back(cv::Point3f(i*CellSize, j*CellSize, 0.0f));

			cv::Mat image, grayImg;
			int successes = 0;
			for (int i = 0; i < imagelist.size(); i++) {
				image = imagelist[i];
				cv::cvtColor(image, grayImg, cv::COLOR_BGR2GRAY);
				bool found = cv::findChessboardCorners(grayImg, boardSize, imageCorners);
				if (found) {
					cv::cornerSubPix(grayImg, imageCorners, cv::Size(5, 5), cv::Size(-1, -1),
						cv::TermCriteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 30, 0.001));
					cv::drawChessboardCorners(image, boardSize, imageCorners, found);
				}
				if (imageCorners.size() == boardSize.area()) {
					imagePoints.push_back(imageCorners);
					objectPoints.push_back(objectCorners);
					cv::imwrite(cv::format("captures/img%02d.jpg", i), image);
					successes++;
				}
			}
			return successes;
		}

		double calibrate() {
			mustInitUndistort = true;
			std::vector<cv::Mat> rvecs, tvecs;

			return cv::calibrateCamera(
				objectPoints, imagePoints, this->imgSize,
				cameraMatrix, distCoeffs,
				rvecs, tvecs, flag);
		}

		double calibrate(cv::Size &imageSize) {
			mustInitUndistort = true;
			std::vector<cv::Mat> rvecs, tvecs;

			return cv::calibrateCamera(
				objectPoints, imagePoints, imageSize,
				cameraMatrix, distCoeffs,
				rvecs, tvecs, flag);
		}

		//cv::Mat remap(const cv::Mat &image) {
		//	cv::Mat undistorted;
		//	if (mustInitUndistort) {
		//		cv::initUndistortRectifyMap(
		//			cameraMatrix, distCoeffs,
		//			cv::Mat(), cv::Mat(),
		//			image.size(), CV_32FC1,
		//			map1, map2);
		//		mustInitUndistort = false;
		//	}
		//	cv::remap(image, undistorted, map1, map2, cv::INTER_LINEAR);
		//	return undistorted;
		//}

		bool isCalibrate() {
			return mustInitUndistort;
		}

		void outputFile(const char* fileName) {
			std::ofstream output(fileName);
			//output << "# CameraMatrix 3X3, distCoeffs 5X1" << std::endl << std::endl;
			//output << cameraMatrix << std::endl << std::endl
			//	<< distCoeffs << std::endl << std::endl;
			output << cameraMatrix.at<double>(0, 0) << " ";
			output << cameraMatrix.at<double>(1, 1) << " ";
			output << cameraMatrix.at<double>(0, 2) << " ";
			output << cameraMatrix.at<double>(1, 2) << " ";

			output << distCoeffs.at<double>(0, 0) << " ";
			output << distCoeffs.at<double>(0, 1) << " ";
			output << distCoeffs.at<double>(0, 2) << " ";
			output << distCoeffs.at<double>(0, 3) << " ";
			output << distCoeffs.at<double>(0, 4) << " ";

			output << std::endl << std::endl;
			output << cameraMatrix << std::endl << std::endl
				<< distCoeffs << std::endl << std::endl;

			output << imgSize << std::endl;

			output.close();
		}

		void undistort(const cv::Mat &src, cv::Mat &dst) {
			cv::undistort(src, dst, this->cameraMatrix, this->distCoeffs);
		}

		void setCameraMat(const cv::Mat &src) {
			src.copyTo(this->cameraMatrix);
		}

		void setCameraMat(const double *src) {
			double cameraData[] = {
				src[0], 0, src[2],
				0, src[1], src[3],
				0, 0, 1
			};
			cv::Mat(3, 3, CV_64F, cameraData).copyTo(this->cameraMatrix);
		}

		void setDistCoeffs(const cv::Mat &src) {
			src.copyTo(this->distCoeffs);
		}

		void setDistCoeffs(const double *src) {
			double coeffsData[] = { src[4], src[5], src[6], src[7], src[8] };
			cv::Mat(1, 5, CV_64F, coeffsData).copyTo(this->distCoeffs);
		}
	};
}
