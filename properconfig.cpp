#pragma execution_character_set("utf-8")


#include <QMainWindow>
#include <QTreeWidget>

#include <QTreeWidgetItem>
#include <QDropEvent>

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

#include "properconfig.h"

#include "haik_cam.h"

extern std::string UTF8_To_string__kk(const std::string& str);
extern std::string string_To_UTF8(const std::string & str);
properconfig::properconfig()
{


}

properconfig::~properconfig()
{
    if(tree_br !=nullptr)
    {
        delete tree_br;
        tree_br = nullptr;
    }
}


properconfig:: properconfig(QString name_win,QString task_id_str ,QString unit_id_str){


    m_map_index_name_camera_factor[MaiDe_CAM]=QObject::tr("迈德微视相机");
    m_map_index_name_camera_factor[FangCheng_CAM]=QObject::tr("方成相机");
    m_map_index_name_camera_factor[HaiKang_CAM]=QObject::tr("海康相机");
    m_map_index_name_camera_factor[DuShen_CAM]=QObject::tr("度申相机");

    m_map_name_index_camera_factor[QObject::tr("迈德微视相机")]=MaiDe_CAM;
    m_map_name_index_camera_factor[QObject::tr("方成相机")]=FangCheng_CAM;
    m_map_name_index_camera_factor[QObject::tr("海康相机")]=HaiKang_CAM;
    m_map_name_index_camera_factor[QObject::tr("度申相机")]=DuShen_CAM;

    m_task_id_str = task_id_str;
    m_unit_id_str = unit_id_str;
    //

    source_name = name_win;
    tree_br = new QtTreePropertyBrowser();
    tree_br->setWindowTitle(source_name);
    m_pVarManager = new QtVariantPropertyManager(tree_br);//关联factory，属性可以修改
    groupItem  = m_pVarManager->addProperty(QtVariantPropertyManager::groupTypeId(),QObject::tr("配置"));//添加组

    enumManager = new QtEnumPropertyManager(tree_br);

    m_pVarFactory = new QtVariantEditorFactory(tree_br);
    comboBoxFactory = new QtEnumEditorFactory(tree_br);
    fileEditFactory = new FileEditFactory(tree_br);

    checkBoxFactory = new QtCheckBoxFactory(tree_br);
    spinBoxFactory = new QtSpinBoxFactory(tree_br);
    sliderFactory = new QtSliderFactory(tree_br);
    scrollBarFactory = new QtScrollBarFactory(tree_br);
    lineEditFactory = new QtLineEditFactory(tree_br);

    fileManager = new FilePathManager(tree_br);

    connect(m_pVarManager,&QtVariantPropertyManager::valueChanged,this, &properconfig::onValueChanged);//绑定信号槽，当值改变的时候会发送信号
    connect(enumManager,&QtEnumPropertyManager::valueChanged,this, &properconfig::onValueChanged);//绑定信号槽，当值改变的时候会发送信号
    connect(fileManager,&FilePathManager::valueChanged,this, &properconfig::onValueChanged);//绑定信号槽，当值改变的时候会发送信号
    tree_br->addProperty(groupItem);

    if(QObject::tr("相机") ==name_win ) camera_proper_conf();
    if(QObject::tr("图片") ==name_win ) camera_proper_conf();
    if(QObject::tr("显示") ==name_win ) camera_proper_conf();

    if(QObject::tr("输出") ==name_win ) output_proper_conf();

    if(QObject::tr("整体识别") ==name_win ) alg_proper_conf();
    if(QObject::tr("局部识别") ==name_win ) alg_proper_conf();
    if(QObject::tr("字符识别") ==name_win ) alg_proper_conf();
    if(QObject::tr("分割") ==name_win ) alg_proper_conf();
    if(QObject::tr("尺寸测量") ==name_win ) alg_proper_conf();

}


void properconfig::onValueChanged(QtProperty *property, const QVariant &value)
{

    emit send_to_main(property, value,source_name ,m_unit_id_str,m_task_id_str,tree_br,this);

}

QtVariantProperty * properconfig::add_number_param(int propertyType, const QString & name)
{
    item=m_pVarManager->addProperty(propertyType, name);
    record_property_unix(item, name, groupItem, m_pVarManager,m_pVarFactory);
    return item;
}



