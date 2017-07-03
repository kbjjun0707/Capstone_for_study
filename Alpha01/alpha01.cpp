#include "alpha01.h"
#include <qmessagebox.h>
#include <QtWidgets\qlistwidget.h>

#include <fstream>

//------------------------------------------------------------
// MainWindow
//------------------------------------------------------------
Alpha01::Alpha01(QWidget *parent)
	: QMainWindow(parent) {
	move(QPoint(10, 10));
	m_Calibrator = nullptr;
	m_Scanner = nullptr;

	ui_start = nullptr;
	ui_liveCam = nullptr;
	ui_scan = nullptr;

	ui_start = new BeginPage();
	ui_start->setUp(this);

}

Alpha01::~Alpha01() {
	if (nullptr != m_Calibrator) delete m_Calibrator;
	if (nullptr != m_Scanner) delete m_Scanner;

	if (nullptr != ui_start) delete ui_start;
	if (nullptr != ui_liveCam) delete ui_liveCam;
	if (nullptr != ui_scan) delete ui_scan;
}

void Alpha01::mat2QImageRGB888(const cv::Mat &src, QImage &dst) {
	dst = QImage(src.data, src.cols, src.rows, QImage::Format::Format_RGB888);
}

void Alpha01::calibratorCall() {
	if (ui_liveCam == nullptr) {
		ui_liveCam = new LiveCamPage();
	}
	ui_liveCam->setUp(this);

	delete ui_start;
	ui_start = nullptr;
}

void Alpha01::calibratorClose() {
	if (ui_start == nullptr) {
		ui_start = new BeginPage();
	}
	ui_start->setUp(this);

	delete ui_liveCam;
	ui_liveCam = nullptr;
}

void Alpha01::scanPageCall() {
	if (ui_scan == nullptr) {
		ui_scan = new ScanPage();
	}
	ui_scan->setUp(this);

	delete ui_start;
	ui_start = nullptr;
}

void Alpha01::scanPageClose() {
	if (ui_start == nullptr) {
		ui_start = new BeginPage();
	}
	ui_start->setUp(this);

	delete ui_scan;
	ui_scan = nullptr;
}



//------------------------------------------------------------
// StartPage
//------------------------------------------------------------
BeginPage::BeginPage() {
	ui = new Ui::Alpha01Class();
	m_LoadedPreference = false;
	m_CamRunning = false;
}

BeginPage::~BeginPage() {
	if (m_Cam.isOpened()) closeCam();

	delete ui;
	ui = nullptr;
}

void BeginPage::setUp(Alpha01 *qMainWindow) {
	this->qMainWin = qMainWindow;
	ui->setupUi(qMainWindow);

	// page init
	{
		double tmps[9] = { 0 };
		std::ifstream input("preference.txt");
		if (input.is_open() && !input.eof()) {
			for (int i = 0; i < 10 && !input.eof(); i++)
				input >> tmps[i];
			setText(tmps);
			m_LoadedPreference = true;
		}
		if (m_LoadedPreference) {
			if (qMainWin->m_Calibrator == nullptr) {
				std::ifstream input("settings.txt");
				int cellsize, hor, ver;
				input >> hor >> ver >> cellsize;
				input.close();
				qMainWin->m_Calibrator = new iMiner::CameraCalibrator(cellsize);
			}
			qMainWin->m_Calibrator->setCameraMat(tmps);
			qMainWin->m_Calibrator->setDistCoeffs(tmps);
		}
		input.close();
	}

	// menu event
	connect(ui->menuQuit, &QAction::triggered, this, &BeginPage::quitProgram);
	connect(ui->menuLiveCam, &QAction::triggered, qMainWindow, &Alpha01::calibratorCall);
	connect(ui->menuViewCM, &QAction::triggered, this, &BeginPage::viewCameraMatrix);
	connect(ui->menuScan, &QAction::triggered, qMainWindow, &Alpha01::scanPageCall);

	//ui event
	connect(ui->btnOpenCam1, &QPushButton::clicked, this, &BeginPage::startCam);
	connect(ui->btnOpenCam2, &QPushButton::clicked, this, &BeginPage::startCalCam);
	connect(ui->btnCloseCam, &QPushButton::clicked, this, &BeginPage::closeCam);


	ui->btnCloseCam->setEnabled(false);
	if (!m_LoadedPreference)
		ui->btnOpenCam2->setEnabled(false);
	else
		ui->btnOpenCam2->setEnabled(true);
}

