#include "boilerefficiencyWindow.h"
#include <QtWidgets/QApplication>
#include "BoilerEffi.h"
//int Boiler::BoilerEffi::bbb = 1;

int main(int argc, char *argv[])
{
	
	QApplication a(argc, argv);
	BoilerEfficiencyWindow w;
	w.show();

	return a.exec();
}
