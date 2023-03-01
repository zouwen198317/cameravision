#include "outputresult.h"
#include "ui_outputresult.h"


#include<QDebug>

extern map<QString,queue<for_queue_resultinfo>> map_q_result_info;
OutPutResult::OutPutResult(QWidget *parent,QString source_name ,QString task_id_str  , QString unit_id_str ) :
    QMainWindow(parent),
    ui(new Ui::OutPutResult)
{
    ui->setupUi(this);


    m_source_name =source_name;
    m_task_id_str = task_id_str;
    m_unit_id_str= unit_id_str;

    if(!inputstr.empty()) inputstr.clear();




#if USING_MAIDEWEISHI
    //mdws_cam hk_objxx;
    hk_objxx.list_camera(inputstr);
#else

    haik_cam hk_obj;
    hk_obj.list_camera(inputstr);
#endif


    ui->checkBox_is_move_car->setChecked(true);

    int cols = 6;
    ui->tableWidget->setColumnCount(cols);
    ui->tableWidget->setRowCount(inputstr.size());
    QHeaderView* headerView = ui->tableWidget->horizontalHeader();
    headerView->setDefaultAlignment(Qt::AlignLeft);     //表头字 左对齐
    headerView->setStretchLastSection(true);
    ui->tableWidget->setShowGrid(true);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);  //可多选 单选
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);   //设置选择行为时每次选择一行
    //ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);    //设置编辑
    ui->tableWidget->setStyleSheet("selection-background-color:lightblue;");//设置选中背景色
    // ui->tableWidget->setColumnWidth(0,100);        //设置第一列的宽度
    ui->tableWidget->setColumnWidth(1,50);       //设置第二列的宽度
    ui->tableWidget->setColumnWidth(2,50);

    //ui->tableWidget->setRowCount(20);
    QStringList listcam;
    listcam.append(QString::fromLocal8Bit("相机"));
    listcam.append(QString::fromLocal8Bit("启用"));
    listcam.append(QString::fromLocal8Bit("电平"));
    listcam.append(QString::fromLocal8Bit("内容"));
    listcam.append(QString::fromLocal8Bit("延时"));
    ui->tableWidget->setHorizontalHeaderLabels(listcam);

    for(int i = 0; i < inputstr.size();i++)
    {
        //cam_model->setItem(i,0,new QStandardItem(QString::fromStdString(inputstr[i])));
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(QString::fromStdString(inputstr[i])));
        xiangmu_cols[QString::fromLocal8Bit("相机")] =0;

        // dev_str_mem.cam_name_0_i[i] = QString::fromStdString(inputstr[i]);

        //相机名称
        //head_infor[i].name =  QString::fromStdString(inputstr[i]);
        QString name_dev = QString::fromStdString(inputstr[i]);
        rec_name[i] =name_dev;

        //高低电平设置
        head_infor[name_dev].combox_dianpin = new QComboBox(); // 下拉选择框控件
        head_infor[name_dev].combox_dianpin->addItem(QString::fromLocal8Bit("低"));
        head_infor[name_dev].combox_dianpin->addItem(QString::fromLocal8Bit("高"));
        ui->tableWidget->setCellWidget(i, 2, (QWidget*)head_infor[name_dev].combox_dianpin);

        xiangmu_cols[QString::fromLocal8Bit("电平")] =2;


        connect( head_infor[name_dev].combox_dianpin, SIGNAL(currentIndexChanged(QString)), this, SLOT(clickCombobox_dianpin(QString)));


        // 内容
        xiangmu_cols[QString::fromLocal8Bit("内容")] =3;
        head_infor[name_dev].combox_neirong = new QComboBox(); // 下拉选择框控件
        head_infor[name_dev].combox_neirong->addItem(QString::fromLocal8Bit("UNDEF"));
        head_infor[name_dev].combox_neirong->addItem(QString::fromLocal8Bit("OK"));
        head_infor[name_dev].combox_neirong->addItem(QString::fromLocal8Bit("NG"));

        ui->tableWidget->setCellWidget(i,3, (QWidget*)head_infor[name_dev].combox_neirong);

        connect(head_infor[name_dev].combox_neirong, SIGNAL(currentIndexChanged(QString)), this, SLOT(clickCombobox_neirong(QString)));


        //延时
        ui->tableWidget->setItem(i,4,new QTableWidgetItem(QString::number(20)));
        head_infor[name_dev].yanshi = 20;
        xiangmu_cols[QString::fromLocal8Bit("延时")] =4;


        //相机里面哪个IO口来通信
        xiangmu_cols[QString::fromLocal8Bit("选择IO")] =5;
        head_infor[name_dev].combox_IO_index = new QComboBox(); // 下拉选择框控件
        for(int k = 0; k < 64;k++)
        {
            head_infor[name_dev].combox_IO_index->addItem(QString::number(k));

        }
        ui->tableWidget->setCellWidget(i,5, (QWidget*)head_infor[name_dev].combox_IO_index);
        connect(head_infor[name_dev].combox_IO_index, SIGNAL(currentIndexChanged(QString)), this, SLOT(clickCombobox_io(QString)));


        //是否选用这个相机
        head_infor[name_dev].checkBox = new QTableWidgetItem();
        head_infor[name_dev].checkBox ->setCheckState(Qt::Unchecked);
        head_infor[name_dev].checkBox ->setText(QString::fromLocal8Bit("勾选启用"));
        ui->tableWidget->setItem(i, 1,  head_infor[name_dev].checkBox);
        head_infor[name_dev].if_enable = 0;
        xiangmu_cols[QString::fromLocal8Bit("启用")] =1;




    }


    connect( this, SIGNAL(send_out_msg_queue(QString )), this, SLOT(all_output_infor_queue(QString)));
    initial_ok = 1;



    // connect( this, SIGNAL(send_save_img_sig(cv::Mat  , int  )), this, SLOT(  slot_save_image(cv::Mat  , int  )));

}

