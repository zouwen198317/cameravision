#ifndef PROPERCONFIG_H
#define PROPERCONFIG_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include<QtCheckBoxFactory>

#include <QScrollArea>
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include <qtpropertybrowser.h>
#include <qtvariantproperty.h>
#include "qttreepropertybrowser.h"
#include "qtbuttonpropertybrowser.h"
#include "qtgroupboxpropertybrowser.h"


#include <QGridLayout>
#include<iostream>
#include<string>

#include "filepathmanager.h"
#include "fileeditfactory.h"

#include "opencv/highgui.h"
#include <fstream>

#include "camera_commen.h"
//using namespace  cv;
using namespace  std;




//相机单元配置
typedef struct strc_camera_config
{
    string camera_name ="";
    string changjia_id;
    int  task_id = 0;
    string task_name ="";
    double  export_val =1;
    double gain_val = 1;
    int  flip =0;
    vector<cv::Rect> vc_roi;
}Camera_config;

//显示窗口配置
typedef struct strc_show_win_config
{
    string win_name ="";
    int  task_id = 0;
    string task_name ="";
    int resize_flag = 0;

}Show_win_config;

//保存图片配置
typedef struct strc_save_img_config
{
    string path ="";
    string file_name ="";
    int  task_id = 0;
    string task_name ="";

}Save_img_config;


typedef struct strc_alg_guolv_config
{
    int enable_flag = 1;
    string name_obj ="";
    int id_obj = 0;
    double prob = 0;
    double area = 0;
    float w= 0;
    float h= 0;
    float x= 0;
    float y= 0;
}Alg_class_config;

typedef struct strc_alg_config
{
    string alg_name ="";
    string alg_zhongwen_name ="";
    int alg_type = 0;
    string alg_config_path ="";
    int  task_id = 0;
    string task_name ="";
    int enable_flag =0;
    vector<Alg_class_config> class_val;

}Alg_config;

typedef struct strc_task_result_config
{
    string alg_name ="";
    int alg_type = 0;
    string alg_config_path ="";
    int  task_id = 0;
    string task_name ="";
    int enable_flag =0;

}Task_result_config;

class properconfig : public QtTreePropertyBrowser
{
    Q_OBJECT
public:
    properconfig();
    ~properconfig();
    properconfig(QString name_win,QString task_id_str , QString unit_id_str);
    // int create_proper_cf(QString sorce_str,int index);
public:
    // FCT_Ref m_FCT_Ref;
    static int camera_proper_confK( void *);

    int camera_proper_conf();
    int show_proper_conf();
    int output_proper_conf();
    int alg_proper_conf();
    int save_img_proper_conf();
    int list_camera();
    // void change_classes(vector<string> class_str);
    void change_classes();
    QtVariantProperty *m_mubiaoi_x_item =nullptr;
    QString m_model_file_path = "classes_name_file.txt";

    //  void bang_string_funct_map(QStringList name, )
signals:
    void send_to_main(QtProperty *property, const QVariant &value, QString source_name ,
                      QString unit_id_str,QString task_id_str,QtTreePropertyBrowser * tree_br,properconfig * prt);




public  slots:
    void onValueChanged(QtProperty *property, const QVariant &value);

private:

public:

    QtVariantProperty * add_number_param(int propertyType, const QString &name = QString());
    QtProperty * add_enum_param( QStringList enumNames_tr , const QString &name = QString());

    QtVariantProperty * add_number_param_sub(int propertyType, const QString & name);
    QtVariantProperty * remove_number_param_sub( const QString & name);

    template <class PropertyManager>
    void record_property_unix(QtProperty * prp, QString name,   QtVariantProperty *groupItem,
                              PropertyManager *manager,QtAbstractEditorFactory<PropertyManager> *factory);


    template <class PropertyManager>
    void record_property_unix_sub(QtProperty * prp, QString name,   QtVariantProperty *groupItem,
                                  PropertyManager *manager,QtAbstractEditorFactory<PropertyManager> *factory);


    template <class PropertyManager>
    void remove_record_property_unix_sub(QtProperty * prp, QString name,   QtVariantProperty *groupItem,
                                         PropertyManager *manager,QtAbstractEditorFactory<PropertyManager> *factory);


    template <class PropertyManager>
    void My_setFactoryForManager(PropertyManager *manager,
                                 QtAbstractEditorFactory<PropertyManager> *factory) {

        tree_br->setFactoryForManager(manager,factory);

    }


    QtVariantProperty *  creater_unit(QString Name);
    QtVariantProperty * add_class_name_param(   QtVariantProperty * main_ptr,const QString &name = QString(), const QString &name_zh= QString(),int index =0);

    QtVariantProperty * remove_class_name_param(   QtVariantProperty * main_ptr,const QString &name = QString(), const QString &name_zh= QString(),int index =0);


    void deleteQString( QtProperty * screenGroup,   QMap< QtProperty *,QString > & itemMap);
    void deleteQtProperty( QString screenGroup,   QMap< QString, QtProperty * > & itemMap);
    void delete_name_class( QString name_class_title,   QMap< QString, QMap<QString, QtProperty *> > & itemMap);



    //    template <typename T>
    //    void m_f_proper(T input)
    //    {

    //    }
public:
    Camera_config m_cma_prop_cf;
    Show_win_config  m_show_win_prop_cf;
    Save_img_config m_save_img_prop_cfg;
    Alg_config m_alg_prop_cfg;
    Task_result_config m_task_re_prop_cf;
public:
    QString source_name ="";
    QString m_task_id_str="";
    QString m_unit_id_str="";
    QtTreePropertyBrowser * tree_br =nullptr;
    vector<string> m_input_str;
    std::vector<std::string>  classnamestr;

    QString model_pathFile_root ="";

    QMap<QString, int> alg_be_call_num;
    QtVariantProperty *groupItem =nullptr;
    // 变量
    QtVariantPropertyManager *m_pVarManager =nullptr;
    QtEnumPropertyManager *enumManager = nullptr;
    QtVariantProperty *item  = nullptr;

    FilePathManager * fileManager =nullptr;


    //变量工厂
    QtVariantEditorFactory *m_pVarFactory =nullptr;
    QtCheckBoxFactory *checkBoxFactory =nullptr;
    QtSpinBoxFactory *spinBoxFactory =nullptr;
    QtSliderFactory *sliderFactory =nullptr;
    QtScrollBarFactory *scrollBarFactory =nullptr;
    QtLineEditFactory *lineEditFactory =nullptr;
    QtEnumEditorFactory *comboBoxFactory=nullptr;
    FileEditFactory *fileEditFactory = nullptr;

    QMap<QtProperty *, QString> propertyToId;
    QMap<QString, QtProperty *> idToProperty;

    QMap<QString,int(*)(void *)> map_fc;

    QMap<int, QString > m_map_index_name_camera_factor;

    QMap<QString, int  > m_map_name_index_camera_factor;

     QtProperty *item2_model_file = nullptr;






    QMap<QString,  QMap<QString, QtProperty *>> m_classNameToIdToProperty;


};

//Q_DECLARE_METATYPE(properconfig)
#endif // PROPERCONFIG_H
