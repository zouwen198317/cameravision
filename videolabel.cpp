#include "videolabel.h"
#include<iostream>

videoLabel::videoLabel(QWidget *parent):QLabel (parent)
{

    this->setMouseTracking(true);
}

void videoLabel::setLocalFilename(QString name)
{
    LocalFileName = name;
}

void videoLabel::setPaintingType(int num)
{
    paintingType = num;
    return;
}

void videoLabel::setBrightness(float num)
{
    brightness = num;
    isBriAndConChanged = true;
    update();
}

void videoLabel::setContrast(float num)
{
    contrast = num;
    isBriAndConChanged = true;
    update();
}

void videoLabel::changePaintingState()
{
    isPainting == false?isPainting=true:isPainting=false;
}

void videoLabel::ChangeBriAndConChangedState()
{
    isBriAndConChanged == false?isBriAndConChanged=true:isBriAndConChanged=false;
}


bool videoLabel::_rectsIsEmpty()
{
    if(_rects.size()==0)
    {
        return true;
    }
    else {
        return false;
    }
}

bool videoLabel::_lineIsEmpty()
{
    if(_measure_lineV.size()==0)
    {
        return true;
    }
    else {
        return false;
    }

}

bool videoLabel::_ROIRCIsEmpty()
{

    if(_roi_rects.size()==0)
    {
        return true;
    }
    else {
        return false;
    }
}
bool videoLabel::_ellipsesIsEmpty()
{
    if(_ellipses.size()==0)
    {
        return true;
    }
    else {
        return false;
    }
}

