#pragma once
#include <QtCore>
class IExceptionBase:public QException
{
public:
	IExceptionBase();
	virtual ~IExceptionBase();
	virtual QString ShowError()=0;
};

