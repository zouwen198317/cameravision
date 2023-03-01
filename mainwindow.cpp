#pragma execution_character_set("utf-8")
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QMessageBox>
#include<QDebug>

#include <QDateTime>
#include <QString>
#include <QTime>
#include <iostream>
#include <thread>
#include <QSettings>
#include <QCoreApplication>
#include <QDir>
#include<direct.h>

#include <QMetaType>


extern QMap<QString,DangLiang> xj_dangliang;

using namespace  std;
using namespace  cv;

Q_DECLARE_METATYPE(Fanhuijieguo)


//extern std::string UTF8_To_string__kk(const std::string& str);
//extern std::string string_To_UTF8(const std::string & str);

std::string string_To_UTF8(const std::string & str)
{
    int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

    wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴
    ZeroMemory(pwBuf, nwLen * 2 + 2);

    ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

    int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

    char * pBuf = new char[nLen + 1];
    ZeroMemory(pBuf, nLen + 1);

    ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

    std::string retStr(pBuf);

    delete []pwBuf;
    delete []pBuf;

    pwBuf = NULL;
    pBuf = NULL;

    return retStr;
}

std::string UTF8_To_string__kk(const std::string& str)
{
    int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

    wchar_t* pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴
    memset(pwBuf, 0, nwLen * 2 + 2);

    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);

    int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

    char* pBuf = new char[nLen + 1];
    memset(pBuf, 0, nLen + 1);

    WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

    std::string retStr = pBuf;

    delete[]pBuf;
    delete[]pwBuf;

    pBuf = NULL;
    pwBuf = NULL;

    return retStr;
}