QImage videoLabel::Mat2QImage(const cv::Mat &mat)
{
    //8位，通道数为1
    if (mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        image.setColorCount(256);
        for (int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }

        uchar *pSrc = mat.data;
        for (int row = 0; row < mat.rows; row++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    //8位，通道数为3
    else if (mat.type() == CV_8UC3)
    {
        const uchar *pSrc = (const uchar*)mat.data;
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    //8位，通道数为4
    else if (mat.type() == CV_8UC4)
    {
        qDebug() << "CV_8UC4";

        const uchar *pSrc = (const uchar*)mat.data;
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    //32位，通道数为3
    else if (mat.type() == CV_32FC3)
    {
        cv::Mat temp;
        mat.convertTo(temp, CV_8UC3);

        const uchar *pSrc = (const uchar*)temp.data;
        QImage image(pSrc, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

cv::Mat videoLabel::QImageToMat(QImage image)
{
    cv::Mat mat;
    switch (image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
         std::cout<<1<<endl;
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
         std::cout<<2<<endl;
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
    {
        std::cout<<3<<endl;
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    }

    case QImage::Format_Grayscale8:
    {

        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    }
    default://Format_Grayscale8
        std::cout<<"No format"<<image.format()<<std::endl;
        break;
    }
    return mat;
}

QImage videoLabel::ImageAdjustment(QImage image)
{
    //    Mat image_Mat = QImageToMat(image);
    image_Mat = cv::imread(LocalFileName.toLocal8Bit().toStdString());
    image_Mat.convertTo(image_Mat, -1,static_cast<double>(contrast),static_cast<double>(brightness));
    return Mat2QImage(image_Mat);
}

void videoLabel::contextMenuEvent(QContextMenuEvent *event)
{
    QPoint pos = event->pos();
    pos = this->mapToGlobal(pos);
    QMenu *menu = new QMenu(this);

    QAction *loadImage = new QAction(this);
    loadImage->setText(QString::fromLocal8Bit("选择图片"));
    connect(loadImage, &QAction::triggered, this, &videoLabel::OnSelectImage);
    menu->addAction(loadImage);
    menu->addSeparator();

    QAction *zoomInAction = new QAction(this);
    zoomInAction->setText(QString::fromLocal8Bit("放大"));
    connect(zoomInAction, &QAction::triggered, this, &videoLabel::OnZoomInImage);
    menu->addAction(zoomInAction);

    QAction *zoomOutAction = new QAction(this);
    zoomOutAction->setText(QString::fromLocal8Bit("缩小"));
    connect(zoomOutAction, &QAction::triggered, this, &videoLabel::OnZoomOutImage);
    menu->addAction(zoomOutAction);

    QAction *presetAction = new QAction(this);
    presetAction->setText(QString::fromLocal8Bit("还原"));
    connect(presetAction, &QAction::triggered, this, &videoLabel::OnPresetImage);
    menu->addAction(presetAction);
    menu->addSeparator();

    QAction *pantingBack = new QAction(this);
    pantingBack->setText(QString::fromLocal8Bit("撤回"));
    connect(pantingBack, &QAction::triggered, this, &videoLabel::OnPaintingback);
    menu->addAction(pantingBack);

    menu->exec(pos);
}
void  videoLabel::draw_obj(QString index,MeasureUnit obj)
{
    m_list_ms[index] = obj;

    update();

    return;
}

void videoLabel::draw_matchs(int index, std::vector<s_SingleTargetMatch> obj )
{

    m_list_matchs[index] = obj;
    update();
    return;
}

#define M_PI       3.14159

#define RAD2DEG(x) ((x)*180./M_PI)  //弧度转角度
#define DEG2RAD(x) ((x)*M_PI/180.)  //角度转弧度


void videoLabel::draw_ref_line( QPainter & painterte,QPointF tpLineSt,  QPointF tpLineEd,int iCapLength, int iCapWidth, int nCalipers )
{



        if (Image.isNull()) return;

        const int X = Image.width(), Y = Image.height();

        //判断越界
        if (tpLineSt.x() < 0 || tpLineSt.y() < 0 || tpLineSt.x() >X || tpLineSt.y()>Y || tpLineEd.x()<0 || tpLineEd.y()<0 || tpLineEd.x()>X || tpLineEd.y()>Y) {
            return ;
        }

        //主轴倾角
        double tQ;
        tQ = atan2(tpLineEd.y() - tpLineSt.y(), tpLineEd.x() - tpLineSt.x());

        //参数限制
        iCapWidth = iCapWidth <= 0 ? iCapWidth = 0 : iCapWidth - 1;
        iCapLength = iCapLength <= 0 ? 8 : iCapLength;
        nCalipers = nCalipers <= 0 ? 5 : nCalipers;

        //直线上的坐标
        float L = (float)cv::norm(cv::Point2d(tpLineSt.x(), tpLineSt.y()) - cv::Point2d(tpLineEd.x(), tpLineEd.y()));
        //步长
        float plusStep = (L - (float)nCalipers*(float)iCapWidth) / ((float)nCalipers + 1.0f);

        for (int n = 0; n <= nCalipers; n++)
        {

            //采样位置
            cv::Point2f midLine((float)tpLineSt.x() + ((float)n*(plusStep + (float)iCapWidth) - (float)iCapWidth + (float)iCapWidth / 2.0f) * (float)cos(tQ),
                                (float)tpLineSt.y() + ((float)n*(plusStep + (float)iCapWidth) - (float)iCapWidth + (float)iCapWidth / 2.0f) * (float)sin(tQ));
            //各位置采样路径
            cv::Point2f midLineStart, midLineEnd;
            midLineStart = cv::Point2f(-iCapLength*(float)cos(tQ + 1*CV_PI / 2.0) + midLine.x, -iCapLength*(float)sin(tQ + 1*CV_PI / 2.0) + midLine.y);
            midLineEnd = cv::Point2f(iCapLength*(float)cos(tQ + 1* CV_PI / 2) + midLine.x, iCapLength*(float)sin(tQ + 1*CV_PI / 2.0) + midLine.y);

            QPointF pt1(midLineStart.x, midLineStart.y);
            QPointF pt2(midLineEnd.x, midLineEnd.y);
            //QLineF l1(pt1,pt2);
            //m_obj.m_jiantou_xian.append(l1);

            painterte.drawLine( cacul_img_ref_pt(pt1), cacul_img_ref_pt(pt2));

        }





}

void videoLabel::paintEvent(QPaintEvent *event)
{
    QTime qTime;
    qTime.start();

    // auto rect = event->rect();

    QPainter painterte(this);
    painterte.setRenderHint(QPainter::Antialiasing);
    painterte.setPen(QPen(Qt::red,5));

    QPen pen(QColor(Qt::blue),2);

    QPainter painter(this);
    painter.setPen(QPen(Qt::green,2)); //设置画笔粗细为3，颜色为红色

    if (!Image.isNull())
    {

        // 根据窗口计算应该显示的图片的大小
        int width = qMin(Image.width(), this->width());
        int height = width * 1.0 / (Image.width() * 1.0 / Image.height());
        height = qMin(height, this->height());
        width = height * 1.0 * (Image.width() * 1.0 / Image.height());


        // 平移
        painter.translate(this->width() / 2 + XPtInterval, this->height() / 2 + YPtInterval);
        // qDebug()<<QPoint(XPtInterval, YPtInterval);

        //qDebug()<<"------>"<<ZoomValue;
        // 缩放
        painter.scale(ZoomValue, ZoomValue);//垂直水平两个方向

        //改变亮度和对比度
        if(isBriAndConChanged)
        {
            Image = ImageAdjustment(Image);
            isBriAndConChanged = false;
        }

        // 绘制图像
        QRect picRect(-width / 2, -height / 2, width, height);

        // painter.begin(this);
        // ...//绘图操作
        // painter.end();

        painter.drawImage(picRect, Image);
        // painter.drawRect(picRect);

        image_pos = picRect;

    }

    //绘画已有图形

    QPainter painter2(this);
    painter2.setPen(QPen(Qt::red,2)); //设置画笔粗细为3，颜色为红色

    painter2.translate(this->width() / 2 + XPtInterval, this->height() / 2 + YPtInterval);
    painter2.scale(ZoomValue, ZoomValue);//垂直水平两个方向

    if (!Image.isNull())
    {
        QMap< QString,   MeasureUnit >::const_iterator it = m_list_ms.constBegin();
        while (it != m_list_ms.constEnd())
        {

            MeasureUnit  temp_m_list_ms = it.value();

            painter2.setPen(QPen(Qt::blue,1)); //设置画笔粗细为3，颜色为红色

            for(int r =0; r <temp_m_list_ms.fppx.size(); r++ )
            {

                for (int j = 0; j < 4; j++)
                {
                    if(temp_m_list_ms.shifouhuajiantou)
                    {
                        painter2.drawLine( cacul_img_ref_pt(temp_m_list_ms.fppx[r].xppt[j].pt1), cacul_img_ref_pt(temp_m_list_ms.fppx[r].xppt[j].pt2));
                    }


                }

            }

            painter2.setPen(QPen(Qt::red,1)); //设置画笔粗细为3，颜色为红色


            for(int r =0; r <temp_m_list_ms.m_jiantou_xian.size(); r++ )
            {
                QLineF k;
                k.setP1(cacul_img_ref_pt(temp_m_list_ms.m_jiantou_xian[r].p1()));
                k.setP2(cacul_img_ref_pt(temp_m_list_ms.m_jiantou_xian[r].p2()));


                if(temp_m_list_ms.shifouhuajiantou)
                {
                    painter2.drawLine( k);
                }

                QLineF line(k.p1(), k.p2());
                double angle = std::atan2(-line.dy(), line.dx());
                qreal arrowSize = 3;
                QPointF arrowP1 = line.p2() - QPointF(sin(angle + PI / 3) * arrowSize,
                                                      cos(angle + PI / 3) * arrowSize);
                QPointF arrowP2 = line.p2() - QPointF(sin(angle + PI - PI / 3) * arrowSize,
                                                      cos(angle + PI - PI / 3) * arrowSize);
                QPolygonF arrowHead;
                arrowHead  << line.p2() << arrowP2 << arrowP1;


                if(temp_m_list_ms.shifouhuajiantou)
                {

                    painter2.drawLine(line);
                    painter2.drawLine( k.p2(), arrowP1);
                    painter2.drawLine( k.p2(), arrowP2);
                }




            }

            for(int r =0; r <temp_m_list_ms.m_v_pt.size(); r++ )
            {
                painter2.setPen(QPen(Qt::green,1)); //设置画笔粗细为3，颜色为红色



                painter2.drawPoint( cacul_img_ref_pt(temp_m_list_ms.m_v_pt[r]));

                painter2.setPen(QPen(Qt::red,1)); //设置画笔粗细为3，颜色为红色

            }

            painter2.setPen(QPen(Qt::blue,2)); //设置画笔粗细为3，颜色为红色
            QLineF k;
            k.setP1(cacul_img_ref_pt(temp_m_list_ms.m_zhouxian.p1()));
            k.setP2(cacul_img_ref_pt(temp_m_list_ms.m_zhouxian.p2()));

            if(temp_m_list_ms.shifouhuajiantou)
            {
                painter2.drawLine( k);
            }

            QLineF line(k.p1(), k.p2());
            double angle = std::atan2(-line.dy(), line.dx());
            qreal arrowSize = 5;
            QPointF arrowP1 = line.p2() - QPointF(sin(angle + PI / 3) * arrowSize,
                                                  cos(angle + PI / 3) * arrowSize);
            QPointF arrowP2 = line.p2() - QPointF(sin(angle + PI - PI / 3) * arrowSize,
                                                  cos(angle + PI - PI / 3) * arrowSize);
            QPolygonF arrowHead;
            arrowHead  << line.p2() << arrowP2 << arrowP1;

            if(temp_m_list_ms.shifouhuajiantou)
            {
                painter2.drawLine(line);
                painter2.drawLine( k.p2(), arrowP1);
                painter2.drawLine( k.p2(), arrowP2);
            }


            painter2.setPen(QPen(Qt::red,1)); //设置画笔粗细为3，颜色为红色


            ++it;
        }

        QMap<int,   std::vector<s_SingleTargetMatch>  >::const_iterator it_m = m_list_matchs.constBegin();
        while (it_m != m_list_matchs.constEnd())
        {

            std::vector<s_SingleTargetMatch> m_vecSingleTargetData = it_m.value();
            int iSize = (int)m_vecSingleTargetData.size ();

            for (int i = 0; i < iSize; i++)
            {

                painter2.setPen(QPen(Qt::blue,1)); //设置画笔粗细为3，颜色为红色
                QPoint ptLT (m_vecSingleTargetData[i].ptLT.x, m_vecSingleTargetData[i].ptLT.y);
                QPoint ptLB (m_vecSingleTargetData[i].ptLB.x, m_vecSingleTargetData[i].ptLB.y);
                QPoint ptRB (m_vecSingleTargetData[i].ptRB.x, m_vecSingleTargetData[i].ptRB.y);
                QPoint ptRT (m_vecSingleTargetData[i].ptRT.x, m_vecSingleTargetData[i].ptRT.y);
                QPoint ptC (m_vecSingleTargetData[i].ptCenter.x, m_vecSingleTargetData[i].ptCenter.y);




                painter2.drawLine( cacul_img_ref_pt(ptLT), cacul_img_ref_pt(ptLB));
                painter2.drawLine( cacul_img_ref_pt(ptLB), cacul_img_ref_pt(ptRB));
                painter2.drawLine( cacul_img_ref_pt(ptRB), cacul_img_ref_pt(ptRT));
                painter2.drawLine( cacul_img_ref_pt(ptRT), cacul_img_ref_pt(ptLT));

                //左上及角落邊框
                QPointF ptDis1, ptDis2;
                if ( Image.width() > Image.height())
                {
                    ptDis1 = (ptLB - ptLT) / 3;
                    ptDis2 = (ptRT - ptLT) / 3 * (Image.height() / (float)Image.width() );
                }
                else
                {
                    ptDis1 = (ptLB - ptLT) / 3 * (Image.width() / (float)Image.height());
                    ptDis2 = (ptRT - ptLT) / 3;
                }

                painter2.setPen(QPen(Qt::green,1)); //设置画笔粗细为3，颜色为红色


                painter2.drawLine( cacul_img_ref_pt(ptLT), cacul_img_ref_pt( ptLT + ptDis1 / 2));
                painter2.drawLine( cacul_img_ref_pt(ptLT), cacul_img_ref_pt(  ptLT + ptDis2 / 2 ));
                painter2.drawLine( cacul_img_ref_pt(ptRT), cacul_img_ref_pt(  ptRT + ptDis1 / 2));
                painter2.drawLine( cacul_img_ref_pt(ptRT), cacul_img_ref_pt( ptRT - ptDis2 / 2));
                painter2.drawLine( cacul_img_ref_pt(ptRB), cacul_img_ref_pt( ptRB - ptDis1 / 2));
                painter2.drawLine( cacul_img_ref_pt(ptRB), cacul_img_ref_pt( ptRB - ptDis2 / 2));

                painter2.drawLine( cacul_img_ref_pt(ptLB), cacul_img_ref_pt( ptLB - ptDis1 / 2));
                painter2.drawLine( cacul_img_ref_pt(ptLB), cacul_img_ref_pt( ptLB + ptDis2 / 2));


                painter2.setPen(QPen(Qt::red,2)); //设置画笔粗细为3，颜色为红色

                int length_cross = 30;
                QString infor =QString::number(i) +"  Score:" + QString::number(m_vecSingleTargetData[i].dMatchScore,'f',4) + " Angle:" + QString::number(m_vecSingleTargetData[i].dMatchedAngle,'f',4);
                painter2.drawText( cacul_img_ref_pt ((ptLT + ptRT) / 2),infor);

                QPointF ptCRoss(m_vecSingleTargetData[i].ptCenter.x, m_vecSingleTargetData[i].ptCenter.y);

                QLineF L1=QLineF(cacul_img_ref_pt(ptCRoss - QPointF(length_cross, 0)), cacul_img_ref_pt( ptCRoss + QPointF(length_cross, 0) ));


                double l1_ange = L1.angle() + m_vecSingleTargetData[i].dMatchedAngle;
                L1.setAngle( l1_ange);

                QLineF L2=QLineF( cacul_img_ref_pt(ptCRoss - QPointF(0, length_cross)), cacul_img_ref_pt( ptCRoss + QPointF(0, length_cross) ) );

                double l2_ange = L2.angle() + m_vecSingleTargetData[i].dMatchedAngle;
                L2.setAngle( l2_ange);

                painter2.drawLine(L1 );
                painter2.drawLine(L2);

                painter2.setPen(QPen(Qt::blue,1)); //设置画笔粗细为3，颜色为红色


            }

            painter2.setPen(QPen(Qt::red,1)); //设置画笔粗细为3，颜色为红色

            ++it_m;
        }


    }



    int i1=0,i2=0,i3=0, i4 = 0;;//各种图形的索引
    for(int c = 0;c<shapeType.size();c++)
    {
        if(shapeType.at(c)==RECTTYPE)
        {
            painter2.drawRect(_rects.at(i1++));
        }
        else if(shapeType.at(c)==ELLIPSETYPE)
        {
            painter2.drawEllipse(_ellipses.at(i2++));
        }
        else if(shapeType.at(c)==LINEDRAW)
        {

            painter2.drawLine(_measure_lineV.at(i3).pt_start,_measure_lineV.at(i3).pt_end);

            QLineF lk(_measure_lineV.at(i3).pt_start,_measure_lineV.at(i3).pt_end);

            QPen pen(QColor(Qt::green),2);

            painter2.setPen(pen);

            xLine xl = _measure_lineV.at(i3);

            painter2.drawRect(xl.lt_rc);
            painter2.drawRect(xl.rt_rc);



            painter2.drawText(xl.pt_start,QString::number(xl.pt_img_start.x(),'f',2)+" " + QString::number(xl.pt_img_start.y(),'f',2  ));
            painter2.drawText(xl.pt_end,QString::number(xl.pt_img_end.x(),'f',2)+" " + QString::number(xl.pt_img_end.y(),'f',2  ));

            QPointF centorPT =QPointF(xl.pt_start +  (xl.pt_end - xl.pt_start ) /2);

            painter2.drawText(centorPT, QString::number(i3)+ " ang:" + QString::number(lk.angle(),'f',3));



            QPen peny(QColor(Qt::yellow),1);

            painter2.setPen(peny);
            if(_measure_lineV.at(i3).xuanzhong == 1)
            {
                draw_ref_line( painter2,xl.pt_img_start,  xl.pt_img_end,500, 20, 15 );
            }





            QPen penx(QColor(Qt::red),2);

            painter2.setPen(penx);



            i3++;
        }else if(shapeType.at(c)==ROITYPE)
        {


            xRect roi_ =_roi_rects.at(i4++);
            painter2.drawRect(roi_.main_rc);

            QPen pen(QColor(Qt::green),2);

            painter2.setPen(pen);


            painter2.drawRect(roi_.lt_rc);
            painter2.drawRect(roi_.lb_rc);
            painter2.drawRect(roi_.rt_rc);
            painter2.drawRect(roi_.rb_rc);

            //QString str =QString::

            painter2.drawText(roi_.main_rc.topLeft(),QString::number(roi_.img_space_rc.topLeft().x(),'f',2)+" " + QString::number(roi_.img_space_rc.topLeft().y(),'f',2));

            painter2.drawText(roi_.main_rc.topRight(),QString::number(roi_.img_space_rc.topRight().x(),'f',2)+" " + QString::number(roi_.img_space_rc.topRight().y(),'f',2));
            painter2.drawText(roi_.main_rc.bottomLeft(),QString::number(roi_.img_space_rc.bottomLeft().x(),'f',2)+" " + QString::number(roi_.img_space_rc.bottomLeft().y(),'f',2));

            painter2.drawText(roi_.main_rc.bottomRight(),QString::number(roi_.img_space_rc.bottomRight().x(),'f',2)+" " + QString::number(roi_.img_space_rc.bottomRight().y(),'f',2));

            QPen penx(QColor(Qt::red),2);

            painter2.setPen(penx);



        }
    }

    //    //根据是否需要的绘画状态进行绘图
    //    if (isPainting&&(Pressed==true))
    //    {
    //        if(shapeType.last()==RECTTYPE)
    //        {
    //            painter2.drawRect(_rects.last());
    //        }
    //        if(shapeType.last()==ELLIPSETYPE)
    //        {
    //            painter2.drawEllipse(_ellipses.last());
    //        }
    //        if(shapeType.last()==LINEDRAW)
    //        {
    //            painter2.drawLine(_lineV.last().pt_start,_lineV.last().pt_end);
    //        }
    //    }

    //qDebug()<<QString::fromLocal8Bit("update()刷新时间：")<<qTime.elapsed()<<"ms";


}

void videoLabel::clear_all_draw()
{
    if( this->m_list_ms.size() >0)
    {
        this-> m_list_ms.clear();

    }

    if( this->m_list_matchs.size() >0)
    {
        this->m_list_matchs.clear();

    }

    if( this->shapeType.size()>0)
    {


        if( this->shapeType.last()==RECTTYPE)
        {

            this->_rects.pop_back();
            this->shapeType.pop_back();

        }
        else if(  this->shapeType.last()==ELLIPSETYPE)
        {

            this->_ellipses.pop_back();
            this->shapeType.pop_back();
        }
        else if(  this->shapeType.last()==LINEDRAW)
        {

            this->_measure_lineV.pop_back();
            this->shapeType.pop_back();



        }else if( this->shapeType.last()==ROITYPE)
        {
            this->_roi_rects.pop_back();
            this->shapeType.pop_back();

        }


    }
    update();
}
int videoLabel::delete_line(int index )
{
    if(_measure_lineV.size() >0)
    {
        _measure_lineV.pop_back();
        shapeType.pop_back();
        update();
    }


    return 0;

}
void videoLabel::wheelEvent(QWheelEvent *event)
{
    int value = event->delta();
    if (value > 0)  //放大
        OnZoomInImage();
    else            //缩小
        OnZoomOutImage();



    if(_measure_lineV.size() >0)
    {

        for(int i = 0; i < _measure_lineV.size(); i++)
        {
            _measure_lineV[i].xuanzhong = 0;
        }

    }

    update();
}

QPointF videoLabel::ref2pos(const QPointF pt)
{
    QPoint yidong =QPoint(XPtInterval,YPtInterval);

    QPointF returnPt = pt*ZoomValue + yidong + QPoint(this->width()/2,this->height()/2);
    return returnPt;
    //   lastRect.setTopLeft((ev->pos()-QPoint(this->width()/2,this->height()/2)-QPoint(XPtInterval,YPtInterval))/ZoomValue);//记录鼠标的坐标(新矩形的左上角坐标)
}
void videoLabel::mousePressEvent(QMouseEvent *ev)
{



    //开始绘图
    if(isPainting)
    {

        //xuanzhe_rect = 0;
        Pressed = true;
        this->setCursor(Qt::CrossCursor);
        if(ev->button()==Qt::LeftButton)
        {

            m_realTimeImgPT =cacul_img_pt( ev->pos());

            if(paintingType == RECTTYPE)
            {
                QRect rect;//鼠标按下，矩形开始
                _rects.append(rect);
                shapeType.append(RECTTYPE);
                QRect& lastRect = _rects.last();//拿到新矩形
                lastRect.setTopLeft((ev->pos()-QPoint(this->width()/2,this->height()/2)-QPoint(XPtInterval,YPtInterval))/ZoomValue);//记录鼠标的坐标(新矩形的左上角坐标)
            }
            else if (paintingType == ELLIPSETYPE)
            {
                QRect ellipse;//鼠标按下，圆形开始
                _ellipses.append(ellipse);
                shapeType.append(ELLIPSETYPE);
                QRect& lastEllipse = _ellipses.last();
                lastEllipse.setTopLeft((ev->pos()-QPoint(this->width()/2,this->height()/2)-QPoint(XPtInterval,YPtInterval))/ZoomValue);
            } else if (paintingType == LINEDRAW)
            {
                xLine  line_x;//鼠标按下，直线开始
                _measure_lineV.append(line_x);
                shapeType.append(LINEDRAW);
                xLine& lastline = _measure_lineV.last();
                lastline.pt_start = QPoint((ev->pos()- QPoint(this->width()/2,this->height()/2) - QPoint(XPtInterval,YPtInterval))/ZoomValue);
                lastline.pt_img_start = cacul_img_pt( ref2pos( lastline.pt_start));


                _measure_lineV.last().lt_rc =QRect( _measure_lineV.last().pt_start.x()-3, _measure_lineV.last().pt_start.y()-3,6,6);

                _measure_lineV.last().rt_rc =QRect( _measure_lineV.last().pt_end.x()-3, _measure_lineV.last().pt_end.y()-3,6,6);

                xuanzhe_zhixian = _measure_lineV.size() -1;

                  lastline.xuanzhong = 1;


            }else if(paintingType == ROITYPE )
            {
                //QRect rect;//鼠标按下，矩形开始
                xRect rect;
                _roi_rects.append(rect);

                shapeType.append(ROITYPE);
                QRect& lastRect = _roi_rects.last().main_rc;//拿到新矩形
                lastRect.setTopLeft((ev->pos()-QPoint(this->width()/2,this->height()/2)-QPoint(XPtInterval,YPtInterval))/ZoomValue);//记录鼠标的坐标(新矩形的左上角坐标)

                _roi_rects.last().img_space_rc.setTopLeft(cacul_img_pt(ref2pos(_roi_rects.last().main_rc.topLeft())));


                _roi_rects.last().lt_rc =QRect(lastRect.x()-3,lastRect.y()-3,6,6);
                _roi_rects.last().lb_rc =QRect(lastRect.x()-3,lastRect.bottomLeft().y()-3,6,6);
                _roi_rects.last().rt_rc =QRect(lastRect.topRight().x()-3,lastRect.y()-3,6,6);
                _roi_rects.last().rb_rc =QRect(lastRect.topRight().x()-3,lastRect.bottomLeft().y()-3,6,6);

                xuanzhe_rect = _roi_rects.size() -1;

            }
        }

        return;
    }else
    {

        //处理矩形移动
        selectedRect=false;

        if(_roi_rects.size()>0)
        {
            QPoint realTime_pt = QPoint((ev->pos()-QPoint(this->width()/2,this->height()/2)-QPoint(XPtInterval,YPtInterval))/ZoomValue);

            for(int i = 0; i < _roi_rects.size(); i++)
            {

                QRect lastRect = _roi_rects.at(i).main_rc;//拿到新矩形


                //鼠标点击左上角
                if(realTime_pt.x()<=lastRect.topLeft().x()+3&&realTime_pt.x()>= lastRect.topLeft().x()-3&&
                        realTime_pt.y()<=lastRect.topLeft().y()+3&&realTime_pt.y()>=lastRect.topLeft().y()-3)
                {
                    selectedRect=true;
                    press=true;
                    pointPress=QPoint(ev->pos());
                    Select=Select_RoiTopleft;
                    nayigexingzhuang= i;
                    xuanzhe_rect = i;


                }
                //鼠标点击右上角
                else if(realTime_pt.x()<=lastRect.topRight().x()+3&&realTime_pt.x()>= lastRect.topRight().x()-3&&
                        realTime_pt.y()<=lastRect.topRight().y()+3&&realTime_pt.y()>=lastRect.topRight().y()-3)
                {
                    selectedRect=true;
                    press=true;
                    pointPress=QPoint(ev->pos());
                    Select=Select_RoiTopright;
                    nayigexingzhuang= i;
                    xuanzhe_rect = i;


                }
                //鼠标点击左下角
                else if(realTime_pt.x()<=lastRect.bottomLeft().x()+3&&realTime_pt.x()>= lastRect.bottomLeft().x()-3&&
                        realTime_pt.y()<=lastRect.bottomLeft().y()+3&&realTime_pt.y()>=lastRect.bottomLeft().y()-3)
                {
                    selectedRect=true;
                    press=true;
                    pointPress=QPoint(ev->pos());
                    Select=Select_Bottomleft;
                    nayigexingzhuang= i;
                    xuanzhe_rect = i;


                }
                //鼠标点击右下角
                else if(realTime_pt.x()<=lastRect.bottomRight().x()+3&&realTime_pt.x()>= lastRect.bottomRight().x()-3&&
                        realTime_pt.y()<=lastRect.bottomRight().y()+3&&realTime_pt.y()>=lastRect.bottomRight().y()-3)
                {
                    selectedRect=true;
                    press=true;
                    pointPress=QPoint(ev->pos());
                    Select=Select_Bottomright;
                    nayigexingzhuang = i;
                    xuanzhe_rect = i;


                }
                //鼠标点击矩形区域内
                else if(realTime_pt.x()<=lastRect.topRight().x()&&realTime_pt.x()>= lastRect.topLeft().x()&&
                        realTime_pt.y()<=lastRect.bottomLeft().y()&&realTime_pt.y()>=lastRect.topLeft().y())
                {
                    selectedRect=true;
                    press=true;
                    pointPress=QPoint(ev->pos());
                    Select=Select_Roi;
                    nayigexingzhuang = i;

                    xuanzhe_rect = i;



                }
                //                else /*if(realTime_pt.x()<=lastRect.topRight().x()&&realTime_pt.x()>= lastRect.topLeft().x()&&
                //                        realTime_pt.y()<=lastRect.bottomLeft().y()&&realTime_pt.y()>=lastRect.topLeft().y())*/
                //                {
                //                    selectedRect=false;
                //                    press=true;
                //                    pointPress=QPoint(ev->pos());
                //                    Select=Select_image;
                //                   //nayigexingzhuang = -1;
                //                    std::cout<<"---6--------"<<i<<std::endl;
                //                }

            }

        }

        if(_measure_lineV.size() >0)
        {


            QPoint realTime_pt = QPoint((ev->pos()-QPoint(this->width()/2,this->height()/2)-QPoint(XPtInterval,YPtInterval))/ZoomValue);





            for(int i = 0; i < _measure_lineV.size(); i++)
            {

                QRect L_lastRect = _measure_lineV.at(i).lt_rc;//拿到新矩形
                QRect R_lastRect = _measure_lineV.at(i).rt_rc;//拿到新矩形


                double len1 = sqrt((_measure_lineV.at(i).pt_start.x() - realTime_pt.x())*(_measure_lineV.at(i).pt_start.x() - realTime_pt.x()) + (_measure_lineV.at(i).pt_start.y() - realTime_pt.y())*(_measure_lineV.at(i).pt_start.y() - realTime_pt.y()));
                double len2 = sqrt((_measure_lineV.at(i).pt_end.x() - realTime_pt.x())*(_measure_lineV.at(i).pt_end.x() - realTime_pt.x()) + (_measure_lineV.at(i).pt_end.y() - realTime_pt.y())*(_measure_lineV.at(i).pt_end.y() - realTime_pt.y()));

                double lenLine = sqrt((_measure_lineV.at(i).pt_end.x() - _measure_lineV.at(i).pt_start.x())*(_measure_lineV.at(i).pt_end.x() - _measure_lineV.at(i).pt_start.x()) + (_measure_lineV.at(i).pt_end.y() -
                                                                                                                                                                                     _measure_lineV.at(i).pt_start.y())*(_measure_lineV.at(i).pt_end.y() - _measure_lineV.at(i).pt_start.y()));

                bool xf =false;

                //qDebug() << "line->getLength() = " << lenLine << "  len1 + len2 = " << len1 + len2;
                if (lenLine + 0.01 > (len1+len2)&& (len1 + len2)>= lenLine)	{
                    xf = true;
                }else {
                    xf = false;
                }

                //鼠标点击左上角
                if(realTime_pt.x()<=L_lastRect.topLeft().x()+3&&realTime_pt.x()>= L_lastRect.topLeft().x()-3&&
                        realTime_pt.y()<=L_lastRect.topLeft().y()+3&&realTime_pt.y()>=L_lastRect.topLeft().y()-3)
                {
                    selectedRect=true;
                    press=true;
                    pointPress=QPoint(ev->pos());
                    Select=Select_Leftline;
                    nayigexingzhuang= i;

                    xuanzhe_zhixian = i;
                    dianji_zuoyou_dian = 1;


                    _measure_lineV[i].xuanzhong = 1;




                }
                //鼠标点击右上角
                else if(realTime_pt.x()<=R_lastRect.topRight().x()+3&&realTime_pt.x()>= R_lastRect.topRight().x()-3&&
                        realTime_pt.y()<=R_lastRect.topRight().y()+3&&realTime_pt.y()>=R_lastRect.topRight().y()-3)
                {
                    selectedRect=true;
                    press=true;
                    pointPress=QPoint(ev->pos());
                    Select=Select_Rightline;
                    nayigexingzhuang= i;
                    xuanzhe_zhixian = i;

                    dianji_zuoyou_dian = 1;

                       _measure_lineV[i].xuanzhong = 1;



                }else if(xf)
                {

                    selectedRect=true;
                    press=true;
                    pointPress=QPoint(ev->pos());
                    Select=Select_Line;
                    nayigexingzhuang= i;
                    xuanzhe_zhixian = i;
                       _measure_lineV[i].xuanzhong = 1;



                }



            }

        }
        // _measure_lineV.last().rt_rc =QRect( _measure_lineV.last().pt_end.x()-3, _measure_lineV.last().pt_end.y()-3,6,6);
    }

    //记录初始坐标
    OldPos = ev->pos();
    Pressed = true;
}

QPointF videoLabel::cacul_img_pt(const QPointF pt)
{

    if(!Image.isNull())
    {

        // 根据窗口计算应该显示的图片的大小
        int width = qMin(Image.width(), this->width());
        int height = width * 1.0 / (Image.width() * 1.0 / Image.height());
        height = qMin(height, this->height());
        width = height * 1.0 * (Image.width() * 1.0 / Image.height());

        QPoint labeCenterPos(this->width ()/2,this->height ()/2);


        QPointF leftTop = QPointF(labeCenterPos.x ()-width /2 *ZoomValue ,
                                  labeCenterPos.y ()-height /2 * ZoomValue)  +QPointF(XPtInterval, YPtInterval) ;

        //        QPointF rightBotom = QPointF(labeCenterPos.x () + width /2 *ZoomValue,
        //                                     labeCenterPos.y ()+ height /2 * ZoomValue ) +QPointF(XPtInterval, YPtInterval);


        float x_s = ((float)Image.width()/ (float)width ) / ZoomValue ;
        float  y_s = ((float)Image.height()/ (float)height) /ZoomValue;

        QPointF temg = (pt- leftTop);
        QPointF img_ptx =  QPointF( temg.x() *  x_s, temg.y() *  y_s );

        return img_ptx;
    }
    return pt;

}

QPointF videoLabel::cacul_pos_pt(const QPointF pt)
{

    if(!Image.isNull())
    {


        // 根据窗口计算应该显示的图片的大小
        int width = qMin(Image.width(), this->width());
        int height = width * 1.0 / (Image.width() * 1.0 / Image.height());
        height = qMin(height, this->height());
        width = height * 1.0 * (Image.width() * 1.0 / Image.height());


        QPoint labeCenterPos(this->width ()/2,this->height ()/2);


        QPointF leftTop = QPointF(labeCenterPos.x ()-width /2 *ZoomValue ,
                                  labeCenterPos.y ()-height /2 * ZoomValue)  +QPointF(XPtInterval, YPtInterval) ;


        float x_s = ((float)Image.width()/ (float)width ) / ZoomValue ;
        float  y_s = ((float)Image.height()/ (float)height) /ZoomValue;

        QPointF  temg =  QPointF( pt.x()/x_s, pt.y()/y_s );

        QPointF pos_pt = (temg + leftTop);

        return pos_pt;

    }

    return pt;
}


QPointF videoLabel::cacul_img_ref_pt(const QPointF pt)
{

    if(!Image.isNull())
    {


        // 根据窗口计算应该显示的图片的大小
        int width = qMin(Image.width(), this->width());
        int height = width * 1.0 / (Image.width() * 1.0 / Image.height());
        height = qMin(height, this->height());
        width = height * 1.0 * (Image.width() * 1.0 / Image.height());


        QPoint labeCenterPos(this->width ()/2,this->height ()/2);


        QPointF leftTop = QPointF(labeCenterPos.x ()-width /2 *ZoomValue ,
                                  labeCenterPos.y ()-height /2 * ZoomValue)  +QPointF(XPtInterval, YPtInterval) ;


        float x_s = ((float)Image.width()/ (float)width ) / ZoomValue ;
        float  y_s = ((float)Image.height()/ (float)height) /ZoomValue;

        QPointF  temg =  QPointF( pt.x()/x_s, pt.y()/y_s );

        QPointF pos_pt = (temg + leftTop);



        QPointF ref_pt = (pos_pt-QPoint(this->width()/2,this->height()/2)-QPoint(XPtInterval,YPtInterval))/ZoomValue;

        return ref_pt;

    }

    return pt;


}


void videoLabel::cacul_axies()
{

    if(!Image.isNull())
    {

        // 根据窗口计算应该显示的图片的大小
        int width = qMin(Image.width(), this->width());
        int height = width * 1.0 / (Image.width() * 1.0 / Image.height());
        height = qMin(height, this->height());
        width = height * 1.0 * (Image.width() * 1.0 / Image.height());

        m_Img_rt = QRect(0,0,Image.width(),Image.height());


        m_jl_bh.huabu_r = QRectF(0,0,width,height);
        m_jl_bh.img_real_r = QRectF(0,0,Image.width(),Image.height());
        m_jl_bh.hb_pt = _pos;

        QPoint labeCenterPos(this->width ()/2,this->height ()/2);


        m_jl_bh.leftTop = QPointF(labeCenterPos.x ()-width /2 *ZoomValue ,
                                  labeCenterPos.y ()-height /2 * ZoomValue)  +QPointF(XPtInterval, YPtInterval) ;

        m_jl_bh.rightBotom = QPointF(labeCenterPos.x () + width /2 *ZoomValue,
                                     labeCenterPos.y ()+ height /2 * ZoomValue ) +QPointF(XPtInterval, YPtInterval);



        m_jl_bh.fangda_beishu =ZoomValue;
        m_jl_bh.x_s = ((float)Image.width()/ (float)width ) / ZoomValue ;
        m_jl_bh.y_s = ((float)Image.height()/ (float)height) /ZoomValue;

        QPointF temg = (_pos- m_jl_bh.leftTop);
        m_jl_bh.img_pt =  QPointF( temg.x() *  m_jl_bh.x_s, temg.y() *  m_jl_bh.y_s );
        _imgPos = m_jl_bh.img_pt ;
    }
    m_jl_bh.hb_pt = _pos;


    emit sigSendMouseMovePos(m_jl_bh);//改了ImgPos

}

///根据XY方向移动距离修改矩形区域左上角右下角的位置
void videoLabel::movePoint(QRect *rect, int moveX, int moveY)
{
    rect->setTopLeft(QPoint(rect->x()+moveX,rect->y()+moveY));
    rect->setBottomRight(QPoint(rect->bottomRight().x()+moveX,rect->bottomRight().y()+moveY));
}
void videoLabel::mouseMoveEvent(QMouseEvent *event)
{

    _pos = event->pos ();

    cacul_axies();



    if (!Pressed)
        return QWidget::mouseMoveEvent(event);

    if(isPainting&&Pressed&&(event->buttons()==Qt::LeftButton) )
    {
        if(paintingType == RECTTYPE)
        {
            QRect& lastRect = _rects.last();//拿到新矩形

            lastRect.setBottomRight((event->pos()-QPoint(this->width()/2,this->height()/2)-QPoint(XPtInterval,YPtInterval))/ZoomValue);//更新矩形的右下角坐标

        }
        else if (paintingType == ELLIPSETYPE)
        {
            QRect& lastEllipse = _ellipses.last();
            lastEllipse.setBottomRight((event->pos()-QPoint(this->width()/2,this->height()/2)-QPoint(XPtInterval,YPtInterval))/ZoomValue);
        }else if (paintingType == LINEDRAW)
        {

            //记录远动过程中结束点
            xLine& lastline = _measure_lineV.last();
            lastline.pt_end = QPoint((event->pos()-QPoint(this->width()/2,this->height()/2)-QPoint(XPtInterval,YPtInterval))/ZoomValue);

            _measure_lineV.last().rt_rc =QRect( _measure_lineV.last().pt_end.x()-3, _measure_lineV.last().pt_end.y()-3,6,6);



            lastline.pt_img_end = cacul_img_pt( ref2pos( lastline.pt_end));


           lastline.xuanzhong = 1;

        }else if(paintingType == ROITYPE)
        {
            QRect& lastRect = _roi_rects.last().main_rc;//拿到新矩形

            lastRect.setBottomRight((event->pos()-QPoint(this->width()/2,this->height()/2)-QPoint(XPtInterval,YPtInterval))/ZoomValue);//更新矩形的右下角坐标

            _roi_rects.last().img_space_rc.setBottomRight(cacul_img_pt(ref2pos(_roi_rects.last().main_rc.bottomRight())));
            //

            _roi_rects.last().lt_rc =QRect(lastRect.x()-3,lastRect.y()-3,6,6);
            _roi_rects.last().lb_rc =QRect(lastRect.x()-3,lastRect.bottomLeft().y()-3,6,6);
            _roi_rects.last().rt_rc =QRect(lastRect.topRight().x()-3,lastRect.y()-3,6,6);
            _roi_rects.last().rb_rc =QRect(lastRect.topRight().x()-3,lastRect.bottomLeft().y()-3,6,6);


        }
        update();
        return;
    }else if( Pressed&&(event->buttons()==Qt::LeftButton) )
    {



        QPoint pos_r = QPoint((event->pos()-QPoint(this->width()/2,this->height()/2)-QPoint(XPtInterval,YPtInterval))/ZoomValue);
        QPoint pos_p = QPoint((pointPress-QPoint(this->width()/2,this->height()/2)-QPoint(XPtInterval,YPtInterval))/ZoomValue);

        int  width1 =  (pos_r.x() - pos_p.x());
        int height1 =( pos_r.y() - pos_p.y()) ;

        if(press)
        {
            switch (Select)
            {
            case Select_image:
            {


                if (LocalFileName.isNull())
                    return ;
                this->setCursor(Qt::SizeAllCursor);
                QPoint pos = event->pos();
                int xPtInterval = pos.x() - OldPos.x();
                int yPtInterval = pos.y() - OldPos.y();

                XPtInterval += xPtInterval;
                YPtInterval += yPtInterval;

                OldPos = pos;
                pointPress=QPoint(event->pos());
                update();
                break;
            }
            case Select_Roi:
            {
                //判断矩形框移动后会不会超出图像范围

                if(_roi_rects.size()>0)
                {
                    QPoint realTime_pt = QPoint((event->pos()-QPoint(this->width()/2,this->height()/2)-QPoint(XPtInterval,YPtInterval))/ZoomValue);

                    for(int i = 0; i < _roi_rects.size(); i++)
                    {

                        if( (nayigexingzhuang == i )&& selectedRect==true)
                        {
                            QRect lastRect = _roi_rects.at(i).main_rc;//拿到新矩形



                            //   bool b=m_Img_rt.intersects(lastRect);  //两个QRect是否相交
                            //  if(b)
                            {
                                movePoint(&_roi_rects[i].main_rc, width1,height1);//矩形跟随鼠标移动


                                movePoint(&_roi_rects[i].lt_rc, width1,height1);//矩形跟随鼠标移动
                                movePoint(&_roi_rects[i].lb_rc, width1,height1);//矩形跟随鼠标移动
                                movePoint(&_roi_rects[i].rt_rc, width1,height1);//矩形跟随鼠标移动
                                movePoint(&_roi_rects[i].rb_rc, width1,height1);//矩形跟随鼠标移动


                                _roi_rects[i].img_space_rc =  QRectF(cacul_img_pt( ref2pos(_roi_rects[i].main_rc.topLeft())) ,cacul_img_pt(ref2pos(_roi_rects[i].main_rc.bottomRight())));

                                pointPress=QPoint(event->pos());
                                update();
                            }

                        }

                    }
                }

                pointPress=QPoint(event->pos());
                update();

                break;
            }
            case Select_RoiTopleft:
            {

                if(_roi_rects.size()>0)
                {
                    QPoint realTime_pt = QPoint((event->pos()-QPoint(this->width()/2,this->height()/2)-QPoint(XPtInterval,YPtInterval))/ZoomValue);

                    for(int i = 0; i < _roi_rects.size(); i++)
                    {

                        if(nayigexingzhuang == i)
                        {
                            QRect lastRect = _roi_rects.at(i).main_rc;//拿到新矩形

                            QPointF imgpt = cacul_img_pt(event->pos());
                            // movePoint(&_roi_rects[i].main_rc, width1,height1);//矩形跟随鼠标移动
                            _roi_rects[i].main_rc.setTopLeft(QPoint(_roi_rects[i].main_rc.topLeft().x()+width1,_roi_rects[i].main_rc.topLeft().y()+height1));
                            _roi_rects[i].lt_rc =QRect(_roi_rects[i].main_rc.x()-3,_roi_rects[i].main_rc.y()-3,6,6);
                            _roi_rects[i].lb_rc =QRect(_roi_rects[i].main_rc.x()-3,_roi_rects[i].main_rc.bottomLeft().y()-3,6,6);
                            _roi_rects[i].rt_rc =QRect(_roi_rects[i].main_rc.topRight().x()-3,_roi_rects[i].main_rc.y()-3,6,6);
                            _roi_rects[i].rb_rc =QRect(_roi_rects[i].main_rc.topRight().x()-3,_roi_rects[i].main_rc.bottomLeft().y()-3,6,6);

                            _roi_rects[i].img_space_rc =  QRectF(cacul_img_pt( ref2pos(_roi_rects[i].main_rc.topLeft())) ,cacul_img_pt(ref2pos(_roi_rects[i].main_rc.bottomRight())));


                            pointPress=QPoint(event->pos());
                            update();
                        }

                    }
                }



                pointPress=QPoint(event->pos());
                update();
                break;
            }
            case Select_RoiTopright:
            {

                if(_roi_rects.size()>0)
                {
                    QPoint realTime_pt = QPoint((event->pos()-QPoint(this->width()/2,this->height()/2)-QPoint(XPtInterval,YPtInterval))/ZoomValue);

                    for(int i = 0; i < _roi_rects.size(); i++)
                    {

                        if(nayigexingzhuang == i)
                        {
                            QRect lastRect = _roi_rects.at(i).main_rc;//拿到新矩形

                            QPointF imgpt = cacul_img_pt(event->pos());
                            // movePoint(&_roi_rects[i].main_rc, width1,height1);//矩形跟随鼠标移动
                            _roi_rects[i].main_rc.setTopRight(QPoint(_roi_rects[i].main_rc.topRight().x()+width1,_roi_rects[i].main_rc.topRight().y()+height1));
                            _roi_rects[i].lt_rc =QRect(_roi_rects[i].main_rc.x()-3,_roi_rects[i].main_rc.y()-3,6,6);
                            _roi_rects[i].lb_rc =QRect(_roi_rects[i].main_rc.x()-3,_roi_rects[i].main_rc.bottomLeft().y()-3,6,6);
                            _roi_rects[i].rt_rc =QRect(_roi_rects[i].main_rc.topRight().x()-3,_roi_rects[i].main_rc.y()-3,6,6);
                            _roi_rects[i].rb_rc =QRect(_roi_rects[i].main_rc.topRight().x()-3,_roi_rects[i].main_rc.bottomLeft().y()-3,6,6);


                            _roi_rects[i].img_space_rc =  QRectF(cacul_img_pt( ref2pos(_roi_rects[i].main_rc.topLeft())) ,cacul_img_pt(ref2pos(_roi_rects[i].main_rc.bottomRight())));


                            pointPress=QPoint(event->pos());
                            update();
                        }

                    }
                }


                pointPress=QPoint(event->pos());
                update();
                break;
            }
            case Select_Bottomleft:
            {

                if(_roi_rects.size()>0)
                {
                    QPoint realTime_pt = QPoint((event->pos()-QPoint(this->width()/2,this->height()/2)-QPoint(XPtInterval,YPtInterval))/ZoomValue);

                    for(int i = 0; i < _roi_rects.size(); i++)
                    {

                        if(nayigexingzhuang == i)
                        {
                            QRect lastRect = _roi_rects.at(i).main_rc;//拿到新矩形


                            // movePoint(&_roi_rects[i].main_rc, width1,height1);//矩形跟随鼠标移动
                            _roi_rects[i].main_rc.setBottomLeft(QPoint(_roi_rects[i].main_rc.bottomLeft().x()+width1,_roi_rects[i].main_rc.bottomLeft().y()+height1));
                            _roi_rects[i].lt_rc =QRect(_roi_rects[i].main_rc.x()-3,_roi_rects[i].main_rc.y()-3,6,6);
                            _roi_rects[i].lb_rc =QRect(_roi_rects[i].main_rc.x()-3,_roi_rects[i].main_rc.bottomLeft().y()-3,6,6);
                            _roi_rects[i].rt_rc =QRect(_roi_rects[i].main_rc.topRight().x()-3,_roi_rects[i].main_rc.y()-3,6,6);
                            _roi_rects[i].rb_rc =QRect(_roi_rects[i].main_rc.topRight().x()-3,_roi_rects[i].main_rc.bottomLeft().y()-3,6,6);

                            _roi_rects[i].img_space_rc =  QRectF(cacul_img_pt( ref2pos(_roi_rects[i].main_rc.topLeft())) ,cacul_img_pt(ref2pos(_roi_rects[i].main_rc.bottomRight())));


                            pointPress=QPoint(event->pos());
                            update();
                        }

                    }
                }


                pointPress=QPoint(event->pos());
                update();

                break;
            }
            case Select_Bottomright:
            {


                if(_roi_rects.size()>0)
                {
                    QPoint realTime_pt = QPoint((event->pos()-QPoint(this->width()/2,this->height()/2)-QPoint(XPtInterval,YPtInterval))/ZoomValue);

                    for(int i = 0; i < _roi_rects.size(); i++)
                    {

                        if(nayigexingzhuang == i)
                        {
                            QRect lastRect = _roi_rects.at(i).main_rc;//拿到新矩形


                            // movePoint(&_roi_rects[i].main_rc, width1,height1);//矩形跟随鼠标移动
                            _roi_rects[i].main_rc.setBottomRight(QPoint(_roi_rects[i].main_rc.bottomRight().x()+width1,_roi_rects[i].main_rc.bottomRight().y()+height1));
                            _roi_rects[i].lt_rc =QRect(_roi_rects[i].main_rc.x()-3,_roi_rects[i].main_rc.y()-3,6,6);
                            _roi_rects[i].lb_rc =QRect(_roi_rects[i].main_rc.x()-3,_roi_rects[i].main_rc.bottomLeft().y()-3,6,6);
                            _roi_rects[i].rt_rc =QRect(_roi_rects[i].main_rc.topRight().x()-3,_roi_rects[i].main_rc.y()-3,6,6);
                            _roi_rects[i].rb_rc =QRect(_roi_rects[i].main_rc.topRight().x()-3,_roi_rects[i].main_rc.bottomLeft().y()-3,6,6);

                            _roi_rects[i].img_space_rc =  QRectF(cacul_img_pt( ref2pos(_roi_rects[i].main_rc.topLeft())) ,cacul_img_pt(ref2pos(_roi_rects[i].main_rc.bottomRight())));


                            pointPress=QPoint(event->pos());
                            update();
                        }

                    }
                }


                pointPress=QPoint(event->pos());
                update();
                break;
            }
            case Select_Leftline:
            {


                if(_measure_lineV.size()>0)
                {
                    QPoint realTime_pt = QPoint((event->pos()-QPoint(this->width()/2,this->height()/2)-QPoint(XPtInterval,YPtInterval))/ZoomValue);

                    for(int i = 0; i < _measure_lineV.size(); i++)
                    {


                        if(nayigexingzhuang == i)
                        {



                            _measure_lineV[i].pt_start= realTime_pt;

                            _measure_lineV[i].pt_img_start =   cacul_img_pt(event->pos());

                            _measure_lineV[i].lt_rc =QRect(   _measure_lineV[i].pt_start.x()-3,  _measure_lineV[i].pt_start.y()-3,6,6);

                            _measure_lineV[i].rt_rc =QRect(   _measure_lineV[i].pt_end.x()-3,   _measure_lineV[i].pt_end.y()-3,6,6);

                            _measure_lineV[i].xuanzhong = 1;

                            pointPress=QPoint(event->pos());
                            update();
                        }

                    }
                }


                pointPress=QPoint(event->pos());
                update();
                break;
            }

            case Select_Rightline:
            {


                if(_measure_lineV.size()>0)
                {

                    QPoint realTime_pt = QPoint((event->pos()-QPoint(this->width()/2,this->height()/2)-QPoint(XPtInterval,YPtInterval))/ZoomValue);

                    for(int i = 0; i < _measure_lineV.size(); i++)
                    {

                        if(nayigexingzhuang == i)
                        {



                            _measure_lineV[i].pt_end= realTime_pt;

                            _measure_lineV[i].pt_img_end =   cacul_img_pt(event->pos());

                            _measure_lineV[i].lt_rc =QRect(   _measure_lineV[i].pt_start.x()-3,  _measure_lineV[i].pt_start.y()-3,6,6);

                            _measure_lineV[i].rt_rc =QRect(   _measure_lineV[i].pt_end.x()-3,   _measure_lineV[i].pt_end.y()-3,6,6);

                            pointPress=QPoint(event->pos());
                             _measure_lineV[i].xuanzhong = 1;
                            update();
                        }

                    }
                }


                pointPress=QPoint(event->pos());
                update();
                break;
            }
            case Select_Line:
            {


                if(_measure_lineV.size()>0)
                {

                    if(dianji_zuoyou_dian !=1)
                    {
                        QPoint realTime_pt = QPoint((event->pos()-QPoint(this->width()/2,this->height()/2)-QPoint(XPtInterval,YPtInterval))/ZoomValue);

                        for(int i = 0; i < _measure_lineV.size(); i++)
                        {

                            if(nayigexingzhuang == i)
                            {

                                _measure_lineV[i].pt_end= QPoint( _measure_lineV[i].pt_end.x()+width1, _measure_lineV[i].pt_end.y()+height1);
                                _measure_lineV[i].pt_start= QPoint( _measure_lineV[i].pt_start.x()+width1, _measure_lineV[i].pt_start.y()+height1);


                                //QPointF pos_r_yuandian  =cacul_img_pt(event->pos());
                                //QPointF pos_p_press =cacul_img_pt(pointPress);//

                                // int  width1 =  (pos_r_yuandian.x() - pos_p_press.x());
                                // int height1 =( pos_r_yuandian.y() - pos_p_press.y()) ;

                                _measure_lineV[i].pt_img_end = cacul_img_pt(ref2pos(_measure_lineV[i].pt_end));
                                _measure_lineV[i].pt_img_start = cacul_img_pt(ref2pos(_measure_lineV[i].pt_start));;

                                _measure_lineV[i].lt_rc =QRect(   _measure_lineV[i].pt_start.x()-3,  _measure_lineV[i].pt_start.y()-3,6,6);

                                _measure_lineV[i].rt_rc =QRect(   _measure_lineV[i].pt_end.x()-3,   _measure_lineV[i].pt_end.y()-3,6,6);

                                pointPress=QPoint(event->pos());

                                 _measure_lineV[i].xuanzhong = 1;
                                update();
                            }

                        }
                    }

                }


                pointPress=QPoint(event->pos());
                update();
                break;
            }


            default:
            {

                if (LocalFileName.isNull())
                    return ;
                this->setCursor(Qt::SizeAllCursor);
                QPoint pos = event->pos();
                int xPtInterval = pos.x() - OldPos.x();
                int yPtInterval = pos.y() - OldPos.y();

                XPtInterval += xPtInterval;
                YPtInterval += yPtInterval;

                OldPos = pos;


                update();

                break;
            }

            }

            return;
        }

    }

    if (LocalFileName.isNull())
        return ;
    this->setCursor(Qt::SizeAllCursor);
    QPoint pos = event->pos();
    int xPtInterval = pos.x() - OldPos.x();
    int yPtInterval = pos.y() - OldPos.y();

    XPtInterval += xPtInterval;
    YPtInterval += yPtInterval;

    OldPos = pos;



    update();

}

void videoLabel::set_isPainting(bool in_x)
{

    if(_measure_lineV.size() >0)
    {

        for(int i = 0; i < _measure_lineV.size(); i++)
        {
            _measure_lineV[i].xuanzhong = 0;
        }

    }

    isPainting = in_x;
}
void videoLabel::mouseReleaseEvent(QMouseEvent *event)
{


    if(_measure_lineV.size() >0)
    {

        for(int i = 0; i < _measure_lineV.size(); i++)
        {
            _measure_lineV[i].xuanzhong = 0;
        }

    }

    if(isPainting&&Pressed)
    {
        if(paintingType == RECTTYPE)
        {
            QRect& lastRect = _rects.last();//拿到新矩形
            lastRect.setBottomRight((event->pos()-QPoint(this->width()/2,this->height()/2)-QPoint(XPtInterval,YPtInterval))/ZoomValue);//更新矩形的右下角坐标
        }
        else if (paintingType == ELLIPSETYPE)
        {
            QRect& lastEllipse = _ellipses.last();
            lastEllipse.setBottomRight((event->pos()-QPoint(this->width()/2,this->height()/2)-QPoint(XPtInterval,YPtInterval))/ZoomValue);
        }else if (paintingType == LINEDRAW)
        {
            //记录结束点
            xLine& lastline = _measure_lineV.last();
            lastline.pt_end = QPoint((event->pos()-QPoint(this->width()/2,this->height()/2)-QPoint(XPtInterval,YPtInterval))/ZoomValue);
            //lastline.pt_img_end =   cacul_img_pt(event->pos());

            lastline.pt_img_end = cacul_img_pt( ref2pos( lastline.pt_end));


            _measure_lineV.last().rt_rc =QRect( _measure_lineV.last().pt_end.x()-3, _measure_lineV.last().pt_end.y()-3,6,6);
            lastline.xuanzhong = 0;

        }else if(paintingType == ROITYPE)
        {
            QRect& lastRect = _roi_rects.last().main_rc;//拿到新矩形
            lastRect.setBottomRight((event->pos()-QPoint(this->width()/2,this->height()/2)-QPoint(XPtInterval,YPtInterval))/ZoomValue);//更新矩形的右下角坐标

            _roi_rects.last().img_space_rc.setBottomRight(cacul_img_pt( event->pos()));

            _roi_rects.last().lt_rc =QRect(lastRect.x()-3,lastRect.y()-3,6,6);
            _roi_rects.last().lb_rc =QRect(lastRect.x()-3,lastRect.bottomLeft().y()-3,6,6);
            _roi_rects.last().rt_rc =QRect(lastRect.topRight().x()-3,lastRect.y()-3,6,6);
            _roi_rects.last().rb_rc =QRect(lastRect.topRight().x()-3,lastRect.bottomLeft().y()-3,6,6);

        }


        changePaintingState();//isPainting 改变 状态
        Pressed = false;
        press=false;
        dianji_zuoyou_dian = 0;

        nayigexingzhuang  = -1;
        this->setCursor(Qt::ArrowCursor);
        return;
    }


    nayigexingzhuang  = -1;
    dianji_zuoyou_dian = 0;
    Pressed = false;
    press=false;
    setCursor(Qt::ArrowCursor);
}

void videoLabel::OnSelectImage()
{
    LocalFileName = QFileDialog::getOpenFileName(this, "Open Image", "./", tr("Image Files(*.jpg *.png *.bmp *.pgm *.pbm *.xpm);;All(*.*)"));// "Image Files(*.jpg *.png *.bmp *.pgm *.pbm *.xpm);;All(*.*)");

    QFile file(LocalFileName);
    if (!file.exists())
        return;


    QFileInfo fileinfo;
   // file_full = QFileDialog::getOpenFileName(this,.....);
    fileinfo = QFileInfo(LocalFileName);
    //文件名
    file_name = fileinfo.fileName();
//    //文件后缀
//    file_suffix = fileinfo.suffix()
//    //绝对路径
     file_path = LocalFileName;



    ZoomValue = 1.0;
    XPtInterval = 0;
    YPtInterval = 0;
    Image.load(LocalFileName);

    update();
}

void videoLabel::OnZoomInImage()
{
    ZoomValue += 0.15;
    cacul_axies();
    update();


}

void videoLabel::OnZoomOutImage()
{
    ZoomValue -= 0.15;
    if (ZoomValue <= 0)
    {
        ZoomValue = 0.15;
        return;
    }

    cacul_axies();
    update();


}

void videoLabel::OnPresetImage()
{
    ZoomValue = 1.0;
    XPtInterval = 0;
    YPtInterval = 0;
    //    brightness = 0;
    //    contrast = 1;
    //    ChangeBriAndConChangedState();

    cacul_axies();
    update();

}

void videoLabel::OnPaintingback()
{
    if(_rectsIsEmpty()&&_ellipsesIsEmpty() &&_lineIsEmpty() &&_ROIRCIsEmpty())
        return;

    if(shapeType.last()==RECTTYPE)
    {
        _rects.pop_back();
        shapeType.pop_back();
        update();
    }
    else if(shapeType.last()==ELLIPSETYPE)
    {
        _ellipses.pop_back();
        shapeType.pop_back();

        update();
    }
    else if(shapeType.last()==LINEDRAW)
    {
        _measure_lineV.pop_back();
        shapeType.pop_back();
        update();
    }
    else if(shapeType.last()==ROITYPE)
    {
        _roi_rects.pop_back();
        shapeType.pop_back();
        update();


    }
}
