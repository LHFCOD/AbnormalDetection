#ifndef QTABNORMALDETECTION_H
#define QTABNORMALDETECTION_H

#include <QtWidgets/QMainWindow>
#include "ui_qtabnormaldetection.h"

class QtAbnormalDetection : public QMainWindow
{
	Q_OBJECT

public:
	QtAbnormalDetection(QWidget *parent = 0);
	~QtAbnormalDetection();

private:
	Ui::QtAbnormalDetectionClass ui;
};

#endif // QTABNORMALDETECTION_H
