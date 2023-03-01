#ifndef MOVE_CTL_H
#define MOVE_CTL_H

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

using namespace std;

#include "move_ctl.h"
#include "MCDLL_NET.h"

#include <QMessageBox>
#include <QEventLoop>

enum DOUT_INDEX{OUT_CCD1, OUT_CCD2, OUT_CCD3, OUT_NG_CQ,OUT_OK_CQ,OUT_PY_DCF,OUT_JL_DCF,OUT_DOUT7,OUT_DOUT8,OUT_DOUT9,OUT_DOUT10,OUT_ZDP_CQ,OUT_ZDP_QD,OUT_HONG,OUT_LV,OUT_HUANG };
enum DIN_INDEX{IN_YUADIAN_ZP, IN_LAILIAO_GY, IN_QIDONG, IN_TINGZHI,IN_HUIYUANDIAN,IN_JITING,IN_6,IN_PYQG_YW,IN_PYQG_CW,IN_JLQG_YW,IN_JLQG_CW };

enum WORK_THREA{THRED_CAM0,THRED_CAM_1_2,THRED_CHUIQI1,THRED_CHUIQI2,THREAD_CGQ};
enum GONGWEI{GW0,GW1,GW2,GW3};

enum E_DIANPIAN{DI_DIANPIN,GAO_DIANPIN};

typedef struct str_wuliao_queue{
    long maichong_xunshun = -1;
    int have_flag = 2;
    int if_tiliao = 0;
}Wuliao_queue;

class MESSThread: public QObject
{
    Q_OBJECT

private:
    const QString m_title;
    const QString m_message;

public:
    MESSThread(const QString &title, const QString &message);

    static void show(const QString &title, const QString &message);

private:
    void readyShow(void);

private slots:
    void onShow(void);
};




namespace Ui {
class move_ctl;
}

class move_ctl : public QMainWindow
{
    Q_OBJECT

public:
    explicit move_ctl(QWidget *parent = nullptr);
    ~move_ctl();

public:
    int initial();
    int close();
    int jog_initial();

public:
    short rtn = -1;
    unsigned short Connection_Number = 1;
    unsigned short Station_Number[8] = { 0 , 1, 2, 3, 4, 5, 6, 7 };//����վ��˳��
    unsigned short Station_Type[1] = {2 };	//����վ������
    unsigned short StationNumber = 0;


    int axis_index = 0;
    int m_tingzhi_quxian = 0;
    int weizhi_mode = 0;
    int chushi_sudu = 0;
    int yunxing_sud = 10000;
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

    //运行状态
    double m_Vadd = 0;//   '���ٶ�
    double m_Vjerk = 0;//   '�Ӽ��ٶ�

    short m_Profile = 0;
    int if_open_car = 0;
    int  run_state_flag  = 0;
    int  shineng_state_flag = 0;
    int if_in_yuandian = 0;

    //位置记录
    int liaocao_jiange = 540;
    int pos_yuandian = 0;
    int pos_ganyingdian = 0;
    long pos_gw1 = 0;
    long pos_gw2 = 0;
    long pos_chuiqi1 = 0;
    long pos_chuiqi2 = 0;
    int tingdun_shijian = 1000;

    long gw2_pl_chuiqi1 = 0;
    long gw2_pl_chuiqi2 = 0;
    int yiquan_maichongshu = 10800;

    // int ceshi_start_stop = 0;
    int  cishu_yunxing = 1;
    long current_maichong = 0;
    int if_ceshizhong = 0;


    queue <Wuliao_queue> cganqi_gongwei_1;
    queue <Wuliao_queue> cganqi_gongwei_2;
    queue <Wuliao_queue> cganqi_chuiqi_1;
    queue <Wuliao_queue> cganqi_chuiqi_2;

    queue <long> liangxiangji_gw;

    queue <long> tiliao_duilie;

    vector< queue <Wuliao_queue>> chuanganqi_queue_v;


    queue <Wuliao_queue>  tiliao_chuiqi_1;
    queue <Wuliao_queue>  tiliao_chuiqi_2;
    void clear_queue();
    void add_tr_point(unsigned short ch,unsigned short io_index,long pos);

    std::thread new_thread_gw1 ;
    std::thread new_thread_gw2;
    std::thread new_thread_gw3;
    std::thread new_thread_gw0 ;
    std::thread jiankong_chuanganqi_threa5 ;

    std::thread jiankong_wulianniu_xinxi ;
    int qidong_flag = 0;
    int tingzhi_flag = 0;
    int zanting_flag = 0;
    int jiting_flag = 0;

    //写成配置档案
    int shifou_jiaqu = 1;

    int yuandian_zankongbi_shijian = 10;
    int ganyingdian_zankongbi_shijian = 10;



signals:
    void  sig_car_ui_send_qidong(int flag);
    void  sig_car_ui_send_tingzhi(int flag);
    void  sig_car_ui_send_jiting(int flag);
    void  sig_car_ui_send_huiyuandian(int flag);

    //public slots:


    //    void  slot_get_main_ui_qidong(int flag);
    //    void  slot_get_main_ui_tingzhi(int flag);
    //    void  slot_get_main_ui_jiting(int flag);
    //    void  slot_get_main_ui_huiyuandian(int flag);




signals:
    void send_zhuandong_tr(long obj_position);
    void send_warnning();
    void send_camera_cap(int cam_index ,long Encoder);//发送给相机，触发拍照，内容包括哪个相机和什么脉冲值

public:
    void get_sudu(void * ptr);
    void thread_jiankong(void* ptr);
    void  huiyuandian_thread(void* ptr);
    //void  panduan_shifou_paizhao(void* ptr,int index);

    void  proecess_gw0(void* ptr);
    void  proecess_gw1(void* ptr);
    void  jiankong_wuli_anniu(void* ptr);// 监控物理按钮是否按下事件


    void  thread_chuanganqi(void* ptr);
    int chuanganqi_qidong_state = 0;
    int chuanganqi_xiancheng_runxing = 1;

    void main_thread(void* ptr);
    void go_position(int position,int flag_dir);
    void tixing(QString neirong);

    void add_maichong_map(long maichong);
    void jiaqu_motion();
    void jiechu_jiaqu();





public slots:
    void refact_warnning();
    void  on_ps_open_car_clicked();
    void  on_ps_close_car_clicked();
    void on_ps_zheng_move_clicked();
    void on_ps_fu_move_clicked();
    void get_int_num(QString num);
    void on_pb_shineng_guan_clicked();
    void on_pb_shineng_kai_clicked();
    void on_ps_jog_zheng_clicked();
    void on_ps_jog_fu_clicked();
    void on_Stop1_clicked();
    void on_stop_man_clicked();
    void on_ps_yushe_clicked();
    void on_fenxuan_qidong_clicked();
    void on_fenxuan_tingzhi_clicked();
    void on_pushButton_ceshi_clicked();
    void on_pushButton_tingzhi_ceshi_clicked();
    void on_pushButton_huiyuandian_clicked();
    

signals:
    void send_int_num(QString num);
private slots:
    void on_ps_bianmqinglng_clicked();

    void on_pushButton_danbu_ceshi_clicked();


    void on_push_gaodianpin_tr_clicked();

    void on_push_didianpin_tr_clicked();

    void on_pushButton_baocuncanshu_clicked();

    void on_push_gaodianpin_tr_2_clicked();

    void on_push_didianpin_tr_2_clicked();

    void on_push_gaodianpin_tr_3_clicked();

    void on_push_didianpin_tr_3_clicked();


private:
    Ui::move_ctl *ui;
};

#endif // MOVE_CTL_H


