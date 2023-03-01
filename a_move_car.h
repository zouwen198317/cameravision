#ifndef A_MOVE_CAR_H
#define A_MOVE_CAR_H

#include <QMainWindow>

#include "afxtempl.h"
#include<Windows.h>
#include <QMainWindow>

#include<qfiledialog.h>
#include<qdir.h>
#include<qfile.h>
#include"opencv2/opencv.hpp"
#include <iostream>

#include<thread>
#include <queue>
#include <assert.h>

#include <QWidget>

#include <QMainWindow>
#include <QObject>
#include <QWidget>

#include <QWidget>

#include <QMainWindow>
#include <QObject>
#include <QWidget>

#include <QMdiSubWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsObject>

#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>


#include <opencv2\opencv.hpp>
#include <math.h>

#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <time.h>
#include <io.h>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <thread>

#include <QMenu>
#include <QContextMenuEvent>
#include <QStyleOption>
#include <QPainter>
#include <QFileDialog>
#include <QStandardItem>



#include "MCDLL_Define.h"
#include "MCDLL_NET.h"

#include <QMessageBox>
#include <QEventLoop>


#include <QTableWidget>

#include"io_code_index.h"
#include"camera_commen.h"

using namespace std;

#define IF_WRITE_READ_INI 1

#define OKSignal   true
#define NGSignal   false


typedef struct str_jieguo_tongji{
    int geshu =0;
    int flag_ng_ok = 1;
    float lianglv = 0;
    float bulianglv = 0;
}jieguo_tongji;





typedef struct str_car_base_infor{

    QString  name ="yuanjing";


   int zhanhao_index = 0;
    int axis_index = 0;
   
    int xiangji_shu = 8;


    int m_tingzhi_quxian = 0;
    int weizhi_mode = 0;
    int chushi_sudu = 0;
    int yunxing_sud = 20000;
    int jia_sudu = 100000;
    int huiyuandian_sudu = 150;
    int jiajia_sudu = 1000000;
    int  jiesu_sudu = 100000;
    int zhuandongfangxia = 0;



    int  MaxSizePosition = 1000000;					//记录物体最大Size
   int  MinSizePosition = 0;					//记录物体最小Size
   int  Min_DistancePosition = 10;				//记录两物体最小间隔
   int Min_Time_Interval = 0;				//记录最小间隔时间





}car_base_infor;


typedef struct str_head_zhuanpan_infor{


    QTableWidgetItem *if_enable_checkBox = nullptr;
    int shebei_weizhi = 1000;
    QComboBox *combox_yundong_fangxia= nullptr;
    QComboBox *combox_shuchu_weihao= nullptr;
    QComboBox *combox_shuchu_dongzuo= nullptr;
    int yanshi = 0;
    int chufa_geshu = 0;

}head_zhuanpan_infor;


typedef struct str_rec_dev_infor{

    QString  name;
    int if_enable = 0;
    int if_enable_checkBox = 0;
    int shebei_weizhi = 1000;
    int yundong_fangxia = 0;
    int shuchu_weihao = 0;
    int shuchu_dongzuo = 0;
    int yanshi = 0;
    int chufa_geshu = 0;

}rec_dev_infor;



typedef struct str_jiluxinxi_BZ{


    QString xinghao_name = "";
    map<int ,car_base_infor> zhou_infor;
    int  xiangjishu = 12;

   map<QString, int> io_input;
   map<QString, int> io_output;
   

   map<int , rec_dev_infor> m_dev_infor;

}Jiluxinxi_bz;




typedef struct str_moren_jiluxinxi_BIAOZHUN{

    int moren_index = 0;
    QString str_moren_index ="";
    map<int ,Jiluxinxi_bz> m_Jiluxinxi;
    map<QString ,Jiluxinxi_bz> m_str_Jiluxinxi;
}moren_jiluxinxi_biaozhun;



#define CAM_NUM_G  10

#define DEV_NUM 12
namespace Ui {
class a_move_car;
}

class a_move_car : public QMainWindow
{
    Q_OBJECT

public:
    explicit a_move_car(QWidget *parent = nullptr);
    ~a_move_car();

public://data

    short rtn = -1;
    unsigned short Connection_Number = 1;
    unsigned short Station_Number[8] = { 0 , 1, 2, 3, 4, 5, 6, 7 };
    unsigned short Station_Type[1] = {2 };
    unsigned short StationNumber = 0;

    int jiankong_flag = 0;





    //int zhanhao_index = 0;
   // int axis_index = 0;
    int m_tingzhi_quxian = 0;
    int weizhi_mode = 0;
    int chushi_sudu = 0;
    int yunxing_sud = 20000;
    int jia_sudu = 100000;
    int huiyuandian_sudu = 150;
    int jiajia_sudu = 1000000;
    int  jiesu_sudu = 100000;
    int mubiao_weizhi = 20000;
    int Read_Pos1 = 0;
    long Read_Enc1 = 0;
    double  Read_Vel1 = 0;
    int flag_get_infor = 1;
    int flag_jiankong = 1;
   // int  zhuandongfangxia = 0;

