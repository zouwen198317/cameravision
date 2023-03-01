#include "form_jieguo.h"
#include "ui_form_jieguo.h"
#include<windows.h>

Form_jieguo::Form_jieguo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_jieguo)
{
    ui->setupUi(this);


    flag_yunxing = 1;




    QObject::connect(  this ,SIGNAL( set_meifenzhong(double x)), this ,SLOT(get_meifenzhong(double x)));

    std::thread t1(&Form_jieguo::tongji_fenzhong,this,this);
    t1.detach();

}



void Form_jieguo::tongji_fenzhong(void * ptr)
{
    while(flag_yunxing)
    {




    }
}

void Form_jieguo::get_meifenzhong(double x)
{
    ui->lineEdit_meifenzhong->setText(QString::number(x,'f',2));
}

Form_jieguo::~Form_jieguo()
{
    flag_yunxing = 0;
    delete ui;
}

void Form_jieguo::set_zongshu(int num)
{
    ui->lineEdit_zongshu->setText(QString::number(num));
    Time_tongji x;
    x.xunhao = num;
    x.startTime =  QDateTime::currentDateTime();
   // m_tongji.push_back(x);
}
void Form_jieguo::set_liangpingshu(int num)
{
    ui->lineEdit_liangpinshu->setText(QString::number(num));
}
void Form_jieguo::set_buliangshu(int num)
{
    ui->lineEdit_ngshu->setText(QString::number(num));
}
void Form_jieguo::set_lianglv(float num)
{
    ui->lineEdit_lianglv->setText(QString::number(num,'f',3));
}
void Form_jieguo::set_bulianglv(float num)
{
    ui->lineEdit_bulianglv->setText(QString::number(num,'f',3));
}
void Form_jieguo::set_label_ng_ok(int flag)
{
    if(flag==0){
        ui->label->setStyleSheet("background-color: rgb(250, 255, 255);font-size:60px;color:red");

        ui->label->setText("NG");
    }else if(flag==1)
    {
        ui->label->setStyleSheet("background-color: rgb(250, 255, 255);font-size:60px;color:green");
        ui->label->setText("OK");

    }else
    {
        ui->label->setStyleSheet("background-color: rgb(250, 255, 255);font-size:60px;color:red");
        ui->label->setText("unknow");
    }

}

QString Form_jieguo::get_shuru_gongdanhao(){

    if(ui->lineEdit_gongdanhao->text()=="")
    {
        QDateTime current_date_time =QDateTime::currentDateTime();
        QString current_date =current_date_time.toString("yyyy-MM-dd-hh-mm-ss-zzz-ddd");
        return current_date;
    }else
    {
        return ui->lineEdit_gongdanhao->text();
    }
}
