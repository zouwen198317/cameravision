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


extern QString   quanju_liaopan_id_str;
extern int quanju_dijikeliao;

extern int number_show ;
extern QString txm_sn  ;
//extern int shifou_tm_paizhao ;

extern QMap<QString,DangLiang> xj_dangliang;

Form_show_img::Form_show_img(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_show_img)
{

    //this->setWindowFlags(Qt::FramelessWindowHint);
    ui->setupUi(this);

    setMouseTracking(true);

    //startPos = QPoint(0, 0);

    connect( this, SIGNAL(send_save_img_sig(cv::Mat  , int  )), this, SLOT(  slot_save_image(cv::Mat  , int  )));


    //创建显示容器
    scene = new QGraphicsScene();
    ui->graphicsView_PreView->setScene(scene);
    scene->clear();


    loadedPixmapItem = new MyGraphicsItem();
    scene->addItem(loadedPixmapItem);


    ui->checkBox_save_org->setChecked(0);
    ui->checkBox_save_ng->setChecked(0);
    ui->checkBox_save_label->setChecked(0);


    connect(this,SIGNAL(send_slotDispImage(QImage)),this,SLOT(slotDispImage(QImage)));

    m_t1 = std::thread( &Form_show_img::th_xs,this, this);
    m_t1.detach();

    m_t2 = std::thread( &Form_show_img::th_bcun,this, this);
    m_t2.detach();





}


QGraphicsView* Form_show_img::getGraphicsView() const {
    return ui->graphicsView_PreView;
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



void Form_show_img::th_bcun(void * ptr)
{
    while(m_flag_xs_t)
    {
        if(bc_img_vc.size()>0 )
        {

            Mat img = bc_img_vc.front();
            int flag = bc_img_flag.front();


            bc_img_vc.pop_front();
            bc_img_flag.pop_front();


            tupian++;

            QDateTime current_date_time = QDateTime::currentDateTime();
            QString current_date_first = current_date_time.toString("yyyy-MM-dd");//yyyy-MM-dd

            dir_obj_path_zong =m_save_img_root_path.toStdString() + current_date_first.toStdString() +"/";


            mkMutiDir(  QString::fromStdString(dir_obj_path_zong));

            if (_access(dir_obj_path_zong.c_str(), 0) == -1)
            {
                cout<<"xxxxxxxxxxxxxxxx1xxxxxxxxxxxxx"<<endl;
                int i = _mkdir(dir_obj_path_zong.c_str());

            }


            dir_obj_path_ok =dir_obj_path_zong+ "ok/";

            if (_access(dir_obj_path_ok.c_str(), 0) == -1)
            {
                cout<<"xxxxxxxxxxxxx2xxxxxxxxxxxxxxxx"<<endl;
                int i = _mkdir(dir_obj_path_ok.c_str());
            }

            dir_obj_path_ng =dir_obj_path_zong+ "ng/";

            if (_access(dir_obj_path_ng.c_str(), 0) == -1)
            {
                cout<<"xxxxxxxxxxx3xxxxxxxxxxxxxxxxxx"<<endl;
                int i = _mkdir(dir_obj_path_ng.c_str());
            }


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

            if(ui->checkBox_save_label->isChecked())
            {
                budai_label = 1;
            }else
            {
                budai_label = 0;
            }


            if(flag ==2 || flag ==1)
            {

                if(if_save_img==1 )
                {
                    QDateTime time = QDateTime::currentDateTime();


                    QString path =QString::fromStdString(dir_obj_path_ok) +"/";

                    if (_access(path.toStdString().c_str(), 0) == -1)
                    {

                        int i = _mkdir(path.toStdString().c_str());
                    }

                    QString dateTime =path+ camera_name+"_"+time.toString("yyyy_MM_dd_hh_mm_ss_zzz")+"_"+QString::number(wind_id++)+ "_"+QString::number(tupian)+".jpg" ;


                    if (_access(path.toStdString().c_str(), 0) == -1)
                    {

                    }else
                    {
                        if( img.cols >10 )
                        {
                            vector<int> compression_params;
                            compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);  //选择jpeg
                            compression_params.push_back(100); //在这个填入你要的图片质量

                            //imwrite("test2.jpg", img, compression_params);

                            cv::imwrite(dateTime.toStdString(),img,compression_params);
                        }
                    }

                }

                // save_list1.push_back(img.clone());
            }

            if(flag ==0)
            {

                if(if_save_img && if_save_img_ng )
                {
                    QDateTime time = QDateTime::currentDateTime();
                    QString path =QString::fromStdString(dir_obj_path_ng) +"/";

                    if (_access(path.toStdString().c_str(), 0) == -1)
                    {

                        int i = _mkdir(path.toStdString().c_str());
                    }

                    QString dateTime =path+ camera_name+"_"+time.toString("yyyy_MM_dd_hh_mm_ss_zzz")+"_"+QString::number(wind_id)+ "_"+QString::number(tupian)+".jpg" ;


                    if (_access(path.toStdString().c_str(), 0) == -1)
                    {
                    }else
                    {
                        if( img.cols >10 )
                        {
                            //cv::imwrite(dateTime.toStdString(),img);


                            vector<int> compression_params;
                            compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);  //选择jpeg
                            compression_params.push_back(100); //在这个填入你要的图片质量

                            //imwrite("test2.jpg", img, compression_params);

                            cv::imwrite(dateTime.toStdString(),img,compression_params);

                        }
                    }
                }

            }


        }else{
            Sleep(60);
        }
    }

}
void Form_show_img::slot_save_image(Mat img,int flag)
{

    bc_img_flag.push_back(flag);
    bc_img_vc.push_back(img);


}


