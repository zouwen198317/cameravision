
#include "move_ctl.h"
#include "ui_move_ctl.h"

#include<QMessageBox>

#include <QMessageBox>
#include <QEventLoop>
#include <QDateTime>
#include <QTimer>

MESSThread::MESSThread(const QString &title, const QString &message):
    m_title(title),
    m_message(message)
{ }

void MESSThread::show(const QString &title, const QString &message)
{
    QEventLoop eventLoop;
    auto messageBox = new MESSThread(title, message);
    connect(messageBox, SIGNAL(destroyed()), &eventLoop, SLOT(quit()));
    messageBox->readyShow();
    eventLoop.exec();
}

void MESSThread::readyShow(void)
{
    this->moveToThread(qApp->thread());
    QTimer::singleShot(0, this, SLOT(onShow()));
}

void MESSThread::onShow(void)
{
    QMessageBox::information(NULL, m_title, m_message);
    this->deleteLater();
}

void move_ctl::on_pushButton_baocuncanshu_clicked()
{
    shifou_jiaqu = ui->checkBox_jiaqu->isChecked();


    yuandian_zankongbi_shijian = ui->lineEdit_yuandian_zksj->text().toInt();
    ganyingdian_zankongbi_shijian = ui->lineEdit_ganyingdian_zksj->text().toInt();


    rtn =MCF_Set_Input_Filter_Time_Bit_Net(IN_YUADIAN_ZP,yuandian_zankongbi_shijian, StationNumber);
    rtn =MCF_Set_Input_Filter_Time_Bit_Net(IN_LAILIAO_GY,ganyingdian_zankongbi_shijian, StationNumber);
    //rtn =MCF_Set_Input_Filter_Time_Bit_Net(IN_LAILIAO_GY,5, StationNumber);

}

int move_ctl::initial()
{
    axis_index = ui->lineEdit_zhou->text().toInt();
    cout << "start open move control car" << endl;
    MCF_Open_Input_Lock_Bit_Net(0, 0);
    /* int rtn = MCF_Open_Net(1, 0, 2);*/
    rtn = MCF_Open_Net(Connection_Number, &Station_Number[0], &Station_Type[0]);		//�����˶����ƿ�
    if (rtn != 0)
    {

        cout << "------->initial fail" << endl;
        if_open_car = 0;
        return  -1;
    }
    else
    {

        cout << "------->initial ok" << endl;

        ui->comB_tingzhiquxian->setCurrentIndex(0);
        ui->comB_weizhimoshi->setCurrentIndex(0);
        if_open_car = 1;

        //夹取开光复位
        MCF_Set_Output_Bit_Net(5, 1);
        MCF_Set_Output_Bit_Net(6, 1);



        //原点感应器和物料感器下降沿触发清零
        rtn = MCF_Clear_Input_Fall_Bit_Net(IN_YUADIAN_ZP, StationNumber);
        rtn = MCF_Clear_Input_Fall_Bit_Net (IN_LAILIAO_GY, StationNumber);



        rtn = MCF_Clear_Input_Fall_Bit_Net(IN_QIDONG, StationNumber);
        rtn = MCF_Clear_Input_Fall_Bit_Net (IN_TINGZHI, StationNumber);


        rtn = MCF_Clear_Input_Fall_Bit_Net (IN_HUIYUANDIAN, StationNumber);
        rtn = MCF_Clear_Input_Fall_Bit_Net (IN_JITING, StationNumber);




        yuandian_zankongbi_shijian = ui->lineEdit_yuandian_zksj->text().toInt();
        ganyingdian_zankongbi_shijian = ui->lineEdit_ganyingdian_zksj->text().toInt();


        rtn =MCF_Set_Input_Filter_Time_Bit_Net(IN_YUADIAN_ZP,yuandian_zankongbi_shijian, StationNumber);
        rtn =MCF_Set_Input_Filter_Time_Bit_Net(IN_LAILIAO_GY,ganyingdian_zankongbi_shijian, StationNumber);

        for(int i = 0; i< 9; i++)
        {
            MCF_Set_Compare_Config_Net(i, 1, 1);    //    '打开位置比较功能     0-位置比较 1-编码器比较
            MCF_Clear_Compare_Points_Net(i);  //         '清除通道所有位置比较点数据
            MCF_Clear_Compare_Current_Points_Net(i); //   '清除通道当前位置比较点数据
        }
        //OUT_NG_CQ,OUT_OK_CQ



        MCF_Set_Encoder_Net(axis_index, 0);
        MCF_Set_Position_Net(axis_index, 0);


        jiankong_wulianniu_xinxi = std::thread(&move_ctl::jiankong_wuli_anniu, this, this);
        jiankong_wulianniu_xinxi.detach();


        return 0;
    }
}




