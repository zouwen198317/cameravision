#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "imagelook.h"

#include<iostream>
#include<iterator>
#include<typeinfo>

#include <QMainWindow>

#include <QCloseEvent>
#include<qfiledialog.h>
#include<qdir.h>
#include<qfile.h>


#include <qtpropertybrowser.h>
#include <qtpropertymanager.h>
#include <qtvariantproperty.h>
#include <qttreepropertybrowser.h>
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "qtbuttonpropertybrowser.h"
#include "qtgroupboxpropertybrowser.h"

#include "treeview.h"
#include "camera_commen.h"
#include "form_show_img.h"
#include "properconfig.h"
#include "dialog_new_project.h"
#include "zhengtishibie_form.h"
#include "jubushibie_form.h"
#include "outputresult.h"




#include "move_ctl.h"
#include "a_move_car.h"

#include "form_jieguo.h"
#include "chicunceliang.h"
//#include "a_chichunceliang.h"
using namespace tinyxml2;

enum SOURCE_INDEX { SRC_XIANGJI,SRC_TUPIAN,SRC_SHIPING,SRC_JIEPING,SRC_XIANSHI,SRC_BAOCUN_TUPIAN,
                    ALG_ZHENGTI_SHIBIE,ALG_ZIFU_SHIBIE,ALG_FENGE,ALG_JUBU_SHIBIE};
//enum ALG_INDEX { ALG_ZHENGTI_SHIBIE,ALG_ZIFU_SHIBIE,ALG_FENGE,ALG_JUBU_SHIBIE};




#define ZHENGSHI_YUNXING_ING 1


//工程
typedef struct str_elm_name_index_infor
{
    QString name ="";
    int index =0;
}elm_name_index_infor;

//方法单元
//template <class Unit_type1>
typedef struct str_Task_unit_infor
{
    QString str_method_in_tsk_id="";
    QString  task_id_str ="";
    QTreeWidgetItem *top_item =nullptr ;
    properconfig * proper_config= nullptr;
    OutPutResult * out_result_win = nullptr;


    int above_index = -1;
    int below_index = -1;
    void * in_unit = nullptr;
    void * out_unit = nullptr;
    QString src_name ="";
    QString task_win_name ="";
    QString benshen_name ="";

    int changshang=-1;
    int task_pos_index = -1;//在任务中的位置
    int name_index = -1;
    int  level = -1;
    int  task_id = -1;
    int flag_src_alg = -1; // 0 是资源 1 是算法
    int if_bang_flag = 0;
    void * src_ptr =nullptr;
    //Unit_type1 * all_src_ptr =nullptr;
    //  camera_commen * src_ptr_cam =nullptr;//相机指针

    //整体识别
    // zhengtishibie_form * zhengtishibie_ptr = nullptr;

    // jubushibie_form *jubushibie_ptr= nullptr;

    // Form_show_img   * src_ptr_form =nullptr;
}Task_unit_infor;

template <class Unit_type1>
struct str_MAP_INFOR
{
    QString str_method_in_tsk_id="";
    QString  task_id_str ="";
    QTreeWidgetItem *top_item =nullptr ;
    properconfig * proper_config= nullptr;

    int above_index = -1;
    int below_index = -1;
    void * in_unit = nullptr;
    void * out_unit = nullptr;
    QString src_name ="";
    QString task_win_name ="";
    QString benshen_name ="";

    int changshang=-1;
    int task_pos_index = -1;//在任务中的位置
    int name_index = -1;
    int  level = -1;
    int  task_id = -1;
    int flag_src_alg = -1; // 0 是资源 1 是算法
    int if_bang_flag = 0;

    Unit_type1 * all_src_ptr =nullptr;

};

//typedef str_Task_unit_infor<void> Task_unit_infor;

//任务
typedef struct str_vct_DragTreeWidget
{
    DragTreeWidget * ptr_DragTreeWidget =nullptr;//任务树
    QString tile_text ="";
    //int task_first_id =0;
    // int task_second_id =0;
    QString task_id_str ="";
    QMdiSubWindow * win_haldle=nullptr;
    QMap<QTreeWidgetItem *, Task_unit_infor > map_unit_method;//任务单元里面的方法属性 要记录顺序吗
    QMap<QTreeWidgetItem *, QVariant > v_map_unit_method;
}SCT_DragTreeWidget;


