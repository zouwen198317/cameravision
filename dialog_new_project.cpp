#include "dialog_new_project.h"
#include "ui_dialog_new_project.h"

using namespace std;
vector<string> filesfortrain_jpg;
void dir_file_find_jpg(string path)
{
    intptr_t hFile = 0;
    struct _finddata_t fileInfo;
    string pathName, exdName;


    if ((hFile = _findfirst(pathName.assign(path).
                            append("\\*").c_str(), &fileInfo)) == -1) {
        return;
    }
    do {
        if (fileInfo.attrib&_A_SUBDIR) {
            string fname = string(fileInfo.name);
            if (fname != ".." && fname != ".") {
                //files.push_back(path + "\\" + fname);
                dir_file_find_jpg(path + "\\" + fname);
            }
        }
        else {
            //cout << path << "\\" << fileInfo.name << endl;
            string str = path + "\\" + string(fileInfo.name);
            filesfortrain_jpg.push_back(str);
        }
    } while (_findnext(hFile, &fileInfo) == 0);
    _findclose(hFile);
    return;
}



Dialog_new_project::Dialog_new_project(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_new_project)
{
    ui->setupUi(this);

    //  ui->lineEdit_in_project_name->setText(QObject::tr("缺陷外观检查"));
    ui->lineEdit_in_project_name->setText("pro_name");


    QString path =QCoreApplication::applicationDirPath();
    std::string exe_filepath = path.toStdString();
    dir_user =exe_filepath+ "/user/";

    // read ini  file first



    upata_pro_xml_list();



}

void Dialog_new_project::upata_pro_xml_list()
{


    QString path =QCoreApplication::applicationDirPath();
    std::string exe_filepath = path.toStdString();
    dir_user =exe_filepath+ "/user";

    QString path_set =QString::fromStdString(dir_user)+"/setting.ini";

    QSettings* configIni = new QSettings(path_set, QSettings::IniFormat);

    int shifoum   = configIni->value("setting/m_shifou_shezhi_moren").toString().toInt();
    QString  xuanzhongmingc   = configIni->value("setting/m_moren_wenjian_name").toString();




    delete configIni;



    if (filesfortrain_jpg.size() > 0)
        filesfortrain_jpg.clear();

    srand((unsigned)time(NULL));
    dir_file_find_jpg(dir_user);

    if (filesfortrain_jpg.size() <= 0)
        cout << "no  file" << endl;
    int xuhao = 1;

    ui->tableWidget->setRowCount(filesfortrain_jpg.size());
    QHeaderView* headerView = ui->tableWidget->horizontalHeader();
    headerView->setDefaultAlignment(Qt::AlignLeft);     //表头字 左对齐
    headerView->setStretchLastSection(true);
    ui->tableWidget->setShowGrid(true);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);  //可多选 单选
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);   //设置选择行为时每次选择一行
    //ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);    //设置编辑
    ui->tableWidget->setStyleSheet("selection-background-color:lightblue;");//设置选中背景色
    ui->tableWidget->setColumnWidth(0,20);
    for (int i = 0; i < filesfortrain_jpg.size(); i++)
    {

        string fileName = filesfortrain_jpg[i];
        //cout << fileName << endl;
        char drive[_MAX_DRIVE] = { 0 };
        char dir[_MAX_DIR] = { 0 };
        char fname[_MAX_FNAME] = { 0 };
        char ext[_MAX_EXT] = { 0 };
        _splitpath(fileName.c_str(), drive, dir, fname, ext);

        string file_format = string(ext);

        //printf("1 \n");

        if (file_format == string(".xml"))
        {
            string pathFile = string(drive) + string(dir) + string(fname) + string(ext);
            pathFile.replace(pathFile.find("\\") ,1, "/" );
            pro_head_biaoge_infor temp;
            temp.name  =QString::fromStdString(fname);
            temp.path= QString::fromStdString(pathFile);
            temp.checkBox = 0;


            ui->tableWidget->setItem(i,0,new QTableWidgetItem(QString::number(xuhao)));
            xiangmu_cols[QString::fromLocal8Bit("序号")] =0;

            ui->tableWidget->setItem(i,1,new QTableWidgetItem(QString::fromStdString(string(fname) )));
            xiangmu_cols[QString::fromLocal8Bit("名称")] =1;


            ui->tableWidget->setItem(i,2,new QTableWidgetItem( temp.path));
            xiangmu_cols[QString::fromLocal8Bit("路径")] =2;


            head_infor[temp.name].checkBox = new QTableWidgetItem();


            if( shifoum)
            {


                if(xuanzhongmingc == temp.name )
                {
                    head_infor[temp.name].checkBox ->setCheckState(Qt::Checked);
                }else
                {
                    head_infor[temp.name].checkBox ->setCheckState(Qt::Unchecked);
                }
            }else
            {
                head_infor[temp.name].checkBox ->setCheckState(Qt::Unchecked);
            }



            head_infor[temp.name].checkBox ->setCheckState(Qt::Unchecked);
            head_infor[temp.name].checkBox ->setText(QObject::tr("勾选启用"));
            ui->tableWidget->setItem(i, 3,  head_infor[temp.name].checkBox);
            xiangmu_cols[QString::fromLocal8Bit("选中")] =3;

            ui->tableWidget->setItem(i,4,new QTableWidgetItem( temp.miaosu));
            xiangmu_cols[QString::fromLocal8Bit("描述")] =4;

            head_infor[temp.name] = temp;

            xuhao++;

        }

    }
}


