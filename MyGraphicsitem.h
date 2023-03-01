#ifndef MYGRAPHICSITEM_H
#define MYGRAPHICSITEM_H



#include <QWidget>
#include <QtGui>
#include <QPixmap>
#include <QPainter>
#include <QRectF>
#include <QMouseEvent>
#include <QPointF>
#include <QDragEnterEvent>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsItem>

#include <QObject>


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


class MyGraphicsItem :/*public QWidget,*/public QGraphicsItem
{
   // Q_OBJECT
public:
    MyGraphicsItem();
    //void    loadPixmap(QPixmap *pixmap);
    void    loadImage(QImage pixmap);
    QRectF  boundingRect() const;
    void    paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void    wheelEvent(QGraphicsSceneWheelEvent *event);
    void    ResetItemPos();
    void    mousePressEvent(QGraphicsSceneMouseEvent *event);
    void    mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void    mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    qreal   getScaleValue() const;
    void    setQGraphicsViewWH(int nwidth,int nheight);

    void copyItem(  );
    void deleteItem( );



#if 1
protected:

    //void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
   // void paintEvent(QPaintEvent *event) override;
   // void wheelEvent(QWheelEvent *event) override;
   // void mousePressEvent(QMouseEvent *event) override;
   // void mouseMoveEvent(QMouseEvent *event) override;
  //  void mouseReleaseEvent(QMouseEvent *event) override;
   // void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;




private:
   // QImage m_Image;
   // QImage * m_pt_Image = nullptr;
    QPixmap background;
   // qreal m_ZoomValue = 1.0;
   // int m_XPtInterval = 0;
   // int m_YPtInterval = 0;
    //QPoint m_OldPos;
    //bool m_Pressed = false;
    //QPoint startPos;
    //float zoom=1;  //缩放系数


private slots:
    //void onLoadImage(void);
    void onZoomInImage(void);
    void onZoomOutImage(void);
    void onPresetImage(void);



#endif

private:
    qreal       m_scaleValue;
    qreal       m_scaleDafault;
    //QPixmap     m_pix;
    QImage      m_pix;
    int         m_zoomState;
    bool        m_isMove;
    QPointF     m_startPos;

    bool        m_bFlag = false;
};

#endif // MYGRAPHICSITEM_H
