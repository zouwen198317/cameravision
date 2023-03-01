#pragma execution_character_set("utf-8")
#ifndef DIALOG_NEW_PROJECT_H
#define DIALOG_NEW_PROJECT_H

#include <QObject>
#include <QWidget>
#include <QMimeData>
#include <QApplication>
#include <QDrag>
#include <QLabel>
#include <QMdiSubWindow>
#include <iostream>

#include <QSettings>
#include <QCoreApplication>
#include <QDir>
#include<QMessageBox>
#include<QDebug>
#include<QTextCodec>
#include <QDialog>

#include <QWidget>

#include <QMainWindow>
#include <QObject>
#include <QWidget>

#include <QWidget>

#include <QMainWindow>
#include <QObject>
#include <QWidget>

#include <QMdiSubWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsObject>

#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>


#include <opencv2\opencv.hpp>
#include <math.h>
#include <map>
#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <time.h>
#include <io.h>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <thread>

#include <QMenu>
#include <QContextMenuEvent>
#include <QStyleOption>
#include <QPainter>
#include <QFileDialog>
#include <QStandardItem>
#include <QTableWidget>

typedef struct str_pro_head_biaoge_infor{

    int if_enable = 0;
    QString  name;
    QString  path;
    QTableWidgetItem *checkBox = nullptr;
    QString  miaosu;

}pro_head_biaoge_infor;



namespace Ui {
class Dialog_new_project;
}

class Dialog_new_project : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_new_project(QWidget *parent = nullptr);
    ~Dialog_new_project();
public:
    QString get_name()
    {
        return project_name;
    }
public:
    QString  project_name="";

    std::map<QString, pro_head_biaoge_infor> head_infor;
    std::string dir_user ="./";

    std::map<int,QString> rec_name;
    std::map<QString, int> state_row;
    std::map<QString, int> xiangmu_cols;

public:
    void open_pro_new_win();
    void upata_pro_xml_list();

signals:
    void send_project_infor(QString name);

private slots:
    void on_pushButton_canel_new_clicked();

    void on_pushButton_new_proj_clicked();

    void on_pushButton_openlujing_clicked();

    void on_pushButton_delete_lujing_clicked();

    void on_tableWidget_cellChanged(int row, int column);

private:
    Ui::Dialog_new_project *ui;
};

#endif // DIALOG_NEW_PROJECT_H
