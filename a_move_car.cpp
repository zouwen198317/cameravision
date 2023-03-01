#include "a_move_car.h"
#include "ui_a_move_car.h"
#include<QtDebug>
#include<QSettings>
#include <QDateTime>
#include <QString>
#include <QTime>
#include <iostream>
#include <thread>
#include <QSettings>
#include <QCoreApplication>
#include <QDir>
#include<direct.h>

#include "opencv2/calib3d/calib3d.hpp"

extern std::string string_To_UTF8(const std::string & str);

extern std::string UTF8_To_string__kk(const std::string& str);



/**
 * @brief mkMutiDir  创建多级目录
 * @param path	     未创建的目录
 * @return 	     返回已创建的目录
 */

QString mkMutiDir(const QString path){
    QDir dir(path);
    if ( dir.exists(path)){
        return path;
    }
    QString parentDir = mkMutiDir(path.mid(0,path.lastIndexOf('/')));
    QString dirname = path.mid(path.lastIndexOf('/') + 1);
    QDir parentPath(parentDir);
    if ( !dirname.isEmpty() )
        parentPath.mkpath(dirname);
    return parentDir + "/" + dirname;
}



a_move_car::a_move_car(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::a_move_car)
{
    ui->setupUi(this);

    string path_ini="./cfg";
    if (_access(path_ini.c_str(), 0) == -1)
    {
        int i = _mkdir(path_ini.c_str());
    }




    ui->lineEdit_huiyuandian_sudu->setText(QString::number(huiyuandian_sudu));
    ui->lineEdit_dangqian_weizhi->setText(QString::number(Read_Pos1));
    ui->lineEdit_dangqian_bianmaq->setText(QString::number(Read_Enc1));
    ui->lineEdit_dangqian_sudu->setText(QString::number(Read_Vel1));



    ///////////////////////////////////////////////////////////////////////
    int cols =  ui->lineEdit_xiangjishu->text().toInt();//DEV_NUM;
    int rows = 7;
    int  cols_sz = 70;
    ui->tableWidget->setColumnCount(cols);
    ui->tableWidget->setRowCount(rows);

    QHeaderView* headerView = ui->tableWidget->horizontalHeader();
    headerView->setDefaultAlignment(Qt::AlignLeft);     //表头字 左对齐
    headerView->setStretchLastSection(true);
    ui->tableWidget->setShowGrid(true);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);  //可多选 单选
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);   //设置选择行为时每次选择一行
    //ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);    //设置编辑
    ui->tableWidget->setStyleSheet("selection-background-color:lightblue;");//设置选中背景色

    ui->tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;color: black;}");
    ui->tableWidget->verticalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;color: black;}");



    //这两个循环是必须要创建
    for(int i = 0; i < cols; i++)
    {
        ui->tableWidget->setColumnWidth(i,cols_sz);        //设置第一列的宽度

        xiangmu_cols[QString::fromLocal8Bit( ui->tableWidget->horizontalHeaderItem(i)->text().toLocal8Bit())] =i;

        cols_xiangmu[i] =QString::fromLocal8Bit( ui->tableWidget->horizontalHeaderItem(i)->text().toLocal8Bit()) ;

        rec_dev_infor new_str;
        new_str.name =QString::fromLocal8Bit( ui->tableWidget->horizontalHeaderItem(i)->text().toLocal8Bit());

        qstr_dev_unit_infor[QString::fromLocal8Bit( ui->tableWidget->horizontalHeaderItem(i)->text().toLocal8Bit())] = new_str;

        int_dev_unit_infor[i] = new_str;

    }

    for(int j = 0; j < rows; j++)
    {

        qDebug()<<ui->tableWidget->verticalHeaderItem(j)->text();

        xiangmu_rows[QString::fromLocal8Bit( ui->tableWidget->verticalHeaderItem(j)->text().toLocal8Bit())] =j;

        rows_xiangmu[j] =QString::fromLocal8Bit( ui->tableWidget->verticalHeaderItem(j)->text().toLocal8Bit()) ;

    }


    // 初始化是否 启用，要 调用ini来初始化
    for(int i = 0; i < cols; i++)
    {
        QString name_dev = ui->tableWidget->horizontalHeaderItem(i)->text().toLocal8Bit();
        // dev_unit_infor[name_dev].name = name_dev;

        //是否选用这个相机
        head_infor[name_dev].if_enable_checkBox = new QTableWidgetItem();
        head_infor[name_dev].if_enable_checkBox ->setCheckState(Qt::Unchecked);
        head_infor[name_dev].if_enable_checkBox ->setText(QString::fromLocal8Bit("钩启用"));
        ui->tableWidget->setItem(0, i,  head_infor[name_dev].if_enable_checkBox);

        //位置
        ui->tableWidget->setItem(1, i,new QTableWidgetItem(QString::number(1000)));
        head_infor[name_dev].shebei_weizhi = 1000;

        //运动方向
        head_infor[name_dev].combox_yundong_fangxia = new QComboBox(); // 下拉选择框控件
        head_infor[name_dev].combox_yundong_fangxia->addItem(QString::fromLocal8Bit("正方向"));
        head_infor[name_dev].combox_yundong_fangxia->addItem(QString::fromLocal8Bit("逆方向"));
        ui->tableWidget->setCellWidget(2,  i, (QWidget*)head_infor[name_dev].combox_yundong_fangxia);

        connect(head_infor[name_dev].combox_yundong_fangxia, SIGNAL(currentIndexChanged(QString)), this, SLOT(clickCombobox_yundong_fx(QString)));

        //输出位号
        head_infor[name_dev].combox_shuchu_weihao = new QComboBox(); // 下拉选择框控件
        for(int k = 0; k<16; k++)
        {
            QString haowei="DO"+QString::number(k);
            head_infor[name_dev].combox_shuchu_weihao->addItem(haowei);
        }
        head_infor[name_dev].combox_shuchu_weihao->addItem(QString::fromLocal8Bit("无"));
        ui->tableWidget->setCellWidget(3,  i, (QWidget*)head_infor[name_dev].combox_shuchu_weihao);

        connect(head_infor[name_dev].combox_shuchu_weihao, SIGNAL(currentIndexChanged(QString)), this, SLOT(clickCombobox_shuchu_wh(QString)));

        //输出动作
        head_infor[name_dev].combox_shuchu_dongzuo = new QComboBox(); // 下拉选择框控件

        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("1:IO断路输出"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("2:IO开漏输出电平"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("3:取反IO"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("4:不用"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("5:输出500us低脉冲"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("6:输出1ms脉冲"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("7:保留"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("8:保留"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("9:输出2ms脉冲"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("10:输出3ms脉冲"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("11:输出4ms脉冲"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("12:输出5ms脉冲"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("13:输出6ms脉冲"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("14:输出7ms脉冲"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("15:输出8ms脉冲"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("16:输出9ms脉冲"));

        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("17:输出10ms脉冲"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("18:输出20ms脉冲"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("19:输出30ms脉冲"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("20:输出40ms脉冲"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("21:输出50ms脉冲"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("22:输出60ms脉冲"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("23:输出70ms脉冲"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("24:输出80ms脉冲"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("25:输出90ms脉冲"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("26:输出100ms脉冲"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("27:输出200ms脉冲"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("28:输出300ms脉冲"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("29:输出400ms脉冲"));
        head_infor[name_dev].combox_shuchu_dongzuo->addItem(QString::fromLocal8Bit("30:输出500ms脉冲"));



        ui->tableWidget->setCellWidget(4,  i, (QWidget*)head_infor[name_dev].combox_shuchu_dongzuo);
        connect(head_infor[name_dev].combox_shuchu_dongzuo, SIGNAL(currentIndexChanged(QString)), this, SLOT(clickCombobox_shuchu_dongzuo(QString)));


        //延时
        ui->tableWidget->setItem(5, i,new QTableWidgetItem(QString::number(0)));
        head_infor[name_dev].yanshi = 0;
        // dev_unit_infor[name_dev].shebei_weizhi = 1000;

        // 触发个数
        ui->tableWidget->setItem(6, i,new QTableWidgetItem(QString::number(0)));
        head_infor[name_dev].chufa_geshu = 0;

    }



    m_listfile_xinghao = saomiao_file("./cfg");
    m_listfile_moren =  saomiao_file("./moren");

    ui->comboBox_jizhong->addItems(m_listfile_xinghao);


    read_ini_file_g("./cfg");


    int countx =   m_jlcs.m_str_Jiluxinxi.count(m_moren_xinghao_ming);

    if(countx >=1)
    {
        int k = 0;
        for(int i = 0; i < ui->comboBox_jizhong->count(); i++)
        {
            if(ui->comboBox_jizhong->itemText(i) == m_moren_xinghao_ming)
            {
                //查找到了你需要的查找的字符串在combox中的位置
                k = i;
            }
        }

        ui->comboBox_jizhong->setCurrentIndex(k);

        set_win_from_ini_file_g(m_moren_xinghao_ming);

    }

    flag_changge_zuihou = 1;


    flag_data_ch = 1;



}

void  a_move_car::chushihua_ok_ng_chuiqi()
{
    // unsigned short Piece_Input_Number = 0; //对应的触发器链接口
    //unsigned long Piece_Find = 0; //物件个数
    // unsigned long Piece_Size[10] = {0}; //物件尺寸：10 组数据
    // unsigned long Piece_Distance_To_nextp[10] = {0}; //物件间隔：10 组数据
    // unsigned long Piece_Cross_Camera = 0; //通过全部相机的个数
    // unsigned long Virtual_Piece_Cross_Camera = 0; //存储通过全部相机的个数


    //    rtn = MCF_Sorting_Get_Piece_State_Net(Piece_Input_Number,
    //                                          &Piece_Find,
    //                                          &Piece_Size[0],
    //            &Piece_Distance_To_nextp[0],
    //            &Virtual_Piece_Cross_Camera, StationNumber);


}

short a_move_car::SetCameraFunction(unsigned short CameraNumber, unsigned short StationNumber)
{

    short          i = 0;
    short		   rtn = 0;
    unsigned long  Customet_CameraPosition = 0;
    unsigned long  CameraPosition = 0;
    unsigned short CameraActionMode = 0;
    unsigned short CameraActionIO = 0;
    unsigned short CameraMotionDir = 0;
    double Delay_Camera_Count_MS = 0;

    int xiangjishu = ui->lineEdit_xiangjishu->text().toInt();
    for(int i = 0; i <  xiangjishu; i++)
    {

        if((i + 1) == CameraNumber)
        {
            rec_dev_infor  x = qstr_dev_unit_infor[cols_xiangmu[i]];
            if(x.if_enable ==1)
            {
                //获取方向
                CameraMotionDir			= x.yundong_fangxia;

                //获取位置
                CameraPosition			= x.shebei_weizhi;

                //获取ActionMode

                CameraActionMode		= (unsigned short)(x.shuchu_dongzuo + 1);
                //获取ActionIO

                CameraActionIO			= (unsigned short)x.shuchu_weihao;

                if(i < 8)
                {
                    Delay_Camera_Count_MS   = x.yanshi;
                }


                if(CameraNumber < 9)
                {

                    for(int x = 0; x < 8; x++)
                    {
                        if(x != i)
                        {
                            if(qstr_dev_unit_infor[cols_xiangmu[x]].shuchu_weihao == CameraActionIO  &&(qstr_dev_unit_infor[cols_xiangmu[x]].if_enable))
                            {

                                // cout<<i<<"  "<<x<<endl;
                                QMessageBox::information(NULL,  QObject::tr("IO"), QObject::tr("REP"),  QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
                            }
                        }
                    }
                    rtn = MCF_Sorting_Set_Camera_Net(CameraNumber, CameraPosition,CameraMotionDir, CameraActionMode, CameraActionIO, StationNumber);
                    rtn = MCF_Sorting_Set_Trig_Camera_Delay_Count_Net(CameraNumber, Delay_Camera_Count_MS, StationNumber);
                }else if(CameraNumber == 9)
                {
                    rtn = MCF_Sorting_Set_Blow_OK_Net(CameraPosition, CameraMotionDir, CameraActionMode, CameraActionIO, StationNumber);

                    xialiao_bangding["OK"] =CameraActionIO;

                }
                else if(CameraNumber == 10)
                {
                    rtn = MCF_Sorting_Set_Blow_NG_Net(CameraPosition, CameraMotionDir, CameraActionMode, CameraActionIO, StationNumber);
                    xialiao_bangding["NG"] =CameraActionIO;

                }
                else if(CameraNumber == 11)
                {
                    rtn = MCF_Sorting_Set_Blow_1_Net(CameraPosition, CameraMotionDir, CameraActionMode, CameraActionIO, StationNumber);
                    xialiao_bangding["TBD1"] =CameraActionIO;

                }
                else if(CameraNumber == 12)
                {
                    rtn = MCF_Sorting_Set_Blow_2_Net(CameraPosition, CameraMotionDir, CameraActionMode, CameraActionIO, StationNumber);
                    xialiao_bangding["TBD2"] =CameraActionIO;
                }

            }

        }

    }
    return rtn;
}


void a_move_car::set_move_car_param()
{
    short rtn = 0;
    unsigned short StationNumber =  ui->comboBox_3->currentIndex();;

    //1 设置物体Size
    QString  xinghao = ui->comboBox_jizhong->currentText();
    int bankahao = ui->comboBox_3->currentIndex();
    int  zh_i = ui->comboBox_2->currentIndex();


    MaxSizePosition					=  m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].MaxSizePosition * Main_PulseEquivalent;

    MinSizePosition					=m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].MinSizePosition * Main_PulseEquivalent;
    rtn = MCF_Sorting_Set_Piece_Size_Net(MaxSizePosition, MinSizePosition, StationNumber);

    //1 设置两物体间隔

    Min_DistancePosition			= m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].Min_DistancePosition * Main_PulseEquivalent;

    Min_Time_Interval				= m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].Min_Time_Interval;
    rtn = MCF_Sorting_Set_Piece_Place_Net(Min_DistancePosition, Min_Time_Interval, StationNumber);
}
void a_move_car::on_pushButton_clicked()
{
    write_ini_file_g("./cfg");
    set_move_car_param();
    unsigned short StationNumber =   ui->comboBox_3->currentIndex();;


    for(int i = 1; i < 12; i++)
    {

        SetCameraFunction(i, StationNumber);
    }


}

//	开启线程 -- 线程开始后  再来启动轴转动；
void a_move_car::on_pushButton_kaiqishaixuan_clicked()
{

    short rtn = 0;
    unsigned short StationNumber =ui->comboBox_3->currentIndex();





    //1 开启筛选功能
    rtn = MCF_Sorting_Start_Net(StationNumber);

    chushihua_ok_ng_chuiqi();


}
#if 0
// 同时更新硬件数据,包括写入
void  a_move_car::write_ini_file(QString in_path)
{



    string path_ini="./cfg";
    if (_access(path_ini.c_str(), 0) == -1)
    {
        int i = _mkdir(path_ini.c_str());
    }


    QString path =in_path+"/"+ preject_name + ".ini";
    QSettings* configIni = new QSettings(path, QSettings::IniFormat);
    configIni->setValue("project/name", preject_name);

    //baocun_canshu.axis_index = ui->lineEdit_zhou->text().toInt();
    //configIni->setValue("project/axis_index",  baocun_canshu.axis_index);


    //baocun_canshu.zhanhao_index = ui->lineEdit_zhanhao->text().toInt();
    // configIni->setValue("project/zhanhao_index",   baocun_canshu.zhanhao_index );

    baocun_canshu.xiangji_shu = ui->lineEdit_xiangjishu->text().toInt();
    configIni->setValue("project/xiangji_shu",  baocun_canshu.xiangji_shu);


    baocun_canshu.m_tingzhi_quxian = ui->comB_tingzhiquxian->currentIndex();
    configIni->setValue("project/m_tingzhi_quxian",    baocun_canshu.m_tingzhi_quxian);

    baocun_canshu.weizhi_mode = ui->comB_weizhimoshi->currentIndex();
    configIni->setValue("project/weizhi_mode",  ui->comB_weizhimoshi->currentIndex());


    baocun_canshu.chushi_sudu = ui->lineEdit_chushihua_sudu->text().toInt();
    configIni->setValue("project/chushi_sudu",   baocun_canshu.chushi_sudu );


    baocun_canshu.yunxing_sud = ui->lineEdit_yunxing_sudu->text().toInt();
    configIni->setValue("project/yunxing_sud",  ui->lineEdit_yunxing_sudu->text().toDouble());

    baocun_canshu.jia_sudu = ui->lineEdit_jiasudu->text().toInt();
    configIni->setValue("project/jia_sudu",  ui->lineEdit_jiasudu->text().toDouble());

    baocun_canshu.jiajia_sudu = ui->lineEdit_jjsud->text().toInt();

    configIni->setValue("project/jiajia_sudu",  ui->lineEdit_jjsud->text().toDouble());

    baocun_canshu.jiesu_sudu = ui->lineEdit_jiesu_sud->text().toInt();
    configIni->setValue("project/jiesu_sudu",  ui->lineEdit_jiesu_sud->text().toDouble());

    baocun_canshu.zhuandongfangxia = ui->lineEdit_zhuandongfangxiang->text().toInt();
    configIni->setValue("project/zhuandongfangxia",  ui->lineEdit_zhuandongfangxiang->text().toInt());

    baocun_canshu.MaxSizePosition = ui->lineEdit_wuliao_zuidachicun->text().toInt();
    configIni->setValue("project/MaxSizePosition",  ui->lineEdit_wuliao_zuidachicun->text().toInt());

    baocun_canshu.MinSizePosition = ui->lineEdit_wuliao_zuixiaochicun->text().toInt();
    configIni->setValue("project/MinSizePosition",  ui->lineEdit_wuliao_zuixiaochicun->text().toInt());

    baocun_canshu.Min_DistancePosition = ui->lineEdit_wuliao_zuixiaojianju->text().toInt();
    configIni->setValue("project/Min_DistancePosition",  ui->lineEdit_wuliao_zuixiaojianju->text().toInt());


    baocun_canshu.Min_Time_Interval = ui->lineEdit_wuliao_zuixiao_sj_jiange->text().toInt();
    configIni->setValue("project/Min_Time_Interval",  ui->lineEdit_wuliao_zuixiao_sj_jiange->text().toInt());

    for(int i = 0; i < DEV_NUM; i++)
    {
        QString rootx =cols_xiangmu[i] +"/";

        rec_dev_infor  x =qstr_dev_unit_infor[cols_xiangmu[i]];
        configIni->setValue(rootx+QString::fromLocal8Bit("if_enable"), QString::number(x.if_enable));
        configIni->setValue(rootx+QString::fromLocal8Bit("shebei_weizhi"), QString::number(x.shebei_weizhi));
        configIni->setValue(rootx+QString::fromLocal8Bit("yundong_fangxia"), QString::number(x.yundong_fangxia));
        configIni->setValue(rootx+QString::fromLocal8Bit("shuchu_weihao"), QString::number(x.shuchu_weihao));
        configIni->setValue(rootx+QString::fromLocal8Bit("shuchu_dongzuo"), QString::number(x.shuchu_dongzuo));
        configIni->setValue(rootx+QString::fromLocal8Bit("yanshi"), QString::number(x.yanshi));
    }

    delete configIni;
}
#endif
//void  a_move_car::read_ini_file(QString in_path)
//{

//    QString path =in_path+"/"+ preject_name + ".ini";
//    if (_access(path.toStdString().c_str(), 0) == -1)
//    {
//        QMessageBox::information(NULL,  QObject::tr("Warning"), QObject::tr("NO_INI_FILE"),  QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
//        return;
//    }



//    QSettings* configIni = new QSettings(path, QSettings::IniFormat);
//    baocun_canshu.axis_index = configIni->value("project/axis_index").toString().toInt();
//    axis_index = baocun_canshu.axis_index;
//    baocun_canshu.zhanhao_index = configIni->value("project/zhanhao_index").toString().toInt();
//    baocun_canshu.xiangji_shu = configIni->value("project/xiangji_shu").toString().toInt();

//    baocun_canshu.m_tingzhi_quxian = configIni->value("project/m_tingzhi_quxian").toString().toInt();
//    baocun_canshu.weizhi_mode = configIni->value("project/weizhi_mode").toString().toInt();

//    m_tingzhi_quxian =  baocun_canshu.m_tingzhi_quxian;
//    weizhi_mode =  baocun_canshu.weizhi_mode;

//    baocun_canshu.chushi_sudu = configIni->value("project/chushi_sudu").toString().toInt();
//    baocun_canshu.yunxing_sud = configIni->value("project/yunxing_sud").toString().toInt();
//    baocun_canshu.jia_sudu = configIni->value("project/jia_sudu").toString().toInt();
//    baocun_canshu.jiajia_sudu = configIni->value("project/jiajia_sudu").toString().toInt();
//    baocun_canshu.jiesu_sudu = configIni->value("project/jiesu_sudu").toString().toInt();
//    baocun_canshu.zhuandongfangxia = configIni->value("project/zhuandongfangxia").toString().toInt();
//    baocun_canshu.MaxSizePosition = configIni->value("project/MaxSizePosition").toString().toInt();
//    baocun_canshu.MinSizePosition = configIni->value("project/MinSizePosition").toString().toInt();
//    baocun_canshu.Min_DistancePosition = configIni->value("project/Min_DistancePosition").toString().toInt();
//    baocun_canshu.Min_Time_Interval = configIni->value("project/Min_Time_Interval").toString().toInt();


//    for(int i = 0; i <  baocun_canshu.xiangji_shu; i++)
//    {
//        QString rootx =cols_xiangmu[i] +"/";

//        rec_dev_infor  x;

//        x.if_enable =  configIni->value(rootx+QString::fromLocal8Bit("if_enable")).toString().toInt();
//        x.shebei_weizhi =  configIni->value(rootx+QString::fromLocal8Bit("shebei_weizhi")).toString().toInt();
//        x.yundong_fangxia =  configIni->value(rootx+QString::fromLocal8Bit("yundong_fangxia")).toString().toInt();
//        x.shuchu_weihao =  configIni->value(rootx+QString::fromLocal8Bit("shuchu_weihao")).toString().toInt();
//        x.shuchu_dongzuo =  configIni->value(rootx+QString::fromLocal8Bit("shuchu_dongzuo")).toString().toInt();
//        x.yanshi =  configIni->value(rootx+QString::fromLocal8Bit("yanshi")).toString().toInt();

//        qstr_dev_unit_infor[cols_xiangmu[i]] = x;
//    }
//    delete configIni;

//    // baocun_canshu.axis_index = ui->lineEdit_zhou->text().toInt();

//    // ui->lineEdit_zhou->setText(QString::number( baocun_canshu.axis_index));
//    // ui->lineEdit_zhanhao->setText(QString::number( baocun_canshu.zhanhao_index));
//    ui->lineEdit_xiangjishu->setText(QString::number( baocun_canshu.xiangji_shu));
//    ui->comB_tingzhiquxian->setCurrentIndex(  baocun_canshu.m_tingzhi_quxian);
//    ui->comB_weizhimoshi->setCurrentIndex(  baocun_canshu.weizhi_mode);

//    ui->lineEdit_chushihua_sudu->setText(QString::number( baocun_canshu.chushi_sudu));
//    ui->lineEdit_yunxing_sudu->setText(QString::number( baocun_canshu.yunxing_sud));
//    ui->lineEdit_jiasudu->setText(QString::number( baocun_canshu.jia_sudu));
//    ui->lineEdit_jjsud->setText(QString::number( baocun_canshu.jiajia_sudu));
//    ui->lineEdit_jiesu_sud->setText(QString::number( baocun_canshu.jiesu_sudu));

//    ui->lineEdit_zhuandongfangxiang->setText(QString::number( baocun_canshu.zhuandongfangxia));


//    ui->lineEdit_wuliao_zuidachicun->setText(QString::number( baocun_canshu.MaxSizePosition));
//    ui->lineEdit_wuliao_zuixiaochicun->setText(QString::number( baocun_canshu.MinSizePosition));
//    ui->lineEdit_wuliao_zuixiaojianju->setText(QString::number( baocun_canshu.Min_DistancePosition));
//    ui->lineEdit_wuliao_zuixiao_sj_jiange->setText(QString::number( baocun_canshu.Min_Time_Interval));

//    //初始化表格
//    for(int i = 0; i <  baocun_canshu.xiangji_shu; i++)
//    {
//        rec_dev_infor  x = qstr_dev_unit_infor[cols_xiangmu[i]];


//        QString name_dev  =cols_xiangmu[i];
//        if(x.if_enable){
//            head_infor[name_dev].if_enable_checkBox ->setCheckState(  Qt::Checked);
//        }else
//        {
//            head_infor[name_dev].if_enable_checkBox ->setCheckState(  Qt::Unchecked);
//        }

//        //初始位置
//        ui->tableWidget->setItem(1, i,new QTableWidgetItem(QString::number(x.shebei_weizhi)));

//        //运动方向
//        head_infor[name_dev].combox_yundong_fangxia->setCurrentIndex(x.yundong_fangxia);
//        //输出位号
//        head_infor[name_dev].combox_shuchu_weihao->setCurrentIndex(x.shuchu_weihao);
//        //输出动作
//        head_infor[name_dev].combox_shuchu_dongzuo->setCurrentIndex(x.shuchu_dongzuo);

//        ui->tableWidget->setItem(5, i,new QTableWidgetItem(QString::number(x.yanshi)));

//    }
//}

void a_move_car::on_tableWidget_cellChanged(int row, int column)
{
    QString name = cols_xiangmu[column ];
    if (row == 0)
    {
        QTableWidgetItem* item = ui->tableWidget->item(row, column);
        if (item != NULL)
        {
            if (item->checkState() == Qt::Checked)
            {
                //qDebug() <<QString::fromLocal8Bit("选择") << row << " " << column;
                // 选中绿色
                item->setBackgroundColor(QColor(0x00, 0xFF, 0x00));

                qstr_dev_unit_infor[name].if_enable  =1;

                int_dev_unit_infor[column] = qstr_dev_unit_infor[name];

                //int_dev_unit_infor[i] = new_str;
                // struct_int_dev_unit_infor[new_str] = i;

            }
            else if (item->checkState() == Qt::Unchecked)
            {
                // 没选中白色
                // qDebug() <<QString::fromLocal8Bit(" 没有选择")<< row << " " << column;
                item->setBackgroundColor(QColor(0xFF, 0xFF, 0xFF));
                qstr_dev_unit_infor[name].if_enable  =0;
                int_dev_unit_infor[column] = qstr_dev_unit_infor[name];
            }

        }
    }




    if(xiangmu_rows[QString::fromLocal8Bit("延时")] ==row)
    {
        head_infor[name].yanshi = ui->tableWidget->item(row,column)->text().toInt();

        qstr_dev_unit_infor[name].yanshi  =ui->tableWidget->item(row,column)->text().toInt();

        int_dev_unit_infor[column] = qstr_dev_unit_infor[name];

    }

    if(xiangmu_rows[QString::fromLocal8Bit("设备位置")] ==row)
    {
        head_infor[name].yanshi = ui->tableWidget->item(row,column)->text().toInt();

        qstr_dev_unit_infor[name].shebei_weizhi  = ui->tableWidget->item(row,column)->text().toInt();
        int_dev_unit_infor[column] = qstr_dev_unit_infor[name];

    }
    // if(flag_data_ch ==1)  write_ini_file_g("./cfg");

}

void   a_move_car::clickCombobox_yundong_fx(QString text)
{
    QComboBox *comBox_ = dynamic_cast<QComboBox*>(this->sender());

    if(NULL == comBox_)
    {
        return;
    }
    int x = comBox_->frameGeometry().x();
    int y = comBox_->frameGeometry().y();
    QModelIndex index = ui->tableWidget->indexAt(QPoint(x, y));
    int row = index.row();
    int column = index.column();
    //  qDebug() << " " << row << " " << column;
    // qDebug() << " " << text;

    int index_int = comBox_->currentIndex();
    QString name = cols_xiangmu[column ];
    qstr_dev_unit_infor[name].yundong_fangxia  =index_int;
    int_dev_unit_infor[column] = qstr_dev_unit_infor[name];
    //if(flag_data_ch ==1)  write_ini_file("./cfg");
}
void   a_move_car::clickCombobox_shuchu_wh(QString text)
{
    QComboBox *comBox_ = dynamic_cast<QComboBox*>(this->sender());
    if(NULL == comBox_)
    {
        return;
    }
    int x = comBox_->frameGeometry().x();
    int y = comBox_->frameGeometry().y();
    QModelIndex index = ui->tableWidget->indexAt(QPoint(x, y));
    int row = index.row();
    int column = index.column();
    //qDebug() << " " << row << " " << column;
    //qDebug() << " " << text;

    int index_int = comBox_->currentIndex();
    QString name = cols_xiangmu[column ];
    qstr_dev_unit_infor[name].shuchu_weihao  =index_int;
    int_dev_unit_infor[column] = qstr_dev_unit_infor[name];

    // if(flag_data_ch ==1)  write_ini_file("./cfg");
}
void   a_move_car::clickCombobox_shuchu_dongzuo(QString text)
{
    QComboBox *comBox_ = dynamic_cast<QComboBox*>(this->sender());
    if(NULL == comBox_)
    {
        return;
    }
    int x = comBox_->frameGeometry().x();
    int y = comBox_->frameGeometry().y();
    QModelIndex index = ui->tableWidget->indexAt(QPoint(x, y));
    int row = index.row();
    int column = index.column();
    // qDebug() << " " << row << " " << column;
    // qDebug() << " " << text;

    int index_int = comBox_->currentIndex();
    QString name = cols_xiangmu[column ];
    qstr_dev_unit_infor[name].shuchu_dongzuo  =index_int;
    int_dev_unit_infor[column] = qstr_dev_unit_infor[name];

    // if(flag_data_ch ==1)  write_ini_file("./cfg");
}

void a_move_car::go_position(int dDist,int flag_dir)
{
    if(shineng_state_flag)
    {

        int axis_index = ui->comboBox_2->currentIndex();

        double  Vini = ui->lineEdit_chushihua_sudu->text().toDouble();  //  '��ʼ�ٶ�
        double Vmax = ui->lineEdit_yunxing_sudu->text().toDouble();//   '�ٶ�
        double Vadd = ui->lineEdit_jiasudu->text().toDouble();//   '���ٶ�
        double Vjerk = ui->lineEdit_jjsud->text().toDouble();//   '�Ӽ��ٶ�
        double Vend = ui->lineEdit_jiesu_sud->text().toDouble();//   '�����ٶ�
        short Profile = ui->comB_tingzhiquxian->currentIndex();
        short Position = ui->comB_weizhimoshi->currentIndex();//   '���� ����
        //int  dDist = ui->lineEdit_mubiao_weizhi->text().toInt();//

        rtn = MCF_Set_Axis_Profile_Net(axis_index, Vini, Vmax, Vadd, Vjerk, Vend, Profile, 0);
        rtn = MCF_Uniaxial_Net(axis_index, dDist*flag_dir, Position, 0);
        int if_daowei = 0;
        do {

            long pos;
            rtn = MCF_Get_Position_Net(axis_index, &pos, 0);
            if(pos ==dDist) if_daowei =1;
        }while(!if_daowei);

    }
}

a_move_car::~a_move_car()
{
    jiankong_flag = 1;
    delete ui;
}

void a_move_car::Get_Camera_Blow_TriCount( void * lparm)
{


    a_move_car *pDlg = (a_move_car *)lparm;
    short rtn = 0;
    unsigned short StationNumber = 0;
    short i = 0;
    unsigned long Input_Count_Fall = 0;
    unsigned long Lock_Data_Buffer[10] = {0};
    while( !jiankong_flag)
    {
        StationNumber = STATION_0;
        if(CameraBlowTriFuncNum == 0)
            break;
        else if(CameraBlowTriFuncNum == 1)
        {
            for(i = 0; i < Main_CameraNumber; i++)
            {
                rtn = MCF_Sorting_Get_Trig_Camera_Count_Net(i+1, &Trig_Camera_Count[i], StationNumber);
            }
            rtn = MCF_Sorting_Get_Trig_Blow_NG_Count_Net(&Trig_Blow_NG_Count, StationNumber);
            rtn = MCF_Sorting_Get_Trig_Blow_OK_Count_Net(&Trig_Blow_OK_Count, StationNumber);
            rtn = MCF_Sorting_Get_Piece_State_Net(Piece_Input_Number, &Piece_Find, &Piece_Size[0], &Piece_Distance_To_nextp[0], &Piece_Cross_Camera, StationNumber);
            rtn = MCF_Sorting_Get_Trig_Blow_1_Count_Net(&Trig_Blow_1_Count, StationNumber);
            rtn = MCF_Sorting_Get_Trig_Blow_2_Count_Net(&Trig_Blow_2_Count, StationNumber);

            rtn = MCF_Get_Encoder_Net(AXIE_INDEX_0, &Encoder, StationNumber);
            rtn = MCF_Get_Position_Net(AXIE_INDEX_0, &Position, StationNumber);

            MCF_Get_Input_Fall_Count_Bit_Net         (AXIE_INDEX_0, &Input_Count_Fall, &Lock_Data_Buffer[0], StationNumber);

            MCF_Sorting_Get_Piece_Pass_Net(Piece_Input_Number, &Piece_Pass, StationNumber);

            double Vel1[1] = { 0 };
            double Vel2[1] = { 0 };
            rtn = MCF_Get_Vel_Net(AXIE_INDEX_0, Vel1, Vel2, StationNumber);

            dangqian_sudu = Vel1[0];


            ui->lineEdit_dangqian_weizhi->setText(QString::number(Position));
            ui->lineEdit_dangqian_bianmaq->setText(QString::number(Encoder));
            ui->lineEdit_dangqian_sudu->setText(QString::number(Vel1[0]));


        }
        Sleep(10);
    }
    //return 0;
}

void a_move_car::on_ps_open_car_clicked()
{

    // axis_index = ui->lineEdit_zhou->text().toInt();
    // zhanhao_index = ui->lineEdit_zhanhao->text().toInt();
    Main_CameraNumber = ui->lineEdit_xiangjishu->text().toInt();
    cout << "start open move control car" << endl;
    //MCF_Open_Input_Lock_Bit_Net(0, 0);

    //打开IO触发
    MCF_Sorting_Init_Net(STATION_0);

    rtn = MCF_Open_Net(Connection_Number, &Station_Number[0], &Station_Type[0]);		//�����˶����ƿ�

    if(rtn != 0)
    {
        cout << "fail open move control car" << endl;
    }
    else
    {

        ui->ps_open_car->setStyleSheet("background-color:rgb(0,255,0)");
        ui->ps_close_car->setStyleSheet("background-color:rgb(255,255,255)");

        //zhuandongfangxia = ui->lineEdit_zhuandongfangxiang->text().toInt();


        int bankahao = ui->comboBox_3->currentIndex();



        int coutzhou = ui->comboBox_2->count();


        for(int i = 0; i < coutzhou; i++)
        {
            QString xinghao =ui->comboBox_jizhong->currentText();

            MCF_Set_Pulse_Mode_Net(i,  m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[i].zhuandongfangxia,bankahao);


        }


        //打开采集数据的线程
        if(CameraBlowTriState == false)
        {
            CameraBlowTriState = true;
            CameraBlowTriFuncNum = 1;
            thread_jiankong_state = std::thread(&a_move_car::Get_Camera_Blow_TriCount, this,this);
            thread_jiankong_state.detach();



            //get input io state
            Return_Get_Input_Temp = 0;
            thread_get_input_state = std::thread(&a_move_car::thred_Get_Input, this,this);
            thread_get_input_state.detach();

            //监控急停按钮事件
            std::thread  jiankong_jiting_t1(&a_move_car::jiankong_jiting_yinganniu_thred_thred, this,this);
            jiankong_jiting_t1.detach();

            //监控启动按钮
            std::thread  jiankongqidongt_t1(&a_move_car::jiankong_qidong_yinganniu_thred, this,this);
            jiankongqidongt_t1.detach();

            //监控停止按钮
            std::thread  jiankong_tingzhi_t1(&a_move_car::jiankong_tingzhi_yinganniu_thred, this,this);
            jiankong_tingzhi_t1.detach();


            //设置回来 紧急停止等参数函数
            set_car_huiling_xianwei();
        }
    }

}
//急停  回零限位 设置
void a_move_car::set_car_huiling_xianwei()
{
    //STATION_1


    unsigned long TriggerMode = 0;//脉冲方向

    for(int i = 0; i < num_axie; i++)
    {

        TriggerMode = 0;
        MCF_Set_Pulse_Mode_Net(i, TriggerMode, STATION_0); //脉冲方向0

        //TriggerMode = 1;
        //MCF_Set_ELP_Trigger_Net(i, TriggerMode, STATION_0);//正限位

        // TriggerMode = 1;
        // MCF_Set_ELN_Trigger_Net(i, TriggerMode, STATION_0);//负限位

        // TriggerMode = 0;
        // MCF_Set_Home_Trigger_Net(i, TriggerMode, STATION_0);//HOME不触发；

        TriggerMode = 0;
        MCF_Set_Index_Trigger_Net(i, TriggerMode, STATION_0);//index不触发；

        //TriggerMode = 2;
        //MCF_Set_Alarm_Trigger_Net(i, TriggerMode, STATION_0);//Alarm高电平触发紧急停止
    }
    int jiting_input_id = ui->lineEdit_jiting_hao->text().toInt();

    MCF_Set_EMG_Bit_Net(jiting_input_id, EMG_Trigger_Low_IMD, STATION_0 );//EMG_Trigger_Low_IMD EMG_Trigger_High_IMD

    on_pb_shineng_kai_clicked();
    //on_pb_shineng_kai_clicked();
    //on_pb_shineng_kai_all_clicked();

    // MCF_Set_Compare_Config_Net(AXIE_INDEX_0, 1, 1,STATION_0);
    // MCF_Set_Compare_Config_Net(AXIE_INDEX_0, 1, 1,STATION_0);    //    '打开位置比较功能     0-位置比较 1-编码器比较


    //    for(short i = 0; i < 12; i++)
    //    {
    //        MCF_Clear_Compare_Points_Net(i, STATION_0);
    //        Sleep(1);
    //    }


}

void  a_move_car::thred_Get_Input( void * lparm)
{
    a_move_car *pDlg = (a_move_car *)lparm;

    unsigned short Input;
    int k = 0;


    while( !jiankong_flag)
    {
LABEL_GOTO_1:
        k =0;
        if(Return_Get_Input_Temp == 1) return;	//退出线程
        for(int i = 0; i < 8; i++)
        {
            for(int j = 0; j < 6; j++)
            {

                rtn =MCF_Set_Input_Filter_Time_Bit_Net(k,yuandian_zankongbi_shijian, STATION_0);


                short re =  MCF_Get_Input_Bit_Net(k, &Input, STATION_0);

                QTableWidgetItem* item = ui->tableWidget_2->item(  i, j);
                if (item != NULL)
                {

                    if(Input == 0)
                    {

                        item->setBackgroundColor(QColor(0xFF, 0, 0));

                    }else{




                        item->setBackgroundColor(QColor(0x00, 0xFF, 0x00));
                    }


                    ui->tableWidget_2->viewport()->update();
                    qApp->processEvents();
                }

                k++;
                if(k==ui->lineEdit_shurudian->text().toInt())
                {
                    goto   LABEL_GOTO_1;
                }

                xSleep(15);

            }

        }
    }
}

//急停
void a_move_car::jiankong_jiting_yinganniu_thred_thred(void * ptm)
{
    ushort Input6;
    do{

        int io_jiting = ui->lineEdit_jiting_hao->text().toInt();
        MCF_Get_Input_Bit_Net(io_jiting, &Input6, STATION_0);
        int  zhoushu = ui->comboBox_2->count();

        if(0 == Input6)
        {
            if(jiankong_jiting_yinganniu_thred_flag == 0)
            {


                int io_hongdeng = ui->lineEdit_out_hongdeng->text().toInt();
                MCF_Set_Output_Bit_Net(io_hongdeng , Bit_Output_Close, STATION_0);
                jiankong_jiting_yinganniu_thred_flag = 1;

                //on_pb_tingzhizidongquliao_clicked();
                //on_pb_jiting_zong_clicked();

                int hongdeng_io=ui->lineEdit_out_hongdeng->text().toInt();
                MCF_Set_Output_Bit_Net(hongdeng_io , Bit_Output_Close, STATION_0);

                for(int i = 0; i < zhoushu;i++)
                {
                    rtn = MCF_Axis_Stop_Net (i,Axis_Stop_IMD, STATION_0);
                }



            }
        }else
        {

            if(jiankong_jiting_yinganniu_thred_flag == 1)
            {
                int io_hongdeng = ui->lineEdit_out_hongdeng->text().toInt();
                MCF_Set_Output_Bit_Net(io_hongdeng , Bit_Output_Open, STATION_0);

                jiankong_jiting_yinganniu_thred_flag = 0;

            }

        }
        Sleep(30);
    }  while ( !jiankong_flag);

}


//启动
void a_move_car::jiankong_qidong_yinganniu_thred(void *ptm)
{
    ushort Input6 = 1;
    do{

        int io_hao = ui->lineEdit_qidong_hao->text().toInt();
        short re = MCF_Get_Input_Bit_Net(io_hao, &Input6, STATION_0);

        if(re == 0)
        {
            if(0 == Input6)
            {
                if(jiankong_qidong_yinganniu_thred_flag == 0)
                {
                    jiankong_qidong_yinganniu_thred_flag = 1;
                    jiankong_tingzhi_yinganniu_thred_flag = 0;
                    //on_pb_zidongquliao_clicked();
                    on_pb_start_zhuandong_clicked();
                    cout<<"---------------------------------------------"<<endl;



                }
            }else {

                if(jiankong_qidong_yinganniu_thred_flag == 1)
                {


                    jiankong_qidong_yinganniu_thred_flag = 0;

                }

            }
        }
        Sleep(50);
    }while( !jiankong_flag );
}

//停止
void a_move_car::jiankong_tingzhi_yinganniu_thred(void * ptm)
{


    ushort Input6 = 1;
    do{
        int io_index = ui->lineEdit_tingzhi_hao->text().toInt();
        short re = MCF_Get_Input_Bit_Net(io_index, &Input6, STATION_0);

        if(re == 0)
        {

            if(0 == Input6)
            {
                if(jiankong_tingzhi_yinganniu_thred_flag == 0)
                {

                    // on_pb_tingzhizidongquliao_clicked();
                    on_pb_start_zhuandong_tingzhi_clicked();
                    jiankong_tingzhi_yinganniu_thred_flag = 1;

                    jiankong_qidong_yinganniu_thred_flag =0;

                }
            }else {

                if(jiankong_tingzhi_yinganniu_thred_flag == 1)
                {


                    jiankong_tingzhi_yinganniu_thred_flag = 0;

                }

            }
        }

        Sleep(50);
    }while( !jiankong_flag );


}
void a_move_car::xSleep(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}




void a_move_car::gei_main_diaoyong_qidong()
{
    on_pb_start_zhuandong_clicked();
}


void a_move_car::gei_main_diaoyong_tingzhi()
{
    on_pb_start_zhuandong_tingzhi_clicked();
}


void a_move_car::jiankong_qidong_ruan_anniu_thred(void *)
{


    rtn = MCF_Sorting_Get_Piece_State_Net(Piece_Input_Number, &Piece_Find, &Piece_Size[0],  &Piece_Distance_To_nextp[0],  &Piece_Cross_Camera, StationNumber);
    // cout<<"-------------:"<<Virtual_Piece_Cross_Camera<<" Piece_Cross_Camera:"<<Piece_Cross_Camera<<endl;

    if(Piece_Cross_Camera>0)
    {
        on_pb_start_zhuandong_tingzhi_2_clicked();
    }



    int hongdeng_io=ui->lineEdit_out_hongdeng->text().toInt();
    MCF_Set_Output_Bit_Net(hongdeng_io , Bit_Output_Open, STATION_0);


    int lvdeng_io=ui->lineEdit_out_lvdeng->text().toInt();
    MCF_Set_Output_Bit_Net(lvdeng_io , Bit_Output_Close, STATION_0);

    int huangdeng_io=ui->lineEdit_out_huangdeng->text().toInt();
    MCF_Set_Output_Bit_Net(huangdeng_io , Bit_Output_Open, STATION_0);

    //先更新筛选参数
    on_pushButton_clicked();
    xSleep(100);

    //开启筛选功能
    on_pushButton_kaiqishaixuan_clicked();
    xSleep(100);


    short rtn = 0;

    unsigned short StationNumber = ui->comboBox_3->currentIndex();

    double Vmax = ui->lineEdit_yunxing_sudu->text().toDouble();//   '�ٶ�
    double Vadd = ui->lineEdit_jiasudu->text().toDouble();//   '���ٶ�


    int zhuandongfangxia = ui->lineEdit_zhuandongfangxiang->text().toInt();

    int axis_index = ui->comboBox_2->currentIndex();

    MCF_Set_Pulse_Mode_Net(axis_index, zhuandongfangxia);
    rtn = MCF_JOG_Net(axis_index, Vmax, Vadd, StationNumber);

    Sleep(1500);
}

void a_move_car::on_pb_start_zhuandong_clicked()
{

    std::thread t1(&a_move_car::jiankong_qidong_ruan_anniu_thred,this,this);
    t1.detach();


    int zhendongpanchuiq= 9;
    MCF_Set_Output_Bit_Net(zhendongpanchuiq , Bit_Output_Close, STATION_0);


    int zhendongpan= ui->lineEdit_out_zhizhen->text().toInt();
    MCF_Set_Output_Bit_Net(zhendongpan , Bit_Output_Close, STATION_0);


    int zhizhen= ui->lineEdit_out_zhizhen->text().toInt();
    MCF_Set_Output_Bit_Net(zhizhen , Bit_Output_Close, STATION_0);

}

void a_move_car::jiankong_tingzhi_ruan_anniu_thred(void *)
{
    float  x = yanchangjimiao() * 1000;

    int y = cvRound(x);

    Sleep(y);

    //先关闭筛选功能
    on_pushButton_guanbishaixuan_clicked();
    xSleep(100);

    int hongdeng_io=ui->lineEdit_out_hongdeng->text().toInt();
    MCF_Set_Output_Bit_Net(hongdeng_io , Bit_Output_Open, STATION_0);


    int lvdeng_io=ui->lineEdit_out_lvdeng->text().toInt();
    MCF_Set_Output_Bit_Net(lvdeng_io , Bit_Output_Open, STATION_0);

    int huangdeng_io=ui->lineEdit_out_huangdeng->text().toInt();
    MCF_Set_Output_Bit_Net(huangdeng_io , Bit_Output_Close, STATION_0);


    short rtn;

    unsigned short StationNumber = ui->comboBox_3->currentIndex();
    // double Vmax = ui->lineEdit_yunxing_sudu->text().toDouble();//   '�ٶ�
    double Vadd = ui->lineEdit_jiasudu->text().toDouble();//   '���ٶ�

    rtn = MCF_Set_Axis_Stop_Profile_Net(0, Vadd, 0, 0, StationNumber);
    rtn = MCF_Axis_Stop_Net(0, 1, StationNumber);

}
void a_move_car::on_pb_start_zhuandong_tingzhi_clicked()
{
    int zhendongpan= ui->lineEdit_out_zhizhen->text().toInt();
    MCF_Set_Output_Bit_Net(zhendongpan , Bit_Output_Open, STATION_0);


    int zhizhen= ui->lineEdit_out_zhizhen->text().toInt();
    MCF_Set_Output_Bit_Net(zhizhen , Bit_Output_Open, STATION_0);



    int zhendongpanchuiq= 9;
    MCF_Set_Output_Bit_Net(zhendongpanchuiq , Bit_Output_Open, STATION_0);


    std::thread t1(&a_move_car::jiankong_tingzhi_ruan_anniu_thred,this,this);
    t1.detach();



}

void  a_move_car::chulijieguo_hanshu_vst(int flag_ng_ok, std::vector< tr_s_resultinfo> input_vst , int paizhaocshu)
{

    {
        chulicishu_temp = 0;

        rtn = MCF_Sorting_Get_Piece_State_Net(Piece_Input_Number, &Piece_Find, &Piece_Size[0],  &Piece_Distance_To_nextp[0],  &Piece_Cross_Camera, StationNumber);
        //cout<<"-------------:"<<Virtual_Piece_Cross_Camera<<" Piece_Cross_Camera:"<<Piece_Cross_Camera<<endl;
        duoshaogewuliao = Virtual_Piece_Cross_Camera ;

        if( 2==flag_ng_ok)
        {
            rtn = MCF_Sorting_Set_Trig_Blow_NG_Net (Virtual_Piece_Cross_Camera, StationNumber);
            m_jljg[duoshaogewuliao].flag_ng_ok ==NG_CODE_RESULT;
        } else if( 1==flag_ng_ok)
        {
            rtn = MCF_Sorting_Set_Trig_Blow_OK_Net (Virtual_Piece_Cross_Camera, StationNumber);

            m_jljg[duoshaogewuliao].flag_ng_ok ==OK_CODE_RESULT;
        }
        Virtual_Piece_Cross_Camera++;
    }

    

    return;

}

//void  a_move_car::chulijieguo_hanshu_vst(int flag_ng_ok, std::vector< tr_s_resultinfo> input_vst , int paizhaocshu)
//{
//    chulicishu_temp++;

//    vct_result.push_back(input_vst);
//    vct_result_int.push_back(flag_ng_ok);
//    if(chulicishu_temp == paizhaocshu)
//    {
//        chulicishu_temp = 0;

//        rtn = MCF_Sorting_Get_Piece_State_Net(Piece_Input_Number, &Piece_Find, &Piece_Size[0],  &Piece_Distance_To_nextp[0],  &Piece_Cross_Camera, StationNumber);



//        int tongji_ng_shu = 0;

//        int chuliaokou_1 = 0;
//        int chuliaokou_2 = 0;
//        int chuliaokou_3 = 0;
//        int chuliaokou_4 = 0;


//        duoshaogewuliao = Virtual_Piece_Cross_Camera ;

//        for (int k = 0; k <vct_result_int.size();k++  ) {

//            if(vct_result_int[k]==2)
//            {
//                tongji_ng_shu++;


//                for(int i = 0; i <vct_result[k].size();i++ )
//                {

//                    if(input_vst[i].chuliaokou ==2)//有属于最高级别NG的,直接跳出
//                    {

//                        chuliaokou_2 = 1;
//                        break;
//                    }

//                    if(input_vst[i].chuliaokou ==3 )
//                    {

//                        chuliaokou_3++;
//                    }

//                    if(input_vst[i].chuliaokou ==4 )
//                    {

//                        chuliaokou_4++;
//                    }

//                }

//            }

//            if(chuliaokou_2 == 1) break;

//        }



//        if(tongji_ng_shu > 0){


//            if(chuliaokou_2 == 1)
//            {

//                rtn = MCF_Sorting_Set_Trig_Blow_NG_Net (Virtual_Piece_Cross_Camera, StationNumber);

//            }else if( tongji_ng_shu == chuliaokou_3)
//            {
//                rtn = MCF_Sorting_Set_Trig_Blow_1_Net (Virtual_Piece_Cross_Camera, StationNumber);
//            }else if( tongji_ng_shu == chuliaokou_4)
//            {
//                rtn = MCF_Sorting_Set_Trig_Blow_2_Net (Virtual_Piece_Cross_Camera, StationNumber);
//            }else if(tongji_ng_shu==(chuliaokou_3 +chuliaokou_4 ))
//            {
//                if(chuliaokou_3 >chuliaokou_4 )
//                {
//                    rtn = MCF_Sorting_Set_Trig_Blow_1_Net (Virtual_Piece_Cross_Camera, StationNumber);
//                }else
//                {
//                    rtn = MCF_Sorting_Set_Trig_Blow_2_Net (Virtual_Piece_Cross_Camera, StationNumber);
//                }
//            }

//            m_jljg[duoshaogewuliao].flag_ng_ok ==NG_CODE_RESULT;

//        }else
//        {
//            rtn = MCF_Sorting_Set_Trig_Blow_OK_Net (Virtual_Piece_Cross_Camera, StationNumber);

//            m_jljg[duoshaogewuliao].flag_ng_ok ==OK_CODE_RESULT;

//        }
//        vct_result.clear();
//        vct_result_int.clear();

//        // chuli_jieguo_tongj(this);
//        Virtual_Piece_Cross_Camera++;
//    }

//}
void a_move_car::chuli_jieguo_tongj(void *ptm)
{
    a_move_car * ptx= (a_move_car *)ptm;
    if(m_jljg.size() > 0)
    {


        int zongshu = 0;
        int liangpshu = 0;
        int buliangshu = 0;
        float lplv = 0;
        float bllv =0;
        int ng_ok_state = 0;
        for(map<int ,jieguo_tongji >::iterator iter=ptx->m_jljg.begin() ;iter!=ptx->m_jljg.end();)
        {
            //cout<<it->first<<" occurs "<<it->second<<" times"<<endl;
            int un_id =  iter->first;
            if(ptx->m_jljg[un_id].flag_ng_ok ==NG_CODE_RESULT) buliangshu++;
            if(ptx->m_jljg[un_id].flag_ng_ok ==OK_CODE_RESULT) liangpshu++;

            zongshu++;
            ng_ok_state = ptx->m_jljg[un_id].flag_ng_ok;
            iter++;

        }

        lplv = (float)buliangshu/(float)zongshu;
        bllv = (float)buliangshu/(float)zongshu;

        Fanhuijieguo obj;
        obj.zongshu =zongshu;
        obj.ok_shu = liangpshu;
        obj.ng_shu =buliangshu;
        obj.lianglv =lplv;
        obj.state  =ng_ok_state;

        emit  ptx->send_jieguo_to_main(obj);
    }

}



void a_move_car::operator_result(int NGSignalNumber)
{


    rtn = MCF_Sorting_Get_Piece_State_Net(Piece_Input_Number,
                                          &Piece_Find,
                                          &Piece_Size[0],
            &Piece_Distance_To_nextp[0],
            &Piece_Cross_Camera, StationNumber);

    if(NGSignalNumber == 1)
    {
        rtn = MCF_Sorting_Set_Trig_Blow_OK_Net  (Piece_Cross_Camera, StationNumber);

    }
    else if(NGSignalNumber == 2)
    {
        rtn = MCF_Sorting_Set_Trig_Blow_NG_Net (Piece_Cross_Camera, StationNumber);

    }
    else if(NGSignalNumber == 3)
    {
        rtn = MCF_Sorting_Set_Trig_Blow_1_Net (Piece_Cross_Camera, StationNumber);

    }
    else if(NGSignalNumber == 4)
    {
        rtn = MCF_Sorting_Set_Trig_Blow_2_Net (Piece_Cross_Camera, StationNumber);

    }
    // Virtual_Piece_Cross_Camera++;


}



void a_move_car::on_pushButton_guanbishaixuan_clicked()
{
    //关闭筛选功能

    Virtual_Piece_Cross_Camera = 1;

    short rtn = 0;
    unsigned short StationNumber = ui->comboBox_3->currentIndex();

    rtn = MCF_Clear_Input_Fall_Bit_Net(0, StationNumber);


    MCF_Clear_Compare_Points_Net             (AXIE_INDEX_0);
    MCF_Clear_Compare_Current_Points_Net     (AXIE_INDEX_0);

    rtn = MCF_Sorting_Close_Net(StationNumber);
}

void a_move_car::on_ps_close_car_clicked()
{


    ui->ps_open_car->setStyleSheet("background-color:rgb(255,255,255)");
    ui->ps_close_car->setStyleSheet("background-color:rgb(0,255,0)");

    Return_Get_Input_Temp = 1;
    CameraBlowTriFuncNum = 0;
    CameraBlowTriState = false;
    // if_open_car = 0;
    /// ///

    rtn = 0;
    rtn = MCF_Close_Net();
}

void a_move_car::on_ps_bianmqinglng_clicked()
{

    QMessageBox::information(NULL,  QObject::tr("warning"), QObject::tr("clear"),  QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    //MCF_Set_Encoder_Net(axis_index, 0,StationNumber);
    //MCF_Set_Position_Net(axis_index, 0,StationNumber);

    for(int i = 0; i < AXIS_COUNT_G; i++)
    {
        MCF_Set_Encoder_Net(i, 0, STATION_0);
    }


    for(int i = 0; i < AXIS_COUNT_G; i++)
    {

        MCF_Set_Position_Net(i, 0, STATION_0);
    }


    //Sleep(30);


    rtn = MCF_Get_Encoder_Net(AXIE_INDEX_0, &Encoder, STATION_0);
    rtn = MCF_Get_Position_Net(AXIE_INDEX_0, &Position, STATION_0);


    ui->lineEdit_dangqian_weizhi->setText(QString::number(Position));
    ui->lineEdit_dangqian_bianmaq->setText(QString::number(Encoder));


}

void a_move_car::on_pb_shineng_guan_clicked()
{

    if(shineng_state_flag ==1)
    {
        int bankahao = ui->comboBox_3->currentIndex();
        int axis_index = ui->comboBox_2->currentIndex();
        MCF_Set_Servo_Enable_Net(AXIE_INDEX_0, 1,bankahao);

        shineng_state_flag = 0;
        ui->pb_shineng_kai->setStyleSheet("background-color:rgb(255,255,255)");
        ui->pb_shineng_guan->setStyleSheet("background-color:rgb(0,255,0)");
    }
}
void a_move_car::on_pb_shineng_kai_clicked()
{
    if(CameraBlowTriState== true)
    {
        // axis_index = ui->lineEdit_zhou->text().toInt();

        int bankahao = ui->comboBox_3->currentIndex();


        int coutzhou = ui->comboBox_2->count();

        for(int i = 0; i < coutzhou; i++)
        {
            MCF_Set_Servo_Enable_Net(i, 0, bankahao);


        }


        shineng_state_flag = 1;
        ui->pb_shineng_kai->setStyleSheet("background-color:rgb(0,255,0)");
        ui->pb_shineng_guan->setStyleSheet("background-color:rgb(255,255,255)");
    }




}




void a_move_car::on_Stop1_clicked()
{

    //axis_index = ui->lineEdit_zhou->text().toInt();

    int bankahao = ui->comboBox_3->currentIndex();
    // axis_index = ui->comboBox_2->currentIndex();



    double Vadd = ui->lineEdit_jiasudu->text().toDouble();//   '���ٶ�
    double Vjerk = ui->lineEdit_jjsud->text().toDouble();//   '�Ӽ��ٶ�

    short Profile = ui->comB_tingzhiquxian->currentIndex();

    m_Vadd = Vadd;//   '���ٶ�
    m_Vjerk = Vjerk;//   '�Ӽ��ٶ�

    m_Profile = Profile;

    int coutzhou = ui->comboBox_2->count();


    for(int i = 0; i < coutzhou; i++)
    {
        rtn = MCF_Set_Axis_Stop_Profile_Net(AXIE_INDEX_0, Vadd, Vjerk, Profile, bankahao);
        rtn = MCF_Axis_Stop_Net(i, 0, 0);


    }



    int hongdeng_io=ui->lineEdit_out_hongdeng->text().toInt();
    MCF_Set_Output_Bit_Net(hongdeng_io , Bit_Output_Close, bankahao);


    int lvdeng_io=ui->lineEdit_out_lvdeng->text().toInt();
    MCF_Set_Output_Bit_Net(lvdeng_io , Bit_Output_Open, bankahao);

    int huangdeng_io=ui->lineEdit_out_huangdeng->text().toInt();
    MCF_Set_Output_Bit_Net(huangdeng_io , Bit_Output_Open, bankahao);


}

void a_move_car::on_stop_man_clicked()
{
    //axis_index = ui->lineEdit_zhou->text().toInt();
    double Vadd = ui->lineEdit_jiasudu->text().toDouble();//   '���ٶ�
    double Vjerk = ui->lineEdit_jjsud->text().toDouble();//   '�Ӽ��ٶ�

    short Profile = ui->comB_tingzhiquxian->currentIndex();

    int coutzhou = ui->comboBox_2->count();


    for(int i = 0; i < coutzhou; i++)
    {
        rtn = MCF_Set_Axis_Stop_Profile_Net(i, Vadd, Vjerk, Profile, 0);
        rtn = MCF_Axis_Stop_Net(i, 1, 0);

    }




    int hongdeng_io=ui->lineEdit_out_hongdeng->text().toInt();
    MCF_Set_Output_Bit_Net(hongdeng_io , Bit_Output_Open, STATION_0);


    int lvdeng_io=ui->lineEdit_out_lvdeng->text().toInt();
    MCF_Set_Output_Bit_Net(lvdeng_io , Bit_Output_Open, STATION_0);

    int huangdeng_io=ui->lineEdit_out_huangdeng->text().toInt();
    MCF_Set_Output_Bit_Net(huangdeng_io , Bit_Output_Close, STATION_0);



}




void a_move_car::on_tableWidget_cellClicked(int row, int column)
{
    current_row = row;
    current_column = column;
}


void a_move_car::on_lineEdit_zhou_textChanged(const QString &arg1)
{
    // if(flag_data_ch ==1)  write_ini_file("./cfg");
}

void a_move_car::on_lineEdit_zhanhao_textChanged(const QString &arg1)
{
    //if(flag_data_ch ==1)  write_ini_file("./cfg");
}

void a_move_car::on_lineEdit_xiangjishu_textChanged(const QString &arg1)
{
    //if(flag_data_ch ==1)  write_ini_file("./cfg");
}



void a_move_car::on_comB_weizhimoshi_currentIndexChanged(int index)
{
    //if(flag_data_ch ==1)  write_ini_file("./cfg");
}

void a_move_car::on_comB_tingzhiquxian_currentIndexChanged(int index)
{
    //if(flag_data_ch ==1)  write_ini_file("./cfg");
}

void a_move_car::on_lineEdit_chushihua_sudu_textChanged(const QString &arg1)
{
    //if(flag_data_ch ==1)  write_ini_file("./cfg");
}

void a_move_car::on_lineEdit_yunxing_sudu_textChanged(const QString &arg1)
{
    //if(flag_data_ch ==1)  write_ini_file("./cfg");
}

void a_move_car::on_lineEdit_jiasudu_textChanged(const QString &arg1)
{
    //if(flag_data_ch ==1)  write_ini_file("./cfg");
}

void a_move_car::on_lineEdit_jjsud_textChanged(const QString &arg1)
{
    // if(flag_data_ch ==1)  write_ini_file("./cfg");
}

void a_move_car::on_lineEdit_jiesu_sud_textChanged(const QString &arg1)
{
    //if(flag_data_ch ==1)  write_ini_file("./cfg");
}

void a_move_car::on_lineEdit_zhuandongfangxiang_textChanged(const QString &arg1)
{
    //if(flag_data_ch ==1)  write_ini_file("./cfg");
}

void a_move_car::on_lineEdit_wuliao_zuidachicun_textChanged(const QString &arg1)
{
    //if(flag_data_ch ==1)  write_ini_file("./cfg");
}

void a_move_car::on_lineEdit_wuliao_zuixiaochicun_textChanged(const QString &arg1)
{
    //if(flag_data_ch ==1)  write_ini_file("./cfg");
}

void a_move_car::on_lineEdit_wuliao_zuixiaojianju_textChanged(const QString &arg1)
{
    // if(flag_data_ch ==1)  write_ini_file("./cfg");
}

void a_move_car::on_lineEdit_wuliao_zuixiao_sj_jiange_textChanged(const QString &arg1)
{
    // if(flag_data_ch ==1)  write_ini_file("./cfg");
}

void a_move_car::on_pushButton_2_clicked()
{
    Virtual_Piece_Cross_Camera = 1;

    for(map<int ,jieguo_tongji >::iterator iter=m_jljg.begin() ;iter!=m_jljg.end();)
    {
        m_jljg.erase(iter++);

    }
    duoshaogewuliao = 0;
}

void a_move_car::on_pb_start_zxyd_clicked()
{
    short rtn = 0;

    int axis_index = ui->comboBox_2->currentIndex();


    int bankahao = ui->comboBox_3->currentIndex();

    double Vmax = ui->lineEdit_yunxing_sudu->text().toDouble();//
    double Vadd = ui->lineEdit_jiasudu->text().toDouble();//
    int zhuandongfangxia = ui->lineEdit_zhuandongfangxiang->text().toInt();

    MCF_Set_Pulse_Mode_Net(axis_index, 0,bankahao);
    rtn = MCF_JOG_Net(axis_index, Vmax , Vadd, bankahao);
}

void a_move_car::on_pb_start_zhuandong_fxyd_clicked()
{
    short rtn = 0;
    int bankahao = ui->comboBox_3->currentIndex();
    int axis_index = ui->comboBox_2->currentIndex();

    double Vmax = ui->lineEdit_yunxing_sudu->text().toDouble();//
    double Vadd = ui->lineEdit_jiasudu->text().toDouble();//
    int zhuandongfangxia = ui->lineEdit_zhuandongfangxiang->text().toInt();

    MCF_Set_Pulse_Mode_Net(axis_index, 0,bankahao);
    rtn = MCF_JOG_Net(axis_index,  0-Vmax, Vadd, bankahao);
}




QStringList a_move_car::allDrivers()
{
    QStringList resultDriversName;
    foreach (QFileInfo my_info, QDir::drives())
    {
        resultDriversName.push_back(my_info.absolutePath());
    }
    return resultDriversName;
}

QStringList a_move_car::allChildDir(QString path)
{
    QStringList resultDirName;
#ifdef Q_OS_WIN
    if (path == "")
    {
        resultDirName = allDrivers();
        return resultDirName;
    }
#endif
    if (!QFile::exists(path))
    {
        return resultDirName;
    }
    QDir sourceDir(path);

    QFileInfoList fileInfoList = sourceDir.entryInfoList();

    foreach (QFileInfo fileInfo, fileInfoList)
    {
        if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
        {
            continue;
        }
        if (fileInfo.isDir())
        {
            resultDirName.push_back(fileInfo.fileName());
        }
    }
    return resultDirName;
}

QStringList a_move_car::allChildFile(QString path)
{
    QStringList resultDocName;
    if (path == "")
    {
        return resultDocName;
    }

    QDir sourceDir(path);
    QFileInfoList fileInfoList = sourceDir.entryInfoList();
    foreach (QFileInfo fileInfo, fileInfoList)
    {
        if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;
        if (fileInfo.isDir())
            continue;
        //resultDocName.push_back(fileInfo.fileName());//fileInfo.baseName()
        if(fileInfo.suffix()=="ini")
        {
            resultDocName.push_back(fileInfo.baseName());//fileInfo.baseName()
        }
    }
    return resultDocName;
}



QStringList a_move_car::saomiao_file(QString path)
{

    QStringList strList =  allChildFile( path);
    return strList;
}

QStringList   a_move_car::xinghao_win(QComboBox * ptr)
{
    ptr->clear();
    ptr->addItems(m_listfile_xinghao);
    return m_listfile_xinghao;
}


void a_move_car::on_pushButton_zengjiaxinghao_clicked()
{
    QString xinghao_zengj = ui->lineEdit_xinghao_xinxi->text();
    if(xinghao_zengj!="")
    {
        m_listfile_xinghao.append(xinghao_zengj);
        ui->comboBox_jizhong->clear();
        ui->comboBox_jizhong->addItems(m_listfile_xinghao);
    }
}

void a_move_car::on_pushButto_xie_bccs_clicked()
{
#if IF_WRITE_READ_INI
    if(flag_data_ch ==1)
    {

        write_ini_file_g("./cfg");
    }

#endif
}


#if IF_WRITE_READ_INI

void  a_move_car::write_ini_file_g(QString in_path)
{
    string path_ini="./cfg";
    if (_access(path_ini.c_str(), 0) == -1)
    {
        int i = _mkdir(path_ini.c_str());
    }

    QString xinghao =ui->comboBox_jizhong->currentText();
    int xinghao_index =ui->comboBox_jizhong->currentIndex();


    m_jlcs.moren_index = xinghao_index;
    m_jlcs.str_moren_index = xinghao;
    m_jlcs.m_str_Jiluxinxi[xinghao].xiangjishu = ui->lineEdit_xiangjishu->text().toInt();


    m_jlcs.m_str_Jiluxinxi[xinghao].xinghao_name = xinghao;

    m_jlcs.m_str_Jiluxinxi[xinghao].io_input[QString("qidong")] = ui->lineEdit_qidong_hao->text().toInt();
    m_jlcs.m_str_Jiluxinxi[xinghao].io_input[QString("tingzhi")] = ui->lineEdit_tingzhi_hao->text().toInt();
    m_jlcs.m_str_Jiluxinxi[xinghao].io_input[QString("fuwei")] = ui->lineEdit_fuwei_hao->text().toInt();
    m_jlcs.m_str_Jiluxinxi[xinghao].io_input[QString("jiting")] = ui->lineEdit_jiting_hao->text().toInt();
    m_jlcs.m_str_Jiluxinxi[xinghao].io_input[QString("lailiaoganying")] = ui->lineEdit_lailiaoganying_hao->text().toInt();

    m_jlcs.m_str_Jiluxinxi[xinghao].io_input[QString("danbu")] = ui->lineEdit_danbu_hao->text().toInt();


    m_jlcs.m_str_Jiluxinxi[xinghao].io_input[QString("yanshitingzhi")] = ui->lineEdit_out_yancchangchangjimiao->text().toInt();


    m_jlcs.m_str_Jiluxinxi[xinghao].io_output[QString("hongdeng")] = ui->lineEdit_out_hongdeng->text().toInt();
    m_jlcs.m_str_Jiluxinxi[xinghao].io_output[QString("lvdeng")] = ui->lineEdit_out_lvdeng->text().toInt();
    m_jlcs.m_str_Jiluxinxi[xinghao].io_output[QString("huangdeng")] = ui->lineEdit_out_huangdeng->text().toInt();
    m_jlcs.m_str_Jiluxinxi[xinghao].io_output[QString("jinliao")] = ui->lineEdit_out_jinliao_hao->text().toInt();
    m_jlcs.m_str_Jiluxinxi[xinghao].io_output[QString("chuliao")] = ui->lineEdit_out_chuliao_hao->text().toInt();

    m_jlcs.m_str_Jiluxinxi[xinghao].io_output[QString("zhendongpan")] = ui->lineEdit_out_zhendongpan->text().toInt();



    QString path =in_path+"/"+ xinghao + ".ini";
    QSettings* configIni = new QSettings(path, QSettings::IniFormat);
    configIni->setValue("xinghao/name", xinghao);
    configIni->setValue("xinghao/xiangjishu",   m_jlcs.m_str_Jiluxinxi[xinghao].xiangjishu);
    configIni->setValue("xinghao/qidong",   m_jlcs.m_str_Jiluxinxi[xinghao].io_input[QString("qidong")]);
    configIni->setValue("xinghao/tingzhi",    m_jlcs.m_str_Jiluxinxi[xinghao].io_input[QString("tingzhi")]);
    configIni->setValue("xinghao/fuwei",   m_jlcs.m_str_Jiluxinxi[xinghao].io_input[QString("fuwei")]);
    configIni->setValue("xinghao/jiting",   m_jlcs.m_str_Jiluxinxi[xinghao].io_input[QString("jiting")]);

    configIni->setValue("xinghao/danbu",   m_jlcs.m_str_Jiluxinxi[xinghao].io_input[QString("danbu")]);

    configIni->setValue("xinghao/hongdeng",    m_jlcs.m_str_Jiluxinxi[xinghao].io_output[QString("hongdeng")]);

    configIni->setValue("xinghao/lvdeng",   m_jlcs.m_str_Jiluxinxi[xinghao].io_output[QString("lvdeng")] );

    configIni->setValue("xinghao/huangdeng",   m_jlcs.m_str_Jiluxinxi[xinghao].io_output[QString("huangdeng")]);

    configIni->setValue("xinghao/jinliao",  m_jlcs.m_str_Jiluxinxi[xinghao].io_output[QString("jinliao")]);

    configIni->setValue("xinghao/chuliao", m_jlcs.m_str_Jiluxinxi[xinghao].io_output[QString("chuliao")] );

    configIni->setValue("xinghao/zhendongpan",   m_jlcs.m_str_Jiluxinxi[xinghao].io_output[QString("zhendongpan")]);

    configIni->setValue("xinghao/yanshitingzhi",   m_jlcs.m_str_Jiluxinxi[xinghao].io_output[QString("yanshitingzhi")]);






    int zh_i = ui->comboBox_2->currentIndex();
    int zhanhao_xuan = ui->comboBox_3->currentIndex();

    m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].zhanhao_index = zhanhao_xuan;




    m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].axis_index =  ui->comboBox_2->currentIndex();

    m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].zhanhao_index = ui->comboBox_3->currentIndex();;

    m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].m_tingzhi_quxian =ui->comB_tingzhiquxian->currentIndex();
    m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].weizhi_mode =ui->comB_weizhimoshi->currentIndex();
    m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].chushi_sudu = ui->lineEdit_chushihua_sudu->text().toInt();
    m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].yunxing_sud = ui->lineEdit_yunxing_sudu->text().toInt();
    m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].jia_sudu = ui->lineEdit_jiasudu->text().toInt();
    m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].jiajia_sudu =  ui->lineEdit_jjsud->text().toInt();
    m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].jiesu_sudu  = ui->lineEdit_jiesu_sud->text().toInt();
    m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].zhuandongfangxia =  ui->lineEdit_zhuandongfangxiang->text().toInt();



    m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].MaxSizePosition = ui->lineEdit_wuliao_zuidachicun->text().toInt();
    m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].MinSizePosition = ui->lineEdit_wuliao_zuixiaochicun->text().toInt();
    m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].Min_DistancePosition = ui->lineEdit_wuliao_zuixiaojianju->text().toInt();
    m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].Min_Time_Interval = ui->lineEdit_wuliao_zuixiao_sj_jiange->text().toInt();





    QString zouhao ="z"+QString::number(zh_i+1) +"/";



    configIni->setValue(zouhao+QString::fromLocal8Bit("axis_index"), zh_i);

    configIni->setValue(zouhao+QString::fromLocal8Bit("zhanhao_index"), m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].zhanhao_index);

    configIni->setValue(zouhao+QString::fromLocal8Bit("m_tingzhi_quxian"),   m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].m_tingzhi_quxian);
    configIni->setValue(zouhao+QString::fromLocal8Bit("weizhi_mode"),   m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].weizhi_mode);
    configIni->setValue(zouhao+QString::fromLocal8Bit("chushi_sudu"), m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].chushi_sudu);
    configIni->setValue(zouhao+QString::fromLocal8Bit("yunxing_sud"),   m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].yunxing_sud);
    configIni->setValue(zouhao+QString::fromLocal8Bit("jia_sudu"), m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].jia_sudu);
    configIni->setValue(zouhao+QString::fromLocal8Bit("jiajia_sudu"),   m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].jiajia_sudu);
    configIni->setValue(zouhao+QString::fromLocal8Bit("jiesu_sudu"), m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].jiesu_sudu);
    configIni->setValue(zouhao+QString::fromLocal8Bit("zhuandongfangxia"),   m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].zhuandongfangxia);

    configIni->setValue(zouhao+QString::fromLocal8Bit("MaxSizePosition"),   m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].MaxSizePosition);
    configIni->setValue(zouhao+QString::fromLocal8Bit("MinSizePosition"),   m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].MinSizePosition);
    configIni->setValue(zouhao+QString::fromLocal8Bit("Min_DistancePosition"),   m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].Min_DistancePosition);
    configIni->setValue(zouhao+QString::fromLocal8Bit("Min_Time_Interval"),   m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].Min_Time_Interval);









    for(int i = 0; i <  m_jlcs.m_str_Jiluxinxi[xinghao].xiangjishu; i++)
    {
        QString rootx =cols_xiangmu[i] +"/";

        rec_dev_infor  x =qstr_dev_unit_infor[cols_xiangmu[i]];
        m_jlcs.m_str_Jiluxinxi[xinghao].m_dev_infor[i] =x;


        configIni->setValue(rootx+QString::fromLocal8Bit("if_enable"), QString::number(x.if_enable));
        configIni->setValue(rootx+QString::fromLocal8Bit("shebei_weizhi"), QString::number(x.shebei_weizhi));
        configIni->setValue(rootx+QString::fromLocal8Bit("yundong_fangxia"), QString::number(x.yundong_fangxia));
        configIni->setValue(rootx+QString::fromLocal8Bit("shuchu_weihao"), QString::number(x.shuchu_weihao));
        configIni->setValue(rootx+QString::fromLocal8Bit("shuchu_dongzuo"), QString::number(x.shuchu_dongzuo));
        configIni->setValue(rootx+QString::fromLocal8Bit("yanshi"), QString::number(x.yanshi));





    }




    delete configIni;

    std::string mr_str ="./moren";
    QString path_moren = "./moren/moren.ini";


    if (_access(mr_str.c_str(), 0) == -1)
    {
        int i = _mkdir(mr_str.c_str());
    }


    // QString path_moren =in_path+"/"+ "moren" + ".mr";
    QSettings* configIni_moren = new QSettings(path_moren, QSettings::IniFormat);
    configIni_moren->setValue("moren/name", xinghao);
    configIni_moren->setValue("moren/index", xinghao_index);

    delete configIni_moren;
}

