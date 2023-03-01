#include "a_chichunceliang.h"
#include "ui_a_chichunceliang.h"


#include <QLabel>
#include <QFileDialog>
#include <QMenu>
#include <QPainter>
#include <QWheelEvent>
#include <QDebug>
#include <QTime>

#include <QMessageBox>

a_chichunceliang::a_chichunceliang(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::a_chichunceliang)
{
    ui->setupUi(this);
}

a_chichunceliang::~a_chichunceliang()
{
    delete ui;
}

void a_chichunceliang::on_action_open_image_file_triggered()
{

    LocalFileName = QFileDialog::getOpenFileName(this, "Open Image", "./", tr("Image Files(*.jpg *.png *.bmp *.pgm *.pbm *.xpm);;All(*.*)"));// "Image Files(*.jpg *.png *.bmp *.pgm *.pbm *.xpm);;All(*.*)");

    QFile file(LocalFileName);
    if (!file.exists())
        return;

     backgroundPixmap = new QPixmap(LocalFileName);
     sizedImage = QImage(backgroundPixmap->toImage());


     ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
     ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

     QPixmap ConvertPixmap = QPixmap::fromImage(sizedImage);//The QPixmap class is an off-screen image representation that can be used as a paint device
     QGraphicsScene  *qgraphicsScene = new QGraphicsScene;//
     m_Image = new YJGraphicShape(&ConvertPixmap);//
     int nwith = ui->graphicsView->width();//
     int nheight = ui->graphicsView->height();//
     m_Image->setQGraphicsViewWH(nwith,nheight);//
     qgraphicsScene->addItem((QGraphicsItem*)m_Image);//
     ui->graphicsView->setSceneRect(QRectF(-(nwith/2),-(nheight/2),nwith,nheight));//
     ui->graphicsView->setScene(qgraphicsScene);//Sets the current scene to scene. If scene is already being viewed, this function does nothing.
     ui->graphicsView->setFocus();//



    //ZoomValue = 1.0;
   // XPtInterval = 0;
   // YPtInterval = 0;
    //Image.load(LocalFileName);

}


void a_chichunceliang::on_action_cexiaocaozuo_triggered()
{

}