move_ctl::move_ctl(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::move_ctl)
{
    ui->setupUi(this);

    axis_index = 0;

    m_tingzhi_quxian = ui->comB_tingzhiquxian->currentIndex();
    weizhi_mode = ui->comB_weizhimoshi->currentIndex();
    ui->lineEdit_chushihua_sudu->setText(QString::number(chushi_sudu));
    ui->lineEdit_yunxing_sudu->setText(QString::number(yunxing_sud));
    ui->lineEdit_jiasudu->setText(QString::number(jia_sudu));
    ui->lineEdit_huiyuandian_sudu->setText(QString::number(huiyuandian_sudu));

    ui->lineEdit_jjsud->setText(QString::number(jiajia_sudu));
    ui->lineEdit_jiesu_sud->setText(QString::number(jiesu_sudu));
    ui->lineEdit_zhou->setText(QString::number(axis_index));
    ui->lineEdit_mubiao_weizhi->setText(QString::number(mubiao_weizhi));

    ui->lineEdit_dangqian_weizhi->setText(QString::number(Read_Pos1));
    ui->lineEdit_dangqian_bianmaq->setText(QString::number(Read_Enc1));
    ui->lineEdit_dangqian_sudu->setText(QString::number(Read_Vel1));

    //读取初始化的值

    liaocao_jiange = 540;

    pos_yuandian = 0;
    pos_ganyingdian = liaocao_jiange *4;
    pos_gw1 = liaocao_jiange * 1;
    pos_gw2 =  liaocao_jiange * 6;
    pos_chuiqi1 =  liaocao_jiange * 10;
    pos_chuiqi2 =  liaocao_jiange * 12;


    gw2_pl_chuiqi1 = 4*liaocao_jiange;
    gw2_pl_chuiqi2 = 6*liaocao_jiange;

    ui->checkBox_pbtiliao->setChecked(false);

    ui->lineEdit_pos_gw2_pl_chuiqi1->setText(QString::number(gw2_pl_chuiqi1));
    ui->lineEdit_pos_gw2_pl_chuiqi2->setText(QString::number(gw2_pl_chuiqi2));


    //感器器占空比时间
    yuandian_zankongbi_shijian = 10;
    ganyingdian_zankongbi_shijian = 80;

    ui->lineEdit_yuandian_zksj->setText(QString::number(yuandian_zankongbi_shijian));
    ui->lineEdit_ganyingdian_zksj->setText(QString::number(ganyingdian_zankongbi_shijian));



    tingdun_shijian = 1000;


    ui->lineEdit_weizhi_jiange->setText(QString::number(liaocao_jiange));
    ui->lineEdit_yuandian_weizhi->setText(QString::number(pos_yuandian));

    ui->lineEdit_ganyingdian_weizhi->setText(QString::number(pos_ganyingdian));
    ui->lineEdit_pos_gw1->setText(QString::number(pos_gw1));
    ui->lineEdit_pos_gw2->setText(QString::number(pos_gw2));

    ui->lineEdit_pos_chuiqi1 ->setText(QString::number(pos_chuiqi1 ));
    ui->lineEdit_pos_chuiqi2->setText(QString::number(pos_chuiqi2));

    ui->lineEdit_tingdunshijian->setText(QString::number(tingdun_shijian));

    ui->lineEdit_yiquanmaichongshu->setText(QString::number(yiquan_maichongshu));



    chuanganqi_queue_v.push_back(cganqi_gongwei_1);
    chuanganqi_queue_v.push_back(cganqi_gongwei_2);
    chuanganqi_queue_v.push_back(cganqi_chuiqi_1);
    chuanganqi_queue_v.push_back(cganqi_chuiqi_2);


    //夹取机构要复位
    connect(this, SIGNAL(send_int_num(QString)), this, SLOT(get_int_num(QString)));

    connect(this, SIGNAL( send_warnning()), this, SLOT(   refact_warnning()));


    new_thread_gw0 = std::thread(&move_ctl::proecess_gw0, this,this);
    new_thread_gw0.detach();

    new_thread_gw1 = std::thread(&move_ctl::proecess_gw1, this,this);
    new_thread_gw1.detach();


    //    new_thread_gw2 = std::thread(&move_ctl::proecess_gw2, this,this);
    //    new_thread_gw2.detach();
    //    new_thread_gw3 = std::thread(&move_ctl::proecess_gw3, this,this);
    //    new_thread_gw3.detach();




    jiankong_chuanganqi_threa5 = std::thread(&move_ctl::thread_chuanganqi, this, this);
    jiankong_chuanganqi_threa5.detach();

    ui->checkBox_jiaqu->setChecked(true);

    shifou_jiaqu =  ui->checkBox_jiaqu->isChecked();

}


void move_ctl::get_int_num(QString num)
{

    ui->lineEdit_dangqian_weizhi->setText(num);
}
void move_ctl::get_sudu(void* prt_in)
{
    move_ctl* ptr = (move_ctl*)prt_in;
    while (if_open_car)
    {
        if (if_open_car == 1)
        {
            long Position[1] = { 0 };
            //long Encoder[1] = { 0 };
            long Encoder;
            rtn = MCF_Get_Position_Net(ptr->axis_index, &Position[0], 0);
            ptr->Read_Pos1 = Position[0];
            ptr->ui->lineEdit_dangqian_weizhi->setText(QString::number(ptr->Read_Pos1));
            emit ptr->send_int_num(QString::number(ptr->Read_Pos1));

            rtn = MCF_Get_Encoder_Net(ptr->axis_index, &Encoder, 0);
            ptr->Read_Enc1 = Encoder;
            ptr->ui->lineEdit_dangqian_bianmaq->setText(QString::number(ptr->Read_Enc1));



            double Vel1[1] = { 0 };
            double Vel2[1] = { 0 };

            rtn = MCF_Get_Vel_Net(ptr->axis_index, Vel1, Vel2, 0);

            ptr->Read_Vel1 = Vel1[0];
            ptr->ui->lineEdit_dangqian_sudu->setText(QString::number(ptr->Read_Vel1));


            Sleep(20);
        }
        else
        {
            Sleep(200);
            continue;
        }

    }


}

