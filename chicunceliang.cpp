#pragma execution_character_set("utf-8")

#include "chicunceliang.h"
#include "ui_chicunceliang.h"

#include <QLabel>
#include <QFileDialog>
#include <QMenu>
#include <QPainter>
#include <QWheelEvent>
#include <QDebug>
#include <QTime>
#include<QSettings>
#include <QDateTime>
#include <QString>
#include <QTime>
#include <iostream>
#include <thread>
#include <QSettings>
#include <QCoreApplication>
#include <QDir>
#include<direct.h>

#include <QMessageBox>

chicunceliang::chicunceliang(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::chicunceliang)
{
    ui->setupUi(this);



    QString exe_path = QCoreApplication::applicationDirPath();

    QDir dir_list_fold(exe_path+"/user/cl");

    QVector<QString> folder;

    dir_list_fold.setFilter(QDir::Dirs);

    ui->comboBox_peizhi->clear();


    foreach(QFileInfo fullDir, dir_list_fold.entryInfoList())
    {

        if(fullDir.fileName() == "." || fullDir.fileName() == "..") continue;

        //folder.push_back(fullDir.absoluteFilePath());

        QString cfg_path = fullDir.absoluteFilePath();
        QString name = fullDir.fileName();
        QString ini_file =cfg_path +"/" + name + ".ini" ;




        measure_bc tempX;



        QSettings* configIni = new QSettings(ini_file, QSettings::IniFormat);

        tempX.ID =  configIni->value("project/ID").toString();


        tempX.src_name =  configIni->value("project/src_name").toString();
        tempX.moren =  configIni->value("project/moren").toInt();
        tempX.moban_name =  configIni->value("project/moban_name").toString();
        tempX.moban_in_src_rc = QRectF( configIni->value("project/moban_x").toDouble() ,configIni->value("project/moban_y").toDouble(),
                                        configIni->value("project/moban_w").toDouble() ,configIni->value("project/moban_h").toDouble());

        tempX.m_dMaxOverlap =  configIni->value("project/m_dMaxOverlap").toDouble();
        tempX.m_dScore =  configIni->value("project/m_dScore").toDouble();
        tempX.m_dToleranceAngle =  configIni->value("project/m_dToleranceAngle").toDouble();
        tempX.m_iMinReduceArea =  configIni->value("project/m_iMinReduceArea").toInt();
        tempX.m_iMaxPos =  configIni->value("project/m_iMaxPos").toInt();
        tempX.m_bSubPixelEstimation =  configIni->value("project/m_bSubPixelEstimation").toInt();

        QString img_src =cfg_path +"/" +  tempX.ID + "_src.jpg" ;
        tempX.m_matSrc = cv::imread(img_src.toStdString(),cv::IMREAD_GRAYSCALE).clone();


        QString img_dst =cfg_path +"/" +  tempX.ID + "_tpl.jpg" ;
        tempX.m_matDst = cv::imread(img_dst.toStdString(),cv::IMREAD_GRAYSCALE).clone();

        int line_num  =  configIni->value("project/line_num").toInt();

        tempX.line_num = line_num;

        ui->comboBox_peizhi->addItems(QStringList()<< tempX.ID);

        for(int i =0; i < line_num; i++)
        {

            Mesure_Unit_Set  dan_obj;

            QString line_str = "line_"+QString::number(i) + "/";
            dan_obj.ID =  configIni->value(line_str + "ID").toString();
            dan_obj.duosao_bian =  configIni->value(line_str + "duosao_bian").toInt();
            dan_obj.kuan_yz =  configIni->value(line_str + "kuan_yz").toDouble();
            dan_obj.gao_yz =  configIni->value(line_str + "gao_yz").toDouble();
            dan_obj.min_v =  configIni->value(line_str + "min_v").toDouble();
            dan_obj.max_v =  configIni->value(line_str + "max_v").toDouble();
            dan_obj.buchangzhi =  configIni->value(line_str + "buchangzhi").toDouble();

            dan_obj.m_unit[0].cfg.pt1.x = configIni->value(line_str + "pt1_x").toInt();
            dan_obj.m_unit[0].cfg.pt1.y = configIni->value(line_str + "pt1_y").toInt();

            dan_obj.m_unit[0].cfg.pt2.x = configIni->value(line_str + "pt2_x").toInt();
            dan_obj.m_unit[0].cfg.pt2.y = configIni->value(line_str + "pt2_y").toInt();


            dan_obj.m_unit[0].cfg.m_kachi_cd = configIni->value(line_str + "m_kachi_cd").toInt();
            dan_obj.m_unit[0].cfg.m_kachi_kd = configIni->value(line_str + "m_kachi_kd").toInt();
            dan_obj.m_unit[0].cfg.m_kachi_sl = configIni->value(line_str + "m_kachi_sl").toInt();
            dan_obj.m_unit[0].cfg.m_kachi_lvbochic = configIni->value(line_str + "m_kachi_lvbochic").toInt();
            dan_obj.m_unit[0].cfg.m_kachi_ssfx = configIni->value(line_str + "m_kachi_ssfx").toInt();

            dan_obj.m_unit[0].cfg.m_kachi_yuzhi = configIni->value(line_str + "m_kachi_yuzhi").toInt();
            dan_obj.m_unit[0].cfg.m_kachi_moshi = configIni->value(line_str + "m_kachi_moshi").toString();



            dan_obj.m_unit[1].cfg.pt1.x = configIni->value(line_str + "pt1_x").toInt();
            dan_obj.m_unit[1].cfg.pt1.y = configIni->value(line_str + "pt1_y").toInt();

            dan_obj.m_unit[1].cfg.pt2.x = configIni->value(line_str + "pt2_x").toInt();
            dan_obj.m_unit[1].cfg.pt2.y = configIni->value(line_str + "pt2_y").toInt();


            dan_obj.m_unit[1].cfg.m_kachi_cd = configIni->value(line_str + "m_kachi_cd").toInt();
            dan_obj.m_unit[1].cfg.m_kachi_kd = configIni->value(line_str + "m_kachi_kd").toInt();
            dan_obj.m_unit[1].cfg.m_kachi_sl = configIni->value(line_str + "m_kachi_sl").toInt();
            dan_obj.m_unit[1].cfg.m_kachi_lvbochic = configIni->value(line_str + "m_kachi_lvbochic").toInt();
            dan_obj.m_unit[1].cfg.m_kachi_ssfx = - (dan_obj.m_unit[0].cfg.m_kachi_ssfx);/* configIni->value(line_str + "m_kachi_ssfx").toInt()*/;

            dan_obj.m_unit[1].cfg.m_kachi_yuzhi = configIni->value(line_str + "m_kachi_yuzhi").toInt();
            dan_obj.m_unit[1].cfg.m_kachi_moshi = configIni->value(line_str + "m_kachi_moshi").toString();

            dan_obj.Template_img_space_rc =  tempX.moban_in_src_rc;
            dan_obj.shifou_chushihua = 1;
            dan_obj.if_have_template_flag = 1;


            tempX.m_mesure_set[ dan_obj.ID ] = dan_obj;

        }

        delete configIni;

        QString xy = tempX.ID;
        map_kachi_Xdiaoyong[xy]= &tempX;

        if(tempX.moren == 1)
        {

            ui->listWidget_Kachi->clear();
            copyPtr2Obj(m_kachi_diaoyong, map_kachi_Xdiaoyong[xy]);

            Temlplate_img = m_kachi_diaoyong.m_matDst;

            QImage xxx = ui->label_yj->Mat2QImage(Temlplate_img);
            ui->label_show_template->setPixmap(QPixmap::fromImage(xxx.scaled(ui->label_show_template->size())));

            if_have_template = 1;


            QMap<QString ,Mesure_Unit_Set>::const_iterator it_m = m_kachi_diaoyong.m_mesure_set.constBegin();// 有几个测量对象
            while (it_m != m_kachi_diaoyong.m_mesure_set.constEnd())
            {


                Mesure_Unit_Set  dan_obj = it_m.value();

                QString ID = dan_obj.ID;

                QString type =ID +(":单线");
                ui->listWidget_Kachi->addItem (type );

                int row = ui->listWidget_Kachi-> count ()-1;//获取当前鼠标所选行
                QListWidgetItem * curent =  ui->listWidget_Kachi->item(row);



                m_kachi_diaoyong.m_mesure_set_item_str[curent] = ID;


                ++it_m;
            }

        }

    }

    //设置只读
    if (ui->plainTextEdit->isReadOnly())
    {
        ui->plainTextEdit->setReadOnly(false);
    }

    this->setMouseTracking(true);


    _labScreen = new QLabel(this);
    _labScreen->setText ("Scre()");

    ui->statusBar->addWidget (_labScreen);


    zhuangtai_flag[ "鼠标状态"] = 1;


    connect(ui->label_yj,SIGNAL(sigSendMouseMovePos(const Beishu&)),this,SLOT(slotMouseMove(const Beishu&)));


}



