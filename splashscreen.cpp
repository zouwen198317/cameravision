#include "splashscreen.h"

SplashScreen::SplashScreen(QPixmap pixmap,QWidget *parent):
    QSplashScreen(parent,pixmap)
{
    installEventFilter(this);
    progressBar = new QProgressBar(this);
    progressBar->setAlignment(Qt::AlignCenter);
    // 设置进度条的位置
    progressBar->setGeometry(0,pixmap.height()/6*4,pixmap.width(),30);
}

void SplashScreen::setRange(int min,int max)
{
    progressBar->setRange(min,max);
}

void SplashScreen::updateNum(int n)
{
   // qDebug()<<n;
    // 延迟凸显效果
    mySleep(500);
    progressBar->setValue(n);
}

void SplashScreen::mySleep(int ms)
{
    QTime time = QTime::currentTime().addMSecs(ms);
    while (QTime::currentTime()<time) {
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
    }
}

bool SplashScreen::eventFilter(QObject *watched, QEvent *event)
{
    // 使用事件过滤器，防止进度条在加载中的时候，用户点击了图片，造成窗口被隐藏
    // 可以去除体验效果
    if(event->type()==QEvent::Type::MouseButtonPress || event->type()==QEvent::Type::MouseButtonDblClick)
    {
        return true;
    }
    return false;
}
