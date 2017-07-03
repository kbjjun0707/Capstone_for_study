#pragma once

#include <opencv2\opencv.hpp>
#include <opencv2\stereo.hpp>
#include <opencv2\ximgproc.hpp>

namespace iMiner {

	class DisparityMaker {
		cv::Ptr<cv::StereoSGBM>		sgbm;	// Left -> Right
		double						downscale;
		
		// param
		int							sgbmWinSize;
		int							numberOfDisparities, minDisparites;
		int							cn;
		double						lambda;
		double						sigma;
		//double						vis_multi;
		
		// filter
		cv::Ptr<cv::ximgproc::DisparityWLSFilter>	wls_filter;
		cv::Ptr<cv::StereoMatcher>					sm;		// Right -> Left

		cv::Mat imgDisparity16S;
		cv::Mat imgDisparity16Sr;
		cv::Mat imgFilteredDisparity16S;

	public:
		/*
		* @param downscale 0.0< param, param <= 1.0
		*/
		DisparityMaker(double downscale = 1.0);
		~DisparityMaker();

		void setting(int preFilter = 63, int winblockSize = 5, 
			int numberOfDisparities = (16 * 1), int mindisparites = 0,
			int channel = 3, double lambda = 8000.0, double sigma =1.5, int method = cv::StereoSGBM::MODE_SGBM_3WAY);

		void compute(const cv::Mat &pLeft, const cv::Mat &pRight);

		void filter(const cv::Mat &pLeft);

		cv::Mat getVisDisparity();

		cv::Mat getVisFiltedDisparity();
	};


	typedef DisparityMaker DethMaker;
}