void  a_move_car::read_ini_file_g(QString path)
{
    QString path_inx = path + "/";

    int size_list = m_listfile_xinghao.size();
    for(int i = 0; i < size_list; i++)
    {


        QString  path_in = path_inx +m_listfile_xinghao[i] + ".ini" ;


        QSettings* configIni = new QSettings(path_in, QSettings::IniFormat);

        QString xinghao = configIni->value("xinghao/name").toString();

        m_jlcs.m_str_Jiluxinxi[xinghao].xiangjishu =  configIni->value("xinghao/xiangjishu").toString().toInt();


        m_jlcs.m_str_Jiluxinxi[xinghao].io_input[QString("qidong")] =  configIni->value("xinghao/qidong").toString().toInt();
        m_jlcs.m_str_Jiluxinxi[xinghao].io_input[QString("tingzhi")] =   configIni->value("xinghao/tingzhi").toString().toInt();
        m_jlcs.m_str_Jiluxinxi[xinghao].io_input[QString("fuwei")] =  configIni->value("xinghao/fuwei").toString().toInt();
        m_jlcs.m_str_Jiluxinxi[xinghao].io_input[QString("jiting")] =  configIni->value("xinghao/jiting").toString().toInt();

        m_jlcs.m_str_Jiluxinxi[xinghao].io_input[QString("danbu")] =  configIni->value("xinghao/danbu").toString().toInt();

        m_jlcs.m_str_Jiluxinxi[xinghao].io_output[QString("hongdeng")] =  configIni->value("xinghao/hongdeng").toString().toInt();

        m_jlcs.m_str_Jiluxinxi[xinghao].io_output[QString("lvdeng")]  =  configIni->value("xinghao/lvdeng").toString().toInt();

        m_jlcs.m_str_Jiluxinxi[xinghao].io_output[QString("huangdeng")] =  configIni->value("xinghao/huangdeng").toString().toInt();

        m_jlcs.m_str_Jiluxinxi[xinghao].io_output[QString("jinliao")] =  configIni->value("xinghao/jinliao").toString().toInt();

        m_jlcs.m_str_Jiluxinxi[xinghao].io_output[QString("chuliao")]  =  configIni->value("xinghao/chuliao").toString().toInt();

        m_jlcs.m_str_Jiluxinxi[xinghao].io_output[QString("zhendongpan")] =  configIni->value("xinghao/zhendongpan").toString().toInt();




        m_jlcs.m_str_Jiluxinxi[xinghao].io_input[QString("yanshitingzhi")] =  configIni->value("xinghao/yanshitingzhi").toString().toInt();

        int zhoushu =ui->comboBox_2->count();
        for(int i = 0; i < zhoushu; i++)
        {
            int zh_i = i;


            QString zouhao ="z"+QString::number(zh_i+1) +"/";

            m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].zhanhao_index = configIni->value(zouhao+QString::fromLocal8Bit("zhanhao_index")).toString().toInt();

            m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].axis_index =  configIni->value(zouhao+QString::fromLocal8Bit("axis_index")).toString().toInt();

            m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].zhanhao_index = configIni->value(zouhao+QString::fromLocal8Bit("zhanhao_index")).toString().toInt();

            m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].m_tingzhi_quxian  = configIni->value(zouhao+QString::fromLocal8Bit("m_tingzhi_quxian")).toString().toInt();
            m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].weizhi_mode  =  configIni->value(zouhao+QString::fromLocal8Bit("weizhi_mode")).toString().toInt();

            m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].chushi_sudu  = configIni->value(zouhao+QString::fromLocal8Bit("chushi_sudu")).toString().toInt();
            m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].yunxing_sud  =  configIni->value(zouhao+QString::fromLocal8Bit("yunxing_sud")).toString().toInt();
            m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].jia_sudu  =  configIni->value(zouhao+QString::fromLocal8Bit("jia_sudu")).toString().toInt();
            m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].jiajia_sudu  =  configIni->value(zouhao+QString::fromLocal8Bit("jiajia_sudu")).toString().toInt();
            m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].jiesu_sudu  = configIni->value(zouhao+QString::fromLocal8Bit("jiesu_sudu")).toString().toInt();
            m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].zhuandongfangxia  =  configIni->value(zouhao+QString::fromLocal8Bit("zhuandongfangxia")).toString().toInt();



            m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].MaxSizePosition  =  configIni->value(zouhao+QString::fromLocal8Bit("MaxSizePosition")).toString().toInt();
            m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].MinSizePosition  =  configIni->value(zouhao+QString::fromLocal8Bit("MinSizePosition")).toString().toInt();
            m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].Min_DistancePosition  = configIni->value(zouhao+QString::fromLocal8Bit("Min_DistancePosition")).toString().toInt();
            m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[zh_i].Min_Time_Interval  =  configIni->value(zouhao+QString::fromLocal8Bit("Min_Time_Interval")).toString().toInt();


        }



        for(int i = 0; i <  m_jlcs.m_str_Jiluxinxi[xinghao].xiangjishu; i++)
        {
            QString rootx =cols_xiangmu[i] +"/";

            rec_dev_infor  x;

            x.if_enable =  configIni->value(rootx+QString::fromLocal8Bit("if_enable")).toString().toInt();
            x.shebei_weizhi =  configIni->value(rootx+QString::fromLocal8Bit("shebei_weizhi")).toString().toInt();
            x.yundong_fangxia =  configIni->value(rootx+QString::fromLocal8Bit("yundong_fangxia")).toString().toInt();
            x.shuchu_weihao =  configIni->value(rootx+QString::fromLocal8Bit("shuchu_weihao")).toString().toInt();
            x.shuchu_dongzuo =  configIni->value(rootx+QString::fromLocal8Bit("shuchu_dongzuo")).toString().toInt();
            x.yanshi =  configIni->value(rootx+QString::fromLocal8Bit("yanshi")).toString().toInt();

            qstr_dev_unit_infor[cols_xiangmu[i]] = x;

            m_jlcs.m_str_Jiluxinxi[xinghao].m_dev_infor[i] =x;
        }


        delete configIni;
    }


    if(m_listfile_moren.size() >0)
    {

        QString path_moren = "./moren/moren.ini";
        QSettings* configIni_moren = new QSettings(path_moren, QSettings::IniFormat);
        m_moren_xinghao_ming = configIni_moren->value("moren/name").toString();

        //configIni_moren->setValue("moren/index", xinghao_index);

        delete configIni_moren;
    }


}