template <class PropertyManager>
void properconfig::record_property_unix(QtProperty * prp, QString name,   QtVariantProperty *groupItem,
                                        PropertyManager *manager,QtAbstractEditorFactory<PropertyManager> *factory)
{
    //propertyToId[prp] =name ;
    // idToProperty[name] =prp;

    if(prp->subProperties().size() > 0)
    {

        QList<QtProperty *> sub_pro_xx =prp->subProperties();
        for(int i = 0; i <sub_pro_xx.size(); i++ )
        {
            QtProperty * sub_unit=sub_pro_xx[i];
            propertyToId[sub_unit] =sub_unit->propertyName() ;

            idToProperty[sub_unit->propertyName()] =sub_unit;

        }
    }

    propertyToId[prp] =prp->propertyName() ;
    idToProperty[prp->propertyName()] =prp;
    groupItem->addSubProperty(prp);
    My_setFactoryForManager(manager, factory);

}
void properconfig::deleteQString( QtProperty * screenGroup,   QMap< QtProperty *,QString > & itemMap)
{
    QMap< QtProperty *,QString >::iterator it;
    for(it = itemMap.begin();it!=itemMap.end();)
    {
        if(it.key() == screenGroup)
        {
            it = itemMap.erase(it);
            continue;
        }
        ++it;
    }
}

void properconfig::deleteQtProperty( QString screenGroup,   QMap< QString, QtProperty * > & itemMap)
{
    QMap< QString,QtProperty * >::iterator it;
    for(it = itemMap.begin();it!=itemMap.end();)
    {
        if(it.key() == screenGroup)
        {
            it = itemMap.erase(it);
            continue;
        }
        ++it;
    }
}

void properconfig::delete_name_class( QString name_class_title,   QMap< QString, QMap<QString, QtProperty *> > & itemMap)
{
    QMap< QString, QMap<QString, QtProperty *>  >::iterator it;
    for(it = itemMap.begin();it!=itemMap.end();)
    {
        if(it.key() == name_class_title)
        {
            it = itemMap.erase(it);
            continue;
        }
        ++it;
    }
}

template <class PropertyManager>
void properconfig::remove_record_property_unix_sub(QtProperty * prp, QString name,   QtVariantProperty *groupItem,
                                                   PropertyManager *manager,QtAbstractEditorFactory<PropertyManager> *factory)
{
    if(prp->subProperties().size() > 0)
    {
        QList<QtProperty *> sub_pro_xx =prp->subProperties();
        for(int i = 0; i <sub_pro_xx.size(); i++ )
        {
            QtProperty * sub_unit=sub_pro_xx[i];

            deleteQString(sub_unit,   propertyToId);
            deleteQtProperty( sub_unit->propertyName(),  idToProperty);
            // propertyToId[sub_unit] =sub_unit->propertyName() ;
            // idToProperty[sub_unit->propertyName()] =sub_unit;

        }
    }

    deleteQString(prp,   propertyToId);
    deleteQtProperty( prp->propertyName(),  idToProperty);

    //  propertyToId[prp] =prp->propertyName() ;

    // idToProperty[prp->propertyName()] =prp;


}

template <class PropertyManager>
void properconfig::record_property_unix_sub(QtProperty * prp, QString name,   QtVariantProperty *groupItem,
                                            PropertyManager *manager,QtAbstractEditorFactory<PropertyManager> *factory)
{
    if(prp->subProperties().size() > 0)
    {
        QList<QtProperty *> sub_pro_xx =prp->subProperties();
        for(int i = 0; i <sub_pro_xx.size(); i++ )
        {
            QtProperty * sub_unit=sub_pro_xx[i];
            propertyToId[sub_unit] =sub_unit->propertyName() ;
            idToProperty[sub_unit->propertyName()] =sub_unit;

        }
    }
    propertyToId[prp] =prp->propertyName() ;

    idToProperty[prp->propertyName()] =prp;
    My_setFactoryForManager(manager, factory);

}

