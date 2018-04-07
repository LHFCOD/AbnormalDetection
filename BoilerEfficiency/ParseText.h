#pragma once
#include "QtCore"
#include "IExceptionBase.h"
namespace Parse
{
	class Exception :public IExceptionBase
	{
	public:
		enum type
		{
			IndexOverflow,//����Խ��
			ContentIrregular//�ı����ݲ�����
		};
		type m_type;
		Exception(type _type);
		virtual QString ShowError();

		class MPTypeConverter {
		public:
			inline	MPTypeConverter() {
				map.insert(type::ContentIrregular, "ContentIrregular");
				map.insert(type::IndexOverflow, "IndexOverflow");
			}

			inline	QString ToString(type key) {
				MPTypeStringMap::iterator pos = map.find(key);
				if (pos != map.end())
					return QString(pos->data());
				return QString("");
			}

		private:
			typedef QMap<type, QString> MPTypeStringMap;
			MPTypeStringMap map;
		};

		MPTypeConverter Converter;
	};
	class ParseText
	{
	public:
		ParseText();
		ParseText(QString str);
		~ParseText();
	public:
		QString pattern = "\\b\\w+\\b";//ƥ��ģʽ
		QString Serch(int row, int column);
		QList<QString> strList;
		int rowCount;
		int columnCount;
	};


}

