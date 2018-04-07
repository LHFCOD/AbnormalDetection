#include "svm.h"
#include <string.h>
#include<iostream>
#include<time.h>
#include<cmath>
using namespace std;

svm_parameter param;
#define PI 3.1415926
void init_param() {
	param.svm_type = ONE_CLASS;
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
	param.nu = 0.01;////

}
int main() {
	int a = 0xE0 | 0x0D;
	printf("%x", a);
	init_param();
	if (param.gamma == 0) param.gamma = 0.5;
	svm_problem prob;
	int num = 100;
	prob.l = num; //样本数
	prob.y = new double[prob.l];
	double d;
	int probfeature = 2; //样本特征维数
	svm_node *x_space = new svm_node[(probfeature + 1)*prob.l];//样本特征存储空间
	prob.x = new svm_node *[prob.l]; //每一个X指向一个样本
	cout << "size: " << sizeof(x_space) << endl;
	srand((unsigned)time(NULL));
	for (int index = 0; index < num; index++)
	{
		int temp_index = index*(probfeature + 1);
		double radius= rand() / double(RAND_MAX);
		double angle= rand() / double(RAND_MAX)*2*PI;
		x_space[temp_index].index = 1;
		x_space[temp_index].value = radius*cos(angle);
		x_space[temp_index +1].index = 2;
		x_space[temp_index +1].value = radius*sin(angle);
		x_space[temp_index +2].index = -1;
		prob.x[index] = &x_space[temp_index];///
		prob.y[index] = 1;
		///没有y
	}

	
	const char* string = svm_check_parameter(&prob, &param);
	
	//train data 身高190cm,体重70kg，男
	/*x_space[0].index = 1;
	x_space[0].value = 190;
	x_space[1].index = 2;
	x_space[1].value = 70;
	x_space[2].index = -1;
	prob.x[0] = &x_space[0];
	prob.y[0] = 1;
	x_space[3].index = 1;
	x_space[3].value = 180;
	x_space[4].index = 2;
	x_space[4].value = 80;
	x_space[5].index = -1;
	prob.x[1] = &x_space[3];
	prob.y[1] = 1;
	x_space[6].index = 1;
	x_space[6].value = 161;
	x_space[7].index = 2;
	x_space[7].value = 45;
	x_space[8].index = -1;
	prob.x[2] = &x_space[6];
	prob.y[2] = -1;
	x_space[9].index = 1;
	x_space[9].value = 163;
	x_space[10].index = 2;
	x_space[10].value = 47;
	x_space[11].index = -1;
	prob.x[3] = &x_space[9];
	prob.y[3] = -1;*/
	svm_model *model = svm_train(&prob, &param);
	//predict 身高180cm, 体重85kg
	svm_node xnode[3];
	xnode[0].index = 1;
	xnode[0].value = 0100;
	xnode[1].index = 2;
	xnode[1].value = 0.9;
	xnode[2].index = -1;
	d = svm_predict(model, xnode);
	cout << d << " " << endl;
	//predict 身高161cm,体重50kg
	xnode[0].index = 1;
	xnode[0].value = 0;
	xnode[1].index = 2;
	xnode[1].value = 0;
	xnode[2].index = -1;
	d = svm_predict(model, xnode);
	cout << d << " " << endl;

	delete[] x_space;
	delete[] prob.x;
	delete[] prob.y;
	cin.get();
}