QtVariantProperty * properconfig::add_number_param_sub(int propertyType, const QString & name)
{
    item=m_pVarManager->addProperty(propertyType, name);
    record_property_unix_sub(item, name, groupItem, m_pVarManager,m_pVarFactory);
    return item;
}


QtVariantProperty * properconfig::remove_number_param_sub(const QString & name)
{
    // item=m_pVarManager->addProperty(propertyType, name);
    QtProperty *item  = idToProperty[name];

    if(item!=nullptr)
    {
        remove_record_property_unix_sub(item, name, groupItem, m_pVarManager,m_pVarFactory);
        groupItem->removeSubProperty(item);
        m_pVarManager->propertyRemoved( item,groupItem);

    }
    //  QtProperty *item_peizhi= idToProperty["配置"];
    // record_property_unix_sub(item, name, groupItem, m_pVarManager,m_pVarFactory);
    return nullptr;
}


QtProperty * properconfig::add_enum_param( QStringList enumNames_tr, const QString &name)
{
    QtProperty *item_camera_list = enumManager->addProperty(name);
    enumManager->setEnumNames(item_camera_list, enumNames_tr);
    record_property_unix(item_camera_list, name, groupItem,enumManager, comboBoxFactory);

    return item_camera_list;

}





QtVariantProperty * properconfig::remove_class_name_param(  QtVariantProperty * main_ptr,const QString &name, const QString &name_zh,int index)
{
    QString name_class_title=QObject::tr("类别名称")+"_" +QString::number(index) +QObject::tr("号");

    delete_name_class( name_class_title,  m_classNameToIdToProperty);

    remove_number_param_sub(name_class_title);

    return nullptr;
}

