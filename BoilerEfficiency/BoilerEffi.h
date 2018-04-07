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
		ReceivedBasis basis;//煤质
		double m_aerfa;//过量空气系数
		QString m_file;//气体比热容性质
		double t0;//环境温度
		double tpy;//排烟温度
	};
	
	class SmokeLoss
	{
	public:
		
		SmokeLoss(ReceivedBasis _basis,double _aerfa, double Qnet,QString file,double t0,double tpy);
		~SmokeLoss();
	public:
		double V0;//理论空气量（体积m3/kg）
		double L0;//理论空气量（质量kg/kg）

		double VRO2;//三原子体积
		double V0H2O;//理论水蒸气体积
		double V0N2;//理论氮气体积

		double V0gy;//理论干烟气量
		double V0y;//理论烟气量

		double VN2;//实际氮气量
		double Vgy;//实际干烟气量
		double Vy;//实际烟气量
		double VH2O;//实际水蒸气量

		double Cpgy;//干烟比容
		double Qgy;//干烟吸热

		double q2;//排烟损失比例
		double Q2;//排烟损失
		

		QList<double> CpCO2;
		QList<double> CpN2;
		QList<double> CpH2O;

		bool GetHeatCapacity(QString file);//获取气体比热容

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
	  InputArg m_input;//输入参数

	  double Qgr;//煤高位发热量
	  double Qnet;//煤低位发热量
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
			CoalComposition,//煤质组成错误
			ExcessAir,//过量空气系数错误
			FileError,//输入文件错误
			FhOrLzNumError//飞灰或炉渣的训练数目错误
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