extern "C"   int jianchashifou_youjiamigou();
//线程
int MainWindow::jiami_thread()
{
    printf("ThreadID_1: %d\n", GetCurrentThreadId());
    while ( !jiankong_flag)
    {
        if (-1 == jianchashifou_youjiamigou())
        {
            int x;
            x = MessageBox(GetForegroundWindow(), L"警告", L"请插入加密狗!", 1);
            printf("%d\n", x);


            //QMessageBox mesg;
            //mesg.about( NULL,"警告", "请插入加密狗!");

            //QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
        //printf("running \n");
        Sleep(10000);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    return 0;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

#if 1
    printf("ThreadID_0: %d\n", GetCurrentThreadId());
    if (-1 == jianchashifou_youjiamigou())
    {
        QMessageBox mesg;
        mesg.warning(this, "警告", "请插入加密狗!");
        exit(-1);
    }

    std::thread jiami_thread(&MainWindow::jiami_thread, this);
    jiami_thread.detach();

#endif


    chuli_qidongxiancheng_flag = 1;

    chuli_jieguo_t1 = std::thread(&MainWindow::chuli_jieguo_thread,this ,this);
    chuli_jieguo_t1.detach();





    int size_icon = size_icon_m;
    ui->alg_listWidget->setIconSize(QSize(size_icon, size_icon));
    ui->source_listWidget->setIconSize(QSize(size_icon, size_icon));
    ui->alg_listWidget->setAcceptDrops(true);
    ui->source_listWidget->setDragEnabled(true);
    ui->alg_listWidget->setDragEnabled(true);
    renwu_index =0;

    int source_index = 0;

    pt_resul_form = new  Form_jieguo(this);

    ui->toolBar->addWidget(pt_resul_form);

    //int source_cout = ui->source_listWidget->count();
    for(int i  =0; i < ui->source_listWidget->count(); i++)
    {
        QString itemPath = ui->source_listWidget->item(i)->text();

        m_source.push_back(itemPath);

        m_all_unit.push_back(itemPath);
        str2int_src_index_map[itemPath] =source_index;
        int2str_src_index_map[source_index] =itemPath;
        str2QListWidgetItem_map[itemPath] = ui->source_listWidget->item(i);
        QListWidgetItem2str_map[ui->source_listWidget->item(i)] = itemPath;

        str2QIcon_map[itemPath] = ui->source_listWidget->item(i)->icon();
        source_index++;
    }

    for(int i  =0; i < ui->alg_listWidget->count(); i++)
    {
        QString itemPath = ui->alg_listWidget->item(i)->text();
        m_alg.push_back(itemPath);
        m_all_unit.push_back(itemPath);

        str2int_src_index_map[itemPath] =source_index;
        int2str_src_index_map[source_index] =itemPath;

        str2QListWidgetItem_map[itemPath] = ui->alg_listWidget->item(i);
        QListWidgetItem2str_map[ui->alg_listWidget->item(i)] = itemPath;

        str2QIcon_map[itemPath] = ui->alg_listWidget->item(i)->icon();

        source_index++;
    }

    connect(this,SIGNAL(sig_bang()),this, SLOT(slot_bang()));
    connect(&dlg,&Dialog_new_project::send_project_infor,this, &MainWindow::onGetDlg_infor);

    //根据相机名称来初始化调用，一个相机只能打开一次
    vector<string> inputstr;
#if USING_MAIDEWEISHI

    hk_objxx.list_camera(inputstr);
#else

    haik_cam hk_obj;
    hk_obj.list_camera(inputstr);
#endif

    for(int i = 0; i < inputstr.size();i++)
    {
        m_cout_camera_use[QString::fromStdString(inputstr[i])] = 0;

        ui->listWidget_camera->addItem ( QString::fromStdString(inputstr[i]) );

        xj_dangliang[QString::fromStdString(inputstr[i])].w = 1;
        xj_dangliang[QString::fromStdString(inputstr[i])].h = 1;

    }

    ui->listWidget_camera->setContextMenuPolicy(Qt::CustomContextMenu);

    //QMap<QString ,int > m_source;
    //QMap<QString ,int > m_alg;

    for(int i =0; i<80; i++)
    {
        m_mp_alg_busy_flag[i] = 0;
    }

    //    label_OK = new QLabel(this);
    //    label_NG = new QLabel(this);

    //    QPixmap myPix_ng(":/images/nglog.png");
    //    QPixmap myPix_ok(":/images/oklog.png");
    //    QSize labelSize_ng = ui->label->size();
    //    QPixmap ng_newBMP = myPix_ng.scaled(labelSize_ng);
    //    label_NG->setPixmap(ng_newBMP);




    //    ui->toolBar->addWidget(label_OK);
    //    ui->toolBar->addWidget(label_NG);
    qRegisterMetaType<Fanhuijieguo>("Fanhuijieguo");

    QObject::connect(  this ,SIGNAL( set_main_Fanhuijieguo(Fanhuijieguo )), this ,SLOT(set_result_data(Fanhuijieguo  )));

    // mvctrl.on_ps_open_car_clicked();
    // mvctrl.on_pb_shineng_kai_clicked();



    Sleep(200);
    //开机初始化
    //开机初始化
    //#if ZHENGSHI_YUNXING_ING
    //    one_key_intial_device(0);//开机一定要打开
    //#endif




    //#if ZHENGSHI_YUNXING_ING

    //#endif
    //    std_mv_ctrl.on_ps_open_car_clicked(); //开机要打开



    QString path =QCoreApplication::applicationDirPath();
    QString xdir_user =path+ "/user";
    QString path_set =xdir_user+"/setting.ini";
    QSettings* configIni = new QSettings(path_set, QSettings::IniFormat);

    m_save_img_path = configIni->value("setting/path").toString()+"/";
    m_shifou_shezhi_moren = configIni->value("setting/m_shifou_shezhi_moren").toString().toInt();

    // std_mv_ctrl.tuxiang_baocun_lj = m_save_img_path;

    delete configIni;

    if(m_shifou_shezhi_moren ==1)
    {

        //开机初始化
#if ZHENGSHI_YUNXING_ING
        one_key_intial_device(0);//开机一定要打开
#endif


#if ZHENGSHI_YUNXING_ING
        std_mv_ctrl.on_ps_open_car_clicked(); //开机要打开

#endif


    }

    // QObject::connect(  &mvctrl ,SIGNAL(sig_car_ui_send_qidong(int)), this ,SLOT( slot_get_car_ui_qidong(int)));
    // QObject::connect(  &mvctrl ,SIGNAL(sig_car_ui_send_tingzhi(int)), this ,SLOT( slot_get_car_ui_tingzhi(int)));
    // QObject::connect(  &mvctrl ,SIGNAL(sig_car_ui_send_jiting(int)), this ,SLOT( slot_get_car_ui_jiting(int)));
    // QObject::connect(  &mvctrl ,SIGNAL(sig_car_ui_send_huiyuandian(int)), this ,SLOT( slot_get_car_ui_huiyuandian(int)));


}

//接收硬件启动信号
void  MainWindow::slot_get_car_ui_qidong(int flag)
{
    if(shifou_qidong_ceshi==0)
    {
        //软件启动
        cout<<"shoudao xinhao--->"<<endl;
        on_ac_run_task_triggered();

        shifou_qidong_ceshi = 1;
    }else
    {
        std_mv_ctrl.on_pb_start_zhuandong_clicked();
    }
    // Sleep(100);
}

//接收停止信号,软件停止 要重新初始化
void  MainWindow::slot_get_car_ui_tingzhi(int flag)
{


    //on_act_stop_task_triggered();
}
void  MainWindow::slot_get_car_ui_jiting(int flag)
{
}
void  MainWindow::slot_get_car_ui_huiyuandian(int flag)
{

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_listWidget_camera_itemClicked(QListWidgetItem *item)
{

    QString name_camera = item->text();

    camera_commen*   vvv =m_name_ptr_camera[name_camera];
    if(vvv!=nullptr)
    {
        vvv->show_setting_menu();
    }

}



void MainWindow::on_listWidget_camera_customContextMenuRequested(const QPoint &pos)
{

    //ui->listWidget_camera->i
    QListWidgetItem *item =ui->listWidget_camera->itemAt(pos);
    if(item == nullptr)
        return;
    QMenu *menu = new QMenu(this);
    QAction *act = new QAction("菜单项目", this);
    menu->addAction(act);

    connect(act, SIGNAL(triggered()), this, SLOT(my_slot()));
    //    //menu->exec(Cursor::pos()); //在鼠标位置弹出菜单

    delete menu;
    delete act;

}

void MainWindow::on_listWidget_task_itemDoubleClicked(QListWidgetItem *item)
{

    int selet_count = ui->listWidget_task-> currentRow ();
    cout<<selet_count<<endl;

    QMap<QString,SCT_DragTreeWidget>::iterator m_k =  m_current_project.map_task_all.begin() + selet_count;
    ui->mdiArea->setActiveSubWindow( m_k.value().win_haldle);

    QString first = m_k.key();
    //show_sub_win->move(30, 30);
    int coutss = m_current_project.map_task_all.count(first);
    if(coutss > 0) jilu_win_pt[first]->move(50, 50);


}


void MainWindow::onPropertyValueChanged(QtProperty *property, const QVariant &val )
{

}

template <typename Key,typename Val>
void MainWindow::del_map_fcn(QMap<Key,Val> &in_map, Key key_v)
{

    QMapIterator<Key , Val> i(in_map);
    while(i.hasNext()) {
        auto item =  i.next();
        qDebug() << item.key();
        // qDebug() << item.value();
        if (item.key() == key_v) {
            in_map.remove(item.key());
            //in_map.erase(item);
            // in_map.erase(in_map[key_v]);
            // delete in_map.value();
            // in_map.erase(item);
        }
    }
}


//当前选择任务中方法节点
void MainWindow::setTreaNode(QTreeWidgetItem* ptr_tree_node ,QModelIndex index,QString in_win_id)
{
    current_tree_node_ptr =ptr_tree_node;
    QModelIndex indextem =index;
    current_tree_node_index  = indextem.row();
    //  cout<<current_tree_node_index<<"  " <<" "<<index.column() <<UTF8_To_string__kk (ptr_tree_node->text(0).toStdString())<<endl;

    current_tree_win_id = in_win_id;
    SCT_DragTreeWidget temp =  m_current_project.map_task_all[in_win_id];
    Task_unit_infor tsk_unit = temp.map_unit_method[ptr_tree_node];
    current_select_unit_id = tsk_unit.str_method_in_tsk_id;
    cout<<tsk_unit.task_pos_index <<" "<<UTF8_To_string__kk (tsk_unit.src_name.toStdString())<<" "<<current_select_unit_id.toStdString()<<endl;

}





void MainWindow::on_ac_add_task_triggered()
{

    QMessageBox::StandardButton rb = QMessageBox::question(this,  "新建任务",  "是否新建任务", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if(rb == QMessageBox::Yes)
    {

        QDateTime current_date_time = QDateTime::currentDateTime();
        QString current_date_first = current_date_time.toString("yyyyMMdd");//yyyy-MM-dd
        QString current_date_second = current_date_time.toString("hhmmsszzz");//yyyy-MM-dd
        QString current_date_str =current_date_time.toString("yyyyMMddhhmmsszzz");//yyyy-MM-dd

        int task_newId_fist=current_date_first.toInt();
        int task_newId_second=current_date_second.toInt();

        //任务的唯一性
        renwu_index++;
        QString name_task_win =tr("处理任务:")+ QString::number(renwu_index);
        DragTreeWidget *pWidgetls = new DragTreeWidget();

        pWidgetls->id_first =task_newId_fist;
        pWidgetls->id_second =task_newId_second;
        pWidgetls->id_str = current_date_str;

        pWidgetls->setWindowTitle(name_task_win);
        pWidgetls->setIconSize(QSize(size_icon_m, size_icon_m));
        QIcon myIcon=QIcon(":/images/renwu1.jpeg");
        pWidgetls->setWindowIcon(myIcon);
        pWidgetls->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
        QMdiSubWindow *h_temp= ui->mdiArea->addSubWindow(pWidgetls);
        h_temp->setWindowFlags(Qt::CustomizeWindowHint/*Window*/ | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);

        pWidgetls->win_id = renwu_index;
        pWidgetls->setAcceptDrops(true);
        //pWidgetls->setDragEnabled(true);

        pWidgetls->show();
        connect(pWidgetls,&DragTreeWidget:: send_infor_changed_RightButton,this, &MainWindow::on_changed_from_tree_RightButton);//绑定信号槽，当值改变的时候会发送信号
        connect( pWidgetls,SIGNAL(itemClicked_QModelIndex(QTreeWidgetItem*,QModelIndex,QString)), this,SLOT(setTreaNode(QTreeWidgetItem*,QModelIndex,QString )));
        connect( pWidgetls,SIGNAL(send_msg_add_event(QListWidgetItem*,QString)), this,SLOT(slot_drop_new_method(QListWidgetItem*,QString)));

        //ptr_task_win_list.push_back(pWidgetls);

        //构建任务信息
        SCT_DragTreeWidget temp;
        temp.ptr_DragTreeWidget  = pWidgetls;
        temp.tile_text = name_task_win;

        // temp.task_first_id =task_newId_fist;
        //  temp.task_second_id =task_newId_second;
        temp.task_id_str =  pWidgetls->id_str;
        temp.win_haldle = h_temp;


        m_current_project.map_task_all[ pWidgetls->id_str] =temp;

        add_task_to_doc(  m_str_project_file_path,temp);

        //m_current_project.ptr_task_all.push_back(temp);

        //这里可能有疑问
        ui->listWidget_task->insertItem(renwu_index,name_task_win);

    }
}

// 资源释放方面,还需要改善,
void MainWindow::delete_all_method_unit( QMap<QTreeWidgetItem *, Task_unit_infor > & in_map_unit_method)
{

    QMap<QTreeWidgetItem *, Task_unit_infor > ::Iterator sub_medth_unit_it =in_map_unit_method.begin();


    while(sub_medth_unit_it!=in_map_unit_method.end())
    {
        disconnect(sub_medth_unit_it.value().proper_config,
                   SIGNAL(send_to_main(QtProperty *, const QVariant &, QString  , QString ,QString ,QtTreePropertyBrowser * ,properconfig *)), nullptr, nullptr);

        QString unit_id_str=sub_medth_unit_it.value().str_method_in_tsk_id;
        if(sub_medth_unit_it.value().src_name== QObject::tr("局部识别")) //m_map_record_ptr_jubushibie
        {

            if (m_map_record_ptr_jubushibie.count(unit_id_str) > 0)
            {
                if(m_map_record_ptr_jubushibie[unit_id_str]!=nullptr)
                {
                    jubushibie_form * temp_ptr = (jubushibie_form * ) m_map_record_ptr_jubushibie[unit_id_str];
                    if(temp_ptr!=nullptr)
                    {
                        m_mp_alg_busy_flag[temp_ptr->m_alg_infor.m_model_id_int] = 0;
                        //disconnect
                        QObject::disconnect(temp_ptr,SIGNAL( send_result_next(cv::Mat,   Alg_infor alg_infor,std::vector<tr_s_resultinfo>)),nullptr, nullptr);
                        QObject::disconnect(temp_ptr,SIGNAL( send_to_tongji_unit( QString   ,int  ,  Map_resultinfo &  , std::vector<tr_s_resultinfo>  )),nullptr, nullptr);
                        QObject::disconnect(temp_ptr,SIGNAL(send_to_tongji_unit_queue(queue<for_queue_resultinfo> &  , std::vector<tr_s_resultinfo>  ) ),nullptr, nullptr);

                        delete temp_ptr;
                        m_map_record_ptr_jubushibie[unit_id_str] =nullptr;
                        Del_Alg_jubu_Unit( unit_id_str , m_map_record_ptr_jubushibie);
                    }


                }
            }

        }
        if(sub_medth_unit_it.value().src_name== QObject::tr("整体识别")) {

        }
        if(sub_medth_unit_it.value().src_name== QObject::tr("字符识别")) {

        }
        if(sub_medth_unit_it.value().src_name== QObject::tr("分割")) {

        }


        delete sub_medth_unit_it.value().proper_config;
        sub_medth_unit_it.value().proper_config = nullptr;
        in_map_unit_method.erase(sub_medth_unit_it++);
        // ++sub_medth_unit_it++;
    }

}
//删除任务 某个
void MainWindow::on_ac_delete_task_item_triggered()
{
    if(ui->listWidget_task->count() >0)
    {
        int selet_count = ui->listWidget_task-> currentRow ();
        if(selet_count >=0)
        {
            ui->listWidget_task->takeItem(selet_count);

            QMap<QString,SCT_DragTreeWidget>::iterator m_k =  m_current_project.map_task_all.begin() + selet_count;

            ui->mdiArea->setActiveSubWindow( m_k.value().win_haldle);
            ui->mdiArea->removeSubWindow( m_k.value().ptr_DragTreeWidget);
            ui->mdiArea->closeActiveSubWindow();

            delete   m_k.value().ptr_DragTreeWidget;
            m_k.value().ptr_DragTreeWidget = nullptr;

            cout<<"delete task:"<< m_k.key().toStdString()<<endl;

            Remove_task_Xml(m_str_project_file_path,m_k.value());

            // QMap<QTreeWidgetItem *, Task_unit_infor > ::Iterator sub_medth_unit_it =m_k.value().map_unit_method.begin();
            delete_all_method_unit( m_k.value().map_unit_method);
            //删除这个任务中的所有方法单元
            //            while(sub_medth_unit_it!=m_k.value().map_unit_method.end())
            //            {
            //                disconnect(sub_medth_unit_it.value().proper_config,
            //                           SIGNAL(send_to_main(QtProperty *, const QVariant &, QString  , QString ,QString ,QtTreePropertyBrowser * ,properconfig *)), nullptr, nullptr);

            //                delete sub_medth_unit_it.value().proper_config;
            //                sub_medth_unit_it.value().proper_config = nullptr;
            //                m_k.value().map_unit_method.erase(sub_medth_unit_it++);
            //                // ++sub_medth_unit_it++;
            //            }


            m_current_project.map_task_all.erase(m_k);

            renwu_index--;
        }
    }
}



//关闭工程和及其下面的任务，是关闭不是删除
void  MainWindow::close_current_project()
{
    if(m_current_project.map_task_all.size() >0)
    {
        QMap<QString ,SCT_DragTreeWidget>::Iterator it=m_current_project.map_task_all.begin();
        while(it!=m_current_project.map_task_all.end())
        {
            QString tsk_win_str_id = it.key();

            ui->mdiArea->setActiveSubWindow( it.value().win_haldle);
            ui->mdiArea->removeSubWindow( it.value().ptr_DragTreeWidget);
            ui->mdiArea->closeActiveSubWindow();

            delete   it.value().ptr_DragTreeWidget;
            it.value().ptr_DragTreeWidget = nullptr;


            //释放资源,包括算法获取的显卡资源
            delete_all_method_unit( it.value().map_unit_method);

            //            while(sub_medth_unit_it!=it.value().map_unit_method.end())
            //            {


            //                //有没有资源要分配
            //                disconnect(sub_medth_unit_it.value().proper_config, SIGNAL(send_to_main(QtProperty *, const QVariant &, QString , QString ,QString ,QtTreePropertyBrowser * ,properconfig *)), nullptr, nullptr);
            //                delete sub_medth_unit_it.value().proper_config;
            //                sub_medth_unit_it.value().proper_config = nullptr;
            //                it.value().map_unit_method.erase(sub_medth_unit_it++);


            //            }
            m_current_project.map_task_all.erase( it++);
        }
    }
    if(ui->listWidget_task->count()>0) ui->listWidget_task->clear();
    renwu_index = 0;

    // ui->listWidget_task->takeItem(selet_count);
}

void MainWindow::struct_task_by_xml( QString task_name,  QString str_id, QMap<QString,SCT_DragTreeWidget > & inpu_map_task_all , QListWidget *inqlw)
{
    renwu_index++;
    QString name_task_win =task_name;
    DragTreeWidget *pWidgetls = new DragTreeWidget();

    pWidgetls->id_str = str_id;
    pWidgetls->setWindowTitle(name_task_win);
    pWidgetls->setIconSize(QSize(size_icon_m, size_icon_m));
    QIcon myIcon=QIcon(":/images/renwu1.jpeg");
    pWidgetls->setWindowIcon(myIcon);
    pWidgetls->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    QMdiSubWindow *h_temp= ui->mdiArea->addSubWindow(pWidgetls);
    h_temp->setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);

    pWidgetls->win_id = renwu_index;
    pWidgetls->setAcceptDrops(true);


    pWidgetls->show();
    connect(pWidgetls,&DragTreeWidget:: send_infor_changed_RightButton,this, &MainWindow::on_changed_from_tree_RightButton);//绑定信号槽，当值改变的时候会发送信号
    connect( pWidgetls,SIGNAL(itemClicked_QModelIndex(QTreeWidgetItem*,QModelIndex,QString)), this,SLOT(setTreaNode(QTreeWidgetItem*,QModelIndex,QString )));
    connect( pWidgetls,SIGNAL(send_msg_add_event(QListWidgetItem*,QString)), this,SLOT(slot_drop_new_method(QListWidgetItem*,QString)));


    //构建任务信息
    SCT_DragTreeWidget temp;
    temp.ptr_DragTreeWidget  = pWidgetls;
    temp.tile_text = name_task_win;
    temp.task_id_str =  pWidgetls->id_str;
    temp.win_haldle = h_temp;

    inpu_map_task_all[ pWidgetls->id_str] =temp;
    //这里可能有疑问
    inqlw->insertItem(renwu_index,name_task_win);
}

void  MainWindow::tianxie_camera(XMLElement* cur_method ,QString unit_name,  QString str_unit_id, QString str_tsk_id, properconfig * proper_config)
{

    //选择相机厂家
    QtProperty * prp =  proper_config->idToProperty[QObject::tr("相机厂商列表")];
    XMLElement* temp = GetElePtr_by_name(cur_method,"相机厂商列表");
    QString camera_factor = QString::fromStdString( temp->GetText());

    int camera_index = proper_config->m_map_name_index_camera_factor[camera_factor];
    proper_config->enumManager->setValue(prp, camera_index);

    //写相机名称
    prp =  proper_config->idToProperty[QObject::tr("相机列表")];
    temp = GetElePtr_by_name(cur_method,"相机列表");
    QString camera_name = QString::fromStdString( temp->GetText());
    for(int i = 0; i < proper_config->m_input_str.size(); i++)
    {
        QString instr = QString::fromStdString(proper_config->m_input_str[i]);

        if(camera_name == instr)
        {

            proper_config->enumManager->setValue(prp, i);
        }
    }

    prp =  proper_config->idToProperty[QObject::tr("相机工作模式")];
    temp = GetElePtr_by_name(cur_method,"相机工作模式");
    int gongzuomoshi = QString::fromStdString( temp->GetText()).toInt();
    proper_config->enumManager->setValue(prp, gongzuomoshi);


    prp =  proper_config->idToProperty[QObject::tr("曝光时间")];
    temp = GetElePtr_by_name(cur_method,"曝光时间");
    double baoguangshijian = QString::fromStdString( temp->GetText()).toDouble();
    // qDebug()<<"===>"<<baoguangshijian<<endl;
    proper_config->m_pVarManager->setValue(prp, baoguangshijian);


    prp =  proper_config->idToProperty[QObject::tr("增益")];
    temp = GetElePtr_by_name(cur_method,"增益");
    double zengyi = QString::fromStdString( temp->GetText()).toDouble();
    proper_config->m_pVarManager->setValue(prp, zengyi);
}

void  MainWindow::tianxie_zhengtishibie(XMLElement* cur_method ,QString unit_name,  QString str_unit_id, QString str_tsk_id, properconfig * proper_config)
{


    //算法类型
    QtProperty * prp =  proper_config->idToProperty[QObject::tr("算法类型")];
    XMLElement* temp = GetElePtr_by_name(cur_method,"算法");
    QString alg_leixing = QString::fromStdString( temp->GetText());
    QStringList alg_list= proper_config->enumManager->enumNames(prp);
    int index = alg_list.indexOf(alg_leixing);
    proper_config->enumManager->setValue(prp, index);

    //子算法
    prp =  proper_config->idToProperty[QObject::tr("子算法")];
    temp = GetElePtr_by_name(cur_method,"子算法");
    QString sub_alg_leixing = QString::fromStdString( temp->GetText());
    QStringList sun_alg_list= proper_config->enumManager->enumNames(prp);
    int sub_index = sun_alg_list.indexOf(sub_alg_leixing);
    proper_config->enumManager->setValue(prp, sub_index);


    prp =  proper_config->idToProperty[QObject::tr("算法模型文件")];
    temp = GetElePtr_by_name(cur_method,"算法模型文件");
    QString alg_mode_file = QString::fromStdString( temp->GetText());
    proper_config->fileManager->setValue(prp, alg_mode_file);

    //gpu
    prp =  proper_config->idToProperty[QObject::tr("选择显卡")];
    temp = GetElePtr_by_name(cur_method,"显卡");
    QString gpu_name = QString::fromStdString( temp->GetText());
    QStringList gpu_list= proper_config->enumManager->enumNames(prp);
    int gpu_index = gpu_list.indexOf(gpu_name);
    proper_config->enumManager->setValue(prp, gpu_index);


    ///////////////////////更新类目文件///////////////////////////////

    string fileName =  alg_mode_file.toStdString();
    char drive[_MAX_DRIVE] = { 0 };
    char dir[_MAX_DIR] = { 0 };
    char fname[_MAX_FNAME] = { 0 };
    char ext[_MAX_EXT] = { 0 };
    _splitpath(fileName.c_str(), drive, dir, fname, ext);

    string pathFile_root = string(drive) + string(dir);
    proper_config->model_pathFile_root =QString::fromStdString(pathFile_root);
    proper_config->alg_be_call_num[proper_config->m_unit_id_str] = 0;

    string classesname = pathFile_root+ string(fname) + string(ext);
    ifstream inFileclasses;
    inFileclasses.open(classesname, ios::in);
    if (!inFileclasses.is_open())//判断文件是否打开
    {
        std::cout << "Error opening inFileclasses" << std::endl;

    }else
    {
        char line[1024] = { 0 };


        if (proper_config->classnamestr.size() > 0)
            proper_config->classnamestr.clear();


        while (inFileclasses.getline(line, sizeof(line)))
        {
            std::stringstream word(line);
            string tempstr;
            word >> tempstr;
            proper_config->classnamestr.push_back(tempstr);
        }

        inFileclasses.close();
    }

#if USING_PAIJICI_FABU

    // 从xml读进来
    prp =  proper_config->idToProperty[QObject::tr("拍照几次才计算")];
    temp = GetElePtr_by_name(cur_method,"拍照几次才计算");


    int pzjc_js = QString::fromStdString( temp->GetText()).toInt();


    proper_config->m_pVarManager->setValue(prp, pzjc_js);

#endif




    //先全部删除目标
    proper_config->remove_number_param_sub(QObject::tr("目标"));

    //再重新根据类目文件来生成
    QtVariantProperty *x_item =proper_config->creater_unit( QObject::tr("目标"));
    x_item->setValue(QString::number( proper_config->classnamestr.size()));
    proper_config->change_classes();

    //////////////////////////////////////////////////////////


    XMLElement* target_unit_index = GetElePtr_by_name(cur_method,"目标");

    cout<<"size:"<<proper_config->classnamestr.size()<<endl;
    for(int i = 0; i < proper_config->classnamestr.size(); i++)
    {


        QString name_class_title=QObject::tr("类别名称")+"_" +QString::number(i) +QObject::tr("号");


        QMap<QString, QtProperty *>  class_infor = proper_config->m_classNameToIdToProperty[name_class_title];



        prp =  class_infor[name_class_title];
        XMLElement* class_unit_index = GetElePtr_by_name(target_unit_index,name_class_title.toStdString());

        XMLElement* obj_temp = GetElePtr_by_name(class_unit_index,"名称");
        QString obj_name = QString::fromStdString( obj_temp->GetText());

        qDebug()<<obj_name<<"kaishi"<<endl;;
        proper_config->m_pVarManager->setValue(prp, obj_name);

        prp =  class_infor[QObject::tr("中文名称")];
        obj_temp = GetElePtr_by_name(class_unit_index,"中文名称");
        QString zh_name = QString::fromStdString( obj_temp->GetText());
        proper_config->m_pVarManager->setValue(prp, zh_name);


#if ZJ_CHULIAOKOU_HAO


        prp =  class_infor[QObject::tr("出料口顺序号")];
        obj_temp = GetElePtr_by_name(class_unit_index,"出料口顺序号");
        QString  chuliaohao   = QString::fromStdString( obj_temp->GetText());
        proper_config->m_pVarManager->setValue(prp, chuliaohao);


# endif



        prp =  class_infor[QObject::tr("是否启用")];
        obj_temp = GetElePtr_by_name(class_unit_index,"是否启用");
        int shifou = QString::fromStdString( obj_temp->GetText()).toInt();
        proper_config->m_pVarManager->setValue(prp, shifou);


        prp =  class_infor[QObject::tr("得分最小值")];
        obj_temp = GetElePtr_by_name(class_unit_index,"得分最小值");
        double  zuixiao_defen = QString::fromStdString( obj_temp->GetText()).toDouble();
        proper_config->m_pVarManager->setValue(prp, zuixiao_defen);

        prp =  class_infor[QObject::tr("得分最大值")];
        obj_temp = GetElePtr_by_name(class_unit_index,"得分最大值");
        double  zuida_defen = QString::fromStdString( obj_temp->GetText()).toDouble();
        proper_config->m_pVarManager->setValue(prp, zuida_defen);


        prp =  class_infor[QObject::tr("目标框最小值")];
        obj_temp = GetElePtr_by_name(class_unit_index,"目标框最小值");
        XMLElement* obj_w =  GetElePtr_by_name(obj_temp,"Width");
        double obj_w_zui = QString::fromStdString( obj_w->GetText()).toDouble();
        XMLElement* obj_h =  GetElePtr_by_name(obj_temp,"Height");
        double obj_h_zui = QString::fromStdString( obj_h->GetText()).toDouble();
        QSizeF obj_wh=QSizeF(obj_w_zui,obj_h_zui );
        proper_config->m_pVarManager->setValue(prp, obj_wh);

        prp =  class_infor[QObject::tr("目标框最大值")];
        obj_temp = GetElePtr_by_name(class_unit_index,"目标框最大值");
        obj_w =  GetElePtr_by_name(obj_temp,"Width");
        obj_w_zui = QString::fromStdString( obj_w->GetText()).toDouble();
        obj_h =  GetElePtr_by_name(obj_temp,"Height");
        obj_h_zui = QString::fromStdString( obj_h->GetText()).toDouble();
        obj_wh=QSizeF(obj_w_zui,obj_h_zui );
        proper_config->m_pVarManager->setValue(prp, obj_wh);

        prp =  class_infor[QObject::tr("面积最小值")];
        obj_temp = GetElePtr_by_name(class_unit_index,"面积最小值");
        zuida_defen = QString::fromStdString( obj_temp->GetText()).toDouble();
        proper_config->m_pVarManager->setValue(prp, zuida_defen);

        prp =  class_infor[QObject::tr("面积最大值")];
        obj_temp = GetElePtr_by_name(class_unit_index,"面积最大值");
        zuida_defen = QString::fromStdString( obj_temp->GetText()).toDouble();
        proper_config->m_pVarManager->setValue(prp, zuida_defen);


        prp =  class_infor[QObject::tr("宽当量mm")];
        obj_temp = GetElePtr_by_name(class_unit_index,"宽当量mm");
        zuida_defen = QString::fromStdString( obj_temp->GetText()).toDouble();
        proper_config->m_pVarManager->setValue(prp, zuida_defen);


        prp =  class_infor[QObject::tr("高当量mm")];
        obj_temp = GetElePtr_by_name(class_unit_index,"高当量mm");
        zuida_defen = QString::fromStdString( obj_temp->GetText()).toDouble();
        proper_config->m_pVarManager->setValue(prp, zuida_defen);

        prp =  class_infor[QObject::tr("是否乘以当量")];
        obj_temp = GetElePtr_by_name(class_unit_index,"是否乘以当量");
        shifou = QString::fromStdString( obj_temp->GetText()).toInt();
        proper_config->m_pVarManager->setValue(prp, shifou);


        prp =  class_infor[QObject::tr("是否启用ROI")];
        obj_temp = GetElePtr_by_name(class_unit_index,"是否启用ROI");
        shifou = QString::fromStdString( obj_temp->GetText()).toInt();
        proper_config->m_pVarManager->setValue(prp, shifou);

        prp =  class_infor[QObject::tr("感兴趣区域ROI")];
        obj_temp = GetElePtr_by_name(class_unit_index,"感兴趣区域ROI");



        XMLElement* roi_x =  GetElePtr_by_name(obj_temp,"X");
        double v_roi_x = QString::fromStdString( roi_x->GetText()).toDouble();

        XMLElement* roi_y =  GetElePtr_by_name(obj_temp,"Y");
        double v_roi_y = QString::fromStdString( roi_y->GetText()).toDouble();

        XMLElement* roi_Width =  GetElePtr_by_name(obj_temp,"Width");
        double v_roi_Width = QString::fromStdString( roi_Width->GetText()).toDouble();

        XMLElement* roi_Height =  GetElePtr_by_name(obj_temp,"Height");
        double v_roi_Height = QString::fromStdString( roi_Height->GetText()).toDouble();

        QRectF obj_roi= QRectF(v_roi_x,v_roi_y,v_roi_Width,v_roi_Height );
        proper_config->m_pVarManager->setValue(prp, obj_roi);

    }

}


//填写属性
void MainWindow::tianxie_method_unit(XMLElement* cur_method ,QString unit_name,  QString str_unit_id, QString str_tsk_id, properconfig * proper_config)
{

    if(proper_config !=nullptr)
    {
        if(proper_config->source_name ==QObject::tr("相机"))
        {

            tianxie_camera(cur_method , unit_name,   str_unit_id,  str_tsk_id,  proper_config);
            cout<<"xiangji ok"<<endl;

        }

        if(proper_config->source_name ==QObject::tr("局部识别"))
        {

            tianxie_zhengtishibie(cur_method , unit_name,   str_unit_id,  str_tsk_id,  proper_config);

        }

        if(proper_config->source_name ==QObject::tr("整体识别"))
        {

            tianxie_zhengtishibie(cur_method , unit_name,   str_unit_id,  str_tsk_id,  proper_config);

        }

    }

}
//打开xml文件构建任务单元
void MainWindow::struct_method_unit_by_xml( XMLElement* cur_method ,QString unit_name,  QString str_unit_id, SCT_DragTreeWidget &task_tree , QString str_tsk_id, int i)
{

    QMap<QTreeWidgetItem *, Task_unit_infor > in_map_unit_method;

    DragTreeWidget *pWidgetls =task_tree.ptr_DragTreeWidget;

    QTreeWidgetItem *unit_tree =  new QTreeWidgetItem(pWidgetls, QStringList(unit_name));
    unit_tree->setIcon(0,str2QIcon_map[unit_name]);

    pWidgetls->addTopLevelItem(unit_tree);

    Task_unit_infor tsk_method_unit;
    tsk_method_unit.src_name =unit_name;
    tsk_method_unit.task_id_str =str_tsk_id;
    tsk_method_unit.name_index = str2int_src_index_map[unit_name];
    tsk_method_unit.str_method_in_tsk_id = str_unit_id;

    int i_task_pos_index = QString::fromStdString( GetElePtr_by_name(cur_method,"task_pos_index")->GetText()).toInt();
    // tsk_unit.top_item =child ;
    tsk_method_unit.task_pos_index = i_task_pos_index;

    if(unit_name=="输出")
    {
        tsk_method_unit.out_result_win = new OutPutResult( this, tsk_method_unit.src_name,task_tree.task_id_str,str_unit_id);
        tsk_method_unit.out_result_win->m_ptr_movectl = &std_mv_ctrl;
        tsk_method_unit.src_ptr =  (OutPutResult *) tsk_method_unit.out_result_win;

        //  connect(tsk_method_unit.out_result_win,&OutPutResult::send_to_main,this, &MainWindow::onValueChanged);//绑定信号槽，当值改变的时候会发送信号
    }else
    {
        tsk_method_unit.proper_config = new properconfig( tsk_method_unit.src_name,str_tsk_id,str_unit_id);

        m_mp_methodID_pro[str_unit_id] = tsk_method_unit.proper_config;
        //把读取进来的数据,对配置表进行填充
        tianxie_method_unit(cur_method , unit_name,   str_unit_id,  str_tsk_id, tsk_method_unit.proper_config);


        connect(tsk_method_unit.proper_config,&properconfig::send_to_main,this, &MainWindow::onValueChanged);//绑定信号槽，当值改变的时候会发送信号

    }




    task_tree.map_unit_method[unit_tree] =tsk_method_unit;

}
//打开工程函数
void MainWindow::open_project_xml(QString filename)
{


    close_current_project();

    ui->label_show_project_name->setText(m_str_project_file_name);//m_str_project_file_name只是工程文件名称而已
    m_str_project_file_path = filename;//工程文件路径

    tinyxml2::XMLDocument doc;
    doc.LoadFile(filename.toStdString().c_str());//本身有关闭功能
    XMLElement *root_node=doc.RootElement();//工程就是他们的根节点

    m_current_project.str_project_file_path =m_str_project_file_path;
    //m_current_project.str_project_file_cfg_fold_path = m_str_project_cfg_set_path;

    XMLElement* temp = GetElePtr_by_name(root_node,"工程名称");
    m_current_project.str_project_name = QString::fromStdString( string(temp->GetText()));

    for (XMLElement* currentele = root_node->FirstChildElement(); currentele; currentele = currentele->NextSiblingElement())
    {
        if(currentele->FindAttribute("tsk_id")!=nullptr)
        {
            QString task_name = QString::fromStdString(currentele->Name());
            QString str_id =QString::fromStdString(currentele->FindAttribute("tsk_id")->Value());

            struct_task_by_xml(task_name, str_id, m_current_project.map_task_all,  ui->listWidget_task);
            int i = 0;
            // 遍历方法
            for (XMLElement* cur_method = currentele->FirstChildElement(); cur_method; cur_method = cur_method->NextSiblingElement())
            {
                if(cur_method->FindAttribute("unit_id")!=nullptr)
                {

                    struct_method_unit_by_xml( cur_method ,cur_method->Name(),  cur_method->FindAttribute("unit_id")->Value(),
                                               m_current_project.map_task_all[str_id] , str_id,    i);
                    i++;

                }

            }

        }

    }


    cout<<"finish"<<endl;
}

// 一键打开工程
void  MainWindow::one_key_start_device(int flag)
{



}



//打开工程
void MainWindow::on_ac_open_project_triggered()
{

    //根据相机名称来初始化调用，一个相机只能打开一次


    QPixmap fitpixmaps;
    QString xmlFileName;
    std::string srcxmlFile;
    xmlFileName = QFileDialog::getOpenFileName(this, tr("选择工程文件"), QDir::currentPath(), tr("File(*.xml)"));
    if (xmlFileName.isEmpty())
    {
        QMessageBox::warning(this, "请选文件", "文件为空");
        return;
    }
    srcxmlFile = xmlFileName.toStdString();
    string fileName = srcxmlFile;
    char drive[_MAX_DRIVE] = { 0 };
    char dir[_MAX_DIR] = { 0 };
    char fname[_MAX_FNAME] = { 0 };
    char ext[_MAX_EXT] = { 0 };
    _splitpath(fileName.c_str(), drive, dir, fname, ext);
    string file_format = string(ext);

    string pathFile_root = string(drive) + string(dir);

    m_str_project_file_path =xmlFileName;//工程文件名路径


    m_current_project.str_project_name =QString::fromStdString(string(fname));

    m_current_project.str_project_file_name =QString::fromStdString(string(fname) +string(".xml"));



    m_str_project_file_name =QString::fromStdString(fname) + QString::fromStdString(file_format) ;//工程文件名称

    open_project_xml(xmlFileName);

    if(ui->listWidget_camera->count()>0) ui->listWidget_camera->clear();

    vector<string> inputstr;
    // haik_cam hk_obj;
    // vector<std::string >& cam_list
    // hk_obj.list_camera(inputstr);

#if USING_MAIDEWEISHI

    hk_objxx.list_camera(inputstr);
#else

    haik_cam hk_obj;
    hk_obj.list_camera(inputstr);
#endif


    for(int i = 0; i < inputstr.size();i++)
    {
        m_cout_camera_use[QString::fromStdString(inputstr[i])] = 0;

        ui->listWidget_camera->addItem ( QString::fromStdString(inputstr[i]) );
        // ui->listWidget_camera->

    }

}



// 创建工程
void MainWindow::create_prject_doc( QString filename, QString project_name)
{

    const char* declaration ="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>";
    tinyxml2::XMLDocument doc;
    doc.Parse(declaration);//会覆盖xml所有内容

    XMLElement* root=doc.NewElement("工程");
    doc.InsertEndChild(root);

    ui->label_show_project_name->setText(project_name);

    m_str_current_project_name =project_name;//当前工程名称

    m_current_project.str_project_name=m_str_current_project_name;
    m_current_project.str_project_file_name= m_str_project_file_name;
    m_current_project.str_project_file_path = m_str_project_file_path;


    XMLElement* gender = doc.NewElement("工程名称");
    XMLText* genderText=doc.NewText( project_name.toStdString().c_str() );
    gender->InsertEndChild(genderText);
    root->InsertEndChild(gender);
    doc.SaveFile(filename.toStdString().c_str());

    return;
}

void MainWindow::onGetDlg_infor(QString infor)
{
    QString path =QCoreApplication::applicationDirPath();
    string exe_filepath = path.toStdString();
    dir_user =exe_filepath+ "/user/";

    if (_access(dir_user.c_str(), 0) == -1)
    {
        _mkdir(dir_user.c_str());
    }
    qDebug()<<"name:"<<  infor;

    m_str_project_file_name =QString::fromStdString(dir_user)+ infor +".xml";

    m_str_project_file_path = m_str_project_file_name;//工程文件全路径
    create_prject_doc(m_str_project_file_path, infor);
}



//新建工程
void MainWindow::on_ac_new_project_triggered()
{


    QString Filepath;
    Filepath =QCoreApplication::applicationDirPath();
    string path =Filepath.toStdString();

    char drive[_MAX_DRIVE] = { 0 };
    char dir[_MAX_DIR] = { 0 };
    char fname[_MAX_FNAME] = { 0 };
    char ext[_MAX_EXT] = { 0 };
    _splitpath(path.c_str(), drive, dir, fname, ext);
    string file_format = string(ext);
    string pathFile = string(drive) + string(dir) ;

    m_str_project_exe_path = QString::fromStdString(pathFile);
    //m_str_project_cfg_set_path = m_str_project_exe_path;
    // m_project_file_path =m_project_exe_path + QString::fromStdString(fname) + QString::fromStdString(file_format);//工程文件全路径
    //dlg.open();
    dlg.open_pro_new_win();

    if(ui->listWidget_camera->count()>0) ui->listWidget_camera->clear();

    //根据相机名称来初始化调用，一个相机只能打开一次
    vector<string> inputstr;
    // haik_cam hk_obj;
    // vector<std::string >& cam_list
    //  hk_obj.list_camera(inputstr);

#if USING_MAIDEWEISHI

    hk_objxx.list_camera(inputstr);
#else

    haik_cam hk_obj;
    hk_obj.list_camera(inputstr);
#endif


    for(int i = 0; i < inputstr.size();i++)
    {
        m_cout_camera_use[QString::fromStdString(inputstr[i])] = 0;
        ui->listWidget_camera->addItem(QString::fromStdString(inputstr[i]));
    }


}

void MainWindow::on_changed_from_tree_RightButton(QTreeWidgetItem * _pSource ,const QPoint& _pos, int win_id,  QMdiSubWindow * win_hanle, QString task_id )
{


    if(!m_current_project.map_task_all[task_id].map_unit_method.empty())
    {
        if(m_current_project.map_task_all[task_id].map_unit_method[_pSource].src_name !=QObject::tr("输出"))
        {
            properconfig * conf = m_current_project.map_task_all[task_id].map_unit_method[_pSource].proper_config;

            conf->tree_br->setWindowIcon( str2QIcon_map[conf->source_name]);
            conf->tree_br->show();

        }else
        {
            OutPutResult * win_conf = m_current_project.map_task_all[task_id].map_unit_method[_pSource].out_result_win;

            if(win_conf !=nullptr)
            {
                win_conf->setWindowTitle(_pSource->text(0));
                win_conf->show();
            }

        }

    }

}

XMLElement*  MainWindow::GetElePtr_by_Attribute(XMLElement * element,const std::string Attribute_str,const std::string find_str)
{
    XMLElement* re_tmpele = nullptr;
    for (XMLElement* currentele = element->FirstChildElement(); currentele; currentele = currentele->NextSiblingElement())
    {
        XMLElement* tmpele = currentele;
        if(tmpele->FindAttribute(Attribute_str.c_str())!=nullptr)
        {
            string a_value =string(tmpele->FindAttribute(Attribute_str.c_str())->Value());
            if(QString::fromStdString(find_str) == QString::fromStdString(a_value))
            {
                re_tmpele = tmpele;
                return re_tmpele;
            }
        }
        if (!tmpele->NoChildren())
        {
            re_tmpele =GetElePtr_by_Attribute(tmpele,Attribute_str,find_str);
            if(re_tmpele !=nullptr) break;
        }

    }
    return re_tmpele;
}


XMLElement* MainWindow::GetElePtr_by_name(XMLElement * element,const std::string find_str)
{
    XMLElement * temp = nullptr;
    for (XMLElement* currentele = element->FirstChildElement(); currentele; currentele = currentele->NextSiblingElement())
    {
        XMLElement* tmpele = currentele;
        if (QString::fromStdString(tmpele->Name()) == QString::fromStdString(find_str))
        {
            // qDebug() << currentele->Name() << ":" << currentele->GetText() << endl;
            temp = tmpele;
            return tmpele;
        }

        if (!tmpele->NoChildren())
        {
            temp=GetElePtr_by_name(tmpele,find_str);
            if(temp!= nullptr) break;
        }

    }
    return temp;
}

void MainWindow::updata_camera_config( XMLElement* unit_enter_ptr, properconfig * proper_config)
{
    if(proper_config !=nullptr)
    {
        if(proper_config->source_name ==QObject::tr("相机"))
        {
            QtProperty * prp =  proper_config->idToProperty[QObject::tr("相机厂商列表")];
            int indexID = proper_config->enumManager->value(prp);

            XMLElement* tempelm= GetElePtr_by_name(unit_enter_ptr,"相机厂商列表");
            if(tempelm!=nullptr) tempelm->SetText( proper_config->m_map_index_name_camera_factor[indexID].toStdString().c_str());

            prp =  proper_config->idToProperty[QObject::tr("相机列表")];
            indexID = proper_config->enumManager->value(prp);
            tempelm= GetElePtr_by_name(unit_enter_ptr,"相机列表");
            if(tempelm!=nullptr) tempelm->SetText(proper_config->m_input_str[indexID].c_str());

            prp =  proper_config->idToProperty[QObject::tr("相机工作模式")];
            indexID = proper_config->enumManager->value(prp);
            tempelm= GetElePtr_by_name(unit_enter_ptr,"相机工作模式");
            if(tempelm!=nullptr) tempelm->SetText( QString::number(indexID).toStdString().c_str());

            prp =  proper_config->idToProperty[QObject::tr("曝光时间")];
            float findexID = proper_config->m_pVarManager->value(prp).toFloat();
            
            tempelm= GetElePtr_by_name(unit_enter_ptr,"曝光时间");
            if(tempelm!=nullptr)  tempelm->SetText( QString::number(findexID).toStdString().c_str());
            
            prp =  proper_config->idToProperty[QObject::tr("增益")];
            findexID = proper_config->m_pVarManager->value(prp).toFloat();
            tempelm= GetElePtr_by_name(unit_enter_ptr,"增益");
            if(tempelm!=nullptr)  tempelm->SetText( QString::number(findexID).toStdString().c_str());

        }
    }
    
}

void MainWindow::updata_alg_config( tinyxml2::XMLDocument &doc, XMLElement* unit_enter_ptr, properconfig * proper_config,QString alg_name)
{
    if(proper_config !=nullptr)
    {
        if(proper_config->source_name ==alg_name)
        {

            QtProperty * prp =  proper_config->idToProperty[QObject::tr("算法类型")];
            int indexID = proper_config->enumManager->value(prp);

            QStringList alg_name_list =proper_config->enumManager->enumNames(prp);

            XMLElement*  tempelm= GetElePtr_by_name(unit_enter_ptr,"算法");
            if(tempelm!=nullptr)  tempelm->SetText( alg_name_list[indexID].toStdString().c_str());


            prp =  proper_config->idToProperty[QObject::tr("子算法")];
            indexID = proper_config->enumManager->value(prp);
            alg_name_list =proper_config->enumManager->enumNames(prp);

            tempelm= GetElePtr_by_name(unit_enter_ptr,"子算法");
            if(tempelm!=nullptr)  tempelm->SetText(  alg_name_list[indexID].toStdString().c_str());

            prp =  proper_config->idToProperty[QObject::tr("算法模型文件")];
            QString file_alg= proper_config->fileManager->value(prp);

            tempelm= GetElePtr_by_name(unit_enter_ptr,"算法模型文件");
            if(tempelm!=nullptr)  tempelm->SetText(  file_alg.toStdString().c_str());


            prp =  proper_config->idToProperty[QObject::tr("选择显卡")];
            indexID = proper_config->enumManager->value(prp);
            alg_name_list =proper_config->enumManager->enumNames(prp);

            tempelm= GetElePtr_by_name(unit_enter_ptr,"显卡");
            if(tempelm!=nullptr)  tempelm->SetText( alg_name_list[indexID].toStdString().c_str());


#if USING_PAIJICI_FABU

            tempelm= GetElePtr_by_name(unit_enter_ptr,"拍照几次才计算");
            prp =  proper_config->idToProperty[QObject::tr("拍照几次才计算")];
            int pzjc_js= proper_config->m_pVarManager->value(prp).toInt();



            if(tempelm!=nullptr)  tempelm->SetText(  QString::number(pzjc_js).toStdString().c_str());


#endif




            prp =  proper_config->idToProperty[QObject::tr("目标")];
            XMLElement* target_unit_index = GetElePtr_by_name(unit_enter_ptr,"目标");

            unit_enter_ptr->DeleteChild(target_unit_index);
#if 1


            target_unit_index =  writer_text_xlm("目标",  "", doc ,unit_enter_ptr );

            for(int i = 0; i < proper_config->classnamestr.size(); i++)
            {

                QString name_class_title=QObject::tr("类别名称")+"_" +QString::number(i) +QObject::tr("号");

                QMap<QString, QtProperty *>  class_infor = proper_config->m_classNameToIdToProperty[name_class_title];
                // qDebug()<<name_class_title;
                prp =  class_infor[name_class_title];

                QString target_name= proper_config->m_pVarManager->value(prp).toString();
                XMLElement* neirong_index =  writer_text_xlm(name_class_title, "", doc ,target_unit_index );
                // neirong_index->SetAttribute("name",target_name.toStdString().c_str() );
                writer_text_xlm("名称", target_name, doc ,neirong_index );

                prp =  class_infor[QObject::tr("中文名称")];
                QString target_ch_name= proper_config->m_pVarManager->value(prp).toString();
                writer_text_xlm("中文名称", target_ch_name, doc ,neirong_index );
                //neirong_index->SetAttribute("中文名称",target_ch_name.toStdString().c_str() );



#if ZJ_CHULIAOKOU_HAO

                prp =  class_infor[QObject::tr("出料口顺序号")];
                QString xunhaostre= proper_config->m_pVarManager->value(prp).toString();
                writer_text_xlm("出料口顺序号", xunhaostre, doc ,neirong_index );
                //neirong_index->SetAttribute("中文名称",target_ch_name.toStdString().c_str() );


# endif

                prp =  class_infor[QObject::tr("是否启用")];
                bool bool_name= proper_config->m_pVarManager->value(prp).toBool();
                writer_text_xlm("是否启用", QString::number(bool_name), doc ,neirong_index );
                //neirong_index->SetAttribute("是否启用", QString::number(bool_name).toStdString().c_str() );


                prp = class_infor[QObject::tr("得分最小值")];
                double d_name= proper_config->m_pVarManager->value(prp).toDouble();
                writer_text_xlm("得分最小值", QString::number(d_name), doc ,neirong_index );
                //neirong_index->SetAttribute("得分最小值", QString::number(d_name).toStdString().c_str() );

                prp =  class_infor[QObject::tr("得分最大值")];
                d_name= proper_config->m_pVarManager->value(prp).toDouble();
                writer_text_xlm("得分最大值", QString::number(d_name), doc ,neirong_index );
                //neirong_index->SetAttribute("得分最大值", QString::number(d_name).toStdString().c_str() );

                prp =  class_infor[QObject::tr("目标框最小值")];
                QSizeF s_name= proper_config->m_pVarManager->value(prp).toSizeF();
                XMLElement* qsize_zuixiao= writer_text_xlm("目标框最小值", "", doc ,neirong_index );

                //qsize_zuixiao->SetAttribute("宽", QString::number(s_name.width()).toStdString().c_str() );
                //qsize_zuixiao->SetAttribute("高", QString::number(s_name.height()).toStdString().c_str() );


                writer_text_xlm("Width", QString::number(s_name.width()), doc ,qsize_zuixiao );
                writer_text_xlm("Height", QString::number(s_name.height()), doc ,qsize_zuixiao );


                prp = class_infor[QObject::tr("目标框最大值")];
                s_name= proper_config->m_pVarManager->value(prp).toSizeF();
                qsize_zuixiao= writer_text_xlm("目标框最大值", "", doc ,neirong_index );

                //qsize_zuixiao->SetAttribute("宽", QString::number(s_name.width()).toStdString().c_str() );
                //qsize_zuixiao->SetAttribute("高", QString::number(s_name.height()).toStdString().c_str() );

                writer_text_xlm("Width", QString::number(s_name.width()), doc ,qsize_zuixiao );
                writer_text_xlm("Height", QString::number(s_name.height()), doc ,qsize_zuixiao );


                prp =  class_infor[QObject::tr("面积最小值")];
                d_name= proper_config->m_pVarManager->value(prp).toDouble();
                writer_text_xlm("面积最小值", QString::number(d_name), doc ,neirong_index );


                prp =  class_infor[QObject::tr("面积最大值")];
                d_name= proper_config->m_pVarManager->value(prp).toDouble();
                writer_text_xlm("面积最大值", QString::number(d_name), doc ,neirong_index );


                prp =  class_infor[QObject::tr("宽当量mm")];
                d_name= proper_config->m_pVarManager->value(prp).toDouble();
                writer_text_xlm("宽当量mm", QString::number(d_name), doc ,neirong_index );


                prp =  class_infor[QObject::tr("高当量mm")];
                d_name= proper_config->m_pVarManager->value(prp).toDouble();
                writer_text_xlm("高当量mm", QString::number(d_name), doc ,neirong_index );


                prp =  class_infor[QObject::tr("是否乘以当量")];
                bool_name= proper_config->m_pVarManager->value(prp).toBool();
                writer_text_xlm("是否乘以当量", QString::number(bool_name), doc ,neirong_index );


                prp =  class_infor[QObject::tr("是否启用ROI")];
                bool_name= proper_config->m_pVarManager->value(prp).toBool();
                writer_text_xlm("是否启用ROI", QString::number(bool_name), doc ,neirong_index );

                prp =  class_infor[QObject::tr("感兴趣区域ROI")];
                QRectF r_name= proper_config->m_pVarManager->value(prp).toRectF();
                qsize_zuixiao= writer_text_xlm("感兴趣区域ROI", "", doc ,neirong_index );

                // qsize_zuixiao->SetAttribute("x", QString::number(r_name.x()).toStdString().c_str() );
                //qsize_zuixiao->SetAttribute("y", QString::number(r_name.y()).toStdString().c_str() );

                // qsize_zuixiao->SetAttribute("width", QString::number(r_name.width()).toStdString().c_str() );
                //qsize_zuixiao->SetAttribute("height", QString::number(r_name.height()).toStdString().c_str() );


                writer_text_xlm("X", QString::number(r_name.x()), doc ,qsize_zuixiao );
                writer_text_xlm("Y", QString::number(r_name.y()), doc ,qsize_zuixiao );
                writer_text_xlm("Width", QString::number(r_name.width()), doc ,qsize_zuixiao );
                writer_text_xlm("Height", QString::number(r_name.height()), doc ,qsize_zuixiao );
            }
            //for end


#else

            for(int i = 0; i < proper_config->classnamestr.size(); i++)
            {

                QString name_class_title=QObject::tr("类别名称")+"_" +QString::number(i) +QObject::tr("号");

                QMap<QString, QtProperty *>  class_infor = proper_config->m_classNameToIdToProperty[name_class_title];

                prp =  class_infor[name_class_title];

                XMLElement* class_unit_index = GetElePtr_by_name(target_unit_index,name_class_title.toStdString());

                QString target_name= proper_config->m_pVarManager->value(prp).toString();

                XMLElement* tempelm_sub= GetElePtr_by_name(class_unit_index,"名称");
                if(tempelm_sub!=nullptr)  tempelm_sub->SetText( target_name.toStdString().c_str());

                prp =  class_infor[QObject::tr("中文名称")];
                QString target_ch_name= proper_config->m_pVarManager->value(prp).toString();

                tempelm_sub= GetElePtr_by_name(class_unit_index,"中文名称");
                if(tempelm_sub!=nullptr)  tempelm_sub->SetText( target_ch_name.toStdString().c_str());


                prp =  class_infor[QObject::tr("是否启用")];

                bool bool_name= proper_config->m_pVarManager->value(prp).toBool();

                tempelm_sub= GetElePtr_by_name(class_unit_index,"是否启用");
                if(tempelm_sub!=nullptr)  tempelm_sub->SetText(  QString::number(bool_name).toStdString().c_str());


                prp =  class_infor[QObject::tr("得分最小值")];
                double d_name= proper_config->m_pVarManager->value(prp).toDouble();

                tempelm_sub= GetElePtr_by_name(class_unit_index,"得分最小值");
                if(tempelm_sub!=nullptr)  tempelm_sub->SetText( QString::number(d_name).toStdString().c_str());


                prp =  class_infor[QObject::tr("得分最大值")];
                d_name= proper_config->m_pVarManager->value(prp).toDouble();

                tempelm_sub= GetElePtr_by_name(class_unit_index,"得分最大值");
                if(tempelm_sub!=nullptr)  tempelm_sub->SetText( QString::number(d_name).toStdString().c_str());

                prp =  class_infor[QObject::tr("目标框最小值")];
                QSizeF s_name= proper_config->m_pVarManager->value(prp).toSizeF();


                tempelm_sub= GetElePtr_by_name(class_unit_index,"目标框最小值");

                XMLElement* obj_w =  GetElePtr_by_name(tempelm_sub,"Width");
                if(obj_w!=nullptr)  obj_w->SetText( QString::number(s_name.width()).toStdString().c_str());

                XMLElement* obj_h =  GetElePtr_by_name(tempelm_sub,"Height");
                if(obj_h!=nullptr)  obj_h->SetText( QString::number(s_name.height()).toStdString().c_str());

                prp =  class_infor[QObject::tr("目标框最大值")];
                s_name= proper_config->m_pVarManager->value(prp).toSizeF();

                tempelm_sub= GetElePtr_by_name(class_unit_index,"目标框最大值");
                obj_w =  GetElePtr_by_name(tempelm_sub,"Width");
                if(obj_w!=nullptr)  obj_w->SetText( QString::number(s_name.width()).toStdString().c_str());

                obj_h =  GetElePtr_by_name(tempelm_sub,"Height");
                if(obj_h!=nullptr)  obj_h->SetText( QString::number(s_name.height()).toStdString().c_str());

                prp =  class_infor[QObject::tr("面积最小值")];
                d_name= proper_config->m_pVarManager->value(prp).toDouble();

                tempelm_sub= GetElePtr_by_name(class_unit_index,"面积最小值");
                if(tempelm_sub!=nullptr)  tempelm_sub->SetText( QString::number(d_name).toStdString().c_str());


                prp =  class_infor[QObject::tr("面积最大值")];
                d_name= proper_config->m_pVarManager->value(prp).toDouble();

                tempelm_sub= GetElePtr_by_name(class_unit_index,"面积最大值");
                if(tempelm_sub!=nullptr)  tempelm_sub->SetText( QString::number(d_name).toStdString().c_str());

                prp =  class_infor[QObject::tr("宽当量mm")];
                d_name= proper_config->m_pVarManager->value(prp).toDouble();

                tempelm_sub= GetElePtr_by_name(class_unit_index,"宽当量mm");
                if(tempelm_sub!=nullptr)  tempelm_sub->SetText( QString::number(d_name).toStdString().c_str());

                prp =  class_infor[QObject::tr("高当量mm")];
                d_name= proper_config->m_pVarManager->value(prp).toDouble();


                tempelm_sub= GetElePtr_by_name(class_unit_index,"高当量mm");
                if(tempelm_sub!=nullptr)  tempelm_sub->SetText( QString::number(d_name).toStdString().c_str());

                prp =  class_infor[QObject::tr("是否乘以当量")];
                bool_name= proper_config->m_pVarManager->value(prp).toBool();


                tempelm_sub= GetElePtr_by_name(class_unit_index,"是否乘以当量");
                if(tempelm_sub!=nullptr)  tempelm_sub->SetText( QString::number(bool_name).toStdString().c_str());

                prp =  class_infor[QObject::tr("是否启用ROI")];
                bool_name= proper_config->m_pVarManager->value(prp).toBool();

                tempelm_sub= GetElePtr_by_name(class_unit_index,"是否启用ROI");
                if(tempelm_sub!=nullptr)  tempelm_sub->SetText( QString::number(bool_name).toStdString().c_str());

                prp =  class_infor[QObject::tr("感兴趣区域ROI")];
                QRectF r_name= proper_config->m_pVarManager->value(prp).toRectF();

                tempelm_sub= GetElePtr_by_name(class_unit_index,"感兴趣区域ROI");
                if(tempelm_sub!=nullptr)
                {
                    XMLElement* obj_x =  GetElePtr_by_name(tempelm_sub,"X");
                    if(obj_x!=nullptr)  obj_x->SetText( QString::number(r_name.x()).toStdString().c_str());

                    XMLElement* obj_y =  GetElePtr_by_name(tempelm_sub,"Y");
                    if(obj_y!=nullptr)  obj_y->SetText( QString::number(r_name.y()).toStdString().c_str());

                    obj_w =  GetElePtr_by_name(tempelm_sub,"Width");
                    if(obj_w!=nullptr)  obj_w->SetText( QString::number(r_name.width()).toStdString().c_str());

                    obj_h =  GetElePtr_by_name(tempelm_sub,"Height");
                    if(obj_h!=nullptr)  obj_h->SetText( QString::number(r_name.height()).toStdString().c_str());
                }
            }
            //for end
#endif

        }
    }

}
//updata  xml
void MainWindow::updata_xml(QString filename, QString source_name, properconfig * proper_config, const  QString task_id,const  QString unit_id )
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile(filename.toStdString().c_str());
    XMLElement *root_node=doc.RootElement();//工程就是他们的根节点

    for (XMLElement* currentele = root_node->FirstChildElement(); currentele; currentele = currentele->NextSiblingElement())
    {
        if(currentele->FindAttribute("tsk_id")!=nullptr)
        {
            if( string(currentele->FindAttribute("tsk_id")->Value())==task_id.toStdString())
            {

                XMLElement* unit_enter_ptr = GetElePtr_by_Attribute(currentele,"unit_id",unit_id.toStdString());
                if(unit_enter_ptr!= nullptr)
                {

                    if(QString::fromStdString(unit_enter_ptr->Name())== QObject::tr("相机"))  updata_camera_config( unit_enter_ptr, proper_config);
                    if(QString::fromStdString(unit_enter_ptr->Name())== QObject::tr("显示"))  updata_camera_config( unit_enter_ptr, proper_config);
                    if(QString::fromStdString(unit_enter_ptr->Name())== QObject::tr("保存图片"))  updata_camera_config( unit_enter_ptr, proper_config);
                    // if(QString::fromStdString(unit_enter_ptr->Name())== QObject::tr("输出"))  updata_camera_config( unit_enter_ptr, proper_config);


                    if(QString::fromStdString(unit_enter_ptr->Name())== QObject::tr("整体识别"))  updata_alg_config(doc, unit_enter_ptr, proper_config,QObject::tr("整体识别"));
                    if(QString::fromStdString(unit_enter_ptr->Name())== QObject::tr("局部识别"))  updata_alg_config(doc,  unit_enter_ptr, proper_config,QObject::tr("局部识别"));
                    if(QString::fromStdString(unit_enter_ptr->Name())== QObject::tr("字符识别"))  updata_alg_config( doc, unit_enter_ptr, proper_config,QObject::tr("字符识别"));
                    if(QString::fromStdString(unit_enter_ptr->Name())== QObject::tr("分割"))  updata_alg_config( doc, unit_enter_ptr, proper_config,QObject::tr("分割"));
                    // qDebug()<<QString::fromStdString(unit_enter_ptr->Name());
                    break;
                }

            }
        }

    }


    doc.SaveFile(filename.toStdString().c_str());
    cout<<"finish"<<endl;
}

//更新算法模型的时候调用
void MainWindow::updata_property_config(QString filename, properconfig * proper_config,QString alg_name)
{

    if(proper_config !=nullptr)
    {
        if(proper_config->source_name ==alg_name)
        {

            QtProperty * prp =  proper_config->idToProperty[QObject::tr("算法模型文件")];
            QString file_alg= proper_config->fileManager->value(prp);

            string fileName =  file_alg.toStdString();
            char drive[_MAX_DRIVE] = { 0 };
            char dir[_MAX_DIR] = { 0 };
            char fname[_MAX_FNAME] = { 0 };
            char ext[_MAX_EXT] = { 0 };
            _splitpath(fileName.c_str(), drive, dir, fname, ext);

            string pathFile_root = string(drive) + string(dir);
            proper_config->model_pathFile_root =QString::fromStdString(pathFile_root);
            proper_config->alg_be_call_num[proper_config->m_unit_id_str] = 0;

            string classesname = pathFile_root+ string(fname) + string(ext);
            ifstream inFileclasses;
            inFileclasses.open(classesname, ios::in);
            if (!inFileclasses.is_open())//判断文件是否打开
            {
                std::cout << "Error opening inFileclasses" << std::endl;

            }else
            {
                char line[1024] = { 0 };


                if (proper_config->classnamestr.size() > 0)
                    proper_config->classnamestr.clear();


                while (inFileclasses.getline(line, sizeof(line)))
                {
                    std::stringstream word(line);
                    string tempstr;
                    word >> tempstr;
                    proper_config->classnamestr.push_back(tempstr);
                }

                inFileclasses.close();
            }

            //先全部删除目标
            proper_config->remove_number_param_sub(QObject::tr("目标"));

            //再重新根据类目文件来生成
            QtVariantProperty *x_item =proper_config->creater_unit( QObject::tr("目标"));
            x_item->setValue(QString::number( proper_config->classnamestr.size()));
            proper_config->change_classes();

            //算法释放，是否可以放这里

        }
    }



    //    tinyxml2::XMLDocument doc;
    //    doc.LoadFile(filename.toStdString().c_str());
    //    XMLElement *root_node=doc.RootElement();//工程就是他们的根节点

    //    for (XMLElement* currentele = root_node->FirstChildElement(); currentele; currentele = currentele->NextSiblingElement())
    //    {
    //        if(currentele->FindAttribute("tsk_id")!=nullptr)
    //        {
    //            if( string(currentele->FindAttribute("tsk_id")->Value())==proper_config->m_task_id_str.toStdString())
    //            {

    //                XMLElement* unit_enter_ptr = GetElePtr_by_Attribute(currentele,"unit_id",proper_config->m_unit_id_str.toStdString());
    //                if(unit_enter_ptr!= nullptr)
    //                {

    //                    if(QString::fromStdString(unit_enter_ptr->Name())== QObject::tr("相机"))  updata_camera_config( unit_enter_ptr, proper_config);
    //                    if(QString::fromStdString(unit_enter_ptr->Name())== QObject::tr("显示"))  updata_camera_config( unit_enter_ptr, proper_config);
    //                    if(QString::fromStdString(unit_enter_ptr->Name())== QObject::tr("保存图片"))  updata_camera_config( unit_enter_ptr, proper_config);
    //                    // if(QString::fromStdString(unit_enter_ptr->Name())== QObject::tr("输出"))  updata_camera_config( unit_enter_ptr, proper_config);


    //                    if(QString::fromStdString(unit_enter_ptr->Name())== QObject::tr("整体识别"))  updata_alg_config( unit_enter_ptr, proper_config,QObject::tr("整体识别"));
    //                    if(QString::fromStdString(unit_enter_ptr->Name())== QObject::tr("局部识别"))  updata_alg_config( unit_enter_ptr, proper_config,QObject::tr("局部识别"));
    //                    if(QString::fromStdString(unit_enter_ptr->Name())== QObject::tr("字符识别"))  updata_alg_config( unit_enter_ptr, proper_config,QObject::tr("字符识别"));
    //                    if(QString::fromStdString(unit_enter_ptr->Name())== QObject::tr("分割"))  updata_alg_config( unit_enter_ptr, proper_config,QObject::tr("分割"));
    //                    // qDebug()<<QString::fromStdString(unit_enter_ptr->Name());
    //                    break;
    //                }

    //            }
    //        }

    //    }


    //    doc.SaveFile(filename.toStdString().c_str());
    //    cout<<"finish"<<endl;


}

void MainWindow::onValueChanged(QtProperty *property, const QVariant &value, QString source_name ,
                                QString unit_id_str,QString task_id_str,QtTreePropertyBrowser * tree_br, properconfig * proper_config)
{

    // 模型更新 当改变模型文件类别数和路径
    if(property ==proper_config->item2_model_file)
    {

        if(source_name== QObject::tr("局部识别"))
        {
            updata_property_config( m_str_project_file_path, proper_config,QObject::tr("局部识别"));
        }
        if(source_name== QObject::tr("整体识别"))  updata_property_config( m_str_project_file_path,  proper_config,QObject::tr("整体识别"));
        if(source_name== QObject::tr("字符识别"))  updata_property_config(  m_str_project_file_path, proper_config,QObject::tr("字符识别"));
        if(source_name== QObject::tr("分割"))  updata_property_config(  m_str_project_file_path, proper_config,QObject::tr("分割"));
    }

    //配置参数更新
    if(source_name== QObject::tr("局部识别"))
    {
        if (m_map_record_ptr_jubushibie.count(unit_id_str) > 0)
        {
            if(m_map_record_ptr_jubushibie[unit_id_str]!=nullptr)
            {

                jubushibie_form * temp_ptr = (jubushibie_form * ) m_map_record_ptr_jubushibie[unit_id_str];
                set_alg_obj_class_propery( proper_config ,temp_ptr->m_alg_infor);
                //temp_ptr->update_property(proper_config);

            }
        }

    }
    if(source_name== QObject::tr("整体识别"))  updata_property_config(  m_str_project_file_path, proper_config,QObject::tr("整体识别"));
    if(source_name== QObject::tr("字符识别"))  updata_property_config( m_str_project_file_path,  proper_config,QObject::tr("字符识别"));
    if(source_name== QObject::tr("分割"))  updata_property_config(  m_str_project_file_path, proper_config,QObject::tr("分割"));

    updata_xml( m_str_project_file_path, source_name ,   proper_config, task_id_str,unit_id_str);

    return;


}

XMLElement* MainWindow::writer_text_xlm(const QString str_elm_name,const QString str_elm_txt,
                                        tinyxml2::XMLDocument &doc ,XMLElement *root_node )
{
    XMLElement* need_create_node = doc.NewElement(str_elm_name.toStdString().c_str());
    if(str_elm_txt!="")
    {
        XMLText* need_Text=doc.NewText( str_elm_txt.toStdString().c_str() );
        need_create_node->InsertEndChild(need_Text);
    }

    root_node->InsertEndChild(need_create_node);
    return need_create_node;

}
//xml增加任务
void  MainWindow::add_task_to_doc(QString filename,const SCT_DragTreeWidget & task_obj)
{


    tinyxml2::XMLDocument doc;
    doc.LoadFile(filename.toStdString().c_str());
    XMLElement *root_node=doc.RootElement();//工程就是他们的根节点

    XMLElement* task_node_index= writer_text_xlm(task_obj.win_haldle->windowTitle(),"", doc ,root_node );

    task_node_index->SetAttribute("tsk_id",task_obj.task_id_str.toStdString().c_str());
    writer_text_xlm("id", task_obj.task_id_str, doc ,task_node_index );
    doc.SaveFile(filename.toStdString().c_str());
    return;

}

//xml删除任务
void MainWindow::Remove_task_Xml(QString filename,const SCT_DragTreeWidget & task_obj)
{
    tinyxml2::XMLDocument doc;
    if(doc.LoadFile(filename.toStdString().c_str())!=0)
    {
        cout<<"load xml file failed"<<endl;
        return ;
    }
    XMLElement* root=doc.RootElement();
    for (XMLElement* currentele = root->FirstChildElement(); currentele; currentele = currentele->NextSiblingElement())
    {
        if(currentele->FindAttribute("tsk_id")!=nullptr)
        {
            string a_value =string(currentele->FindAttribute("tsk_id")->Value());

            if(task_obj.task_id_str.toStdString() == a_value)
            {
                XMLElement* chp = currentele;
                root->DeleteChild(chp);
                break;
            }
        }
    }
    doc.SaveFile(filename.toStdString().c_str());
}

//xml 删除方法节点
void MainWindow::Remove_method_unit_Xml(QString filename,QString unit_id)
{
    tinyxml2::XMLDocument doc;
    if(doc.LoadFile(filename.toStdString().c_str())!=0)
    {
        cout<<"load xml file failed"<<endl;
        return ;
    }
    XMLElement* root=doc.RootElement();

    //任务节点
    for (XMLElement* currentele = root->FirstChildElement(); currentele; currentele = currentele->NextSiblingElement())
    {
        //方法节点
        for (XMLElement* method_currentele = currentele->FirstChildElement(); method_currentele; method_currentele = method_currentele->NextSiblingElement())
        {
            //删除符合属性原则的 方法
            if(method_currentele->FindAttribute("unit_id")!=nullptr)
            {

                if( string(method_currentele->FindAttribute( "unit_id")->Value()) == unit_id.toStdString())
                {
                    XMLElement* chp = method_currentele;
                    currentele->DeleteChild(chp);
                    break;
                }
            }

        }

    }
    doc.SaveFile(filename.toStdString().c_str());
    return;
}

//删除任务中方法节点
void MainWindow::on_ac_delete_jiedian_triggered()
{
    if(current_tree_node_ptr!=nullptr)
    {
        QMap<QString ,SCT_DragTreeWidget>::Iterator it=m_current_project.map_task_all.begin();
        while(it!=m_current_project.map_task_all.end())
        {
            QString tsk_win_str_id = it.key();
            if(current_tree_win_id == tsk_win_str_id)
            {
                qDebug()<<current_tree_win_id;
                QMap<QTreeWidgetItem *, Task_unit_infor > ::Iterator sub_medth_unit_it =it.value().map_unit_method.begin();


                while(sub_medth_unit_it!=it.value().map_unit_method.end())
                {

                    //这里删除资源也要考虑
                    QTreeWidgetItem * tem_pt = sub_medth_unit_it.key();

                    if(tem_pt == current_tree_node_ptr)
                    {
                        // qDebug()<<"sub:"<<sub_medth_unit_it.value().src_name;
                        disconnect(sub_medth_unit_it.value().proper_config, SIGNAL(send_to_main(QtProperty *, const QVariant &, QString  , QString ,QString ,QtTreePropertyBrowser * ,properconfig *)), nullptr, nullptr);

                        Remove_method_unit_Xml(m_str_project_file_path,sub_medth_unit_it.value().str_method_in_tsk_id);
                        QString unit_id_str=sub_medth_unit_it.value().str_method_in_tsk_id;

                        //删除节点时 删除算法分配的资源
                        if(sub_medth_unit_it.value().src_name== QObject::tr("局部识别"))
                        {

                            if (m_map_record_ptr_jubushibie.count(unit_id_str) > 0)
                            {
                                if(m_map_record_ptr_jubushibie[unit_id_str]!=nullptr)
                                {
                                    jubushibie_form * temp_ptr = (jubushibie_form * ) m_map_record_ptr_jubushibie[unit_id_str];
                                    m_mp_alg_busy_flag[temp_ptr->m_alg_infor.m_model_id_int] = 0;
                                    delete temp_ptr;
                                    m_map_record_ptr_jubushibie[unit_id_str] =nullptr;
                                    Del_Alg_jubu_Unit( unit_id_str , m_map_record_ptr_jubushibie);

                                }
                            }

                        }
                        if(sub_medth_unit_it.value().src_name== QObject::tr("整体识别")) {

                        }
                        if(sub_medth_unit_it.value().src_name== QObject::tr("字符识别")) {

                        }
                        if(sub_medth_unit_it.value().src_name== QObject::tr("分割")) {

                        }


                        delete sub_medth_unit_it.value().proper_config;
                        sub_medth_unit_it.value().proper_config = nullptr;
                        it.value().map_unit_method.erase(sub_medth_unit_it++);
                    }else
                    {
                        sub_medth_unit_it++;
                    }


                }
            }

            it++;
        }
        delete current_tree_node_ptr;
        current_tree_node_ptr  =nullptr;
    }

}


//xml 修改方法中某一项
void  MainWindow::rewrite_node_unit_pos_index(QString filename, const  QString task_id ,const  QString unit_name,QString need_ch_name,const  QString need_rewrite_txt)
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile(filename.toStdString().c_str());
    XMLElement *root_node=doc.RootElement();//工程就是他们的根节点

    //  XMLElement* task_node = GetEleValue(root_node,task_id.toStdString());
    XMLElement* task_node = GetElePtr_by_Attribute(root_node ,"tsk_id",task_id.toStdString());
    if(task_node!=nullptr)
    {
        // qDebug()<<task_node->Name();
        //XMLElement* unit_node = GetEleValue(task_node,unit_name.toStdString());
        XMLElement* unit_node = GetElePtr_by_Attribute(task_node ,"unit_id",unit_name.toStdString());

        if(unit_node!=nullptr)
        {
            //  qDebug()<<task_node->Name();
            XMLElement* need_node = GetElePtr_by_name(unit_node,need_ch_name.toStdString());

            if(need_node!=nullptr)
            {
                //  qDebug()<<task_node->Name();
                need_node->SetText(need_rewrite_txt.toStdString().c_str());
            }

        }
    }

    doc.SaveFile(filename.toStdString().c_str());
}

void MainWindow::write_alg_config( tinyxml2::XMLDocument &doc, properconfig * proper_config, XMLElement* method_unit_new,QString alg_name)
{

    if(proper_config !=nullptr)
    {
        if(proper_config->source_name ==alg_name)
        {

            QtProperty * prp =  proper_config->idToProperty[QObject::tr("算法类型")];
            int indexID = proper_config->enumManager->value(prp);


            QStringList alg_name_list =proper_config->enumManager->enumNames(prp);
            writer_text_xlm("算法",  alg_name_list[indexID], doc ,method_unit_new );


            prp =  proper_config->idToProperty[QObject::tr("子算法")];
            indexID = proper_config->enumManager->value(prp);
            alg_name_list =proper_config->enumManager->enumNames(prp);
            writer_text_xlm("子算法",  alg_name_list[indexID], doc ,method_unit_new );



            prp =  proper_config->idToProperty[QObject::tr("算法模型文件")];
            QString file_alg= proper_config->fileManager->value(prp);
            writer_text_xlm("算法模型文件",  file_alg, doc ,method_unit_new );


            prp =  proper_config->idToProperty[QObject::tr("选择显卡")];
            indexID = proper_config->enumManager->value(prp);
            alg_name_list =proper_config->enumManager->enumNames(prp);
            writer_text_xlm("显卡",  alg_name_list[indexID], doc ,method_unit_new );



#if USING_PAIJICI_FABU
            prp =  proper_config->idToProperty[QObject::tr("拍照几次才计算")];
            int pzjc_js= proper_config->m_pVarManager->value(prp).toInt();
            cout<<"--------------write_alg_config----------------------"<<pzjc_js<<endl;
            writer_text_xlm("拍照几次才计算", QString::number(pzjc_js), doc ,method_unit_new );

#endif




            prp =  proper_config->idToProperty[QObject::tr("目标")];
            //QString target= proper_config->m_pVarManager->value(prp).toString();
            XMLElement* target_unit_index =  writer_text_xlm("目标",  "", doc ,method_unit_new );

            for(int i = 0; i < proper_config->classnamestr.size(); i++)
            {

                QString name_class_title=QObject::tr("类别名称")+"_" +QString::number(i) +QObject::tr("号");

                QMap<QString, QtProperty *>  class_infor = proper_config->m_classNameToIdToProperty[name_class_title];
                // qDebug()<<name_class_title;
                prp =  class_infor[name_class_title];

                QString target_name= proper_config->m_pVarManager->value(prp).toString();
                XMLElement* neirong_index =  writer_text_xlm(name_class_title, "", doc ,target_unit_index );
                // neirong_index->SetAttribute("name",target_name.toStdString().c_str() );
                writer_text_xlm("名称", target_name, doc ,neirong_index );

                prp =  class_infor[QObject::tr("中文名称")];
                QString target_ch_name= proper_config->m_pVarManager->value(prp).toString();
                writer_text_xlm("中文名称", target_ch_name, doc ,neirong_index );
                //neirong_index->SetAttribute("中文名称",target_ch_name.toStdString().c_str() );

#if ZJ_CHULIAOKOU_HAO


                prp =  class_infor[QObject::tr("出料口顺序号")];
                QString xhh= proper_config->m_pVarManager->value(prp).toString();
                writer_text_xlm("出料口顺序号", xhh, doc ,neirong_index );
                //neirong_index->SetAttribute("中文名称",target_ch_name.toStdString().c_str() );

# endif



                prp =  class_infor[QObject::tr("是否启用")];
                bool bool_name= proper_config->m_pVarManager->value(prp).toBool();
                writer_text_xlm("是否启用", QString::number(bool_name), doc ,neirong_index );
                //neirong_index->SetAttribute("是否启用", QString::number(bool_name).toStdString().c_str() );


                prp = class_infor[QObject::tr("得分最小值")];
                double d_name= proper_config->m_pVarManager->value(prp).toDouble();
                writer_text_xlm("得分最小值", QString::number(d_name), doc ,neirong_index );
                //neirong_index->SetAttribute("得分最小值", QString::number(d_name).toStdString().c_str() );

                prp =  class_infor[QObject::tr("得分最大值")];
                d_name= proper_config->m_pVarManager->value(prp).toDouble();
                writer_text_xlm("得分最大值", QString::number(d_name), doc ,neirong_index );
                //neirong_index->SetAttribute("得分最大值", QString::number(d_name).toStdString().c_str() );

                prp =  class_infor[QObject::tr("目标框最小值")];
                QSizeF s_name= proper_config->m_pVarManager->value(prp).toSizeF();
                XMLElement* qsize_zuixiao= writer_text_xlm("目标框最小值", "", doc ,neirong_index );

                //qsize_zuixiao->SetAttribute("宽", QString::number(s_name.width()).toStdString().c_str() );
                //qsize_zuixiao->SetAttribute("高", QString::number(s_name.height()).toStdString().c_str() );


                writer_text_xlm("Width", QString::number(s_name.width()), doc ,qsize_zuixiao );
                writer_text_xlm("Height", QString::number(s_name.height()), doc ,qsize_zuixiao );


                prp = class_infor[QObject::tr("目标框最大值")];
                s_name= proper_config->m_pVarManager->value(prp).toSizeF();
                qsize_zuixiao= writer_text_xlm("目标框最大值", "", doc ,neirong_index );

                //qsize_zuixiao->SetAttribute("宽", QString::number(s_name.width()).toStdString().c_str() );
                //qsize_zuixiao->SetAttribute("高", QString::number(s_name.height()).toStdString().c_str() );

                writer_text_xlm("Width", QString::number(s_name.width()), doc ,qsize_zuixiao );
                writer_text_xlm("Height", QString::number(s_name.height()), doc ,qsize_zuixiao );


                prp =  class_infor[QObject::tr("面积最小值")];
                d_name= proper_config->m_pVarManager->value(prp).toDouble();
                writer_text_xlm("面积最小值", QString::number(d_name), doc ,neirong_index );


                prp =  class_infor[QObject::tr("面积最大值")];
                d_name= proper_config->m_pVarManager->value(prp).toDouble();
                writer_text_xlm("面积最大值", QString::number(d_name), doc ,neirong_index );


                prp =  class_infor[QObject::tr("宽当量mm")];
                d_name= proper_config->m_pVarManager->value(prp).toDouble();
                writer_text_xlm("宽当量mm", QString::number(d_name), doc ,neirong_index );


                prp =  class_infor[QObject::tr("高当量mm")];
                d_name= proper_config->m_pVarManager->value(prp).toDouble();
                writer_text_xlm("高当量mm", QString::number(d_name), doc ,neirong_index );


                prp =  class_infor[QObject::tr("是否乘以当量")];
                bool_name= proper_config->m_pVarManager->value(prp).toBool();
                writer_text_xlm("是否乘以当量", QString::number(bool_name), doc ,neirong_index );


                prp =  class_infor[QObject::tr("是否启用ROI")];
                bool_name= proper_config->m_pVarManager->value(prp).toBool();
                writer_text_xlm("是否启用ROI", QString::number(bool_name), doc ,neirong_index );

                prp =  class_infor[QObject::tr("感兴趣区域ROI")];
                QRectF r_name= proper_config->m_pVarManager->value(prp).toRectF();
                qsize_zuixiao= writer_text_xlm("感兴趣区域ROI", "", doc ,neirong_index );

                // qsize_zuixiao->SetAttribute("x", QString::number(r_name.x()).toStdString().c_str() );
                //qsize_zuixiao->SetAttribute("y", QString::number(r_name.y()).toStdString().c_str() );

                // qsize_zuixiao->SetAttribute("width", QString::number(r_name.width()).toStdString().c_str() );
                //qsize_zuixiao->SetAttribute("height", QString::number(r_name.height()).toStdString().c_str() );


                writer_text_xlm("X", QString::number(r_name.x()), doc ,qsize_zuixiao );
                writer_text_xlm("Y", QString::number(r_name.y()), doc ,qsize_zuixiao );
                writer_text_xlm("Width", QString::number(r_name.width()), doc ,qsize_zuixiao );
                writer_text_xlm("Height", QString::number(r_name.height()), doc ,qsize_zuixiao );
            }
            //for end

        }
    }

}


void MainWindow::write_camera_config( tinyxml2::XMLDocument &doc, properconfig * proper_config, XMLElement* method_unit_new)
{
    if(proper_config !=nullptr)
    {
        if(proper_config->source_name ==QObject::tr("相机"))
        {
            QtProperty * prp =  proper_config->idToProperty[QObject::tr("相机厂商列表")];
            int indexID = proper_config->enumManager->value(prp);
            // cout<<indexID<<endl;
            writer_text_xlm("相机厂商列表",  proper_config->m_map_index_name_camera_factor[indexID], doc ,method_unit_new );

            prp =  proper_config->idToProperty[QObject::tr("相机列表")];
            indexID = proper_config->enumManager->value(prp);
            writer_text_xlm("相机列表",  QString::fromStdString(proper_config->m_input_str[indexID]), doc ,method_unit_new );


            prp =  proper_config->idToProperty[QObject::tr("相机工作模式")];
            indexID = proper_config->enumManager->value(prp);
            writer_text_xlm("相机工作模式",  QString::number(indexID), doc ,method_unit_new );

            prp =  proper_config->idToProperty[QObject::tr("曝光时间")];
            float findexID = proper_config->m_pVarManager->value(prp).toFloat();
            writer_text_xlm("曝光时间",  QString::number(findexID), doc ,method_unit_new );

            prp =  proper_config->idToProperty[QObject::tr("增益")];
            findexID = proper_config->m_pVarManager->value(prp).toFloat();
            writer_text_xlm("增益",  QString::number(findexID), doc ,method_unit_new );
        }
    }

}


//xml写属性properconfig
void MainWindow::write_properconfig_xml(QString filename, const  QString task_id,const Task_unit_infor & method_unit )
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile(filename.toStdString().c_str());
    XMLElement *root_node=doc.RootElement();//工程就是他们的根节点

    for (XMLElement* currentele = root_node->FirstChildElement(); currentele; currentele = currentele->NextSiblingElement())
    {
        if(currentele->FindAttribute("tsk_id")!=nullptr)
        {
            if( string(currentele->FindAttribute("tsk_id")->Value())==task_id.toStdString())
            {
                XMLElement* method_unit_new = writer_text_xlm( method_unit.src_name, "", doc ,currentele );
                writer_text_xlm("task_pos_index", QString::number(method_unit.task_pos_index), doc ,method_unit_new );
                writer_text_xlm("name_index", QString::number(method_unit.name_index), doc ,method_unit_new );

                method_unit_new->SetAttribute("unit_id",method_unit.str_method_in_tsk_id.toStdString().c_str());
                properconfig * proper_config = method_unit.proper_config;


                if(proper_config->source_name== QObject::tr("相机"))  write_camera_config( doc, proper_config, method_unit_new);
                if(proper_config->source_name== QObject::tr("显示"))  write_camera_config( doc, proper_config, method_unit_new);
                if(proper_config->source_name== QObject::tr("保存图片"))  write_camera_config( doc, proper_config, method_unit_new);
                // if(proper_config->source_name== QObject::tr("输出"))  write_camera_config( doc, proper_config, method_unit_new);

                if(proper_config->source_name== QObject::tr("整体识别"))  write_alg_config( doc, proper_config, method_unit_new,QObject::tr("整体识别"));
                if(proper_config->source_name== QObject::tr("局部识别"))  write_alg_config( doc, proper_config, method_unit_new,QObject::tr("局部识别"));
                if(proper_config->source_name== QObject::tr("字符识别"))  write_alg_config( doc, proper_config, method_unit_new,QObject::tr("字符识别"));
                if(proper_config->source_name== QObject::tr("分割"))  write_alg_config( doc, proper_config, method_unit_new,QObject::tr("分割"));

                break;
            }
        }

    }
    doc.SaveFile(filename.toStdString().c_str());
    cout<<"finish"<<endl;

}