void BeginPage::setText(double *tmps) {
	ui->label_fx->setText(QString::number(tmps[0]));
	ui->label_fy->setText(QString::number(tmps[1]));
	ui->label_cx->setText(QString::number(tmps[2]));
	ui->label_cy->setText(QString::number(tmps[3]));
	ui->label_k1->setText(QString::number(tmps[4]));
	ui->label_k2->setText(QString::number(tmps[5]));
	ui->label_p1->setText(QString::number(tmps[6]));
	ui->label_p2->setText(QString::number(tmps[7]));
	double cameraData[] = {
		tmps[0], 0, tmps[2],
		0, tmps[1], tmps[3],
		0, 0, 1
	};
	cv::Mat(3, 3, CV_64F, cameraData).copyTo(qMainWin-> m_CameraMat);
	double coeffsData[] = { tmps[4], tmps[5], tmps[6], tmps[7], tmps[8] };
	cv::Mat(1, 5, CV_64F, coeffsData).copyTo(qMainWin->m_DistCoeffs);
}

bool BeginPage::openCam(int p_idx) {
	if (m_Cam.isOpened() || m_Cam.open(p_idx))
		return true;
	return false;
}

void BeginPage::closeCam() {
	m_CamRunning = false;
	if (m_CamThread.joinable())
		m_CamThread.join();
	if (m_Cam.isOpened()) {
		m_Cam.release();
		ui->viewLabel->setPixmap(QPixmap(0, 0));
	}
	ui->btnOpenCam1->setEnabled(true);
	if (!m_LoadedPreference)
		ui->btnOpenCam2->setEnabled(false);
	else
		ui->btnOpenCam2->setEnabled(true);
	ui->btnCloseCam->setEnabled(false);
}

void BeginPage::startCam() {
	closeCam();
	if (openCam()) {
		m_CamRunning = true;
		ui->btnOpenCam1->setEnabled(false);
		m_CamThread = std::thread([&]() {
			cv::Mat cvFrame;
			while (m_CamRunning) {
				m_Cam >> cvFrame;
				cv::cvtColor(cvFrame, cvFrame, cv::COLOR_BGR2RGB);
				Alpha01::mat2QImageRGB888(cvFrame, m_Frame);
				m_PixFrame = QPixmap::fromImage(m_Frame);
				ui->viewLabel->setPixmap(m_PixFrame);
			}
		});
		ui->btnCloseCam->setEnabled(true);
	}
}

void BeginPage::startCalCam() {
	closeCam();
	if (openCam()) {
		m_CamRunning = true;
		ui->btnOpenCam2->setEnabled(false);
		m_CamThread = std::thread([&]() {
			cv::Mat cvFrame;
			while (m_CamRunning) {
				m_Cam >> cvFrame;
				cv::Mat undistortMat;
				qMainWin->m_Calibrator->undistort(cvFrame, undistortMat);
				cv::cvtColor(undistortMat, undistortMat, cv::COLOR_BGR2RGB);
				Alpha01::mat2QImageRGB888(undistortMat, m_Frame);
				m_PixFrame = QPixmap::fromImage(m_Frame);
				ui->viewLabel->setPixmap(m_PixFrame);
				cv::waitKey(40);
			}
		});
		ui->btnCloseCam->setEnabled(true);
	}

}

void BeginPage::viewCameraMatrix() {
	if (ui->menuViewCM->isChecked()) {
		ui->groupBox->setVisible(true);
	} else {
		ui->groupBox->setVisible(false);
	}
}

void BeginPage::quitProgram() {
	exit(0);
}



//------------------------------------------------------------
// LiveCamPage
//------------------------------------------------------------
LiveCamPage::LiveCamPage() {
	ui = new Ui::CamWindow();
	m_GrabFrame = false;
	m_CaptureCnt = 0;
}

LiveCamPage::~LiveCamPage() {
	if (m_Cam.isOpened()) closeCam();

	delete ui;
	ui = nullptr;
}

void LiveCamPage::setUp(Alpha01 *qMainWindow) {
	this->qMainWin = qMainWindow;
	ui->setupUi(qMainWindow);
	ui->btnCap->setEnabled(false);
	ui->btnCali->setEnabled(false);

	connect(ui->btnClose, SIGNAL(clicked()), this, SLOT(closePage()));
	connect(ui->btnStartCam, SIGNAL(clicked()), this, SLOT(startCam()));
	connect(ui->btnCap, SIGNAL(clicked()), this, SLOT(captureFrame()));
	connect(ui->btnCali, SIGNAL(clicked()), this, SLOT(calibrateImages()));
	connect(ui->listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectedItem()));

}

