#include "form_show_img.h"
#include "ui_form_show_img.h"
#include<QGraphicsWidget>
#include<QtDebug>

#include <io.h>
#include <string>
#include <fstream>
#include <sstream>
#include<direct.h>
#include<QDateTime>

Form_show_img::Form_show_img(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_show_img)
{
    ui->setupUi(this);

    setMouseTracking(true);
    //background.load("D:/output/test.jpg");
    startPos = QPoint(0, 0);

    //     m_ui->graphicsView->update();
    connect(this,SIGNAL(send_draw_win(cv::Mat, int)),this,SLOT(get_draw_win(cv::Mat, int)));

    connect( this, SIGNAL(send_save_img_sig(cv::Mat  , int  )), this, SLOT(  slot_save_image(cv::Mat  , int  )));




    ui->checkBox_save_org->setChecked(0);
    ui->checkBox_save_ng->setChecked(0);

}

/**
 * @brief mkMutiDir  创建多级目录
 * @param path	     未创建的目录
 * @return 	     返回已创建的目录
 */

 extern QString mkMutiDir(const QString path);/*{
    QDir dir(path);
    if ( dir.exists(path)){
        return path;
    }
    QString parentDir = mkMutiDir(path.mid(0,path.lastIndexOf('/')));
    QString dirname = path.mid(path.lastIndexOf('/') + 1);
    QDir parentPath(parentDir);
    if ( !dirname.isEmpty() )
        parentPath.mkpath(dirname);
    return parentDir + "/" + dirname;
}*/

void Form_show_img::create_save_img_path()
{


    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date_first = current_date_time.toString("yyyy-MM-dd");//yyyy-MM-dd

    dir_obj_path_zong =m_save_img_root_path.toStdString() + current_date_first.toStdString() +"/";


    mkMutiDir(  QString::fromStdString(dir_obj_path_zong));



    if (_access(dir_obj_path_zong.c_str(), 0) == -1)
    {
        int i = _mkdir(dir_obj_path_zong.c_str());
        // CreateMultiLevel(dir_obj_path_zong);
    }

    dir_obj_path_ok =dir_obj_path_zong+ "ok/";

    if (_access(dir_obj_path_ok.c_str(), 0) == -1)
    {
        int i = _mkdir(dir_obj_path_ok.c_str());
    }

    dir_obj_path_ng =dir_obj_path_zong+ "ng/";

    if (_access(dir_obj_path_ng.c_str(), 0) == -1)
    {
        int i = _mkdir(dir_obj_path_ng.c_str());
    }

}


