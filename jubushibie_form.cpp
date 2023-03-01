#include "jubushibie_form.h"
#include "ui_jubushibie_form.h"
#include<QDebug>


//map<QString,queue<for_queue_resultinfo>> map_q_result_info;

// QHash<QString, QVector< for_queue_resultinfo> > m_duorenwu_jieguo;

extern int fx_initial_gpu_rt(int gpu_id, int cam_id, char* path_input, int net_w, int  net_h);
extern int fx_alg_processing_rt(int cam_id, char* data, int width, int height, int channel);
extern bool fx_get_detection_result_gpu_cam_x_rt(struct tr_s_resultinfo* Result, int Num, int cam_id, int origin_w, int origin_h);
extern int fx_release_detection_gpu_x_cam_x_rt(int cam_id);


int  shifouzaijisuan = 0;
int zongjisuancishu = 10;

extern float colors[6][3];

extern float get_color(int c, int x, int max);

extern  string doubleToString(const double& dbNum);

QMap<QString,DangLiang> xj_dangliang;



//int jisuan_xiancheng_flag = 0;
//void jisuan_xiancheng(void * ptm);
//std::thread jisuan_th;




jubushibie_form::jubushibie_form(QWidget *parent ,     properconfig *  input_procfg , int model_id ,QString str_model_id):
    QMainWindow(parent),
    ui(new Ui::jubushibie_form)
{
    ui->setupUi(this);

    jisuan_xiancheng_flag = 1;

    void jisuan_xiancheng(void * ptm);





    jisuan_th = std::thread(&jubushibie_form::jisuan_xiancheng, this, this);

    jisuan_th.detach();


    m_input_procfg =input_procfg;

    setMouseTracking(true);

    startPos = QPoint(0, 0);

    //m_paizhao_jici = 0;
    jubu_m_paizhao_jici = 0;
    while (!m_duo_paizhao_Queue.q_mat_tupian.empty())
    {
        m_duo_paizhao_Queue.q_mat_tupian.pop();
    }
#if SHIFOU_SUOFANG
    resize_img = cv::Mat(res_w,res_h,CV_8UC3);
#endif
}

jubushibie_form::~jubushibie_form()
{

    jisuan_xiancheng_flag = 0;

    while (!m_duo_paizhao_Queue.q_mat_tupian.empty())
    {

        m_duo_paizhao_Queue.q_mat_tupian.pop();
    }


    if(m_alg_be_call_num > 0)
    {
        release_alg();
        m_alg_be_call_num = 0;
    }

    m_input_procfg = nullptr;
    if (m_alg_infor.classnamestr.size() > 0)
        m_alg_infor.classnamestr.clear();

    if(m_alg_infor.m_map_int_obj.size() >0) m_alg_infor.m_map_int_obj.clear();
    if(m_alg_infor.m_map_str_obj.size() >0) m_alg_infor.m_map_str_obj.clear();

    delete ui;
}

int jubushibie_form::update_property(properconfig *  proper_config)
{

#if USING_PAIJICI_FABU

    QtProperty * prp_1 =  proper_config->idToProperty[QObject::tr("拍照几次才计算")];

    int pzjc_js= proper_config->m_pVarManager->value(prp_1).toInt();
    m_alg_infor.paizhao_jici = pzjc_js;

#endif


    for(int i = 0; i <  m_alg_infor.classnamestr.size(); i++)
    {
        OBJ_infor objInfor;

        QString name_class_title=QObject::tr("类别名称")+"_" +QString::number(i) +QObject::tr("号");

        QMap<QString, QtProperty *>  class_infor = proper_config->m_classNameToIdToProperty[name_class_title];
        QtProperty * prp =  class_infor[name_class_title];

        //英文名称
        QString target_name= proper_config->m_pVarManager->value(prp).toString();
        objInfor.class_name = target_name;


        prp =  class_infor[QObject::tr("中文名称")];
        QString target_ch_name= proper_config->m_pVarManager->value(prp).toString();
        objInfor.class_name_ch = target_ch_name;


        prp =  class_infor[QObject::tr("是否启用")];
        bool bool_name= proper_config->m_pVarManager->value(prp).toBool();
        objInfor.if_enable = bool_name;


        prp = class_infor[QObject::tr("得分最小值")];
        double d_name= proper_config->m_pVarManager->value(prp).toDouble();
        objInfor.min_relation_q = d_name;

        prp =  class_infor[QObject::tr("得分最大值")];
        d_name= proper_config->m_pVarManager->value(prp).toDouble();
        objInfor.max_relation_q = d_name;

        prp =  class_infor[QObject::tr("目标框最小值")];
        QSizeF s_name= proper_config->m_pVarManager->value(prp).toSizeF();
        objInfor.min_size =s_name;

        prp = class_infor[QObject::tr("目标框最大值")];
        s_name= proper_config->m_pVarManager->value(prp).toSizeF();
        objInfor.max_size =s_name;

        prp =  class_infor[QObject::tr("面积最小值")];
        d_name= proper_config->m_pVarManager->value(prp).toDouble();
        objInfor.min_area =d_name;


        prp =  class_infor[QObject::tr("面积最大值")];
        d_name= proper_config->m_pVarManager->value(prp).toDouble();
        objInfor.max_area =d_name;

        prp =  class_infor[QObject::tr("宽当量mm")];
        d_name= proper_config->m_pVarManager->value(prp).toDouble();
        objInfor.dangliang_width =d_name;

        prp =  class_infor[QObject::tr("高当量mm")];
        d_name= proper_config->m_pVarManager->value(prp).toDouble();
        objInfor.dangliang_height =d_name;


        prp =  class_infor[QObject::tr("是否乘以当量")];
        bool_name= proper_config->m_pVarManager->value(prp).toBool();
        objInfor.if_mul_dangliang =bool_name;


        //目前只有一个感兴趣区域
        prp =  class_infor[QObject::tr("是否启用ROI")];
        bool_name= proper_config->m_pVarManager->value(prp).toBool();
        objInfor.map_if_enable_roi[0] =bool_name;
        prp =  class_infor[QObject::tr("感兴趣区域ROI")];
        QRectF r_name= proper_config->m_pVarManager->value(prp).toRectF();
        objInfor.map_roi[0] =r_name;

        objInfor.roi_num =  objInfor.map_if_enable_roi.size();
        m_alg_infor.m_map_int_obj[i] = objInfor;
        m_alg_infor.m_map_str_obj[target_name] =objInfor;


    }
    return 0;

}