//把加入的节点进行绑定  以便新建属性,更新之后,也要保存属性,这个是为了建立属性页面而定义的函数
//像 输出 功能这类函数,属性和实现都一起,就定义一次就好了
void MainWindow::on_act_bangding_tsk_fangfa_triggered()
{

    QMap<QString ,SCT_DragTreeWidget>::Iterator it=m_current_project.map_task_all.begin();
    while(it!=m_current_project.map_task_all.end())
    {
        //cout<<2<<endl;

        QString tsk_win_str_id = it.key();
        QString task_win_name = it.value().tile_text;
        SCT_DragTreeWidget task_tree =it.value();
        DragTreeWidget *pWidgetls =task_tree.ptr_DragTreeWidget;

        int unit_item_size = pWidgetls->topLevelItemCount();
        QTreeWidgetItem *child =nullptr;

        for (int i = 0; i < unit_item_size; i++)
        {
            // cout<<i<<endl;
            child = pWidgetls->topLevelItem(i);

            //如果方法已经存在，那更新
            if ( it.value().map_unit_method.count(child) > 0)
            {

                if( it.value().map_unit_method[child].src_name != QObject::tr("输出"))
                {
                    if( it.value().map_unit_method[child].proper_config ==nullptr)
                    {
                        //cout<<5<<endl;

                        QDateTime current_date_time = QDateTime::currentDateTime();
                        QString current_date_str =current_date_time.toString("yyyyMMddhhmmsszzz")+ QString::number(i);//yyyy-MM-dd

                        it.value().map_unit_method[child].src_name = child->text(0);
                        it.value().map_unit_method[child].task_id_str =task_tree.task_id_str;
                        it.value().map_unit_method[child].name_index = str2int_src_index_map[child->text(0)];
                        it.value().map_unit_method[child].str_method_in_tsk_id = current_date_str;

                        it.value().map_unit_method[child].top_item =child ;
                        it.value().map_unit_method[child].task_pos_index = i;
                        it.value().map_unit_method[child].proper_config = new properconfig( it.value().map_unit_method[child].src_name,task_tree.task_id_str,current_date_str);
                        connect(it.value().map_unit_method[child].proper_config,&properconfig::send_to_main,this, &MainWindow::onValueChanged);//绑定信号槽，当值改变的时候会发送信号
                        m_mp_methodID_pro[current_date_str] =  it.value().map_unit_method[child].proper_config;

                        write_properconfig_xml(m_str_project_file_path, tsk_win_str_id, it.value().map_unit_method[child]);

                    }else
                    {

                        it.value().map_unit_method[child].task_pos_index = i;
                        rewrite_node_unit_pos_index(m_str_project_file_path,tsk_win_str_id ,it.value().map_unit_method[child].str_method_in_tsk_id,
                                                    QString("task_pos_index"),QString::number(i));


                    }

                }else // 专门为输出写
                {

                    if( it.value().map_unit_method[child].out_result_win ==nullptr)
                    {
                        //cout<<5<<endl;

                        QDateTime current_date_time = QDateTime::currentDateTime();
                        QString current_date_str =current_date_time.toString("yyyyMMddhhmmsszzz")+ QString::number(i);//yyyy-MM-dd

                        it.value().map_unit_method[child].src_name = child->text(0);
                        it.value().map_unit_method[child].task_id_str =task_tree.task_id_str;
                        it.value().map_unit_method[child].name_index = str2int_src_index_map[child->text(0)];
                        it.value().map_unit_method[child].str_method_in_tsk_id = current_date_str;

                        it.value().map_unit_method[child].top_item =child ;
                        it.value().map_unit_method[child].task_pos_index = i;
                        it.value().map_unit_method[child].out_result_win = new OutPutResult( this, it.value().map_unit_method[child].src_name,task_tree.task_id_str,current_date_str);
                        it.value().map_unit_method[child].out_result_win->m_ptr_movectl = &std_mv_ctrl;
                        it.value().map_unit_method[child].src_ptr  =  (OutPutResult*) it.value().map_unit_method[child].out_result_win;
                        //connect(it.value().map_unit_method[child].proper_config,&properconfig::send_to_main,this, &MainWindow::onValueChanged);//绑定信号槽，当值改变的时候会发送信号
                        // m_mp_methodID_pro[current_date_str] =  it.value().map_unit_method[child].proper_config;

                        //write_properconfig_xml(m_str_project_file_path, tsk_win_str_id, it.value().map_unit_method[child]);

                    }else
                    {

                        it.value().map_unit_method[child].task_pos_index = i;
                        rewrite_node_unit_pos_index(m_str_project_file_path,tsk_win_str_id ,it.value().map_unit_method[child].str_method_in_tsk_id,
                                                    QString("task_pos_index"),QString::number(i));


                    }
                }


            }else//方法不存在
            {

                if( it.value().map_unit_method[child].src_name != QObject::tr("输出"))
                {
                    QDateTime current_date_time = QDateTime::currentDateTime();
                    QString current_date_str =current_date_time.toString("yyyyMMddhhmmsszzz")+ QString::number(i);//yyyy-MM-dd

                    Task_unit_infor tsk_unit;
                    tsk_unit.src_name = child->text(0);
                    tsk_unit.task_id_str =task_tree.task_id_str;
                    tsk_unit.name_index = str2int_src_index_map[child->text(0)];
                    tsk_unit.str_method_in_tsk_id = current_date_str;

                    tsk_unit.top_item =child ;
                    tsk_unit.task_pos_index = i;
                    tsk_unit.proper_config = new properconfig( tsk_unit.src_name,task_tree.task_id_str,current_date_str);
                    connect(tsk_unit.proper_config,&properconfig::send_to_main,this, &MainWindow::onValueChanged);//绑定信号槽，当值改变的时候会发送信号
                    m_mp_methodID_pro[current_date_str] =   tsk_unit.proper_config ;

                    it.value().map_unit_method[child] =tsk_unit;
                    write_properconfig_xml(m_str_project_file_path, tsk_win_str_id, it.value().map_unit_method[child] );

                }else //输出
                {

                    QDateTime current_date_time = QDateTime::currentDateTime();
                    QString current_date_str =current_date_time.toString("yyyyMMddhhmmsszzz")+ QString::number(i);//yyyy-MM-dd

                    Task_unit_infor tsk_unit;
                    tsk_unit.src_name = child->text(0);
                    tsk_unit.task_id_str =task_tree.task_id_str;
                    tsk_unit.name_index = str2int_src_index_map[child->text(0)];
                    tsk_unit.str_method_in_tsk_id = current_date_str;

                    tsk_unit.top_item =child ;
                    tsk_unit.task_pos_index = i;
                    tsk_unit.out_result_win = new OutPutResult( this, it.value().map_unit_method[child].src_name,task_tree.task_id_str,current_date_str);
                    tsk_unit.out_result_win->m_ptr_movectl = &std_mv_ctrl;
                    tsk_unit.src_ptr = tsk_unit.out_result_win;

                    //connect(tsk_unit.proper_config,&properconfig::send_to_main,this, &MainWindow::onValueChanged);//绑定信号槽，当值改变的时候会发送信号
                    // m_mp_methodID_pro[current_date_str] =   tsk_unit.proper_config ;

                    it.value().map_unit_method[child] =tsk_unit;
                    //write_properconfig_xml(m_str_project_file_path, tsk_win_str_id, it.value().map_unit_method[child] );

                }
                // cout<<7<<endl;

            }


        }

        it++;

    }


    cout<<"bang  finish!"<<endl;
}