void chicunceliang::copyPtr2Obj(measure_bc &tempX, measure_bc * ptr)
{
    tempX.ID = ptr->ID;
    tempX.src_name =  ptr-> src_name;
    tempX.moren = ptr->moren  ;
    tempX.moban_name =ptr->  moban_name;
    tempX.moban_in_src_rc = ptr-> moban_in_src_rc;

    tempX.m_dMaxOverlap = ptr-> m_dMaxOverlap;
    tempX.m_dScore =  ptr->m_dScore;
    tempX.m_dToleranceAngle =  ptr->m_dToleranceAngle;
    tempX.m_iMinReduceArea =  ptr->m_iMinReduceArea;
    tempX.m_iMaxPos =  ptr->m_iMaxPos ;
    tempX.m_bSubPixelEstimation =  ptr->m_bSubPixelEstimation;
    tempX.m_matSrc = ptr-> m_matSrc;
    tempX.m_matDst =ptr-> m_matDst;
    tempX.line_num  = ptr-> line_num;
    QMap< QString,   Mesure_Unit_Set >::const_iterator it = ptr->m_mesure_set.constBegin();
    while (it !=  ptr->m_mesure_set.constEnd())
    {
        Mesure_Unit_Set x = it.value();

        tempX.m_mesure_set[x.ID] = x;

        ++it;
    }


}


chicunceliang::~chicunceliang()
{
    delete ui;
}




void chicunceliang::on_act_test_img_triggered()
{


    on_pushButton_zongceshi_clicked();
}




void chicunceliang::on_action_open_image_file_triggered()
{

    ui->label_yj->OnSelectImage();



    //    QString OpenFile, OpenFilePath;
    //    QImage image;
    //    //打开文件夹中的图片文件
    //    OpenFile = QFileDialog::getOpenFileName(this,
    //                                            "Please choose an image file",
    //                                            "",
    //                                            "Image Files(*.jpg *.png *.bmp *.pgm *.pbm);;All(*.*)");
    //    if( OpenFile != "" )
    //    {

    //        m_img = cv::imread(OpenFile.toStdString(),-1);
    //        //if( image.load(OpenFile) )
    //        if(!m_img.empty())
    //        {
    //            huancun_tuxiang = m_img.clone();
    //            if(m_img.type()==CV_8UC3)
    //            {

    //                QImage imgx((const uchar*)m_img.data, m_img.cols, m_img.rows,m_img.cols*m_img.channels(),QImage::Format_RGB888);
    //                background = QPixmap::fromImage(imgx).scaledToWidth( ui->label_show->width());
    //                this->update();

    //            }else
    //            {

    //                QImage imgx((const uchar*)m_img.data, m_img.cols, m_img.rows,m_img.cols*m_img.channels(),QImage::Format_Indexed8);
    //                background = QPixmap::fromImage(imgx).scaledToWidth( ui->label_show->width());
    //                this->update();

    //            }

    //        }
    //    }

    //    //显示所示图片的路径
    //    QFileInfo OpenFileInfo;
    //    OpenFileInfo = QFileInfo(OpenFile);
    //    OpenFilePath = OpenFileInfo.filePath();
    //   // ui->lineEdit->setText(OpenFilePath);
}



void chicunceliang::slotMouseMove(const Beishu& pos)
{
    _labScreen->setText (QString().sprintf ("Scree(%d,%d)",pos.hb_pt.x (),pos.hb_pt.y ()));
    QPoint leftTopPixmapPos;
    QPoint rightBottomPixmapPos;
    QPoint imgPos;

    if(!ui->label_yj->LocalFileName.isNull() && !ui->label_yj->Image.isNull() )
    {

        //fromGlobalPosToImgPos(QPoint(pos.hb_pt.x (),pos.hb_pt.y ()),leftTopPixmapPos,rightBottomPixmapPos,imgPos);
        if((pos.hb_pt.x ()>=pos.leftTop.x ()) && (pos.hb_pt.x ()<=pos.rightBotom.x ())
                && (pos.hb_pt.y ()>=pos.leftTop.y ()) && (pos.hb_pt.y()<=pos.rightBotom.y ()))
        {

            _labScreen->setText (QString().sprintf ("Scree(%.1f,%.1f),Img(%.1f,%.1f)",pos.hb_pt.x (),pos.hb_pt.y (),pos.img_pt.x () ,pos.img_pt.y ()));


            //ui->labImg->_isDrawCross = true;
            // ui->labImg->_isInImgInner = true;

        }
        else
        {

            _labScreen->setText (QString().sprintf ("Scree(%.1f,%.1f)",pos.hb_pt.x (),pos.hb_pt.y () ));

            // ui->labImg->_isDrawCross = false;
            // ui->labImg->_isInImgInner = false;

        }
    }
    else
    {
        _labScreen->setText (QString().sprintf ("Scree(%.1f,%.1f)",pos.hb_pt.x (),pos.hb_pt.y () ));
    }
    /* else if(_fileList.length ()>0 && ui->labImg->_isEditing)
            {
                fromGlobalPosToImgPos(pos,leftTopPixmapPos,rightBottomPixmapPos,imgPos);
                if((pos.x ()>=leftTopPixmapPos.x ()) && (pos.x ()<=rightBottomPixmapPos.x ())
                        && (pos.y ()>=leftTopPixmapPos.y ()) && (pos.y()<=rightBottomPixmapPos.y ()))
                {
                    qDebug()<<"图像坐标:"<<imgPos;
                    ui->labImg->setImgPos (imgPos);
                    _labScreen->setText (QString().sprintf ("Scree(%d,%d),Img(%d,%d)",pos.x (),pos.y (),imgPos.x (),imgPos.y ()));
                    //ui->labImg->_isInImgInner = true;
                }
                else
                {
                   // ui->labImg->_isInImgInner = false;
                }
            }*/



    //        if(_fileList.length ()>0&&(_isCreateBox||_isCreatePolygon))
    //        {
    //            fromGlobalPosToImgPos(pos,leftTopPixmapPos,rightBottomPixmapPos,imgPos);
    //            if((pos.x ()>=leftTopPixmapPos.x ()) && (pos.x ()<=rightBottomPixmapPos.x ())
    //                    && (pos.y ()>=leftTopPixmapPos.y ()) && (pos.y()<=rightBottomPixmapPos.y ()))
    //            {
    //                qDebug()<<"图像坐标:"<<imgPos;
    //                _labScreen->setText (QString().sprintf ("Scree(%d,%d),Img(%d,%d)",pos.x (),pos.y (),imgPos.x (),imgPos.y ()));
    //                QPoint gloPos = fromImgPosToGlobalPos(imgPos);
    //                qDebug()<<"世界坐标:"<<gloPos;

    //                ui->labImg->_isDrawCross = true;
    //                ui->labImg->_isInImgInner = true;

    //            }
    //            else
    //            {
    //                ui->labImg->_isDrawCross = false;
    //                ui->labImg->_isInImgInner = false;

    //            }
    //        }
    //        else if(_fileList.length ()>0 && ui->labImg->_isEditing)
    //        {
    //            fromGlobalPosToImgPos(pos,leftTopPixmapPos,rightBottomPixmapPos,imgPos);
    //            if((pos.x ()>=leftTopPixmapPos.x ()) && (pos.x ()<=rightBottomPixmapPos.x ())
    //                    && (pos.y ()>=leftTopPixmapPos.y ()) && (pos.y()<=rightBottomPixmapPos.y ()))
    //            {
    //                qDebug()<<"图像坐标:"<<imgPos;
    //                ui->labImg->setImgPos (imgPos);
    //                _labScreen->setText (QString().sprintf ("Scree(%d,%d),Img(%d,%d)",pos.x (),pos.y (),imgPos.x (),imgPos.y ()));
    //                ui->labImg->_isInImgInner = true;
    //            }
    //            else
    //            {
    //                ui->labImg->_isInImgInner = false;
    //            }
    //        }

}



void chicunceliang::on_act_shubiao_triggered()
{

    ui->label_yj->set_isPainting(false);

}

void chicunceliang::on_act_huaxian_triggered()
{


    ui->label_yj->changePaintingState();
    ui->label_yj->setPaintingType(LINEDRAW);
}

void chicunceliang::on_action_set_roi_triggered()
{
    ui->label_yj->changePaintingState();
    ui->label_yj->setPaintingType(ROITYPE);

}

void chicunceliang::on_action_set_moban_triggered()
{
    ui->label_yj->changePaintingState();
    ui->label_yj->setPaintingType(TEMPLATETYPE);
}


void chicunceliang::on_act_fangdatuxiang_triggered()
{
    ui->label_yj->OnZoomInImage();
}

void chicunceliang::on_action_suoxiaotuxiang_triggered()
{

    ui->label_yj->OnZoomOutImage();


}

void chicunceliang::on_action_yuanshidaxiao_triggered()
{
    ui->label_yj->OnPresetImage();
}

void chicunceliang::on_action_huajuxing_triggered()
{
    ui->label_yj->changePaintingState();
    ui->label_yj->setPaintingType(RECTTYPE);
}

void chicunceliang::on_action_huayuanxing_triggered()
{

}


void chicunceliang::on_action_cexiaocaozuo_triggered()
{
    if(ui->label_yj->_rectsIsEmpty()&&ui->label_yj->_ellipsesIsEmpty() && ui->label_yj->_lineIsEmpty()&& ui->label_yj->_ROIRCIsEmpty())
    {
        QMessageBox::information(this,"information", ("无对象"));
    }
    else{
        ui->label_yj->OnPaintingback();
    }

}