int jubushibie_form::alg_intial()
{

    // fx_initial_gpu(m_alg_infor.gpu_id, m_alg_infor.m_model_id_int, /*(char*)path.c_str()*/ (char*)m_alg_infor.alg_model_path.toStdString().c_str());
    // cout<<"GPU ID---------->"<<m_alg_infor.gpu_id<<endl;




#if IF_USING_TENSORT

    fx_initial_gpu_rt(m_alg_infor.gpu_id, m_alg_infor.m_model_id_int, /*(char*)path.c_str()*/ (char*)m_alg_infor.alg_model_path.toStdString().c_str(), 352,352);

    for (int i = 0; i < 10; i++)
    {

        int width = net_w;
        int height =net_h;
        cv::Mat yure_mt = cv::Mat::zeros(height, width, CV_8UC3);
        //cv::Mat temp_out_img = cv::Mat(height, width, CV_8UC1, cv::Scalar(0));
        int re = fx_alg_processing_rt(m_alg_infor.m_model_id_int,(char *)yure_mt.data, yure_mt.cols, yure_mt.rows, 3);
        std::cout << "pre HOT GPU:" << i << std::endl;


    }



#else
    fx_initial_gpu(m_alg_infor.gpu_id, m_alg_infor.m_model_id_int, /*(char*)path.c_str()*/ (char*)m_alg_infor.alg_model_path.toStdString().c_str());

#endif


    m_alg_be_call_num++;
    std::cout<<"initial finish"<< std::endl;
    return 0;
}

int jubushibie_form::alg_processing(char* data, int width, int height, int channel)
{

    if(channel==1)
    {
        // src_image =cv::Mat(height,width,CV_8UC1);
        src_image =cv::Mat(height,width,CV_8UC3);
        Mat col_m =cv::Mat(height,width,CV_8UC1,data);

        cv::cvtColor(col_m, src_image, CV_GRAY2RGB);
        //  src_image = cv::Mat(height,width,CV_8UC1,data).clone();
    }else
    {
        src_image =cv::Mat(height,width,CV_8UC3);
        src_image = cv::Mat(height,width,CV_8UC3,data).clone();
    }

    real_detect_obj_num = 0;



#if IF_USING_TENSORT

    big_width = width;
    big_height = height;

    real_detect_obj_num=fx_alg_processing_rt( m_alg_infor.m_model_id_int, (char *)src_image.data,  width,  height,  3);//这里这个通道数,如果已经转成彩色,就得用3,而不是输入的channel值

#else
    real_detect_obj_num=fx_alg_processing( m_alg_infor.m_model_id_int, (char *)src_image.data,  width,  height,  3);//这里这个通道数,如果已经转成彩色,就得用3,而不是输入的channel值
#endif


    return real_detect_obj_num;
}



//处理结果步骤,后期转化成方法,每一种方法都要有输出,后面组合成一个方法
int jubushibie_form::add_chuli_jieguo()
{
    // E_NG, E_OK,E_UNKONW
    if(out_put_ng.size() >0)
    {

        result_info.frame_index =frame_index;
        result_info.m_reslt_state_flag[ frame_index ] = E_HAVE_OBJ;

        result_info.m_reslt_src_name[ frame_index ] = source_name;
        result_info.m_reslt_task_name[ frame_index ] = m_task_id_str;
        result_info.m_reslt_unit_name[ frame_index ] = m_unit_id_str;



    }else
    {

        result_info.frame_index =frame_index;
        result_info.m_reslt_state_flag[ frame_index ] = E_NO_OBJ;
        result_info.m_reslt_src_name[ frame_index ] = source_name;
        result_info.m_reslt_task_name[ frame_index ] = m_task_id_str;
        result_info.m_reslt_unit_name[ frame_index ] = m_unit_id_str;

    }
    curent_frame_id = frame_index;

    frame_index++;
    if( frame_index > MAX_REC_NUM_BUF) frame_index =0;

    return  0;
}


  int jubushibie_form::add_chuli_jieguo_queue(int x)
  {

      if(x >0)
      {

          current_str_temp.m_reslt_state_flag = E_HAVE_OBJ;
          current_str_temp.m_reslt_src_name = source_name;
          current_str_temp.m_reslt_task_name = m_task_id_str;
          current_str_temp.m_reslt_unit_name = m_unit_id_str;
          current_str_temp.paizhaojici = m_alg_infor.paizhao_jici;


          m_shibie[m_unit_id_str].push(current_str_temp);

      }else
      {

          current_str_temp.m_reslt_state_flag = E_NO_OBJ;
          current_str_temp.m_reslt_src_name = source_name;
          current_str_temp.m_reslt_task_name = m_task_id_str;
          current_str_temp.m_reslt_unit_name = m_unit_id_str;

          current_str_temp.paizhaojici = m_alg_infor.paizhao_jici;


          m_shibie[m_unit_id_str].push(current_str_temp);

      }



      return    m_shibie[m_unit_id_str].size();


  }
int jubushibie_form::add_chuli_jieguo_queue()
{


    if(out_put_ng.size() >0)
    {


        current_str_temp.m_reslt_state_flag = E_HAVE_OBJ;
        current_str_temp.m_reslt_src_name = source_name;
        current_str_temp.m_reslt_task_name = m_task_id_str;
        current_str_temp.m_reslt_unit_name = m_unit_id_str;
        current_str_temp.paizhaojici = m_alg_infor.paizhao_jici;
        // map_q_result_info[m_unit_id_str].push(current_str_temp);

        //m_duorenwu_jieguo[m_unit_id_str].push_back(current_str_temp);

        m_shibie[m_unit_id_str].push(current_str_temp);

    }else
    {

        current_str_temp.m_reslt_state_flag = E_NO_OBJ;
        current_str_temp.m_reslt_src_name = source_name;
        current_str_temp.m_reslt_task_name = m_task_id_str;
        current_str_temp.m_reslt_unit_name = m_unit_id_str;

        current_str_temp.paizhaojici = m_alg_infor.paizhao_jici;
        // map_q_result_info[m_unit_id_str].push(current_str_temp);
        // m_duorenwu_jieguo[m_unit_id_str].push_back(current_str_temp);

        m_shibie[m_unit_id_str].push(current_str_temp);

    }



    return    m_shibie[m_unit_id_str].size();

}

