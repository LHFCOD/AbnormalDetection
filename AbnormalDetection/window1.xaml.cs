using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Accord.MachineLearning.VectorMachines.Learning;
using Accord.Statistics.Kernels;
using Accord.MachineLearning.VectorMachines;
using Accord.Math.Optimization.Losses;
using System.Data.OleDb;
using System.Data;
using System.IO;

namespace AbnormalDetection
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private OleDbConnection conn;
        OleDbCommand cmd;

        public MainWindow()
        {
            InitializeComponent();
            ConnectToDatabase();

        }
        void ConnectToDatabase()
        {
            try
            {
                string str_conn = "provider=oraoledb.oracle;user id=imsoft;data source=orcl10g_my;password=imsoft";
                conn = new OleDbConnection(str_conn);
                conn.Open();
                cmd = new OleDbCommand();
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
                Application.Current.Shutdown();
            }
        }
        void OnSteadyState(object sender, RoutedEventArgs args)////进行稳态筛选
        {
            try
            {
                FileStream file = new FileStream("file.txt", FileMode.Create);///创建文件用于存放稳态工况数据
                StreamWriter writer = new StreamWriter(file);
                writer.WriteLine("创建日期"+DateTime.Now.ToString());
                writer.Flush();

                List<DateTime> timeList = new List<DateTime>();//存放稳态工况的时间点
                string strStart = ((DateTime)StartDate.SelectedDate).ToString("yyyy-MM-dd");///开始时间字符串
                string strEnd = ((DateTime)EndDate.SelectedDate).ToString("yyyy-MM-dd");//结束时间字符串

                ////从参数表中取出参数详细信息
                DataTable measure_dt = QueryData("select id,name,saveindex from tb_nx_measure");
                int fuhe_index = 0;///功率存储索引
                var re1 = from h in measure_dt.AsEnumerable()
                          where h.Field<string>("name") == "#5机组发电机功率"
                          select Convert.ToInt32(h.Field<object>("saveindex"));
                foreach (int d in re1)
                {
                    fuhe_index = d;
                }
                string strSql = "select cytime,v" + fuhe_index.ToString() + " from tb_nx_his_run where cytime between '" + strStart + "' and '" + strEnd + "'";
                DataTable data_dt = QueryData(strSql);///查询指定时间内的功率数据
                TimeSpan Windowspan = new TimeSpan(0, int.Parse(窗口宽度.Text), 0);///窗口宽度/min
                TimeSpan Scannspan=new TimeSpan(0, int.Parse(扫描间隔.Text), 0);///扫描间隔/min
                double maxFuheInt = double.Parse(功率最大宽度.Text);

                DateTime loopTime = (DateTime)StartDate.SelectedDate;///开始时间
                DateTime EndTime = (DateTime)EndDate.SelectedDate;//结束时间
                do
                {
                    DateTime tempEndTime = loopTime + Windowspan;
                    var re2 = from h in data_dt.AsEnumerable()
                              let tempTime = Convert.ToDateTime(h.Field<string>("cytime"))
                              where tempTime >= loopTime && tempTime < tempEndTime
                              select h;
                    double maxFuhe = re2.Max(x => Convert.ToDouble(x.Field<object>(1)));
                    double minFuhe = re2.Min(x => Convert.ToDouble(x.Field<object>(1)));
                    if(maxFuhe-minFuhe<maxFuheInt)///如果负荷变化幅度小于设定值
                    {
                        foreach(var d in re2)
                        {
                            DateTime temp_time = Convert.ToDateTime(d.Field<string>("cytime"));
                            if(!timeList.Contains(temp_time))
                            timeList.Add(temp_time);
                        }
                    }
                    loopTime += Scannspan;///将循环时间增加一个扫面时间间隔
                    if (loopTime > (EndTime - Windowspan))///如果循环时间不足以进行下一次的计算，则推出循环
                        break;
                }
                while (true);

                //foreach(DateTime date in timeList)
                //{
                //    var re3 = from h in data_dt.AsEnumerable()
                //              let tempTime = Convert.ToDateTime(h.Field<string>("cytime"))
                //              where tempTime == date
                //              select Convert.ToDouble(h.Field<object>(1));
                //    foreach(double d in re3)
                //    {
                //        writer.WriteLine(date.ToString("yyyy-MM-dd HH:mm:ss") + "\t" + d.ToString());
                //    }
                //}
                //writer.Flush();
                foreach(DataRow row in data_dt.Rows)
                {
                    DateTime tempTime = Convert.ToDateTime(row.Field<string>("cytime"));//取出时间
                    double tempFuhe= Convert.ToDouble(row.Field<object>(1));//取出功率
                    if(timeList.Contains(tempTime))
                    {
                        writer.WriteLine(tempTime.ToString("yyyy-MM-dd HH:mm:ss") + "\t" + tempFuhe.ToString());
                    }
                    else
                    {
                        writer.WriteLine();
                    }
                    writer.Flush();
                }
            }
            catch(Exception ex)
            {
                MessageBox.Show(ex.Message);
            }

        }
        void TestSVM()///检测ＳＶＭ正确性
        {
            Accord.Math.Random.Generator.Seed = 0;

            // Example regression problem. Suppose we are trying
            // to model the following equation: f(x, y) = 2x + y

            //            double[][] inputs = // (x, y)
            //            {
            //    new double[] { 0,  1 }, // 2*0 + 1 =  1
            //    new double[] { 4,  3 }, // 2*4 + 3 = 11
            //    new double[] { 8, -8 }, // 2*8 - 8 =  8
            //    new double[] { 2,  2 }, // 2*2 + 2 =  6
            //    new double[] { 6,  1 }, // 2*6 + 1 = 13
            //    new double[] { 5,  4 }, // 2*5 + 4 = 14
            //    new double[] { 9,  1 }, // 2*9 + 1 = 19
            //    new double[] { 1,  6 }, // 2*1 + 6 =  8
            //};
            double[][] inputs = new double[8][];
            for (int i = 0; i < 8; i++)
            {
                double[] temp = new double[1];
                temp[0] = Math.PI / 2 / 8 * i;
                inputs[i] = temp;
            }
            //            double[] outputs = // f(x, y)
            //            {
            //    1, 11, 8, 6, 13, 14, 19, 8
            //};
            double[] outputs = new double[8];
            for (int i = 0; i < 8; i++)
            {
                outputs[i] = Math.Sin(Math.PI / 2 / 8 * i);
            }

            // Create the sequential minimal optimization teacher
            var learn = new SequentialMinimalOptimizationRegression<Gaussian>()
            {
                //Kernel = new Polynomial(3), // Polynomial Kernel of 2nd degree
                //Kernel = new Gaussian(2),
                Complexity = 100
            };

            // Run the learning algorithm
            SupportVectorMachine<Gaussian> svm = learn.Learn(inputs, outputs);

            // Compute the predicted scores
            double[] predicted = svm.Score(inputs);

            // Compute the error between the expected and predicted
            double error = new SquareLoss(outputs).Loss(predicted);

            // Compute the answer for one particular example
            double fxy = svm.Score(new double[1] { 2 }); // 1.0003849827673186

            //double[] myerror = new double[8];
            //for(int i=0;i<8;i++)
            //{
            //    myerror[i]=Math.Abs(predicted[i]-outputs[i])/
            //}
        }
        DataTable QueryData(string strSql)
        {
            try
            {
                cmd.CommandText = strSql;
                OleDbDataAdapter adp = new OleDbDataAdapter(cmd);
                DataSet set = new DataSet();
                DataTable dt = new DataTable();
                adp.Fill(set);
                dt = set.Tables[0];
                return dt;
            }
            catch (Exception ex)
            {
                //MessageBox.Show(ex.Message);
                throw new Exception(ex.Message);
            }
        }
    }
}
