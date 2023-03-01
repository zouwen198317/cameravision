#ifndef FORM_SHOW_IMG_H
#define FORM_SHOW_IMG_H

#include "imagelook.h"
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
#include <QMenu>
#include <QContextMenuEvent>
#include <QStyleOption>
#include <QPainter>
#include <QFileDialog>
#include<QVector>

#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>

#include "camera_commen.h"
#include <opencv2/opencv.hpp>
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



#include <QGraphicsView>
#include <QGraphicsPixmapItem>
//#include "ImageAcquisition.h"
#include "MyGraphicsitem.h"


#include "opencv/highgui.h"



#define NO_USING_LABEL 0

//#include "show_result_win.h"
using namespace  cv;

namespace Ui {
class Form_show_img;
}

class Form_show_img : public QWidget
{
    Q_OBJECT

public:
    explicit Form_show_img(QWidget *parent = nullptr);
    ~Form_show_img();

    // ImageLook *imgL;
public:
    void show_img_xx(cv::Mat  img, int flag);

    QVector<cv::Mat> xs_vc;
    QVector<cv::Mat> bc_img_vc;
    QVector<int> bc_img_flag;
    int xianshi_haishi_chuli = 0;
    int m_flag_xs_t = 1;
    std::thread m_t1;
    std::thread m_t2;
    int tupian = 0;
    void th_xs(void * ptr);
    void th_bcun(void * ptr);
    QImage Mat2QImage(const cv::Mat &mat);


    QGraphicsView*  getGraphicsView() const;

signals:
    void send_draw_win(cv::Mat, int  );
    void send_save_img_sig(cv::Mat img, int flag);
    void send_show_win_mat(cv::Mat mt);
    void send_slotDispImage(QImage Img);


public slots:
    void slotDispImage(QImage Img);
    void  xianshi(cv::Mat  img, int index);
    void pre_unix_input(  cv::Mat  img,  Alg_infor alg_infor, std::vector<tr_s_resultinfo>  next_put_ng);//接收上一单元检测数据
    //static int show_thread( void *, cv::Mat , int);

    void   slot_save_image(cv::Mat img, int flag);

public:
    unsigned int        m_DisplayCount=0;

    std::thread img_process_save_image;

    int wind_id = 0;
    int index_suiji = 0;
    int if_save_img = 0;
    int if_save_img_ng = 0;
    int budai_label = 0;
    cv::Mat save_image_ok;

    QString camera_name ="";

    QImage imgx_tem;
    QString m_save_img_root_path ="./save_image";
    void create_save_img_path();

    //show_result_win * ptr_show_resultx= nullptr;
    // show_result_win  *ckou;
    // int number_show = 0;
    void show_dialg(cv::Mat img);
    map<int,cv::Mat> smap_mat;
    cv::Mat  huanc;

private:
    Alg_infor m_alg_infor;
    // std::vector<tr_s_resultinfo>  m_put_ng;
    int jieguo_c = 0;
    std::vector<tr_s_resultinfo>  hc_m_put_ng;
    QVector< std::vector<tr_s_resultinfo> > x_m_put_ng;

    int shif_baocun_yuantu = 0;
    int shif_baocun_ngtu = 0;
    string dir_obj_path_zong = "";
    string dir_obj_path_ok = "";
    string dir_obj_path_ng = "";

    //QGraphicsPixmapItem pixmapRow;
    // QGraphicsScene *scene = nullptr;


#if NO_USING_LABEL
protected:

    void contextMenuEvent(QContextMenuEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QImage m_Image;
   // QImage * m_pt_Image = nullptr;
    QPixmap background;
    qreal m_ZoomValue = 1.0;
    int m_XPtInterval = 0;
    int m_YPtInterval = 0;
    QPoint m_OldPos;
    bool m_Pressed = false;
    QPoint startPos;
    float zoom=1;  //缩放系数

private slots:
    //void onLoadImage(void);
    void onZoomInImage(void);
    void onZoomOutImage(void);
    void onPresetImage(void);


#endif



private:
    Ui::Form_show_img *ui;

    QGraphicsScene      *scene;                      //QGraphicsScene，在graphicsview高效显示图片
    MyGraphicsItem      *loadedPixmapItem;            //自定义MyGraphicsItem
};

#endif // FORM_SHOW_IMG_H