void move_ctl::go_position(int dDist,int flag_dir)
{
    if(shineng_state_flag)
    {

        axis_index = ui->lineEdit_zhou->text().toInt();

        double  Vini = ui->lineEdit_chushihua_sudu->text().toDouble();  //  '��ʼ�ٶ�
        double Vmax = ui->lineEdit_yunxing_sudu->text().toDouble();//   '�ٶ�
        double Vadd = ui->lineEdit_jiasudu->text().toDouble();//   '���ٶ�
        double Vjerk = ui->lineEdit_jjsud->text().toDouble();//   '�Ӽ��ٶ�
        double Vend = ui->lineEdit_jiesu_sud->text().toDouble();//   '�����ٶ�
        short Profile = ui->comB_tingzhiquxian->currentIndex();
        short Position = ui->comB_weizhimoshi->currentIndex();//   '���� ����
        //int  dDist = ui->lineEdit_mubiao_weizhi->text().toInt();//

        rtn = MCF_Set_Axis_Profile_Net(axis_index, Vini, Vmax, Vadd, Vjerk, Vend, Profile, 0);
        rtn = MCF_Uniaxial_Net(axis_index, dDist*flag_dir, Position, 0);
        int if_daowei = 0;
        do {

            long pos;
            rtn = MCF_Get_Position_Net(axis_index, &pos, 0);
            if(pos ==dDist) if_daowei =1;
        }while(!if_daowei);

    }
}
//单步测试
void move_ctl::on_pushButton_danbu_ceshi_clicked()
{
    if(if_open_car ==1 && shineng_state_flag)
    {
        // int jiange = ui->lineEdit_weizhi_jiange->text().toUInt();
        // int  yanshi = ui->lineEdit_tingdunshijian->text().toUInt();

        long Encoder;
        rtn = MCF_Get_Position_Net(axis_index, &Encoder, 0);
        int jiange = ui->lineEdit_weizhi_jiange->text().toUInt();
        int  yanshi = ui->lineEdit_tingdunshijian->text().toUInt();

        //转动方向，0为顺时针方向
        MCF_Set_Pulse_Mode_Net(axis_index, 0);
        long pos = Encoder +jiange;
        go_position( pos  ,1);
        Sleep(yanshi);


    }else
    {
        MESSThread::show(QObject::tr("提示"), QObject::tr("使能"));
    }
}

//主发送转动脉冲线程
void move_ctl::main_thread(void* ptr)
{
    int k = 0;
    if(shineng_state_flag)
    {
        qidong_flag = 1;
        do{

            long Encoder;
            long positing;

            pos_ganyingdian = ui->lineEdit_ganyingdian_weizhi->text().toInt();
            long chuanganq_pl_yuandian = pos_ganyingdian;


            int jiange = ui->lineEdit_weizhi_jiange->text().toUInt();
            int  yanshi = ui->lineEdit_tingdunshijian->text().toUInt();
            rtn = MCF_Get_Position_Net(axis_index, &positing, 0);
            rtn = MCF_Get_Encoder_Net(axis_index, &Encoder, 0);
            // if(positing!=Encoder)
            {
                //MCF_Set_Encoder_Net(axis_index, positing);
            }
            // current_maichong = positing;
            // dangqian_pos.push(positing+ chuanganq_pl_yuandian);
            //转动方向，0为顺时针方向
            MCF_Set_Pulse_Mode_Net(axis_index, 0);
            // cout<<positing <<endl;
            long pos = positing +jiange;
            //要去的目标
            go_position( pos  ,1);
            cishu_yunxing++;
            Sleep(yanshi);


        }while(qidong_flag);

    }else
    {
        MESSThread::show(QObject::tr("提示"), QObject::tr("使能"));
    }
}

void move_ctl::thread_chuanganqi(void* prt_in)
{
    move_ctl* ptr = (move_ctl*)prt_in;

    do{
        // long  current = obj_position;
        long Encoder;

        ushort Bit_Input_Fall = 0;
        //判断当前位置是否有物料
        rtn = MCF_Get_Input_Fall_Bit_Net(IN_LAILIAO_GY, &Bit_Input_Fall, StationNumber);
        if(Bit_Input_Fall == 1) //如果有下升沿触发就保存当前编码器位置点,
        {
            rtn = MCF_Get_Encoder_Net(axis_index, &Encoder, 0);
            // cout<<Encoder<<"------------>chufa"<<endl;
            add_maichong_map(Encoder);
            MCF_Clear_Input_Fall_Bit_Net(IN_LAILIAO_GY,StationNumber);//
        }

    }while(1);
}

//暂停测试
void move_ctl::on_pushButton_tingzhi_ceshi_clicked()
{
    qidong_flag = 0;
    if_ceshizhong = 0;

    //关闭振动盘
    MCF_Set_Output_Bit_Net(OUT_ZDP_QD, GAO_DIANPIN);


    MCF_Set_Output_Bit_Net(OUT_ZDP_CQ, GAO_DIANPIN);




    //亮黄灯
    MCF_Set_Output_Bit_Net(OUT_HUANG, DI_DIANPIN);

    //关闭绿灯
    MCF_Set_Output_Bit_Net(OUT_LV, GAO_DIANPIN);

    //关闭红灯
    MCF_Set_Output_Bit_Net(OUT_HONG, GAO_DIANPIN);

    //void  sig_car_ui_send_qidong(int flag);
    // void  sig_car_ui_send_tingzhi(int flag);
    // void  sig_car_ui_send_jiting(int flag);
    // void  sig_car_ui_send_huiyuandian(int flag);

}

