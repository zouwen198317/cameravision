#include "model_infor.h"
#include "ui_model_infor.h"

model_infor::model_infor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::model_infor)
{
    ui->setupUi(this);
}

model_infor::~model_infor()
{
    delete ui;
}

void model_infor::on_pushButton_open_jujing_clicked()
{

}

void model_infor::on_pushButton_delete_clicked()
{

}