void Form_show_img::save_thread(cv::Mat img, int flag)
{


    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date_first = current_date_time.toString("yyyy-MM-dd");//yyyy-MM-dd

    dir_obj_path_zong =m_save_img_root_path.toStdString() + current_date_first.toStdString() +"/";



    mkMutiDir(  QString::fromStdString(dir_obj_path_zong));

    if (_access(dir_obj_path_zong.c_str(), 0) == -1)
    {
        int i = _mkdir(dir_obj_path_zong.c_str());
        //  CreateMultiLevel(dir_obj_path_zong);
    }



    if(flag ==2 || flag ==1)
    {
        if(if_save_img)
        {

            if (_access(dir_obj_path_ok.c_str(), 0) == -1)
            {
                int i = _mkdir(dir_obj_path_ok.c_str());
            }

            QDateTime time = QDateTime::currentDateTime();
            QString dateTime =QString::fromStdString(dir_obj_path_ok) +"/"+ time.toString("yyyy_MM_dd_hh_mm_ss_zzz")+"_"+QString::number(wind_id)+".jpg" ;
            cv::imwrite(dateTime.toStdString(),img);
        }
    }

    if(flag ==0)
    {
        if(if_save_img && if_save_img_ng )
        {

            if (_access(dir_obj_path_ng.c_str(), 0) == -1)
            {
                int i = _mkdir(dir_obj_path_ng.c_str());
            }

            QDateTime time = QDateTime::currentDateTime();
            QString dateTime =QString::fromStdString(dir_obj_path_ng) +"/"+ time.toString("yyyy_MM_dd_hh_mm_ss_zzz")+"_"+QString::number(wind_id)+".jpg" ;
            cv::imwrite(dateTime.toStdString(),img);
        }
    }
}
void Form_show_img::slot_save_image(cv::Mat img,int flag)
{
    //Sif(ui->checkBox_save_org->che)
    if(ui->checkBox_save_org->isChecked())
    {
        if_save_img = 1;
    }else
    {
        if_save_img = 0;
    }

    if(ui->checkBox_save_ng->isChecked())
    {
        if_save_img_ng = 1;
    }else
    {
        if_save_img_ng = 0;
    }


    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date_first = current_date_time.toString("yyyy-MM-dd");//yyyy-MM-dd

    dir_obj_path_zong =m_save_img_root_path.toStdString() + current_date_first.toStdString() +"/";



    mkMutiDir(  QString::fromStdString(dir_obj_path_zong));

    if (_access(dir_obj_path_zong.c_str(), 0) == -1)
    {
        int i = _mkdir(dir_obj_path_zong.c_str());
        //  CreateMultiLevel(dir_obj_path_zong);
    }



    if(flag ==2 || flag ==1)
    {
        if(if_save_img)
        {

            if (_access(dir_obj_path_ok.c_str(), 0) == -1)
            {
                int i = _mkdir(dir_obj_path_ok.c_str());
            }

            QDateTime time = QDateTime::currentDateTime();
            QString dateTime =QString::fromStdString(dir_obj_path_ok) +"/"+ time.toString("yyyy_MM_dd_hh_mm_ss_zzz")+"_"+QString::number(wind_id)+".jpg" ;
            cv::imwrite(dateTime.toStdString(),img);
        }
    }

    if(flag ==0)
    {
        if(if_save_img && if_save_img_ng )
        {

            if (_access(dir_obj_path_ng.c_str(), 0) == -1)
            {
                int i = _mkdir(dir_obj_path_ng.c_str());
            }

            QDateTime time = QDateTime::currentDateTime();
            QString dateTime =QString::fromStdString(dir_obj_path_ng) +"/"+ time.toString("yyyy_MM_dd_hh_mm_ss_zzz")+"_"+QString::number(wind_id)+".jpg" ;
            cv::imwrite(dateTime.toStdString(),img);
        }
    }



    //  if_save_img =
    //img_process_save_image = std::thread(&Form_show_img::save_thread, this, img, flag);
    //img_process_save_image.detach();
}


Form_show_img::~Form_show_img()
{
    delete ui;
    // delete imgL;
}

void  Form_show_img::xianshi(cv::Mat img,int index)
{

    show_img_xx(img,0);
    emit send_save_img_sig(img  , 2 );
}

void Form_show_img::pre_unix_input(  cv::Mat  img,  Alg_infor alg_infor, std::vector<tr_s_resultinfo>  next_put_ng)//接收上一单元数据
{
    m_alg_infor = alg_infor;
    m_put_ng = next_put_ng;
    //cout<<next_put_ng.size()<<endl;
    show_img_xx(img, 1);

}

int Form_show_img::show_thread( void * ptr_this,cv::Mat img, int flag)
{

    Form_show_img * ptr =( Form_show_img *) ptr_this;

    ptr->fx_show_img(img, flag);
    return 0;
}

float colors[6][3] = { { 1,0,1 },{ 0,0,1 },{ 0,1,1 },{ 0,1,0 },{ 1,1,0 },{ 1,0,0 } };

float get_color(int c, int x, int max)
{
    float ratio = ((float)x / max) * 5;
    int i = floor(ratio);
    int j = ceil(ratio);
    ratio -= i;
    float r = (1 - ratio) * colors[i][c] + ratio * colors[j][c];
    //printf("%f\n", r);
    return r;
}

