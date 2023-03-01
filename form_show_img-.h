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

#include "opencv/highgui.h"

#include "camera_commen.h"
//using namespace  cv;

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

signals:
    void send_draw_win(cv::Mat, int  );
    void send_save_img_sig(cv::Mat img, int flag);


public slots:
    void  xianshi(cv::Mat  img, int index);
    void pre_unix_input(  cv::Mat  img,  Alg_infor alg_infor, std::vector<tr_s_resultinfo>  next_put_ng);//接收上一单元检测数据
    static int show_thread( void *, cv::Mat , int);
    void get_draw_win(cv::Mat, int  );
    void   slot_save_image(cv::Mat img, int flag);

public:
    std::thread img_process_save_image;
    void save_thread(cv::Mat img, int flag);
    int wind_id = 0;
    int index_suiji = 0;
    int if_save_img = 0;
    int if_save_img_ng = 0;
    cv::Mat save_image_ok;

     QImage imgx_tem;

     QString m_save_img_root_path ="./save_image";
     void create_save_img_path();


private:
    Alg_infor m_alg_infor;
    std::vector<tr_s_resultinfo>  m_put_ng;
    void  fx_show_img(cv::Mat  &img, int flag);
    int shif_baocun_yuantu = 0;
    int shif_baocun_ngtu = 0;
     string dir_obj_path_zong = "";
     string dir_obj_path_ok = "";
      string dir_obj_path_ng = "";

    //QGraphicsPixmapItem pixmapRow;
    // QGraphicsScene *scene = nullptr;



protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QImage m_Image;
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






private:
    Ui::Form_show_img *ui;
};

#endif // FORM_SHOW_IMG_H
