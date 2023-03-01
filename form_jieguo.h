#ifndef FORM_JIEGUO_H
#define FORM_JIEGUO_H

#include <QWidget>
#include<QDateTime>
#include<thread>
#include<QMap>

namespace Ui {
class Form_jieguo;
}
typedef struct time_tongji
{
     QDateTime startTime;
     int xunhao;

}Time_tongji;
class Form_jieguo : public QWidget
{
    Q_OBJECT

public:
    explicit Form_jieguo(QWidget *parent = nullptr);
    ~Form_jieguo();

public:
    void set_zongshu(int);
    void set_liangpingshu(int);
      void set_buliangshu(int);
    void set_lianglv(float);
    void set_bulianglv(float);
    void set_label_ng_ok(int flag);
    QString get_shuru_gongdanhao();
    QMap<QString,int> m_tongjifenzhong;

    int flag_yunxing = 0;
    void tongji_fenzhong(void * ptr);

    QVector<Time_tongji> m_tongji;
    int count_x = 0;

public  slots:
    void get_meifenzhong(double x);

 signals:
    void set_meifenzhong(double x);


private:
    Ui::Form_jieguo *ui;
};

#endif // FORM_JIEGUO_H