bool LiveCamPage::openCam(int p_idx) {
	if (m_Cam.isOpened() || m_Cam.open(p_idx))
		return true;
	return false;
}

void LiveCamPage::startCam() {
	if (m_Capturring = openCam()) {
		ui->btnStartCam->setEnabled(false);
		ui->listWidget->setEnabled(false);
		m_CapThread = std::thread([&]() {
			cv::Mat cvFrame;
			while (m_Capturring) {
				m_Cam >> cvFrame;
				m_Mutex.lock(); {
					cv::cvtColor(cvFrame, cvFrame, cv::COLOR_BGR2RGB);
					Alpha01::mat2QImageRGB888(cvFrame, m_Frame);
					m_PixFrame = QPixmap::fromImage(m_Frame);
					ui->label->setPixmap(m_PixFrame);
				} m_Mutex.unlock();
				cv::waitKey(40);
			}
		});
		ui->btnCap->setEnabled(true);

		return;
	}
	QMessageBox::warning(qMainWin, "Error", "No Camera!");
	ui->btnStartCam->setEnabled(true);
	return;
}

void LiveCamPage::closePage() {
	closeCam();
	qMainWin->calibratorClose();
}

void LiveCamPage::selectedItem() {
	QList<QListWidgetItem *> tmp = ui->listWidget->selectedItems();
	QImage screenImg("captures/" + tmp[0]->text());
	ui->label->setPixmap(QPixmap::fromImage(screenImg));
}

void LiveCamPage::calibrateImages() {
	closeCam();
	ui->label->setText("계산 중...");
	std::ifstream input("settings.txt");
	int cellsize, hor, ver;
	input >> hor >> ver >> cellsize;
	input.close();

	if (nullptr == qMainWin->m_Calibrator)
		qMainWin->m_Calibrator = new iMiner::CameraCalibrator(cellsize);
	std::vector<std::string> imageNames;
	for (int i = 0; i < m_CaptureCnt; i++) {
		imageNames.push_back("captures/" + ui->listWidget->item(i)->text().toStdString());
	}
	int rst = qMainWin->m_Calibrator->addChessboardPoints(imageNames, cv::Size(hor, ver));
	qMainWin->m_Calibrator->calibrate();
	qMainWin->m_Calibrator->outputFile("preference.txt");

	if (rst > 0)	ui->listWidget->setItemSelected(ui->listWidget->item(0), true);
	else ui->label->setText("사진 없음.");

	ui->listWidget->setEnabled(true);
}

void LiveCamPage::captureFrame() {
	if (m_Capturring) {
		m_Mutex.lock(); {
			m_PixFrame.save(cv::format("captures/img%02d.jpg", m_CaptureCnt++).c_str());
		} m_Mutex.unlock();
		ui->listWidget->addItem(cv::format("img%02d.jpg", m_CaptureCnt - 1).c_str());
		ui->btnCali->setEnabled(true);
	}
}

void LiveCamPage::closeCam() {
	m_Capturring = false;
	if (m_CapThread.joinable())
		m_CapThread.join();
	if (m_Cam.isOpened()) {
		m_Cam.release();
		ui->label->setPixmap(QPixmap(0, 0));
		ui->btnStartCam->setEnabled(true);
		ui->btnCap->setEnabled(false);
		ui->btnCali->setEnabled(false);
	}
}



//------------------------------------------------------------
// ScanPage
//------------------------------------------------------------
ScanPage::ScanPage() {
	ui = new Ui::ScanWindow();
	m_GrabFrame = false;
	m_CapCmp = false;
	m_Capturring = false;
	m_CaptureCnt = 0;
	
	m_TempFolder.setPath(m_TempFolder.currentPath() + "/Captmp");
}

ScanPage::~ScanPage() {
	m_TempFolder.removeRecursively();

	closeCam();

	delete ui;
	ui = nullptr;
}

