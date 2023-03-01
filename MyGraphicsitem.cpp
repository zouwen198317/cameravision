#include "MyGraphicsitem.h"
#include<QSpinBox>
#include<QWidgetAction>

#include <QObject>    // 如果没有QObject的头文件也需要包含
#include <QAction>

MyGraphicsItem::MyGraphicsItem()
{
    setAcceptDrops(true);//接受拖拽
    m_scaleValue = 0;
    m_scaleDafault = 0;
    m_isMove = false;
}

void MyGraphicsItem::loadImage(QImage image)
{
    m_pix = image;
    setScale(m_scaleValue);
}


QRectF MyGraphicsItem::boundingRect() const
{
    return QRectF(-m_pix.width() / 2, -m_pix.height() / 2, m_pix.width(), m_pix.height());
}

void MyGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->drawImage(-m_pix.width() / 2, -m_pix.height() / 2, m_pix);
}

//void MyGraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
//{
//    //ResetItemPos();
//}

void MyGraphicsItem::onZoomInImage(void)
{
    //m_ZoomValue += 0.2;
    m_scaleValue*=1.1;
    // this->update();
}

void MyGraphicsItem::onZoomOutImage(void)
{
    //每次缩小10%
    m_scaleValue*=0.9;
    //    if (m_ZoomValue <= 0)
    //    {
    //        m_ZoomValue += 0.2;
    //        return;
    //    }

   // this->update();
}

void MyGraphicsItem::onPresetImage(void)
{
    m_scaleDafault = 1.0;
    //每次缩小10%
    ResetItemPos();
}



void MyGraphicsItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    if((event->delta() > 0)&&(m_scaleValue >= 50))//最大放大到原始图像的50倍
    {
        return;
    }
    else if((event->delta() < 0)&&(m_scaleValue <= m_scaleDafault/2))//图像缩小到(自适应大小/2) 之后就不继续缩小
    {
        //ResetItemPos();//重置图片大小和位置，使之自适应控件窗口大小
    }
    else
    {
        qreal qrealOriginScale = m_scaleValue;
        //鼠标滚轮向前滚动
        if(event->delta() > 0)
        {
            //每次放大10%
            m_scaleValue*=1.1;
        }
        else
        {
            //每次缩小10%
            m_scaleValue*=0.9;
        }
        setScale(m_scaleValue);
        if(event->delta() > 0)
        {
            moveBy(-event->pos().x()*qrealOriginScale*0.1, -event->pos().y()*qrealOriginScale*0.1);//使图片缩放的效果看起来像是以鼠标所在点为中心进行缩放的
        }
        else
        {
            moveBy(event->pos().x()*qrealOriginScale*0.1, event->pos().y()*qrealOriginScale*0.1);//使图片缩放的效果看起来像是以鼠标所在点为中心进行缩放的
        }
    }
}

//void MyGraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
//{
//    if (event->reason() == QGraphicsSceneContextMenuEvent::Mouse)
//    {
//        QMenu menu;




//           //QAction *loadImage = new QAction(tr("Load Image"));
//           // QObject::connect(loadImage, &QAction::triggered, this, &ImageView::onLoadImage);
//           // menu->addAction(loadImage);
//           // menu->addSeparator();

//           QAction *zoomInAction = new QAction(QString::fromLocal8Bit("缩小Zoom In"));
//           QObject::connect(zoomInAction, &QAction::triggered, this, &MyGraphicsItem::onZoomInImage);
//           menu.addAction(zoomInAction);

//           QAction *zoomOutAction = new QAction(QString::fromLocal8Bit("放大Zoom Out"));
//           QObject::connect(zoomOutAction, &QAction::triggered, this, &MyGraphicsItem::onZoomOutImage);
//           menu.addAction(zoomOutAction);

//           QAction *presetAction = new QAction(QString::fromLocal8Bit("复位Preset"));
//           QObject::connect(presetAction, &QAction::triggered, this, &MyGraphicsItem::onPresetImage);
//           menu.addAction(presetAction);


//        // 在光标位置显示菜单
//        menu.exec(event->screenPos());



//    }
//}

void MyGraphicsItem::copyItem()
{
    qDebug() << "Copying item...";
}

void MyGraphicsItem::deleteItem()
{
    qDebug() << "Deleting item...";
}




void MyGraphicsItem::ResetItemPos()
{
    //缩放比例回到一开始的自适应比例
    m_scaleValue = m_scaleDafault;
    //缩放到一开始的自适应大小
    setScale(m_scaleDafault);
    //setPos(0,0);
}

void MyGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button()== Qt::LeftButton)
    {
        //鼠标左击时，获取当前鼠标在图片中的坐标，
        m_startPos = event->pos();
        //标记鼠标左键被按下
        m_isMove = true;
    }
    else if(event->button() == Qt::RightButton)
    {
        //右击鼠标重置大小
        ResetItemPos();
    }
}

void MyGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_isMove)
    {
        QPointF point = (event->pos() - m_startPos)*m_scaleValue;
        moveBy(point.x(), point.y());
    }
}

void MyGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    m_isMove = false;//标记鼠标左键已经抬起
}

qreal MyGraphicsItem::getScaleValue() const
{
    return m_scaleValue;
}

void MyGraphicsItem::setQGraphicsViewWH(int nwidth, int nheight)
{
    if(m_bFlag==false)
    {
        m_bFlag = true;
    }
    else
    {
        return ;
    }
    int nImgWidth = m_pix.width();
    int nImgHeight = m_pix.height();
    qreal temp1 = nwidth*1.0/nImgWidth;
    qreal temp2 = nheight*1.0/nImgHeight;

    m_scaleDafault = temp1>temp2?temp2:temp1;

    setScale(m_scaleDafault);
    m_scaleValue = m_scaleDafault;
}