void a_move_car::set_win_from_ini_file_g(QString xinghao)
{

    int countx =   m_jlcs.m_str_Jiluxinxi.count(xinghao);
    if(countx >=1)
    {

        ui->lineEdit_xiangjishu->setText(QString::number(     m_jlcs.m_str_Jiluxinxi[xinghao].xiangjishu  ) );
        ui->lineEdit_qidong_hao->setText(QString::number(  m_jlcs.m_str_Jiluxinxi[xinghao].io_input[QString("qidong")] ) );
        ui->lineEdit_tingzhi_hao->setText(QString::number(  m_jlcs.m_str_Jiluxinxi[xinghao].io_input[QString("tingzhi")]  ) );
        ui->lineEdit_fuwei_hao->setText(QString::number(  m_jlcs.m_str_Jiluxinxi[xinghao].io_input[QString("fuwei")] ) );
        ui->lineEdit_jiting_hao->setText(QString::number( m_jlcs.m_str_Jiluxinxi[xinghao].io_input[QString("jiting")] ));
        ui->lineEdit_lailiaoganying_hao->setText(QString::number(   m_jlcs.m_str_Jiluxinxi[xinghao].io_input[QString("lailiaoganying")] ) );

        ui->lineEdit_danbu_hao->setText(QString::number(  m_jlcs.m_str_Jiluxinxi[xinghao].io_input[QString("danbu")] ) );





        ui->lineEdit_out_hongdeng->setText(QString::number(  m_jlcs.m_str_Jiluxinxi[xinghao].io_output[QString("hongdeng")]  ) );
        ui->lineEdit_out_lvdeng->setText(QString::number( m_jlcs.m_str_Jiluxinxi[xinghao].io_output[QString("lvdeng")]  ) );
        ui->lineEdit_out_huangdeng->setText(QString::number(  m_jlcs.m_str_Jiluxinxi[xinghao].io_output[QString("huangdeng")] ) );
        ui->lineEdit_out_jinliao_hao->setText(QString::number(  m_jlcs.m_str_Jiluxinxi[xinghao].io_output[QString("jinliao")]  ) );
        ui->lineEdit_out_chuliao_hao->setText(QString::number( m_jlcs.m_str_Jiluxinxi[xinghao].io_output[QString("chuliao")] ) );

        ui->lineEdit_out_zhendongpan->setText(QString::number( m_jlcs.m_str_Jiluxinxi[xinghao].io_output[QString("zhendongpan")]  ) );
        ui->lineEdit_out_yancchangchangjimiao->setText(QString::number(  m_jlcs.m_str_Jiluxinxi[xinghao].io_input[QString("yanshitingzhi")] ) );


        ui->comB_tingzhiquxian->setCurrentIndex( m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[ 0 ].m_tingzhi_quxian);
        ui->comB_weizhimoshi->setCurrentIndex( m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[0].weizhi_mode);

        ui->lineEdit_chushihua_sudu->setText(QString::number( m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[0].chushi_sudu));
        ui->lineEdit_yunxing_sudu->setText(QString::number( m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[0].yunxing_sud));
        ui->lineEdit_jiasudu->setText(QString::number( m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[0].jia_sudu));
        ui->lineEdit_jjsud->setText(QString::number( m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[0].jiajia_sudu));
        ui->lineEdit_jiesu_sud->setText(QString::number(  m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[0].jiesu_sudu));
        ui->lineEdit_zhuandongfangxiang->setText(QString::number( m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[0].zhuandongfangxia));


        ui->lineEdit_jiasudu->setText(QString::number( m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[0].jia_sudu));
        ui->lineEdit_jjsud->setText(QString::number( m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[0].jiajia_sudu));
        ui->lineEdit_jiesu_sud->setText(QString::number(  m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[0].jiesu_sudu));
        ui->lineEdit_zhuandongfangxiang->setText(QString::number( m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[0].zhuandongfangxia));


        ui->lineEdit_wuliao_zuidachicun->setText(QString::number(   m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[0].MaxSizePosition));
        ui->lineEdit_wuliao_zuixiaochicun->setText(QString::number(  m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[0].MinSizePosition));
        ui->lineEdit_wuliao_zuixiaojianju->setText(QString::number( m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[0].Min_DistancePosition));
        ui->lineEdit_wuliao_zuixiao_sj_jiange->setText(QString::number(  m_jlcs.m_str_Jiluxinxi[xinghao].zhou_infor[0].Min_Time_Interval ));


        //初始化表格
        for(int i = 0; i < m_jlcs.m_str_Jiluxinxi[xinghao].xiangjishu; i++)
        {
            // rec_dev_infor  x = qstr_dev_unit_infor[cols_xiangmu[i]];

            rec_dev_infor  x =   m_jlcs.m_str_Jiluxinxi[xinghao].m_dev_infor[i];




            QString name_dev  =cols_xiangmu[i];
            if(x.if_enable){
                head_infor[name_dev].if_enable_checkBox ->setCheckState(  Qt::Checked);
            }else
            {
                head_infor[name_dev].if_enable_checkBox ->setCheckState(  Qt::Unchecked);
            }

            //初始位置
            ui->tableWidget->setItem(1, i,new QTableWidgetItem(QString::number(x.shebei_weizhi)));

            //运动方向
            head_infor[name_dev].combox_yundong_fangxia->setCurrentIndex(x.yundong_fangxia);
            //输出位号
            head_infor[name_dev].combox_shuchu_weihao->setCurrentIndex(x.shuchu_weihao);
            //输出动作
            head_infor[name_dev].combox_shuchu_dongzuo->setCurrentIndex(x.shuchu_dongzuo);

            ui->tableWidget->setItem(5, i,new QTableWidgetItem(QString::number(x.yanshi)));

        }

    }
}