void MainWindow::slot_bang()
{
    on_act_bangding_tsk_fangfa_triggered();
}
int MainWindow::bangding_thread( void * ptr_this)
{
    Sleep(50);
    MainWindow * ptr =( MainWindow *) ptr_this;
    emit ptr->sig_bang();
    return 0;
}
//当拖动一个方法进来的响应
void MainWindow::slot_drop_new_method(QListWidgetItem* items_ptr,QString task_id_str )
{
    qDebug()<<items_ptr->text()<<endl;
    std::thread start_bang(&bangding_thread,this);
    start_bang.detach();

}



bool sort_task_index(Task_unit_infor & t1,Task_unit_infor &t2)
{
    return t1.task_pos_index <t2.task_pos_index;
}


//第一次写，还有一个更新函数

int MainWindow::set_alg_obj_class_propery(  properconfig * input_procfg , Alg_infor  & alg_infor )
{


#if USING_PAIJICI_FABU

    QtProperty * prp_1 =  input_procfg->idToProperty[QObject::tr("拍照几次才计算")];

    int pzjc_js= input_procfg->m_pVarManager->value(prp_1).toInt();


    alg_infor.paizhao_jici = pzjc_js;

#endif


    for(int i = 0; i <  alg_infor.classnamestr.size(); i++)
    {
        OBJ_infor objInfor;

        QString name_class_title=QObject::tr("类别名称")+"_" +QString::number(i) +QObject::tr("号");

        QMap<QString, QtProperty *>  class_infor = input_procfg->m_classNameToIdToProperty[name_class_title];
        QtProperty *   prp =  class_infor[name_class_title];

        //英文名称
        QString target_name= input_procfg->m_pVarManager->value(prp).toString();
        objInfor.class_name = target_name;


        prp =  class_infor[QObject::tr("中文名称")];
        QString target_ch_name= input_procfg->m_pVarManager->value(prp).toString();
        objInfor.class_name_ch = target_ch_name;


#if ZJ_CHULIAOKOU_HAO

        prp =  class_infor[QObject::tr("出料口顺序号")];
        QString xunshuhaochuliao= input_procfg->m_pVarManager->value(prp).toString();
        objInfor.chuliaokou_xunhao = xunshuhaochuliao.toInt();



# endif




        prp =  class_infor[QObject::tr("是否启用")];
        bool bool_name= input_procfg->m_pVarManager->value(prp).toBool();
        objInfor.if_enable = bool_name;

        prp = class_infor[QObject::tr("得分最小值")];
        double d_name= input_procfg->m_pVarManager->value(prp).toDouble();
        objInfor.min_relation_q = d_name;

        prp =  class_infor[QObject::tr("得分最大值")];
        d_name= input_procfg->m_pVarManager->value(prp).toDouble();
        objInfor.max_relation_q = d_name;

        prp =  class_infor[QObject::tr("目标框最小值")];
        QSizeF s_name= input_procfg->m_pVarManager->value(prp).toSizeF();
        objInfor.min_size =s_name;

        prp = class_infor[QObject::tr("目标框最大值")];
        s_name= input_procfg->m_pVarManager->value(prp).toSizeF();
        objInfor.max_size =s_name;

        prp =  class_infor[QObject::tr("面积最小值")];
        d_name= input_procfg->m_pVarManager->value(prp).toDouble();
        objInfor.min_area =d_name;


        prp =  class_infor[QObject::tr("面积最大值")];
        d_name= input_procfg->m_pVarManager->value(prp).toDouble();
        objInfor.max_area =d_name;

        prp =  class_infor[QObject::tr("宽当量mm")];
        d_name= input_procfg->m_pVarManager->value(prp).toDouble();
        objInfor.dangliang_width =d_name;

        prp =  class_infor[QObject::tr("高当量mm")];
        d_name= input_procfg->m_pVarManager->value(prp).toDouble();
        objInfor.dangliang_height =d_name;


        prp =  class_infor[QObject::tr("是否乘以当量")];
        bool_name= input_procfg->m_pVarManager->value(prp).toBool();
        objInfor.if_mul_dangliang =bool_name;


        //目前只有一个感兴趣区域
        prp =  class_infor[QObject::tr("是否启用ROI")];
        bool_name= input_procfg->m_pVarManager->value(prp).toBool();


        objInfor.map_if_enable_roi[0] =bool_name;
        prp =  class_infor[QObject::tr("感兴趣区域ROI")];
        QRectF r_name= input_procfg->m_pVarManager->value(prp).toRectF();
        objInfor.map_roi[0] =r_name;

        objInfor.roi_num =  objInfor.map_if_enable_roi.size();
        alg_infor.m_map_int_obj[i] = objInfor;
        alg_infor.m_map_str_obj[target_name] =objInfor;
    }
    return 0;
}