string doubleToString(const double& dbNum)
{
    char* chCode;
    chCode = new(std::nothrow)char[20];
    sprintf(chCode, "%.2lf", dbNum);  // .2 是控制输出精度的，两位小数
    string strCode(chCode);
    delete[]chCode;
    return strCode;
}
void  Form_show_img::fx_show_img(cv::Mat  &show_temp_img ,int flag)
{
    cv::RNG rng(time(NULL));
    //ui->label_xianshi->setParent(imgL);
    cv::Mat img = cv::Mat(show_temp_img.size(),show_temp_img.type());
    save_image_ok = cv::Mat(show_temp_img.size(),show_temp_img.type());
    img = show_temp_img.clone();
    save_image_ok = show_temp_img.clone();
    emit send_save_img_sig(save_image_ok , 1);
    int class_num =m_alg_infor.classnamestr.size();
    vector<cv::Scalar > rgb_vc;
    for (int i = 0; i < class_num; i++)
    {
        // cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        cv::Scalar color(rand()&255, rand()&255, rand()&255);
        rgb_vc.push_back(color);
    }
    if(flag == 1)
    {
        for(int i =0 ; i < m_alg_infor.m_map_int_obj.size(); i++)
        {
            OBJ_infor temp = m_alg_infor.m_map_int_obj[i];
            for(int k = 0; k < temp.map_roi.size(); k++ )
            {

                // cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
                QRectF rcf=temp.map_roi[k];
                cv::Rect cvrc = cv::Rect(rcf.x()* m_alg_infor.sc_w, rcf.y() * m_alg_infor.sc_h,rcf.width()* m_alg_infor.sc_w, rcf.height()* m_alg_infor.sc_h);

                cv::rectangle(img, cvrc, rgb_vc[k], 20);
            }
        }

        int set_count = 0;

        if(m_put_ng.size() >0)
        {

            for (int i = 0; i < m_put_ng.size(); i++)
            {
                cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));


                float cal_w = m_put_ng[i].w * m_alg_infor.sc_w ;
                float cal_h = m_put_ng[i].h  * m_alg_infor.sc_h;
                float cal_are = cal_w * cal_h;

                int classes = 13;
                int classIDtem = m_put_ng[i].classID;
                int offset = i * 123457 % classes;
                float red = get_color(2, offset, classes);
                float green = get_color(1, offset, classes);
                float blue = get_color(0, offset, classes);
                float rgb[3];

                rgb[0] = red;
                rgb[1] = green;
                rgb[2] = blue;

                int left = (m_put_ng[i].x);
                int right = (m_put_ng[i].x + m_put_ng[i].w);
                int top = (m_put_ng[i].y);
                int bot = (m_put_ng[i].y + m_put_ng[i].h);

                if (left < 0) left = 0;
                if (right > img.cols - 1) right = img.cols - 1;
                if (top < 0) top = 0;
                if (bot > img.rows - 1) bot = img.rows - 1;

                float const font_size = img.rows / 1000.F;
                cv::Point pt1, pt2, pt_text, pt_text_bg1, pt_text_bg2;
                pt1.x = left;
                pt1.y = top;
                pt2.x = right;
                pt2.y = bot;
                pt_text.x = left;
                pt_text.y = top - 12;
                pt_text_bg1.x = left;
                pt_text_bg1.y = top - (10 + 25 * font_size);
                pt_text_bg2.x = right;
                pt_text_bg2.y = top;
                //                cv::Scalar color;
                //                color.val[0] = red * 256;
                //                color.val[1] = green * 256;
                //                color.val[2] = blue * 256;

                cv::Scalar black_color;
                black_color.val[0] = 0;
                black_color.val[1] = 255;
                black_color.val[0] = 128;

                int font_face = cv::FONT_HERSHEY_COMPLEX;
                double font_scale = 0.65;
                int thickness = 2;
                std::string str_id = std::to_string(i) + ":" + m_put_ng[i].classname +
                        " w:" + doubleToString(cal_w) + " h:" + doubleToString(cal_h) + " area:" + doubleToString(cal_are) +
                        " prob:" + doubleToString(m_put_ng[i].prob);
               // std::string str_idx = std::to_string(i);
                  std::string str_idx = /*std::to_string(classIDtem)*/m_put_ng[i].classname + doubleToString(cal_w) +":"+ doubleToString(cal_h);
                QString xxstr = QString::fromStdString(str_id);
                //emit ptui->Sig_Send_for_show_msg(xxstr);

                //int baseline;
                ////获取文本框的长宽
                //cv::Size text_size = cv::getTextSize(text, font_face, font_scale, thickness, &baseline);///
                cv::Point2f tr_pt[4];
                tr_pt[0].x = m_put_ng[i].pt_x_0;
                tr_pt[0].y = m_put_ng[i].pt_y_0;

                tr_pt[1].x = m_put_ng[i].pt_x_1;
                tr_pt[1].y = m_put_ng[i].pt_y_1;

                tr_pt[2].x = m_put_ng[i].pt_x_2;
                tr_pt[2].y = m_put_ng[i].pt_y_2;

                tr_pt[3].x = m_put_ng[i].pt_x_3;
                tr_pt[3].y = m_put_ng[i].pt_y_3;


                cv::rectangle(img, pt1, pt2, color, 4, 8);


                /*for (int k = 0; k < 4; k++) {
                                        cv::line(src_img, tr_pt[k], tr_pt[(k + 1) % 4], cv::Scalar(0, 255, 0), 4, 8);
                                    }*/
                cv::putText(img, str_idx, pt_text, font_face, font_scale, cv::Scalar(0, 0, 255), thickness, 8, 0);

                float h_rows_start = (float)50;
                float step_pt = (float)img.rows / 40.0;
                cv::Point2f pt_show = cv::Point2f((float)img.cols / 400.0, h_rows_start + step_pt * set_count);


                cv::Point2f pt_show1 = cv::Point2f((float)img.cols - 400, 500);
                cv::putText(img, "NG", pt_show1, font_face, 3, cv::Scalar(255, 0, 0), thickness, 8, 0);
                //cv::FONT_HERSHEY_COMPLEX;


                set_count++;

                emit send_save_img_sig(img  , 0 );



            }


        }else
        {
            int font_face = cv::FONT_HERSHEY_COMPLEX;
            double font_scale = 0.65;
            int thickness = 2;

            cv::Point2f pt_show1 = cv::Point2f((float)img.cols - 400, 500);
            cv::putText(img, "OK", pt_show1, font_face, 3, cv::Scalar(0,255, 0), thickness, 8, 0);
            //cv::FONT_HERSHEY_COMPLEX;

            emit send_save_img_sig(img  , 1 );
        }
    }


    if(img.type()==CV_8UC3)
    {
        cv::Mat color_img = cv::Mat(img.size(), img.type());
        cv::cvtColor(img,color_img, CV_RGB2BGR);
        QImage imgx((const uchar*)color_img.data, color_img.cols, color_img.rows,color_img.cols*color_img.channels(),QImage::Format_RGB888);
        background = QPixmap::fromImage(imgx).scaledToWidth( color_img.cols);

        this->update();

    }else
    {
        QImage imgx((const uchar*)img.data, img.cols, img.rows,img.cols*img.channels(),QImage::Format_Indexed8);
        background = QPixmap::fromImage(imgx).scaledToWidth( img.cols);
        this->update();

    }
}


