#ifndef IMAGELOOK_H
#define IMAGELOOK_H

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

//using namespace cv;


namespace Ui {
class ImageLook;
}

class ImageLook : public QWidget
{
    Q_OBJECT
public:
    explicit ImageLook(QWidget *parent = 0);
    ~ImageLook();
    void showImage(cv::Mat,QString,QString);
public slots:
    void  resive(cv::Mat & img ,int index);
    void create_img_show();

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *event);

private slots:
    void my_change();//用于接收放缩值变化
    void on_spinBox_valueChanged(int arg1);//系统自动生成槽函数，检测对应控件值
    void resetzoom();//恢复图像
    void on_B_Boost_clicked();//放大按钮
    void on_B_Reduce_clicked();//缩小按钮

public:
    QPoint last; //点
    MyPixItem *pixItem =nullptr;       //自定义的图元类
    QGraphicsScene *m_graphicsScene = nullptr;  //场景
    QGraphicsView *m_graphicsView= nullptr; ;
private:
    Ui::ImageLook *ui;



    int wt;
    int ht;
    IplImage *pImg;
    cv::Mat pImgs;
    QString globNames;
    QString controlNameGlob;


};

#endif // IMAGELOOK_H