void chicunceliang::on_horizontalSlider_duibidu_sliderReleased()
{
    //ui->label_yj->setContrast((float)ui->horizontalSlider_duibidu->value()/100);
}

void chicunceliang::on_horizontalSlider_liangdu_sliderReleased()
{
    // ui->label_yj->setBrightness(ui->horizontalSlider_liangdu->value());
}

void chicunceliang::on_horizontalSlider_duibidu_valueChanged(int value)
{
    //ui->doubleSpinBox_duibidui->setValue((double)ui->horizontalSlider_duibidu->value()/100);
}

void chicunceliang::on_horizontalSlider_liangdu_valueChanged(int value)
{
    // ui->doubleSpinBox_liangdu->setValue(ui->horizontalSlider_liangdu->value());
}

void chicunceliang::on_doubleSpinBox_liangdu_valueChanged(double arg1)
{
    // ui->horizontalSlider_liangdu->setValue(ui->doubleSpinBox_liangdu->value());
    // ui->label_yj->setBrightness(ui->horizontalSlider_liangdu->value());


}

void chicunceliang::on_doubleSpinBox_duibidui_valueChanged(double arg1)
{

    // ui->horizontalSlider_duibidu->setValue(ui->doubleSpinBox_duibidui->value()*100);
    // ui->label_yj->setContrast((float)ui->horizontalSlider_duibidu->value()/100);
}



void chicunceliang::on_action_huatuanyuanxing_triggered()
{
    ui->label_yj->changePaintingState();
    ui->label_yj->setPaintingType(ELLIPSETYPE);
}



void chicunceliang::on_action_delete_line_triggered()
{
    ui->label_yj->delete_line(0);
}



void chicunceliang::on_pushButton_set_ROI_clicked()
{

    if(ui->label_yj->_roi_rects.size() >0)
    {
        int index = ui->label_yj->xuanzhe_rect;

        ROI_img_space_rc = ui->label_yj->_roi_rects[index].img_space_rc;

        if_have_ROI =1;

    }else if(!ui->label_yj->Image.isNull())
    {
        ROI_img_space_rc = QRectF(0 ,0 ,ui->label_yj->Image.width(), ui->label_yj->Image.height());
        if_have_ROI =1;
    }
}

void chicunceliang::on_pushButton_set_template_clicked()
{

    //如果有矩形,就获取选中的矩形作为模板的矩形
    if(ui->label_yj->_roi_rects.size() >0)
    {
        int index = ui->label_yj->xuanzhe_rect;

        Template_img_space_rc = ui->label_yj->_roi_rects[index].img_space_rc;




        guocheng_4_pt.ptLT =  cv::Point2f(Template_img_space_rc.topLeft().x() , Template_img_space_rc.topLeft().y());
        guocheng_4_pt.ptLB = cv::Point2f(Template_img_space_rc.bottomLeft().x() , Template_img_space_rc.bottomLeft().y());//Template_img_space_rc.bottomLeft();
        guocheng_4_pt.ptRT = cv::Point2f(Template_img_space_rc.topRight().x() , Template_img_space_rc.topRight().y());;//Template_img_space_rc.topRight();//Left();
        guocheng_4_pt.ptRB = cv::Point2f(Template_img_space_rc.bottomRight().x() , Template_img_space_rc.bottomRight().y());// Template_img_space_rc.bottomRight();


        if(!ui->label_yj->Image.isNull())
        {
            cv::Mat x =  ui->label_yj->QImageToMat(  ui->label_yj->Image).clone();

            if (  x.empty() )
            {
                cout<<"x empty"<<endl;
                return  ;
            }

            QImage dstx = ui->label_yj->Image.copy(  Template_img_space_rc.x(), Template_img_space_rc.y(),Template_img_space_rc.width(),Template_img_space_rc.height());


            std::cout<<dstx.width()<<"  "<< dstx.height()<<std::endl;
            Temlplate_img = ui->label_yj->QImageToMat(dstx).clone();


            if (  Temlplate_img.empty() )
            {
                cout<<"Temlplate_img empty"<<endl;
                return  ;
            }
            ui->label_show_template->setPixmap(QPixmap::fromImage(dstx.scaled(ui->label_show_template->size())));

            if_have_template = 1;


            m_kachi_diaoyong.m_dMaxOverlap  = ui->lineEdit_chongdiexishu->text().toDouble();
            m_kachi_diaoyong.m_dScore =  ui->lineEdit_fengzhi->text().toDouble();
            m_kachi_diaoyong.m_dToleranceAngle =  ui->lineEdit_jiaodufanwei->text().toDouble();
            m_kachi_diaoyong.m_iMinReduceArea =  ui->lineEdit_zuixiaomianji->text().toInt();
            m_kachi_diaoyong.m_iMaxPos =  ui->lineEdit_mubiaoshu->text().toInt();
            m_kachi_diaoyong.m_bSubPixelEstimation =  ui->checkBox_cixiangshu->isChecked();

            cout<<"cteater template ok"<<endl;




        }


    }
}

void chicunceliang::on_pushButton_delete_template_clicked()
{
    if_have_template = 0;
}

