#include "mainwindow.h"

#include <QApplication>

#include "splashscreen.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    QPixmap pixmap(":images/yuanjingzhinengqidong.png");
    SplashScreen sp(pixmap);
    sp.setRange(0,10);
    sp.show();

    int i=0;
    while(i<=10){
        sp.updateNum(i);
        i++;
    }



    MainWindow w;
    w.showMaximized();
    w.show();
    sp.finish(&w);
    return a.exec();
}