int jubushibie_form::del_chuli_jieguo_queue()
{


    //  while (!m_duorenwu_jieguo[m_unit_id_str].empty()) m_duorenwu_jieguo[m_unit_id_str].pop_front();

    while (!m_shibie[m_unit_id_str].empty()) m_shibie[m_unit_id_str].pop();

    // while (!m_shibie_jieguo[m_unit_id_str].empty()) m_shibie_jieguo[m_unit_id_str].pop_front();

    while (!m_shibie_jieguo[m_unit_id_str].empty()) m_shibie_jieguo[m_unit_id_str].pop();



    return 0;


}
int jubushibie_form::qingkong_queue()
{

    //if(m_duorenwu_jieguo[m_unit_id_str].size() > 0) m_duorenwu_jieguo[m_unit_id_str].clear();

    //if(m_shibie[m_unit_id_str].size() > 0) m_shibie[m_unit_id_str].clear();

    //if(m_shibie_jieguo[m_unit_id_str].size() > 0) m_shibie_jieguo[m_unit_id_str].clear();

    while (!m_shibie[m_unit_id_str].empty()) m_shibie[m_unit_id_str].pop();
    while (!m_shibie_jieguo[m_unit_id_str].empty()) m_shibie_jieguo[m_unit_id_str].pop();



    return 0;
}



int  jubushibie_form::jisuan_mat_thread(void * ptm)
{

    return  0;
}


#if IF_USING_TENSORT

const int SIZE_CHAE = 512;
int JieMix(std::string infile, std::string outfile)
{
    //cout << "解密文件名: " << infile << "   解密后输出文件名: " << outfile << endl;
    std::fstream fio(infile.c_str(), std::ios::in);

    if (!fio)
    {
        std::cerr << "文件打开失败" << std::endl;
        return -1;
    }

    std::ofstream fout(outfile.c_str());
    if (!fout)
    {
        std::cerr << "文件打开失败" << std::endl;
        return -1;
    }

    char chArry[SIZE_CHAE];
    int readNum = 0;

    while (!fio.eof())
    {
        fio.read(chArry, sizeof(chArry));
        readNum = fio.gcount();
        for (int i = 0; i < readNum; i++)
        {
            chArry[i] = chArry[i] - 66;
            chArry[i] = ~chArry[i];
        }
        fout.write(chArry, readNum);
    }

    fout.close();
    fio.close();
    std::cout << "处理完成" << std::endl;
    return 0;


}




int jubushibie_form::fx_initial_gpu_rt_x(int gpu_id, int cam_id,  char* path_input)
{


    std::string cfg_filename_derect = std::string(path_input);

    JieMix(cfg_filename_derect + "/sm/model/obj-yuanjing-bobo_zjx.zou", cfg_filename_derect + "/sm/model/obj-yuanjing-bobo_zjy.cfg");

    std::string cfg_filename = cfg_filename_derect + "/sm/model/obj-yuanjing-bobo_zjy.cfg";
    std::string weight_filename = cfg_filename_derect + "/sm/model/backup/obj-yuanjing-biaomian_final.weights";
    std::string objname = cfg_filename_derect + "/sm/model/lianti/obj.names";
    string filename =objname;

    cout<<cfg_filename<<endl;
    cout<<weight_filename<<endl;
    cout<<objname<<endl;



    if (_access(cfg_filename.c_str(), 0) == -1)
    {
        return -1;
    }

    if (_access(weight_filename.c_str(), 0) == -1)
    {
        return -1;
    }


    if (_access(objname.c_str(), 0) == -1)
    {
        return -1;
    }

    ifstream file(filename,ifstream::in);
    string line,classLabel;//行、路径、标签

    while(getline(file,line)){
        stringstream liness(line);
        getline(liness,classLabel);
        labels.push_back(classLabel);
    }
    file.close();


    //Config config_v4;

    config_v4.gpu_id = gpu_id;
    //    config_v4.net_type = YOLOV4;
    //    config_v4.file_model_cfg = cfg_filename;//"../configs/sample0/sm/model/obj-xiaoke-bobo.cfg";
    //    config_v4.file_model_weights = weight_filename;//"../configs/sample0/sm/model/backup/obj-xiaoke-biaomian_final.weights";
    //    config_v4.calibration_image_list_file_txt = "./save_train.txt";
    //    config_v4.inference_precison = flagx;// INT8;
    //    config_v4.detect_thresh = yuzhi;

    config_v4.net_type = YOLOV4;
    config_v4.file_model_cfg = "K:/tianchongbeifen/trt/yolo-tensorrt-master/configs/sample0/sm/model/obj-xiaoke-bobo_zjy.cfg";
    config_v4.file_model_weights = "k:/tianchongbeifen/trt/yolo-tensorrt-master/configs/sample0/sm/model/backup/obj-xiaoke-biaomian_final.weights";
    config_v4.calibration_image_list_file_txt = "./save_train.txt";
    config_v4.inference_precison = flagx;// INT8;
    config_v4.detect_thresh = yuzhi;

    detector = std::make_unique<Detector>();

    detector->init(config_v4);


    for (int i = 0; i < 10; i++)
    {

        int width = net_w;
        int height =net_h;
        cv::Mat yure_mt = cv::Mat::zeros(height, width, CV_8UC3);
        //cv::Mat temp_out_img = cv::Mat(height, width, CV_8UC1, cv::Scalar(0));
        int re = fx_alg_processing_rt_x(cam_id,(char *)yure_mt.data, yure_mt.cols, yure_mt.rows, 3);
        std::cout << "pre HOT GPU:" << i << std::endl;


    }




    //    // std::string cfg_filename = cfg_filename_derect + "/sm/model/obj-xiaoke-bobo_zjy.zou";
    //    int flag = remove(cfg_filename.c_str());  /*remove()函数返回值为0或EPF(-1)*/
    //    if (flag == 0) std::cout << "D Ok!" << std::endl;
    //    else
    //    {
    //        std::cout << "D F!" << std::endl;

    //    }

    std::cout << "initial ok!" << std::endl;


    return 0;
}
int jubushibie_form::fx_alg_processing_rt_x(int cam_id, char* data, int width, int height, int channel)
{

    if (data != NULL)
    {
        std::vector<cv::Mat> batch_img;
        if (channel == 3)
        {
            cv::Mat matImage( cvSize(width, height), CV_8UC3,data );

            batch_img.push_back(matImage);


        }
        else
        {
            cv::Mat matImageGray( cvSize(width, height),  CV_8UC1, data);
            cv::Mat matImage(  cvSize(width, height),CV_8UC3);
            cv::cvtColor(matImageGray, matImage, cv::COLOR_GRAY2BGR);
            batch_img.push_back(matImage);

        }

        big_width =batch_img[0].cols;
        big_height = batch_img[0].rows;
        if(batch_res.size() > 0) batch_res.clear();

        detector->detect(batch_img, batch_res);
        if(batch_res.size() > 0)  return  batch_res[0].size();
    }
    return 0;
}


