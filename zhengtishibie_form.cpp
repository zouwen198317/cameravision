#include "zhengtishibie_form.h"
#include "ui_zhengtishibie_form.h"

zhengtishibie_form::zhengtishibie_form(QWidget *parent, properconfig * input_procfg) :
    QMainWindow(parent),
    ui(new Ui::zhengtishibie_form),m_input_procfg(input_procfg)
{

    ui->setupUi(this);
//    m_input_procfg =input_procfg;
//    QtProperty * prp =  input_procfg->idToProperty[QObject::tr("算法类型")];
//    cout<<"----11--->xxx"<<endl;
//    int indexID = input_procfg->enumManager->value(prp);
//    cout<<"----12--->jubushibie_form:"<<indexID<<endl;
//    QStringList alg_name_list =input_procfg->enumManager->enumNames(prp);

//    //获取算法类型
//   QString alg_type = alg_name_list[indexID];


//    m_graphicsView = ui->graphicsView;

//   // QPixmap pixmap(2448,2048);
//  //  pixItem = new MyPixItem(new QPixmap(pixmap));
//    //将该图元对象添加到场景中，并设置此图元在场景中的位置为中心（0，0）
//    m_graphicsScene = new QGraphicsScene;  //new 一个新的场景对象
//    ui->graphicsView->setSceneRect(-10/2,-10/2,10,10);
//    //设置开启滚动条范围，不然即使隐藏了滚动条后还是有滚动效果
//   // ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏横向滚动条
//   // ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏纵向滚动条
//    ui->graphicsView->setScene(m_graphicsScene);//添加场景
//   // m_graphicsScene->addItem(pixItem);//将图元添加到场景中
//   // pixItem->setPos(0,0);//中心位置
//    // setWindowTitle(globNames);
//    // scene->addPixmap(QPixmap::fromImage(qImage));
//   // ui->graphicsView->show();


}

zhengtishibie_form::~zhengtishibie_form()
{
     m_input_procfg = nullptr;
    delete ui;
}


void  zhengtishibie_form::xianshi(cv::Mat img,int index)
{


    show_img_xx(img);
    emit send_mat_to_main(img, index);

}
void zhengtishibie_form::my_change()
{
    //ui->horizontalSlider->setValue(pixItem->getScaleValue());//获取图元的放大缩小值
}

void zhengtishibie_form::show_img_xx(cv::Mat img)
{
// std::cout<<"zhengtishibie_form"<<std::endl;

    //    QMdiSubWindow *subwindow1=new QMdiSubWindow;
    //    subwindow1->setWidget(imgL);
    //    subwindow1->setAttribute(Qt::WA_DeleteOnClose);
    //    ui->mdiArea->addSubWindow(subwindow1);
    //    subwindow1->show();


#if 1
    //ui->label_xianshi->setParent(imgL);
    if(img.type()==CV_8UC3)
    {
       // QImage imgx((const uchar*)img.data, img.cols, img.rows,img.cols*img.channels(),QImage::Format_RGB888);
       // QPixmap my_pixmap = QPixmap::fromImage(imgx).scaledToWidth( ui->graphicsView->width());
      //  m_graphicsScene->addPixmap( my_pixmap);
        //ui->graphicsView->setPixmap(my_pixmap);
    }else
    {
       // QImage imgx((const uchar*)img.data, img.cols, img.rows,img.cols*img.channels(),QImage::Format_Indexed8);
       // QPixmap my_pixmap = QPixmap::fromImage(imgx).scaledToWidth( ui->graphicsView->width());
         //ui->graphicsView->setPixmap(my_pixmap);
       // m_graphicsScene->addPixmap( my_pixmap);
    }

    // imgL->
#else

    QGraphicsScene *scene = new QGraphicsScene;//最好用new的方法，临时变量有时候不起效
    static  int kkb =0;
    qDebug()<<"-----LLLL"<<kkb++<<endl;
    // scene->addWidget(show_camera_img);
    QImage imgx((const uchar*)img.data, img.cols, img.rows,img.cols*img.channels(),QImage::Format_RGB888);
    QPixmap my_pixmap = QPixmap::fromImage(imgx).scaledToWidth(512);
    scene->addPixmap(my_pixmap);
    // QGraphicsWidget *form = new QGraphicsWidget;
    //form->setWindowFlags(Qt::Window);
    // form->setWindowTitle("Widget Item");
    // form->
    // form->setLayout(layout);
    //  scene->addItem(form);
    // ui->graphicsView->setScene(scene);
    // ui->graphicsView1->setScene(scene);
    // ui->graphicsView1->show();

    //QGraphicsView *view = new QGraphicsView( scene,this );
    // ui->label_xianshi->setParent(view);
    // view->show();

#endif



}
