#ifndef ZHENGTISHIBIE_FORM_H
#define ZHENGTISHIBIE_FORM_H

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


#include "opencv/highgui.h"

#include "camera_commen.h"
//using namespace  cv;


namespace Ui {
class zhengtishibie_form;
}

class zhengtishibie_form : public QMainWindow
{
    Q_OBJECT

public:
    explicit zhengtishibie_form(QWidget *parent = nullptr, properconfig * input_procfg = nullptr );
    ~zhengtishibie_form();
public:
    MyPixItem *pixItem =nullptr;       //自定义的图元类
    QGraphicsScene *m_graphicsScene = nullptr;  //场景
    QGraphicsView *m_graphicsView= nullptr;

 public:
    properconfig * m_input_procfg =nullptr;
public:
    void show_img_xx(cv::Mat  img);
public slots:
    void  xianshi(cv::Mat  img, int index);


signals:
    void send_mat_to_main(cv::Mat  img, int index);

private slots:
    void my_change();//用于接收放缩值变化

private:
    Ui::zhengtishibie_form *ui;
};

#endif // ZHENGTISHIBIE_FORM_H
