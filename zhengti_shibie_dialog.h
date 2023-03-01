#ifndef ZHENGTI_SHIBIE_DIALOG_H
#define ZHENGTI_SHIBIE_DIALOG_H

#include <QDialog>

namespace Ui {
class Zhengti_shibie_Dialog;
}

class Zhengti_shibie_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Zhengti_shibie_Dialog(QWidget *parent = nullptr);
    ~Zhengti_shibie_Dialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::Zhengti_shibie_Dialog *ui;
};

#endif // ZHENGTI_SHIBIE_DIALOG_H