bool jubushibie_form::fx_get_detection_result_gpu_cam_x_rt_x(struct tr_s_resultinfo* Result, int Num, int cam_id)
{

    if (batch_res.size() > 0)
    {

        BatchResult read_r=batch_res[0];

        //增加 修改
        for (int i = 0; i < read_r.size(); i++)
        {

            std::string str_classname = labels[read_r[i].id];
            std::stringstream sstr;
            sstr.clear();
            sstr << str_classname;//将s的值放到stringstream
            sstr >> Result[i].classname;//类型名
            Result[i].classID =read_r[i].id;
            Result[i].prob = read_r[i].prob;

            Result[i].x =read_r[i].rect.x;

            if (Result[i].x < 0) Result[i].x = 0;
            if (Result[i].x > big_width - 1) Result[i].x = big_width - 1;

            Result[i].y = read_r[i].rect.y;

            if (Result[i].y < 0) Result[i].y = 0;
            if (Result[i].y > big_height - 1) Result[i].y = big_height - 1;


            Result[i].w = read_r[i].rect.width;

            if (Result[i].w < 0) Result[i].w = 0;
            if (Result[i].w > big_width - 1) Result[i].w = big_width - 1;


            Result[i].h = read_r[i].rect.height;
            if (Result[i].h < 0) Result[i].h = 0;
            if (Result[i].h > big_height - 1) Result[i].h = big_height - 1;
        }

    }

    return 0;
}
int jubushibie_form::fx_release_detection_gpu_x_cam_x_rt_x(int cam_id)
{
    detector.release();
    return 0;
}
#endif


void jubushibie_form::clear_img_queue()
{


    jubu_m_paizhao_jici = 0;
    while (!m_duo_paizhao_Queue.q_mat_tupian.empty())
    {
        m_duo_paizhao_Queue.q_mat_tupian.pop();
    }

    while (!m_duo_paizhao_Queue.q_jieguo.empty())
    {
        m_duo_paizhao_Queue.q_jieguo.pop();
    }

}

void jubushibie_form::jisuan_xiancheng(void * ptm)
{

    while ( jisuan_xiancheng_flag)
    {
        if(!m_duo_paizhao_Queue.q_mat_tupian.empty())
        {

            double t = (double)cvGetTickCount();

#if 0

            cv::Mat jsm =m_duo_paizhao_Queue.q_mat_tupian.front();
            alg_processing((char*)jsm.data, jsm.cols, jsm.rows, jsm.channels());
            alg_get_result(); //主要获取out_put_ng，， 包含 filter_result
            add_chuli_jieguo_queue();//q_result_info

            emit send_result_next(src_image, m_alg_infor , out_put_ng);//向下一单元输出 这里存在一个问题,可能src_image没有显示就销毁了
            emit send_to_tongji_unit_queue(m_shibie[m_unit_id_str] /*发送的这个变量没有用到*/, out_put_ng);//向统计单元输出 pre_alg_unix_input_queue 最后统计所有相机结果,以便判断是否ok或ng
            //m_shibie_jieguo[m_unit_id_str].push(out_put_ng);
            m_duo_paizhao_Queue.q_mat_tupian.pop();

            t = (double)cvGetTickCount() - t;
            printf( "detection time = %gms\n",  t/((double)cvGetTickFrequency()*1000.) );

#else

#if IF_USING_TENSORT

            cv::Mat jsm =m_duo_paizhao_Queue.q_mat_tupian.front();


            float scale_w=1.0;
            float scale_h =1.0;

            int xnum =alg_processing((char*)jsm.data, jsm.cols, jsm.rows, jsm.channels());
            std::vector<tr_s_resultinfo>   xout_put_ng;
            alg_get_result(  xout_put_ng,  xnum);
           // alg_get_result(); //主要获取out_put_ng，， 包含 filter_result
            add_chuli_jieguo_queue(xout_put_ng.size());//q_result_info

            m_alg_infor.sc_w =scale_w;
            m_alg_infor.sc_h =scale_h;


            emit send_result_next(jsm, m_alg_infor , xout_put_ng);//向下一单元输出 这里存在一个问题,可能src_image没有显示就销毁了
            emit send_to_tongji_unit_queue(m_shibie[m_unit_id_str] /*发送的这个变量没有用到*/, xout_put_ng);//向统计单元输出 pre_alg_unix_input_queue 最后统计所有相机结果,以便判断是否ok或ng

            m_duo_paizhao_Queue.q_mat_tupian.pop();

            t = (double)cvGetTickCount() - t;
            printf( "detection time = %gms\n",  t/((double)cvGetTickFrequency()*1000.) );



#else

#if SHIFOU_SUOFANG
            cv::Mat jsm =m_duo_paizhao_Queue.q_mat_tupian.front();

            float scale_w= (float)jsm.cols / (float)res_w;
            float scale_h =(float)jsm.rows / (float)res_h;

            cv::resize(jsm,resize_img,cv::Size(res_w, res_h),0, 0,INTER_LINEAR);

            alg_processing((char*)resize_img.data, resize_img.cols, resize_img.rows, resize_img.channels());
            alg_get_result(); //主要获取out_put_ng，， 包含 filter_result
            add_chuli_jieguo_queue();//q_result_info

            m_alg_infor.sc_w =scale_w;
            m_alg_infor.sc_h =scale_h;


            emit send_result_next(jsm, m_alg_infor , out_put_ng);//向下一单元输出 这里存在一个问题,可能src_image没有显示就销毁了
            emit send_to_tongji_unit_queue(m_shibie[m_unit_id_str] /*发送的这个变量没有用到*/, out_put_ng);//向统计单元输出 pre_alg_unix_input_queue 最后统计所有相机结果,以便判断是否ok或ng
            //m_shibie_jieguo[m_unit_id_str].push(out_put_ng);
            m_duo_paizhao_Queue.q_mat_tupian.pop();

            t = (double)cvGetTickCount() - t;
            printf( "detection time = %gms\n",  t/((double)cvGetTickFrequency()*1000.) );


#endif


#endif




#endif
        }else
        {
            Sleep(10);

        }
    }

    return;
}