void OutPutResult::clickCombobox_dianpin(QString text)
{
    QComboBox *comBox_ = dynamic_cast<QComboBox*>(this->sender());
    if(NULL == comBox_)
    {
        return;
    }
    int x = comBox_->frameGeometry().x();
    int y = comBox_->frameGeometry().y();
    QModelIndex index = ui->tableWidget->indexAt(QPoint(x, y));
    int row = index.row();
    int column = index.column();
    qDebug() << " " << row << " " << column;
    qDebug() << " " << text;

}

void OutPutResult::clickCombobox_io(QString text)
{
    QComboBox *comBox_ = dynamic_cast<QComboBox*>(this->sender());
    if(NULL == comBox_)
    {
        return;
    }
    int x = comBox_->frameGeometry().x();
    int y = comBox_->frameGeometry().y();
    QModelIndex index = ui->tableWidget->indexAt(QPoint(x, y));
    // int row = index.row();
    //int column = index.column();
    // qDebug() << " " << row << " " << column;
    //qDebug() << " " << text;

}

void OutPutResult::clickCombobox_neirong(QString text)
{
    QComboBox *comBox_ = dynamic_cast<QComboBox*>(this->sender());
    if(NULL == comBox_)
    {
        return;
    }
    int x = comBox_->frameGeometry().x();
    int y = comBox_->frameGeometry().y();
    QModelIndex index = ui->tableWidget->indexAt(QPoint(x, y));
    int row = index.row();
    int column = index.column();

    state_row[text] = row;


}

void OutPutResult::on_tableWidget_cellClicked(int row, int column)
{

    current_row = row;
    current_column = column;
}


void OutPutResult::on_tableWidget_cellChanged(int row, int column)
{

    if(initial_ok)
    {
        qDebug() << "==== " << row << " " << column;
        if(ui->tableWidget->item (row,column)->checkState ()==Qt::Checked){
            ui->tableWidget->item(row,column)->setBackgroundColor (Qt::green);

            QString name = rec_name[row ];
            head_infor[name].if_enable = 1;
            state_row[ head_infor[name].combox_neirong->currentText()] = row;
            // qDebug() << head_infor[name].combox_neirong->currentText();


        }else
        {
            ui->tableWidget->item(row,column)->setBackgroundColor (Qt::white);
            //if( sele_comm.cam_name_0_i.count(column)) sele_comm.cam_name_0_i.erase(column);
            QString name = rec_name[row];
            head_infor[name].if_enable = 0;
            //qDebug() << head_infor[name].combox_neirong->currentText();
            state_row[ head_infor[name].combox_neirong->currentText()] = row;


        }

        QString name = rec_name[row ];
        if(xiangmu_cols[QString::fromLocal8Bit("延时")] ==column)
        {
            head_infor[name].yanshi = ui->tableWidget->item(row,column)->text().toInt();
        }
    }
}


OutPutResult::~OutPutResult()
{
    delete ui;
}


void OutPutResult::pre_unix_input(  cv::Mat  img,  Alg_infor alg_infor, std::vector<tr_s_resultinfo>  next_put_ng)//接收上一单元数据
{
    emit send_result_next(img,   alg_infor, next_put_ng);

}

// map<QString, queue<for_queue_resultinfo >> m_tongji_out_queue;
void OutPutResult::clear_output_infor_queue()
{
//    for (map<QString, queue<for_queue_resultinfo >>::iterator iter = m_tongji_out_queue.begin(); iter != m_tongji_out_queue.end();iter++)
//    {
//        QString un_id =  iter->first;

//        while (!m_tongji_out_queue[un_id].empty()) m_tongji_out_queue[un_id].pop();

//    }

//    for (map<QString, queue<for_queue_resultinfo >>::iterator iter = map_q_result_info.begin(); iter != map_q_result_info.end();iter++)
//    {
//        QString un_id =  iter->first;

//        while (!map_q_result_info[un_id].empty()) map_q_result_info[un_id].pop();

//    }

}
//作为输出
void OutPutResult::pre_alg_unix_input_queue( queue<for_queue_resultinfo> & tongji_infor_queue, std::vector<tr_s_resultinfo> next_put_ng ) //接收上一算法单元检测数据
{
     emit send_Fanhuijieguo(m_jieguo);

}

void OutPutResult::set_out_Fanhuijieguo(Fanhuijieguo obj)
{
    m_jieguo.ng_shu =obj.ng_shu;
    m_jieguo.ok_shu =obj.ok_shu;
    m_jieguo.lianglv  =obj.lianglv;
    m_jieguo.zongshu = obj.zongshu;
}

void OutPutResult::on_pushButton_send_cam_tr_test_clicked()
{
    QString name = rec_name[current_row ];
    if( head_infor[name].if_enable)
    {
        int yanshi = head_infor[name].yanshi;
        int dianpin_state =head_infor[name].combox_dianpin->currentIndex();
        int iokou =head_infor[name].combox_IO_index->currentIndex();
        cam_prt[name]->send_state_to_device(iokou,dianpin_state,yanshi);
    }
}