int MainWindow::set_alg_use_propery(  properconfig * input_procfg ,int model_id, Alg_infor  & alg_infor )
{
    QtProperty * prp =  input_procfg->idToProperty[QObject::tr("算法类型")];

    int indexID = input_procfg->enumManager->value(prp);

    QStringList alg_name_list =input_procfg->enumManager->enumNames(prp);

    //获取算法类型
    alg_infor.alg_type = alg_name_list[indexID];

    prp =  input_procfg->idToProperty[QObject::tr("子算法")];
    indexID = input_procfg->enumManager->value(prp);
    alg_name_list =input_procfg->enumManager->enumNames(prp);
    alg_infor.alg_sub_type = alg_name_list[indexID];


    //显卡名称
    prp =  input_procfg->idToProperty[QObject::tr("选择显卡")];
    indexID = input_procfg->enumManager->value(prp);
    alg_name_list =input_procfg->enumManager->enumNames(prp);
    alg_infor.gpu_str  = alg_name_list[indexID];
    alg_infor.gpu_id = indexID;

    alg_infor.m_alg_id_str = input_procfg->m_unit_id_str;
    alg_infor.m_model_id_int = model_id;

    prp =  input_procfg->idToProperty[QObject::tr("算法模型文件")];
    QString file_alg= input_procfg->fileManager->value(prp);


#if USING_PAIJICI_FABU

    prp =  input_procfg->idToProperty[QObject::tr("拍照几次才计算")];
    int pzjc_js  = input_procfg->m_pVarManager->value(prp).toInt();
    cout<<"--------------set_alg_use_propery----------------------"<<pzjc_js<<endl;
    alg_infor.paizhao_jici = pzjc_js;
#endif




    string fileName = file_alg.toStdString();
    char drive[_MAX_DRIVE] = { 0 };
    char dir[_MAX_DIR] = { 0 };
    char fname[_MAX_FNAME] = { 0 };
    char ext[_MAX_EXT] = { 0 };
    _splitpath(fileName.c_str(), drive, dir, fname, ext);

    string pathFile_root = string(drive) + string(dir);
    alg_infor.alg_model_path  =QString::fromStdString(pathFile_root);

    string classesname =pathFile_root+ string(fname) + string(ext);
    ifstream inFileclasses;
    inFileclasses.open(classesname, ios::in);
    if (!inFileclasses.is_open())//判断文件是否打开
    {
        std::cout << "Error opening inFileclasses" << std::endl;

    }else
    {
        char line[1024] = { 0 };


        if (alg_infor.classnamestr.size() > 0)
            alg_infor.classnamestr.clear();


        while (inFileclasses.getline(line, sizeof(line)))
        {
            std::stringstream word(line);
            string tempstr;
            word >> tempstr;
            alg_infor.classnamestr.push_back(tempstr);
        }

        inFileclasses.close();
    }
    alg_infor.alg_class_file_path =file_alg;


    if(alg_infor.m_map_int_obj.size() >0) alg_infor.m_map_int_obj.clear();
    if(alg_infor.m_map_str_obj.size() >0) alg_infor.m_map_str_obj.clear();

    for(int i = 0; i <  alg_infor.classnamestr.size(); i++)
    {
        OBJ_infor objInfor;

        QString name_class_title=QObject::tr("类别名称")+"_" +QString::number(i) +QObject::tr("号");

        QMap<QString, QtProperty *>  class_infor = input_procfg->m_classNameToIdToProperty[name_class_title];
        prp =  class_infor[name_class_title];

        //英文名称
        QString target_name= input_procfg->m_pVarManager->value(prp).toString();
        objInfor.class_name = target_name;


        prp =  class_infor[QObject::tr("中文名称")];
        QString target_ch_name= input_procfg->m_pVarManager->value(prp).toString();
        objInfor.class_name_ch = target_ch_name;


#if ZJ_CHULIAOKOU_HAO
        prp =  class_infor[QObject::tr("出料口顺序号")];
        int  chuliaokouxunhao= input_procfg->m_pVarManager->value(prp).toString().toInt();
        objInfor.chuliaokou_xunhao = chuliaokouxunhao;

# endif


        prp =  class_infor[QObject::tr("是否启用")];
        bool bool_name= input_procfg->m_pVarManager->value(prp).toBool();
        objInfor.if_enable = bool_name;

        prp = class_infor[QObject::tr("得分最小值")];
        double d_name= input_procfg->m_pVarManager->value(prp).toDouble();
        objInfor.min_relation_q = d_name;

        prp =  class_infor[QObject::tr("得分最大值")];
        d_name= input_procfg->m_pVarManager->value(prp).toDouble();
        objInfor.max_relation_q = d_name;

        prp =  class_infor[QObject::tr("目标框最小值")];
        QSizeF s_name= input_procfg->m_pVarManager->value(prp).toSizeF();
        objInfor.min_size =s_name;

        prp = class_infor[QObject::tr("目标框最大值")];
        s_name= input_procfg->m_pVarManager->value(prp).toSizeF();
        objInfor.max_size =s_name;

        prp =  class_infor[QObject::tr("面积最小值")];
        d_name= input_procfg->m_pVarManager->value(prp).toDouble();
        objInfor.min_area =d_name;


        prp =  class_infor[QObject::tr("面积最大值")];
        d_name= input_procfg->m_pVarManager->value(prp).toDouble();
        objInfor.max_area =d_name;

        prp =  class_infor[QObject::tr("宽当量mm")];
        d_name= input_procfg->m_pVarManager->value(prp).toDouble();
        objInfor.dangliang_width =d_name;

        prp =  class_infor[QObject::tr("高当量mm")];
        d_name= input_procfg->m_pVarManager->value(prp).toDouble();
        objInfor.dangliang_height =d_name;


        prp =  class_infor[QObject::tr("是否乘以当量")];
        bool_name= input_procfg->m_pVarManager->value(prp).toBool();
        objInfor.if_mul_dangliang =bool_name;


        //目前只有一个感兴趣区域
        prp =  class_infor[QObject::tr("是否启用ROI")];
        bool_name= input_procfg->m_pVarManager->value(prp).toBool();


        objInfor.map_if_enable_roi[0] =bool_name;
        prp =  class_infor[QObject::tr("感兴趣区域ROI")];
        QRectF r_name= input_procfg->m_pVarManager->value(prp).toRectF();
        objInfor.map_roi[0] =r_name;

        objInfor.roi_num =  objInfor.map_if_enable_roi.size();
        alg_infor.m_map_int_obj[i] = objInfor;
        alg_infor.m_map_str_obj[target_name] =objInfor;





    }
    return 0;
}