//工程
typedef struct str_project_create_infor
{
    QString str_project_file_name ="";
    QString str_project_name ="";
    QString str_project_file_path ="";
    QString str_project_file_cfg_fold_path ="";
    int type =0;
    QString zhushi ="";
    int  id =0;
    int  task_count =0;
    //std::vector< SCT_DragTreeWidget > ptr_task_all;
    QMap<QString,SCT_DragTreeWidget > map_task_all;
}Project_create_infor;


// template <class T1>
// void save_map(QString id, T1 *mp)
// {
//     QMap<QString, T1 *> unit;
//     unit[id] = mp;
//     return unit;
// }


template <class T1>
struct str_mp_st{
    QString src_type_name ="";
    QMap<QString, T1 *>mp;
};

// str_mp_st<camera_commen> xxx;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    int jiankong_flag = 0;

public:
    int  size_icon_m = 24;
    Form_show_img *show_camera_img =nullptr;

    int renwu_index = 0;

    std::vector< SCT_DragTreeWidget > ptr_task_all;
    QMap<QString,SCT_DragTreeWidget > map_task_all;

    std::vector<std::vector< Task_unit_infor > > m_task_connect_top_link;//总的任务链表
    std::vector<QString > m_source;
    std::vector<QString > m_alg;
    QString send_msg_unit_id="";

    QString shangyici_shijian ="";
    int flag_shangyici_shijian = 0;

    std::vector<QString > m_all_unit;

    QMap<QtProperty *, QString> propertyToId;
    QMap<QString, QtProperty *> idToProperty;

    QMap<QString, int> str2int_src_index_map;
    QMap<int, QString> int2str_src_index_map;

    QMap<QString, QListWidgetItem *> str2QListWidgetItem_map;
    QMap<QListWidgetItem * ,QString> QListWidgetItem2str_map;

    QMap<QString, QIcon> str2QIcon_map;

    QMap<QString,ImageLook *> v_imgL;



    QMap<QtProperty *,XMLElement* > m_map_pro_elm;//记录属性项目和表格项指针
    QMap<XMLElement *,QtProperty* > m_map_elm_pro;//记录属性项目和表格项指针

    QMap<elm_name_index_infor,QtProperty* > m_map_xmlindex_pro;//记录属性项目和表格项顺序
    QMap<QtProperty *,elm_name_index_infor > m_map_pro_xmlindex;//记录属性项目和表格项顺序
    int m_index_xml_elm = 0;


    Project_create_infor m_read_project;
    Project_create_infor m_current_project;
    Project_create_infor m_write_project;
signals:
    void sig_bang();
    void set_main_Fanhuijieguo(Fanhuijieguo );

public slots:
    void on_changed_from_tree_RightButton(QTreeWidgetItem * _pSource ,const QPoint& _pos, int win_id,  QMdiSubWindow * win_hanle, QString task_id );
    void onPropertyValueChanged(QtProperty *property, const QVariant &val );
    void onValueChanged(QtProperty *property,
                        const QVariant &value,
                        QString source_name ,
                        QString unit_id_str,
                        QString task_id_str,
                        QtTreePropertyBrowser * tree_br,
                        properconfig *ptr);
    void setTreaNode(QTreeWidgetItem*,QModelIndex,QString tsk_win_id);
    void slot_drop_new_method(QListWidgetItem*,QString);
    void slot_bang();
    // void  ImageLook_xianshi(cv::Mat  img, int index);

    void get_Fanhuijieguo(Fanhuijieguo );
    void set_result_data(Fanhuijieguo );