Form_show_img::~Form_show_img()
{

    scene->removeItem(loadedPixmapItem);
    delete loadedPixmapItem;

    m_flag_xs_t = 0;
    delete ui;
    // delete imgL;
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

void Form_show_img::show_dialg(cv::Mat imgx)
{

}

#if NO_USING_LABEL
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
#if 0
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


#else

#if 1
    // 绘制样式
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    if (m_Image.isNull())
        return QWidget::paintEvent(event);


    // 根据窗口计算应该显示的图片的大小
    int width = qMin(m_Image.width(), this->width());
    int height = width * 1.0 / (m_Image.width() * 1.0 / m_Image.height());
    height = qMin(height, this->height());
    width = height * 1.0 * (m_Image.width() * 1.0 / m_Image.height());

    // 平移
    painter.translate(this->width() / 2 + m_XPtInterval, this->height() / 2 + m_YPtInterval);
    // 缩放
    painter.scale(m_ZoomValue, m_ZoomValue);
    // 绘制图像
    QRect picRect(-width / 2, -height / 2, width, height);
    // painter.drawImage(picRect, m_Image);

    painter.drawImage(picRect, m_Image);



#else

    // 绘制样式
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    if (m_pt_Image->isNull())
        return QWidget::paintEvent(event);


    // 根据窗口计算应该显示的图片的大小
    int width = qMin(m_pt_Image->width(), this->width());
    int height = width * 1.0 /(m_pt_Image->width() * 1.0 / m_pt_Image->height());
    height = qMin(height, this->height());
    width = height * 1.0 * (m_pt_Image->width() * 1.0 / m_pt_Image->height());

    // 平移
    painter.translate(this->width() / 2 + m_XPtInterval, this->height() / 2 + m_YPtInterval);
    // 缩放
    painter.scale(m_ZoomValue, m_ZoomValue);
    // 绘制图像
    QRect picRect(-width / 2, -height / 2, width, height);
    // painter.drawImage(picRect, m_Image);

    painter.drawImage(picRect, *m_pt_Image);

    delete m_pt_Image;
    m_pt_Image = nullptr;


#endif

#endif

}

#endif

QImage Form_show_img::Mat2QImage(const cv::Mat &mat)
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