void a_move_car::read_wuzhou_zhou_infor(int index)
{

    int chanpin_index = ui->comboBox_jizhong->currentIndex();

    QString xinghao_str = ui->comboBox_jizhong->currentText();



#if IF_WRITE_READ_INI
    m_jlcs.m_str_Jiluxinxi[xinghao_str].zhou_infor[index].m_tingzhi_quxian =ui->comB_tingzhiquxian->currentIndex();
    m_jlcs.m_str_Jiluxinxi[xinghao_str].zhou_infor[index].weizhi_mode =ui->comB_weizhimoshi->currentIndex();
    m_jlcs.m_str_Jiluxinxi[xinghao_str].zhou_infor[index].chushi_sudu = ui->lineEdit_chushihua_sudu->text().toInt();
    m_jlcs.m_str_Jiluxinxi[xinghao_str].zhou_infor[index].yunxing_sud = ui->lineEdit_yunxing_sudu->text().toInt();
    m_jlcs.m_str_Jiluxinxi[xinghao_str].zhou_infor[index].jia_sudu = ui->lineEdit_jiasudu->text().toInt();
    m_jlcs.m_str_Jiluxinxi[xinghao_str].zhou_infor[index].jiajia_sudu =  ui->lineEdit_jjsud->text().toInt();
    m_jlcs.m_str_Jiluxinxi[xinghao_str].zhou_infor[index].jiesu_sudu  = ui->lineEdit_jiesu_sud->text().toInt();
    m_jlcs.m_str_Jiluxinxi[xinghao_str].zhou_infor[index].zhuandongfangxia =  ui->lineEdit_zhuandongfangxiang->text().toInt();

#endif


}


