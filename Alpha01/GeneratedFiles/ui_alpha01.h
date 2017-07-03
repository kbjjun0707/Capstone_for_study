/********************************************************************************
** Form generated from reading UI file 'alpha01.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ALPHA01_H
#define UI_ALPHA01_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Alpha01Class
{
public:
    QAction *menuLiveCam;
    QAction *menuLoadImages;
    QAction *menuQuit;
    QAction *menuViewCM;
    QAction *menuScan;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout_13;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout_11;
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLabel *label_fx;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_3;
    QLabel *label_fy;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_5;
    QLabel *label_cx;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_7;
    QLabel *label_cy;
    QSpacerItem *horizontalSpacer_4;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_9;
    QLabel *label_k1;
    QSpacerItem *horizontalSpacer_5;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_15;
    QLabel *label_k2;
    QSpacerItem *horizontalSpacer_8;
    QHBoxLayout *horizontalLayout_10;
    QLabel *label_19;
    QLabel *label_p1;
    QSpacerItem *horizontalSpacer_10;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_17;
    QLabel *label_p2;
    QSpacerItem *horizontalSpacer_9;
    QSpacerItem *verticalSpacer_2;
    QSpacerItem *verticalSpacer_3;
    QFrame *line;
    QHBoxLayout *horizontalLayout_12;
    QLabel *viewLabel;
    QFrame *line_3;
    QVBoxLayout *verticalLayout_2;
    QPushButton *btnOpenCam1;
    QPushButton *btnOpenCam2;
    QFrame *line_2;
    QPushButton *btnCloseCam;
    QSpacerItem *verticalSpacer_4;
    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *menuCalibrate;
    QMenu *menu_2;
    QMenu *menuView;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *Alpha01Class)
    {
        if (Alpha01Class->objectName().isEmpty())
            Alpha01Class->setObjectName(QStringLiteral("Alpha01Class"));
        Alpha01Class->setWindowModality(Qt::ApplicationModal);
        Alpha01Class->resize(400, 449);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Alpha01Class->sizePolicy().hasHeightForWidth());
        Alpha01Class->setSizePolicy(sizePolicy);
        menuLiveCam = new QAction(Alpha01Class);
        menuLiveCam->setObjectName(QStringLiteral("menuLiveCam"));
        menuLoadImages = new QAction(Alpha01Class);
        menuLoadImages->setObjectName(QStringLiteral("menuLoadImages"));
        menuQuit = new QAction(Alpha01Class);
        menuQuit->setObjectName(QStringLiteral("menuQuit"));
        menuViewCM = new QAction(Alpha01Class);
        menuViewCM->setObjectName(QStringLiteral("menuViewCM"));
        menuViewCM->setCheckable(true);
        menuViewCM->setChecked(true);
        menuScan = new QAction(Alpha01Class);
        menuScan->setObjectName(QStringLiteral("menuScan"));
        centralWidget = new QWidget(Alpha01Class);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout_13 = new QHBoxLayout(centralWidget);
        horizontalLayout_13->setSpacing(6);
        horizontalLayout_13->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_13->setObjectName(QStringLiteral("horizontalLayout_13"));
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setCheckable(false);
        horizontalLayout_11 = new QHBoxLayout(groupBox);
        horizontalLayout_11->setSpacing(6);
        horizontalLayout_11->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        label_fx = new QLabel(groupBox);
        label_fx->setObjectName(QStringLiteral("label_fx"));

        horizontalLayout->addWidget(label_fx);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_2->addWidget(label_3);

        label_fy = new QLabel(groupBox);
        label_fy->setObjectName(QStringLiteral("label_fy"));

        horizontalLayout_2->addWidget(label_fy);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QStringLiteral("label_5"));

        horizontalLayout_3->addWidget(label_5);

        label_cx = new QLabel(groupBox);
        label_cx->setObjectName(QStringLiteral("label_cx"));

        horizontalLayout_3->addWidget(label_cx);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_7 = new QLabel(groupBox);
        label_7->setObjectName(QStringLiteral("label_7"));

        horizontalLayout_4->addWidget(label_7);

        label_cy = new QLabel(groupBox);
        label_cy->setObjectName(QStringLiteral("label_cy"));

        horizontalLayout_4->addWidget(label_cy);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_4);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label_9 = new QLabel(groupBox);
        label_9->setObjectName(QStringLiteral("label_9"));

        horizontalLayout_5->addWidget(label_9);

        label_k1 = new QLabel(groupBox);
        label_k1->setObjectName(QStringLiteral("label_k1"));

        horizontalLayout_5->addWidget(label_k1);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_5);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        label_15 = new QLabel(groupBox);
        label_15->setObjectName(QStringLiteral("label_15"));

        horizontalLayout_8->addWidget(label_15);

        label_k2 = new QLabel(groupBox);
        label_k2->setObjectName(QStringLiteral("label_k2"));

        horizontalLayout_8->addWidget(label_k2);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_8);


        verticalLayout->addLayout(horizontalLayout_8);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setSpacing(6);
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        label_19 = new QLabel(groupBox);
        label_19->setObjectName(QStringLiteral("label_19"));

        horizontalLayout_10->addWidget(label_19);

        label_p1 = new QLabel(groupBox);
        label_p1->setObjectName(QStringLiteral("label_p1"));

        horizontalLayout_10->addWidget(label_p1);

        horizontalSpacer_10 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_10);


        verticalLayout->addLayout(horizontalLayout_10);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        label_17 = new QLabel(groupBox);
        label_17->setObjectName(QStringLiteral("label_17"));

        horizontalLayout_9->addWidget(label_17);

        label_p2 = new QLabel(groupBox);
        label_p2->setObjectName(QStringLiteral("label_p2"));

        horizontalLayout_9->addWidget(label_p2);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_9);


        verticalLayout->addLayout(horizontalLayout_9);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_3);


        horizontalLayout_11->addLayout(verticalLayout);


        horizontalLayout_13->addWidget(groupBox);

        line = new QFrame(centralWidget);
        line->setObjectName(QStringLiteral("line"));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);

        horizontalLayout_13->addWidget(line);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setSpacing(6);
        horizontalLayout_12->setObjectName(QStringLiteral("horizontalLayout_12"));
        viewLabel = new QLabel(centralWidget);
        viewLabel->setObjectName(QStringLiteral("viewLabel"));

        horizontalLayout_12->addWidget(viewLabel);

        line_3 = new QFrame(centralWidget);
        line_3->setObjectName(QStringLiteral("line_3"));
        line_3->setFrameShape(QFrame::VLine);
        line_3->setFrameShadow(QFrame::Sunken);

        horizontalLayout_12->addWidget(line_3);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        btnOpenCam1 = new QPushButton(centralWidget);
        btnOpenCam1->setObjectName(QStringLiteral("btnOpenCam1"));

        verticalLayout_2->addWidget(btnOpenCam1);

        btnOpenCam2 = new QPushButton(centralWidget);
        btnOpenCam2->setObjectName(QStringLiteral("btnOpenCam2"));

        verticalLayout_2->addWidget(btnOpenCam2);

        line_2 = new QFrame(centralWidget);
        line_2->setObjectName(QStringLiteral("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        verticalLayout_2->addWidget(line_2);

        btnCloseCam = new QPushButton(centralWidget);
        btnCloseCam->setObjectName(QStringLiteral("btnCloseCam"));

        verticalLayout_2->addWidget(btnCloseCam);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_4);


        horizontalLayout_12->addLayout(verticalLayout_2);

        horizontalLayout_12->setStretch(0, 1);

        horizontalLayout_13->addLayout(horizontalLayout_12);

        horizontalLayout_13->setStretch(2, 1);
        Alpha01Class->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(Alpha01Class);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 400, 21));
        menu = new QMenu(menuBar);
        menu->setObjectName(QStringLiteral("menu"));
        menuCalibrate = new QMenu(menu);
        menuCalibrate->setObjectName(QStringLiteral("menuCalibrate"));
        menu_2 = new QMenu(menuBar);
        menu_2->setObjectName(QStringLiteral("menu_2"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QStringLiteral("menuView"));
        Alpha01Class->setMenuBar(menuBar);
        statusBar = new QStatusBar(Alpha01Class);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        Alpha01Class->setStatusBar(statusBar);

        menuBar->addAction(menu->menuAction());
        menuBar->addAction(menu_2->menuAction());
        menuBar->addAction(menuView->menuAction());
        menu->addAction(menuCalibrate->menuAction());
        menu->addSeparator();
        menu->addAction(menuQuit);
        menuCalibrate->addAction(menuLiveCam);
        menuCalibrate->addAction(menuLoadImages);
        menu_2->addAction(menuScan);
        menuView->addAction(menuViewCM);

        retranslateUi(Alpha01Class);

        QMetaObject::connectSlotsByName(Alpha01Class);
    } // setupUi

    void retranslateUi(QMainWindow *Alpha01Class)
    {
        Alpha01Class->setWindowTitle(QApplication::translate("Alpha01Class", "3D Scanner Project", Q_NULLPTR));
        menuLiveCam->setText(QApplication::translate("Alpha01Class", "Live Cam", Q_NULLPTR));
        menuLoadImages->setText(QApplication::translate("Alpha01Class", "Load saved images", Q_NULLPTR));
        menuQuit->setText(QApplication::translate("Alpha01Class", "Quit", Q_NULLPTR));
        menuViewCM->setText(QApplication::translate("Alpha01Class", "Camera Matrix", Q_NULLPTR));
        menuScan->setText(QApplication::translate("Alpha01Class", "Scan", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("Alpha01Class", "Camera Matrix", Q_NULLPTR));
        label->setText(QApplication::translate("Alpha01Class", "fx : ", Q_NULLPTR));
        label_fx->setText(QApplication::translate("Alpha01Class", "0.0", Q_NULLPTR));
        label_3->setText(QApplication::translate("Alpha01Class", "fy : ", Q_NULLPTR));
        label_fy->setText(QApplication::translate("Alpha01Class", "0.0", Q_NULLPTR));
        label_5->setText(QApplication::translate("Alpha01Class", "cx : ", Q_NULLPTR));
        label_cx->setText(QApplication::translate("Alpha01Class", "0.0", Q_NULLPTR));
        label_7->setText(QApplication::translate("Alpha01Class", "cy : ", Q_NULLPTR));
        label_cy->setText(QApplication::translate("Alpha01Class", "0.0", Q_NULLPTR));
        label_9->setText(QApplication::translate("Alpha01Class", "k1 : ", Q_NULLPTR));
        label_k1->setText(QApplication::translate("Alpha01Class", "0.0", Q_NULLPTR));
        label_15->setText(QApplication::translate("Alpha01Class", "k2 : ", Q_NULLPTR));
        label_k2->setText(QApplication::translate("Alpha01Class", "0.0", Q_NULLPTR));
        label_19->setText(QApplication::translate("Alpha01Class", "p1 : ", Q_NULLPTR));
        label_p1->setText(QApplication::translate("Alpha01Class", "0.0", Q_NULLPTR));
        label_17->setText(QApplication::translate("Alpha01Class", "p2 : ", Q_NULLPTR));
        label_p2->setText(QApplication::translate("Alpha01Class", "0.0", Q_NULLPTR));
        viewLabel->setText(QString());
        btnOpenCam1->setText(QApplication::translate("Alpha01Class", "Original", Q_NULLPTR));
        btnOpenCam2->setText(QApplication::translate("Alpha01Class", "Calibrated", Q_NULLPTR));
        btnCloseCam->setText(QApplication::translate("Alpha01Class", "Close", Q_NULLPTR));
        menu->setTitle(QApplication::translate("Alpha01Class", "File", Q_NULLPTR));
        menuCalibrate->setTitle(QApplication::translate("Alpha01Class", "Camera Calibrate", Q_NULLPTR));
        menu_2->setTitle(QApplication::translate("Alpha01Class", "Tool", Q_NULLPTR));
        menuView->setTitle(QApplication::translate("Alpha01Class", "View", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Alpha01Class: public Ui_Alpha01Class {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ALPHA01_H
