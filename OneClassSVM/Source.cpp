#include "svm.h"
#include <string.h>

#include<iostream>
#include<time.h>
#include<cmath>
#include <fstream>
#include <regex>
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
	int num = 1000;//ѵ��������
	int testNum = 10000;//����������
	prob.l = num; //������
	prob.y = new double[prob.l];
	double d;
	int probfeature = 2; //��������ά��
	svm_node *x_space = new svm_node[(probfeature + 1)*prob.l];//���������洢�ռ�
	prob.x = new svm_node *[prob.l]; //ÿһ��Xָ��һ������
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
		///û��y
	}

	
	const char* string = svm_check_parameter(&prob, &param);
	
	//train data ���190cm,����70kg����
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
	svm_model *model = svm_train(&prob, &param);//ѵ��
	svm_node xnode[3];//������Զ�ά���������ṹ��
	ofstream outfile;//�������ļ���
	outfile.open("result.txt");//�����������ļ�
	
	ofstream outTempFile;//��ʱ�ļ�
	outTempFile.open("tempFile.txt");
	outTempFile <<"����������"<<"\t"<< "�������" << "\t" << "������" << "\t" << "�쳣��" << endl;
	for (testNum = 100; testNum <= 500; testNum++)
	{
		int normalNum = 0;//����������
		int erroNum = 0;//����������
		for (int index = 0; index < testNum; index++)
		{
			double x = rand() / double(RAND_MAX) * 2 - 1;
			double y = rand() / double(RAND_MAX) * 2 - 1;
			xnode[0].index = 1;
			xnode[0].value = x;
			xnode[1].index = 2;
			xnode[1].value = y;
			xnode[2].index = -1;
			d = svm_predict(model, xnode);
			//outfile << d << endl;//�����������ļ�
			if (d == 1)
			{
				normalNum++;
			}
			if (x*x + y*y <= 1)
			{
				if (d == -1)
					erroNum++;
			}
			else
			{
				if (d == 1)
					erroNum++;
			}
			//cout << d << " " << endl;
		}
		double rate = 0;//������
		double errorate = 0;//������
		rate = normalNum / (double)testNum;
		errorate = erroNum / (double)testNum;
		outTempFile <<testNum<<"\t"<< PI / 4 << "\t" << rate << "\t" << errorate << endl;
	}
	/*cout << "�������Ϊ:" << PI / 4<<endl;
	cout <<"������Ϊ:"<< rate<<endl;
	cout << "������Ϊ:" << errorate << endl;*/
	outfile.close();
	outTempFile.close();

	delete[] x_space;
	delete[] prob.x;
	delete[] prob.y;
	cin.get();
}