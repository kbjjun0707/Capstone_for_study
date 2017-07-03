#ifndef ALPHA01_H
#define ALPHA01_H

#include <QtWidgets/QMainWindow>

#include "ui_alpha01.h"
#include "ui_camwindow.h"
#include "ui_scanwindow.h"

#include <opencv2\opencv.hpp>
#include <opencvLibs.h>

#include "cameracalibrator.h"
#include "scanner_.h"

#include <thread>
#include <mutex>

class BeginPage;
class LiveCamPage;
class ScanPage;

class Alpha01 : public QMainWindow {
	Q_OBJECT

public:
	Alpha01(QWidget *parent = 0);
	~Alpha01();

	cv::Mat m_CameraMat, m_DistCoeffs;
	iMiner::CameraCalibrator *m_Calibrator;
	iMiner::Scanner *m_Scanner;

	static void mat2QImageRGB888(const cv::Mat &src, QImage &dst);

private:
	BeginPage *ui_start;
	LiveCamPage *ui_liveCam;
	ScanPage *ui_scan;

	///////////////////////////SLOTS//////////////////////////
	public slots:

	// pagejumpFunc
	void calibratorCall();
	void calibratorClose();
	void scanPageCall();
	void scanPageClose();
};



/*======================== PAGES ===========================*/
//------------------------------------------------------------
// StartPage
//------------------------------------------------------------
class BeginPage : public QObject {
	Q_OBJECT
public:
	BeginPage();
	~BeginPage();

	void setUp(Alpha01 *qMainWindow);

private:
	Alpha01 *qMainWin;
	Ui::Alpha01Class *ui;

	cv::VideoCapture m_Cam;
	std::thread m_CamThread;
	bool m_CamRunning;
	QImage m_Frame;
	QPixmap m_PixFrame;

	bool m_LoadedPreference;

	//////////////////////// Function ////////////////////////
	void setText(double *tmps);
	bool openCam(int p_idx = 0);
	void closeCam();


	///////////////////////////SLOTS//////////////////////////
	private slots:
	void quitProgram();
	void startCam();
	void startCalCam();
	void viewCameraMatrix();
};



//------------------------------------------------------------
// LiveCamPage
//------------------------------------------------------------
class LiveCamPage : public QObject {
	Q_OBJECT
public:
	LiveCamPage();
	~LiveCamPage();

	void setUp(Alpha01 *qMainWindow);

private:
	Alpha01 *qMainWin;
	Ui::CamWindow *ui;

	bool m_Capturring;
	cv::VideoCapture m_Cam;
	std::thread m_CapThread;
	std::mutex m_Mutex;
	bool m_GrabFrame;
	int m_CaptureCnt;
	QImage m_Frame;
	QPixmap m_PixFrame;

	//////////////////////// Function ////////////////////////
	bool openCam(int p_idx = 0);
	//void mat2QImageRGB888(const cv::Mat src, QImage &dst);
	void closeCam();

	///////////////////////////SLOTS//////////////////////////
	private slots:
	void calibrateImages();
	void captureFrame();
	void startCam();
	void closePage();
	void selectedItem();
};



//------------------------------------------------------------
// ScanPage
//------------------------------------------------------------
class ScanPage : public QObject {
	Q_OBJECT
public:
	ScanPage();
	~ScanPage();

	void setUp(Alpha01 *QMainWindow);
	void capturing();
	void processing();

private:
	Alpha01 *qMainWin;
	Ui::ScanWindow *ui;

	bool m_Capturring, m_VisiableCam, m_VisiablePnt, m_CapCmp, m_GrabFrame;
	cv::VideoCapture m_Cam;
	std::thread m_CapThread;
	std::mutex m_Mutex;
	QImage m_Frame;
	cv::Mat m_CvFrame;
	QPixmap m_PixFrame, m_PixStdFrame, m_PixComFrame;
	int m_CaptureCnt, m_ProcessDone;
	QDir m_TempFolder;

	//////////////////////// Function ////////////////////////
	bool openCam(int p_idx = 0);
	//void startProcess();
	void closeCam();
	void startCam();
	void captureFrame();
	void clearStorage();
	void chkViewMode();

	///////////////////////////SLOTS//////////////////////////
	private slots:
	void reconfirmMenuChk();
	void captureButton();
	void clearButton();
	void clickList();
};


#endif // ALPHA01_H