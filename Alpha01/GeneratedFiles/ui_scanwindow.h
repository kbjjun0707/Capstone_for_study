/********************************************************************************
** Form generated from reading UI file 'scanwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCANWINDOW_H
#define UI_SCANWINDOW_H

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
#include <glrender.h>

QT_BEGIN_NAMESPACE

class Ui_ScanWindow
{
public:
    QAction *menuQuit_scanwindow;
    QAction *menuCamera_Models;
    QAction *menu3D_Points;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout_3;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *lbVideo;
    QLabel *lbStandard;
    QLabel *lbCompare;
    QSpacerItem *horizontalSpacer;
    GlRender *openGLWidget;
    QFrame *line_2;
    QVBoxLayout *verticalLayout_2;
    QPushButton *btnClear;
    QListWidget *listWidget;
    QSpacerItem *verticalSpacer;
    QPushButton *btnClose;
    QFrame *line;
    QPushButton *btnCapture;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuView;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ScanWindow)
    {
        if (ScanWindow->objectName().isEmpty())
            ScanWindow->setObjectName(QStringLiteral("ScanWindow"));
        ScanWindow->setWindowModality(Qt::ApplicationModal);
        ScanWindow->resize(601, 551);
        menuQuit_scanwindow = new QAction(ScanWindow);
        menuQuit_scanwindow->setObjectName(QStringLiteral("menuQuit_scanwindow"));
        menuCamera_Models = new QAction(ScanWindow);
        menuCamera_Models->setObjectName(QStringLiteral("menuCamera_Models"));
        menuCamera_Models->setCheckable(true);
        menuCamera_Models->setChecked(true);
        menu3D_Points = new QAction(ScanWindow);
        menu3D_Points->setObjectName(QStringLiteral("menu3D_Points"));
        menu3D_Points->setCheckable(true);
        centralwidget = new QWidget(ScanWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        horizontalLayout_3 = new QHBoxLayout(centralwidget);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        lbVideo = new QLabel(centralwidget);
        lbVideo->setObjectName(QStringLiteral("lbVideo"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lbVideo->sizePolicy().hasHeightForWidth());
        lbVideo->setSizePolicy(sizePolicy);
        lbVideo->setMinimumSize(QSize(160, 120));
        lbVideo->setMaximumSize(QSize(160, 120));
        lbVideo->setFrameShape(QFrame::Box);
        lbVideo->setScaledContents(false);

        horizontalLayout->addWidget(lbVideo);

        lbStandard = new QLabel(centralwidget);
        lbStandard->setObjectName(QStringLiteral("lbStandard"));
        sizePolicy.setHeightForWidth(lbStandard->sizePolicy().hasHeightForWidth());
        lbStandard->setSizePolicy(sizePolicy);
        lbStandard->setMinimumSize(QSize(160, 120));
        lbStandard->setMaximumSize(QSize(160, 120));
        lbStandard->setFrameShape(QFrame::Box);
        lbStandard->setScaledContents(false);

        horizontalLayout->addWidget(lbStandard);

        lbCompare = new QLabel(centralwidget);
        lbCompare->setObjectName(QStringLiteral("lbCompare"));
        sizePolicy.setHeightForWidth(lbCompare->sizePolicy().hasHeightForWidth());
        lbCompare->setSizePolicy(sizePolicy);
        lbCompare->setMinimumSize(QSize(160, 120));
        lbCompare->setMaximumSize(QSize(160, 120));
        lbCompare->setFrameShape(QFrame::Box);
        lbCompare->setScaledContents(false);

        horizontalLayout->addWidget(lbCompare);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        horizontalLayout->setStretch(3, 1);

        verticalLayout->addLayout(horizontalLayout);

        openGLWidget = new GlRender(centralwidget);
        openGLWidget->setObjectName(QStringLiteral("openGLWidget"));
        openGLWidget->setMinimumSize(QSize(480, 360));

        verticalLayout->addWidget(openGLWidget);

        verticalLayout->setStretch(1, 1);

        horizontalLayout_2->addLayout(verticalLayout);

        line_2 = new QFrame(centralwidget);
        line_2->setObjectName(QStringLiteral("line_2"));
        line_2->setFrameShape(QFrame::VLine);
        line_2->setFrameShadow(QFrame::Sunken);

        horizontalLayout_2->addWidget(line_2);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        btnClear = new QPushButton(centralwidget);
        btnClear->setObjectName(QStringLiteral("btnClear"));
        btnClear->setMinimumSize(QSize(80, 0));

        verticalLayout_2->addWidget(btnClear);

        listWidget = new QListWidget(centralwidget);
        listWidget->setObjectName(QStringLiteral("listWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(listWidget->sizePolicy().hasHeightForWidth());
        listWidget->setSizePolicy(sizePolicy1);
        listWidget->setMinimumSize(QSize(80, 320));

        verticalLayout_2->addWidget(listWidget);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        btnClose = new QPushButton(centralwidget);
        btnClose->setObjectName(QStringLiteral("btnClose"));
        btnClose->setMinimumSize(QSize(80, 0));

        verticalLayout_2->addWidget(btnClose);

        line = new QFrame(centralwidget);
        line->setObjectName(QStringLiteral("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout_2->addWidget(line);

        btnCapture = new QPushButton(centralwidget);
        btnCapture->setObjectName(QStringLiteral("btnCapture"));
        sizePolicy.setHeightForWidth(btnCapture->sizePolicy().hasHeightForWidth());
        btnCapture->setSizePolicy(sizePolicy);
        btnCapture->setMinimumSize(QSize(80, 80));
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        btnCapture->setFont(font);

        verticalLayout_2->addWidget(btnCapture);

        verticalLayout_2->setStretch(1, 1);

        horizontalLayout_2->addLayout(verticalLayout_2);

        horizontalLayout_2->setStretch(0, 1);

        horizontalLayout_3->addLayout(horizontalLayout_2);

        ScanWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ScanWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 601, 21));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuView = new QMenu(menubar);
        menuView->setObjectName(QStringLiteral("menuView"));
        ScanWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(ScanWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        ScanWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuView->menuAction());
        menuFile->addSeparator();
        menuFile->addAction(menuQuit_scanwindow);
        menuView->addAction(menuCamera_Models);
        menuView->addAction(menu3D_Points);

        retranslateUi(ScanWindow);

        QMetaObject::connectSlotsByName(ScanWindow);
    } // setupUi

    void retranslateUi(QMainWindow *ScanWindow)
    {
        ScanWindow->setWindowTitle(QApplication::translate("ScanWindow", "3D Scanner Project", Q_NULLPTR));
        menuQuit_scanwindow->setText(QApplication::translate("ScanWindow", "Quit", Q_NULLPTR));
        menuCamera_Models->setText(QApplication::translate("ScanWindow", "Camera Models", Q_NULLPTR));
        menu3D_Points->setText(QApplication::translate("ScanWindow", "3D Points", Q_NULLPTR));
        lbVideo->setText(QString());
        lbStandard->setText(QString());
        lbCompare->setText(QString());
        btnClear->setText(QApplication::translate("ScanWindow", "Clear", Q_NULLPTR));
        btnClose->setText(QApplication::translate("ScanWindow", "Close", Q_NULLPTR));
        btnCapture->setText(QApplication::translate("ScanWindow", "Capture", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("ScanWindow", "File", Q_NULLPTR));
        menuView->setTitle(QApplication::translate("ScanWindow", "View", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ScanWindow: public Ui_ScanWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCANWINDOW_H