void chicunceliang::on_pushButton_test_ch_clicked()
{

    //measure_bc x_m_msobj;
#if 1
    if(ui->label_yj->Image.isNull()) return;

    // cv::Mat x =  ui->label_yj->QImageToMat(  ui->label_yj->Image).clone();

    QImage ROI_x = ui->label_yj->Image;


    m_matSrc = ui->label_yj->QImageToMat(ROI_x).clone();

    Temlplate_img =m_kachi_diaoyong.m_matDst;

    if (m_matSrc.empty () || Temlplate_img.empty ())
        return  ;
    if ((Temlplate_img.cols < m_matSrc.cols && Temlplate_img.rows > m_matSrc.rows) || (Temlplate_img.cols > m_matSrc.cols && Temlplate_img.rows < m_matSrc.rows))
        return  ;
    if (Temlplate_img.size ().area () > m_matSrc.size ().area ())
        return  ;
    //if (!m_TemplData.bIsPatternLearned)
    //    return  ;

    if(m_matSrc.channels() == 4 )
    {

        cv::cvtColor(m_matSrc, m_kachi_diaoyong.m_matSrc, CV_RGBA2GRAY);


    }else if(m_matSrc.channels() == 3 )
    {
        cv::cvtColor(m_matSrc, m_kachi_diaoyong.m_matSrc, CV_RGB2GRAY);


    }else
    {
        m_kachi_diaoyong.m_matSrc = m_matSrc;
    }



#else
    x_m_msobj.m_dMaxOverlap  = ui->lineEdit_chongdiexishu->text().toDouble();
    x_m_msobj.m_dScore =  ui->lineEdit_fengzhi->text().toDouble();
    x_m_msobj.m_dToleranceAngle =  ui->lineEdit_jiaodufanwei->text().toDouble();
    x_m_msobj.m_iMinReduceArea =  ui->lineEdit_zuixiaomianji->text().toInt();
    x_m_msobj.m_iMaxPos =  ui->lineEdit_mubiaoshu->text().toInt();


    x_m_msobj.m_matSrc = cv::imread("./bidui/0_src.jpg", cv::IMREAD_GRAYSCALE);
    x_m_msobj.m_matDst = cv::imread("./bidui/0_temp.jpg", cv::IMREAD_GRAYSCALE);
#endif



    double t = (double)cvGetTickCount();
    m_kachi_diaoyong.LearnPattern ();
    m_kachi_diaoyong.Match();

    if(m_kachi_diaoyong.m_vecSingleTargetData.size()>0)
    {
        //Template_img_space_rc_guocheng = x_m_msobj.m_vecSingleTargetData[0].result_rc;

        guocheng_4_pt.ptLT = m_kachi_diaoyong.m_vecSingleTargetData[0].ptLT;
        guocheng_4_pt.ptLB = m_kachi_diaoyong.m_vecSingleTargetData[0].ptLB;
        guocheng_4_pt.ptRT = m_kachi_diaoyong.m_vecSingleTargetData[0].ptRT;
        guocheng_4_pt.ptRB =  m_kachi_diaoyong.m_vecSingleTargetData[0].ptRB;

    }
    ui->label_yj->draw_matchs(1, m_kachi_diaoyong.m_vecSingleTargetData);

    t = (double)cvGetTickCount() - t;
    printf( "detection time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );

}
void chicunceliang::on_pushButton_test_template_match_clicked()
{


    measure_bc x_m_msobj;
#if 1
    if(ui->label_yj->Image.isNull()) return;

    if( ui->label_yj->_roi_rects.size() <=0) return;

    cv::Mat x =  ui->label_yj->QImageToMat(  ui->label_yj->Image).clone();


    if(if_have_ROI ==0)
    {
        ROI_img_space_rc = QRectF(0,0,ui->label_yj->Image.width(), ui->label_yj->Image.height()) ;
    }
    qDebug()<<ROI_img_space_rc;

    QImage ROI_x = ui->label_yj->Image.copy( ROI_img_space_rc.x(), ROI_img_space_rc.y(),ROI_img_space_rc.width(),ROI_img_space_rc.height());


    m_matSrc = ui->label_yj->QImageToMat(ROI_x).clone();

    if ( Temlplate_img.empty ())
    {
        std::cout<<"Temlplate_img empty"<<std::endl;
        return  ;
    }

    if (m_matSrc.empty ()  )
    {
        std::cout<<"m_matSrc empty"<<std::endl;
        return  ;
    }

    if (Temlplate_img.rows > m_matSrc.rows || Temlplate_img.cols > m_matSrc.cols )
    {

        std::cout<<"Temlplate_img.cols < m_matSrc.cols "<<std::endl;
        return  ;
    }

    if (Temlplate_img.size ().area () > m_matSrc.size ().area ())
        return  ;
    //if (!m_TemplData.bIsPatternLearned)
    //    return  ;

    std::cout<<"start test mcc"<<std::endl;

    x_m_msobj.m_dMaxOverlap  = ui->lineEdit_chongdiexishu->text().toDouble();
    x_m_msobj.m_dScore =  ui->lineEdit_fengzhi->text().toDouble();
    x_m_msobj.m_dToleranceAngle =  ui->lineEdit_jiaodufanwei->text().toDouble();
    x_m_msobj.m_iMinReduceArea =  ui->lineEdit_zuixiaomianji->text().toInt();
    x_m_msobj.m_iMaxPos =  ui->lineEdit_mubiaoshu->text().toInt();
    x_m_msobj.m_bSubPixelEstimation =  ui->checkBox_cixiangshu->isChecked();


    m_kachi_diaoyong.m_dMaxOverlap  = ui->lineEdit_chongdiexishu->text().toDouble();
    m_kachi_diaoyong.m_dScore =  ui->lineEdit_fengzhi->text().toDouble();
    m_kachi_diaoyong.m_dToleranceAngle =  ui->lineEdit_jiaodufanwei->text().toDouble();
    m_kachi_diaoyong.m_iMinReduceArea =  ui->lineEdit_zuixiaomianji->text().toInt();
    m_kachi_diaoyong.m_iMaxPos =  ui->lineEdit_mubiaoshu->text().toInt();
    m_kachi_diaoyong.m_bSubPixelEstimation =  ui->checkBox_cixiangshu->isChecked();




    if(m_matSrc.channels() == 4 )
    {

        cv::cvtColor(m_matSrc, x_m_msobj.m_matSrc, CV_RGBA2GRAY);


    }else if(m_matSrc.channels() == 3 )
    {
        cv::cvtColor(m_matSrc, x_m_msobj.m_matSrc, CV_RGB2GRAY);


    }else
    {
        x_m_msobj.m_matSrc = m_matSrc;
    }


    if(Temlplate_img.channels() == 4 )
    {

        cv::cvtColor(Temlplate_img,  x_m_msobj.m_matDst, CV_RGBA2GRAY);


    }else  if(Temlplate_img.channels() == 3 )
    {
        cv::cvtColor(Temlplate_img,  x_m_msobj.m_matDst, CV_RGB2GRAY);

    }else
    {

        x_m_msobj.m_matDst = Temlplate_img;
    }

    x_m_msobj.set_roi_offset(ROI_img_space_rc.topLeft().x(), ROI_img_space_rc.topLeft().y());




    cv::imwrite("./bidui/0_src.jpg", x_m_msobj.m_matSrc);
    cv::imwrite("./bidui/0_temp.jpg", x_m_msobj.m_matDst);

#else
    x_m_msobj.m_dMaxOverlap  = ui->lineEdit_chongdiexishu->text().toDouble();
    x_m_msobj.m_dScore =  ui->lineEdit_fengzhi->text().toDouble();
    x_m_msobj.m_dToleranceAngle =  ui->lineEdit_jiaodufanwei->text().toDouble();
    x_m_msobj.m_iMinReduceArea =  ui->lineEdit_zuixiaomianji->text().toInt();
    x_m_msobj.m_iMaxPos =  ui->lineEdit_mubiaoshu->text().toInt();


    x_m_msobj.m_matSrc = cv::imread("./bidui/0_src.jpg", cv::IMREAD_GRAYSCALE);
    x_m_msobj.m_matDst = cv::imread("./bidui/0_temp.jpg", cv::IMREAD_GRAYSCALE);
#endif



    double t = (double)cvGetTickCount();
    x_m_msobj.LearnPattern ();
    x_m_msobj.Match();

    if(x_m_msobj.m_vecSingleTargetData.size()>0)
    {
        //Template_img_space_rc_guocheng = x_m_msobj.m_vecSingleTargetData[0].result_rc;

        guocheng_4_pt.ptLT = x_m_msobj.m_vecSingleTargetData[0].ptLT;
        guocheng_4_pt.ptLB = x_m_msobj.m_vecSingleTargetData[0].ptLB;
        guocheng_4_pt.ptRT = x_m_msobj.m_vecSingleTargetData[0].ptRT;
        guocheng_4_pt.ptRB =  x_m_msobj.m_vecSingleTargetData[0].ptRB;

    }
    ui->label_yj->draw_matchs(1, x_m_msobj.m_vecSingleTargetData);

    t = (double)cvGetTickCount() - t;
    printf( "detection time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );

}


void chicunceliang::on_action_jisuangduixiang_triggered()
{

    cv::Mat x =  ui->label_yj->QImageToMat(  ui->label_yj->Image).clone();




    if(ui->label_yj->shapeType.last()==RECTTYPE)
    {
        // painter2.drawRect(ui->label_yj->_rects.last());
    }
    if(ui->label_yj->shapeType.last()==ELLIPSETYPE)
    {
        //painter2.drawEllipse(_ellipses.last());
    }
    if(ui->label_yj->shapeType.last()==LINEDRAW)
    {

        cv::Point2f pt1(ui->label_yj->_measure_lineV.last().pt_img_start.x() , ui->label_yj->_measure_lineV.last().pt_img_start.y());
        cv::Point2f pt2(ui->label_yj->_measure_lineV.last().pt_img_end.x() , ui->label_yj->_measure_lineV.last().pt_img_end.y());

        qDebug()<<pt1.x<<" "<< pt1.y;
        qDebug()<<pt2.x<<" "<< pt2.y;



        measure_bc m_msobj;

        m_msobj.m_obj.cfg.pt1 = cv::Point( cvRound(pt1.x) , cvRound(pt1.y));
        m_msobj.m_obj.cfg.pt2 = cv::Point(cvRound(pt2.x) , cvRound(pt2.y));


        m_msobj.m_obj.cfg.m_kachi_cd = ui->lineEdit_kachichangdu->text().toInt();
        m_msobj.m_obj.cfg.m_kachi_kd = ui->lineEdit_kachikuandu->text().toInt();
        m_msobj.m_obj.cfg.m_kachi_sl = ui->lineEdit_kachishuliang->text().toInt();
        m_msobj.m_obj.cfg.m_kachi_lvbochic = ui->lineEdit_lvbochicun->text().toInt();
        m_msobj.m_obj.cfg.m_kachi_ssfx = ui->lineEdit_sousfx->text().toInt();
        m_msobj.m_obj.cfg.m_kachi_yuzhi = ui->lineEdit_bianyuanyuzhi->text().toInt();
        m_msobj.m_obj.cfg.m_kachi_moshi = ui->lineEdit_tiqumoshi->text();

        //m_msobj.m_obj.cfg.kuan_yz = ui->lineEdit_kuan_yinzi->text().toDouble();
        // m_msobj.m_obj.cfg.gao_yz = ui->lineEdit_gao_yinzi->text().toDouble();
        // m_msobj.m_obj.cfg.jieguomoshi= ui->lineEdit_jieguo_moshi->text().toDouble();

        double t = (double)cvGetTickCount();


        m_msobj.Edge1dFindLine(x,//用OpenCV中Mat替代
                               m_msobj.m_obj.cfg.pt1,//起点，用cv替代
                               m_msobj.m_obj.cfg.pt2,//终点，用cv替代
                               m_msobj.m_obj.cfg.m_kachi_cd,//卡尺长度
                               m_msobj.m_obj.cfg.m_kachi_kd,//卡尺宽度
                               m_msobj.m_obj.cfg.m_kachi_sl, //卡尺数量
                               m_msobj.m_obj.cfg.m_kachi_lvbochic,//滤波尺寸（建议2，默认[1,5]）
                               m_msobj.m_obj.cfg.m_kachi_ssfx,//搜索方向，1默认方向，-1反方向
                               m_msobj.m_obj.cfg.m_kachi_yuzhi,//边缘阈值
                               m_msobj.m_obj.cfg.m_kachi_moshi.toStdString().c_str());//提取模式("all","positive","negative")三个选项，all提取最强边缘，positive 提取由黑到白 negative 提取由白到黑


        ui->label_yj->draw_obj("1", m_msobj.m_obj);

        t = (double)cvGetTickCount() - t;
        printf( "detection time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );



    }



}



void chicunceliang::on_pushButton_add_kachi_clicked()
{


    if(ui->label_yj->shapeType.last()==LINEDRAW)
    {

        cv::Point2f pt1(ui->label_yj->_measure_lineV.last().pt_img_start.x() , ui->label_yj->_measure_lineV.last().pt_img_start.y());
        cv::Point2f pt2(ui->label_yj->_measure_lineV.last().pt_img_end.x() , ui->label_yj->_measure_lineV.last().pt_img_end.y());
    }

    line_index++;



}



void chicunceliang::on_pushButton_bianyuanshibie_clicked()
{



    if(ui->label_yj->_measure_lineV.size() > 0 &&  !ui->label_yj->Image.isNull())
    {
        cv::Mat x =  ui->label_yj->QImageToMat(  ui->label_yj->Image).clone();

        int index = ui->label_yj->xuanzhe_zhixian;
        cv::Point2f pt1(ui->label_yj->_measure_lineV.at(index).pt_img_start.x() , ui->label_yj->_measure_lineV.at(index).pt_img_start.y());
        cv::Point2f pt2(ui->label_yj->_measure_lineV.at(index).pt_img_end.x() , ui->label_yj->_measure_lineV.at(index).pt_img_end.y());

        qDebug()<<"选择:"<<index<<endl;

        measure_bc m_msobj;

        m_msobj.m_obj.cfg.pt1 = cv::Point( cvRound(pt1.x) , cvRound(pt1.y));//起点，用cv替代
        m_msobj.m_obj.cfg.pt2 = cv::Point(cvRound(pt2.x) , cvRound(pt2.y));//终点，用cv替代


        m_msobj.m_obj.cfg.m_kachi_cd = ui->lineEdit_kachichangdu->text().toInt();//卡尺长度
        m_msobj.m_obj.cfg.m_kachi_kd = ui->lineEdit_kachikuandu->text().toInt();//卡尺宽度
        m_msobj.m_obj.cfg.m_kachi_sl = ui->lineEdit_kachishuliang->text().toInt();//卡尺数量
        m_msobj.m_obj.cfg.m_kachi_lvbochic = ui->lineEdit_lvbochicun->text().toInt();//滤波尺寸（建议2，默认[1,5]）
        m_msobj.m_obj.cfg.m_kachi_ssfx = ui->lineEdit_sousfx->text().toInt();//搜索方向，1默认方向，-1反方向
        m_msobj.m_obj.cfg.m_kachi_yuzhi = ui->lineEdit_bianyuanyuzhi->text().toInt();//边缘阈值
        m_msobj.m_obj.cfg.m_kachi_moshi = ui->lineEdit_tiqumoshi->text();//提取模式("all","positive","negative")三个选项，all提取最强边缘，positive 提取由黑到白 negative 提取由白到黑


        double t = (double)cvGetTickCount();
        m_msobj.Edge1dFindLine(x);
        ui->label_yj->draw_obj("1", m_msobj.m_obj);

        t = (double)cvGetTickCount() - t;
        printf( "detection time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );

    }

}

void chicunceliang::appendMessageToEditor(const QString &message)
{
    ui->plainTextEdit->appendPlainText(message);
    ui->plainTextEdit->moveCursor(QTextCursor::End);
    //ui->plainTextEdit->textCursor().deletePreviousChar();
}


void chicunceliang::on_listWidget_Kachi_itemPressed(QListWidgetItem *item)
{

    if( m_kachi_diaoyong.m_mesure_set.size() >0)
    {
        QString ID = m_kachi_diaoyong.m_mesure_set_item_str[item];

        Mesure_Unit_Set sg_obj = m_kachi_diaoyong.m_mesure_set[ID];

        ui->plainTextEdit->clear();


        //设置光标到文本末尾
        ui->plainTextEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
        //        //当文本数量超出一定范围就清除
        //        if ( ui->plainTextEdit->toPlainText().size() > 1024 * 4)
        //        {
        //            ui->plainTextEdit->clear();
        //        }

        for(int i = 0; i < 2; i++)
        {
            MeasureUnit A =sg_obj. m_unit[ i ];
            appendMessageToEditor(("卡尺长度:") + QString::number(A.cfg.m_kachi_cd));


            appendMessageToEditor(("卡尺宽度:") + QString::number(A.cfg.m_kachi_kd));


            appendMessageToEditor(("卡尺数量:") + QString::number(A.cfg.m_kachi_sl));


            appendMessageToEditor(("滤波尺寸:") + QString::number(A.cfg.m_kachi_lvbochic));

            appendMessageToEditor(("搜索方向:") + QString::number(A.cfg.m_kachi_ssfx));

            appendMessageToEditor(("边缘阈值:") + QString::number(A.cfg.m_kachi_yuzhi));

            appendMessageToEditor(("提取模式:") +  (A.cfg.m_kachi_moshi));


        }


        appendMessageToEditor(("ID ") + sg_obj.ID );

        appendMessageToEditor(("多少边:") + QString::number(sg_obj.duosao_bian));

        appendMessageToEditor(("宽比例因子:") + QString::number(sg_obj.kuan_yz));

        appendMessageToEditor(("高比例因子:") + QString::number(sg_obj.gao_yz));

        appendMessageToEditor(("允许最小值:") + QString::number(sg_obj.min_v));

        appendMessageToEditor(("允许最大值:") + QString::number(sg_obj.max_v));

        appendMessageToEditor(("补偿值:") + QString::number(sg_obj.buchangzhi));

        appendMessageToEditor(("是否依靠模板定位:") + QString::number(sg_obj.if_have_template_flag));

        appendMessageToEditor(("是否初始化:") + QString::number(sg_obj.shifou_chushihua));

        //MeasureUnit A =sg_obj. m_unit[0];
        //MeasureUnit B =sg_obj. m_unit[0];


    }



}


void chicunceliang::on_pushButton_add_create_kachi_clicked()
{

    QDateTime time = QDateTime::currentDateTime();
    QString ID =time.toString("yyyyMMddhhmmsszzz");


    if(ui->label_yj->_measure_lineV.size() > 0 &&  !ui->label_yj->Image.isNull())
    {

        int moshi = ui->lineEdit_jieguo_qubian_moshi->text().toInt();
        if(moshi == 1)
        {

            QString type =ID +(":单线");
            ui->listWidget_Kachi->addItem (type );

            int row = ui->listWidget_Kachi-> count ()-1;//获取当前鼠标所选行
            QListWidgetItem * curent =  ui->listWidget_Kachi->item(row);

            Mesure_Unit_Set sg_obj;

            sg_obj.duosao_bian = ui->lineEdit_jieguo_qubian_moshi->text().toInt();
            sg_obj.max_v =ui->lineEdit_long_max->text().toDouble();
            sg_obj.min_v =ui->lineEdit_long_min->text().toDouble();
            sg_obj.buchangzhi =ui->lineEdit_long_buchangzhi->text().toDouble();

            sg_obj.kuan_yz =ui->lineEdit_kuan_yinzi->text().toDouble();
            sg_obj.gao_yz =ui->lineEdit_gao_yinzi->text().toDouble();


            sg_obj.kuan_yz =ui->lineEdit_kuan_yinzi->text().toDouble();
            sg_obj.gao_yz =ui->lineEdit_gao_yinzi->text().toDouble();
            sg_obj.ID = ID;

            sg_obj.ptr_item = curent;

            if(if_have_template == 1)
            {
                sg_obj.if_have_template_flag = 1;
                sg_obj.Template_img_space_rc  = Template_img_space_rc;


                guocheng_4_pt.ptLT =  cv::Point2f(Template_img_space_rc.topLeft().x() , Template_img_space_rc.topLeft().y());
                guocheng_4_pt.ptLB = cv::Point2f(Template_img_space_rc.bottomLeft().x() , Template_img_space_rc.bottomLeft().y());//Template_img_space_rc.bottomLeft();
                guocheng_4_pt.ptRT = cv::Point2f(Template_img_space_rc.topRight().x() , Template_img_space_rc.topRight().y());;//Template_img_space_rc.topRight();//Left();
                guocheng_4_pt.ptRB = cv::Point2f(Template_img_space_rc.bottomRight().x() , Template_img_space_rc.bottomRight().y());// Template_img_space_rc.bottomRight();
            }else
            {
                sg_obj.if_have_template_flag = 0;
            }


            m_kachi_diaoyong.m_mesure_set[ID] = sg_obj;
            m_kachi_diaoyong.m_mesure_set_item_str[curent] = ID;
        }

        if(moshi == 2)
        {
            QString type =ID +(":双线");
            ui->listWidget_Kachi->addItem (type );

            int row = ui->listWidget_Kachi-> count ()-1;//获取当前鼠标所选行
            QListWidgetItem * curent =  ui->listWidget_Kachi->item(row);


            Mesure_Unit_Set sg_obj;

            sg_obj.duosao_bian = ui->lineEdit_jieguo_qubian_moshi->text().toInt();
            sg_obj.max_v =ui->lineEdit_long_max->text().toDouble();
            sg_obj.min_v =ui->lineEdit_long_min->text().toDouble();
            sg_obj.buchangzhi =ui->lineEdit_long_buchangzhi->text().toDouble();

            sg_obj.kuan_yz =ui->lineEdit_kuan_yinzi->text().toDouble();
            sg_obj.gao_yz =ui->lineEdit_gao_yinzi->text().toDouble();


            sg_obj.kuan_yz =ui->lineEdit_kuan_yinzi->text().toDouble();
            sg_obj.gao_yz =ui->lineEdit_gao_yinzi->text().toDouble();
            sg_obj.ID = ID;
            sg_obj.ptr_item = curent;

            if(if_have_template == 1)
            {
                sg_obj.if_have_template_flag = 1;
                sg_obj.Template_img_space_rc  = Template_img_space_rc;



                guocheng_4_pt.ptLT =  cv::Point2f(Template_img_space_rc.topLeft().x() , Template_img_space_rc.topLeft().y());
                guocheng_4_pt.ptLB = cv::Point2f(Template_img_space_rc.bottomLeft().x() , Template_img_space_rc.bottomLeft().y());//Template_img_space_rc.bottomLeft();
                guocheng_4_pt.ptRT = cv::Point2f(Template_img_space_rc.topRight().x() , Template_img_space_rc.topRight().y());;//Template_img_space_rc.topRight();//Left();
                guocheng_4_pt.ptRB = cv::Point2f(Template_img_space_rc.bottomRight().x() , Template_img_space_rc.bottomRight().y());// Template_img_space_rc.bottomRight();


            }else
            {
                sg_obj.if_have_template_flag = 0;
            }


            //读取的时候也可以按照这个模式来做
            m_kachi_diaoyong.m_mesure_set[ID] =sg_obj;
            m_kachi_diaoyong.m_mesure_set_item_str[curent] = ID;

        }



    }
}

void chicunceliang::on_pushButton_del_kachi_clicked()
{
    if(ui->listWidget_Kachi-> count () > 0)
    {
        int row = ui->listWidget_Kachi->currentRow();//获取当前鼠标所选行
        // ui->listWidget->takeItem(row);//删除该行
        QListWidgetItem * curent =  ui->listWidget_Kachi->takeItem(row);

        if( m_kachi_diaoyong.m_mesure_set_item_str.contains(curent))
        {
            QString id =  m_kachi_diaoyong.m_mesure_set_item_str[curent];

            int b = m_kachi_diaoyong.m_mesure_set_item_str.remove(curent);
            b = m_kachi_diaoyong.m_mesure_set.remove(id);
        }



        ui->listWidget_Kachi->removeItemWidget(curent);
        delete curent;
    }

}


void chicunceliang::on_pushButton_add_danbian_kachi_clicked()
{

    if(ui->listWidget_Kachi-> count () > 0)
    {
        int row = ui->listWidget_Kachi->currentRow();//获取当前鼠标所选行

        QListWidgetItem *  curent = ui->listWidget_Kachi->item (row );

        if( m_kachi_diaoyong.m_mesure_set_item_str.contains(curent))
        {
            QString ID =  m_kachi_diaoyong.m_mesure_set_item_str[curent];

            Mesure_Unit_Set & dan_obj = m_kachi_diaoyong.m_mesure_set[ID];

            if(ui->label_yj->_measure_lineV.size() > 0 &&  !ui->label_yj->Image.isNull())
            {
                cv::Mat x =  ui->label_yj->QImageToMat(  ui->label_yj->Image).clone();

                //选择哪跟线作为卡尺
                int index = ui->label_yj->xuanzhe_zhixian;

                QString type =ID +(":单线  线序:") +  QString::number(index);
                curent->setText(type);

                cv::Point2f pt1(ui->label_yj->_measure_lineV.at(index).pt_img_start.x() , ui->label_yj->_measure_lineV.at(index).pt_img_start.y());
                cv::Point2f pt2(ui->label_yj->_measure_lineV.at(index).pt_img_end.x() , ui->label_yj->_measure_lineV.at(index).pt_img_end.y());

                MeasureUnit m_obj_A;



                if(dan_obj.if_have_template_flag)
                {

                    // Template_img_space_rc_guocheng = dan_obj.Template_img_space_rc;

                    guocheng_4_pt.ptLT =  cv::Point2f(Template_img_space_rc.topLeft().x() , Template_img_space_rc.topLeft().y());
                    guocheng_4_pt.ptLB = cv::Point2f(Template_img_space_rc.bottomLeft().x() , Template_img_space_rc.bottomLeft().y());//Template_img_space_rc.bottomLeft();
                    guocheng_4_pt.ptRT = cv::Point2f(Template_img_space_rc.topRight().x() , Template_img_space_rc.topRight().y());;//Template_img_space_rc.topRight();//Left();
                    guocheng_4_pt.ptRB = cv::Point2f(Template_img_space_rc.bottomRight().x() , Template_img_space_rc.bottomRight().y());// Template_img_space_rc.bottomRight();



                    m_obj_A.cfg.pt1 = cv::Point( cvRound(pt1.x) , cvRound(pt1.y));//起点，用cv替代
                    m_obj_A.cfg.pt2 = cv::Point(cvRound(pt2.x) , cvRound(pt2.y));//终点，用cv替代




                }else
                {
                    m_obj_A.cfg.pt1 = cv::Point( cvRound(pt1.x) , cvRound(pt1.y));//起点，用cv替代
                    m_obj_A.cfg.pt2 = cv::Point(cvRound(pt2.x) , cvRound(pt2.y));//终点，用cv替代


                }
                m_obj_A.cfg.m_kachi_cd = ui->lineEdit_kachichangdu->text().toInt();//卡尺长度
                m_obj_A.cfg.m_kachi_kd = ui->lineEdit_kachikuandu->text().toInt();//卡尺宽度
                m_obj_A.cfg.m_kachi_sl = ui->lineEdit_kachishuliang->text().toInt();//卡尺数量
                m_obj_A.cfg.m_kachi_lvbochic = ui->lineEdit_lvbochicun->text().toInt();//滤波尺寸（建议2，默认[1,5]）
                m_obj_A.cfg.m_kachi_ssfx = ui->lineEdit_sousfx->text().toInt();//搜索方向，1默认方向，-1反方向
                m_obj_A.cfg.m_kachi_yuzhi = ui->lineEdit_bianyuanyuzhi->text().toInt();//边缘阈值
                m_obj_A.cfg.m_kachi_moshi = ui->lineEdit_tiqumoshi->text();//提取模式("all","positive","negative")三个选项，all提取最强边缘，positive 提取由黑到白 negative 提取由白到黑


                MeasureUnit m_obj_B;



                if(dan_obj.if_have_template_flag)
                {
                    // Template_img_space_rc_guocheng = dan_obj.Template_img_space_rc;

                    guocheng_4_pt.ptLT =  cv::Point2f(Template_img_space_rc.topLeft().x() , Template_img_space_rc.topLeft().y());
                    guocheng_4_pt.ptLB = cv::Point2f(Template_img_space_rc.bottomLeft().x() , Template_img_space_rc.bottomLeft().y());//Template_img_space_rc.bottomLeft();
                    guocheng_4_pt.ptRT = cv::Point2f(Template_img_space_rc.topRight().x() , Template_img_space_rc.topRight().y());;//Template_img_space_rc.topRight();//Left();
                    guocheng_4_pt.ptRB = cv::Point2f(Template_img_space_rc.bottomRight().x() , Template_img_space_rc.bottomRight().y());// Template_img_space_rc.bottomRight();



                    m_obj_B.cfg.pt1 = cv::Point( cvRound(pt1.x) , cvRound(pt1.y));//起点，用cv替代
                    m_obj_B.cfg.pt2 = cv::Point(cvRound(pt2.x) , cvRound(pt2.y));//终点，用cv替代



                }else
                {
                    m_obj_B.cfg.pt1 = cv::Point( cvRound(pt1.x) , cvRound(pt1.y));//起点，用cv替代
                    m_obj_B.cfg.pt2 = cv::Point(cvRound(pt2.x) , cvRound(pt2.y));//终点，用cv替代



                }


                m_obj_B.cfg.m_kachi_cd = ui->lineEdit_kachichangdu->text().toInt();//卡尺长度
                m_obj_B.cfg.m_kachi_kd = ui->lineEdit_kachikuandu->text().toInt();//卡尺宽度
                m_obj_B.cfg.m_kachi_sl = ui->lineEdit_kachishuliang->text().toInt();//卡尺数量
                m_obj_B.cfg.m_kachi_lvbochic = ui->lineEdit_lvbochicun->text().toInt();//滤波尺寸（建议2，默认[1,5]）
                m_obj_B.cfg.m_kachi_ssfx = - ui->lineEdit_sousfx->text().toInt();//搜索方向，1默认方向，-1反方向
                m_obj_B.cfg.m_kachi_yuzhi = ui->lineEdit_bianyuanyuzhi->text().toInt();//边缘阈值
                m_obj_B.cfg.m_kachi_moshi = ui->lineEdit_tiqumoshi->text();//提取模式("all","positive","negative")三个选项，all提取最强边缘，positive 提取由黑到白 negative 提取由白到黑

                dan_obj.shifou_chushihua = 1;

                dan_obj.m_unit[0] =m_obj_A;
                dan_obj.m_unit[1] =m_obj_B;
            }

        }

    }

}

void chicunceliang::on_pushButton_add_A_kachi_clicked()
{

}

void chicunceliang::on_pushButton_add_B_kachi_clicked()
{

}



void chicunceliang::on_pushButton_test_ch_kachi_clicked()
{

    on_pushButton_test_ch_clicked();

    if(ui->listWidget_Kachi-> count () > 0 && !ui->label_yj->Image.isNull())
    {

        cv::Mat x =  ui->label_yj->QImageToMat(  ui->label_yj->Image).clone();

        double t = (double)cvGetTickCount();

        QMap<QString ,Mesure_Unit_Set>::const_iterator it_m = m_kachi_diaoyong.m_mesure_set.constBegin();// 有几个测量对象
        while (it_m != m_kachi_diaoyong.m_mesure_set.constEnd())
        {

            QString ID = it_m.key();
            Mesure_Unit_Set  dan_obj = it_m.value();

            if( dan_obj.shifou_chushihua ==1)
            {
                cv::Mat x =  ui->label_yj->QImageToMat(  ui->label_yj->Image).clone();


                measure_bc m_msobj_A, m_msobj_B;

                qDebug()<<dan_obj.Template_img_space_rc;
                qDebug()<<ID;

                if(if_have_template == 1)
                {
                    m_kachi_diaoyong.set_dx_pt(dan_obj.m_unit[0], dan_obj.Template_img_space_rc, guocheng_4_pt,if_have_template );

                }
                m_msobj_A.m_obj =dan_obj.m_unit[0];

                m_msobj_A.m_obj.shifouhuajiantou = ui->checkBox_shifouhuajiantou->isChecked();

                if(if_have_template == 1)
                {
                    m_kachi_diaoyong.set_dx_pt(dan_obj.m_unit[1], dan_obj.Template_img_space_rc,guocheng_4_pt,if_have_template );

                }

                m_msobj_B.m_obj =dan_obj.m_unit[1];

                m_msobj_B.m_obj.shifouhuajiantou = ui->checkBox_shifouhuajiantou->isChecked();

                m_msobj_A.Edge1dFindLine(x);


                m_msobj_B.Edge1dFindLine(x);

                ui->label_yj->draw_obj(ID+"A", m_msobj_A.m_obj);

                ui->label_yj->draw_obj(ID+"B", m_msobj_B.m_obj);

            }

            ++it_m;
        }


        t = (double)cvGetTickCount() - t;
        printf( "detection time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );

    }





#if 0

    if(ui->listWidget_Kachi-> count () > 0 && !ui->label_yj->Image.isNull())
    {
        int row = ui->listWidget_Kachi->currentRow();//获取当前鼠标所选行

        QListWidgetItem *  curent = ui->listWidget_Kachi->item (row );

        if( m_kachi_diaoyong.m_mesure_set_item_str.contains(curent))
        {
            QString ID =  m_kachi_diaoyong.m_mesure_set_item_str[curent];

            Mesure_Unit_Set  dan_obj = m_kachi_diaoyong.m_mesure_set[ID];
            if( dan_obj.shifou_chushihua ==1)
            {
                cv::Mat x =  ui->label_yj->QImageToMat(  ui->label_yj->Image).clone();


                measure_bc m_msobj_A, m_msobj_B;


                qDebug()<<dan_obj.Template_img_space_rc;
                qDebug()<<ID;

                if(if_have_template == 1)
                {
                    m_kachi_diaoyong.set_dx_pt(dan_obj.m_unit[0], dan_obj.Template_img_space_rc, guocheng_4_pt,if_have_template );

                }
                m_msobj_A.m_obj =dan_obj.m_unit[0];

                m_msobj_A.m_obj.shifouhuajiantou = ui->checkBox_shifouhuajiantou->isChecked();



                if(if_have_template == 1)
                {
                    m_kachi_diaoyong.set_dx_pt(dan_obj.m_unit[1], dan_obj.Template_img_space_rc,guocheng_4_pt,if_have_template );

                }


                m_msobj_B.m_obj =dan_obj.m_unit[1];

                m_msobj_B.m_obj.shifouhuajiantou = ui->checkBox_shifouhuajiantou->isChecked();



                double t = (double)cvGetTickCount();








                m_msobj_A.Edge1dFindLine(x);
                imwrite("x.jpg",x);






                m_msobj_B.Edge1dFindLine(x);



                ui->label_yj->draw_obj(ID+"A", m_msobj_A.m_obj);

                ui->label_yj->draw_obj(ID+"B", m_msobj_B.m_obj);



                t = (double)cvGetTickCount() - t;
                printf( "detection time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );

            }

        }
    }
#endif

}

//测试单元卡尺
void chicunceliang::on_pushButton_test_kachi_2_clicked()
{


    if(ui->listWidget_Kachi-> count () > 0 && !ui->label_yj->Image.isNull())
    {
        int row = ui->listWidget_Kachi->currentRow();//获取当前鼠标所选行

        QListWidgetItem *  curent = ui->listWidget_Kachi->item (row );

        if( m_kachi_diaoyong.m_mesure_set_item_str.contains(curent))
        {
            QString ID =  m_kachi_diaoyong.m_mesure_set_item_str[curent];

            Mesure_Unit_Set  dan_obj = m_kachi_diaoyong.m_mesure_set[ID];
            if(dan_obj.duosao_bian == 1 && dan_obj.shifou_chushihua ==1)
            {
                cv::Mat x =  ui->label_yj->QImageToMat(  ui->label_yj->Image).clone();


                measure_bc m_msobj_A, m_msobj_B;



                if(if_have_template == 1)
                {
                    m_kachi_diaoyong.set_dx_pt(dan_obj.m_unit[0], dan_obj.Template_img_space_rc, guocheng_4_pt,if_have_template );

                }


                qDebug()<< dan_obj.Template_img_space_rc;


                m_msobj_A.m_obj =dan_obj.m_unit[0];

                m_msobj_A.m_obj.shifouhuajiantou = ui->checkBox_shifouhuajiantou->isChecked();



                if(if_have_template == 1)
                {
                    m_kachi_diaoyong.set_dx_pt(dan_obj.m_unit[1], dan_obj.Template_img_space_rc,guocheng_4_pt,if_have_template );

                }


                m_msobj_B.m_obj =dan_obj.m_unit[1];

                m_msobj_B.m_obj.shifouhuajiantou = ui->checkBox_shifouhuajiantou->isChecked();



                double t = (double)cvGetTickCount();


                m_msobj_A.Edge1dFindLine(x);

                m_msobj_B.Edge1dFindLine(x);



                ui->label_yj->draw_obj(ID+"A", m_msobj_A.m_obj);

                ui->label_yj->draw_obj(ID+"B", m_msobj_B.m_obj);



                t = (double)cvGetTickCount() - t;
                printf( "detection time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );

            }

        }
    }

}

void chicunceliang::on_action_clear_draw_triggered()
{
    ui->label_yj->clear_all_draw();

    ui->listWidget_Kachi->clear();
    m_kachi_diaoyong.m_mesure_set_item_str.clear();
    m_kachi_diaoyong.m_mesure_set.clear();


}

void chicunceliang::on_pushButton_zongceshi_clicked()
{


    on_pushButton_test_template_match_clicked();

    on_pushButton_test_kachi_2_clicked();
}

void chicunceliang::on_action_open_template_triggered()
{

}



bool copyFile(QString srcPath, QString dstPath, bool coverFileIfExist)
{
    srcPath.replace("\\", "/");
    dstPath.replace("\\", "/");
    if (srcPath == dstPath) {
        return true;
    }

    if (!QFile::exists(srcPath)) {  //源文件不存在
        return false;
    }

    if (QFile::exists(dstPath)) {
        if (coverFileIfExist) {
            QFile::remove(dstPath);
        }
    }

    if (!QFile::copy(srcPath, dstPath)){
        return false;
    }
    return true;
}


void chicunceliang::on_pushButton_new_kachi_kongjian_clicked()
{


    QMessageBox msgBox;
    msgBox.setText(("提示" ));
    msgBox.setInformativeText(("确实要创建吗?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    if(ret == QMessageBox::Ok){
        QString exe_path = QCoreApplication::applicationDirPath();
        string path_ini=exe_path.toStdString() +"/user";
        if (_access(path_ini.c_str(), 0) == -1)
        {
            int i = _mkdir(path_ini.c_str());
        }

        path_ini=exe_path.toStdString() +"/user/cl";
        if (_access(path_ini.c_str(), 0) == -1)
        {
            int i = _mkdir(path_ini.c_str());
        }

        QDateTime time = QDateTime::currentDateTime();
        QString ID =time.toString("yyyy-MM-dd-hh-mm-ss-zzz");


        if(!ui->label_yj->Image.isNull())
        {



            m_kachi_diaoyong.src_name = ui->label_yj->file_name;


            if(ui->lineEdit_cfg_name->text().isEmpty())
            {
                QString path_fold =exe_path + "/user/cl/" + ID;

                if (_access(path_fold.toStdString().c_str(), 0) == -1)
                {
                    int i = _mkdir(path_fold.toStdString().c_str());
                }


                m_kachi_diaoyong.path_fold =path_fold;
                m_kachi_diaoyong.ID = ID;

                m_kachi_diaoyong.src_name =ID+"_src.jpg";

                copyFile(ui->label_yj->file_path,path_fold+"/"+ ID+"_src.jpg",true);

                QString path =path_fold+"/"+  ID+ ".ini";
                QSettings* configIni = new QSettings(path, QSettings::IniFormat);
                configIni->setValue("project/ID", ID);
                configIni->setValue("project/src_name",  m_kachi_diaoyong.src_name);


                configIni->setValue("project/moren",  1);

                delete configIni;


                QMessageBox msgBox;
                msgBox.setText(("提示" ));
                msgBox.setInformativeText(("创建成功"));
                msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
                msgBox.setDefaultButton(QMessageBox::Ok);
                int ret = msgBox.exec();


            }else
            {
                ID = ui->lineEdit_cfg_name->text();

                m_kachi_diaoyong.ID = ID;

                m_kachi_diaoyong.src_name =ID+"_src.jpg";

                QString path_fold =exe_path + "/user/cl/" + ID;

                if (_access(path_fold.toStdString().c_str(), 0) == -1)
                {
                    int i = _mkdir(path_fold.toStdString().c_str());
                }

                m_kachi_diaoyong.path_fold =path_fold;
                m_kachi_diaoyong.ID = ID;

                m_kachi_diaoyong.src_name =ID+"_src.jpg";

                copyFile(ui->label_yj->file_path,path_fold+"/"+ ID+"_src.jpg",true);

                QString path =path_fold+"/"+  ID+ ".ini";
                QSettings* configIni = new QSettings(path, QSettings::IniFormat);
                configIni->setValue("project/ID", ID);
                configIni->setValue("project/src_name",  m_kachi_diaoyong.src_name);


                configIni->setValue("project/moren",  1);

                delete configIni;

                QMessageBox msgBox;
                msgBox.setText(("提示" ));
                msgBox.setInformativeText(("创建成功"));
                msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
                msgBox.setDefaultButton(QMessageBox::Ok);
                int ret = msgBox.exec();


            }

        }else
        {
            QMessageBox msgBox;
            msgBox.setText(("提示" ));
            msgBox.setInformativeText(("没有图像"));
            msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Ok);
            int ret = msgBox.exec();
        }
    }
}



void chicunceliang::on_pushButton_kachi_baocun_moban_clicked()
{

    if( m_kachi_diaoyong.path_fold != "" && (if_have_template == 1))
    {

    }else
    {

        QMessageBox msgBox;
        msgBox.setText(("提示" ));
        msgBox.setInformativeText(("没设定模板或创建空间"));
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();

        return;
    }

    QMessageBox msgBox;
    msgBox.setText(("提示" ));
    msgBox.setInformativeText(("确实要保存模板?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    if(ret == QMessageBox::Ok){

        if( m_kachi_diaoyong.path_fold != "" && (if_have_template == 1))
        {

            QString path =m_kachi_diaoyong.path_fold+"/"+ m_kachi_diaoyong.ID+"_tpl.jpg";

            cv::imwrite(path.toStdString(),Temlplate_img);

            m_kachi_diaoyong.moban_name =  m_kachi_diaoyong.ID+"_tpl.jpg";

            m_kachi_diaoyong.moban_in_src_rc =Template_img_space_rc;

            QString pathINI =m_kachi_diaoyong.path_fold+"/"+  m_kachi_diaoyong.ID+ ".ini";
            QSettings* configIni = new QSettings(pathINI, QSettings::IniFormat);

            configIni->setValue("project/moban_name",   m_kachi_diaoyong.moban_name);
            configIni->setValue("project/moban_x",   m_kachi_diaoyong.moban_in_src_rc.x());
            configIni->setValue("project/moban_y",   m_kachi_diaoyong.moban_in_src_rc.y());

            configIni->setValue("project/moban_w",   m_kachi_diaoyong.moban_in_src_rc.width());
            configIni->setValue("project/moban_h",   m_kachi_diaoyong.moban_in_src_rc.height());


            configIni->setValue("project/m_dMaxOverlap",   m_kachi_diaoyong.m_dMaxOverlap);
            configIni->setValue("project/m_dScore",   m_kachi_diaoyong.m_dScore);
            configIni->setValue("project/m_dToleranceAngle",   m_kachi_diaoyong.m_dToleranceAngle);
            configIni->setValue("project/m_iMinReduceArea",   m_kachi_diaoyong.m_iMinReduceArea);
            configIni->setValue("project/m_iMaxPos",   m_kachi_diaoyong.m_iMaxPos);
            configIni->setValue("project/m_bSubPixelEstimation",   m_kachi_diaoyong.m_bSubPixelEstimation);

            delete configIni;

            QMessageBox msgBox;
            msgBox.setText(("提示" ));
            msgBox.setInformativeText(("保存模板成功"));
            msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Ok);
            int ret = msgBox.exec();

        }

    }

}

void chicunceliang::on_pushButton_baocunkachi_clicked()
{


    if( m_kachi_diaoyong.path_fold != "")
    {

        QMessageBox msgBox;
        msgBox.setText(("提示" ));
        msgBox.setInformativeText(("是否要保存卡尺"));
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();

        if(ret == QMessageBox::Ok){

        }else
        {
            return;
        }


    }else
    {

        QMessageBox msgBox;
        msgBox.setText(("提示" ));
        msgBox.setInformativeText(("没有卡尺空间"));
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();

        return;
    }

    if(m_kachi_diaoyong.m_mesure_set.size() >0)
    {

        int size_line = m_kachi_diaoyong.m_mesure_set.size();

        QString pathINI =m_kachi_diaoyong.path_fold+"/"+  m_kachi_diaoyong.ID+ ".ini";
        QSettings* configIni = new QSettings(pathINI, QSettings::IniFormat);

        configIni->setValue("project/line_num",  size_line);

        int jishu = 0;
        QMap<QString ,Mesure_Unit_Set>::const_iterator it_m = m_kachi_diaoyong.m_mesure_set.constBegin();// 有几个测量对象
        while (it_m != m_kachi_diaoyong.m_mesure_set.constEnd())
        {


            Mesure_Unit_Set  dan_obj = it_m.value();

            QString line_str = "line_"+QString::number(jishu) + "/";

            QString ID = dan_obj.ID;

            configIni->setValue(line_str + "ID",   ID);

            configIni->setValue(line_str + "duosao_bian",   dan_obj.duosao_bian);
            configIni->setValue(line_str + "kuan_yz",   dan_obj.kuan_yz);
            configIni->setValue(line_str + "gao_yz",   dan_obj.gao_yz);
            configIni->setValue(line_str + "min_v",   dan_obj.min_v);
            configIni->setValue(line_str + "max_v",   dan_obj.max_v);

            configIni->setValue(line_str + "buchangzhi",   dan_obj.buchangzhi);

            MeasureUnit m_obj_A  = dan_obj.m_unit[0] ;

            configIni->setValue(line_str + "pt1_x",   m_obj_A.cfg.pt1.x);
            configIni->setValue(line_str + "pt1_y",   m_obj_A.cfg.pt1.y);

            configIni->setValue(line_str + "pt2_x",   m_obj_A.cfg.pt2.x);
            configIni->setValue(line_str + "pt2_y",   m_obj_A.cfg.pt2.y);

            configIni->setValue(line_str + "m_kachi_cd",   m_obj_A.cfg.m_kachi_cd);
            configIni->setValue(line_str + "m_kachi_kd",   m_obj_A.cfg.m_kachi_kd);
            configIni->setValue(line_str + "m_kachi_sl",   m_obj_A.cfg.m_kachi_sl);
            configIni->setValue(line_str + "m_kachi_lvbochic",   m_obj_A.cfg.m_kachi_lvbochic);
            configIni->setValue(line_str + "m_kachi_ssfx",   m_obj_A.cfg.m_kachi_ssfx);
            configIni->setValue(line_str + "m_kachi_yuzhi",   m_obj_A.cfg.m_kachi_yuzhi);
            configIni->setValue(line_str + "m_kachi_moshi",   m_obj_A.cfg.m_kachi_moshi);


            jishu++;

            ++it_m;
        }

        delete configIni;


        QMessageBox msgBox;
        msgBox.setText(("提示" ));
        msgBox.setInformativeText(("卡尺保存成功"));
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();

    }



}

void chicunceliang::on_action_open_celiang_cfg_triggered()
{



}


void chicunceliang::on_pushButton_baocun_peizhi_clicked()
{

    string path_ini="./user";
    if (_access(path_ini.c_str(), 0) == -1)
    {
        int i = _mkdir(path_ini.c_str());
    }

    path_ini="./user/cl";
    if (_access(path_ini.c_str(), 0) == -1)
    {
        int i = _mkdir(path_ini.c_str());
    }



    if(ui->listWidget_Kachi-> count () > 0 && !ui->label_yj->Image.isNull())
    {


        on_pushButton_kachi_baocun_moban_clicked();
        on_pushButton_baocunkachi_clicked();

    }

}
void chicunceliang::on_action_save_celiang_cfg_triggered()
{

    on_pushButton_baocun_peizhi_clicked();

}




void chicunceliang::on_action_cccl_train_triggered()
{

}

void chicunceliang::on_action_baocun_template_triggered()
{
    on_pushButton_kachi_baocun_moban_clicked();
}