QtVariantProperty * properconfig::add_class_name_param(  QtVariantProperty * main_ptr,const QString &name, const QString &name_zh,int index)
{
    // QtVariantProperty *main_item = m_pVarManager->addProperty(QVariant::String,QObject::tr("类别名称"));

    QMap<QString, QtProperty *> str_pro_temp;
    // m_classNameToIdToProperty;

    QString name_class_title=QObject::tr("类别名称")+"_" +QString::number(index) +QObject::tr("号");
    QtVariantProperty *main_item =add_number_param_sub(QVariant::String, name_class_title);
    main_item->setValue(name);

    str_pro_temp[name_class_title] =main_item;

    QtVariantProperty *item_sub = add_number_param_sub(QVariant::String, QObject::tr("中文名称"));
    item_sub->setValue(name_zh);
    main_item->addSubProperty(item_sub);

    str_pro_temp[QObject::tr("中文名称")] =item_sub;


#if ZJ_CHULIAOKOU_HAO
    item_sub = add_number_param_sub(QVariant::String, QObject::tr("出料口顺序号"));
    item_sub->setValue(2);
    main_item->addSubProperty(item_sub);
    str_pro_temp[QObject::tr("出料口顺序号")] =item_sub;

# endif

    item_sub =add_number_param_sub(QVariant::Bool,QObject::tr("是否启用"));
    item_sub->setValue(true);
    main_item->addSubProperty(item_sub);
    str_pro_temp[QObject::tr("是否启用")] =item_sub;



    item_sub = add_number_param_sub(QVariant::Double,QObject::tr("得分最小值"));
    item_sub->setValue(0.1);
    item_sub->setAttribute(QLatin1String("minimum"), 0.001);
    item_sub->setAttribute(QLatin1String("maximum"),1);
    item_sub->setAttribute(QLatin1String("decimals"), 3);
    item_sub->setAttribute(QLatin1String("singleStep"), 0.01);
    main_item->addSubProperty(item_sub);
    str_pro_temp[QObject::tr("得分最小值")] =item_sub;


    item_sub = add_number_param_sub(QVariant::Double,QObject::tr("得分最大值"));
    item_sub->setValue(1);
    item_sub->setAttribute(QLatin1String("minimum"), 0.001);
    item_sub->setAttribute(QLatin1String("maximum"),1);
    item_sub->setAttribute(QLatin1String("decimals"), 3);
    item_sub->setAttribute(QLatin1String("singleStep"), 0.01);
    main_item->addSubProperty(item_sub);
    str_pro_temp[QObject::tr("得分最大值")] =item_sub;


    item_sub = add_number_param_sub(QVariant::SizeF, QObject::tr("目标框最小值"));
    item_sub->setValue(QSizeF(2, 2));
    item_sub->setAttribute(QLatin1String("minimum"), QSizeF(1, 1));
    item_sub->setAttribute(QLatin1String("maximum"), QSizeF(3000000, 3000000));
    item_sub->setAttribute(QLatin1String("decimals"), 4);
    main_item->addSubProperty(item_sub);
    str_pro_temp[QObject::tr("目标框最小值")] =item_sub;

    item_sub =add_number_param_sub(QVariant::SizeF, QObject::tr("目标框最大值"));
    item_sub->setValue(QSizeF(2000, 2000));
    item_sub->setAttribute(QLatin1String("minimum"), QSizeF(1, 1));
    item_sub->setAttribute(QLatin1String("maximum"), QSizeF(3000000, 3000000));
    item_sub->setAttribute(QLatin1String("decimals"), 4);
    main_item->addSubProperty(item_sub);
    str_pro_temp[QObject::tr("目标框最大值")] =item_sub;


    item_sub =add_number_param_sub(QVariant::Double,QObject::tr("面积最小值"));
    item_sub->setValue(10);
    item_sub->setAttribute(QLatin1String("minimum"), 0.00001);
    item_sub->setAttribute(QLatin1String("maximum"),100000000);
    main_item->addSubProperty(item_sub);
    str_pro_temp[QObject::tr("面积最小值")] =item_sub;

    item_sub = add_number_param_sub(QVariant::Double,QObject::tr("面积最大值"));
    item_sub->setValue(10000);
    item_sub->setAttribute(QLatin1String("minimum"), 0.001);
    item_sub->setAttribute(QLatin1String("maximum"),100000000);
    main_item->addSubProperty(item_sub);

    str_pro_temp[QObject::tr("面积最大值")] =item_sub;


    item_sub = add_number_param_sub(QVariant::Double,QObject::tr("宽当量mm"));
    item_sub->setValue(0.008);
    item_sub->setAttribute(QLatin1String("minimum"), 0.00001);
    item_sub->setAttribute(QLatin1String("maximum"),10000);
    item_sub->setAttribute(QLatin1String("decimals"), 6);
    item_sub->setAttribute(QLatin1String("singleStep"), 0.001);
    main_item->addSubProperty(item_sub);

    str_pro_temp[QObject::tr("宽当量mm")] =item_sub;

    item_sub = add_number_param_sub(QVariant::Double,QObject::tr("高当量mm"));
    item_sub->setValue(0.008);
    item_sub->setAttribute(QLatin1String("minimum"), 0.00001);
    item_sub->setAttribute(QLatin1String("maximum"),10000);
    item_sub->setAttribute(QLatin1String("decimals"), 6);
    item_sub->setAttribute(QLatin1String("singleStep"), 0.001);
    main_item->addSubProperty(item_sub);
    str_pro_temp[QObject::tr("高当量mm")] =item_sub;




    item_sub =add_number_param_sub(QVariant::Bool,QObject::tr("是否乘以当量"));
    item_sub->setValue(false);
    main_item->addSubProperty(item_sub);

    str_pro_temp[QObject::tr("是否乘以当量")] =item_sub;



    item_sub =add_number_param_sub(QVariant::Bool,QObject::tr("是否启用ROI"));
    item_sub->setValue(true);
    main_item->addSubProperty(item_sub);
    str_pro_temp[QObject::tr("是否启用ROI")] =item_sub;


    item_sub =add_number_param_sub(QVariant::RectF, QObject::tr("感兴趣区域ROI"));
    item_sub->setValue(QRectF(0, 0, 2448, 2048));
    item_sub->setAttribute(QLatin1String("constraint"), QRectF(0, 0, 200000, 200000));

    item_sub->setAttribute(QLatin1String("decimals"), 4);
    main_item->addSubProperty(item_sub);

    str_pro_temp[QObject::tr("感兴趣区域ROI")] =item_sub;

    m_classNameToIdToProperty[name_class_title] = str_pro_temp;

    return main_item;


}