void Form_show_img::contextMenuEvent(QContextMenuEvent *event)
{
    QPoint pos = event->pos();
    pos = this->mapToGlobal(pos);
    QMenu *menu = new QMenu(this);

    //QAction *loadImage = new QAction(tr("Load Image"));
    // QObject::connect(loadImage, &QAction::triggered, this, &ImageView::onLoadImage);
    // menu->addAction(loadImage);
    // menu->addSeparator();

    QAction *zoomInAction = new QAction(QString::fromLocal8Bit("缩小Zoom In"));
    QObject::connect(zoomInAction, &QAction::triggered, this, &Form_show_img::onZoomInImage);
    menu->addAction(zoomInAction);

    QAction *zoomOutAction = new QAction(QString::fromLocal8Bit("放大Zoom Out"));
    QObject::connect(zoomOutAction, &QAction::triggered, this, &Form_show_img::onZoomOutImage);
    menu->addAction(zoomOutAction);

    QAction *presetAction = new QAction(QString::fromLocal8Bit("复位Preset"));
    QObject::connect(presetAction, &QAction::triggered, this, &Form_show_img::onPresetImage);
    menu->addAction(presetAction);

    menu->exec(pos);
}



void Form_show_img::wheelEvent(QWheelEvent *event)
{
    int value = event->delta();
    if (value > 0)
        onZoomInImage();
    else
        onZoomOutImage();
    this->update();
}

void Form_show_img::mousePressEvent(QMouseEvent *event)
{
    m_OldPos = event->pos();
    m_Pressed = true;
}

