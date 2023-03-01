#include "zhengti_shibie_dialog.h"
#include "ui_zhengti_shibie_dialog.h"

Zhengti_shibie_Dialog::Zhengti_shibie_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Zhengti_shibie_Dialog)
{
    ui->setupUi(this);
}

Zhengti_shibie_Dialog::~Zhengti_shibie_Dialog()
{
    delete ui;
}

void Zhengti_shibie_Dialog::on_buttonBox_accepted()
{

}
