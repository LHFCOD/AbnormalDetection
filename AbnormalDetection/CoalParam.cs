using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace AbnormalDetection
{
    class CoalParam
    {
        double Pe=1000;//功率
double P0=26.25;//主汽压力
double P1=5.555;//高压缸排汽压力
double P2=5;//再热蒸汽压力
double P3=0.619;//中压缸排汽压力
double Pgs=32.702;//给水压力
double Pgjs=32.702;//过热器减温水压力
double Pzjs=12;//再热器减温水压力
double t0=600;//主汽温度
double t1=352.7;//高压缸排汽温度
double t2=600;//再热蒸汽温度
double tgs=290;//给水温度
double tgjs=290;//过热器减温水温度
double tzjs=210;//再热器减温水温度
double Dgs=2719.8;//给水流量,
double Dgjs1=0;
double Dgjs2=0;
double Dgjs3=0;
double Dgjs4=0;//过热器减温水四个分流量
double Dzjs1=0;
double Dzjs2=0;//再热器减温水两个分流量 
double[] A= { 63.25, 3.4 ,11.17, 0.64, 0.5 ,14 ,7.04 }; //依次是收到基C,H,O,N,S,M,A
double[] d= { 0.17 ,1.81 };//分别为飞灰含碳量、炉渣含碳量
double Qr=23390;//低位发热量
double tpy1=124.5;//空预器A出口烟温
double tpy2=124.5;//空预器B出口烟温
double t01=5.26;//送风机A入口温度
double t02=5.26;//送风机B入口温度
double O21=3.23;//空预器A出口氧量
double O22=3.23;//空预器B出口氧量
double pg1=21.63;//#1高厂变有功功率
double pg2=17.17;//#2高厂变有功功率
 ///以上参数如果没有实时值的话，建议留成开口或者直接写死

    }
}