//硬启动测试按钮
void move_ctl::on_pushButton_ceshi_clicked()
{
    cout<<"qidong_flag:"<<qidong_flag<<endl;

    //qidong_flag = 0;
    // if_ceshizhong = 0;

    if(qidong_flag ==0)
    {

        MCF_Set_Encoder_Net(axis_index, 0);
        MCF_Set_Position_Net(axis_index, 0);

        MCF_Set_Output_Bit_Net(OUT_ZDP_QD, DI_DIANPIN);


        MCF_Set_Output_Bit_Net(OUT_ZDP_CQ, DI_DIANPIN);
        //emit  sig_car_ui_send_qidong(1);
        Sleep(300);

        on_pb_shineng_kai_clicked();
        //关闭黄灯
        MCF_Set_Output_Bit_Net(OUT_HUANG, GAO_DIANPIN);

        if(if_open_car ==1 && shineng_state_flag)
        {
            MCF_Set_Output_Bit_Net(OUT_ZDP_QD, DI_DIANPIN);

            MCF_Set_Output_Bit_Net(OUT_ZDP_CQ, DI_DIANPIN);
            MCF_Set_Output_Bit_Net(OUT_LV, DI_DIANPIN);
            qidong_flag  =1;
            Sleep(200);
            if_ceshizhong = 1;
            std::thread main_th(&move_ctl::main_thread, this, this);
            main_th.detach();
        }
    }

}

// 监控物理按钮是否按下事件
void  move_ctl::jiankong_wuli_anniu(void* ptr)
{
    ushort Bit_Input_Fall_jiting = 0;
    ushort Bit_Input_Fall_zanting = 0;
    ushort Bit_Input_Fall = 0;
    do{
        ushort bianliang = 0;


        //启动

        rtn = MCF_Get_Input_Fall_Bit_Net(IN_QIDONG, &Bit_Input_Fall, StationNumber);
        if(Bit_Input_Fall == 1) //如果有下升沿触发就保存当前编码器位置点,
        {
            emit  sig_car_ui_send_qidong(1);
            //on_pushButton_ceshi_clicked();

            MCF_Clear_Input_Fall_Bit_Net(IN_QIDONG,StationNumber);//
        }


        //暂停
        rtn = MCF_Get_Input_Fall_Bit_Net(IN_TINGZHI, &Bit_Input_Fall_zanting, StationNumber);
        if(Bit_Input_Fall_zanting == 1) //如果有下升沿触发就保存当前编码器位置点,
        {
            on_pushButton_tingzhi_ceshi_clicked();
            qidong_flag = 0;
            MCF_Clear_Input_Fall_Bit_Net(IN_TINGZHI,StationNumber);//
        }


        //急停

        rtn = MCF_Get_Input_Bit_Net(IN_JITING, &Bit_Input_Fall, StationNumber);
        if(Bit_Input_Fall == 0) //如果有下升沿触发就保存当前编码器位置点,
        {
            cout<<"jiting:"<<Bit_Input_Fall<<endl;
            MCF_Set_Output_Bit_Net(OUT_HONG, GAO_DIANPIN);
            on_Stop1_clicked();
            qidong_flag = 0;
            //亮红灯
            MCF_Set_Output_Bit_Net(OUT_HONG, GAO_DIANPIN);
            Bit_Input_Fall_jiting = 1;

            //MCF_Clear_Input_Fall_Bit_Net(IN_JITING,StationNumber);//
        }else
        {
            Bit_Input_Fall_jiting = 1;
        }

        Sleep(20);
    }while(if_open_car);

}



//工位一 相机0 拍照 另外一种方式
void  move_ctl::proecess_gw0(void* prt_in)
{
    move_ctl* ptr = (move_ctl*)prt_in;
    do{

        if(chuanganqi_queue_v[GW0].empty())
        {
            Sleep(10);
        }else
        {
            long Encoder;
            ushort yuandianzhi = 0;


            rtn = MCF_Get_Encoder_Net(axis_index, &Encoder, 0);

            //判断是否到达拍照位置
            long queue_pos =chuanganqi_queue_v[GW0].front().maichong_xunshun;
            if(  (Encoder- queue_pos )<=5  && (Encoder- queue_pos )>=0 )
                //if(map_duilie_vec[GW0].count(Encoder))
            {

                long tempk = Encoder;

                if(shifou_jiaqu)
                {
                    //发送 出位 指令
                    MCF_Set_Output_Bit_Net(OUT_PY_DCF, DI_DIANPIN);
                    int flag = 0;
                    int num_loop = 0;
                    //看平移是否出到位
                    do{
                        Sleep(100);
                        ushort reflag  =-1;
                        MCF_Get_Input_Bit_Net(IN_PYQG_CW,&reflag);//低电平触发
                        if(reflag ==0)
                        {
                            // cout<<"CAM0 chuwei wancheng"<<endl;
                            flag = 1;
                        }else
                        {
                            //如果读取多次气缸未弹出，停机报警
                            num_loop++;
                            if(num_loop >50)
                            {
                                //出位不到位，报警，停机
                                emit send_warnning();
                                break;
                            }
                        }

                    }while(!flag);
                    //发夹料指令
                    MCF_Set_Output_Bit_Net(OUT_JL_DCF, DI_DIANPIN);

                    flag = 0;
                    num_loop = 0;
                    //看看夹取是否到位
                    do{
                        Sleep(400);
                        ushort reflag  =-1;
                        MCF_Get_Input_Bit_Net(IN_JLQG_CW,&reflag);//低电平触发
                        if(reflag ==0)
                        {
                            flag = 1;
                        }else
                        {
                            //如果读取多次气缸未弹出，停机报警
                            num_loop++;
                            if(num_loop >50)
                            {
                                emit send_warnning();
                                break;
                            }
                        }
                    }while(!flag);
                }

                //发送拍照信号
                //emit send_camera_cap(GW0 ,tempk);
                MCF_Set_Output_Bit_Net(OUT_CCD1, GAO_DIANPIN);
                Wuliao_queue xx;
                xx.maichong_xunshun =tempk;

                Sleep(10);
                if(shifou_jiaqu)
                {
                    MCF_Set_Output_Bit_Net(OUT_CCD1, DI_DIANPIN);
                    //发夹料松指令
                    MCF_Set_Output_Bit_Net(OUT_JL_DCF, GAO_DIANPIN);
                    Sleep(50);
                    //发送入位指令
                    MCF_Set_Output_Bit_Net(OUT_PY_DCF, GAO_DIANPIN);
                    Sleep(50);
                }
                //map_duilie_vec[GW0].erase(tempk);
                chuanganqi_queue_v[GW0].pop();
            }
        }
    }while(1);
}