//新建的时候调用
int properconfig::alg_proper_conf()
{

    QStringList enumNames_tr;
    enumNames_tr.append(QObject::tr("整体识别"));
    enumNames_tr.append(QObject::tr("字符识别"));
    enumNames_tr.append(QObject::tr("分割"));
    enumNames_tr.append(QObject::tr("局部识别"));
    enumNames_tr.append(QObject::tr("尺寸测量"));
    QtProperty *item_camera_list =add_enum_param( enumNames_tr, QObject::tr("算法类型"));

    QStringList enumNames_sub_alg;
    enumNames_sub_alg.append(QObject::tr("pytorch"));
    enumNames_sub_alg.append(QObject::tr("c++"));
    enumNames_sub_alg.append(QObject::tr("cuda"));
    enumNames_sub_alg.append(QObject::tr("D"));



    QtProperty *item_camera_list_sub =add_enum_param( enumNames_sub_alg, QObject::tr("子算法"));// enumManager->addProperty(QObject::tr("子算法"));
    m_model_file_path= "predefined_classes.txt";
    QtProperty *item20 = fileManager->addProperty(QObject::tr("算法模型文件"));
    fileManager->setValue(item20, m_model_file_path/*"predefined_classes.txt"*/);
    fileManager->setFilter(item20,"Source files (*.xml *.ini *.txt)");
    record_property_unix(item20, QObject::tr("算法模型文件"), groupItem, fileManager,fileEditFactory);




    item2_model_file = item20;

    string fileName =  fileManager->value(item20).toStdString();
    char drive[_MAX_DRIVE] = { 0 };
    char dir[_MAX_DIR] = { 0 };
    char fname[_MAX_FNAME] = { 0 };
    char ext[_MAX_EXT] = { 0 };
    _splitpath(fileName.c_str(), drive, dir, fname, ext);
    //string file_format = string(ext);

    string pathFile_root = string(drive) + string(dir);
    model_pathFile_root =QString::fromStdString(pathFile_root);

    alg_be_call_num[m_unit_id_str] = 0;

    string classesname =pathFile_root+ string(fname) + string(ext);
    ifstream inFileclasses;
    inFileclasses.open(classesname, ios::in);
    if (!inFileclasses.is_open())//判断文件是否打开
    {
        std::cout << "Error opening inFileclasses" << std::endl;
        classnamestr.push_back("null");
    }else
    {
        char line[1024] = { 0 };


        if (classnamestr.size() > 0)
            classnamestr.clear();


        while (inFileclasses.getline(line, sizeof(line)))
        {
            std::stringstream word(line);
            string tempstr;
            word >> tempstr;
            classnamestr.push_back(tempstr);
        }

        inFileclasses.close();
    }


    QStringList enumNames_gpuID;
    enumNames_gpuID.append(QObject::tr("GPU0"));
    enumNames_gpuID.append(QObject::tr("GPU1"));
    enumNames_gpuID.append(QObject::tr("GPU2"));
    enumNames_gpuID.append(QObject::tr("GPU3"));
    enumNames_gpuID.append(QObject::tr("GPU4"));
    enumNames_gpuID.append(QObject::tr("GPU5"));
    enumNames_gpuID.append(QObject::tr("GPU6"));
    enumNames_gpuID.append(QObject::tr("GPU7"));
    enumNames_gpuID.append(QObject::tr("GPU8"));


    item_camera_list_sub =add_enum_param( enumNames_gpuID, QObject::tr("选择显卡"));// enumManager->addProperty(QObject::tr("子算法"));


#if USING_PAIJICI_FABU

    QtVariantProperty * item_sub = add_number_param(QVariant::Int,QObject::tr("拍照几次才计算"));
    item_sub->setValue(1);
    item_sub->setAttribute(QLatin1String("minimum"), 0);
    item_sub->setAttribute(QLatin1String("maximum"),10000);

    item_sub->setAttribute(QLatin1String("singleStep"), 1);

#endif

    QtVariantProperty *x_item = add_number_param(QVariant::String, QObject::tr("目标"));
    m_mubiaoi_x_item = x_item;

    x_item->setValue(QString::number(classnamestr.size()));
    change_classes();


    return 0;
}