//如果 某个工位 拍 n次,怎么弄?
void  jubushibie_form::pre_unix_input(cv::Mat img,int index)//接收上一单元数据
{

    jubu_m_paizhao_jici++;
    m_duo_paizhao_Queue.q_mat_tupian.push(img);

    m_duo_paizhao_Queue.frame_index = jubu_m_paizhao_jici;
    m_duo_paizhao_Queue.unit_name = m_unit_id_str;

}
int jubushibie_form::filter_result(std::vector<tr_s_resultinfo> resultsrtForsort,std::vector<tr_s_resultinfo> &output_vc_resultsrtForsort)
{


    if(output_vc_resultsrtForsort.size() >0) output_vc_resultsrtForsort.clear();

    if (resultsrtForsort.size() > 0) {

        //增加 修改
        for (int i = 0; i < resultsrtForsort.size(); i++)
        {
            OBJ_infor sign_obj =m_alg_infor.m_map_int_obj[resultsrtForsort[i].classID];

            if( sign_obj.if_enable == 1)
            {
                if (resultsrtForsort[i].prob >=sign_obj.min_relation_q  &&  resultsrtForsort[i].prob <= sign_obj.max_relation_q)
                {


                    double obj_rect_w = 0;
                    double obj_rect_h = 0;
                    double obj_rect_are = 0;

                    cv::Point2f pt_lt={0.0};
                    cv::Point2f pt_lb={0.0};
                    cv::Point2f pt_rt={0.0};
                    cv::Point2f pt_rb={0.0};

                    if(sign_obj.if_mul_dangliang)//实际量
                    {
                        xj_dangliang[camera_name].w =sign_obj.dangliang_width;
                        xj_dangliang[camera_name].h =sign_obj.dangliang_height;
                        obj_rect_w = resultsrtForsort[i].w * sign_obj.dangliang_width;
                        obj_rect_h = resultsrtForsort[i].h *sign_obj.dangliang_height;
                        obj_rect_are =obj_rect_w *obj_rect_h;

                        pt_lt = cv::Point2f( resultsrtForsort[i].x * sign_obj.dangliang_width,resultsrtForsort[i].y*sign_obj.dangliang_height);
                        pt_lb = cv::Point2f( resultsrtForsort[i].x * sign_obj.dangliang_width,(resultsrtForsort[i].y +resultsrtForsort[i].h) *sign_obj.dangliang_height);
                        pt_rt = cv::Point2f( (resultsrtForsort[i].x + resultsrtForsort[i].w) * sign_obj.dangliang_width,resultsrtForsort[i].y  *sign_obj.dangliang_height);
                        pt_rb = cv::Point2f( (resultsrtForsort[i].x + resultsrtForsort[i].w) * sign_obj.dangliang_width,(resultsrtForsort[i].y +resultsrtForsort[i].h) *sign_obj.dangliang_height);
                    }else //像素量
                    {

                        xj_dangliang[camera_name].w =1.0;
                        xj_dangliang[camera_name].h =1.0;

                        obj_rect_w = resultsrtForsort[i].w ;
                        obj_rect_h = resultsrtForsort[i].h ;
                        obj_rect_are = obj_rect_w * obj_rect_h;

                        pt_lt = cv::Point2f( resultsrtForsort[i].x ,resultsrtForsort[i].y);
                        pt_lb = cv::Point2f( resultsrtForsort[i].x ,(resultsrtForsort[i].y +resultsrtForsort[i].h) );
                        pt_rt = cv::Point2f( (resultsrtForsort[i].x + resultsrtForsort[i].w) ,resultsrtForsort[i].y  );
                        pt_rb = cv::Point2f( (resultsrtForsort[i].x + resultsrtForsort[i].w) ,(resultsrtForsort[i].y +resultsrtForsort[i].h));
                    }


                    bool area_panduan = false; //面积判断
                    bool  h_panduan =false; //h判断
                    bool  w_panduan =false;//w判断
                    bool  in_roi_puand =false;

                    bool AreaConform_min = sign_obj.min_area <= obj_rect_are;
                    bool AreaConform_max = sign_obj.max_area  >= obj_rect_are;
                    if(AreaConform_min &&AreaConform_max ) area_panduan = true;

                    bool HeightConform_min =sign_obj.min_size.height() <= obj_rect_h;
                    bool HeightConform_max =sign_obj.max_size.height() >= obj_rect_h;
                    if(HeightConform_min && HeightConform_max ) h_panduan = true;


                    bool WidthConform_min = sign_obj.min_size.width() <= obj_rect_w;
                    bool WidthConform_max = sign_obj.max_size.width() >= obj_rect_w;
                    if( WidthConform_min && WidthConform_max ) w_panduan = true;


                    //目前只有一个roi
                    for(int kr= 0; kr < sign_obj.roi_num; kr++)
                    {

                        if(sign_obj.map_if_enable_roi[kr])
                        {

                            QRectF temp = sign_obj.map_roi[kr];
                            if(sign_obj.if_mul_dangliang)//实际量
                            {

                                QRectF needrf =QRectF( QPointF(pt_lt.x, pt_lt.y),  QSizeF(obj_rect_w, obj_rect_h));


                                temp.setRect(temp.x() * sign_obj.dangliang_width,
                                             temp.y() * sign_obj.dangliang_height,
                                             temp.width() * sign_obj.dangliang_width,
                                             temp.height() * sign_obj.dangliang_height);

                                if(temp.contains(needrf))
                                {

                                    in_roi_puand = true;
                                }

                            }else
                            {

                                QRectF needrf =QRectF( QPointF(pt_lt.x, pt_lt.y),  QSizeF(obj_rect_w, obj_rect_h));

                                // qDebug()<<needrf;
                                if(temp.contains(needrf))
                                {
                                    in_roi_puand = true;
                                }

                            }

                        }else
                        {
                            in_roi_puand = true;
                        }
                    }

                    // std::cout<<" "<<in_roi_puand<<" "<<area_panduan<<" "<<h_panduan<<" "<<w_panduan<<std::endl;
                    if(in_roi_puand)
                    {
                        if ((area_panduan || h_panduan || w_panduan))
                        {

                            output_vc_resultsrtForsort.push_back(resultsrtForsort[i]);
                        }

                    }

                }
            }
        }//遍历目标
    }//目标数是否大于0

    return output_vc_resultsrtForsort.size();
}

 bool jubushibie_form::alg_get_result(std::vector<tr_s_resultinfo>&  xout_put_ng,int xnum)
 {
     if(xout_put_ng.size()>0) xout_put_ng.clear();//每张图片做后要清零

     bool re_result = true;
     tr_s_resultinfo* resultsrtForsort = (tr_s_resultinfo*)malloc(real_detect_obj_num * sizeof(tr_s_resultinfo));


 #if IF_USING_TENSORT
     int re = fx_get_detection_result_gpu_cam_x_rt(resultsrtForsort, real_detect_obj_num, m_alg_infor.m_model_id_int,big_width, big_height);

 #else
     int re = fx_get_detection_result_gpu_cam_x(resultsrtForsort, real_detect_obj_num, m_alg_infor.m_model_id_int);
 #endif


     std::vector<tr_s_resultinfo> vc_resultsrtForsort;

     for (int i = 0; i < xnum; i++)
     {
         tr_s_resultinfo temp = resultsrtForsort[i];
         vc_resultsrtForsort.push_back(temp);
     }
     free(resultsrtForsort);


     if(vc_resultsrtForsort.size() >0)
     {
         filter_result(vc_resultsrtForsort, xout_put_ng);

     }

     return re_result;
 }


