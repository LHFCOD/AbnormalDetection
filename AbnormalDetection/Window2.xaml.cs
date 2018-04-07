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
using System.Windows.Shapes;
using Accord.MachineLearning.VectorMachines.Learning;
using Accord.Statistics.Kernels;
using Accord.MachineLearning.VectorMachines;
using Accord.Math.Optimization.Losses;
using Accord.Math;
using System.Data.OleDb;
using System.Data;
using System.IO;
using System.Xml;

namespace AbnormalDetection
{
    /// <summary>
    /// Interaction logic for Window2.xaml
    /// </summary>
    public partial class Window2 : Window
    {
        private OleDbConnection conn;
        private OleDbCommand cmd;
        List<ParameterSetting> ParaList = new List<ParameterSetting>();
        public Window2()
        {
            InitializeComponent();

            //double[] samples = GenerateRandomData();
            //List<int> target = MoveWindowFunc(samples,100,100);
            //MessageBox.Show("共" + target.Count + "条记录,占比" + target.Count + "/" + samples.Count() + "=" + ((double)target.Count / samples.Count()));
            //PrintSteadyData(samples, target, "DF.txt");


            // DataTable dt = new DataTable();
            // dt.Columns.Add(new DataColumn("序号", typeof(int)));
            // dt.Columns.Add(new DataColumn("距离", typeof(double)));
            // DataRow row1 = dt.NewRow();
            // row1["序号"] = 1;
            // row1["距离"] = 1;
            // dt.Rows.Add(row1);
            // DataRow row2 = dt.NewRow();
            // row2["序号"] = 2;
            // row2["距离"] = 2;
            // dt.Rows.Add(row2);
            // DataRow row3 = dt.NewRow();
            // row3["序号"] = 3;
            // row3["距离"] = 3;
            // dt.Rows.Add(row3);
            // DataRow row4 = dt.NewRow();
            // row4["序号"] = 4;
            // row4["距离"] = 4;
            // dt.Rows.Add(row4);
            // var re = from h in dt.AsEnumerable()
            //          orderby h.Field<int>("序号") descending
            //          select h;
            //var temp= re.Take(2);

            LOF lof = new LOF();
              lof.GenerateSamplesFromExcel(13541,5);
         
        
               // lof.GenerateUniformSamples(2, 3, extent);
                lof.ComputeLOF();

            lof.WriteToExcel(13541, 5);
            lof.WriteToTxt("正常样本3000.txt");
            MessageBox.Show("LOF计算成功！");
       //     lof.samples = lof.GenerateSamples(20, 3);
         //   lof.SamplesInfor();
         //   var ree=lof.k_domain(0);
            //ConnectToDatabase();///链接到数据库

        }
        private void OnCompute(object sender,RoutedEventArgs args)
        {
            try
            {
                ParaList = ReadXml(@"..\..\ParaXml.xml");//读取xml文档
                FileStream file = new FileStream("file.txt", FileMode.Create);///创建文件用于存放稳态工况数据
                StreamWriter writer = new StreamWriter(file);
                writer.WriteLine("创建日期" + DateTime.Now.ToString());
                writer.Flush();

                List<DateTime> timeList = new List<DateTime>();//存放稳态工况的时间点
                string strStart = ((DateTime)StartDate.SelectedDate).ToString("yyyy-MM-dd");///开始时间字符串
                string strEnd = ((DateTime)EndDate.SelectedDate).ToString("yyyy-MM-dd");//结束时间字符串
                DateTime loopTime = (DateTime)StartDate.SelectedDate;///开始时间,用于循环
                DateTime EndTime = (DateTime)EndDate.SelectedDate;//结束时间
                ////从参数表中取出参数详细信息
                DataTable measure_dt = QueryData("select id,name,saveindex from tb_nx_measure");
                string strSql = "select cytime";
                foreach (ParameterSetting childParameter in ParaList)///查找存储索引并构建字符串
                {
                    var re = from h in measure_dt.AsEnumerable()
                              where h.Field<string>("name") == childParameter.name
                              select Convert.ToInt32(h.Field<object>("saveindex"));
                    if (re == null)
                        throw new Exception("结果为null");
                    if (re.Count() !=1)
                        throw new Exception("未找到指定参数");              
                    foreach (int d in re)
                    {
                        childParameter.saveindex = d;
                        strSql += ",V";
                        strSql += d.ToString();
                    }
                }
                strSql=strSql+ " from tb_nx_his_run where cytime between '" + strStart + "' and '" + strEnd + "'";//完成对字符串的构建
                DataTable data_dt = QueryData(strSql);///查询指定时间内的数据
                TimeSpan Windowspan = new TimeSpan(0, ParaList[0].time, 0);///窗口宽度/min
                TimeSpan Scannspan = new TimeSpan(0, int.Parse(扫描间隔.Text), 0);///扫描间隔/min

                do
                {
                    DateTime tempEndTime = loopTime + Windowspan;
                    var re2 = from h in data_dt.AsEnumerable()
                              let tempTime = Convert.ToDateTime(h.Field<string>("cytime"))
                              where tempTime >= loopTime && tempTime < tempEndTime
                              select h;
                    bool isQualified = true;//先假设该窗口合格
                                            //单纯的稳态判别方式
                                            //for(int index=0;index<ParaList.Count();index++)
                                            //{
                                            //    double max = re2.Max(x => Convert.ToDouble(x.Field<object>(index+1)));
                                            //    double min = re2.Min(x => Convert.ToDouble(x.Field<object>(index+1)));
                                            //    if(ParaList[index].type==0)
                                            //    {
                                            //        if (Math.Abs(max - min) > ParaList[index].value)
                                            //        {
                                            //            isQualified = false;
                                            //            break;
                                            //        }
                                            //    }
                                            //    if (ParaList[index].type == 1)
                                            //    {
                                            //        if (Math.Abs(max - min)/max > ParaList[index].value)
                                            //        {
                                            //            isQualified = false;
                                            //            break;
                                            //        }
                                            //    }
                                            //}


                    ///DF检验判别方式
                    double[] samples = new double[re2.Count()];//新建指定宽度窗口内的样本
               
                    for(int index=0;index<re2.Count();index++)
                    {
                        samples[index]=Convert.ToDouble(re2.ElementAt(index).Field<object>(1));
                    }
                  ParaVec paraVec= ComputeParaVec( GenerateDataFromData(samples));
                    if (paraVec.tao1 > -2.86)
                        isQualified = false;
                    if (isQualified)///如果窗口合格
                    {
                        foreach (var d in re2)
                        {
                            DateTime temp_time = Convert.ToDateTime(d.Field<string>("cytime"));
                            if (!timeList.Contains(temp_time))
                                timeList.Add(temp_time);
                        }
                    }
                    loopTime += Scannspan;///将循环时间增加一个扫面时间间隔
                    if (loopTime > (EndTime - Windowspan))///如果循环时间不足以进行下一次的计算，则推出循环
                        break;
                }
                while (true);
                writer.WriteLine("共" + timeList.Count + "条记录,占比"+timeList.Count+"/"+data_dt.Rows.Count+"="+ ((double)timeList.Count/ data_dt.Rows.Count));
                //foreach (DateTime date in timeList)
                //{
                //    var re3 = from h in data_dt.AsEnumerable()
                //              let tempTime = Convert.ToDateTime(h.Field<string>("cytime"))
                //              where tempTime == date
                //              select Convert.ToDouble(h.Field<object>(1));
                //    foreach (double d in re3)
                //    {
                //        writer.WriteLine(date.ToString("yyyy-MM-dd HH:mm:ss") + "\t" + d.ToString());
                //    }
                //}
                //writer.Flush();
                //writer.Close();
                //file.Close();
                foreach (DataRow row in data_dt.Rows)
                {
                    DateTime tempTime = Convert.ToDateTime(row.Field<string>("cytime"));//取出时间
                    string temp_str = tempTime.ToString("yyyy-MM-dd HH:mm:ss");
                    for(int i=1;i<data_dt.Columns.Count;i++)
                    {
                        temp_str = temp_str + "\t" + row.Field<object>(i).ToString();
                    }
                   
                    if (timeList.Contains(tempTime))
                    {
                        writer.WriteLine(temp_str);
                    }
                    else
                    {
                        writer.WriteLine();
                    }
                    writer.Flush();
                }
                file.Close();
                MessageBox.Show("共" + timeList.Count + "条记录,占比" + timeList.Count + "/" + data_dt.Rows.Count + "=" + ((double)timeList.Count / data_dt.Rows.Count));
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        void ConnectToDatabase()///连接到数据库
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
        List<ParameterSetting> ReadXml(string strPath)///根据路径解析xml文档
        {
            try
            {
                List<ParameterSetting> ParaList = new List<ParameterSetting>();//新建参数配置动态链表
                XmlDocument doc = new XmlDocument();//新建xml文档
                XmlReaderSettings setting = new XmlReaderSettings();//新建xml设置
                setting.IgnoreComments = true;///忽略注释
                XmlReader reader = XmlReader.Create(strPath, setting);//根据设置和文件路径读取xml文档
                doc.Load(reader);///根据读入流加载xml文档
                XmlNode root = doc.SelectSingleNode("Root");//获取根节点
                foreach (XmlNode node in root.ChildNodes)///对根节点的子节点进行遍历
                {
                    ParameterSetting temp_setting = new ParameterSetting();
                    XmlElement element = (XmlElement)node;///将节点强制转化为元素
                    temp_setting.name = element.GetAttribute("name").ToString();///获取元素的属性
                    temp_setting.type = int.Parse(element.GetAttribute("type").ToString());
                    temp_setting.time = int.Parse(element.GetAttribute("time").ToString());
                    temp_setting.value = double.Parse(element.GetAttribute("value").ToString());
                    ParaList.Add(temp_setting);//添加到链表中
                }
                reader.Close();
           
                return ParaList;
            }
            catch(Exception ex)
            {
                throw new Exception(ex.Message);
            }
            
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
    public class ParameterSetting
    {
        public int type;//0-绝对误差，1-相对误差
        public int time;///持续时间,分钟
        public double value;//波动值
        public string name;//参数名称
        public int saveindex;//存储索引
    }
   
}