void Form_show_img::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_Pressed)
        return QWidget::mouseMoveEvent(event);

    this->setCursor(Qt::SizeAllCursor);
    QPoint pos = event->pos();
    int xPtInterval = pos.x() - m_OldPos.x();
    int yPtInterval = pos.y() - m_OldPos.y();

    m_XPtInterval += xPtInterval;
    m_YPtInterval += yPtInterval;

    m_OldPos = pos;
    this->update();
}

void Form_show_img::mouseReleaseEvent(QMouseEvent *event)
{
    m_Pressed = false;
    this->setCursor(Qt::ArrowCursor);
}

//void Form_show_img::onLoadImage(void)
//{
//	QString imageFile = QFileDialog::getOpenFileName(this, "Open Image", "./", tr("Images (*.png *.xpm *.jpg)"));

//	QFile file(imageFile);
//	if (!file.exists())
//		return;

//	m_Image.load(imageFile);
//}

void Form_show_img::onZoomInImage(void)
{
    m_ZoomValue += 0.2;
    this->update();
}

void Form_show_img::onZoomOutImage(void)
{
    m_ZoomValue -= 0.2;
    if (m_ZoomValue <= 0)
    {
        m_ZoomValue += 0.2;
        return;
    }

    this->update();
}

void Form_show_img::onPresetImage(void)
{
    m_ZoomValue = 1.0;
    m_XPtInterval = 0;
    m_YPtInterval = 0;
    this->update();
}

void Form_show_img::paintEvent(QPaintEvent *event)
{
#if 1
    // 绘制样式
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    if (background.isNull())
        return QWidget::paintEvent(event);


    // 根据窗口计算应该显示的图片的大小
    int width = qMin(background.width(), this->width());
    int height = width * 1.0 / (background.width() * 1.0 / background.height());
    height = qMin(height, this->height());
    width = height * 1.0 * (background.width() * 1.0 / background.height());

    // 平移
    painter.translate(this->width() / 2 + m_XPtInterval, this->height() / 2 + m_YPtInterval);
    // 缩放
    painter.scale(m_ZoomValue, m_ZoomValue);
    // 绘制图像
    QRect picRect(-width / 2, -height / 2, width, height);
    // painter.drawImage(picRect, m_Image);
    painter.drawPixmap(picRect, background);

#endif

}

