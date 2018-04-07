#pragma once
#include <QtCore>
#include <math.h>
#include "IExceptionBase.h"
#include "ParseText.h"
#include "svm.h"
namespace Boiler
{
	static const  double ebsilon = 2;
	struct ReceivedBasis
	{
		double Car;
		double Har;
		double Oar;
		double Nar;
		double Sar;
		double Aar;
		double Mar;
	};

	class InputArg
	{
	public:

		InputArg();
		~InputArg();
	public:
		ReceivedBasis basis;//ú��
		double m_aerfa;//��������ϵ��
		QString m_file;//�������������
		double t0;//�����¶�
		double tpy;//�����¶�
	};
	
	class SmokeLoss
	{
	public:
		
		SmokeLoss(ReceivedBasis _basis,double _aerfa, double Qnet,QString file,double t0,double tpy);
		~SmokeLoss();
	public:
		double V0;//���ۿ����������m3/kg��
		double L0;//���ۿ�����������kg/kg��

		double VRO2;//��ԭ�����
		double V0H2O;//����ˮ�������
		double V0N2;//���۵������

		double V0gy;//���۸�������
		double V0y;//����������

		double VN2;//ʵ�ʵ�����
		double Vgy;//ʵ�ʸ�������
		double Vy;//ʵ��������
		double VH2O;//ʵ��ˮ������

		double Cpgy;//���̱���
		double Qgy;//��������

		double q2;//������ʧ����
		double Q2;//������ʧ
		

		QList<double> CpCO2;
		QList<double> CpN2;
		QList<double> CpH2O;

		bool GetHeatCapacity(QString file);//��ȡ���������

	};

	class SolidLoss
	{
	public:
		SolidLoss(double _Qnet, double _Aar, double _Aerfa);
		double GetSolidLossFromO(double O2);
		double GetSolidLossFromArefa(double Arefa);
		~SolidLoss();
		void init_param();
		svm_model* TrainModel(double* x, double* y, int num);
		double Predict(svm_model* model, double input);
	public:
		svm_parameter param;
		svm_model *fh_model;
		svm_model *lz_model;
		double Aar;
		double Qnet;
		double q4;
	};

	class BoilerEffi
	{
	public:
		BoilerEffi(InputArg _input);
		BoilerEffi();
		~BoilerEffi();
		void UpdateBoilerEffi();
		 bool ElementalAnalysis(ReceivedBasis _basis);
		inline bool CheckAerfa(double _aerfa);
	public:	 
			 void InitInput(InputArg _input);
	  InputArg m_input;//�������

	  double Qgr;//ú��λ������
	  double Qnet;//ú��λ������
	public:	 
		SmokeLoss *m_smokeLoss;
	public:
		SolidLoss *m_solidLoss;
	};

	
	
	class Exception:public IExceptionBase
	{
	public:
		enum type
		{
			CoalComposition,//ú����ɴ���
			ExcessAir,//��������ϵ������
			FileError,//�����ļ�����
			FhOrLzNumError//�ɻһ�¯����ѵ����Ŀ����
		};
	private: 
		type m_type;
	public:
		inline	Exception(type _type)
		{
			m_type = _type;
		};
		virtual QString ShowError();

		class MPTypeConverter {
		public:
		inline	MPTypeConverter() {
				map.insert(type::CoalComposition, "CoalComposition");
				map.insert(type::ExcessAir, "ExcessAir");
				map.insert(type::FileError, "FileError");				
				map.insert(type::FhOrLzNumError, "FhOrLzNumError");
			}

		inline	QString ToString(type key) {
				MPTypeStringMap::iterator pos = map.find(key);
				if (pos != map.end())
					return QString(pos->data());
				return QString("");
			}

		private:
			typedef QMap<type,QString> MPTypeStringMap;
			MPTypeStringMap map;
		};
		MPTypeConverter Converter;
	};

	
}