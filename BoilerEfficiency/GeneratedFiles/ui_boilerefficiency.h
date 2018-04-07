/********************************************************************************
** Form generated from reading UI file 'boilerefficiency.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BOILEREFFICIENCY_H
#define UI_BOILEREFFICIENCY_H

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

class Ui_BoilerEfficiencyClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *BoilerEfficiencyClass)
    {
        if (BoilerEfficiencyClass->objectName().isEmpty())
            BoilerEfficiencyClass->setObjectName(QStringLiteral("BoilerEfficiencyClass"));
        BoilerEfficiencyClass->resize(600, 400);
        menuBar = new QMenuBar(BoilerEfficiencyClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        BoilerEfficiencyClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(BoilerEfficiencyClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        BoilerEfficiencyClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(BoilerEfficiencyClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        BoilerEfficiencyClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(BoilerEfficiencyClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        BoilerEfficiencyClass->setStatusBar(statusBar);

        retranslateUi(BoilerEfficiencyClass);

        QMetaObject::connectSlotsByName(BoilerEfficiencyClass);
    } // setupUi

    void retranslateUi(QMainWindow *BoilerEfficiencyClass)
    {
        BoilerEfficiencyClass->setWindowTitle(QApplication::translate("BoilerEfficiencyClass", "BoilerEfficiency", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class BoilerEfficiencyClass: public Ui_BoilerEfficiencyClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BOILEREFFICIENCY_H