//工位2 两个相机拍照 ，相机1和相机2
void  move_ctl::proecess_gw1(void* prt_in){

    move_ctl* ptr = (move_ctl*)prt_in;
    do{

        if(chuanganqi_queue_v[GW1].empty())
        {
            Sleep(30);
        }else
        {
            long Encoder;
            ushort yuandianzhi = 0;


            rtn = MCF_Get_Encoder_Net(axis_index, &Encoder, 0);
            long queue_pos =chuanganqi_queue_v[GW1].front().maichong_xunshun;
            if(  (Encoder- queue_pos )<=5  && (Encoder- queue_pos )>=0 )
                //if(map_duilie_vec[GW0].count(Encoder))

            {

                long tempk = Encoder;

                liangxiangji_gw.push(tempk);

                //硬触发拍照信号
                Sleep(10);

                // cout<<"1-cam1 ---------------------->"<<endl;
                MCF_Set_Output_Bit_Net(OUT_CCD2, GAO_DIANPIN);
                Sleep(10);
                MCF_Set_Output_Bit_Net(OUT_CCD2, DI_DIANPIN);
                Sleep(5);
                MCF_Set_Output_Bit_Net(OUT_CCD3, GAO_DIANPIN);

                Sleep(10);
                MCF_Set_Output_Bit_Net(OUT_CCD3, DI_DIANPIN);


                //发送拍照信号
                //emit send_camera_cap(GW0 ,tempk);
                chuanganqi_queue_v[GW1].pop();

            }
        }
    }while(1);
}

// NG OK踢料压队列
void move_ctl::add_tr_point(unsigned short ch,unsigned short io_index,long pos)
{

    MCF_Add_Compare_Point_Net(ch, pos, 1, 8, io_index);// '输出100ms
}

void move_ctl::clear_queue()
{

    for(int i = 0; i< 9; i++)
    {
        MCF_Set_Compare_Config_Net(i, 1, 1);    //    '打开位置比较功能     0-位置比较 1-编码器比较
        MCF_Clear_Compare_Points_Net(i);  //         '清除通道所有位置比较点数据
        MCF_Clear_Compare_Current_Points_Net(i); //   '清除通道当前位置比较点数据
    }


    while (!tiliao_chuiqi_2.empty()) tiliao_chuiqi_2.pop();
    while (!tiliao_chuiqi_1.empty()) tiliao_chuiqi_1.pop();

    while (!liangxiangji_gw.empty()) liangxiangji_gw.pop();

    for(int i =0;i < chuanganqi_queue_v.size();i++)
    {

        while (!chuanganqi_queue_v[i].empty())  chuanganqi_queue_v[i].pop();
    }


}


void move_ctl::add_maichong_map(long maichong)
{

    pos_gw1 = ui->lineEdit_pos_gw1->text().toInt();
    pos_gw2 = ui->lineEdit_pos_gw2->text().toInt();


    pos_chuiqi1 = ui->lineEdit_pos_chuiqi1->text().toInt();
    pos_chuiqi2 = ui->lineEdit_pos_chuiqi2->text().toInt();

    gw2_pl_chuiqi1 = ui->lineEdit_pos_gw2_pl_chuiqi1->text().toInt();
    gw2_pl_chuiqi2 = ui->lineEdit_pos_gw2_pl_chuiqi2->text().toInt();

    //tiliao_duilie.push();
    Wuliao_queue temp1;

    //gongwei 1
    temp1.maichong_xunshun =maichong + pos_gw1 ;
    temp1.have_flag  = 1;
    //  cout<<"g1 yanru:"<< temp1.maichong_xunshun<<endl;

    chuanganqi_queue_v[GW0].push(temp1);

    temp1.maichong_xunshun =maichong + pos_gw2 ;
    temp1.have_flag  = 1;

    //  cout<<"g2 yanru:"<< temp1.maichong_xunshun<<endl;

    chuanganqi_queue_v[GW1].push(temp1);






    temp1.maichong_xunshun =maichong + pos_chuiqi1 ;
    temp1.have_flag  = 1;

    //  cout<<"g3 yanru:"<< temp1.maichong_xunshun<<endl;


    chuanganqi_queue_v[GW2].push(temp1);

    // 感应器点到各个工位的 脉冲值距离
    temp1.maichong_xunshun =maichong + pos_chuiqi2 ;
    temp1.have_flag  = 1;

    //cout<<"g4 yanru:"<< temp1.maichong_xunshun<<endl;

    chuanganqi_queue_v[GW3].push(temp1);

}

//实施夹取动作
void  move_ctl::jiaqu_motion()
{


}

//解除夹取动作
void  move_ctl::jiechu_jiaqu()
{

}

void move_ctl::refact_warnning()
{
    qidong_flag = 0;
}

void move_ctl::on_push_gaodianpin_tr_clicked()
{

    int tongdao = ui->lineEdit_tongdaoshu->text().toInt();

    MCF_Set_Output_Bit_Net(tongdao, 1);
}

void move_ctl::on_push_didianpin_tr_clicked()
{

    int tongdao = ui->lineEdit_tongdaoshu->text().toInt();

    MCF_Set_Output_Bit_Net(tongdao, 0);
}




move_ctl::~move_ctl()
{
    delete ui;
}

