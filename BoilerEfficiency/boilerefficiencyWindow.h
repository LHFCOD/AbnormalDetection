#ifndef BOILEREFFICIENCY_H
#define BOILEREFFICIENCY_H

#include <QtWidgets/QMainWindow>
#include "ui_boilerefficiency.h"

class BoilerEfficiencyWindow : public QMainWindow
{
	Q_OBJECT

public:
	BoilerEfficiencyWindow(QWidget *parent = 0);
	~BoilerEfficiencyWindow();

private:
	Ui::BoilerEfficiencyClass ui;
};

#endif // BOILEREFFICIENCY_H
