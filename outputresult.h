#ifndef OUTPUTRESULT_H
#define OUTPUTRESULT_H



#include <QMainWindow>


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

#include "camera_commen.h"


#include "customerqstyleditemdelegate.h"

#include <QTableWidget>

//#include "move_ctl.h"
#include "a_move_car.h"
//using namespace  cv;


typedef struct str_communit_cfg{

    map<int, QString> cam_name_0_i;
    map<int, int> qiyong_1_i;
    map<int, QString> dianpin_2_i;
    map<int, QString> neirong_3_i;
    map<int, int> yanshi_4_i;

}Communit_cfg;


typedef struct str_select_communit_cfg{

    map<int, QString> cam_name_0_i;
    map<int, int> qiyong_1_i;
    map<int, QString> dianpin_2_i;
    map<int, QString> neirong_3_i;
    map<int, int> yanshi_4_i;

}Select_Communit_cfg;






namespace Ui {
class OutPutResult;
}



typedef struct str_head_biaoge_infor{

    int if_enable = 0;
    QString  name;
    QTableWidgetItem *checkBox = nullptr;
    QComboBox *combox_dianpin= nullptr;
    QComboBox *combox_neirong= nullptr;
    QComboBox *combox_IO_index= nullptr;
    int yanshi;

}head_biaoge_infor;



class OutPutResult : public QMainWindow
{
    Q_OBJECT

public:
    explicit OutPutResult(QWidget *parent = nullptr,QString name_win ="",QString task_id_str ="" , QString unit_id_str ="");
    ~OutPutResult();

signals:
    void send_mat_to_main(cv::Mat  img, int index);
    void send_result_next(cv::Mat  img,  Alg_infor alg_infor, std::vector<tr_s_resultinfo>  next_put_ng);//向下一单元输出
    void send_out_msg(int frameID);
    void send_out_msg_queue(QString frameID);
    void send_Fanhuijieguo(Fanhuijieguo );



public slots:
    void pre_unix_input(  cv::Mat  img,  Alg_infor  alg_infor, std::vector<tr_s_resultinfo>  next_put_ng);//接收上一单元检测数据
    //void pre_alg_unix_input( QString unit_id ,int frameID,  Map_resultinfo & tongji_infor, std::vector<tr_s_resultinfo> next_put_ng );//接收上一算法单元检测数据
    void pre_alg_unix_input_queue( queue<for_queue_resultinfo> & tongji_infor_queue, std::vector<tr_s_resultinfo> next_put_ng );//接收上一算法单元检测数据
    void set_out_Fanhuijieguo(Fanhuijieguo );



    //void pre_alg_unix_input_cam( QString unit_id,int frameID,  Map_resultinfo & tongji_infor, std::vector<tr_s_resultinfo> next_put_ng , void * ptr);//接收上一算法单元检测数据

   // void  all_output_infor_queue(QString frameID/*, vector<QString> & out_infor*/);
    void clear_output_infor_queue();

    void   clickCombobox_dianpin(QString text);
    void   clickCombobox_neirong(QString text);
    void   clickCombobox_io(QString text);



public:
    QString m_source_name ="";
    QString m_task_id_str = "";
    QString m_unit_id_str= "";
    QString tiliaogongwei ="";
    int task_num = 0;
    int frame_index = 0;
    int yanshi = 0;
    map<int , int> m_reslt;
    map<int , QString> m_reslt_unit_id;

    map<QString, map<int ,int >> m_tongji_out;

    //获取之前结果
    map<QString, queue<for_queue_resultinfo >> m_tongji_out_queue;

    map<QString,int> dev_str_int;
    map<int,QString> dev_int_str;
    int initial_ok = 0;
    vector<string> inputstr;
    // map<QString,Communit_cfg> dev_str_mem;

    map<QString, head_biaoge_infor> head_infor;
    map<int,QString> rec_name;
    map<QString, int> state_row;
    map<QString, int> xiangmu_cols;

    map<QString, camera_commen*> cam_prt;
    map<camera_commen* ,QString> cam_prt_str;

    Fanhuijieguo m_jieguo;

    // Communit_cfg  m_communit_cf;
    int current_row = 0;
    int current_column = 0;

    //相机列表
    QStandardItemModel *cam_model =nullptr;

   // move_ctl *m_ptr_movectl = nullptr;


     std::vector<tr_s_resultinfo> m_temp_tr_s_resultinfo;
     a_move_car *m_ptr_movectl = nullptr;

    //构建一个 以单元id作为标识的map 来保存每个识别算法的队列
    // map<QString, queue


private slots:
    // void on_tableWidget_itemClicked(QTableWidgetItem *item);

    void on_tableWidget_cellChanged(int row, int column);

    void on_tableWidget_cellClicked(int row, int column);


    void on_pushButton_send_cam_tr_test_clicked();

private:
    Ui::OutPutResult *ui;
};

#endif // OUTPUTRESULT_H