bool jubushibie_form::alg_get_result()
{
    if(out_put_ng.size()>0) out_put_ng.clear();//每张图片做后要清零

    bool re_result = true;
    tr_s_resultinfo* resultsrtForsort = (tr_s_resultinfo*)malloc(real_detect_obj_num * sizeof(tr_s_resultinfo));


#if IF_USING_TENSORT
    int re = fx_get_detection_result_gpu_cam_x_rt(resultsrtForsort, real_detect_obj_num, m_alg_infor.m_model_id_int,big_width, big_height);

#else
    int re = fx_get_detection_result_gpu_cam_x(resultsrtForsort, real_detect_obj_num, m_alg_infor.m_model_id_int);
#endif


    std::vector<tr_s_resultinfo> vc_resultsrtForsort;

    for (int i = 0; i < real_detect_obj_num; i++)
    {
        tr_s_resultinfo temp = resultsrtForsort[i];
        vc_resultsrtForsort.push_back(temp);
    }
    free(resultsrtForsort);


    if(vc_resultsrtForsort.size() >0)
    {
        filter_result(vc_resultsrtForsort, out_put_ng);

    }

    return re_result;
}


int jubushibie_form::release_alg()
{



#if IF_USING_TENSORT
    fx_release_detection_gpu_x_cam_x_rt(m_alg_infor.m_model_id_int);
#else
    fx_release_detection_gpu_x_cam_x(m_alg_infor.m_model_id_int);
#endif


    m_alg_be_call_num = 0;
    return 0;
}
int jubushibie_form::fx_initial_gpu(int gpu_id, int cam_id, char* path_input)
{
    inital_detection_gpu_x_cam_x(gpu_id, cam_id, path_input);
    return 0;
}
int jubushibie_form::fx_alg_processing(int cam_id, char* data, int width, int height, int channel)
{
    int num = image_detection_gpu_x_cam_x(cam_id, data, width, height, channel);
    return  num;
}
bool jubushibie_form::fx_get_detection_result_gpu_cam_x(struct tr_s_resultinfo* Result, int Num, int cam_id)
{
    bool xx = get_detection_result_gpu_cam_x(Result, Num, cam_id);
    return xx;
}
int jubushibie_form::fx_release_detection_gpu_x_cam_x(int cam_id)
{
    release_detection_gpu_x_cam_x(cam_id);
    return 0;
}

void  jubushibie_form::xianshi(cv::Mat img,int index)
{

    show_img_xx(img);
    emit send_mat_to_main(img, index);

}

void jubushibie_form::show_img_xx(cv::Mat img)
{

    if(img.type()==CV_8UC3)
    {
        // QImage imgx((const uchar*)img.data, img.cols, img.rows,img.cols*img.channels(),QImage::Format_RGB888);
        // QPixmap my_pixmap = QPixmap::fromImage(imgx).scaledToWidth( ui->graphicsView->width());
        //  m_graphicsScene->addPixmap( my_pixmap);
        //ui->graphicsView->setPixmap(my_pixmap);
    }else
    {
        // QImage imgx((const uchar*)img.data, img.cols, img.rows,img.cols*img.channels(),QImage::Format_Indexed8);
        // QPixmap my_pixmap = QPixmap::fromImage(imgx).scaledToWidth( ui->graphicsView->width());
        //ui->graphicsView->setPixmap(my_pixmap);
        // m_graphicsScene->addPixmap( my_pixmap);
    }

}