//初始化
void MainWindow::on_ac_config_triggered()
{


    int task_num = 0;
    m_task_connect_top_link.clear();

    QMap<QString ,SCT_DragTreeWidget>::Iterator it=m_current_project.map_task_all.begin();

    int task_num_size = m_current_project.map_task_all.size();

    send_msg_unit_id = "";
    while(it!=m_current_project.map_task_all.end())
    {
        //send_msg_taks_id
        QString tsk_win_str_id = it.key();
        SCT_DragTreeWidget task_tree =it.value();
        DragTreeWidget *pWidgetls =task_tree.ptr_DragTreeWidget;

        int unit_item_size = pWidgetls->topLevelItemCount();
        QTreeWidgetItem *child = nullptr;

        std::vector< Task_unit_infor > v_unit_task;

        QString tsk_win_name =task_tree.tile_text;



        for (int i = 0; i < unit_item_size; i++)
        {
            child = pWidgetls->topLevelItem(i);

            //相机要获取哪个相机，要从配置表里面获取， 算法要获取算法类型 ,如果相机已经打开，那怎么分享信息，如果算法已经被占据，怎么共享
            Task_unit_infor tsk_unit =task_tree.map_unit_method[child];

            if(tsk_unit.src_name==QObject::tr("相机"))
            {

                if(i !=0)
                {
                    QMessageBox::warning(this,tr("初始化"), tr("相机要首位"));
                    return ;
                }

                QtProperty * prp =   tsk_unit.proper_config->idToProperty[QObject::tr("相机厂商列表")];
                int indexID_changshang_ID = tsk_unit.proper_config->enumManager->value(prp);



                prp =  tsk_unit.proper_config->idToProperty[QObject::tr("相机列表")];
                int indexID_name = tsk_unit.proper_config->enumManager->value(prp);
                QString camera_name = QString::fromStdString(tsk_unit.proper_config->m_input_str[indexID_name]);

                tsk_unit.benshen_name =camera_name;

                //相机使用次数
                if( m_cout_camera_use[camera_name] ==0)
                {

                    camera_commen * ptcam=new camera_commen(this, tsk_unit.proper_config);
                    ptcam->open_camera((CameraType)indexID_changshang_ID,camera_name);
                    //这里先默认初始化IO口输出为低电平,后续可以用配置单形式来初始化

                    ptcam->set_state_tr(0, 0);

                    m_cout_camera_use[camera_name]++;

                    prp =  tsk_unit.proper_config->idToProperty[QObject::tr("相机工作模式")];
                    int indexID_mode = tsk_unit.proper_config->enumManager->value(prp);

                    ptcam->set_camera_mode(indexID_mode);
                    m_name_ptr_camera[camera_name] = ptcam;

                    tsk_unit.src_ptr = ptcam;

                }else
                {

                    //tsk_unit.src_ptr_cam = ( camera_commen *) m_name_ptr_camera[camera_name];
                    tsk_unit.src_ptr =  ( camera_commen *) m_name_ptr_camera[camera_name];
                    m_cout_camera_use[camera_name]++;
                }
            }

            if(tsk_unit.src_name==QObject::tr("显示"))
            {
                if(i ==0)
                {
                    QMessageBox::warning(this,tr("初始化"), tr("显示不能首位"));
                    return ;
                }

                tsk_unit.task_win_name =tsk_win_name;

                tsk_unit.src_ptr =  new Form_show_img;
                Form_show_img *  pt_win= (Form_show_img*) tsk_unit.src_ptr ;


                pt_win->m_save_img_root_path = m_save_img_path;
                pt_win->create_save_img_path();
                jilu_Form_show_img_pt[tsk_unit.task_id_str] = pt_win;

                pt_win->wind_id = task_num;

                QGraphicsScene *scene = new QGraphicsScene(this);//最好用new的方法，临时变量有时候不起效
                scene->addWidget(pt_win);
                QGraphicsView *view = new QGraphicsView( scene,this );
                view->setWindowTitle( tsk_unit.task_win_name);

                QMdiSubWindow * show_sub_win = ui->mdiArea->addSubWindow(view);
                show_sub_win->setWindowFlags(Qt::CustomizeWindowHint/*Window*/ | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);

                jilu_win_pt[tsk_unit.task_id_str] = show_sub_win;

                m_name_ptr_show[  tsk_unit.task_win_name] =view;
                m_show_MdiSubWindow[ tsk_unit.task_win_name] = show_sub_win;

                view->show();


            }

            if(tsk_unit.src_name==QObject::tr("局部识别"))
            {
                if(i ==0)
                {
                    QMessageBox::warning(this,tr("初始化"), tr("局部识别不能首位"));
                    return ;
                }


                if( tsk_unit.proper_config->alg_be_call_num[tsk_unit.str_method_in_tsk_id] ==0)
                {

                    // tsk_unit.src_name =tsk_win_name;
                    tsk_unit.task_win_name =tsk_win_name;

                    if(tsk_unit.proper_config != nullptr)
                    {

                        for(int index = 0; index <m_mp_alg_busy_flag.size(); index++)
                        {
                            if(0 == m_mp_alg_busy_flag[index] )
                            {

                                // 每个识别任务,任务id和他本身的单元id都要唯一,因为要根据这两个信息来修饰计算结果queue的 id
                                m_mp_alg_busy_flag[index] =  1;
                                tsk_unit.src_ptr = new jubushibie_form(this, tsk_unit.proper_config, index,tsk_unit.str_method_in_tsk_id);
                                jubushibie_form * temp = ( jubushibie_form *) tsk_unit.src_ptr;

                                temp->source_name =tsk_unit.src_name;
                                temp->m_task_id_str = tsk_unit.task_id_str;
                                temp->m_unit_id_str = tsk_unit.str_method_in_tsk_id;

                                if( (task_num_size-1) ==task_num)
                                {
                                    send_msg_unit_id =  temp->m_unit_id_str;

                                }

                                set_alg_use_propery(  tsk_unit.proper_config ,index, temp->m_alg_infor );
                                temp->alg_intial();

                                m_map_record_ptr_jubushibie[tsk_unit.str_method_in_tsk_id] = (jubushibie_form *) tsk_unit.src_ptr;
                                jubushibie_form * temp_ptr = (jubushibie_form *) tsk_unit.src_ptr;
                                temp_ptr->m_alg_infor.m_model_id_int = index;
                                m_map_jbsb_modelID_prt[index] =(jubushibie_form *) tsk_unit.src_ptr;
                                tsk_unit.proper_config->alg_be_call_num[tsk_unit.str_method_in_tsk_id] = 1;


                                //记录每个任务的识别模块的入口指针
                                m_jiebushibie_ptr[temp->m_unit_id_str] = temp_ptr;

                                break;
                            }

                        }


                    }

                }else//包含模型更改
                {
                    if(  m_map_record_ptr_jubushibie[tsk_unit.str_method_in_tsk_id] != nullptr &&  tsk_unit.proper_config->alg_be_call_num[tsk_unit.str_method_in_tsk_id]==1)
                    {
                        tsk_unit.task_win_name =tsk_win_name;

                        jubushibie_form * temp_old = ( jubushibie_form *) m_map_record_ptr_jubushibie[tsk_unit.str_method_in_tsk_id];


                        int ziyuan_id_index =  temp_old->m_alg_infor.m_model_id_int;
                        if(temp_old !=nullptr)
                        {
                            //temp_old->release_alg();
                            //temp_old->m_alg_be_call_num = 0;
                            delete temp_old;
                        }

                        Del_Alg_jubu_Unit(tsk_unit.str_method_in_tsk_id , m_map_record_ptr_jubushibie);

                        tsk_unit.src_ptr = new jubushibie_form(this, tsk_unit.proper_config, ziyuan_id_index,tsk_unit.str_method_in_tsk_id );

                        jubushibie_form * temp = ( jubushibie_form *) tsk_unit.src_ptr;

                        temp->source_name =tsk_unit.src_name;
                        temp->m_task_id_str = tsk_unit.task_id_str;
                        temp->m_unit_id_str = tsk_unit.str_method_in_tsk_id;

                        //最后一个任务 识别完成后,发送踢料指令
                        if( (task_num_size-1) ==task_num)
                        {
                            send_msg_unit_id =  temp->m_unit_id_str;

                        }

                        set_alg_use_propery(  tsk_unit.proper_config ,ziyuan_id_index, temp->m_alg_infor );
                        temp->alg_intial();

                        m_map_record_ptr_jubushibie[tsk_unit.str_method_in_tsk_id] = (jubushibie_form *) tsk_unit.src_ptr;
                        jubushibie_form * temp_ptr = (jubushibie_form *) tsk_unit.src_ptr;
                        temp_ptr->m_alg_infor.m_model_id_int = ziyuan_id_index;
                        m_map_jbsb_modelID_prt[ziyuan_id_index] =(jubushibie_form *) tsk_unit.src_ptr;
                        tsk_unit.proper_config->alg_be_call_num[tsk_unit.str_method_in_tsk_id] = 1;

                        //记录每个任务的识别模块的入口指针
                        m_jiebushibie_ptr[temp->m_unit_id_str] = temp_ptr;

                    }
                }

            }


            if(tsk_unit.src_name==QObject::tr("整体识别"))
            {
                if(i ==0)
                {
                    QMessageBox::warning(this,tr("初始化"), tr("整体识别不能首位"));
                    return ;
                }
                // tsk_unit.src_name =tsk_win_name;
                tsk_unit.task_win_name =tsk_win_name;
                // tsk_unit.zhengtishibie_ptr = new zhengtishibie_form(this, tsk_unit.proper_config);
                tsk_unit.src_ptr = new zhengtishibie_form(this, tsk_unit.proper_config);
            }

            if(tsk_unit.src_name==QObject::tr("输出"))
            {
                if(i ==0)
                {
                    QMessageBox::warning(this,tr("初始化"), tr("输出不能首位"));
                    return ;
                }

                //最后一个任务 识别完成后,发送踢料指令
                if( (task_num_size-1) ==task_num)
                {
                    tsk_unit.out_result_win->tiliaogongwei  = send_msg_unit_id;
                }
            }

            v_unit_task.push_back(tsk_unit);
        }
        m_task_connect_top_link.push_back(v_unit_task);
        it++;

        task_num++;
    }

    // 找到输出单元
    int have_shuchu = 0;
    int  jil_x = 0, jil_y = 0;
    for(int i = 0 ; i < m_task_connect_top_link.size(); i++)
    {
        std::vector< Task_unit_infor > vct_unit_task =m_task_connect_top_link[i] ;
        for(int k = 0 ; k < vct_unit_task.size(); k++)
        {
            if(vct_unit_task[k].src_name==QObject::tr("输出"))
            {
                jil_x = i;
                jil_y = k;
                have_shuchu = 1;
            }
        }
    }

    int  if_use_cam_com = 1;
    if(if_use_cam_com && have_shuchu)
    {
        //把输出单元 的相机指针 指向当前的相机
        for(int i = 0 ; i < m_task_connect_top_link.size(); i++)
        {
            std::vector< Task_unit_infor > vct_unit_task =m_task_connect_top_link[i] ;
            for(int k = 0 ; k < vct_unit_task.size(); k++)
            {
                if(vct_unit_task[k].src_name==QObject::tr("相机"))
                {
                    m_task_connect_top_link[jil_x][jil_y].out_result_win->cam_prt[vct_unit_task[k].benshen_name] = (camera_commen *)vct_unit_task[k].src_ptr;
                    m_task_connect_top_link[jil_x][jil_y].out_result_win->cam_prt_str[(camera_commen *)vct_unit_task[k].src_ptr]= vct_unit_task[k].benshen_name;
                }
            }
        }
    }


    // QMessageBox::warning(this,tr("初始化"), tr("完成"));

    ui->mdiArea->tileSubWindows();

    return;
}