void  move_ctl::on_ps_open_car_clicked()
{
    if(if_open_car== 0)
    {
        int re =  initial();
        if(re==-1)
        {
            return;
        }else
        {
            ui->ps_open_car->setStyleSheet("background-color:rgb(0,255,0)");
            ui->ps_close_car->setStyleSheet("background-color:rgb(255,255,255)");

            std::thread th_get(&move_ctl::get_sudu, this, this);
            th_get.detach();
        }

    }


}





void move_ctl::thread_jiankong(void* prt_in)
{
    move_ctl* ptr = (move_ctl*)prt_in;

    ulong Input_Count_Fall[10] = { 0 };
    ulong Lock_Data_Buffer[10] = { 0 };
    while (flag_jiankong)
    {

        Sleep(10);
        rtn = MCF_Get_Input_Fall_Count_Bit_Net(0, &Input_Count_Fall[0], &Lock_Data_Buffer[0]);

        if (Input_Count_Fall[0] > 0)
        {

            cout << Input_Count_Fall[0] << " : " << Lock_Data_Buffer[0] << endl;
            int temp = Lock_Data_Buffer[0];
            MCF_Add_Compare_Point_Net(0, temp + 17500, 1, 9, 0);    //         '����  ���ڵ���  2ms   DO0
            MCF_Add_Compare_Point_Net(0, temp + 25000, 1, 9, 1);  //           '����  ���ڵ���  2ms   DO0
            MCF_Add_Compare_Point_Net(0, temp + 52000, 1, 9, 2); //            '����  ���ڵ���  2ms   DO0

            Input_Count_Fall[10] = { 0 };
        }


    }

}


void move_ctl::on_fenxuan_tingzhi_clicked()
{
    flag_jiankong = 0;


    axis_index = ui->lineEdit_zhou->text().toInt();
    double Vadd = ui->lineEdit_jiasudu->text().toDouble();//   '���ٶ�
    double Vjerk = ui->lineEdit_jjsud->text().toDouble();//   '�Ӽ��ٶ�

    short Profile = ui->comB_tingzhiquxian->currentIndex();
    rtn = MCF_Set_Axis_Stop_Profile_Net(axis_index, Vadd, Vjerk, Profile, 0);
    rtn = MCF_Axis_Stop_Net(axis_index, 1, 0);

}
void move_ctl::on_fenxuan_qidong_clicked()
{
    flag_get_infor = 1;
    jog_initial();
}
int  move_ctl::jog_initial()
{
    MCF_Set_Input_Filter_Time_Bit_Net(0, 10, 0);//     '���������˲� DI0��DI1
    MCF_Set_Compare_Config_Net(0, 1, 0);//       '����λ�ñȽϹ���     0-λ�ñȽ� 1-�������Ƚ�
    MCF_Clear_Compare_Points_Net(0);//           '����ͨ������λ�ñȽϵ�����
    MCF_Clear_Compare_Current_Points_Net(0);//   '����ͨ����ǰλ�ñȽϵ�����
    //'MCF_Disable_Compare_Any_Points_Net(0, 0);//   '�ر�ͨ������λ�ñȽϵ�����
    MCF_Clear_Input_Fall_Bit_Net(0, 0);//

    flag_jiankong = 1;
    std::thread th_jiankong(&move_ctl::thread_jiankong, this, this);
    th_jiankong.detach();

    on_ps_jog_fu_clicked();

}


//回原点
void  move_ctl::huiyuandian_thread(void* prt_in)
{
    ushort yuandianzhi = 0;
    move_ctl* ptr = (move_ctl*)prt_in;
    if_in_yuandian = 0;
    double yuanlai = ui->lineEdit_huiyuandian_sudu->text().toDouble();//
    if(shineng_state_flag)
    {
        axis_index = ui->lineEdit_zhou->text().toInt();

        double Vmax = yuanlai;//
        double Vadd = ui->lineEdit_jiasudu->text().toDouble();//

        //回原点，反方向
        MCF_Set_Pulse_Mode_Net(axis_index, 1);
        MCF_JOG_Net(axis_index, Vmax, Vadd, 0);

        run_state_flag = 1;
    }
    run_state_flag = 1;
    do{
        // long  current = obj_position;
        long Encoder;

        ushort Bit_Input_Fall = 0;
        //判断当前位置是否有物料
        rtn = MCF_Get_Input_Fall_Bit_Net(IN_YUADIAN_ZP, &Bit_Input_Fall, StationNumber);
        if(Bit_Input_Fall == 1) //如果有下升沿触发就保存当前编码器位置点,
        {
            //on_Stop1_clicked();
            if_in_yuandian = 1;
            MCF_Clear_Input_Fall_Bit_Net(IN_YUADIAN_ZP,StationNumber);//
        }

    }while(!if_in_yuandian);



//    do{
//        MCF_Get_Input_Bit_Net(IN_YUADIAN_ZP,&yuandianzhi);//低电平触发
//        if(yuandianzhi <=0)
//        {
//            on_Stop1_clicked();
//            if_in_yuandian = 1;


//        }
//    }while (!if_in_yuandian);

    if(if_in_yuandian)
    {

         MCF_Set_Servo_Enable_Net(axis_index, 0);

        MCF_Set_Encoder_Net(axis_index, 0);
        MCF_Set_Position_Net(axis_index, 0);


        MESSThread::show(QObject::tr("提示"), QObject::tr("成功"));
    }
   MCF_Set_Pulse_Mode_Net(axis_index, 0);
  // MCF_Set_Servo_Enable_Net(axis_index, 0);
    run_state_flag = 0;


}