void  jubushibie_form::fx_show_img(cv::Mat  &show_temp_img ,int flag)
{


    cv::RNG rng(10000);
    //ui->label_xianshi->setParent(imgL);
    cv::Mat img = cv::Mat(show_temp_img.size(),show_temp_img.type());
    img = show_temp_img.clone();
    //emit send_save_img_sig(img , 1);
    int class_num =m_alg_infor.classnamestr.size();
    vector<cv::Scalar > rgb_vc;
    for (int i = 0; i < class_num; i++)
    {
        // cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        cv::Scalar color(rand()&255, rand()&255, rand()&255);
        rgb_vc.push_back(color);
    }
    if(flag == 1)
    {
        for(int i =0 ; i < m_alg_infor.m_map_int_obj.size(); i++)
        {
            OBJ_infor temp = m_alg_infor.m_map_int_obj[i];
            for(int k = 0; k < temp.map_roi.size(); k++ )
            {

                // cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
                QRectF rcf=temp.map_roi[k];
                cv::Rect cvrc = cv::Rect(rcf.x(), rcf.y(),rcf.width(), rcf.height());

                cv::rectangle(img, cvrc, rgb_vc[k], 20);
            }
        }

        int set_count = 0;

        if(m_put_ng.size() >0)
        {

            for (int i = 0; i < m_put_ng.size(); i++)
            {
                cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));


                float cal_w = m_put_ng[i].w ;
                float cal_h = m_put_ng[i].h ;
                float cal_are = cal_w * cal_h;

                int classes = 13;
                int classIDtem = m_put_ng[i].classID;
                int offset = i * 123457 % classes;
                float red = get_color(2, offset, classes);
                float green = get_color(1, offset, classes);
                float blue = get_color(0, offset, classes);
                float rgb[3];

                rgb[0] = red;
                rgb[1] = green;
                rgb[2] = blue;

                int left = (m_put_ng[i].x);
                int right = (m_put_ng[i].x + m_put_ng[i].w);
                int top = (m_put_ng[i].y);
                int bot = (m_put_ng[i].y + m_put_ng[i].h);

                if (left < 0) left = 0;
                if (right > img.cols - 1) right = img.cols - 1;
                if (top < 0) top = 0;
                if (bot > img.rows - 1) bot = img.rows - 1;

                float const font_size = img.rows / 1000.F;
                cv::Point pt1, pt2, pt_text, pt_text_bg1, pt_text_bg2;
                pt1.x = left;
                pt1.y = top;
                pt2.x = right;
                pt2.y = bot;
                pt_text.x = left;
                pt_text.y = top - 12;
                pt_text_bg1.x = left;
                pt_text_bg1.y = top - (10 + 25 * font_size);
                pt_text_bg2.x = right;
                pt_text_bg2.y = top;
                //                cv::Scalar color;
                //                color.val[0] = red * 256;
                //                color.val[1] = green * 256;
                //                color.val[2] = blue * 256;

                cv::Scalar black_color;
                black_color.val[0] = 0;
                black_color.val[1] = 255;
                black_color.val[0] = 128;

                int font_face = cv::FONT_HERSHEY_COMPLEX;
                double font_scale = 0.65;
                int thickness = 2;
                std::string str_id = std::to_string(i) + ":" + m_put_ng[i].classname +
                        " w:" + doubleToString(cal_w) + " h:" + doubleToString(cal_h) + " area:" + doubleToString(cal_are) +
                        " prob:" + doubleToString(m_put_ng[i].prob);
                std::string str_idx = std::to_string(i);
                QString xxstr = QString::fromStdString(str_id);
                //emit ptui->Sig_Send_for_show_msg(xxstr);

                //int baseline;
                ////获取文本框的长宽
                //cv::Size text_size = cv::getTextSize(text, font_face, font_scale, thickness, &baseline);///
                cv::Point2f tr_pt[4];
                tr_pt[0].x = m_put_ng[i].pt_x_0;
                tr_pt[0].y = m_put_ng[i].pt_y_0;

                tr_pt[1].x = m_put_ng[i].pt_x_1;
                tr_pt[1].y = m_put_ng[i].pt_y_1;

                tr_pt[2].x = m_put_ng[i].pt_x_2;
                tr_pt[2].y = m_put_ng[i].pt_y_2;

                tr_pt[3].x = m_put_ng[i].pt_x_3;
                tr_pt[3].y = m_put_ng[i].pt_y_3;


                cv::rectangle(img, pt1, pt2, color, 4, 8);


                /*for (int k = 0; k < 4; k++) {
                                        cv::line(src_img, tr_pt[k], tr_pt[(k + 1) % 4], cv::Scalar(0, 255, 0), 4, 8);
                                    }*/
                cv::putText(img, str_idx, pt_text, font_face, font_scale, cv::Scalar(0, 0, 255), thickness, 8, 0);

                float h_rows_start = (float)50;
                float step_pt = (float)img.rows / 40.0;
                cv::Point2f pt_show = cv::Point2f((float)img.cols / 400.0, h_rows_start + step_pt * set_count);


                cv::Point2f pt_show1 = cv::Point2f((float)img.cols - 400, 500);
                cv::putText(img, "NG", pt_show1, font_face, 3, cv::Scalar(255, 0, 0), thickness, 8, 0);
                //cv::FONT_HERSHEY_COMPLEX;


                set_count++;

                //emit send_save_img_sig(img  , 0 );



            }


        }
    }


    if(img.type()==CV_8UC3)
    {
        QImage imgx((const uchar*)img.data, img.cols, img.rows,img.cols*img.channels(),QImage::Format_RGB888);
        background = QPixmap::fromImage(imgx).scaledToWidth( ui->label_show->width());
        this->update();
        //ui->label_xianshi->setPixmap(my_pixmap);
        //ui->label_xianshi->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
        //ui->label_xianshi->setScaledContents(true);
    }else
    {
        QImage imgx((const uchar*)img.data, img.cols, img.rows,img.cols*img.channels(),QImage::Format_Indexed8);
        background = QPixmap::fromImage(imgx).scaledToWidth( ui->label_show->width());
        this->update();
        //ui->label_xianshi->setPixmap(my_pixmap);
        //ui->label_xianshi->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
        //ui->label_xianshi->setScaledContents(true);
    }
}

