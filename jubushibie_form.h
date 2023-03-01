#ifndef JUBUSHIBIE_FORM_H
#define JUBUSHIBIE_FORM_H
#include <QMainWindow>

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

#include <QWidget>
#include <QList>
#include <QPixmap>
#include "mypixitem.h"

#include <QGraphicsView>    //视图类
#include <QGraphicsScene>   //场景类
#include <QGraphicsItem>    //图元类
#include <math.h>
#include <QWheelEvent>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

#include <QString>


#include<map>
#include<vector>
#include<string>
#include <iostream>
#include <mutex>


#include <QFileDialog>
#include <QFileInfo>
#include <QImage>



#include "opencv/highgui.h"

#include "camera_commen.h"



#define IF_USING_TENSORT 1




using namespace  cv;


#if IF_USING_TENSORT
#include "class_timer.hpp"
#include "class_detector.h"

#include <memory>

#endif




extern int shifou_tm_paizhao  ;

//  queue<for_queue_resultinfo> q_result_info;

#define SHIFOU_SUOFANG 1

namespace Ui {
class jubushibie_form;
}

class jubushibie_form : public QMainWindow
{
    Q_OBJECT

public:
    explicit jubushibie_form(QWidget *parent = nullptr,     properconfig *  input_procfg = nullptr, int model_id = 0,QString str_model_id="");

    ~jubushibie_form();


public:

    QString source_name ="";
    QString m_task_id_str="";
    QString m_unit_id_str="";
    int m_alg_be_call_num = 0;

    QtTreePropertyBrowser * tree_br =nullptr;
    properconfig * m_input_procfg =nullptr;

    //算法信息
    Alg_infor  m_alg_infor;
    int real_detect_obj_num = 0;
    cv::Mat src_image;
    std::vector<tr_s_resultinfo>  out_put_ng;

    //struct tr_s_resultinfo* Result =nullptr;

    int flag_jisuan_thre=0;
    int jisuan_mat_thread(void * ptm);


    QHash< QString, queue< for_queue_resultinfo> > m_shibie;
    QHash< QString, queue<std::vector<tr_s_resultinfo> >> m_shibie_jieguo;

    int jisuan_xiancheng_flag = 0;
    void jisuan_xiancheng(void * ptm);
    std::thread jisuan_th;

    QString camera_name="";



public:

    int alg_intial();
    int alg_processing(char* data, int width, int height, int channel);

    int filter_result(std::vector<tr_s_resultinfo> input_vc_resultsrtForsort,std::vector<tr_s_resultinfo> &output_vc_resultsrtForsort);
    bool alg_get_result();
     bool alg_get_result(std::vector<tr_s_resultinfo>&  xout_put_ng, int xnum);

    int release_alg();
    int update_property(properconfig *  input_procfg);
    void show_img_xx(cv::Mat  img);


    //基于形状检测的算法系列
    int fx_initial_gpu(int gpu_id, int cam_id,  char* path_input);
    int fx_alg_processing(int cam_id, char* data, int width, int height, int channel);
    bool fx_get_detection_result_gpu_cam_x(struct tr_s_resultinfo* Result, int Num, int cam_id);
    int fx_release_detection_gpu_x_cam_x(int cam_id);



#if IF_USING_TENSORT

    int net_w = 352;
    int net_h = 352;


    vector<string> labels;
    Config config_v4;

    Precision flagx = ALG_FP32;//INT8;//FP32

    float yuzhi = 0.1;

    std::unique_ptr<Detector> detector;

    std::vector<BatchResult> batch_res;
    int big_width =0;
    int big_height = 0;





    int fx_initial_gpu_rt_x(int gpu_id, int cam_id,  char* path_input);
    int fx_alg_processing_rt_x(int cam_id, char* data, int width, int height, int channel);
   bool fx_get_detection_result_gpu_cam_x_rt_x(struct tr_s_resultinfo* Result, int Num, int cam_id);
   int fx_release_detection_gpu_x_cam_x_rt_x(int cam_id);
#endif





    //处理结果队列列
    int add_chuli_jieguo();
    // int del_chuli_jieguo(int frameID);

    int add_chuli_jieguo_queue();
    int add_chuli_jieguo_queue(int x);
    int del_chuli_jieguo_queue();
    int qingkong_queue();

    Map_resultinfo result_info;
    int curent_frame_id= 0;
    int frame_index = 0;

    for_queue_resultinfo current_str_temp;
#if SHIFOU_SUOFANG
    cv::Mat resize_img;
    int res_w = 1500;
    int res_h = 1500;
    float scale_w= 0;
    float scale_h = 0;
#endif
    //queue<for_queue_resultinfo> q_result_info;


    //map<int , int> m_reslt_state_flag;
    //map<int , QString> m_reslt_task_name;
    // map<int , QString> m_reslt_unit_name;
    // map<int , QString> m_reslt_src_name;

#if USING_PAIJICI_FABU

    // int  m_paizhao_jici = 1;
    int  jubu_m_paizhao_jici = 0;
    str_duo_Queue_resultinfo m_duo_paizhao_Queue;
    void clear_img_queue();


#endif



public slots:
    void  xianshi(cv::Mat  img, int index);
    void  pre_unix_input(cv::Mat img,int index);//接收上一单元数据


signals:
    void send_mat_to_main(cv::Mat  img, int index);
    void send_result_next(cv::Mat  img,  Alg_infor alg_infor, std::vector<tr_s_resultinfo>  next_put_ng);//向下一单元输出
    void send_to_tongji_unit( QString unit_id ,int frameID,  Map_resultinfo & tongji_infor, std::vector<tr_s_resultinfo> next_put_ng);//向统计单元输出
    void send_to_tongji_unit_queue(queue<for_queue_resultinfo> & tongji_infor_queue, std::vector<tr_s_resultinfo> next_put_ng);//向统计单元输出

private slots:
    void on_act_open_image_triggered();





protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QImage m_Image;
    cv::Mat m_img;
    QPixmap background;
    qreal m_ZoomValue = 1.0;
    int m_XPtInterval = 0;
    int m_YPtInterval = 0;
    QPoint m_OldPos;
    bool m_Pressed = false;
    QPoint startPos;
    float zoom=1;  //缩放系数

    void fx_show_img(cv::Mat  &show_temp_img ,int flag);
    std::vector<tr_s_resultinfo>  m_put_ng;

private slots:
    //void onLoadImage(void);
    void onZoomInImage(void);
    void onZoomOutImage(void);
    void onPresetImage(void);


    void on_act_test_img_triggered();

private:
    Ui::jubushibie_form *ui;
};

#endif // JUBUSHIBIE_FORM_H