void  move_ctl::tixing(QString neirong)
{
    //QMessageBox::information(NULL, QObject::tr("提示"), QObject::tr("dd"),  QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    // QMessageBox::about(NULL,  "About",  neirong);
}
void move_ctl::on_pushButton_huiyuandian_clicked()
{

    if(shineng_state_flag && qidong_flag==0  &&  if_ceshizhong==0)
    {

        std::thread th_huiyuandian(&move_ctl::huiyuandian_thread, this, this);
        th_huiyuandian.detach();
        cishu_yunxing = 1;

    }
}









void move_ctl::on_ps_yushe_clicked()
{



    MCF_Set_Compare_Config_Net(0, 1, 1);  //     '����λ�ñȽϹ���     0-λ�ñȽ� 1-�������Ƚ�
    MCF_Clear_Compare_Points_Net(0);  //         '����ͨ������λ�ñȽϵ�����
    MCF_Clear_Compare_Current_Points_Net(0); //  '����ͨ����ǰλ�ñȽϵ�����
    MCF_Disable_Compare_Any_Points_Net(0, 0); // '�ر�ͨ������λ�ñȽϵ�����
    int  dis = 5000;//
    MCF_Add_Compare_Point_Net(0, dis, 1, 9, 1);  //            '����  ���ڵ���  2ms   DO0
    MCF_Add_Compare_Point_Net(0, dis + 3000, 1, 9, 0);//
    MCF_Add_Compare_Point_Net(0, dis + 3000, 1, 9, 1);//

    MCF_Add_Compare_Point_Net(0, dis + 10000, 1, 9, 1);//
    cout << "yushe ok" << endl;

}

void move_ctl::on_ps_jog_zheng_clicked()
{
    if(shineng_state_flag)
    {
        axis_index = ui->lineEdit_zhou->text().toInt();

        double Vmax = ui->lineEdit_yunxing_sudu->text().toDouble();//   '�ٶ�
        double Vadd = ui->lineEdit_jiasudu->text().toDouble();//   '���ٶ�
        MCF_Set_Pulse_Mode_Net(axis_index, 0);
        MCF_JOG_Net(axis_index, Vmax, Vadd, 0);

        run_state_flag = 1;
    }

}
void move_ctl::on_ps_jog_fu_clicked()
{
    if(shineng_state_flag)
    {
        axis_index = ui->lineEdit_zhou->text().toInt();

        double Vmax = ui->lineEdit_yunxing_sudu->text().toDouble();//   '�ٶ�
        double Vadd = ui->lineEdit_jiasudu->text().toDouble();//   '���ٶ�
        MCF_Set_Pulse_Mode_Net(axis_index, 1);
        MCF_JOG_Net(axis_index, Vmax, Vadd, 0);
        run_state_flag = 1;
    }
}

void move_ctl::on_ps_fu_move_clicked()
{
    axis_index = ui->lineEdit_zhou->text().toInt();



    double  Vini = ui->lineEdit_chushihua_sudu->text().toDouble();  //  '��ʼ�ٶ�
    double Vmax = ui->lineEdit_yunxing_sudu->text().toDouble();//   '�ٶ�
    double Vadd = ui->lineEdit_jiasudu->text().toDouble();//   '���ٶ�
    double Vjerk = ui->lineEdit_jjsud->text().toDouble();//   '�Ӽ��ٶ�
    double Vend = ui->lineEdit_jiesu_sud->text().toDouble();//   '�����ٶ�
    short Profile = ui->comB_tingzhiquxian->currentIndex();
    short Position = ui->comB_weizhimoshi->currentIndex();//   '���� ����
    int  dDist = ui->lineEdit_mubiao_weizhi->text().toInt();//

    rtn = MCF_Set_Axis_Profile_Net(axis_index, Vini, Vmax, Vadd, Vjerk, Vend, Profile, 0);
    rtn = MCF_Uniaxial_Net(axis_index, -dDist, Position, 0);
}
void move_ctl::on_ps_zheng_move_clicked()
{
    axis_index = ui->lineEdit_zhou->text().toInt();

    double  Vini = ui->lineEdit_chushihua_sudu->text().toDouble();  //  '��ʼ�ٶ�
    double Vmax = ui->lineEdit_yunxing_sudu->text().toDouble();//   '�ٶ�
    double Vadd = ui->lineEdit_jiasudu->text().toDouble();//   '���ٶ�
    double Vjerk = ui->lineEdit_jjsud->text().toDouble();//   '�Ӽ��ٶ�
    double Vend = ui->lineEdit_jiesu_sud->text().toDouble();//   '�����ٶ�
    short Profile = ui->comB_tingzhiquxian->currentIndex();
    short Position = ui->comB_weizhimoshi->currentIndex();//   '���� ����
    int  dDist = ui->lineEdit_mubiao_weizhi->text().toInt();//

    rtn = MCF_Set_Axis_Profile_Net(axis_index, Vini, Vmax, Vadd, Vjerk, Vend, Profile, 0);
    rtn = MCF_Uniaxial_Net(axis_index, dDist, Position, 0);
}