    // 板卡 状态采集  线程
    int Main_PulseEquivalent = 1;						//记录脉冲当量
    int Main_CameraNumber = CAM_NUM_G;							//记录相机个数
    unsigned long  Trig_Camera_Count[CAM_NUM_G] = {0};			//记录相机触发次数
    unsigned long  Trig_Blow_NG_Count = 0;				//记录NG气阀触发次数
    unsigned long  Trig_Blow_OK_Count = 0;				//记录OK气阀触发次数
    unsigned long  Trig_Blow_1_Count = 0;				//记录气阀1触发次数
    unsigned long  Trig_Blow_2_Count = 0;				//记录气阀2触发次数

    unsigned short Piece_Input_Number = 0;				//对应的触发器链接口
    unsigned long  Piece_Find = 0;						//物件个数
    unsigned long  Piece_Size[10] = {0};				//物件尺寸 ： 10组
    unsigned long  Piece_Distance_To_nextp[CAM_NUM_G] = {0};	//物件间隔：10组
    unsigned long  Piece_Cross_Camera = 0;				//通过全部相机的个数

    unsigned long Virtual_Piece_Cross_Camera = 1;

    long Encoder = 0;									//记录编码器值
    long Position = 0;									//记录位置值
    long dangqian_sudu = 0;

    unsigned long MaxSizePosition = 1000000;					//记录物体最大Size
    unsigned long MinSizePosition = 0;					//记录物体最小Size
    unsigned long Min_DistancePosition = 10;				//记录两物体最小间隔
    unsigned long Min_Time_Interval = 0;				//记录最小间隔时间

    unsigned long Piece_Pass = 0;						//记录不合格物件数量




    int  shineng_state_flag = 0;// 电机 是否 使能
    //运行状态
    double m_Vadd = 0;//   '���ٶ�
    double m_Vjerk = 0;//   '�Ӽ��ٶ�

    short m_Profile = 0;

    bool CameraBlowTriState = false;
    short CameraBlowTriFuncNum = 0;
    std::thread thread_jiankong_state ;
    void Get_Camera_Blow_TriCount( void *);
    void  go_position(int dDist,int flag_dir);

    // 设备  表格
    map<QString, head_zhuanpan_infor> head_infor;
    map<QString, rec_dev_infor> qstr_dev_unit_infor;
    map<int, rec_dev_infor> int_dev_unit_infor;

    map<int,QString> rec_name;
    map<QString, int> state_row;
    map<QString, int> xiangmu_cols;
    map<QString, int> xiangmu_rows;

    map<int,QString> cols_xiangmu;
    map<int, QString> rows_xiangmu;
    int current_row = 0;
    int current_column = 0;

    ////////////
    /// \brief preject_name
    ///
    int flag_data_ch = 0;
    //car_base_infor baocun_canshu;
    //unsigned long Virtual_Piece_Cross_Camera = 1;
    QString preject_name ="pro_name";
    void set_project_name(QString input_name){preject_name = input_name; }
    //void  write_ini_file(QString path);
    void  read_ini_file(QString path);
    void set_move_car_param();
    short   SetCameraFunction(unsigned short CameraNumber, unsigned short StationNumber);
    void  operator_result(int NGSignalNumber);
    void  chushihua_ok_ng_chuiqi();

    //获取输入的信号
    //设置轴参数

    void  gei_main_diaoyong_qidong();
    void  gei_main_diaoyong_tingzhi();
    unsigned short num_axie = AXIS_COUNT_G;
    void set_car_huiling_xianwei();
    void xSleep(int msec);
    int yuandian_zankongbi_shijian = 10;
    int Return_Get_Input_Temp = 0;
    std::thread thread_get_input_state ;
    void thred_Get_Input( void *);

    int chulicishu_temp = 0;
    vector<std::vector< tr_s_resultinfo>> vct_result;
    vector<int> vct_result_int;

    int   duoshaogewuliao = 0;

    map<int ,jieguo_tongji> m_jljg;
    void chuli_jieguo_tongj( void * ptm);



    map<QString,int > xialiao_bangding;
    float yanchangjimiao();





    //监控急停按钮
    int jiankong_jiting_yinganniu_thred_flag = 0;
    void jiankong_jiting_yinganniu_thred_thred(void *);


    //监控启动按钮
    int jiankong_qidong_yinganniu_thred_flag = 0;
    void jiankong_qidong_yinganniu_thred(void *);

    //监控停止按钮
    int jiankong_tingzhi_yinganniu_thred_flag = 0;
    void jiankong_tingzhi_yinganniu_thred(void *);


    int jiankong_qidong_ruan_anniu_thred_flag = 0;
    void jiankong_qidong_ruan_anniu_thred(void *);


