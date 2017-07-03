/********************************************************************************
** Form generated from reading UI file 'camwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CAMWINDOW_H
#define UI_CAMWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CamWindow
{
public:
    QAction *menuQuit;
    QAction *actionOption;
    QAction *menuQuit_camwindow;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout_3;
    QHBoxLayout *horizontalLayout_2;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_3;
    QFrame *frame;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QFrame *line;
    QVBoxLayout *verticalLayout;
    QPushButton *btnStartCam;
    QFrame *line_2;
    QPushButton *btnCap;
    QPushButton *btnCali;
    QFrame *line_4;
    QListWidget *listWidget;
    QSpacerItem *verticalSpacer;
    QFrame *line_3;
    QPushButton *btnClose;
    QStatusBar *statusbar;
    QMenuBar *menuBar;
    QMenu *menuTool;
    QMenu *menuFile;

    void setupUi(QMainWindow *CamWindow)
    {
        if (CamWindow->objectName().isEmpty())
            CamWindow->setObjectName(QStringLiteral("CamWindow"));
        CamWindow->resize(452, 372);
        menuQuit = new QAction(CamWindow);
        menuQuit->setObjectName(QStringLiteral("menuQuit"));
        actionOption = new QAction(CamWindow);
        actionOption->setObjectName(QStringLiteral("actionOption"));
        menuQuit_camwindow = new QAction(CamWindow);
        menuQuit_camwindow->setObjectName(QStringLiteral("menuQuit_camwindow"));
        centralwidget = new QWidget(CamWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        horizontalLayout_3 = new QHBoxLayout(centralwidget);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        frame = new QFrame(centralwidget);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setCursor(QCursor(Qt::CrossCursor));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        frame->setLineWidth(2);
        verticalLayout_2 = new QVBoxLayout(frame);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        label->setCursor(QCursor(Qt::CrossCursor));
        label->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        verticalLayout_2->addWidget(label);


        verticalLayout_3->addWidget(frame);


        horizontalLayout->addLayout(verticalLayout_3);


        horizontalLayout_2->addLayout(horizontalLayout);

        line = new QFrame(centralwidget);
        line->setObjectName(QStringLiteral("line"));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);

        horizontalLayout_2->addWidget(line);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(8);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        btnStartCam = new QPushButton(centralwidget);
        btnStartCam->setObjectName(QStringLiteral("btnStartCam"));

        verticalLayout->addWidget(btnStartCam);

        line_2 = new QFrame(centralwidget);
        line_2->setObjectName(QStringLiteral("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_2);

        btnCap = new QPushButton(centralwidget);
        btnCap->setObjectName(QStringLiteral("btnCap"));

        verticalLayout->addWidget(btnCap);

        btnCali = new QPushButton(centralwidget);
        btnCali->setObjectName(QStringLiteral("btnCali"));

        verticalLayout->addWidget(btnCali);

        line_4 = new QFrame(centralwidget);
        line_4->setObjectName(QStringLiteral("line_4"));
        line_4->setFrameShape(QFrame::HLine);
        line_4->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_4);

        listWidget = new QListWidget(centralwidget);
        listWidget->setObjectName(QStringLiteral("listWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(listWidget->sizePolicy().hasHeightForWidth());
        listWidget->setSizePolicy(sizePolicy);
        listWidget->setMaximumSize(QSize(100, 16777215));

        verticalLayout->addWidget(listWidget);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        line_3 = new QFrame(centralwidget);
        line_3->setObjectName(QStringLiteral("line_3"));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_3);

        btnClose = new QPushButton(centralwidget);
        btnClose->setObjectName(QStringLiteral("btnClose"));

        verticalLayout->addWidget(btnClose);


        horizontalLayout_2->addLayout(verticalLayout);

        horizontalLayout_2->setStretch(0, 1);

        horizontalLayout_3->addLayout(horizontalLayout_2);

        CamWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(CamWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        CamWindow->setStatusBar(statusbar);
        menuBar = new QMenuBar(CamWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 452, 21));
        menuTool = new QMenu(menuBar);
        menuTool->setObjectName(QStringLiteral("menuTool"));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        CamWindow->setMenuBar(menuBar);
        QWidget::setTabOrder(btnCap, btnClose);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuTool->menuAction());
        menuTool->addSeparator();
        menuTool->addAction(actionOption);
        menuFile->addSeparator();
        menuFile->addAction(menuQuit_camwindow);

        retranslateUi(CamWindow);

        QMetaObject::connectSlotsByName(CamWindow);
    } // setupUi

    void retranslateUi(QMainWindow *CamWindow)
    {
        CamWindow->setWindowTitle(QApplication::translate("CamWindow", "3D Scanner Project", Q_NULLPTR));
        menuQuit->setText(QApplication::translate("CamWindow", "Quit", Q_NULLPTR));
        actionOption->setText(QApplication::translate("CamWindow", "Option", Q_NULLPTR));
        menuQuit_camwindow->setText(QApplication::translate("CamWindow", "Quit", Q_NULLPTR));
        label->setText(QString());
        btnStartCam->setText(QApplication::translate("CamWindow", "Start Cam", Q_NULLPTR));
        btnCap->setText(QApplication::translate("CamWindow", "Capture", Q_NULLPTR));
        btnCali->setText(QApplication::translate("CamWindow", "Calibrate", Q_NULLPTR));
        btnClose->setText(QApplication::translate("CamWindow", "Close", Q_NULLPTR));
        menuTool->setTitle(QApplication::translate("CamWindow", "Tool", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("CamWindow", "File", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class CamWindow: public Ui_CamWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CAMWINDOW_H