QtVariantProperty * properconfig::creater_unit(QString Name)
{
    QtVariantProperty * x_item = add_number_param(QVariant::String, Name);

    //有个悖论
    //x_item->setValue("nihao");
    m_mubiaoi_x_item = x_item;



    return x_item;
}
void properconfig::change_classes()
{

    if(classnamestr.size()>0)
    {
        for(int i = 0; i < classnamestr.size(); i++)
        {
            QString class_name = QString::fromStdString(classnamestr[i]);
            QtVariantProperty *sub_item=  add_class_name_param(m_mubiaoi_x_item, class_name, class_name,i);
            m_mubiaoi_x_item->addSubProperty(sub_item);
        }
    }

}

int properconfig::save_img_proper_conf()
{
    QString filename=QString("images");
    QtVariantProperty *x_item = add_number_param(QVariant::String, QObject::tr("文件夹名称"));
    x_item->setValue(filename);


    x_item = add_number_param(QVariant::String, QObject::tr("路径"));

    filename =QString("D:/")+filename;
    x_item->setValue(filename);
    return 0;
}


int properconfig::list_camera()
{

#if USING_MAIDEWEISHI

    hk_objxx.list_camera(m_input_str);
#else

    haik_cam hk_obj;
    hk_obj.list_camera(m_input_str);
#endif

    return  m_input_str.size();
}
int properconfig::camera_proper_conf()
{

    //是否增加触发方式 网络触发  串口触发

    QStringList enumCamera_factor;
    enumCamera_factor.append(m_map_index_name_camera_factor[MaiDe_CAM]);
    enumCamera_factor.append(m_map_index_name_camera_factor[FangCheng_CAM]);
    enumCamera_factor.append( m_map_index_name_camera_factor[HaiKang_CAM]);
    enumCamera_factor.append( m_map_index_name_camera_factor[DuShen_CAM]);

    QtProperty * cam_factor_list= add_enum_param( enumCamera_factor, QObject::tr("相机厂商列表"));

    list_camera();
    QStringList enumNames;
    for(int i = 0; i < m_input_str.size(); i++)
    {
        QString instr = QString::fromStdString(m_input_str[i]);
        enumNames << instr;
    }
    QtProperty * cam_list= add_enum_param( enumNames, QObject::tr("相机列表"));
    // QVariant v = QVariant::fromValue((QtProperty *) cam_list);


    QStringList enumNames_tr;
    enumNames_tr.append(QObject::tr("连续采集"));
    enumNames_tr.append(QObject::tr("软触发"));
    enumNames_tr.append(QObject::tr("硬触发"));

    add_enum_param( enumNames_tr, QObject::tr("相机工作模式"));

    QtVariantProperty *x_item = add_number_param(QVariant::Double, QObject::tr("曝光时间"));

    x_item->setAttribute(QLatin1String("minimum"), 0);
    x_item->setAttribute(QLatin1String("maximum"), 100000000);
    x_item->setAttribute(QLatin1String("decimals"), 4);
    x_item->setValue(10.0);

    x_item = add_number_param(QVariant::Double, QObject::tr("增益"));


    x_item->setAttribute(QLatin1String("minimum"), 0);
    x_item->setAttribute(QLatin1String("maximum"), 1000);
    x_item->setAttribute(QLatin1String("decimals"), 3);

    x_item->setValue(1.0);

    return 0;
}

int properconfig::output_proper_conf()
{

    QtVariantProperty *  item_sub =add_number_param(QVariant::Bool,QObject::tr("是否输出触发信号"));
    item_sub->setValue(false);


    QStringList enumNames_tr;
    enumNames_tr.append(QObject::tr("相机IO口输出电平"));
    enumNames_tr.append(QObject::tr("TCP网络输出"));
    enumNames_tr.append(QObject::tr("串口输出"));

    add_enum_param( enumNames_tr, QObject::tr("检查结果信号输出方式"));


    item_sub =add_number_param(QVariant::Bool,QObject::tr("统计结果"));
    item_sub->setValue(false);




    return 0;

}
int properconfig::show_proper_conf()
{

    QtVariantProperty *item_sub = add_number_param_sub(QVariant::String, QObject::tr("中文名称"));
    item_sub->setValue(this->m_task_id_str);

    return 0;
}