void Form_show_img::get_draw_win(cv::Mat show_temp_img, int  flag )
{

    cv::RNG rng(255);
    //ui->label_xianshi->setParent(imgL);
    cv::Mat img = cv::Mat(show_temp_img.size(),show_temp_img.type());
    img = show_temp_img.clone();


    if(flag == 1)
    {
        for(int i =0 ; i < m_alg_infor.m_map_int_obj.size(); i++)
        {
            OBJ_infor temp = m_alg_infor.m_map_int_obj[i];
            for(int k = 0; k < temp.map_roi.size(); k++ )
            {
                QRectF rcf=temp.map_roi[k];
                cv::Rect cvrc = cv::Rect(rcf.x(), rcf.y(),rcf.width(), rcf.height());

                cv::rectangle(img, cvrc, cv::Scalar(0, 0, 255), 40);
            }
        }

        int set_count = 0;
        if(m_put_ng.size() >0)
        {

            for (int i = 0; i < m_put_ng.size(); i++)
            {
                cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));


                float cal_w = m_put_ng[i].w ;
                float cal_h = m_put_ng[i].h ;
                float cal_are = cal_w * cal_h;

                int classes = 13;
                int classIDtem = m_put_ng[i].classID;
                int offset = i * 123457 % classes;
                float red = get_color(2, offset, classes);
                float green = get_color(1, offset, classes);
                float blue = get_color(0, offset, classes);
                float rgb[3];

                rgb[0] = red;
                rgb[1] = green;
                rgb[2] = blue;

                int left = (m_put_ng[i].x);
                int right = (m_put_ng[i].x + m_put_ng[i].w);
                int top = (m_put_ng[i].y);
                int bot = (m_put_ng[i].y + m_put_ng[i].h);

                if (left < 0) left = 0;
                if (right > img.cols - 1) right = img.cols - 1;
                if (top < 0) top = 0;
                if (bot > img.rows - 1) bot = img.rows - 1;

                float const font_size = img.rows / 1000.F;
                cv::Point pt1, pt2, pt_text, pt_text_bg1, pt_text_bg2;
                pt1.x = left;
                pt1.y = top;
                pt2.x = right;
                pt2.y = bot;
                pt_text.x = left;
                pt_text.y = top - 12;
                pt_text_bg1.x = left;
                pt_text_bg1.y = top - (10 + 25 * font_size);
                pt_text_bg2.x = right;
                pt_text_bg2.y = top;
                //                cv::Scalar color;
                //                color.val[0] = red * 256;
                //                color.val[1] = green * 256;
                //                color.val[2] = blue * 256;

                cv::Scalar black_color;
                black_color.val[0] = 0;
                black_color.val[1] = 255;
                black_color.val[0] = 128;

                int font_face = cv::FONT_HERSHEY_COMPLEX;
                double font_scale = 0.65;
                int thickness = 2;
                std::string str_id = std::to_string(i) + ":" + m_put_ng[i].classname +
                        " w:" + doubleToString(cal_w) + " h:" + doubleToString(cal_h) + " area:" + doubleToString(cal_are) +
                        " prob:" + doubleToString(m_put_ng[i].prob);
                std::string str_idx = std::to_string(i);
                QString xxstr = QString::fromStdString(str_id);
                //emit ptui->Sig_Send_for_show_msg(xxstr);

                //int baseline;
                ////获取文本框的长宽
                //cv::Size text_size = cv::getTextSize(text, font_face, font_scale, thickness, &baseline);///
                cv::Point2f tr_pt[4];
                tr_pt[0].x = m_put_ng[i].pt_x_0;
                tr_pt[0].y = m_put_ng[i].pt_y_0;

                tr_pt[1].x = m_put_ng[i].pt_x_1;
                tr_pt[1].y = m_put_ng[i].pt_y_1;

                tr_pt[2].x = m_put_ng[i].pt_x_2;
                tr_pt[2].y = m_put_ng[i].pt_y_2;

                tr_pt[3].x = m_put_ng[i].pt_x_3;
                tr_pt[3].y = m_put_ng[i].pt_y_3;


                cv::rectangle(img, pt1, pt2, color, 4, 8);


                /*for (int k = 0; k < 4; k++) {
                                        cv::line(src_img, tr_pt[k], tr_pt[(k + 1) % 4], cv::Scalar(0, 255, 0), 4, 8);
                                    }*/
                cv::putText(img, str_idx, pt_text, font_face, font_scale, cv::Scalar(0, 0, 255), thickness, 8, 0);

                float h_rows_start = (float)50;
                float step_pt = (float)img.rows / 40.0;
                cv::Point2f pt_show = cv::Point2f((float)img.cols / 400.0, h_rows_start + step_pt * set_count);


                cv::Point2f pt_show1 = cv::Point2f((float)img.cols - 400, 500);
                cv::putText(img, "NG", pt_show1, font_face, 3, cv::Scalar(0, 0, 255), thickness, 8, 0);
                //cv::FONT_HERSHEY_COMPLEX;


                set_count++;

            }


        }
    }


    if(img.type()==CV_8UC3)
    {
        QImage imgx((const uchar*)img.data, img.cols, img.rows,img.cols*img.channels(),QImage::Format_RGB888);
        background = QPixmap::fromImage(imgx).scaledToWidth( ui->label_xianshi->width());
        this->update();
        //ui->label_xianshi->setPixmap(my_pixmap);
        //ui->label_xianshi->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
        //ui->label_xianshi->setScaledContents(true);
    }else
    {
        QImage imgx((const uchar*)img.data, img.cols, img.rows,img.cols*img.channels(),QImage::Format_Indexed8);
        background = QPixmap::fromImage(imgx).scaledToWidth( ui->label_xianshi->width());
        this->update();
        //ui->label_xianshi->setPixmap(my_pixmap);
        //ui->label_xianshi->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
        //ui->label_xianshi->setScaledContents(true);
    }

}
void Form_show_img::show_img_xx(cv::Mat img, int flag)
{

    std::thread start_bang(&show_thread,this, img,flag);
    start_bang.detach();

    // emit send_draw_win(img, flag);


}