Dialog_new_project::~Dialog_new_project()
{
    delete ui;
}


void Dialog_new_project::open_pro_new_win()
{
    upata_pro_xml_list();
    this->open();

}

void Dialog_new_project::on_pushButton_canel_new_clicked()
{
    QDialog::close();
}

void Dialog_new_project::on_pushButton_new_proj_clicked()
{
    // qDebug()<< ui->lineEdit_in_project_name->text();

    project_name =  ui->lineEdit_in_project_name->text();
    emit send_project_infor(project_name);
    QDialog::close();
}

void Dialog_new_project::on_pushButton_openlujing_clicked()
{

}

void Dialog_new_project::on_pushButton_delete_lujing_clicked()
{

}

void Dialog_new_project::on_tableWidget_cellChanged(int row, int column)
{
    if (column ==  xiangmu_cols[QString::fromLocal8Bit("选中")])
    {
        QTableWidgetItem* item = ui->tableWidget->item(row, column);

        if (item != NULL)
        {
            if (item->checkState() == Qt::Checked)
            {
                //qDebug() <<QString::fromLocal8Bit("选择") << row << " " << column;
                // 选中绿色
                item->setBackgroundColor(QColor(0x00, 0xFF, 0x00));

                QString name_xml =ui->tableWidget->item(row,1)->text() + ".xml";


                QString path =QCoreApplication::applicationDirPath();
                std::string exe_filepath = path.toStdString();
                dir_user =exe_filepath+ "/user";

                QString path_set =QString::fromStdString(dir_user)+"/setting.ini";

                QSettings* configIni = new QSettings(path_set, QSettings::IniFormat);
                configIni->setValue("setting/m_shifou_shezhi_moren",  1);
                configIni->setValue("setting/m_moren_wenjian_name",  name_xml);

                delete configIni;

            }
            else if (item->checkState() == Qt::Unchecked)
            {
                // 没选中白色
                // qDebug() <<QString::fromLocal8Bit(" 没有选择")<< row << " " << column;
                item->setBackgroundColor(QColor(0xFF, 0xFF, 0xFF));

                QString path =QCoreApplication::applicationDirPath();
                std::string exe_filepath = path.toStdString();
                dir_user =exe_filepath+ "/user";


                QString path_set =QString::fromStdString(dir_user)+"/setting.ini";

                QSettings* configIni = new QSettings(path_set, QSettings::IniFormat);
                //configIni->setValue("setting/m_shifou_shezhi_moren",  0);

                QString name_xml ="default.xml";
               // configIni->setValue("setting/m_moren_wenjian_name",  name_xml);

                delete configIni;
            }

        }
    }
}
