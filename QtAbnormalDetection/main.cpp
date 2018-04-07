#include "qtabnormaldetection.h"
#include <QtWidgets/QApplication>
#include <QSqlDriver>
#include<QSqlDatabase>
#include<QDebug>
#include<qfile.h>
#include <qtextcodec.h>
#include <qtextstream.h>
#include "svm.h"
#include <string.h>
#include<iostream>
#include<time.h>
#include<cmath>
#include<qmessagebox.h>
#include<qlibrary.h>

#include<QSqlQuery>
#include<QSqlField>
#include<QSqlError>
#include<QSqlTableModel>
#include<QSqlDatabase>
#include<QSqlDriver>
#include<QSqlRecord>
using namespace std;

svm_parameter param;
#define PI 3.1415926
void init_param() {
	param.svm_type = ONE_CLASS;
	param.kernel_type = RBF;
	param.degree = 3;
	param.gamma = 0.000005;
	param.coef0 = 0;
	//param.nu = 0.5;
	param.cache_size = 100;
	param.C = 0;
	param.eps = 1e-5;
	param.shrinking = 1;
	param.probability = 0;
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;
	param.nu = 0.001;////

}
int main(int argc, char *argv[])
{
	
	QApplication a(argc, argv);
	//QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
	QtAbnormalDetection w;
	w.show(); 
	enum MyEnum
	{
		aaa = 1,
		b,
	};
	int a1 = MyEnum();
	QTextCodec *code = QTextCodec::codecForName("GBK");//本人采用的是UTF8格式显示的
	char* bb = "你好";
	wchar_t *aa = L"你好";
	qDebug() << aa ;
	int len = wcslen(aa);
	qDebug() << "Available drivers:";
	QStringList drivers = QSqlDatabase::drivers();
	foreach(QString driver, drivers)
		qDebug() << driver; 
	QFile file(QString::fromLocal8Bit("8月指标数据.txt"));
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Can't open the file!" << endl;
	}
	QLibrary *hello_lib = NULL;
	//写清楚库的路径，如果放在当前工程的目录下，路径为./Oracle.so  
	hello_lib = new QLibrary("D:\\oracle64Client\\product\\11.2.0\\client_1\\BIN\\oci.dll");

	//加载动态库  
	hello_lib->load();
	if (!hello_lib->isLoaded())
	{
		qDebug()<<QString::fromLocal8Bit("load Oracle failed!");
//		return;
	}
	//连接数据库  
	QSqlDatabase db = QSqlDatabase::addDatabase("QOCI");
	db.setPort(1521);
	db.setHostName("HDYC");
	db.setDatabaseName("orcl10g");
	db.setUserName("imsoft");
	db.setPassword("imsoft");
	if (db.open()) {
		qDebug() <<QString::fromLocal8Bit("链接远程数据库成功");
		QMessageBox::information(&w,QString("Warning"),QString::fromLocal8Bit("链接远程数据库成功"),QMessageBox::Ok);
	}
	else {
		qDebug() << QString::fromLocal8Bit("链接远程数据库失败");
		qDebug() << db.lastError().text();
		QMessageBox::information(&w, QString("Warning"), QString::fromLocal8Bit("链接远程数据库失败"), QMessageBox::Ok);
//		return;
	}
	qDebug() << db.isOpenError() << db.isOpen();
	QTextStream stream(&file);
//	stream.setCodec(code);//输出流的设置--------必须嘀
	int lineNum = 0;//文件行数
	while (!stream.atEnd())
	{
 stream.readLine();
 lineNum++;
	}
	int FeaDim = 0;//特征维数
	stream.seek(0);//回到第一行
	QString mystr = stream.readLine();	//读取第一行
	stream.seek(0);//回到第一行
	QRegularExpression regExp("\\d+\\.?\\d+");
	QRegularExpressionMatchIterator iterator = regExp.globalMatch(mystr);//创建正则表达式匹配迭代器
	while (iterator.hasNext())
	{
	iterator.next();
	FeaDim++;
	}
	init_param();
	if (param.gamma == 0) param.gamma = 0.5;
	svm_problem prob;
	//int num = 1990;
	//lineNum = 1000;
	prob.l = lineNum; //样本数
	prob.y = new double[prob.l];
	double d;
	int probfeature = FeaDim; //样本特征维数
	svm_node *x_space = new svm_node[(probfeature + 1)*prob.l];//样本特征存储空间
	prob.x = new svm_node *[prob.l]; //每一个X指向一个样本
	cout << "size: " << sizeof(x_space) << endl;
	srand((unsigned)time(NULL));
	for (int index = 0; index < lineNum; index++)
	{
		QString str = stream.readLine();
		QRegularExpressionMatchIterator temp_ite = regExp.globalMatch(str);
		int temp_index= index*(probfeature + 1);
		int i = 0;//索引值
		if (index == 992)
			int ii = i;
		while (i<=FeaDim)
		{
			double value = 0;
			if (i<FeaDim)
			{
				QRegularExpressionMatch match = temp_ite.next();
				value = match.captured(0).toDouble();
			}
			if (i != FeaDim)
			{
				x_space[temp_index + i].index = i + 1;
				x_space[temp_index + i].value = value;
			}
			else
			{
				x_space[temp_index + i].index = -1;
			}
			i++;				
		}
		prob.x[index] = &x_space[temp_index];///
		prob.y[index] = 1;//类别为1
	}
	
	const char* string = svm_check_parameter(&prob, &param);//检测模型参数配置是否有误

	svm_model *model = svm_train(&prob, &param);//训练模型


	QString sFilePath = "result1.txt";

	QFile myfile(sFilePath);
	//方式：Append为追加，WriteOnly，ReadOnly  
	if (!myfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		//QMessageBox::critical(NULL, "提示", "无法创建文件");
		return -1;
	}
	QTextStream out(&myfile);
	QFile testFile(QString::fromLocal8Bit("8月指标数据.txt"));
	if (!testFile.open(QIODevice::ReadOnly | QIODevice::Text))
		qDebug() << "Can not open file" << endl;
	QTextStream testRead(&testFile);
//	stream.seek(0);
	svm_node *xnode=new svm_node[FeaDim];
	int normalCount = 0;//正常样本数
	for (int index = 0; index < lineNum; index++)
	{
		QString str = testRead.readLine();
		QRegularExpressionMatchIterator temp_ite = regExp.globalMatch(str);
		//int temp_index = index*(probfeature + 1);
		int i = 0;//索引值
		while (temp_ite.hasNext())
		{
			QRegularExpressionMatch match = temp_ite.next();
			double value = match.captured(0).toDouble();
			if (i != FeaDim - 1)
			{
				xnode[i].index = i + 1;
				xnode[i].value = value;
			}
			else
			{
				xnode[i].index = -1;
			}
			i++;
		}
		d = svm_predict(model, xnode);
		if (d == 1)
			normalCount++;
		out << d << endl;
		out.flush();
	}
	double rate = normalCount / (double)lineNum;
	QMessageBox::information(&w, QString("提示"), QString::number(rate),QMessageBox::StandardButton::Ok);
	myfile.close();
	file.close();
	testFile.close();
	delete[] x_space;
	delete[] prob.x;
	delete[] prob.y;
	return a.exec();
}
