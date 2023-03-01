#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QObject>
#include <QSplashScreen>
#include <QPixmap>
#include <QWidget>
#include <QProgressBar>
#include <QTime>
#include <QCoreApplication>


class SplashScreen : public QSplashScreen
{

public:
    explicit SplashScreen(QPixmap pixmap,QWidget *parent=0);
    ~SplashScreen(){}

public:
    void setRange(int min,int max);
    void mySleep(int ms);

public slots:
    void updateNum(int n);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QProgressBar *progressBar;

};

#endif // SPLASHSCREEN_H
