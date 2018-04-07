#include "boilerefficiencyWindow.h"
#include "qdebug.h"
#include "BoilerEffi.h"
#include "ParseText.h"

#include <iostream>
#include <typeinfo>
#include <qmessagebox.h>
using namespace Boiler;
BoilerEfficiencyWindow::BoilerEfficiencyWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	try
	{
		QFile file(QString::fromLocal8Bit("气体比定压热容.txt"));
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			int i=1;
		ReceivedBasis basis = { 63.25,3.4,11.17,0.64,0.5,7.04,14.00 };
		InputArg input;
		input.basis = basis;
		input.m_aerfa = 1.35;
        input.m_file = file.readAll();
        input.tpy = 124;//98.5,101,105.5,114,124
		input.t0 = 20;
		Boiler::BoilerEffi effi(input);	
		double aerfa = 1;
        QString fileName="result"+QString::number(effi.m_input.tpy,'g',6)+".txt";
        QFile outfile(QString::fromLocal8Bit("导数1.35.txt"));
		if (!outfile.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			QMessageBox::information(this, QString::fromLocal8Bit("异常"), QString::fromLocal8Bit("文件写错误！"), NULL);
			return;
		}
		QTextStream out(&outfile);

        QString outStr = QString::fromLocal8Bit("理论空气量：") + QString::number(effi.m_smokeLoss->V0,'g',6) + "\n";
        outStr += QString::fromLocal8Bit("环境温度：") + QString::number(effi.m_input.t0,'g',6) +"\n";
        outStr += QString::fromLocal8Bit("排烟温度：") +QString::number( effi.m_input.tpy,'g',6) + "\n";
		out << outStr;
		out << "Aerfa" << "\t" << "q2" << "\t" << "q4" << "\t"<<"dq2"<<"\t"<<"dq4"<<"\n";
		double _q2 = effi.m_smokeLoss->q2;
		double _q4 = effi.m_solidLoss->q4;
		double dq2 = 0;
		double dq4 = 0;
		double daerfa = 0.01;
        if(false)
		while (aerfa < 2)
		{
			effi.m_input.m_aerfa = aerfa;
			effi.UpdateBoilerEffi();
			double q2 = effi.m_smokeLoss->q2;
			double q4 = effi.m_solidLoss->q4;
			dq2 = (q2 - _q2) / daerfa;
			dq4 = (q4 - _q4) / daerfa;
			out << aerfa << "\t" << q2 << "\t" << q4 << "\t"<<dq2<<"\t"<<dq4<<"\n";
			out.flush();
			aerfa += daerfa;
			_q2 = q2;
			_q4 = q4;
		}
        else
        {
            daerfa=1;
            while(daerfa>1e-8)
            {
                double temp_aerfa=1.35+daerfa;
            effi.m_input.m_aerfa = temp_aerfa;
            effi.UpdateBoilerEffi();
            double q2 = effi.m_smokeLoss->q2;
            double q4 = effi.m_solidLoss->q4;
            dq2 = (q2 - _q2) / daerfa;
            dq4 = (q4 - _q4) / daerfa;
            out << QString::number(temp_aerfa,'g',12) << "\t" << q2 << "\t" << q4 << "\t"<<dq2<<"\t"<<dq4<<"\n";
            out.flush();
            aerfa += daerfa;
            daerfa/=2;
            }
        }
		outfile.close();
		QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("计算完成！"), NULL);
	}
	catch (IExceptionBase* ex)
	{
		//Exception *_ex = static_cast<Exception*>(ex);
		QString str = ex->ShowError();
		QMessageBox::information(this, QString::fromLocal8Bit("异常"), str, NULL);
		delete ex;
	}
	
}

BoilerEfficiencyWindow::~BoilerEfficiencyWindow()
{

}
