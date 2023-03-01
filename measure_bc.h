#ifndef MEASURE_BC_H
#define MEASURE_BC_H


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

#include <afxext.h>
#define _DEBUG 1

#define PI				3.1415926535897932384626433832795	// π
#define TWO_PI			(2.0*PI)							// 2π

// 一般定义
#define FUNC_OK					0							// 正常
#define NOT_ENOUGH_MEM			(-1)						// 工作内存不足
#define ILLEGAL_ARGUMENT		(-2)						// 参数不合适
#define IMAGE_NOT_EXIST			(-3)						// 图像不存在

typedef struct str_pair_pt{

    QPointF pt1;
    QPointF pt2;

}Pair_pt;


typedef struct str_foure_pair_pt{

    Pair_pt xppt[4];

}FourPair_pt;






typedef  struct str_config_kachi
{

    cv::Point pt1;
    cv::Point pt2;

    // QPointF dx_pt1;
    // QPointF dx_pt2;

    QLineF m_line;
    double ang_line = 0;


    int m_kachi_cd = 150;
    int m_kachi_kd = 30;
    int m_kachi_sl =10;
    int m_kachi_lvbochic = 2;
    int m_kachi_ssfx = 1;
    int m_kachi_yuzhi = 50;
    QString m_kachi_moshi ="positive";

    // int if_have_template_flag =0;
    // QRectF Template_img_space_rc;
}Config_kachi;

typedef struct str_measure_unit{

    QVector< QPointF> m_v_pt;
    QVector<QLineF>  m_kachi;
    QVector<QLineF>  m_jiantou_xian;
    QVector<QRectF>  m_kachi_rc;
    QLineF m_zhouxian;

    QVector<FourPair_pt> fppx;
    Config_kachi cfg;

    bool  shifouhuajiantou = true;
}MeasureUnit;


typedef  struct str_mesure_Set
{
    int duosao_bian = 1;

    double kuan_yz = 1.0;
    double gao_yz = 1.0;

    double min_v = 0.0;
    double max_v = 0.0;
    double buchangzhi = 0.0;
    QVector<double> juli_celiangzhi;

    QVector<QPointF> A_pt;
    QVector<QPointF> B_pt;

    int if_have_template_flag =0;
    QRectF Template_img_space_rc;
    int result_flag =0;


    QMap<int ,MeasureUnit> m_unit; // QVector<MeasureUnit> m_unit; 在duosao_bian等于2 的情况下使用
    QString ID ="";
    QListWidgetItem * ptr_item= nullptr;


    int shifou_chushihua = 0;

}Mesure_Unit_Set;






struct s_TemplData
{
    std::vector<cv::Mat> vecPyramid;
    std::vector<cv::Scalar> vecTemplMean;
    std::vector<double> vecTemplNorm;
    std::vector<double> vecInvArea;
    std::vector<BOOL> vecResultEqual1;
    BOOL bIsPatternLearned;
    int iBorderColor;
    void clear ()
    {
        std::vector<cv::Mat> ().swap (vecPyramid);
        std::vector<double> ().swap (vecTemplNorm);
        std::vector<double> ().swap (vecInvArea);
        std::vector<cv::Scalar> ().swap (vecTemplMean);
        std::vector<BOOL> ().swap (vecResultEqual1);
    }
    void resize (int iSize)
    {
        vecTemplMean.resize (iSize);
        vecTemplNorm.resize (iSize, 0);
        vecInvArea.resize (iSize, 1);
        vecResultEqual1.resize (iSize, FALSE);
    }
    s_TemplData ()
    {
        bIsPatternLearned = FALSE;
    }
};
struct s_MatchParameter
{
    cv::Point2d pt;
    double dMatchScore;
    double dMatchAngle;
    //Mat matRotatedSrc;
    cv::Rect rectRoi;
    double dAngleStart;
    double dAngleEnd;
    cv::RotatedRect rectR;
    cv::Rect rectBounding;
    BOOL bDelete;

    double vecResult[3][3];//for subpixel
    int iMaxScoreIndex;//for subpixel
    BOOL bPosOnBorder;
    cv::Point2d ptSubPixel;
    double dNewAngle;

    s_MatchParameter (cv::Point2f ptMinMax, double dScore, double dAngle)//, Mat matRotatedSrc = Mat ())
    {
        pt = ptMinMax;
        dMatchScore = dScore;
        dMatchAngle = dAngle;

        bDelete = FALSE;
        dNewAngle = 0.0;

        bPosOnBorder = FALSE;
    }
    s_MatchParameter ()
    {
        double dMatchScore = 0;
        double dMatchAngle = 0;
    }
    ~s_MatchParameter ()
    {

    }
};
struct s_SingleTargetMatch
{
    cv::Point2d ptLT, ptRT, ptRB, ptLB, ptCenter;
    double dMatchedAngle;
    double dMatchScore;