public:
    void write_properconfig_xml(QString filename, const  QString task_id,const Task_unit_infor & method_unit );
    void updata_xml(QString filename, QString source_name, properconfig * proper_config, const  QString task_id,const  QString unit_id );
    void create_prject_doc( QString filename, QString project_name);

    XMLElement*  GetElePtr_by_Attribute(XMLElement * element,const std::string Attribute_str,const std::string find_str);
    XMLElement* GetElePtr_by_name(XMLElement * element,const std::string find_str);

    void updata_camera_config( XMLElement* unit_enter_ptr, properconfig * proper_config);
    void updata_alg_config(  tinyxml2::XMLDocument &doc, XMLElement* unit_enter_ptr, properconfig * proper_config,QString alg_name);
    void updata_property_config(QString xml_file, properconfig * proper_config,QString alg_name);

    XMLElement* writer_text_xlm(const QString str_elm_name,const QString str_elm_txt,
                                tinyxml2::XMLDocument &doc ,XMLElement *root_node );
    void add_task_to_doc(QString filename,const SCT_DragTreeWidget & task_obj);

    void Remove_task_Xml(QString filename,const SCT_DragTreeWidget & task_obj);
    void Remove_method_unit_Xml(QString filename,QString unit_id);
    void rewrite_node_unit_pos_index(QString filename, const  QString task_id ,
                                     const  QString unit_name,QString need_ch_name,const  QString need_rewrite_txt);

    void write_alg_config( tinyxml2::XMLDocument &doc, properconfig * proper_config, XMLElement* method_unit_new,QString alg_name);
    void write_camera_config( tinyxml2::XMLDocument &doc, properconfig * proper_config, XMLElement* method_unit_new);
    void open_project_xml(QString filename);
    void  close_current_project();

    void  tianxie_camera(XMLElement* cur_method ,QString unit_name,  QString str_unit_id, QString str_tsk_id, properconfig * proper_config);
    void  tianxie_zhengtishibie(XMLElement* cur_method ,QString unit_name,  QString str_unit_id, QString str_tsk_id, properconfig * proper_config);
    void  tianxie_jubushibie(XMLElement* cur_method ,QString unit_name,  QString str_unit_id, QString str_tsk_id, properconfig * proper_config);


    void tianxie_method_unit(XMLElement* cur_method ,QString unit_name,  QString str_unit_id, QString str_tsk_id, properconfig * proper_config);
    void struct_method_unit_by_xml(XMLElement* cur_method, QString unit_name,  QString str_unit_id, SCT_DragTreeWidget &task_tree , QString str_tsk_id, int i);
    void struct_task_by_xml( QString task_name,  QString str_id, QMap<QString,SCT_DragTreeWidget > & inpu_map_task_all ,QListWidget *inqlw);
    // void updata_xml(QString filename, QString source_name, properconfig * proper_config, const  QString task_id,const  QString unit_id );


public:
    template <typename T1,typename T2>
    void del_map_fcn(QMap<T1,T2> &in_map, T1 key);
    static int bangding_thread( void *);
    void  connect_acord_type(Task_unit_infor front,Task_unit_infor below);

    template< class T1, class T2>
    void struct_connect_cameraToOther( QString src_name ,T1 *pre_ptr , T2 *next_ptr);


    QMap<QString,properconfig * > m_mp_methodID_pro;

    std::map<int , int> m_mp_alg_busy_flag;

    QLabel* label_OK = nullptr;
    QLabel* label_NG = nullptr;

    std::string dir_user ="./";
    int if_chushihua = 0;



    QString m_save_img_path="./save_image";
    map<QString,Form_show_img *> jilu_Form_show_img_pt;
    map<QString,QMdiSubWindow * >jilu_win_pt;
    map<QString,QMdiSubWindow * >jilu_win_cam_pt;
    int m_shifou_shezhi_moren = 0;
    QString m_moren_wenjian_name ="default.xml";



     QHash< QString, jubushibie_form *> m_jiebushibie_ptr;

     int chuli_qidongxiancheng_flag = 0;
     void chuli_jieguo_thread(void * ptr);
     std::thread chuli_jieguo_t1;
     queue<int> m_have_result;




    //运动控制窗口

    // move_ctl mvctrl;

    a_move_car std_mv_ctrl;


signals:
    void  sig_main_ui_send_qidong(int flag);
    void  sig_main_ui_send_tingzhi(int flag);
    void  sig_main_ui_send_jiting(int flag);
    void  sig_main_ui_send_huiyuandian(int flag);