//如果是多 板卡怎么操作
void a_move_car::write_wuzhou_zhou_infor(int index )
{

    QString xinghao_str = ui->comboBox_jizhong->currentText();

#if IF_WRITE_READ_INI

    ui->comB_tingzhiquxian->setCurrentIndex(   m_jlcs.m_str_Jiluxinxi[xinghao_str].zhou_infor[index].m_tingzhi_quxian);
    ui->comB_weizhimoshi->setCurrentIndex(  m_jlcs.m_str_Jiluxinxi[xinghao_str].zhou_infor[index].weizhi_mode);
    ui->lineEdit_chushihua_sudu->setText(QString::number(   m_jlcs.m_str_Jiluxinxi[xinghao_str].zhou_infor[index].chushi_sudu));
    ui->lineEdit_yunxing_sudu->setText(QString::number(   m_jlcs.m_str_Jiluxinxi[xinghao_str].zhou_infor[index].yunxing_sud));
    ui->lineEdit_jiasudu->setText(QString::number(   m_jlcs.m_str_Jiluxinxi[xinghao_str].zhou_infor[index].jia_sudu));
    ui->lineEdit_jjsud->setText(QString::number(   m_jlcs.m_str_Jiluxinxi[xinghao_str].zhou_infor[index].jiajia_sudu));
    ui->lineEdit_jiesu_sud->setText(QString::number(   m_jlcs.m_str_Jiluxinxi[xinghao_str].zhou_infor[index].jiesu_sudu));
    ui->lineEdit_zhuandongfangxiang->setText(QString::number(   m_jlcs.m_str_Jiluxinxi[xinghao_str].zhou_infor[index].zhuandongfangxia));
#endif

}