    //QPointF q_ptLT, q_ptRT, q_ptRB, q_ptLB, q_ptCenter;

    QRectF result_rc;
};



typedef struct str_cl_infor{

    QString src_name;
    int src_img_w = 0;
    int src_img_h =0;

    QString moban_name;
    QSizeF moban_size;
    QRect moban_in_src_rc;

    QMap<QString ,Mesure_Unit_Set> m_mesure_set;

}CL_infor;

class measure_bc : public QObject
{
    Q_OBJECT
public:
    explicit measure_bc(QObject *parent = nullptr);




public:
    int  Edge1dFindLine(cv::Mat image);
    int  Edge1dFindLine(cv::Mat image, cv::Point tpLineSt, cv::Point tpLineEd, int iCapLength, int iCapWidth, int nCalipers, int nFilterSize, int iSearchDirec, double dAmpThreshold, const char *ccTransition);
    int  Edge1dFindCircle(cv::Mat image, cv::Point tpPoint, int iRadius, int iCapLength, int iCapWidth, int nCalipers, int nFilterSize, int iSearchDirec, double dAmpThreshold, const char *ccTransition);




public:


    QString path_fold = "";
    QString ID = "";

    QString src_name;
    int src_img_w = 0;
    int src_img_h =0;
    int moren = 0;
    int line_num = 0;

    QString moban_name;

    QRectF moban_in_src_rc;



    MeasureUnit m_obj;
    std::vector<s_SingleTargetMatch> m_vecSingleTargetData;// 一幅图有多个目标

    // std::vector<s_SingleTargetMatch> m_vecSingleTargetData_offset;


    QMap<QString ,Mesure_Unit_Set> m_mesure_set;
    QMap<QListWidgetItem * ,QString> m_mesure_set_item_str;// 可以通过Item到ID 号来查找  measure set

    cv::Mat tsfIPM, tsfIPMInv;


signals:

public://模板匹配用的函数和数据
    BOOL Match ();
    int  GetTopLayer (cv::Mat* matTempl, int iMinDstLength);
    cv::Size  GetBestRotationSize (cv::Size sizeSrc, cv::Size sizeDst, double dRAngle);
    cv::Point2f  ptRotatePt2f (cv::Point2f ptInput, cv::Point2f ptOrg, double dAngle);
    void  MatchTemplate (cv::Mat& matSrc, s_TemplData* pTemplData, cv::Mat& matResult, int iLayer, BOOL bUseSIMD);
    void  CCOEFF_Denominator (cv::Mat& matSrc, s_TemplData* pTemplData, cv::Mat& matResult, int iLayer);
    void  SortPtWithCenter (std::vector<cv::Point2f>& vecSort);

    cv::Point  GetNextMaxLoc (cv::Mat & matResult, cv::Point ptMaxLoc, double dMinValue, int iTemplateW, int iTemplateH, double& dMaxValue, double dMaxOverlap);
    void  GetRotatedROI (cv::Mat& matSrc, cv::Size size, cv::Point2f ptLT, double dAngle, cv::Mat& matROI);
    BOOL  SubPixEsimation (std::vector<s_MatchParameter>* vec, double* dNewX, double* dNewY, double* dNewAngle, double dAngleStep, int iMaxScoreIndex);
    void  FilterWithScore (std::vector<s_MatchParameter>* vec, double dScore);
    void  FilterWithRotatedRect (std::vector<s_MatchParameter>* vec, int iMethod, double dMaxOverLap);
    void LearnPattern ();
    void set_roi_offset(double offset_x, double offset_y);
    void set_dx_pt(MeasureUnit & m_obj , QRectF template_rc_chushihua, s_SingleTargetMatch guocheng_4_pt , int flag);



    //輸出ROI
    void OutputRoi (s_SingleTargetMatch ss);


public:
    cv::Mat m_matSrc;
    cv::Mat m_matDst;

    int m_iMaxPos;
    double m_dMaxOverlap;
    double m_dScore;
    double m_dToleranceAngle;
    int m_iMinReduceArea;
    int m_iMessageCount;
    BOOL m_bDebugMode = FALSE;
    BOOL m_bSubPixelEstimation= FALSE;
    double offset_x =0;
    double offset_y = 0;



    BOOL m_bToleranceRange =FALSE;
    double m_dTolerance1;
    double m_dTolerance2;
    double m_dTolerance3;
    double m_dTolerance4;



private:
    s_TemplData m_TemplData;

};

#endif // MEASURE_BC_H
