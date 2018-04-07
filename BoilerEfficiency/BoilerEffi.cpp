#include "BoilerEffi.h"

using namespace Boiler;

BoilerEffi::BoilerEffi()
{


}
BoilerEffi::BoilerEffi(InputArg _input)
{
	InitInput(_input);
	m_smokeLoss = new SmokeLoss(m_input.basis, m_input.m_aerfa,Qnet, m_input.m_file, m_input.t0, m_input.tpy);//计算排烟损失
	m_solidLoss = new SolidLoss(Qnet,m_input.basis.Aar,_input.m_aerfa);
}

BoilerEffi::~BoilerEffi()
{
	if(m_smokeLoss)
	delete m_smokeLoss;
	if(m_solidLoss)
	delete m_solidLoss;
}

void Boiler::BoilerEffi::UpdateBoilerEffi()
{
	if (m_smokeLoss)
		delete m_smokeLoss;
	if (m_solidLoss)
		delete m_solidLoss;
	InitInput(m_input);
	m_smokeLoss = new SmokeLoss(m_input.basis, m_input.m_aerfa, Qnet, m_input.m_file, m_input.t0, m_input.tpy);//计算排烟损失
	m_solidLoss = new SolidLoss(Qnet, m_input.basis.Aar,m_input.m_aerfa);
}

bool Boiler::BoilerEffi::ElementalAnalysis(ReceivedBasis _basis)//检测煤质
{
	if (_basis.Aar < 0 || _basis.Car < 0 || _basis.Mar < 0 || _basis.Nar < 0 || _basis.Oar < 0 || _basis.Sar < 0)
		return false;
	if (fabs(_basis.Aar + _basis.Car + _basis.Har + _basis.Mar + _basis.Nar + _basis.Oar + _basis.Sar - 100) > ebsilon)
		return false;
	return true;
}
bool BoilerEffi::CheckAerfa(double _aerfa)//检测过量空气系数
{
	if (_aerfa < 0)
		return false;
	return true;
}
void BoilerEffi::InitInput(InputArg _input)
{
	if (BoilerEffi::ElementalAnalysis(_input.basis) == false)
	{
		Exception *ex = new Exception(Exception::type::CoalComposition);
		throw ex;
	}
	if (CheckAerfa(_input.m_aerfa) == false)
	{
		Exception *ex = new Exception(Exception::type::ExcessAir);
		throw ex;
	}
	

	m_input = _input;
	Qgr = 339 *m_input.basis.Car + 1256 * m_input.basis.Har + 109 * m_input.basis.Sar - 109 * m_input.basis.Oar;
	Qnet = Qgr - 2500 * (9 * m_input.basis.Har / 100 + m_input.basis.Mar / 100);
}

SmokeLoss::SmokeLoss(ReceivedBasis _basis, double _aerfa,double Qnet, QString file, double t0, double tpy)//排烟损失构造函数用来计算排烟损失
{
	V0 = 0.0899*(_basis.Car + 0.375*_basis.Sar) + 0.265*_basis.Har - 0.0333*_basis.Oar;
	L0 = 1.293*V0;

	VRO2 = 1.866*_basis.Car / 100 + 0.7*_basis.Sar / 100;
	V0N2 = 0.8*_basis.Nar / 100 + 0.79*V0;
	V0H2O = 11.1*_basis.Har / 100 + 1.24*_basis.Mar / 100 + 0.0161*V0;

	V0gy = VRO2 + V0N2;
	V0y = V0gy + V0H2O;

	Vy = V0y + (_aerfa - 1)*V0 + 0.0161*(_aerfa - 1)*V0;
	Vgy = V0gy + (_aerfa - 1)*V0;
	VH2O = Vy - Vgy;
	VN2 = V0N2 + 0.79*(_aerfa - 1)*V0;

	if (GetHeatCapacity(file) == false)
	{
		Exception *ex = new Exception(Exception::type::FileError);
		throw ex;
	}
	double qCO2 = 0;//单位体积CO2吸热量
	double qN2 = 0;//单位体积N2吸热量
	double qH2O = 0;//单位体积H2O吸热量
	for (int i = 0; i < CpCO2.count(); i++)
	{
		qCO2 += 1.0 / (i + 1)*CpCO2.at(i)*pow(tpy, i + 1);
		qCO2 -= 1.0 / (i + 1)*CpCO2.at(i)*pow(t0, i + 1);

		qN2 += 1.0 / (i + 1)*CpN2.at(i)*pow(tpy, i + 1);
		qN2 -= 1.0 / (i + 1)*CpN2.at(i)*pow(t0, i + 1);

		qH2O += 1.0 / (i + 1)*CpH2O.at(i)*pow(tpy, i + 1);
		qH2O -= 1.0 / (i + 1)*CpH2O.at(i)*pow(t0, i + 1);

	}
	Qgy = VRO2*qCO2 + VN2*qN2;
	Q2 = Qgy + VH2O*qH2O;
	
	q2 = Q2 / Qnet * 100;
}

