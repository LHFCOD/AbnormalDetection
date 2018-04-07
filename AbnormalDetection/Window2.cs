using Accord.MachineLearning.VectorMachines;
using Accord.MachineLearning.VectorMachines.Learning;
using Accord.Math;
using Accord.Statistics;
using Accord.Statistics.Distributions.Univariate;
using Accord.Statistics.Kernels;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace AbnormalDetection
{
    partial class Window2
    {
        double[] GenerateRandomData()//随机生成样本
        {
            int mCount = 1000;///样本总数量
            int inflextion1 = 300;//拐点1所在位置
            int inflextion2 = 600;//拐点2所在位置
            double slope = 1;//斜率
            int windWidth = 200;//稳态检测窗口长度
            int intervalWidth = 40;//每次窗口滑动的间隔长度
            double[] samples = new double[mCount];
            NormalDistribution normal = new NormalDistribution(0, 20);
            double[] Normalsamples = normal.Generate(mCount);///生成正态分布样本
            //  UniformContinuousDistribution normal = new UniformContinuousDistribution(-10, 10);
            // double[] Normalsamples = normal.Generate(mCount);

            ///拐点1前的样本
            for (int index = 0; index < inflextion1; index++)
            {
                samples[index] = Normalsamples[index];
            }
            ///拐点1和拐点2之间的样本
            for (int index = 0; index < inflextion2 - inflextion1; index++)
            {
                double offset = index * slope;
                samples[index + inflextion1] = Normalsamples[index + inflextion1] + offset;
            }
            ///拐点2之后的样本
            for (int index = 0; index < mCount - inflextion2; index++)
            {
                samples[index + inflextion2] = Normalsamples[index + inflextion2]+samples[inflextion2-1];
            }
            return samples;
        }
        //一阶含常数无趋势自回归DF检验
        ParaVec ComputeParaVec(double[] samples)
        {
            int mCount = samples.Count();
            //定义矩阵系数
            double a11 = 0;
            double a12 = 0;
            double a13 = 0;
            double a21 = 0;
            double a22 = 0;
            double a23 = 0;
            double a31 = 0;
            double a32 = 0;
            double a33 = 0;
            ///定义向量
            double b1 = 0;
            double b2 = 0;
            double b3 = 0;
            ///定义解向量
            double c1 = 0;
            double c2 = 0;
            double c3 = 0;
            for (int index = 1; index < mCount; index++)
            {
                a11++;
                a12 += samples[index - 1];
                a13 += (index + 1);
                a22 += samples[index - 1] * samples[index - 1];
                a23 += (index + 1) * samples[index - 1];
                a33 += (index + 1) * (index + 1);

                b1 += samples[index];
                b3 += (index + 1) * samples[index];
                b2 += samples[index - 1] * samples[index];
            }
            a21 = a12;
            a31 = a13;
            a32 = a23;
            ParaVec paraVec = new ParaVec();//新建参数向量矩阵
            //情形1
            {
                double[,] aMatrix = new double[1, 1];
                aMatrix[0, 0] = a22;
                double[] bMatrix = new double[1];
                bMatrix[0] = b2;
                double[] cMatrix = new double[1];
                cMatrix = Matrix.Multiply(aMatrix.Inverse(), bMatrix);//计算参数向量

                paraVec.fai1 = cMatrix[0];
                double squre = aMatrix.Inverse()[0,0];
                double s2t = 0;
                for (int index = 1; index < mCount; index++)
                {
                    paraVec.sigma21 += Math.Pow((samples[index]  - paraVec.fai1 * samples[index - 1]), 2);
                    
                }
                s2t = paraVec.sigma21 + samples[0] * samples[0];
                paraVec.sigma21 /= mCount - 1; 
                s2t /= mCount - 1;
                paraVec.tao1 = (paraVec.fai1 - 1) / Math.Sqrt(s2t * squre);
                paraVec.rou1 = samples.Count() * (paraVec.fai1-1);
            }
            //情形2
            {
                double[,] aMatrix = new double[2, 2];
                aMatrix[0, 0] = a11;
                aMatrix[0, 1] = a12;
                aMatrix[1, 0] = a21;
                aMatrix[1, 1] = a22;
                double[] bMatrix = new double[2];
                bMatrix[0] = b1;
                bMatrix[1] = b2;
                double[] cMatrix = new double[2];
                cMatrix = Matrix.Multiply(aMatrix.Inverse(), bMatrix);//计算参数向量
               
                paraVec.miu2 = cMatrix[0];
                paraVec.fai2 = cMatrix[1];
                paraVec.sigma22 = 0;
                double[] xishu = new double[] { 0, 1 };
                double squre = xishu.Multiply(aMatrix.Inverse()).Multiply(xishu.Transpose())[0];
                double s2t = 0;
                for (int index = 1; index < mCount; index++)
                {
                    paraVec.sigma22 += Math.Pow((samples[index] - paraVec.miu2 - paraVec.fai2 * samples[index - 1]), 2);
                   // squre += Math.Pow(samples[index - 1], 2);
                }
                s2t = paraVec.sigma22 + (samples[0]-paraVec.miu2) * (samples[0] - paraVec.miu2);
                paraVec.sigma22 /= mCount - 1;
                s2t /= mCount - 1;
                paraVec.tao2 = (paraVec.fai2 - 1) / Math.Sqrt(s2t * squre);
                paraVec.rou2 = samples.Count() * (paraVec.fai2 - 1);
            }
            //情形3
            {
                double[,] aMatrix = new double[3, 3];
                aMatrix[0, 0] = a11;
                aMatrix[0, 1] = a12;
                aMatrix[0, 2] = a13;
                aMatrix[1, 0] = a21;
                aMatrix[1, 1] = a22;
                aMatrix[1, 2] = a23;
                aMatrix[2, 0] = a31;
                aMatrix[2, 1] = a32;
                aMatrix[2, 2] = a33;
                double[] bMatrix = new double[3];
                bMatrix[0] = b1;
                bMatrix[1] = b2;
                bMatrix[2] = b3;
                double[] cMatrix = new double[3];
                cMatrix = Matrix.Multiply(aMatrix.Inverse(), bMatrix);//计算参数向量

                paraVec.miu3 = cMatrix[0];
                paraVec.fai3 = cMatrix[1];
                paraVec.beta3 = cMatrix[2];

                double[] xishu = new double[] { 0, 1 ,0};
                double squre = xishu.Multiply(aMatrix.Inverse()).Multiply(xishu.Transpose())[0];
                double s2t = 0;
                for (int index = 1; index < mCount; index++)
                {
                    paraVec.sigma23 += Math.Pow((samples[index] - paraVec.miu3 -paraVec.beta3*(index+1)- paraVec.fai3 * samples[index - 1]), 2);
                 //   squre += Math.Pow(samples[index - 1], 2);
                }
                s2t = paraVec.sigma23 + (samples[0] - paraVec.miu3) * (samples[0] - paraVec.miu3);
                paraVec.sigma23 /= mCount - 1;
                s2t /= mCount - 1;
                paraVec.tao3 = (paraVec.fai3 - 1) / Math.Sqrt(s2t * squre);
                paraVec.rou3 = samples.Count() * (paraVec.fai3 - 1);
            }
            return paraVec;
        }


        double[] GenerateDataFromData(double[] outputs)//扩充小样本
        {
            Accord.Math.Random.Generator.Seed = 0;
            double[][] inputs = new double[outputs.Count()][];
            for(int index=0;index<outputs.Count();index++)
            {
                inputs[index] = new double[] { (double)index / outputs.Count() };
            }

            var learn = new SequentialMinimalOptimizationRegression<Polynomial>()
            {
                Kernel = new Polynomial(2), // Polynomial Kernel of 2nd degree
                Complexity = 100
            };

            // Run the learning algorithm
            SupportVectorMachine<Polynomial> svm = learn.Learn(inputs, outputs);

            // Compute the predicted scores
            

            int mCount = 1000;//输出样本数量
            double[][] tempinputs= new double[mCount][];
            for (int index = 0; index < mCount; index++)
            {
                tempinputs[index] = new double[] { (double)index / mCount };
            }
            double[] predicted = svm.Score(tempinputs);
            return predicted;
        }
        List<int> MoveWindowFunc(double[] samples, int width, int interval)//滑窗法进行筛选
        {
            int index = 0;
            List<int> list = new List<int>();
            do
            {
                double[] temp_samp = new double[width];//临时样本
                bool isQualified = false;
                for (int i = 0; i < width; i++)
                {
                    temp_samp[i] = samples[index + i];
                }
                double means = temp_samp.Mean();
                for (int i = 0; i < width; i++)
                {
                    temp_samp[i] -= means;
                }

                ParaVec paraVec=ComputeParaVec(temp_samp);
                if (paraVec.tao1 < -1.95)//-1.95   -2.89   -3.45
                    isQualified = true;
                if (index >= 300)
                    ;
                if (isQualified)///如果窗口合格
                {
                    for (int j=0;j<width;j++)
                    {
                        if (!list.Contains(j+index))
                            list.Add(j+index);
                    }
                }

                index += interval;
                if(index+width>samples.Count())
                break;
            }
            while (true);
            return list;
        }

        void PrintSteadyData(double[] samples,List<int> target,string fileName)
        {
            FileStream file = new FileStream(fileName, FileMode.Create);
            StreamWriter write = new StreamWriter(file);
            write.AutoFlush = true;
            write.WriteLine("共" + target.Count + "条记录,占比" + target.Count + "/" + samples.Count() + "=" + ((double)target.Count / samples.Count()));
            for(int index=0;index<samples.Count();index++)
            {
                string str=index+"\t"+samples[index];
                if (target.IndexOf(index) != -1)
                {
                    str += "\t";
                    str+=samples[index];
                }
                write.WriteLine(str);
            }
            write.Close();
            file.Close();            
        }
    }
    
    class ParaVec
    {
        //情形1
        public double fai1 = 0;
        public double sigma21 = 0;
        public double tao1 = 0;
        public double rou1 = 0;
        //情形2
        public double miu2 = 0;
        public double fai2 = 0;
        public double sigma22 = 0;
        public double tao2 = 0;
        public double rou2 = 0;
        //情形3
        public double miu3 = 0;
        public double beta3 = 0;
        public double fai3 = 0;
        public double sigma23 = 0;
        public double tao3 = 0;
        public double rou3 = 0;
    }

}
