/********************************************************************************
** Form generated from reading UI file 'qtabnormaldetection.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTABNORMALDETECTION_H
#define UI_QTABNORMALDETECTION_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtAbnormalDetectionClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QtAbnormalDetectionClass)
    {
        if (QtAbnormalDetectionClass->objectName().isEmpty())
            QtAbnormalDetectionClass->setObjectName(QStringLiteral("QtAbnormalDetectionClass"));
        QtAbnormalDetectionClass->resize(600, 400);
        menuBar = new QMenuBar(QtAbnormalDetectionClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        QtAbnormalDetectionClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QtAbnormalDetectionClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        QtAbnormalDetectionClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(QtAbnormalDetectionClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        QtAbnormalDetectionClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(QtAbnormalDetectionClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        QtAbnormalDetectionClass->setStatusBar(statusBar);

        retranslateUi(QtAbnormalDetectionClass);

        QMetaObject::connectSlotsByName(QtAbnormalDetectionClass);
    } // setupUi

    void retranslateUi(QMainWindow *QtAbnormalDetectionClass)
    {
        QtAbnormalDetectionClass->setWindowTitle(QApplication::translate("QtAbnormalDetectionClass", "QtAbnormalDetection", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class QtAbnormalDetectionClass: public Ui_QtAbnormalDetectionClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTABNORMALDETECTION_H