#endif
void a_move_car::on_pushButton_shanchuxinghao_clicked()
{
    QString dq = ui->comboBox_jizhong->currentText();
    m_listfile_xinghao.removeOne(dq);
    ui->comboBox_jizhong->clear();
    ui->comboBox_jizhong->addItems(m_listfile_xinghao);
}

void a_move_car::on_pushButton_qidong_hao_clicked()
{

}

void a_move_car::on_comboBox_2_currentIndexChanged(int index)
{
    current_axie = index;

    write_wuzhou_zhou_infor(  index );
}




void a_move_car::on_comboBox_jizhong_currentIndexChanged(const QString &arg1)
{

    if(flag_changge_zuihou == 1)
    {
        int countx =   m_jlcs.m_str_Jiluxinxi.count(arg1);

        if(countx >=1)
        {

            set_win_from_ini_file_g(arg1);

        }
    }
}


void a_move_car::on_pushButton_out_zhizhen_hao_clicked()
{

}

void a_move_car::on_pushButton_out_hongdeng_hao_clicked()
{

}

void a_move_car::on_pushButton_out_lvdeng_hao_clicked()
{

}

void a_move_car::on_pushButton_out_huangdeng_hao_clicked()
{

}

void a_move_car::on_pushButton_out_zhendongpan_hao_clicked()
{

}