SmokeLoss::~SmokeLoss()
{
}
InputArg::InputArg()
{

}
InputArg::~InputArg()
{

}
bool SmokeLoss::GetHeatCapacity(QString file)
{
	Parse::ParseText parse(file);
	for (int i = 1; i < parse.rowCount; i++)
	{
		CpCO2.append(parse.Serch(i, 1).toDouble());
		CpN2.append(parse.Serch(i, 2).toDouble());
		CpH2O.append(parse.Serch(i, 3).toDouble());
	}
	return true;
}

QString Exception::ShowError()
{
	return Converter.ToString(m_type);
}

Boiler::SolidLoss::SolidLoss(double _Qnet,double _Aar,double _Aerfa)
{
	Qnet = _Qnet;
	Aar = _Aar;
	double O2[] = { 5.54,5.39,4.43,4.43,5.91,4.64,4.49,4.59,4.82,4.64,4.25 };
	double Cfh[] = { 0.15,0.17,0.19,0.15,0.18,0.24,0.46,0.35,0.49,0.57,0.33 };
	double Clz[] = { 0.69,1.35,1.25,1.53,1.41,1.48,1.98,1.30,2.22,2.71,2.23 };
	int size = sizeof(O2);
	if (size != sizeof(Cfh) || size != sizeof(Clz))
	{
		Exception *ex = new Exception(Exception::type::FhOrLzNumError);
		throw ex;
	}
	int num = size / sizeof(double);
	fh_model = TrainModel(O2, Cfh, num);
	lz_model = TrainModel(O2, Clz, num);
	q4=GetSolidLossFromArefa(_Aerfa);
}

double Boiler::SolidLoss::GetSolidLossFromO(double O2)
{
	//O2 = 4.43;
	double Cfh = Predict(fh_model, O2);
	double Clz = Predict(lz_model, O2);
	double solidLoss = (0.9*Cfh / (100 - Cfh) + 0.1*Clz / (100 - Clz)) * 32700 * Aar / Qnet;
	return solidLoss;
}

double Boiler::SolidLoss::GetSolidLossFromArefa(double Arefa)
{
	double O2 = 21 - 21 / Arefa;
	return GetSolidLossFromO(O2);
}

Boiler::SolidLoss::~SolidLoss()
{
	if (fh_model)
		free(fh_model);
	if (lz_model)
		free(lz_model);
}
void SolidLoss::init_param() {
	param.svm_type = EPSILON_SVR;
	param.kernel_type = RBF;
	param.degree = 3;
	param.gamma = 0.0001;
	param.coef0 = 0;
	param.nu = 0.5;
	param.cache_size = 100;
	param.C = 10;
	param.eps = 1e-5;
	param.shrinking = 1;
	param.probability = 0;
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;

	param.coef0 = 1;
	param.p = 0.1;
	//param.

}

svm_model* Boiler::SolidLoss::TrainModel(double* x, double* y, int num)
{
	svm_model *model;
	init_param();
	if (param.gamma == 0) param.gamma = 0.5;
	svm_problem prob;
	prob.l = num; //样本数
	prob.y = new double[prob.l];
	double d;
	int probfeature = 1; //样本特征维数
	svm_node *x_space = new svm_node[(probfeature + 1)*prob.l];//样本特征存储空间
	prob.x = new svm_node *[prob.l]; //每一个X指向一个样本


	for (int index = 0; index < num; index++)
	{
		int temp_index = index*(probfeature + 1);
		x_space[temp_index].index = 1;
		x_space[temp_index].value = *(x + index);
		x_space[temp_index + 1].index = -1;
		prob.x[index] = &x_space[temp_index];///
		prob.y[index] = *(y + index);
		///没有y
	}


	const char* string = svm_check_parameter(&prob, &param);
	model = svm_train(&prob, &param);//训练

//	delete[] x_space;
//	delete[] prob.x;
//	delete[] prob.y;
	return model;
}

double Boiler::SolidLoss::Predict(svm_model * model, double input)
{
	double output;
	svm_node xnode[2];//定义测试二维测试样本结构体
	xnode[0].index = 1;
	xnode[0].value = input;
	xnode[1].index = -1;
	output = svm_predict(model, xnode);
	return output;
}

