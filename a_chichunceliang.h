#ifndef A_CHICHUNCELIANG_H
#define A_CHICHUNCELIANG_H

#include <QMainWindow>


#include "yjgraphicshape.h"

namespace Ui {
class a_chichunceliang;
}

class a_chichunceliang : public QMainWindow
{
    Q_OBJECT

public:
    explicit a_chichunceliang(QWidget *parent = nullptr);
    ~a_chichunceliang();

public:
    QString LocalFileName ="";
    YJGraphicShape * m_Image = nullptr;
    QPixmap *backgroundPixmap  = nullptr;
    QImage sizedImage;
private slots:
    void on_action_open_image_file_triggered();

    void on_action_cexiaocaozuo_triggered();

private:
    Ui::a_chichunceliang *ui;


};

#endif // A_CHICHUNCELIANG_H