void a_move_car::on_pushButton_out_jinliaohao_clicked()
{

}

void a_move_car::on_pushButton_out_chuliaohao_clicked()
{

}

float  a_move_car::yanchangjimiao()
{
    float x = ui->lineEdit_out_yancchangchangjimiao->text().toFloat();
    return x;
}

void a_move_car::thread_changan_fu()
{
    int bankahao = ui->comboBox_3->currentIndex();
    while(!shifang_fu_flag)
    {

        static int kkb = 0;


        int in_axis_index = ui->comboBox_2->currentIndex();

        long pos;
        rtn = MCF_Get_Position_Net(in_axis_index, &pos, bankahao);

        int buchng = ui->lineEdit_press_buchang->text().toInt();



        int dDist =pos- buchng;

        double  Vini = ui->lineEdit_chushihua_sudu->text().toDouble();  //  '��ʼ�ٶ�
        double Vmax = ui->lineEdit_yunxing_sudu->text().toDouble();//   '�ٶ�
        double Vadd = ui->lineEdit_jiasudu->text().toDouble();//   '���ٶ�
        double Vjerk = ui->lineEdit_jjsud->text().toDouble();//   '�Ӽ��ٶ�
        double Vend = ui->lineEdit_jiesu_sud->text().toDouble();//   '�����ٶ�
        short Profile = ui->comB_tingzhiquxian->currentIndex();
        short Position_Mode = ui->comB_weizhimoshi->currentIndex();//   '���� ����

        rtn = MCF_Set_Axis_Profile_Net(in_axis_index, Vini, Vmax, Vadd, Vjerk, Vend, Profile, bankahao);
        rtn = MCF_Uniaxial_Net(in_axis_index, dDist, Position_Mode, bankahao);
    }
}

