using Accord.Statistics.Distributions.Univariate;
using Microsoft.Office.Interop.Excel;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Data;
using System.IO;
using System.Linq;
using System.Text;

namespace AbnormalDetection
{
    class LOF
    {
        int m_k = 3;
        double accuracy = 1e-5;
        public double[][] m_samples;
        int dimension;//样本维度
        int count;//样本数目
        IEnumerable[] domain_set;//样本领域集合
        double[] distance_set;//样本k距离集合
        double[] lrdk;         //局部可达密度 
       public double[] lofk;//局部离群点因子 
        public LOF(int k)
        {
            m_k = k;
          //   ComputeLOF();
        }
        public LOF()
        {
             // ComputeLOF();
        }
     public   void ComputeLOF()
        {
          // samples= GenerateSamples(20, 2);//随机生成样本
            SamplesInfor();//获取样本基本信息
            domain_set = new IEnumerable[count];
            distance_set = new double[count];
            lrdk = new double[count];
            lofk = new double[count];
            for (int index = 0; index < count; index++)
            {
                domain_set[index] = k_domain(index);
                distance_set[index] = ((IEnumerable<DataRow>)domain_set[index]).Last().Field<double>("距离");
            }
            //计算局部可达密度
            for(int index=0;index<count;index++)
            {
                double temp = 0;
                foreach(DataRow row in domain_set[index])
                {
                    int o_seq = row.Field<int>("序号");
                    temp += ReachDist(index, o_seq);
                }
                lrdk[index] = ((IEnumerable<DataRow>)domain_set[index]).Count()/temp ;
            }
            //计算局部离群点因子
            for (int index = 0; index < count; index++)
            {
                double temp = 0;
                foreach (DataRow row in domain_set[index])
                {
                    int o_seq = row.Field<int>("序号");
                    temp += lrdk[o_seq]/lrdk[index];
                }
                lofk[index] = temp/((IEnumerable<DataRow>)domain_set[index]).Count() ;
            }
        }
        //对象p相对于对象o的可达距离 
        public double ReachDist(int p, int o)
        {
            double o_dist = distance_set[o];
            double distance = 0;
            for (int i = 0; i < dimension; i++)
            {
                distance += Math.Pow(m_samples[p][i] - m_samples[o][i], 2);
            }
            if (o_dist > distance)
                return o_dist;
            else
                return distance;
        }
        //获取样本基本信息
        public void SamplesInfor()
        {
            dimension = 0;
            count = m_samples.GetLength(0);
            if (m_samples != null)
                dimension = m_samples[0].Length;
            else
                throw new Exception("样本为空！");
        }
        //计算k领域
        public IEnumerable k_domain(int SerialNum)
        {
            System.Data.DataTable dt = new System.Data.DataTable();
            dt.Columns.Add(new DataColumn("序号", typeof(int)));
            dt.Columns.Add(new DataColumn("距离", typeof(double)));
            for (int index = 0; index < count; index++)
            {
                if (index == SerialNum)//对除要研究的样本点以外的样本进行遍历
                    continue;
                DataRow row = dt.NewRow();
                row["序号"] = index;
                double distance = 0;
                for (int i = 0; i < dimension; i++)
                {
                    distance += Math.Pow(m_samples[index][i] - m_samples[SerialNum][i], 2);
                }
                row["距离"] = Math.Sqrt(distance);
                dt.Rows.Add(row);
            }
            var res = from h in dt.AsEnumerable()
                      orderby h.Field<double>("距离")
                      select h;//依照距离进行排序
            double k_distance = res.ElementAt(m_k - 1).Field<double>("距离");

            int abs_NkP = m_k;//初始化k领域的点的个数，最小为m_k
            for (int index = m_k; index < res.Count(); index++)
            {
                double temp_distance = res.ElementAt(index).Field<double>("距离");
                if (Math.Abs(temp_distance - k_distance) < accuracy)
                    abs_NkP++;
                else
                    break;
            }
            var res_NkP = res.Take(abs_NkP);
            return res_NkP;
        }
        //生成样本
        public void GenerateSamples(int nCount, int nDimension)
        {
            // Create a new uniform continuous distribution from 0.42 to 1.1
            // var uniform = new UniformContinuousDistribution(a: -1, b: 1);
            var uniform = new NormalDistribution(0,2);
            double[][] temp_Samples = new double[nCount][];
            for (int index = 0; index < nCount; index++)
            {
                double[] temp_smp = new double[nDimension];
                for (int i = 0; i < nDimension; i++)
                {
                    temp_smp[i] = uniform.Generate();
                }
                temp_Samples[index] = temp_smp;
            }
            //异常点
            {
                //double[] temp_smp = new double[nDimension];
                //for (int i = 0; i < nDimension; i++)
                //{
                   
                //    temp_smp[i] = 1.2;
                //}
                temp_Samples[0][0] = 1.2;
            }
            m_samples= temp_Samples;
        }
      public  void GenerateSamplesFromExcel(int nCount,int Index=1)
        {
            //启动Excel应用程序
            Microsoft.Office.Interop.Excel.Application xls = new Microsoft.Office.Interop.Excel.Application();
            //如果表已经存在，可以用下面的命令打开
            _Workbook book = xls.Workbooks.Open(System.Environment.CurrentDirectory + @"\content.xlsx");
            _Worksheet sheet;//定义sheet变量
            xls.Visible = false;//设置Excel后台运行
            xls.DisplayAlerts = false;//设置不显示确认修改提示
            sheet = (_Worksheet)book.Worksheets.get_Item(Index);//获得第i个sheet，准备写入
            double[][] temp_Samples = new double[nCount][];
            for(int index=0;index<nCount;index++)
            {
                double[] temp_smp = new double[7];
                temp_smp[0] = Convert.ToDouble(((Range)sheet.Cells[index+2, "B"]).Value);
                temp_smp[1] = Convert.ToDouble(((Range)sheet.Cells[index + 2, "D"]).Value);
                temp_smp[2] = Convert.ToDouble(((Range)sheet.Cells[index + 2, "F"]).Value);
                temp_smp[3] = Convert.ToDouble(((Range)sheet.Cells[index + 2, "G"]).Value);
                temp_smp[4] = Convert.ToDouble(((Range)sheet.Cells[index + 2, "H"]).Value);
                temp_smp[5] = Convert.ToDouble(((Range)sheet.Cells[index + 2, "I"]).Value);
                temp_smp[6] = Convert.ToDouble(((Range)sheet.Cells[index + 2, "J"]).Value);
                temp_Samples[index] = temp_smp;
            }
          //  temp_Samples[0][0] = 20;
            m_samples = temp_Samples;
            book.Close();
            xls.Quit();
        }
        public void GenerateUniformSamples( int nDimension, int segment,double extent, double startPoint=0, double endPoint=1)
        {
            int NumP = segment + 1;
            double length = (endPoint - startPoint) / segment;
            int nCount = Convert.ToInt32(Math.Pow(NumP, nDimension));
            double[][] temp_Samples = new double[nCount+1][];
            for(int index=0;index<nCount;index++)
            {
                double[] temp_smp = new double[nDimension];
                int inter = index;//中间结果
                for(int i=0;i<nDimension;i++)
                {
                    temp_smp[i]=(inter % NumP)*length;
                    inter /= NumP;
                }
                temp_Samples[index] = temp_smp;
            }
            temp_Samples[nCount] = new double[nDimension];
              temp_Samples[11].CopyTo(temp_Samples[nCount],0);
            temp_Samples[nCount][0] = temp_Samples[nCount][0] + length * extent;
            m_samples = temp_Samples;
        }
        public void WriteToTxt(string path)
        {
            FileStream file = new FileStream(path, FileMode.Create);
            StreamWriter writer = new StreamWriter(file);
            writer.AutoFlush = true;
            writer.WriteLine("lofk");
            foreach(double temp in lofk)
            {
                writer.WriteLine(temp);
            }
            writer.Close();
            file.Close();
        }
        public void WriteToExcel(int nCount, int Index = 1)
        {
            //启动Excel应用程序
            Microsoft.Office.Interop.Excel.Application xls = new Microsoft.Office.Interop.Excel.Application();
            //如果表已经存在，可以用下面的命令打开
            _Workbook book = xls.Workbooks.Open(System.Environment.CurrentDirectory + @"\content.xlsx");
            _Worksheet sheet;//定义sheet变量
            xls.Visible = false;//设置Excel后台运行
            xls.DisplayAlerts = false;//设置不显示确认修改提示
            sheet = (_Worksheet)book.Worksheets.get_Item(Index);//获得第i个sheet，准备写入
           
            for (int index = 0; index < nCount; index++)
            {
                if (lofk[index] >= 2)
                {
                    ((Range)sheet.Cells[index + 2, "K"]).Value = ((Range)sheet.Cells[index + 2, "B"]).Value;
                    ((Range)sheet.Cells[index + 2, "L"]).Value = ((Range)sheet.Cells[index + 2, "D"]).Value;
                    ((Range)sheet.Cells[index + 2, "M"]).Value = ((Range)sheet.Cells[index + 2, "F"]).Value;
                    ((Range)sheet.Cells[index + 2, "N"]).Value = ((Range)sheet.Cells[index + 2, "G"]).Value;
                    ((Range)sheet.Cells[index + 2, "O"]).Value = ((Range)sheet.Cells[index + 2, "H"]).Value;
                    ((Range)sheet.Cells[index + 2, "P"]).Value = ((Range)sheet.Cells[index + 2, "I"]).Value;
                    ((Range)sheet.Cells[index + 2, "Q"]).Value = ((Range)sheet.Cells[index + 2, "J"]).Value;
                }
                ((Range)sheet.Cells[index + 2, "R"]).Value = lofk[index];
                
            }
            //  temp_Samples[0][0] = 20;
            book.Save();
            book.Close();
            xls.Quit();
        }
    }
}