void move_ctl::on_stop_man_clicked()
{

    //测试线程关闭
    qidong_flag = 0;

    //测试标志位
    if_ceshizhong = 0;


    axis_index = ui->lineEdit_zhou->text().toInt();
    double Vadd = ui->lineEdit_jiasudu->text().toDouble();//   '���ٶ�
    double Vjerk = ui->lineEdit_jjsud->text().toDouble();//   '�Ӽ��ٶ�

    short Profile = ui->comB_tingzhiquxian->currentIndex();
    rtn = MCF_Set_Axis_Stop_Profile_Net(axis_index, Vadd, Vjerk, Profile, 0);
    rtn = MCF_Axis_Stop_Net(axis_index, 1, 0);
    flag_get_infor = 0;
    flag_jiankong = 0;
    run_state_flag = 0;


    //使能关
    on_pb_shineng_guan_clicked();

    //关闭振动盘
    MCF_Set_Output_Bit_Net(OUT_ZDP_QD, GAO_DIANPIN);
    //关闭振动盘
    MCF_Set_Output_Bit_Net(OUT_ZDP_CQ, GAO_DIANPIN);

    //亮黄灯
    MCF_Set_Output_Bit_Net(OUT_HUANG, DI_DIANPIN);


}
//急停
void move_ctl::on_Stop1_clicked()
{

    //测试线程关闭
    qidong_flag = 0;

    //测试标志位
    if_ceshizhong = 0;



    axis_index = ui->lineEdit_zhou->text().toInt();

    double Vadd = ui->lineEdit_jiasudu->text().toDouble();//   '���ٶ�
    double Vjerk = ui->lineEdit_jjsud->text().toDouble();//   '�Ӽ��ٶ�

    short Profile = ui->comB_tingzhiquxian->currentIndex();

    m_Vadd = Vadd;//   '���ٶ�
    m_Vjerk = Vjerk;//   '�Ӽ��ٶ�

    m_Profile = Profile;
    rtn = MCF_Set_Axis_Stop_Profile_Net(axis_index, Vadd, Vjerk, Profile, 0);
    rtn = MCF_Axis_Stop_Net(axis_index, 0, 0);
    flag_get_infor = 0;
    flag_jiankong = 0;
    run_state_flag = 0;



    //使能关
    on_pb_shineng_guan_clicked();

    //关闭振动盘
    MCF_Set_Output_Bit_Net(OUT_ZDP_QD, GAO_DIANPIN);
    MCF_Set_Output_Bit_Net(OUT_ZDP_CQ, GAO_DIANPIN);

    //亮红灯
    MCF_Set_Output_Bit_Net(OUT_HONG, DI_DIANPIN);

    //关闭黄灯
    MCF_Set_Output_Bit_Net(OUT_HUANG, GAO_DIANPIN);

    //关闭绿灯
    MCF_Set_Output_Bit_Net(OUT_LV, GAO_DIANPIN);


}

void move_ctl::on_pb_shineng_guan_clicked()
{


    axis_index = ui->lineEdit_zhou->text().toInt();
    MCF_Set_Servo_Enable_Net(axis_index, 1);
    flag_get_infor = 0;
    shineng_state_flag = 0;
    ui->pb_shineng_kai->setStyleSheet("background-color:rgb(255,255,255)");
    ui->pb_shineng_guan->setStyleSheet("background-color:rgb(0,255,0)");
}
void move_ctl::on_pb_shineng_kai_clicked()
{
    if(if_open_car==1)
    {
        axis_index = ui->lineEdit_zhou->text().toInt();
        MCF_Set_Servo_Enable_Net(axis_index, 0);
        flag_get_infor = 1;
        shineng_state_flag = 1;
        ui->pb_shineng_kai->setStyleSheet("background-color:rgb(0,255,0)");
        ui->pb_shineng_guan->setStyleSheet("background-color:rgb(255,255,255)");
    }
}

void  move_ctl::on_ps_close_car_clicked()
{
    //if_open_car = 0;
    close();

    ui->ps_open_car->setStyleSheet("background-color:rgb(255,255,255)");
    ui->ps_close_car->setStyleSheet("background-color:rgb(0,255,0)");

}
int move_ctl::close()
{
    if_open_car = 0;
    rtn = MCF_Close_Net();
    return rtn;
}

void move_ctl::on_ps_bianmqinglng_clicked()
{

    if(run_state_flag ==0)
    {
        QMessageBox::information(NULL,  QObject::tr("警告"), QObject::tr("清零"),  QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        MCF_Set_Encoder_Net(axis_index, 0);
        MCF_Set_Position_Net(axis_index, 0);

        //        Private Sub Button4_Click(sender As Object, e As MouseEventArgs) Handles Button4.MouseDown, Button4.Click
        //                If IsAutoRunning Then
        //                    lgbox.msg("自动运行中，不允许清楚坐标")
        //                    Return
        //                End If
        //                If e.Button = Windows.Forms.MouseButtons.Left Then
        //                    MCF_Set_Encoder_Net(0, 0)
        //                    MCF_Set_Position_Net(0, 0)
        //                ElseIf e.Button = Windows.Forms.MouseButtons.Right Then
        //                    Task.Factory.StartNew(Sub()
        //                                              Jog_P()
        //                                              Dim res As Short = 0
        //                                              Do
        //                                                  MCF_Get_Input_Bit_Net(0, res)
        //                                              Loop Until res = 1
        //                                              MCF_Axis_Stop_Net(Axis_1, 0)
        //                                              MCF_Set_Encoder_Net(0, 0)
        //                                              MCF_Set_Position_Net(0, 0)
        //                                          End Sub)
        //                End If

        //            End Sub

    }else
    {
        // QMessageBox::information(this, QString::fromLocal8Bit("警告"),QString::fromLocal8Bit("自动运行中，不允许清除坐标"));
        QMessageBox::information(NULL, QObject::tr("警告"), QObject::tr("运行"),  QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
}



void move_ctl::on_push_gaodianpin_tr_2_clicked()
{
    MCF_Set_Output_Bit_Net(5, 0);
}

void move_ctl::on_push_didianpin_tr_2_clicked()
{
    MCF_Set_Output_Bit_Net(5, 1);
}

void move_ctl::on_push_gaodianpin_tr_3_clicked()
{
    MCF_Set_Output_Bit_Net(6, 0);
}

void move_ctl::on_push_didianpin_tr_3_clicked()
{

    MCF_Set_Output_Bit_Net(6, 1);
}