void jubushibie_form::on_act_open_image_triggered()
{

    QString OpenFile, OpenFilePath;
    QImage image;
    //打开文件夹中的图片文件
    OpenFile = QFileDialog::getOpenFileName(this,
                                            "Please choose an image file",
                                            "",
                                            "Image Files(*.jpg *.png *.bmp *.pgm *.pbm);;All(*.*)");
    if( OpenFile != "" )
    {
        m_img = imread(OpenFile.toStdString(),0);
        //if( image.load(OpenFile) )
        if(!m_img.empty())
        {
            // ui->label_show->setPixmap(QPixmap::fromImage(image));


            // alg_processing((char*)m_img.data, m_img.cols, m_img.rows, m_img.channels());
            // alg_get_result();
            // m_put_ng =out_put_ng;
            //emit send_result_next(src_image, m_alg_infor , out_put_ng);//向下一单元输出
            // fx_show_img(m_img ,1);

            if(m_img.type()==CV_8UC3)
            {
                QImage imgx((const uchar*)m_img.data, m_img.cols, m_img.rows,m_img.cols*m_img.channels(),QImage::Format_RGB888);
                background = QPixmap::fromImage(imgx).scaledToWidth( ui->label_show->width());
                this->update();

            }else
            {
                QImage imgx((const uchar*)m_img.data, m_img.cols, m_img.rows,m_img.cols*m_img.channels(),QImage::Format_Indexed8);
                background = QPixmap::fromImage(imgx).scaledToWidth( ui->label_show->width());
                this->update();

            }

        }
    }

    //显示所示图片的路径
    QFileInfo OpenFileInfo;
    OpenFileInfo = QFileInfo(OpenFile);
    OpenFilePath = OpenFileInfo.filePath();
    ui->lineEdit->setText(OpenFilePath);
}

void jubushibie_form::on_act_test_img_triggered()
{
    if(!m_img.empty())
    {
        alg_processing((char*)m_img.data, m_img.cols, m_img.rows, m_img.channels());
        alg_get_result();
        m_put_ng =out_put_ng;
        //emit send_result_next(src_image, m_alg_infor , out_put_ng);//向下一单元输出
        fx_show_img(m_img ,1);

    }
}

void jubushibie_form::contextMenuEvent(QContextMenuEvent *event)
{
    QPoint pos = event->pos();
    pos = this->mapToGlobal(pos);
    QMenu *menu = new QMenu(this);

    //QAction *loadImage = new QAction(tr("Load Image"));
    // QObject::connect(loadImage, &QAction::triggered, this, &ImageView::onLoadImage);
    // menu->addAction(loadImage);
    // menu->addSeparator();

    QAction *zoomInAction = new QAction(QString::fromLocal8Bit("缩小Zoom In"));
    QObject::connect(zoomInAction, &QAction::triggered, this, &jubushibie_form::onZoomInImage);
    menu->addAction(zoomInAction);

    QAction *zoomOutAction = new QAction(QString::fromLocal8Bit("放大Zoom Out"));
    QObject::connect(zoomOutAction, &QAction::triggered, this, &jubushibie_form::onZoomOutImage);
    menu->addAction(zoomOutAction);

    QAction *presetAction = new QAction(QString::fromLocal8Bit("复位Preset"));
    QObject::connect(presetAction, &QAction::triggered, this, &jubushibie_form::onPresetImage);
    menu->addAction(presetAction);

    menu->exec(pos);
}



void jubushibie_form::wheelEvent(QWheelEvent *event)
{
    int value = event->delta();
    if (value > 0)
        onZoomInImage();
    else
        onZoomOutImage();
    this->update();
}

void jubushibie_form::mousePressEvent(QMouseEvent *event)
{
    m_OldPos = event->pos();
    m_Pressed = true;
}

void jubushibie_form::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_Pressed)
        return QWidget::mouseMoveEvent(event);

    this->setCursor(Qt::SizeAllCursor);
    QPoint pos = event->pos();
    int xPtInterval = pos.x() - m_OldPos.x();
    int yPtInterval = pos.y() - m_OldPos.y();

    m_XPtInterval += xPtInterval;
    m_YPtInterval += yPtInterval;

    m_OldPos = pos;
    this->update();
}

void jubushibie_form::mouseReleaseEvent(QMouseEvent *event)
{
    m_Pressed = false;
    this->setCursor(Qt::ArrowCursor);
}

void jubushibie_form::onZoomInImage(void)
{
    m_ZoomValue += 0.2;
    this->update();
}

void jubushibie_form::onZoomOutImage(void)
{
    m_ZoomValue -= 0.2;
    if (m_ZoomValue <= 0)
    {
        m_ZoomValue += 0.2;
        return;
    }

    this->update();
}

void jubushibie_form::onPresetImage(void)
{
    m_ZoomValue = 1.0;
    m_XPtInterval = 0;
    m_YPtInterval = 0;
    this->update();
}

void jubushibie_form::paintEvent(QPaintEvent *event)
{
    // 绘制样式
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    if (background.isNull())
        return QWidget::paintEvent(event);


    // 根据窗口计算应该显示的图片的大小
    int width = qMin(background.width(), this->width());
    int height = width * 1.0 / (background.width() * 1.0 / background.height());
    height = qMin(height, this->height());
    width = height * 1.0 * (background.width() * 1.0 / background.height());

    // 平移
    painter.translate(this->width() / 2 + m_XPtInterval, this->height() / 2 + m_YPtInterval);
    // 缩放
    painter.scale(m_ZoomValue, m_ZoomValue);
    // 绘制图像
    QRect picRect(-width / 2, -height / 2, width, height);
    // painter.drawImage(picRect, m_Image);
    painter.drawPixmap(picRect, background);


    // QPainter painter(this);
    // painter.drawPixmap(startPos.x(), startPos.y(), background.scaled(background.width()*zoom,background.height()*zoom,Qt::KeepAspectRatio));

}