void Form_show_img::th_xs(void * ptr)
{
    while(m_flag_xs_t)
    {
        if(xs_vc.size()>0)
        {
            cv::Mat show_temp_img = xs_vc.front();
            std::vector<tr_s_resultinfo>  m_put_ng = x_m_put_ng.front();

            cv::RNG rng(time(NULL));

            cv::Mat img = cv::Mat(show_temp_img.size(),show_temp_img.type());

            img = show_temp_img.clone();

            int class_num =m_alg_infor.classnamestr.size();
            vector<cv::Scalar > rgb_vc;
            for (int i = 0; i < class_num; i++)
            {
                // cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
                cv::Scalar color(rand()&255, rand()&255, rand()&255);
                rgb_vc.push_back(color);
            }

            if(xianshi_haishi_chuli == 1)
            {
                for(int i =0 ; i < m_alg_infor.m_map_int_obj.size(); i++)
                {
                    OBJ_infor temp = m_alg_infor.m_map_int_obj[i];
                    for(int k = 0; k < temp.map_roi.size(); k++ )
                    {
                        QRectF rcf=temp.map_roi[k];
                        cv::Rect cvrc = cv::Rect(rcf.x()* m_alg_infor.sc_w, rcf.y() * m_alg_infor.sc_h,rcf.width()* m_alg_infor.sc_w, rcf.height()* m_alg_infor.sc_h);
                        if(budai_label==0)
                        {
                            cv::rectangle(img, cvrc, rgb_vc[k], 10);
                        }
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

                        int left = (m_put_ng[i].x * m_alg_infor.sc_w);
                        int right = (m_put_ng[i].x* m_alg_infor.sc_w + m_put_ng[i].w * m_alg_infor.sc_w);
                        int top = (m_put_ng[i].y* m_alg_infor.sc_h);
                        int bot = (m_put_ng[i].y * m_alg_infor.sc_h + m_put_ng[i].h * m_alg_infor.sc_h);

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
                        int thickness = 1;
                        cal_are = cal_w  * xj_dangliang[camera_name].w * cal_h  * xj_dangliang[camera_name].h;
                        std::string str_id = std::to_string(i) + ":" + m_put_ng[i].classname +
                                " w:" + doubleToString(cal_w * xj_dangliang[camera_name].w ) + " h:" + doubleToString(cal_h * xj_dangliang[camera_name].h) + " area:" + doubleToString(cal_are) +
                                " prob:" + doubleToString(m_put_ng[i].prob);


                        std::string str_idx =str_id;// std::to_string(i);
                        QString xxstr = QString::fromStdString(str_id);


                        if(budai_label==0)
                        {
                            cv::rectangle(img, pt1, pt2, color, 4, 8);



                            /*for (int k = 0; k < 4; k++) {
                                                cv::line(src_img, tr_pt[k], tr_pt[(k + 1) % 4], cv::Scalar(0, 255, 0), 4, 8);
                                            }*/


                            cv::putText(img, str_idx, pt_text, font_face, font_scale, cv::Scalar(0, 0, 255), thickness, 8, 0);

                            float h_rows_start = (float)50;
                            float step_pt = (float)img.rows / 40.0;
                            cv::Point2f pt_show = cv::Point2f((float)img.cols / 400.0, h_rows_start + step_pt * set_count);


                            cv::Point2f pt_show1 = cv::Point2f((float)img.cols - 500, 500);
                            cv::putText(img, "NG", pt_show1, font_face, 9, cv::Scalar(0, 0, 255), thickness, 8, 0);
                            //cv::FONT_HERSHEY_COMPLEX;

                        }
                        set_count++;


                    }


                    bc_img_flag.push_back(0);
                    bc_img_vc.push_back(img);

                    // emit send_show_win_mat(img);


                }else
                {
                    if(budai_label==0)
                    {
                        int font_face = cv::FONT_HERSHEY_COMPLEX;
                        double font_scale = 0.65;
                        int thickness = 5;

                        cv::Point2f pt_show1 = cv::Point2f((float)img.cols - 500, 500);
                        cv::putText(img, "OK", pt_show1, font_face, 9, cv::Scalar(0,255, 0), thickness, 8, 0);
                        //cv::FONT_HERSHEY_COMPLEX;


                    }

                    bc_img_flag.push_back(1);
                    bc_img_vc.push_back(img);

                    // emit send_show_win_mat(img);
                }
            }else
            {


                bc_img_flag.push_back(1);
                bc_img_vc.push_back(img);
            }

            if(img.type()==CV_8UC3)
            {

                //QImage imgx= Mat2QImage(img);
                // QImage imgx ;
                // imgx.loadFromData((uchar *)img.data, img.cols * img.rows *3);

                //QPixmap p;
                //background.loadFromData((uchar *)img.data, img.cols * img.rows *3);
                // cv::Mat color_img = cv::Mat(img.size(), img.type());
                // cv::cvtColor(img,color_img, CV_RGB2BGR);
                // QImage imgx((const uchar*)color_img.data, color_img.cols, color_img.rows,color_img.cols*color_img.channels(),QImage::Format_RGB888);


                // m_Image  = Mat2QImage(img);

                emit send_slotDispImage(Mat2QImage(img));



                // background = QPixmap::fromImage(imgx).scaledToWidth( img.cols);
                //ui->label_xianshi->update();
                this->update();

            }else
            {
                // QImage imgx= Mat2QImage(img);
                //QImage imgx((const uchar*)img.data, img.cols, img.rows,img.cols*img.channels(),QImage::Format_Indexed8);
                // background = QPixmap::fromImage(imgx).scaledToWidth( img.cols);

                // m_Image  = Mat2QImage(img);

                emit send_slotDispImage(Mat2QImage(img));


                //ui->label_xianshi->update();
                this->update();
            }

            xs_vc.pop_front();

            x_m_put_ng.pop_front();

        }else
        {
            Sleep(10);
        }
    }


}
void Form_show_img::slotDispImage(QImage img)
{
    loadedPixmapItem->loadImage(img);


    int nwith = ui->graphicsView_PreView->width(),nheight = ui->graphicsView_PreView->height();
    //将界面控件Graphics View的width和height传进类m_Image中
    loadedPixmapItem->setQGraphicsViewWH(nwith,nheight);
    //使视窗的大小固定在原始大小，不会随图片的放大而放大（默认状态下图片放大的时候视窗两边会自动出现滚动条，并且视窗内的视野会变大），
    //防止图片放大后重新缩小的时候视窗太大而不方便观察图片
    ui->graphicsView_PreView->setSceneRect(QRectF(-(nwith/2),-(nheight/2),nwith,nheight));
    if(m_DisplayCount>=INT_MAX-1)//防止m_DisplayCount溢出
    {
        m_DisplayCount=0;
        // m_DisplayCountBackUp=0;
    }
    m_DisplayCount++;
}
void Form_show_img::show_img_xx(cv::Mat img, int flag)
{

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

    if(ui->checkBox_save_label->isChecked())
    {
        budai_label = 1;
    }else
    {
        budai_label = 0;
    }



    xs_vc.push_back( img);


}

void  Form_show_img::xianshi(cv::Mat img,int index)
{
    xianshi_haishi_chuli = 0;
    show_img_xx(img,0);
}

void Form_show_img::pre_unix_input(  cv::Mat  img,  Alg_infor alg_infor, std::vector<tr_s_resultinfo>  next_put_ng)//接收上一单元数据
{

    xianshi_haishi_chuli = 1;
    m_alg_infor = alg_infor;
    hc_m_put_ng.clear();
    hc_m_put_ng = next_put_ng;
    std::vector<tr_s_resultinfo>  xhc_m_put_ng =next_put_ng;
    x_m_put_ng.push_back( xhc_m_put_ng);
    show_img_xx(img, 1);
}