public slots:
    void  one_key_intial_device(int flag);
    void  one_key_start_device(int flag);


    void  slot_get_car_ui_qidong(int flag);
    void  slot_get_car_ui_tingzhi(int flag);
    void  slot_get_car_ui_jiting(int flag);
    void  slot_get_car_ui_huiyuandian(int flag);


private:
    QTreeWidgetItem* current_tree_node_ptr =nullptr;
    int  current_tree_node_index  = -1;
    QString current_tree_win_id = "";
    QString current_select_unit_id = "";
    properconfig  *m_property_tree_xxx =nullptr;
    Dialog_new_project dlg;

    QFile *m_current_proj_ptr_xml;

    QString m_str_project_exe_path = "";
    // QString m_str_project_cfg_set_path = "";
    QString m_str_project_file_path ="";//工程文件路径
    QString m_str_project_file_name ="";//工程文件名称
    QString m_str_current_project_name ="";//当前工程名称

    QMap<QString, int> m_cout_camera_use;
    QMap<QString , jubushibie_form *> m_map_record_ptr_jubushibie;
    int set_alg_use_propery( properconfig * input_procfg, int model_id ,Alg_infor  & alg_infor);
    int set_alg_obj_class_propery(  properconfig * input_procfg , Alg_infor  & alg_infor );

    void Del_Alg_jubu_Unit( QString model_id_str  , QMap< QString , jubushibie_form * > & itemMap)
    {
        QMap< QString , jubushibie_form * >::iterator it;
        for(it = itemMap.begin();it!=itemMap.end();)
        {
            if(it.key() == model_id_str)
            {
                it = itemMap.erase(it);
                continue;
            }
            ++it;
        }
    }
    void delete_all_method_unit( QMap<QTreeWidgetItem *, Task_unit_infor > & in_map_unit_method);

    QMap<int , jubushibie_form *> m_map_jbsb_modelID_prt;
    // int jubushibie_model_id_index = 0;

    QMap<QString ,camera_commen *> m_name_ptr_camera;
    QMap<QString , QGraphicsView  *> m_name_ptr_show;
    QMap<QString , QMdiSubWindow  *> m_show_MdiSubWindow;


    //QMap<int, QString > m_map_camera_factor;


    // QMap<QString,  template <class Unit_type> xx> rem;
    QMap<QString, void*> m_record_unit;

    template <class Unit_type1, class Unit_type2>
    void record_unit_ptr(QString id1,Unit_type1 *factory1, QString id2,Unit_type2 *factory2);

    // template <class Unit_type1>
    // QMap<QString, str_MAP_INFOR<Unit_type1>>  m_qm;

    int shifou_qidong_ceshi = 0;

    Form_jieguo * pt_resul_form = nullptr;

    Fanhuijieguo lingshi_obj;
    void oper_jieguo();

    //尺寸测量相关
    chicunceliang m_cccl_obj;
    //a_chichunceliang m_cccl_obj;
private slots:
    void on_ac_add_task_triggered();
    //void on_ac_open_task_triggered();
    void on_ac_run_task_triggered();
    void on_ac_delete_task_item_triggered();
    void on_ac_delete_jiedian_triggered();
    void on_ac_config_triggered();
    void on_ac_new_project_triggered();

    void on_listWidget_task_itemDoubleClicked(QListWidgetItem *item);

    void on_act_bangding_tsk_fangfa_triggered();

    void on_ac_open_project_triggered();
    void onGetDlg_infor(QString);

    void on_ac_save_all_task_triggered();

    void on_act_stop_task_triggered();
    void on_listWidget_camera_customContextMenuRequested(const QPoint &pos);

    void on_listWidget_camera_itemClicked(QListWidgetItem *item);

    void on_act_test_fnct_triggered();

    void on_pushButton_qingling_clicked();

    void on_pushButton_zhenqingling_clicked();

    void on_act_move_ctrl_triggered();



    void on_act_huiyuandian_triggered();

    void on_act_morenshezhi_triggered();

    void on_act_stop_warn_triggered();

    void on_action_AI_triggered();

    void on_action_uninstall_AI_triggered();


    void on_pushButton_ceshi_clicked();

    void on_action_ceshicccl_triggered();

    void on_action_set_save_img_path_triggered();

    void on_ac_ceshi_triggered();


private:

    int jiami_thread();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