void a_move_car::thread_changan_zheng()
{
    int bankahao = ui->comboBox_3->currentIndex();
    while(!shifang_flag)
    {

        static int kkb = 0;
        int chanpin_index = ui->comboBox_jizhong->currentIndex();

        int in_axis_index = ui->comboBox_2->currentIndex();

        long pos;
        rtn = MCF_Get_Position_Net(in_axis_index, &pos, bankahao);

        int buchng = ui->lineEdit_press_buchang->text().toInt();

        int dDist =pos+ buchng ;

        double  Vini = ui->lineEdit_chushihua_sudu->text().toDouble();  //  '��ʼ�ٶ�
        double Vmax = ui->lineEdit_yunxing_sudu->text().toDouble();//   '�ٶ�
        double Vadd = ui->lineEdit_jiasudu->text().toDouble();//   '���ٶ�
        double Vjerk = ui->lineEdit_jjsud->text().toDouble();//   '�Ӽ��ٶ�
        double Vend = ui->lineEdit_jiesu_sud->text().toDouble();//   '�����ٶ�
        short Profile = ui->comB_tingzhiquxian->currentIndex();
        short Position_Mode = ui->comB_weizhimoshi->currentIndex();//   '���� ����

        rtn = MCF_Set_Axis_Profile_Net(in_axis_index, Vini, Vmax, Vadd, Vjerk, Vend, Profile, bankahao);
        rtn = MCF_Uniaxial_Net(in_axis_index, dDist, Position_Mode, bankahao);
    }

}


void a_move_car::on_pb_start_zxyd_chixu_zheng_pressed()
{
    shifang_flag = 0;
    std::thread t1(&a_move_car::thread_changan_zheng,this);
    t1.detach();
}

void a_move_car::on_pb_start_zxyd_chixu_zheng_released()
{
    shifang_flag =1;
}

void a_move_car::on_pb_start_zhuandong_fxyd_chixu_fu_pressed()
{
    shifang_fu_flag = 0;
    std::thread t1(&a_move_car::thread_changan_fu,this);
    t1.detach();
}

void a_move_car::on_pb_start_zhuandong_fxyd_chixu_fu_released()
{
    shifang_fu_flag = 1;
}

void a_move_car::on_pushButton_3_clicked()
{
    MCF_Set_Output_Bit_Net(8 , Bit_Output_Close, STATION_0);
    MCF_Set_Output_Bit_Net(7 , Bit_Output_Close, STATION_0);
}

void a_move_car::on_pushButton_4_clicked()
{
    MCF_Set_Output_Bit_Net(8 , Bit_Output_Open, STATION_0);
    MCF_Set_Output_Bit_Net(7 , Bit_Output_Open, STATION_0);
}

void a_move_car::on_pb_start_zhuandong_tingzhi_2_clicked()
{
    rtn = MCF_Clear_Input_Fall_Bit_Net(0, 0);


    MCF_Clear_Compare_Points_Net             (AXIE_INDEX_0);
    MCF_Clear_Compare_Current_Points_Net     (AXIE_INDEX_0);
}

void a_move_car::on_pushButton_jiting_hao_clicked()
{

}
