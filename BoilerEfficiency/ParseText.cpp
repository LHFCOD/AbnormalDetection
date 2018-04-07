#include "ParseText.h"
using namespace Parse;

Exception::Exception(type _type)
{
	m_type = _type;
}
ParseText::ParseText()
{
}
ParseText::ParseText(QString str)
{
	//��ȡ�ַ���
	QRegularExpression exp("\\S+");
	QRegularExpressionMatch match;

	int index = 0;
	do
	{
		match = exp.match(str,index);
		if (match.hasMatch())
		{
			strList.append(match.captured());
			index = match.capturedEnd();
		}
		else
			break;
	} while (index < str.length());
	//��ȡ����
	exp.setPattern("\\S\\n\\S");
	index = 0;
	rowCount = 1;
	do
	{
		match = exp.match(str, index);
		if (match.hasMatch())
		{
			index = match.capturedEnd();
			rowCount++;
		}
		else
			break;
	} while (index < str.length());

	if (strList.count() % rowCount != 0)
	{
		Exception *ex = new Exception(Exception::type::ContentIrregular);
		throw ex;
	}
	//��ȡ����
	columnCount = strList.count() / rowCount;
}

ParseText::~ParseText()
{
}
QString ParseText::Serch(int row, int column)
{
	int num = strList.count();
	if (row>=rowCount || row <0 || column<0 || column>=columnCount)
	{
		Exception *ex = new Exception(Exception::type::IndexOverflow);
		throw ex;
	}
	return strList.at(row*columnCount + column);
}
QString Exception::ShowError()
{
	return Converter.ToString(m_type);
}