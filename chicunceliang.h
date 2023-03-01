#ifndef CHICUNCELIANG_H
#define CHICUNCELIANG_H



//#include <pcl/io/pcd_io.h>
//#include <pcl/point_types.h>
//#include <pcl/registration/icp.h>



#include <QMainWindow>

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

#include <QWidget>
#include <QList>
#include <QPixmap>
#include "mypixitem.h"

#include <QGraphicsView>    //视图类
#include <QGraphicsScene>   //场景类
#include <QGraphicsItem>    //图元类
#include <math.h>
#include <QWheelEvent>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

#include <QString>


#include<map>
#include<vector>
#include<string>
#include <iostream>
#include <mutex>


#include <QFileDialog>
#include <QFileInfo>
#include <QImage>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>



#include "opencv/highgui.h"

#include "camera_commen.h"


#include "videolabel.h"

#include "measure_bc.h"

//using namespace  cv;
class videolabel;




typedef struct str_label_obj_infor {
    vector<cv::Point2f> contour_point;
    string filePath;
    int label_id = 0;
    string label_name;
    cv::Scalar  rgb_label;
    int in_image_index = 0;
    string filePath_xml;
    int circle_label = 4;
    int label_type = 0;
}Label_obj_infor;


namespace Ui {
class chicunceliang;
}

class chicunceliang : public QMainWindow
{
    Q_OBJECT

public:
    explicit chicunceliang(QWidget *parent = nullptr);
    ~chicunceliang();



     void appendMessageToEditor(const QString &message);

public:
    QString source_name ="";
    QString m_task_id_str="";
    QString m_unit_id_str="";
    int m_alg_be_call_num = 0;

    QtTreePropertyBrowser * tree_br =nullptr;
    properconfig * m_input_procfg =nullptr;

    // int Edge1dFindLine(cv::Mat image, cv::Point tpLineSt, cv::Point tpLineEd, int iCapLength, int iCapWidth, int nCalipers, int nFilterSize, int iSearchDirec, double dAmpThreshold, const char *ccTransition);
    //int Edge1dFindCircle(cv::Mat image, cv::Point tpPoint, int iRadius, int iCapLength, int iCapWidth, int nCalipers, int nFilterSize, int iSearchDirec, double dAmpThreshold, const char *ccTransition);





    //画点
public:
    int cl_process_flag = 0;
    QPoint cl_startPos;
    QPoint cl_endPos;

    //int shubiaozhuangtai_flag = 0;
    // int huanxianzhuantai_flag = 0;
    map<QString, int> zhuangtai_flag;

    QRectF ROI_img_space_rc;
    QRectF Template_img_space_rc;
   // QRectF Template_img_space_rc_guocheng;

    s_SingleTargetMatch guocheng_4_pt;
    int if_have_template = 0;
    int if_have_ROI = 0;

    cv::Mat Temlplate_img;
    cv::Mat m_matSrc;

    int line_index = 0;

    measure_bc m_kachi_diaoyong;

    QMap<QString, measure_bc *> map_kachi_Xdiaoyong;
    QVector<measure_bc > vc_kachi_Xdiaoyong ;
    void  copyPtr2Obj(measure_bc &tempX, measure_bc * ptr);




private slots:
    void on_act_test_img_triggered();
    void on_action_open_image_file_triggered();

    void on_act_shubiao_triggered();

    void on_act_huaxian_triggered();


    void on_act_fangdatuxiang_triggered();

    void on_action_suoxiaotuxiang_triggered();

    void on_action_yuanshidaxiao_triggered();

    void on_action_huajuxing_triggered();

    void on_action_cexiaocaozuo_triggered();

    void on_action_huayuanxing_triggered();

    void on_horizontalSlider_duibidu_sliderReleased();

    void on_horizontalSlider_liangdu_sliderReleased();

    void on_horizontalSlider_duibidu_valueChanged(int value);

    void on_horizontalSlider_liangdu_valueChanged(int value);

    void on_doubleSpinBox_liangdu_valueChanged(double arg1);

    void on_doubleSpinBox_duibidui_valueChanged(double arg1);

    void on_action_huatuanyuanxing_triggered();


    void slotMouseMove(const Beishu &pos);

    void on_action_jisuangduixiang_triggered();

    void on_action_delete_line_triggered();

    void on_action_set_roi_triggered();

    void on_action_set_moban_triggered();

    void on_pushButton_set_ROI_clicked();

    void on_pushButton_set_template_clicked();

    void on_pushButton_test_template_match_clicked();

    void on_pushButton_add_kachi_clicked();


    void on_pushButton_bianyuanshibie_clicked();



    void on_pushButton_delete_template_clicked();

    void on_pushButton_add_create_kachi_clicked();

    void on_pushButton_add_danbian_kachi_clicked();

    void on_pushButton_add_A_kachi_clicked();

    void on_pushButton_add_B_kachi_clicked();

    void on_pushButton_del_kachi_clicked();

    void on_pushButton_test_kachi_2_clicked();

    void on_action_clear_draw_triggered();

    void on_listWidget_Kachi_itemPressed(QListWidgetItem *item);

    void on_pushButton_zongceshi_clicked();

    void on_action_open_template_triggered();



    void on_action_save_celiang_cfg_triggered();



    void on_pushButton_new_kachi_kongjian_clicked();

    void on_pushButton_baocun_peizhi_clicked();



    void on_pushButton_kachi_baocun_moban_clicked();

    void on_pushButton_baocunkachi_clicked();

    void on_action_cccl_train_triggered();

    void on_action_baocun_template_triggered();

    void on_action_open_celiang_cfg_triggered();

    void on_pushButton_test_ch_clicked();

    void on_pushButton_test_ch_kachi_clicked();

private:
    Ui::chicunceliang *ui;

    QLabel* _labScreen;

};

#endif // CHICUNCELIANG_H