void MainWindow::on_act_stop_task_triggered()
{
    //mvctrl.on_pushButton_tingzhi_ceshi_clicked();
    //停止
    std_mv_ctrl.gei_main_diaoyong_tingzhi();

}

// 急停
void MainWindow::on_act_stop_warn_triggered()
{
    // 急停
    // mvctrl.on_Stop1_clicked();
}

//回原点
void MainWindow::on_act_huiyuandian_triggered()
{
    // mvctrl.on_pushButton_huiyuandian_clicked();
}

//软启动检查
void MainWindow::on_ac_run_task_triggered()
{
    cout<<"yuanjian  anniu"<<endl;

    // mvctrl.on_pushButton_ceshi_clicked();

    std_mv_ctrl.gei_main_diaoyong_qidong();

    return;
}


void MainWindow::on_action_AI_triggered()
{


    // 这里根据相机建立显示窗口
    if(m_task_connect_top_link.size()>0)
    {
        for(int i = 0; i < m_task_connect_top_link.size(); i++ )
        {

            QString name_benti ="";
            QMap<int, camera_commen *> temp_cam;
            QMap<int, Form_show_img *>temp_form;

            std::vector< Task_unit_infor > unit_task = m_task_connect_top_link[i];
            for(int k = 0; k <unit_task.size(); k = k++ )
            {

                if(unit_task[k].src_name=="相机"&& (k==0))
                {
                    camera_commen*   ptcam =( camera_commen* )unit_task[k].src_ptr ;
                    name_benti = unit_task[k].benshen_name;

                }
                if(unit_task[k].src_name=="显示")
                {
                    Form_show_img *  pt_win= (Form_show_img*)unit_task[k].src_ptr;

                    QString tiltle =  m_show_MdiSubWindow[ unit_task[k ].task_win_name]->windowTitle() + "  "+ name_benti ;
                    m_show_MdiSubWindow[ unit_task[k].task_win_name]->setWindowTitle(tiltle);

                }

            }
        }
    }


    if(m_task_connect_top_link.size()>0)
    {
        for(int i = 0; i < m_task_connect_top_link.size(); i++ )
        {


            std::vector< Task_unit_infor > unit_task = m_task_connect_top_link[i];

            int sizex =unit_task.size();
            for(int k = 0; k <unit_task.size()-1; k = k++ )
            {
                if(unit_task[k].src_name=="相机" && k == 0)//第一轮是相机
                {

                    //struct_connect_cameraToOther( unit_task[k+1].src_name ,unit_task[k].src_ptr ,  unit_task[k+1].src_ptr);

                    if(unit_task[k+1].src_name=="显示")
                    {

                        QObject::connect( (camera_commen *)unit_task[k].src_ptr,SIGNAL(send_mat_to_main(cv::Mat, int)),  (Form_show_img  *) unit_task[k+1].src_ptr,SLOT(xianshi(cv::Mat, int)), Qt::DirectConnection);


                        Form_show_img *  pt_win= (Form_show_img*)unit_task[k+1].src_ptr;

                        pt_win->camera_name = unit_task[k].benshen_name;
                    }

                    if(unit_task[k+1].src_name=="整体识别")
                    {
                        struct_connect_cameraToOther( unit_task[k+1].src_name ,(camera_commen *)unit_task[k].src_ptr,  (zhengtishibie_form * )unit_task[k+1].src_ptr);
                    }

                    if(unit_task[k+1].src_name=="局部识别")
                    {
                        // struct_connect_cameraToOther( unit_task[k+1].src_name ,(camera_commen *)unit_task[k].src_ptr ,  (jubushibie_form *)unit_task[k+1].src_ptr);

                        QObject::connect( (camera_commen *)unit_task[k].src_ptr  ,SIGNAL(send_mat_to_main(cv::Mat, int)),
                                          (jubushibie_form *)unit_task[k+1].src_ptr,SLOT(pre_unix_input(cv::Mat ,int)), Qt::DirectConnection);

                        jubushibie_form * xx = (jubushibie_form *)(unit_task[k+1].src_ptr);
                        xx->camera_name =  unit_task[k].benshen_name;
                    }


                    if(unit_task[k+1].src_name=="字符识别")
                    {

                    }

                    if(unit_task[k+1].src_name=="分割")
                    {

                    }

                    if(unit_task[k+1].src_name=="输出")
                    {

                    }

                }else if(unit_task[k].src_name=="显示")//第二轮是显示
                {
                    if(unit_task[k+1].src_name=="输出")
                    {

                        //  QObject::connect( (Form_show_img *)unit_task[k].src_ptr  ,SIGNAL(send_mat_to_main(cv::Mat, int)),
                        //  (OutPutResult *)unit_task[k+1].src_ptr,SLOT(pre_unix_input(cv::Mat ,int)), Qt::DirectConnection);

                    }
                }else if(unit_task[k].src_name=="整体识别")//第二轮是整体识别
                {

                    //1
                    if(unit_task[k+1].src_name=="显示" )
                    {

                        struct_connect_cameraToOther( unit_task[k+1].src_name ,(zhengtishibie_form * )unit_task[k].src_ptr ,  (Form_show_img   *)unit_task[k+1].src_ptr);
                    }
                    //2
                    if(unit_task[k+1].src_name=="整体识别")
                    {

                    }
                    //3
                    if(unit_task[k+1].src_name=="局部识别")
                    {

                    }

                    //4
                    if(unit_task[k+1].src_name=="字符识别")
                    {

                    }
                    //5
                    if(unit_task[k+1].src_name=="分割")
                    {

                    }
                    //6
                    if(unit_task[k+1].src_name=="输出")
                    {

                    }

                }else if(unit_task[k].src_name=="局部识别")//第二轮是局部
                {

                    //1
                    if(unit_task[k+1].src_name=="显示" )
                    {

                        // struct_connect_cameraToOther( unit_task[k+1].src_name ,(jubushibie_form   *)unit_task[k].src_ptr , (Form_show_img   *) unit_task[k+1].src_ptr);

                        QObject::connect( (jubushibie_form *)unit_task[k].src_ptr  ,SIGNAL(send_result_next(cv::Mat, Alg_infor, std::vector<tr_s_resultinfo>)),
                                          (Form_show_img   *)unit_task[k+1].src_ptr,SLOT(pre_unix_input(cv::Mat , Alg_infor , std::vector<tr_s_resultinfo>)), Qt::DirectConnection);


                        Form_show_img *  pt_win= (Form_show_img*)unit_task[k+1].src_ptr;


                        jubushibie_form * xxx = (jubushibie_form *)unit_task[k].src_ptr;
                        pt_win->camera_name =xxx->camera_name ;


                        if( (k+2) <= (sizex -1))
                        {

                            if(unit_task[k+2].src_name=="输出" )
                            {/*
//                                QObject::connect( (jubushibie_form *)unit_task[k].src_ptr ,SIGNAL(send_to_tongji_unit(QString ,int , Map_resultinfo & , std::vector<tr_s_resultinfo>)),
//                                                  (OutPutResult *)unit_task[k+2].src_ptr,SLOT(pre_alg_unix_input(QString ,int , Map_resultinfo & , std::vector<tr_s_resultinfo>)), Qt::DirectConnection);*/


                                QObject::connect( (jubushibie_form *)unit_task[k].src_ptr ,SIGNAL(send_to_tongji_unit_queue(queue<for_queue_resultinfo> &  , std::vector<tr_s_resultinfo> )),
                                                  (OutPutResult *)unit_task[k+2].src_ptr,SLOT(pre_alg_unix_input_queue(queue<for_queue_resultinfo> &  , std::vector<tr_s_resultinfo>  )), Qt::DirectConnection);


                                QObject::connect(  (OutPutResult *)unit_task[k+2].src_ptr ,SIGNAL( send_Fanhuijieguo(Fanhuijieguo )),
                                        this,SLOT(get_Fanhuijieguo(Fanhuijieguo )), Qt::DirectConnection);//返回结果给统计用

                                QObject::connect(  this ,SIGNAL( set_main_Fanhuijieguo(Fanhuijieguo )),
                                                   (OutPutResult *)unit_task[k+2].src_ptr,SLOT(set_out_Fanhuijieguo(Fanhuijieguo  )), Qt::DirectConnection);
                            }
                        }

                    }
                    //2
                    if(unit_task[k+1].src_name=="整体识别")
                    {

                    }
                    //3
                    if(unit_task[k+1].src_name=="局部识别")
                    {

                    }

                    //4
                    if(unit_task[k+1].src_name=="字符识别")
                    {

                    }
                    //5
                    if(unit_task[k+1].src_name=="分割")
                    {

                    }
                    //6
                    if(unit_task[k+1].src_name=="输出")
                    {



                        QObject::connect( (jubushibie_form *)unit_task[k].src_ptr ,SIGNAL(send_to_tongji_unit_queue(queue<for_queue_resultinfo> &  , std::vector<tr_s_resultinfo> )),
                                          (OutPutResult *)unit_task[k+1].src_ptr,SLOT(pre_alg_unix_input_queue(queue<for_queue_resultinfo> &  , std::vector<tr_s_resultinfo>  )), Qt::DirectConnection);






                        QObject::connect(  (OutPutResult *)unit_task[k+1].src_ptr ,SIGNAL( send_Fanhuijieguo(Fanhuijieguo )),
                                this,SLOT(get_Fanhuijieguo(Fanhuijieguo  )), Qt::DirectConnection);

                        QObject::connect(  this ,SIGNAL( set_main_Fanhuijieguo(Fanhuijieguo )),
                                           (OutPutResult *)unit_task[k+1].src_ptr,SLOT(set_out_Fanhuijieguo(Fanhuijieguo )), Qt::DirectConnection);


                        if( (k+2) <= (sizex -1))
                        {

                            if(unit_task[k+2].src_name=="显示" )
                            {

                                QObject::connect( (jubushibie_form *)unit_task[k].src_ptr  ,SIGNAL(send_result_next(cv::Mat, Alg_infor, std::vector<tr_s_resultinfo>)),
                                                  (Form_show_img   *)unit_task[k+2].src_ptr,SLOT(pre_unix_input(cv::Mat , Alg_infor , std::vector<tr_s_resultinfo>)), Qt::DirectConnection);


                                Form_show_img *  pt_win= (Form_show_img*)unit_task[k+2].src_ptr;

                                jubushibie_form * xxx = (jubushibie_form *)unit_task[k].src_ptr;
                                pt_win->camera_name =xxx->camera_name ;
                            }
                        }

                    }

                }

            }
        }
    }

}