void ScanPage::setUp(Alpha01 * qMainWindow) {
	this->qMainWin = qMainWindow;
	ui->setupUi(qMainWindow);

	if (qMainWindow->m_Scanner == nullptr)
		qMainWindow->m_Scanner = new iMiner::Scanner(qMainWin->m_CameraMat, qMainWin->m_DistCoeffs);
	ui->openGLWidget->setQMain(qMainWindow);
	qMainWindow->m_Scanner->runThread();

	// menu event
	connect(ui->menuQuit_scanwindow, &QAction::triggered, qMainWin, &Alpha01::scanPageClose);
	connect(ui->menu3D_Points, &QAction::triggered, this, &ScanPage::reconfirmMenuChk);
	connect(ui->menuCamera_Models, &QAction::triggered, this, &ScanPage::reconfirmMenuChk);
	connect(ui->listWidget, &QListWidget::itemSelectionChanged, this, &ScanPage::clickList);

	//ui event
	connect(ui->btnClose, &QPushButton::clicked, qMainWin, &Alpha01::scanPageClose);
	connect(ui->btnCapture, &QPushButton::clicked, this, &ScanPage::captureButton);
	connect(ui->btnClear, &QPushButton::clicked, this, &ScanPage::clearButton);

	ui->btnClear->setEnabled(false);
	ui->btnCapture->setEnabled(false);

	if (!m_Capturring) startCam();
	if (!m_TempFolder.exists()) {
		m_TempFolder.mkpath(m_TempFolder.path());
	}
}

void ScanPage::capturing() {
	float imgX = ui->lbVideo->width(), imgY = ui->lbVideo->height();
	cv::Mat tmpImg;
	while (m_Capturring) {
		if (!m_GrabFrame) {
			m_Cam >> m_CvFrame;
			cv::resize(m_CvFrame, tmpImg, cv::Size(imgX, imgY));
			cv::cvtColor(tmpImg, tmpImg, cv::COLOR_BGR2RGB);
			Alpha01::mat2QImageRGB888(tmpImg, m_Frame);
			m_PixFrame = QPixmap::fromImage(m_Frame);
			ui->lbVideo->setPixmap(m_PixFrame);
		}
		cv::waitKey(40);
	}
	
}

bool ScanPage::openCam(int p_idx) {
	if (m_Cam.isOpened() || m_Cam.open(p_idx))
		return true;
	return false;
}

void ScanPage::closeCam() {
	ui->btnCapture->setEnabled(false);
	m_Capturring = false;
	if (m_CapThread.joinable())
		m_CapThread.join();
	if (m_Cam.isOpened())
		m_Cam.release();
}

void ScanPage::startCam() {
	if (m_Capturring = openCam()) {
		ui->btnCapture->setEnabled(true);
		
		m_CapThread = std::thread([&]() {
			capturing();
		});

		return;
	}
	QMessageBox::warning(qMainWin, "Error", "No Camera!");

	return;
}

void ScanPage::captureFrame() {
	m_GrabFrame = true;
	if (m_CapCmp) {		// 기준 있을 때
		this->m_PixComFrame = this->m_PixFrame;
		ui->lbCompare->setPixmap(this->m_PixComFrame);
	} 
	else {
		ui->btnClear->setEnabled(true);
		this->m_PixStdFrame = this->m_PixFrame;
		ui->lbStandard->setPixmap(this->m_PixStdFrame);
		m_CapCmp = true;
	}
	std::string tmpFileName(cv::format("%s/imgs%03d.jpg", m_TempFolder.path().toStdString().c_str(), m_CaptureCnt++).c_str());
	cv::imwrite(tmpFileName, m_CvFrame);
	QListWidgetItem *qwItem = new QListWidgetItem();
	qwItem->setText(QString::fromStdString(tmpFileName));
	ui->listWidget->addItem(qwItem);
	qMainWin->m_Scanner->addImg(tmpFileName.c_str());
	m_GrabFrame = false;
}

void ScanPage::clearStorage() {
	ui->btnClear->setEnabled(false);
	ui->lbStandard->setPixmap(QPixmap(0, 0));
	ui->lbCompare->setPixmap(QPixmap(0, 0));
	ui->listWidget->clear();
	qMainWin->m_Scanner->clearStorage();
}

void ScanPage::chkViewMode() {
	m_VisiableCam = ui->menuCamera_Models->isChecked();
	m_VisiablePnt = ui->menu3D_Points->isChecked();
}

void ScanPage::captureButton() {
	if ((!m_GrabFrame) && (m_Capturring)) {
		captureFrame();
	}
	return;
}

void ScanPage::clearButton() {
	m_CapCmp = false;
	m_CaptureCnt = 0;
	clearStorage();
}

void ScanPage::clickList() {
	QList<QListWidgetItem *> tmp = ui->listWidget->selectedItems();
	QListWidgetItem *item = tmp[0];

}

void ScanPage::reconfirmMenuChk() {
	chkViewMode();
}
