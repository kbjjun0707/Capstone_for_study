#include <opencv2\opencv.hpp>
#include <OpenCVInit.h>
#include <iostream>
#include <fstream>
#include <stdint.h>

using namespace cv;
using namespace std;

int main(int argc, char **args) {
	int no, count;
	char fName[100][1000] = { 0 };
	
	std::ifstream input;
	input.open("filename.txt");
	input >> count;

	for (int i = 0; i < count; i++) {
		input >> fName[i];
		std::cout << i << " : " << fName[i] << endl;
	}
	input.close();

	std::ofstream output;
	output.open("test.txt");

	Mat img[100];
	output << count << endl;
	for (int i = 0; i < count; i++) {
		img[i] = imread(fName[i], CV_8UC1);
		std::cout << img[i].size() << std::endl;
		output << format("test%d.raw", i + 1).c_str() << endl;
		output << img[i].size().width << " " << img[i].size().height << std::endl;
	}

	output.close();

	for (int k = 0; k < count; k++) {
		output.open(format("test%d.raw",k+1).c_str(), std::ofstream::binary);

		for (int i = 0; i < img[k].size().height; i++)
			for (int j = 0; j < img[k].size().width; j++)
				output << img[k].at<uchar>(i, j);
		output.close();
	}
	
	for (int i = 0; i < count; i++) {
		namedWindow(format("test%d.raw", i + 1).c_str(), CV_WINDOW_AUTOSIZE);
		imshow(format("test%d.raw", i + 1).c_str(), img[i]);
	}
	waitKey(0);
	cvDestroyWindow("wTitle");

	return 0;
}