void MainWindow::on_action_uninstall_AI_triggered()
{
    if(m_task_connect_top_link.size()>0)
    {
        for(int i = 0; i < m_task_connect_top_link.size(); i++ )
        {
            std::vector< Task_unit_infor > v_unit_task = m_task_connect_top_link[i];

            for(int k = 0; k <v_unit_task.size(); k = k++ )
            {


                if(v_unit_task[k].src_name==QObject::tr("相机"))
                {

                    if( m_name_ptr_camera[v_unit_task[k].benshen_name] !=nullptr)
                    {

                        camera_commen*  ptcam=( camera_commen* )v_unit_task[k].src_ptr ;
                        QObject::disconnect(ptcam,SIGNAL(send_mat_to_main(cv::Mat, int)),nullptr, nullptr);

                        camera_commen*   vvv =m_name_ptr_camera[v_unit_task[k].benshen_name];
                        delete   vvv;
                        v_unit_task[k].src_ptr = nullptr;

                        m_name_ptr_camera[v_unit_task[k].benshen_name] = nullptr;
                        m_cout_camera_use[v_unit_task[k].benshen_name]  = 0;
                        qDebug()<<v_unit_task[k].benshen_name;
                    }

                }


                if( v_unit_task[k].src_name==QObject::tr("显示"))
                {


                    if(m_show_MdiSubWindow[  v_unit_task[k].task_win_name] !=nullptr)
                    {

                        ui->mdiArea->setActiveSubWindow( m_show_MdiSubWindow[  v_unit_task[k].task_win_name]);
                        ui->mdiArea->removeSubWindow(  m_name_ptr_show[ v_unit_task[k].task_win_name]);
                        ui->mdiArea->closeActiveSubWindow();
                    }

                }

                // 停止运行解除算法绑定
                if( v_unit_task[k].src_name==QObject::tr("局部识别"))
                {

                    QString unit_id_str = v_unit_task[k].str_method_in_tsk_id;

                    if (m_map_record_ptr_jubushibie.count(unit_id_str) > 0)
                    {
                        if(m_map_record_ptr_jubushibie[unit_id_str]!=nullptr)
                        {

                            jubushibie_form * temp_ptr = (jubushibie_form * ) m_map_record_ptr_jubushibie[unit_id_str];

                            //disconnect 要加解绑信号

                            QObject::disconnect(temp_ptr,SIGNAL( send_result_next(cv::Mat, Alg_infor ,std::vector<tr_s_resultinfo>)),nullptr, nullptr);
                        }
                    }



                }
            }

            v_unit_task.clear();
        }
    }

    m_task_connect_top_link.clear();
}

//一键加载
void  MainWindow::one_key_intial_device(int flag)
{
    //打开工程文件
    //    QString path =QCoreApplication::applicationDirPath();
    //    std::string exe_filepath = path.toStdString();
    //    dir_user =exe_filepath+ "/user";

    //    string  tempp =dir_user + "/default.xml";


    //QString path =QString("./user/" )+"setting.ini";
    //QSettings* configIni = new QSettings(path, QSettings::IniFormat);
    //configIni->setValue("setting/path",  m_save_img_path);

    // delete configIni;

    QString path =QCoreApplication::applicationDirPath();
    std::string exe_filepath = path.toStdString();
    dir_user =exe_filepath+ "/user";

    QString path_set =QString::fromStdString(dir_user)+"/setting.ini";
    QSettings* configIni = new QSettings(path_set, QSettings::IniFormat);

    m_moren_wenjian_name = configIni->value("setting/m_moren_wenjian_name").toString();
    delete configIni;


    //打开工程文件

    string  tempp =dir_user + "/" +m_moren_wenjian_name.toStdString() ;
    string  srcxmlFile =tempp;

    QString xmlFileName =QString::fromStdString(tempp);
    string fileName = srcxmlFile;
    char drive[_MAX_DRIVE] = { 0 };
    char dir[_MAX_DIR] = { 0 };
    char fname[_MAX_FNAME] = { 0 };
    char ext[_MAX_EXT] = { 0 };
    _splitpath(fileName.c_str(), drive, dir, fname, ext);
    string file_format = string(ext);

    string pathFile_root = string(drive) + string(dir);

    m_str_project_file_path =xmlFileName;//工程文件名路径


    m_current_project.str_project_name =QString::fromStdString(string(fname));

    m_current_project.str_project_file_name =QString::fromStdString(string(fname) +string(".xml"));



    m_str_project_file_name =QString::fromStdString(fname) + QString::fromStdString(file_format) ;//工程文件名称

    open_project_xml(xmlFileName);

    if(ui->listWidget_camera->count()>0) ui->listWidget_camera->clear();

    vector<string> inputstr;

#if USING_MAIDEWEISHI

    hk_objxx.list_camera(inputstr);
#else

    haik_cam hk_obj;
    hk_obj.list_camera(inputstr);
#endif


    for(int i = 0; i < inputstr.size();i++)
    {
        m_cout_camera_use[QString::fromStdString(inputstr[i])] = 0;

        ui->listWidget_camera->addItem ( QString::fromStdString(inputstr[i]) );
        // ui->listWidget_camera->

    }

    //初始化
    on_ac_config_triggered();

    Sleep(1000);
    //载入AI
    on_action_AI_triggered();

}

//相机到其他方法接口的连接，是否有更好的实现
template< class T1, class T2>
void MainWindow::struct_connect_cameraToOther( QString src_name ,T1 *pre_ptr , T2 *next_ptr)
{

    for(int i = 0; i < m_all_unit.size();i++)
    {
        if(src_name==m_all_unit[i] )
        {
            QObject::connect( pre_ptr,SIGNAL(send_mat_to_main(cv::Mat, int)),  next_ptr,SLOT(xianshi(cv::Mat, int)), Qt::DirectConnection);
        }
    }
}

template <class Unit_type1, class Unit_type2>
void  MainWindow::record_unit_ptr(QString id1,Unit_type1 *factory1, QString id2,Unit_type2 *factory2)
{
    QObject::connect(factory1,SIGNAL(send_mat_to_main(cv::Mat, int)),factory2,SLOT(xianshi(cv::Mat, int)), Qt::DirectConnection);
}

void  MainWindow::connect_acord_type(Task_unit_infor front,Task_unit_infor below)
{


}

void MainWindow::on_ac_save_all_task_triggered()
{



}


void MainWindow::on_act_test_fnct_triggered()
{
    // QMap<QString , jubushibie_form *>::iterator iter = m_map_record_ptr_jubushibie.begin();
    if( m_map_record_ptr_jubushibie.count(current_select_unit_id))
    {

        jubushibie_form * tem =  m_map_record_ptr_jubushibie[current_select_unit_id];
        tem->show();
    }


}

void MainWindow::chuli_jieguo_thread(void * ptr)
{
    MainWindow * pt = ( MainWindow *) ptr;
    while ( chuli_qidongxiancheng_flag)
    {
        if(!m_have_result.empty())
        {
            int state_flag = 1;

            //遍历几个任务
            QHash<QString , jubushibie_form * >::const_iterator it_mx = m_jiebushibie_ptr.constBegin();// 有几个测量对象


            while (it_mx != m_jiebushibie_ptr.constEnd())
            {

                QString un_id =  it_mx.key();


                if( m_jiebushibie_ptr[un_id] != nullptr )
                {

                    jubushibie_form *  x = m_jiebushibie_ptr[un_id];
                    int paizhaojici =  x->m_alg_infor.paizhao_jici;

                    queue< for_queue_resultinfo> qv = x->m_shibie[un_id];

                    if(qv.size() >0)
                    {

                        for_queue_resultinfo xq = x->m_shibie[un_id].front();
                        if(xq.m_reslt_state_flag==E_HAVE_OBJ)
                        {
                            state_flag =E_HAVE_OBJ;

                        }
                        x->m_shibie[un_id].pop();

                    }

                }

                ++it_mx;
            }

            lingshi_obj.zongshu +=1;

            if(state_flag==E_HAVE_OBJ) //NG
            {
                //发送提料信号,这里有相机的选择 ON和OK 分别发送
                cout<<"result NG------------------>"<<endl;
                std::vector< tr_s_resultinfo> input_vst ;
                std_mv_ctrl.chulijieguo_hanshu_vst( 2,  input_vst,1);

                lingshi_obj.state = 0;
                lingshi_obj.ng_shu +=1;

            }else //OK
            {

                cout<<"result OK------------------>"<<endl;
                lingshi_obj.state = 1;
                lingshi_obj.ok_shu +=1;
                std::vector< tr_s_resultinfo> input_vst ;
                std_mv_ctrl.chulijieguo_hanshu_vst( 1,  input_vst,1);

            }

            // emit set_main_Fanhuijieguo(lingshi_obj);
            pt->oper_jieguo();

            m_have_result.pop();

        }


    }
}

void ::MainWindow::oper_jieguo()
{
    emit set_main_Fanhuijieguo(lingshi_obj);
}
void MainWindow::get_Fanhuijieguo(Fanhuijieguo obj)
{
    m_have_result.push(1);

    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy-MM-dd-hh-mm");

    if(flag_shangyici_shijian == 0)
    {
        flag_shangyici_shijian = 1;
        shangyici_shijian = current_date;
    }
    pt_resul_form->m_tongjifenzhong[current_date]+=1;


    if(shangyici_shijian !=current_date)
    {
        flag_shangyici_shijian = 0;

    }




}

//int  tongji_shu = 0;
void MainWindow::set_result_data(Fanhuijieguo  obj)
{
    pt_resul_form->set_zongshu(obj.zongshu);
    pt_resul_form->set_buliangshu(obj.ng_shu);
    pt_resul_form->set_liangpingshu(obj.ok_shu);


    ui->lineEdit_zongshu->setText(QString::number(obj.zongshu));
    ui->lineEdit_OK_shu->setText(QString::number(obj.ok_shu));
    ui->lineEdit_NG_shu->setText(QString::number(obj.ng_shu));
    if( obj.zongshu > 0)
    {
        QString llv =QString::number( (double)obj.ok_shu/ (double)obj.zongshu,'f',3);
        ui->lineEdit_lianglv->setText(llv);


        pt_resul_form->set_lianglv((double)obj.ok_shu/ (double)obj.zongshu);
        pt_resul_form->set_bulianglv((double)obj.ng_shu/ (double)obj.zongshu);


    }
    if(obj.state ==1)
    {
        pt_resul_form->set_label_ng_ok(1);
    }else if(obj.state ==0)
    {
        pt_resul_form->set_label_ng_ok(0);
    }

}
void MainWindow::on_pushButton_ceshi_clicked()
{
    //pt_resul_form
    pt_resul_form->set_zongshu(100);
    pt_resul_form->set_label_ng_ok(3);



}

void MainWindow::on_pushButton_qingling_clicked()
{
    Fanhuijieguo obj;
    obj.ng_shu = 0;
    obj.ok_shu = 0;
    obj.zongshu = 0;
    obj.lianglv = 0;


    emit set_main_Fanhuijieguo(obj);
    pt_resul_form->set_label_ng_ok(3);

}

// 清空相机拍照队列和踢料队列
void MainWindow::on_pushButton_zhenqingling_clicked()
{
    //pushButton_zhenqingling
    //mvctrl.clear_queue();


    QHash<QString , jubushibie_form * >::const_iterator it_mx = m_jiebushibie_ptr.constBegin();// 有几个测量对象
    while (it_mx != m_jiebushibie_ptr.constEnd())
    {

        QString un_id =  it_mx.key();
        if( m_jiebushibie_ptr[un_id] != nullptr)
        {
            jubushibie_form *  x = m_jiebushibie_ptr[un_id];


            // m_jiebushibie_ptr[un_id]->m_shibie[un_id].clear();
            // m_jiebushibie_ptr[un_id]->m_shibie_jieguo[un_id].clear();

            while (!m_jiebushibie_ptr[un_id]->m_shibie[un_id].empty()) m_jiebushibie_ptr[un_id]->m_shibie[un_id].pop();
            while (!m_jiebushibie_ptr[un_id]->m_shibie_jieguo[un_id].empty()) m_jiebushibie_ptr[un_id]->m_shibie_jieguo[un_id].pop();


            m_jiebushibie_ptr[un_id]->clear_img_queue();


        }

        ++it_mx;
    }



    QMap<QString , jubushibie_form *>::iterator iter = m_map_record_ptr_jubushibie.begin();
    while (iter != m_map_record_ptr_jubushibie.end())
    {

        //  qDebug() << "Iterator " << iter.key() << ":" << iter.value(); //
        jubushibie_form * tem = iter.value();

        tem->qingkong_queue();
        iter++;
    }

    if(m_task_connect_top_link.size()>0)
    {
        for(int i = 0; i < m_task_connect_top_link.size(); i++ )
        {
            std::vector< Task_unit_infor > unit_task = m_task_connect_top_link[i];

            int sizex =unit_task.size();
            for(int k = 0; k <unit_task.size(); k ++ )
            {
                if(unit_task[k].src_name=="输出")//
                {

                    OutPutResult * ptr_x= (OutPutResult *)unit_task[k].src_ptr;
                    ptr_x ->clear_output_infor_queue();
                }

            }
        }
    }

}

void MainWindow::on_act_move_ctrl_triggered()
{
    //move_ctrl ct;
    //ct.show();
    // mvctrl.show();
    std_mv_ctrl.show();
}


//默认模型设置
void MainWindow::on_act_morenshezhi_triggered()
{

}



void MainWindow::on_action_ceshicccl_triggered()
{

    m_cccl_obj.show();
}

void MainWindow::on_action_set_save_img_path_triggered()
{
    //QString m_save_img_path="./save_image";

    m_save_img_path = QFileDialog::getExistingDirectory(
                this, "choose save image  Directory",
                "/");

    if (m_save_img_path.isEmpty())
    {
        return;
    }
    else
    {


        qDebug() << "srcDirPath=" << m_save_img_path;


        QString path =QCoreApplication::applicationDirPath();
        std::string exe_filepath = path.toStdString();
        dir_user =exe_filepath+ "/user";

        QString path_set =QString::fromStdString(dir_user)+"/setting.ini";

        QSettings* configIni = new QSettings(path_set, QSettings::IniFormat);
        configIni->setValue("setting/path",  m_save_img_path);

        delete configIni;

        m_save_img_path += "/";
        //std_mv_ctrl.tuxiang_baocun_lj = m_save_img_path;

        if(!jilu_Form_show_img_pt.empty())
        {
            map<QString,Form_show_img *>::iterator it=jilu_Form_show_img_pt.begin();
            while(it !=jilu_Form_show_img_pt.end())
            {
                it->second->m_save_img_root_path = m_save_img_path;
                it->second->create_save_img_path();
                it++;
            }
        }
    }
}

void MainWindow::on_ac_ceshi_triggered()
{

}