    int jiankong_tingzhi_ruan_anniu_thred_flag = 0;
    void jiankong_tingzhi_ruan_anniu_thred(void *);



signals:
    void  send_jieguo_to_main(Fanhuijieguo obj );
public slots:
    void   clickCombobox_yundong_fx(QString text);
    void   clickCombobox_shuchu_wh(QString text);
    void   clickCombobox_shuchu_dongzuo(QString text);
    void  chulijieguo_hanshu_vst(int flag_ng_ok, std::vector< tr_s_resultinfo> input_vst,int paizhaocshu );

public:

#if IF_WRITE_READ_INI
    //moren_jiluxinxi m_jlcs;
    moren_jiluxinxi_biaozhun m_jlcs;
    void  write_ini_file_g(QString path);
    void  read_ini_file_g(QString path);

    void  read_wuzhou_zhou_infor(int index);
    void  write_wuzhou_zhou_infor(int index );

     int flag_changge_zuihou = 0;
     int shifang_flag = 0;
     void thread_changan_zheng();
     int shifang_fu_flag = 0;
     void thread_changan_fu();
#endif


    QStringList  allChildDir(QString path);
    QStringList  allDrivers();
    QStringList  allChildFile(QString path);

    QStringList  saomiao_file(QString path);
    //QStringList  allChildFile(QString path);
    // QStringList  allChildDir(QString path);
    // QStringList  allDrivers();
    void  set_win_from_ini_file_g(QString xinghao);

    QStringList m_listfile_xinghao ;
    QStringList m_listfile_xinghao_quanlujin ;
    QString m_moren_xinghao_ming ="default";
     QStringList m_listfile_moren ;

    void set_xinghao(int index);
    int current_axie = 0;


    QStringList  xinghao_win(QComboBox * ptr);



public slots:
    void on_ps_open_car_clicked();

    void on_ps_close_car_clicked();

    void on_ps_bianmqinglng_clicked();

    void on_pb_shineng_kai_clicked();

    void on_pb_shineng_guan_clicked();

    void on_Stop1_clicked();

    void on_stop_man_clicked();

    void on_pb_start_zhuandong_clicked();

    void on_pb_start_zhuandong_tingzhi_clicked();

    void on_tableWidget_cellClicked(int row, int column);

    void on_tableWidget_cellChanged(int row, int column);

    void on_pushButton_clicked();



    void on_lineEdit_zhou_textChanged(const QString &arg1);

    void on_lineEdit_zhanhao_textChanged(const QString &arg1);

    void on_lineEdit_xiangjishu_textChanged(const QString &arg1);



    void on_comB_weizhimoshi_currentIndexChanged(int index);

    void on_comB_tingzhiquxian_currentIndexChanged(int index);

    void on_lineEdit_chushihua_sudu_textChanged(const QString &arg1);

    void on_lineEdit_yunxing_sudu_textChanged(const QString &arg1);

    void on_lineEdit_jiasudu_textChanged(const QString &arg1);

    void on_lineEdit_jjsud_textChanged(const QString &arg1);

    void on_lineEdit_jiesu_sud_textChanged(const QString &arg1);

    void on_lineEdit_zhuandongfangxiang_textChanged(const QString &arg1);

    void on_lineEdit_wuliao_zuidachicun_textChanged(const QString &arg1);

    void on_lineEdit_wuliao_zuixiaochicun_textChanged(const QString &arg1);

    void on_lineEdit_wuliao_zuixiaojianju_textChanged(const QString &arg1);

    void on_lineEdit_wuliao_zuixiao_sj_jiange_textChanged(const QString &arg1);

    void on_pushButton_kaiqishaixuan_clicked();

    void on_pushButton_guanbishaixuan_clicked();

private slots:
    void on_pushButton_2_clicked();

    void on_pb_start_zxyd_clicked();

    void on_pb_start_zhuandong_fxyd_clicked();

    void on_pushButton_zengjiaxinghao_clicked();

    void on_pushButto_xie_bccs_clicked();

    void on_pushButton_shanchuxinghao_clicked();


    void on_pushButton_qidong_hao_clicked();

    void on_comboBox_2_currentIndexChanged(int index);


    void on_comboBox_jizhong_currentIndexChanged(const QString &arg1);



    void on_pushButton_out_zhizhen_hao_clicked();

    void on_pushButton_out_hongdeng_hao_clicked();

    void on_pushButton_out_lvdeng_hao_clicked();

    void on_pushButton_out_huangdeng_hao_clicked();

    void on_pushButton_out_zhendongpan_hao_clicked();

    void on_pushButton_out_jinliaohao_clicked();

    void on_pushButton_out_chuliaohao_clicked();



    void on_pb_start_zxyd_chixu_zheng_pressed();

    void on_pb_start_zhuandong_fxyd_chixu_fu_pressed();

    void on_pb_start_zxyd_chixu_zheng_released();

    void on_pb_start_zhuandong_fxyd_chixu_fu_released();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pb_start_zhuandong_tingzhi_2_clicked();

    void on_pushButton_jiting_hao_clicked();

private:
    Ui::a_move_car *ui;
};

#endif // A_MOVE_CAR_H
