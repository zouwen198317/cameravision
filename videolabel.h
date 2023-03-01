#ifndef VIDEOLABEL_H
#define VIDEOLABEL_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QFileDialog>
#include <QMenu>
#include <QPainter>
#include <QWheelEvent>
#include <QDebug>

#include <QMap>
#include <QTime>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

#include "measure_bc.h"

const int RECTTYPE = 1;
const int ELLIPSETYPE = 2;
const int LINEDRAW = 3;

const int ROITYPE = 4;
const int TEMPLATETYPE = 5;
const int CACHITYPE = 6;




enum EmDirection
{
    Select_image=0,
    Select_Roi,
    Select_RoiTopleft,
    Select_RoiTopright,
    Select_Bottomleft,
    Select_Bottomright,
    Select_Line,
    Select_Leftline,
    Select_Rightline
};



typedef struct str_Line {
    QPoint pt_start;
    QPoint pt_end;

    QPointF pt_img_start;
    QPointF pt_img_end;

    QRect lt_rc;
    QRect rt_rc;

    int xuanzhong = 0;





}xLine;


typedef struct str_Rect {
    QRect main_rc;
    QRectF img_space_rc;
    QRect lt_rc;
    QRect lb_rc;
    QRect rt_rc;
    QRect rb_rc;



}xRect;

typedef struct str_Circle{
    QPoint pt_start;
    QPoint pt_end;
    QRectF rt;

    QPointF pt_img_start;
    QPointF pt_img_end;
    QRectF img_rt;

    QRect img_space_pt;




}xCircle;

typedef struct str_Beishu{

    QRectF origin_scale_img;
    QRectF  scale_img;
    QRectF huabu_r;
    QRectF img_real_r;

    QPointF hb_pt;//画布
    QPointF img_pt;//图像
    QPointF leftTop;

    QPointF rightBotom;

    float fangda_beishu = 1.0;

    float x_s = 1.0;
    float y_s = 1.0;




}Beishu;

class videoLabel : public QLabel
{
    Q_OBJECT
public:
    videoLabel(QWidget *parent = nullptr);
    ~videoLabel( ){};
public:
    void setLocalFilename(QString name);
    void setPaintingType(int num);
    void setBrightness(float num);
    void setContrast(float num);
    void changePaintingState();
    void ChangeBriAndConChangedState();
    bool _rectsIsEmpty();
    bool _ellipsesIsEmpty();
    bool _lineIsEmpty();

    bool _ROIRCIsEmpty();

    QImage Mat2QImage(const cv::Mat& mat);       //Mat2QImage格式转换
    cv::Mat QImageToMat(QImage image);           //QImage2Mat格式转换
    QImage ImageAdjustment(QImage image);    //改变图片的亮度和对比度
    void set_isPainting(bool in_x);
    void cacul_axies();

    QPointF ref2pos(const QPointF pt);
    QPointF cacul_img_pt(const QPointF pt);
    QPointF cacul_pos_pt(const QPointF pt);
    QPointF cacul_img_ref_pt(const QPointF pt);
    int delete_line(int index );
    void  movePoint(QRect *rect, int moveX, int moveY);
    //void  addMsObj( MeasureUnit obj);
    void  delete_addMsObj_pop_back();
    void  delete_addMsObj_pop_front();
    void  draw_obj(QString index, MeasureUnit obj);
    void draw_matchs(int index, std::vector<s_SingleTargetMatch> obj );
    void clear_all_draw();
    // void  pop_( MeasureUnit obj);
    QPointF m_realTimeImgPT;


    void draw_ref_line(QPainter &painterte, QPointF tpLineSt,  QPointF tpLineEd, int iCapLength, int iCapWidth, int nCalipers );



protected:
    void contextMenuEvent(QContextMenuEvent *event) override;   //右键菜单
    void paintEvent(QPaintEvent *event) override;                        //QPaint画图
    void wheelEvent(QWheelEvent *event) override;               //鼠标滚轮滚动
    void mousePressEvent(QMouseEvent *event) override;          //鼠标摁下
    void mouseMoveEvent(QMouseEvent *event) override;           //鼠标松开
    void mouseReleaseEvent(QMouseEvent *event) override;        //鼠标发射事件

public slots:
    void OnSelectImage();       //选择打开图片
    void OnZoomInImage();       //图片放大
    void OnZoomOutImage();      //图片缩小
    void OnPresetImage();       //图片还原
    void OnPaintingback();       //画图回撤


signals:
    void sigSendMouseMovePos(const Beishu &pos);
    void sigSendRightMousePos(const QPoint &pos);
    void sigSendMouseRelease(const QPoint &pos);
    void sigEditPoint(int whichLine,int changeIndex,QPoint pos);


private:
    QPoint _pos;
    QColor _linColor;
    QVector< QVector<QPoint> > *_vectLines;
    QVector<QPoint> _dragLine;
    QVector<bool> _checkBoxList;
    QVector<int> _colorIndexList;
    bool _leftMouseClik;
    bool _polygonUfinish;
    bool _isExitPolygon;



    //Edit point
    QPointF _imgPos;
    //QVector< MeasureUnit > m_list_ms;


public:
    QString LocalFileName;           //图片路径
    QString file_name;
    QString file_path;
    QImage Image;                    //显示的图片
    qreal ZoomValue = 1.0;           //鼠标缩放值
    QRectF image_pos;


    Beishu  m_jl_bh;


    ///鼠标是否在图像范围内按下按键
    bool press=false;
    //鼠标点击位置是否在矩形范围内
    bool selectedRect=false;
    ///保存鼠标在图像上按下的坐标
    QPoint pointPress;
    EmDirection Select;
    int dianji_zuoyou_dian = 0;

    int nayigexingzhuang = -1;
    int xuanzhe_rect = 0;
    int xuanzhe_zhixian = 0;

    QRect m_Img_rt;
    QMap<QString ,  MeasureUnit  >  m_list_ms;
    QMap<int ,std::vector<s_SingleTargetMatch> /*m_vecSingleTargetData;*/> m_list_matchs;


public:
    // QImage Image;                    //显示的图片
    cv::Mat image_Mat;                   //改变的图片
    // qreal ZoomValue = 1.0;           //鼠标缩放值
    int XPtInterval = 0;             //平移X轴的值
    int YPtInterval = 0;             //平移Y轴的值
    int paintingType = 0;            //绘图种类
    QPoint OldPos;                   //旧的鼠标位置
    bool Pressed = false;            //鼠标是否被摁压
    bool isPainting = false;         //是否在画图
    bool isBriAndConChanged = false; //亮度对比度是否改变
    // QString LocalFileName;           //图片名

    bool isROI_rect_create = false;
    bool isTemplate_create = false;
    bool isLine_create = false;

    QVector<xRect> _roi_rects;
    QVector<xRect> _template_rects;
    QVector<xLine> _measure_lineV;
    // QVector<xLine> _lineV;      //直线容器

    QVector<QRect> _rects;           //矩形容器
    QVector<QRect> _ellipses;        //椭圆形容器

    QVector<xCircle> _circle;      //圆形容器

    QVector<int>shapeType;           //记录绘图类型
    float brightness = 0;            //亮度
    float contrast = 1;              //对比度

};

#endif // VIDEOLABEL_H
