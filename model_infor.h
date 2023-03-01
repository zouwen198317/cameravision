#ifndef MODEL_INFOR_H
#define MODEL_INFOR_H

#include <QMainWindow>

namespace Ui {
class model_infor;
}

class model_infor : public QMainWindow
{
    Q_OBJECT

public:
    explicit model_infor(QWidget *parent = nullptr);
    ~model_infor();

private slots:
    void on_pushButton_open_jujing_clicked();

    void on_pushButton_delete_clicked();

private:
    Ui::model_infor *ui;
};

#endif // MODEL_INFOR_H
