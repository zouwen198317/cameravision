#include "measure_bc.h"

#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/types_c.h>

using namespace  std;
using namespace  cv;



#define VISION_TOLERANCE 0.0000001
#define D2R (CV_PI / 180.0)
#define R2D (180.0 / CV_PI)
#define MATCH_CANDIDATE_NUM 5

#define SUBITEM_INDEX 0
#define SUBITEM_SCORE 1
#define SUBITEM_ANGLE 2
#define SUBITEM_POS_X 3
#define SUBITEM_POS_Y 4

#define MAX_SCALE_TIMES 10
#define MIN_SCALE_TIMES 0
#define SCALE_RATIO 1.25

#define FONT_SIZE 115


bool compareScoreBig2Small (const s_MatchParameter& lhs, const s_MatchParameter& rhs) { return  lhs.dMatchScore > rhs.dMatchScore; }
bool comparePtWithAngle (const pair<cv::Point2f, double> lhs, const pair<cv::Point2f, double> rhs) { return lhs.second < rhs.second; }
bool compareMatchResultByPos (const s_SingleTargetMatch& lhs, const s_SingleTargetMatch& rhs)
{
    double dTol = 2;
    if (fabs (lhs.ptCenter.y - rhs.ptCenter.y) <= dTol)
        return lhs.ptCenter.x < rhs.ptCenter.x;
    else
        return lhs.ptCenter.y < rhs.ptCenter.y;

};
bool compareMatchResultByScore (const s_SingleTargetMatch& lhs, const s_SingleTargetMatch& rhs) { return lhs.dMatchScore > rhs.dMatchScore; }
bool compareMatchResultByPosX (const s_SingleTargetMatch& lhs, const s_SingleTargetMatch& rhs) { return lhs.ptCenter.x < rhs.ptCenter.x; }


measure_bc::measure_bc(QObject *parent) : QObject(parent)
{


    //    m_dDstScale = 1;
    //    m_dSrcScale = 1;
    //    m_iScaleTimes = 0;
    //    m_dNewScale = 1;
    //    m_bShowResult = FALSE;
    //    bInitial = FALSE;
    //    m_bToleranceRange = FALSE;

}

//void measure_bc::copyPtr2Obj(measure_bc &tempX, measure_bc * ptr)
//{
//    tempX.ID = ptr->ID;
//    tempX.src_name =  ptr-> src_name;
//    tempX.moren = ptr->moren  ;
//    tempX.moban_name =ptr->  moban_name;
//    tempX.moban_in_src_rc = ptr-> moban_in_src_rc;

//    tempX.m_dMaxOverlap = ptr-> m_dMaxOverlap;
//    tempX.m_dScore =  ptr->m_dScore;
//    tempX.m_dToleranceAngle =  ptr->m_dToleranceAngle;
//    tempX.m_iMinReduceArea =  ptr->m_iMinReduceArea;
//    tempX.m_iMaxPos =  ptr->m_iMaxPos ;
//    tempX.m_bSubPixelEstimation =  ptr->m_bSubPixelEstimation;


//    tempX.m_matSrc = ptr-> m_matSrc;



//    tempX.m_matDst =ptr-> m_matDst;

//    tempX.line_num  = ptr-> line_num;


//    QMap< QString,   Mesure_Unit_Set >::const_iterator it = ptr->m_mesure_set.constBegin();
//    while (it !=  ptr->m_mesure_set.constEnd())
//    {
//        Mesure_Unit_Set x = it.value();

//        tempX.m_mesure_set[x.ID] = x;

//        ++it;
//    }


//}

static void findPeak(int iSize, float projectMap[], float fAmpThreshold, std::vector<int>& peaks)
{
    std::vector<int> sign;
    for (int i = 1; i < iSize; i++) {
        auto diff = projectMap[i] - projectMap[i - 1];
        if (diff < 0.0f) {
            sign.push_back(-1);
        }
        else if (diff > 0.0f) {
            sign.push_back(1);
        }
        else {
            sign.push_back(0);
        }
    }
    for (int j = 1; j < sign.size(); j++) {
        int diff = sign[j] - sign[j - 1];
        if (diff != 0 && abs(projectMap[j]) > fAmpThreshold) {
            peaks.push_back(j);
        }
    }
}

/**
  * 绘制十字
  * @param[in] img 目标图像
  * @param[in] point 十字中心点
  * @param[in] color 颜色
  * @param[in] size 十字尺寸
  * @param[in] thickness 粗细
*/
void drawCross(cv::Mat img, cv::Point2d point, cv::Scalar color,int size,int thickness)
{
    //绘制横线
    cv::line(img,cv::Point2d(point.x-size/2,point.y),cv::Point2d(point.x+size/2,point.y),color,thickness,CV_AA,0);
    //绘制竖线
    cv::line(img,cv::Point2d(point.x,point.y-size/2),cv::Point2d(point.x,point.y+size/2),color,thickness, CV_AA,0);

    return;
}


bool sort_x_min_vc(const   cv::Point2f& pt1, const cv::Point2f& pt2)
{
    return  pt1.x < pt2.x;//left to right
}

bool sort_x_max_vc(const   cv::Point2f& pt1, const cv::Point2f& pt2)
{
    return  pt1.x > pt2.x;//left to right
}
bool sort_y_max_vc(const   cv::Point2f& pt1, const cv::Point2f& pt2)
{
    return  pt1.y > pt2.y;//left to right
}

bool sort_y_min_vc(const   cv::Point2f& pt1, const cv::Point2f& pt2)
{
    return  pt1.y < pt2.y;//left to right
}
QRectF  find_zui_point2f(vector<cv::Point2f>& ps)
{
    /*vector<cv::Point2f> ps;
    for (int i = 0; i < num; i++)
    {
        ps.push_back(pt[i]);
    }*/
    std::sort(ps.begin(), ps.end(), sort_x_min_vc);
    int lt_x = ps[0].x;
    std::sort(ps.begin(), ps.end(), sort_y_min_vc);
    int lt_y = ps[0].y;
    std::sort(ps.begin(), ps.end(), sort_y_max_vc);
    int rb_y = ps[0].y;
    std::sort(ps.begin(), ps.end(), sort_x_max_vc);
    int rb_x = ps[0].x;

    QRectF rcr = QRectF(QPointF(lt_x, lt_y), QPointF(rb_x, rb_y));


    return rcr;

}



/// <summary>
/// 边缘查找工具
/// </summary>
/// <param name="tpImage">图像</param>
/// <param name="tpLineSt">卡尺起点</param>
/// <param name="tpLineEd">卡尺终点</param>
/// <param name="iCapLength">卡尺长度</param>
/// <param name="iCapWidth">卡尺宽度</param>
/// <param name="nCalipers">卡尺数量</param>
/// <param name="nFilterSize">滤波尺寸(默认为2,如果边缘较窄可以设置为1，建议范围[1，5])</param>
/// <param name="iSearchDirec">搜索方向(1正方向、-1反方向)</param>
/// <param name="dAmpThreshold">边缘阈值</param>
/// <param name="ccTransition">提取模式("all","positive","negative")</param>
/// <param name="hObject">结果</param>
/// <returns></returns>
int  measure_bc::Edge1dFindLine(cv::Mat image)
{
    Edge1dFindLine(image,//用OpenCV中Mat替代
                   this->m_obj.cfg.pt1,//起点，用cv替代
                   this->m_obj.cfg.pt2,//终点，用cv替代
                   this->m_obj.cfg.m_kachi_cd,//卡尺长度
                   this->m_obj.cfg.m_kachi_kd,//卡尺宽度
                   this->m_obj.cfg.m_kachi_sl, //卡尺数量
                   this->m_obj.cfg.m_kachi_lvbochic,//滤波尺寸（建议2，默认[1,5]）
                   this->m_obj.cfg.m_kachi_ssfx,//搜索方向，1默认方向，-1反方向
                   this->m_obj.cfg.m_kachi_yuzhi,//边缘阈值
                   this->m_obj.cfg.m_kachi_moshi.toStdString().c_str());//提取模式("all","positive","negative")三个选项，all提取最强边缘，positive 提取由黑到白 negative 提取由白到黑



}
int measure_bc::Edge1dFindLine(cv::Mat image, cv::Point tpLineSt, cv::Point tpLineEd, int iCapLength, int iCapWidth, int nCalipers, int nFilterSize, int iSearchDirec, double dAmpThreshold, const char *ccTransition)
{
    if (image.empty()) {
        return IMAGE_NOT_EXIST;
    }

    if(m_obj.m_v_pt.size() > 0) m_obj.m_v_pt.clear();
    if(m_obj.m_kachi.size() > 0) m_obj.m_kachi.clear();
    if(m_obj.m_jiantou_xian.size() > 0) m_obj.m_jiantou_xian.clear();
    if(m_obj.m_kachi_rc.size() > 0) m_obj.m_kachi_rc.clear();

    if(m_obj.fppx.size() > 0) m_obj.fppx.clear();



    //灰度图
    int incn = image.channels();
    if (incn > 3) {
        cv::cvtColor(image, image, cv::COLOR_BGRA2GRAY);
    }
    else if (incn == 3) {
        cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
    }
    const int X = image.cols, Y = image.rows;
    //显示用
    cv::Mat cc;
    cv::cvtColor(image, cc, cv::COLOR_GRAY2BGR);
    //判断越界
    if (tpLineSt.x < 0 || tpLineSt.y < 0 || tpLineSt.x>X || tpLineSt.y>Y || tpLineEd.x<0 || tpLineEd.y<0 || tpLineEd.x>X || tpLineEd.y>Y) {
        return ILLEGAL_ARGUMENT;
    }
    //主轴倾角
    double tQ;
    tQ = atan2(tpLineEd.y - tpLineSt.y, tpLineEd.x - tpLineSt.x);
    //参数限制
    iCapWidth = iCapWidth <= 0 ? iCapWidth = 0 : iCapWidth - 1;
    iCapLength = iCapLength <= 0 ? 8 : iCapLength;
    nCalipers = nCalipers <= 0 ? 5 : nCalipers;
    //直线上的坐标
    float L = (float)cv::norm(cv::Point2d(tpLineSt.x, tpLineSt.y) - cv::Point2d(tpLineEd.x, tpLineEd.y));
    //步长
    float plusStep = (L - (float)nCalipers*(float)iCapWidth) / ((float)nCalipers + 1.0f);
    //绘制profileLine
    cv::arrowedLine(cc, cv::Point(cvRound(tpLineSt.x), cvRound(tpLineSt.y)), cv::Point(cvRound(tpLineEd.x), cvRound(tpLineEd.y)), cv::Scalar(255, 153, 0), 2);
    m_obj.m_zhouxian = QLineF(cv::Point(cvRound(tpLineSt.x), cvRound(tpLineSt.y)).x,cv::Point(cvRound(tpLineSt.x), cvRound(tpLineSt.y)).y , cv::Point(cvRound(tpLineEd.x), cvRound(tpLineEd.y)).x, cv::Point(cvRound(tpLineEd.x), cvRound(tpLineEd.y)).y);
    //判断极性
    bool anyPolarity = strcmp("all", ccTransition) == 0;
    //默认过滤一半像素
    float *filterK = new float[2 * nFilterSize + 1]();
    //定义滤波核
    for (int n = 0; n < nFilterSize; n++) {
        filterK[n] = 1;
        filterK[2 * nFilterSize - n] = -1;
    }
    cv::Mat whalf(cv::Size(2 * nFilterSize + 1, 1), CV_32FC1, filterK);
    //线采样,采用双三次插值
    cv::Size szMap(2 * iCapLength + 1, iCapWidth + 1);
    //结果
    std::vector<cv::Point2f> *tpResults = new std::vector<cv::Point2f>();
    for (int n = 1; n <= nCalipers; n++)
    {
        float *pMag = new float[szMap.width*szMap.height];
        for (int m = 0; m <= iCapWidth; m++)
        {
            float plusX, plusY;
            plusX = ((float)n*(plusStep + (float)iCapWidth) - (float)iCapWidth + m) * (float)cos(tQ);
            plusY = ((float)n*(plusStep + (float)iCapWidth) - (float)iCapWidth + m) * (float)sin(tQ);
            //中轴线路径上的点
            cv::Point2f pLine((float)tpLineSt.x + plusX, (float)tpLineSt.y + plusY);
            for (int iR = -iCapLength; iR <= iCapLength; iR++) {
                //待插值坐标
                float _plusX, _plusY;
                _plusX = (float)iR*(float)cos(tQ + iSearchDirec*CV_PI / 2.0) + pLine.x;
                _plusY = (float)iR*(float)sin(tQ + iSearchDirec*CV_PI / 2.0) + pLine.y;
                //防止越界
                if (_plusX < 1 || _plusX >= X - 2 || _plusY < 1 || _plusY >= Y - 2) {
                    pMag[(iCapLength + iR) + m*szMap.width] = -1;
                    continue;
                }
#ifdef _DEBUG
                //drawPoint("", cv::Point2f(_plusX, _plusY), cv::Scalar(36, 127, 255), 1);
                //画像素
                float bb = (float)cos(tQ)*0.5f;
                float aa = (float)sin(tQ)*0.5f;
                cv::Point2f pt(_plusX, _plusY);
                cv::Point2f pts[4];
                pts[0].x = (float)(pt.x - aa - bb);
                pts[0].y = (float)(pt.y + bb - aa);
                pts[1].x = (float)(pt.x + aa - bb);
                pts[1].y = (float)(pt.y - bb - aa);
                pts[2].x = (float)(2 * pt.x - pts[0].x);
                pts[2].y = (float)(2 * pt.y - pts[0].y);
                pts[3].x = (float)(2 * pt.x - pts[1].x);
                pts[3].y = (float)(2 * pt.y - pts[1].y);
                for (int j = 0; j < 4; j++)
                {
                    //drawLine("", pts[j], pts[(j + 1) % 4], cv::Scalar(255, 153, 0));
                }
#endif
                //整数部分
                int x = cvRound(_plusX), y = cvRound(_plusY);
                //小数部分
                float u = abs(_plusX - ((float)x + 0.5f));
                float v = abs(_plusY - ((float)y - 1.0f + 0.5f));
                //插值计算灰度值
                float gv = (1.0f - v)*(image.ptr<uint8_t>(y - 1)[x] * (1.0f - u) + image.ptr<uint8_t>(y - 1)[x - 1] * u)
                        + v*(image.ptr<uint8_t>(y)[x] * (1.0f - u) + image.ptr<uint8_t>(y)[x - 1] * u);
                //填入灰度值
                pMag[(iCapLength + iR) + m*szMap.width] = gv;
            }
        }
        //采样位置
        cv::Point2f midLine((float)tpLineSt.x + ((float)n*(plusStep + (float)iCapWidth) - (float)iCapWidth + (float)iCapWidth / 2.0f) * (float)cos(tQ),
                            (float)tpLineSt.y + ((float)n*(plusStep + (float)iCapWidth) - (float)iCapWidth + (float)iCapWidth / 2.0f) * (float)sin(tQ));
        //各位置采样路径
        cv::Point2f midLineStart, midLineEnd;
        midLineStart = cv::Point2f(-iCapLength*(float)cos(tQ + iSearchDirec*CV_PI / 2.0) + midLine.x, -iCapLength*(float)sin(tQ + iSearchDirec*CV_PI / 2.0) + midLine.y);
        midLineEnd = cv::Point2f(iCapLength*(float)cos(tQ + iSearchDirec* CV_PI / 2) + midLine.x, iCapLength*(float)sin(tQ + iSearchDirec*CV_PI / 2.0) + midLine.y);
        //采样图像
        cv::Mat interMap(szMap, CV_32FC1, pMag);
        //计算投影
        cv::Mat projectedMap;
        cv::reduce(interMap, projectedMap, 0, CV_REDUCE_AVG/*cv::REDUCE_AVG*/, CV_32F);////沿垂直方向投影，得到1行，width列的矩阵
        //差分过滤(TODO:加高斯滤波)
        float *pFilteredMap = new float[szMap.width];
        cv::Mat filteredMap(cv::Size(szMap.width, 1), CV_32FC1, pFilteredMap);
        cv::sepFilter2D(projectedMap, filteredMap, CV_32F, whalf, cv::Mat::ones(1, 1, CV_32F));
        //投影峰值查找
        std::vector<int> peeks;
        findPeak(szMap.width, pFilteredMap, (float)dAmpThreshold, peeks);
        //存在满足幅度值的边缘
        if (!peeks.empty()) {
            float dist = 0; bool found = false;
            //判断灰度值过滤类型
            if (anyPolarity) {
                //不分极性
                float maxDist = 0; int maxPos = 0; found = true;
                for (auto&peek : peeks) {
                    if (abs(pFilteredMap[peek]) > maxDist) {
                        maxDist = abs(pFilteredMap[peek]);
                        maxPos = peek;
                    }
                }
                dist = (float)maxPos;
            }
            else if (strcmp("positive", ccTransition) == 0) {
                int maxPos = 0;
                for (auto&peek : peeks) {
                    if (pFilteredMap[peek] > 0) {
                        maxPos = peek;
                        found = true;
                        break;
                    }
                }
                dist = (float)maxPos;
            }
            else if (strcmp("negative", ccTransition) == 0) {
                int maxPos = 0;
                for (auto&peek : peeks) {
                    if (pFilteredMap[peek] < 0) {
                        maxPos = peek;
                        found = true;
                        break;
                    }
                }
                dist = (float)maxPos;
            }
            //阈值限制,计算亚像素坐标
            if (found) {
                int l, m, r; float a, b, c, u;
                m = (int)dist;
                l = m - 1 < 0 ? m : m - 1;
                r = m + 1 >= szMap.width ? m : m + 1;

                a = pFilteredMap[l]; b = pFilteredMap[m];
                c = pFilteredMap[r];
                u = 0.5f*(a - c) / (a - b - b + c);
                //定位结果
                float dstX, dstY;
                dstX = midLineStart.x + (dist + u)*(float)cos(tQ + iSearchDirec* CV_PI / 2.0);
                dstY = midLineStart.y + (dist + u)*(float)sin(tQ + iSearchDirec* CV_PI / 2.0);
                cv::Point2f tpResult;
                tpResult.x = dstX; tpResult.y = dstY;
                tpResults->push_back(tpResult);

                // drawCross(cc, tpResult, cv::Scalar(0, 255, 0),10,8);
                //cv::circle(cc, tpResult, 20,cv::Scalar(0, 255, 0),1,8);
                QPointF pt(tpResult.x, tpResult.y);
                m_obj.m_v_pt.append(pt);

            }
        }
#ifdef _DEBUG
        cv::arrowedLine(cc, midLineStart, midLineEnd, cv::Scalar(0, 0, 255), 2);

        QPointF pt1(midLineStart.x, midLineStart.y);
        QPointF pt2(midLineEnd.x, midLineEnd.y);
        QLineF l1(pt1,pt2);
        m_obj.m_jiantou_xian.append(l1);



        //画卡尺
        float bb = (float)cos(tQ)*0.5f;
        float aa = (float)sin(tQ)*0.5f;
        cv::Point2f pt(midLine.x, midLine.y);
        cv::Point2f pts[4];
        pts[0].x = (float)(pt.x - aa * szMap.width - bb * szMap.height);
        pts[0].y = (float)(pt.y + bb * szMap.width - aa * szMap.height);
        pts[1].x = (float)(pt.x + aa * szMap.width - bb * szMap.height);
        pts[1].y = (float)(pt.y - bb * szMap.width - aa * szMap.height);
        pts[2].x = (float)(2 * pt.x - pts[0].x);
        pts[2].y = (float)(2 * pt.y - pts[0].y);
        pts[3].x = (float)(2 * pt.x - pts[1].x);
        pts[3].y = (float)(2 * pt.y - pts[1].y);


        vector<cv::Point2f>  ps;

        FourPair_pt duix;

        // QRectF  find_zui_point2f(vector<cv::Point2f>& ps)
        for (int j = 0; j < 4; j++)
        {
            //drawLine("", pts[j], pts[(j + 1) % 4], cv::Scalar(255, 153, 0), 2);
            // cv::line(cc, pts[j], pts[(j + 1) % 4], cv::Scalar(255, 153, 0));
            QPointF pt1(pts[j].x, pts[j].y);
            QPointF pt2(pts[(j + 1) % 4].x,pts[(j + 1) % 4].y);
            QLineF l1(pt1,pt2);
            m_obj.m_kachi.append(l1);

            ps.push_back(pts[j]);


            duix.xppt[j].pt1 = QPointF(pts[j].x ,pts[j].y);
            duix.xppt[j].pt2 = QPointF(pts[(j + 1) % 4].x ,  pts[(j + 1) % 4].y);

        }

        m_obj.fppx.append(duix);

#endif
        //释放资源
        delete[] pMag;
        pMag = NULL;
    }
    //释放资源(Tips:当存在越界时候在用free释放时会报错)
    delete[] filterK;
    filterK = NULL;

    return FUNC_OK;
}





int measure_bc::Edge1dFindCircle(cv::Mat image, cv::Point tpPoint, int iRadius, int iCapLength, int iCapWidth, int nCalipers, int nFilterSize, int iSearchDirec, double dAmpThreshold, const char *ccTransition)
{
    if (image.empty()) {
        return IMAGE_NOT_EXIST;
    }
    //灰度图
    int incn = image.channels();
    if (incn > 3) {
        cv::cvtColor(image, image, cv::COLOR_BGRA2GRAY);
    }
    else if (incn == 3) {
        cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
    }

    const int X = image.cols, Y = image.rows;
    //判断越界
    if (tpPoint.x < 0 || tpPoint.y < 0 || tpPoint.x>X || tpPoint.y>Y) {
        return 0;
    }
    cv::Mat cc;
    cv::cvtColor(image, cc, cv::COLOR_GRAY2BGR);
    //步长
    float plusStep = float(TWO_PI / (float)nCalipers);
    //绘制profileLine
    cv::circle(cc, cv::Point(cvRound(tpPoint.x), cvRound(tpPoint.y)), iRadius, cv::Scalar(), 1);
    //判断极性
    bool anyPolarity = strcmp("all", ccTransition) == 0;
    //默认过滤一半像素
    float *filterK = new float[2 * nFilterSize + 1]();
    //定义滤波核
    for (int n = 0; n < nFilterSize; n++) {
        filterK[n] = 1;
        filterK[2 * nFilterSize - n] = -1;
    }
    cv::Mat whalf(cv::Size(2 * nFilterSize + 1, 1), CV_32FC1, filterK);
    //参数限制
    iCapWidth = iCapWidth <= 0 ? 0 : iCapWidth - 1;
    iCapLength = iCapLength <= 0 ? 12 : iCapLength;
    nCalipers = nCalipers <= 0 ? 10 : nCalipers;
    //线采样
    cv::Size szMap(2 * iCapLength + 1, iCapWidth + 1);
    //结果
    std::vector<cv::Point2f> *tpResults = new std::vector<cv::Point2f>();
    for (float t = -(float)PI; t < PI; t += plusStep)
    {
        int m = 0;
        //路径上的点
        float x = float(tpPoint.x + (float)iRadius*cos(t));
        float y = float(tpPoint.x + (float)iRadius*sin(t));
        //采样图像
        float *pMag = new float[szMap.width*szMap.height * sizeof(float_t)];
        for (float n = -(float)iCapWidth / 2.0f; n <= (float)iCapWidth / 2.0f; n += 1.0f, m++)
        {
            for (int iR = -iCapLength; iR <= iCapLength; iR++) {
                //待插值坐标
                float _plusX, _plusY;
                _plusX = (float)iR*iSearchDirec*(float)cos(t) + x + n*(float)cos(t + CV_PI / 2);
                _plusY = (float)iR*iSearchDirec*(float)sin(t) + y + n*(float)sin(t + CV_PI / 2);
                //防止越界
                if (_plusX < 1 || _plusX >= X - 2 || _plusY < 1 || _plusY >= Y - 2) {
                    continue;
                }
                //drawPoint("", cv::Point2f(_plusX, _plusY), cv::Scalar(36, 127, 255), 1);
                //整数部分
                int x = cvRound(_plusX), y = cvRound(_plusY);
                //小数部分
                float u = abs(_plusX - ((float)x + 0.5f));
                float v = abs(_plusY - ((float)y - 1.0f + 0.5f));
                //插值计算灰度值
                float gv = (1.0f - v)*(image.ptr<uint8_t>(y - 1)[x] * (1.0f - u) + image.ptr<uint8_t>(y - 1)[x - 1] * u)
                        + v*(image.ptr<uint8_t>(y)[x] * (1.0f - u) + image.ptr<uint8_t>(y)[x - 1] * u);
                //填入灰度值
                pMag[(iCapLength + iR) + m*szMap.width] = gv;
            }
        }
        //采样位置中线
        cv::Point2f midLineStart, midLineEnd;
        midLineStart = cv::Point2f(-iCapLength*iSearchDirec*(float)cos(t) + x, -iCapLength*iSearchDirec*(float)sin(t) + y);
        midLineEnd = cv::Point2f(iCapLength*iSearchDirec*(float)cos(t) + x, iCapLength*iSearchDirec*(float)sin(t) + y);
        cv::arrowedLine(cc, cv::Point(), cv::Point(), cv::Scalar(0, 255, 0), 1);
        //采样图像
        cv::Mat interMap(szMap, CV_32FC1, pMag);
        //计算投影
        cv::Mat projectedMap;
        cv::reduce(interMap, projectedMap, 0,  CV_REDUCE_AVG/*cv::REDUCE_AVG*/, CV_32F);
        //差分过滤(TODO:加高斯滤波)
        float *pFilteredMap = new float[szMap.width];
        cv::Mat filteredMap(cv::Size(szMap.width, 1), CV_32FC1, pFilteredMap);
        cv::sepFilter2D(projectedMap, filteredMap, CV_32F, whalf, cv::Mat::ones(1, 1, CV_32F));
        //投影峰值查找
        std::vector<int> peeks;
        findPeak(szMap.width, pFilteredMap, (float)dAmpThreshold, peeks);
        //存在满足幅度值的边缘
        if (!peeks.empty()) {
            float dist = 0; bool found = false;
            //判断灰度值过滤类型
            if (anyPolarity) {
                //不分极性
                float maxDist = 0; int maxPos = 0;
                for (auto&peek : peeks) {
                    if (abs(pFilteredMap[peek]) > maxDist) {
                        maxDist = abs(pFilteredMap[peek]);
                        maxPos = peek;
                    }
                }
                found = true;
                dist = (float)maxPos;
            }
            else if (strcmp("positive", ccTransition) == 0) {
                int maxPos = 0;
                for (auto&peek : peeks) {
                    if (pFilteredMap[peek] > 0) {
                        maxPos = peek;
                        found = true;
                        break;
                    }
                }
                dist = (float)maxPos;
            }
            else if (strcmp("negative", ccTransition) == 0) {
                int maxPos = 0;
                for (auto&peek : peeks) {
                    if (pFilteredMap[peek] < 0) {
                        maxPos = peek;
                        found = true;
                        break;
                    }
                }
                dist = (float)maxPos;
            }
            //阈值限制,计算亚像素坐标
            if (found) {
                int l, m, r; float a, b, c, u;
                m = (int)dist;
                l = m - 1 < 0 ? m : m - 1;
                r = m + 1 >= szMap.width ? m : m + 1;

                a = pFilteredMap[l]; b = pFilteredMap[m];
                c = pFilteredMap[r];
                u = 0.5f*(a - c) / (a - b - b + c);

                //定位结果
                float dstX, dstY;
                dstX = midLineStart.x + iSearchDirec*(dist + u)*(float)cos(t);
                dstY = midLineStart.y + iSearchDirec*(dist + u)*(float)sin(t);
                cv::Point2f tpResult;
                tpResult.x = dstX; tpResult.y = dstY;
                tpResults->push_back(tpResult);


                drawCross(cc, tpResult, cv::Scalar(0, 255, 0),50,8);
            }
        }
#ifdef _DEBUG
        //画卡尺
        float bb = (float)cos(t + CV_PI / 2)*0.5f;
        float aa = (float)sin(t + CV_PI / 2)*0.5f;
        cv::Point2f pt(x, y);
        cv::Point2f pts[4];
        pts[0].x = (float)(pt.x - aa * szMap.width - bb * szMap.height);
        pts[0].y = (float)(pt.y + bb * szMap.width - aa * szMap.height);
        pts[1].x = (float)(pt.x + aa * szMap.width - bb * szMap.height);
        pts[1].y = (float)(pt.y - bb * szMap.width - aa * szMap.height);
        pts[2].x = (float)(2 * pt.x - pts[0].x);
        pts[2].y = (float)(2 * pt.y - pts[0].y);
        pts[3].x = (float)(2 * pt.x - pts[1].x);
        pts[3].y = (float)(2 * pt.y - pts[1].y);

        for (int j = 0; j < 4; j++)
        {
            cv::line(cc, pts[j], pts[(j + 1) % 4], cv::Scalar(255, 153, 0));
        }
#endif
        //释放资源
        delete[] pMag;
        pMag = NULL;
    }
    //释放资源
    delete[] filterK;
    filterK = NULL;
    return FUNC_OK;
}

int measure_bc::GetTopLayer (cv::Mat* matTempl, int iMinDstLength)
{
    int iTopLayer = 0;
    int iMinReduceArea = iMinDstLength * iMinDstLength;
    int iArea = matTempl->cols * matTempl->rows;
    while (iArea > iMinReduceArea)
    {
        iArea /= 4;
        iTopLayer++;
    }
    return iTopLayer;
}

cv::Point2f measure_bc::ptRotatePt2f (cv::Point2f ptInput, cv::Point2f ptOrg, double dAngle)
{
    double dWidth = ptOrg.x * 2;
    double dHeight = ptOrg.y * 2;
    double dY1 = dHeight - ptInput.y, dY2 = dHeight - ptOrg.y;

    double dX = (ptInput.x - ptOrg.x) * cos (dAngle) - (dY1 - ptOrg.y) * sin (dAngle) + ptOrg.x;
    double dY = (ptInput.x - ptOrg.x) * sin (dAngle) + (dY1 - ptOrg.y) * cos (dAngle) + dY2;

    dY = -dY + dHeight;
    return cv::Point2f ((float)dX, (float)dY);
}


cv::Size measure_bc::GetBestRotationSize (cv::Size sizeSrc, cv::Size sizeDst, double dRAngle)
{
    double dRAngle_radian = dRAngle * D2R;
    Point ptLT (0, 0), ptLB (0, sizeSrc.height - 1), ptRB (sizeSrc.width - 1, sizeSrc.height - 1), ptRT (sizeSrc.width - 1, 0);
    Point2f ptCenter ((sizeSrc.width - 1) / 2.0f, (sizeSrc.height - 1) / 2.0f);
    Point2f ptLT_R = ptRotatePt2f (Point2f (ptLT), ptCenter, dRAngle_radian);
    Point2f ptLB_R = ptRotatePt2f (Point2f (ptLB), ptCenter, dRAngle_radian);
    Point2f ptRB_R = ptRotatePt2f (Point2f (ptRB), ptCenter, dRAngle_radian);
    Point2f ptRT_R = ptRotatePt2f (Point2f (ptRT), ptCenter, dRAngle_radian);

    float fTopY = max (max (ptLT_R.y, ptLB_R.y), max (ptRB_R.y, ptRT_R.y));
    float fBottomY = min (min (ptLT_R.y, ptLB_R.y), min (ptRB_R.y, ptRT_R.y));
    float fRightX = max (max (ptLT_R.x, ptLB_R.x), max (ptRB_R.x, ptRT_R.x));
    float fLeftX = min (min (ptLT_R.x, ptLB_R.x), min (ptRB_R.x, ptRT_R.x));

    if (dRAngle > 360)
        dRAngle -= 360;
    else if (dRAngle < 0)
        dRAngle += 360;

    if (fabs (fabs (dRAngle) - 90) < VISION_TOLERANCE || fabs (fabs (dRAngle) - 270) < VISION_TOLERANCE)
    {
        return Size (sizeSrc.height, sizeSrc.width);
    }
    else if (fabs (dRAngle) < VISION_TOLERANCE || fabs (fabs (dRAngle) - 180) < VISION_TOLERANCE)
    {
        return sizeSrc;
    }

    double dAngle = dRAngle;

    if (dAngle > 0 && dAngle < 90)
    {
        ;
    }
    else if (dAngle > 90 && dAngle < 180)
    {
        dAngle -= 90;
    }
    else if (dAngle > 180 && dAngle < 270)
    {
        dAngle -= 180;
    }
    else if (dAngle > 270 && dAngle < 360)
    {
        dAngle -= 270;
    }
    else//Debug
    {
        AfxMessageBox (L"Unkown");
    }

    float fH1 = sizeDst.width * sin (dAngle * D2R) * cos (dAngle * D2R);
    float fH2 = sizeDst.height * sin (dAngle * D2R) * cos (dAngle * D2R);

    int iHalfHeight = (int)ceil (fTopY - ptCenter.y - fH1);
    int iHalfWidth = (int)ceil (fRightX - ptCenter.x - fH2);

    Size sizeRet (iHalfWidth * 2, iHalfHeight * 2);

    BOOL bWrongSize = (sizeDst.width < sizeRet.width && sizeDst.height > sizeRet.height)
            || (sizeDst.width > sizeRet.width && sizeDst.height < sizeRet.height
                || sizeDst.area () > sizeRet.area ());
    if (bWrongSize)
        sizeRet = Size (int (fRightX - fLeftX + 0.5), int (fTopY - fBottomY + 0.5));

    return sizeRet;
}

//From ImageShop
// 4個有符號的32位的數據相加的和。
inline int _mm_hsum_epi32 (__m128i V)      // V3 V2 V1 V0
{
    // 實測這個速度要快些，_mm_extract_epi32最慢。
    __m128i T = _mm_add_epi32 (V, _mm_srli_si128 (V, 8));  // V3+V1   V2+V0  V1  V0
    T = _mm_add_epi32 (T, _mm_srli_si128 (T, 4));    // V3+V1+V2+V0  V2+V0+V1 V1+V0 V0
    return _mm_cvtsi128_si32 (T);       // 提取低位
}
// 基於SSE的字節數據的乘法。
// <param name="Kernel">需要卷積的核矩陣。 </param>
// <param name="Conv">卷積矩陣。 </param>
// <param name="Length">矩陣所有元素的長度。 </param>
inline int IM_Conv_SIMD (unsigned char* pCharKernel, unsigned char *pCharConv, int iLength)
{
    const int iBlockSize = 16, Block = iLength / iBlockSize;
    __m128i SumV = _mm_setzero_si128 ();
    __m128i Zero = _mm_setzero_si128 ();
    for (int Y = 0; Y < Block * iBlockSize; Y += iBlockSize)
    {
        __m128i SrcK = _mm_loadu_si128 ((__m128i*)(pCharKernel + Y));
        __m128i SrcC = _mm_loadu_si128 ((__m128i*)(pCharConv + Y));
        __m128i SrcK_L = _mm_unpacklo_epi8 (SrcK, Zero);
        __m128i SrcK_H = _mm_unpackhi_epi8 (SrcK, Zero);
        __m128i SrcC_L = _mm_unpacklo_epi8 (SrcC, Zero);
        __m128i SrcC_H = _mm_unpackhi_epi8 (SrcC, Zero);
        __m128i SumT = _mm_add_epi32 (_mm_madd_epi16 (SrcK_L, SrcC_L), _mm_madd_epi16 (SrcK_H, SrcC_H));
        SumV = _mm_add_epi32 (SumV, SumT);
    }
    int Sum = _mm_hsum_epi32 (SumV);
    for (int Y = Block * iBlockSize; Y < iLength; Y++)
    {
        Sum += pCharKernel[Y] * pCharConv[Y];
    }
    return Sum;
}
//#define ORG


void measure_bc::CCOEFF_Denominator (cv::Mat& matSrc, s_TemplData* pTemplData, cv::Mat& matResult, int iLayer)
{
    if (pTemplData->vecResultEqual1[iLayer])
    {
        matResult = cv::Scalar::all (1);
        return;
    }
    double *q0 = 0, *q1 = 0, *q2 = 0, *q3 = 0;

    cv::Mat sum, sqsum;
    integral (matSrc, sum, sqsum, CV_64F);

    double d2 = clock ();

    q0 = (double*)sqsum.data;
    q1 = q0 + pTemplData->vecPyramid[iLayer].cols;
    q2 = (double*)(sqsum.data + pTemplData->vecPyramid[iLayer].rows * sqsum.step);
    q3 = q2 + pTemplData->vecPyramid[iLayer].cols;

    double* p0 = (double*)sum.data;
    double* p1 = p0 + pTemplData->vecPyramid[iLayer].cols;
    double* p2 = (double*)(sum.data + pTemplData->vecPyramid[iLayer].rows*sum.step);
    double* p3 = p2 + pTemplData->vecPyramid[iLayer].cols;

    int sumstep = sum.data ? (int)(sum.step / sizeof (double)) : 0;
    int sqstep = sqsum.data ? (int)(sqsum.step / sizeof (double)) : 0;

    //
    double dTemplMean0 = pTemplData->vecTemplMean[iLayer][0];
    double dTemplNorm = pTemplData->vecTemplNorm[iLayer];
    double dInvArea = pTemplData->vecInvArea[iLayer];
    //

    int i, j;
    for (i = 0; i < matResult.rows; i++)
    {
        float* rrow = matResult.ptr<float> (i);
        int idx = i * sumstep;
        int idx2 = i * sqstep;

        for (j = 0; j < matResult.cols; j += 1, idx += 1, idx2 += 1)
        {
            double num = rrow[j], t;
            double wndMean2 = 0, wndSum2 = 0;

            t = p0[idx] - p1[idx] - p2[idx] + p3[idx];
            wndMean2 += t * t;
            num -= t * dTemplMean0;
            wndMean2 *= dInvArea;


            t = q0[idx2] - q1[idx2] - q2[idx2] + q3[idx2];
            wndSum2 += t;


            //t = std::sqrt (MAX (wndSum2 - wndMean2, 0)) * dTemplNorm;

            double diff2 = MAX (wndSum2 - wndMean2, 0);
            if (diff2 <= MIN (0.5, 10 * FLT_EPSILON * wndSum2))
                t = 0; // avoid rounding errors
            else
                t = std::sqrt (diff2)*dTemplNorm;

            if (fabs (num) < t)
                num /= t;
            else if (fabs (num) < t * 1.125)
                num = num > 0 ? 1 : -1;
            else
                num = 0;

            rrow[j] = (float)num;
        }
    }
}


void measure_bc::MatchTemplate (cv::Mat& matSrc, s_TemplData* pTemplData, cv::Mat& matResult, int iLayer, BOOL bUseSIMD)
{
    //if (m_ckSIMD.GetCheck () && bUseSIMD)
    if( bUseSIMD)
    {

        //From ImageShop
        matResult.create (matSrc.rows - pTemplData->vecPyramid[iLayer].rows + 1,
                          matSrc.cols - pTemplData->vecPyramid[iLayer].cols + 1, CV_32FC1);
        matResult.setTo (0);
        cv::Mat& matTemplate = pTemplData->vecPyramid[iLayer];

        int  t_r_end = matTemplate.rows, t_r = 0;
        for (int r = 0; r < matResult.rows; r++)
        {
            float* r_matResult = matResult.ptr<float> (r);
            uchar* r_source = matSrc.ptr<uchar> (r);
            uchar* r_template, *r_sub_source;
            for (int c = 0; c < matResult.cols; ++c, ++r_matResult, ++r_source)
            {
                r_template = matTemplate.ptr<uchar> ();
                r_sub_source = r_source;
                for (t_r = 0; t_r < t_r_end; ++t_r, r_sub_source += matSrc.cols, r_template += matTemplate.cols)
                {
                    *r_matResult = *r_matResult + IM_Conv_SIMD (r_template, r_sub_source, matTemplate.cols);
                }
            }
        }
        //From ImageShop
    }
    else
        matchTemplate (matSrc, pTemplData->vecPyramid[iLayer], matResult, CV_TM_CCORR);

    /*Mat diff;
    absdiff(matResult, matResult, diff);
    double dMaxValue;
    minMaxLoc(diff, 0, &dMaxValue, 0,0);*/
    CCOEFF_Denominator (matSrc, pTemplData, matResult, iLayer);
}


cv::Point measure_bc::GetNextMaxLoc (cv::Mat & matResult, cv::Point ptMaxLoc, double dMinValue, int iTemplateW, int iTemplateH, double& dMaxValue, double dMaxOverlap)
{
    //比對到的區域完全不重疊 : +-一個樣板寬高
    //int iStartX = ptMaxLoc.x - iTemplateW;
    //int iStartY = ptMaxLoc.y - iTemplateH;
    //int iEndX = ptMaxLoc.x + iTemplateW;

    //int iEndY = ptMaxLoc.y + iTemplateH;
    ////塗黑
    //rectangle (matResult, Rect (iStartX, iStartY, 2 * iTemplateW * (1-dMaxOverlap * 2), 2 * iTemplateH * (1-dMaxOverlap * 2)), Scalar (dMinValue), CV_FILLED);
    ////得到下一個最大值
    //Point ptNewMaxLoc;
    //minMaxLoc (matResult, 0, &dMaxValue, 0, &ptNewMaxLoc);
    //return ptNewMaxLoc;

    //比對到的區域需考慮重疊比例
    int iStartX = ptMaxLoc.x - iTemplateW * (1 - dMaxOverlap);
    int iStartY = ptMaxLoc.y - iTemplateH * (1 - dMaxOverlap);
    int iEndX = ptMaxLoc.x + iTemplateW * (1 - dMaxOverlap);

    int iEndY = ptMaxLoc.y + iTemplateH * (1 - dMaxOverlap);
    //塗黑
    rectangle (matResult, Rect (iStartX, iStartY, 2 * iTemplateW * (1- dMaxOverlap), 2 * iTemplateH * (1- dMaxOverlap)), Scalar (dMinValue), CV_FILLED);
    //得到下一個最大值
    Point ptNewMaxLoc;
    minMaxLoc (matResult, 0, &dMaxValue, 0, &ptNewMaxLoc);
    return ptNewMaxLoc;
}
void measure_bc::SortPtWithCenter (vector<cv::Point2f>& vecSort)
{
    int iSize = (int)vecSort.size ();
    cv::Point2f ptCenter;
    for (int i = 0; i < iSize; i++)
        ptCenter += vecSort[i];
    //ptCenter /= iSize;

    ptCenter.x = ptCenter.x/ iSize;
    ptCenter.y = ptCenter.y/ iSize;

    cv::Point2f vecX (1, 0);

    vector<pair<cv::Point2f, double>> vecPtAngle (iSize);
    for (int i = 0; i < iSize; i++)
    {
        vecPtAngle[i].first = vecSort[i];//pt
        cv::Point2f vec1 (vecSort[i].x - ptCenter.x, vecSort[i].y - ptCenter.y);
        float fNormVec1 = vec1.x * vec1.x + vec1.y * vec1.y;
        float fDot = vec1.x;

        if (vec1.y < 0)//若點在中心的上方
        {
            vecPtAngle[i].second = acos (fDot / fNormVec1) * R2D;
        }
        else if (vec1.y > 0)//下方
        {
            vecPtAngle[i].second = 360 - acos (fDot / fNormVec1) * R2D;
        }
        else//點與中心在相同Y
        {
            if (vec1.x - ptCenter.x > 0)
                vecPtAngle[i].second = 0;
            else
                vecPtAngle[i].second = 180;
        }

    }
    sort (vecPtAngle.begin (), vecPtAngle.end (), comparePtWithAngle);
    for (int i = 0; i < iSize; i++)
        vecSort[i] = vecPtAngle[i].first;
}



void  measure_bc::GetRotatedROI (cv::Mat& matSrc, cv::Size size, cv::Point2f ptLT, double dAngle, cv::Mat& matROI)
{
    double dAngle_radian = dAngle * D2R;
    cv::Point2f ptC ((matSrc.cols - 1) / 2.0f, (matSrc.rows - 1) / 2.0f);
    cv::Point2f ptLT_rotate = ptRotatePt2f (ptLT, ptC, dAngle_radian);
    cv::Size sizePadding (size.width + 6, size.height + 6);


    cv::Mat rMat = getRotationMatrix2D (ptC, dAngle, 1);
    rMat.at<double> (0, 2) -= ptLT_rotate.x - 3;
    rMat.at<double> (1, 2) -= ptLT_rotate.y - 3;
    //平移旋轉矩陣(0, 2) (1, 2)的減，為旋轉後的圖形偏移，-= ptLT_rotate.x - 3 代表旋轉後的圖形往-X方向移動ptLT_rotate.x - 3
    //Debug

    //Debug
    warpAffine (matSrc, matROI, rMat, sizePadding);
}

BOOL measure_bc::SubPixEsimation (vector<s_MatchParameter>* vec, double* dNewX, double* dNewY, double* dNewAngle, double dAngleStep, int iMaxScoreIndex)
{
    //Az=S, (A.T)Az=(A.T)s, z = ((A.T)A).inv (A.T)s

    cv::Mat matA (27, 10, CV_64F);
    cv::Mat matZ (10, 1, CV_64F);
    cv::Mat matS (27, 1, CV_64F);

    double dX_maxScore = (*vec)[iMaxScoreIndex].pt.x;
    double dY_maxScore = (*vec)[iMaxScoreIndex].pt.y;
    double dTheata_maxScore = (*vec)[iMaxScoreIndex].dMatchAngle;
    int iRow = 0;
    /*for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            for (int theta = 0; theta <= 2; theta++)
            {*/
    for (int theta = 0; theta <= 2; theta++)
    {
        for (int y = -1; y <= 1; y++)
        {
            for (int x = -1; x <= 1; x++)
            {
                //xx yy tt xy xt yt x y t 1
                //0  1  2  3  4  5  6 7 8 9
                double dX = dX_maxScore + x;
                double dY = dY_maxScore + y;
                //double dT = (*vec)[theta].dMatchAngle + (theta - 1) * dAngleStep;
                double dT = (dTheata_maxScore + (theta - 1) * dAngleStep) * D2R;
                matA.at<double> (iRow, 0) = dX * dX;
                matA.at<double> (iRow, 1) = dY * dY;
                matA.at<double> (iRow, 2) = dT * dT;
                matA.at<double> (iRow, 3) = dX * dY;
                matA.at<double> (iRow, 4) = dX * dT;
                matA.at<double> (iRow, 5) = dY * dT;
                matA.at<double> (iRow, 6) = dX;
                matA.at<double> (iRow, 7) = dY;
                matA.at<double> (iRow, 8) = dT;
                matA.at<double> (iRow, 9) = 1.0;
                matS.at<double> (iRow, 0) = (*vec)[iMaxScoreIndex + (theta - 1)].vecResult[x + 1][y + 1];
                iRow++;
#ifdef _DEBUG
                /*string str = format ("%.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f", dValueA[0], dValueA[1], dValueA[2], dValueA[3], dValueA[4], dValueA[5], dValueA[6], dValueA[7], dValueA[8], dValueA[9]);
                fileA <<  str << endl;
                str = format ("%.6f", dValueS[iRow]);
                fileS << str << endl;*/
#endif
            }
        }
    }
    //求解Z矩陣，得到k0~k9
    //[ x* ] = [ 2k0 k3 k4 ]-1 [ -k6 ]
    //| y* | = | k3 2k1 k5 |   | -k7 |
    //[ t* ] = [ k4 k5 2k2 ]   [ -k8 ]

    //solve (matA, matS, matZ, DECOMP_SVD);
    matZ = (matA.t () * matA).inv () * matA.t ()* matS;
    cv::Mat matZ_t;
    transpose (matZ, matZ_t);
    double* dZ = matZ_t.ptr<double> (0);
    cv::Mat matK1 = (cv::Mat_<double> (3, 3) <<
                     (2 * dZ[0]), dZ[3], dZ[4],
            dZ[3], (2 * dZ[1]), dZ[5],
            dZ[4], dZ[5], (2 * dZ[2]));
    cv::Mat matK2 = (cv::Mat_<double> (3, 1) << -dZ[6], -dZ[7], -dZ[8]);
    cv::Mat matDelta = matK1.inv () * matK2;

    *dNewX = matDelta.at<double> (0, 0);
    *dNewY = matDelta.at<double> (1, 0);
    *dNewAngle = matDelta.at<double> (2, 0) * R2D;
    return TRUE;
}


void measure_bc::FilterWithScore (vector<s_MatchParameter>* vec, double dScore)
{
    sort (vec->begin (), vec->end (), compareScoreBig2Small);
    int iSize = vec->size (), iIndexDelete = iSize + 1;
    for (int i = 0; i < iSize; i++)
    {
        if ((*vec)[i].dMatchScore < dScore)
        {
            iIndexDelete = i;
            break;
        }
    }
    if (iIndexDelete == iSize + 1)//沒有任何元素小於dScore
        return;
    vec->erase (vec->begin () + iIndexDelete, vec->end ());
    return;
    //刪除小於比對分數的元素
    vector<s_MatchParameter>::iterator it;
    for (it = vec->begin (); it != vec->end ();)
    {
        if (((*it).dMatchScore < dScore))
            it = vec->erase (it);
        else
            ++it;
    }
}


void measure_bc::FilterWithRotatedRect (vector<s_MatchParameter>* vec, int iMethod, double dMaxOverLap)
{
    int iMatchSize = (int)vec->size ();
    cv::RotatedRect rect1, rect2;
    for (int i = 0; i < iMatchSize - 1; i++)
    {
        if (vec->at (i).bDelete)
            continue;
        for (int j = i + 1; j < iMatchSize; j++)
        {
            if (vec->at (j).bDelete)
                continue;
            rect1 = vec->at (i).rectR;
            rect2 = vec->at (j).rectR;
            vector<cv::Point2f> vecInterSec;
            int iInterSecType = cv::rotatedRectangleIntersection (rect1, rect2, vecInterSec);
            if (iInterSecType ==  cv::INTERSECT_NONE)//無交集
                continue;
            else if (iInterSecType ==  cv::INTERSECT_FULL) //一個矩形包覆另一個
            {
                int iDeleteIndex;
                if (iMethod == CV_TM_SQDIFF)
                    iDeleteIndex = (vec->at (i).dMatchScore <= vec->at (j).dMatchScore) ? j : i;
                else
                    iDeleteIndex = (vec->at (i).dMatchScore >= vec->at (j).dMatchScore) ? j : i;
                vec->at (iDeleteIndex).bDelete = TRUE;
            }
            else//交點 > 0
            {
                if (vecInterSec.size () < 3)//一個或兩個交點
                    continue;
                else
                {
                    int iDeleteIndex;
                    //求面積與交疊比例
                    SortPtWithCenter (vecInterSec);
                    double dArea = contourArea (vecInterSec);
                    double dRatio = dArea / rect1.size.area ();
                    //若大於最大交疊比例，選分數高的
                    if (dRatio > dMaxOverLap)
                    {
                        if (iMethod == CV_TM_SQDIFF)
                            iDeleteIndex = (vec->at (i).dMatchScore <= vec->at (j).dMatchScore) ? j : i;
                        else
                            iDeleteIndex = (vec->at (i).dMatchScore >= vec->at (j).dMatchScore) ? j : i;
                        vec->at (iDeleteIndex).bDelete = TRUE;
                    }
                }
            }
        }
    }
    vector<s_MatchParameter>::iterator it;
    for (it = vec->begin (); it != vec->end ();)
    {
        if ((*it).bDelete)
            it = vec->erase (it);
        else
            ++it;
    }
}


void measure_bc::LearnPattern ()
{
    //UpdateData (1);;
    m_TemplData.clear ();

    int iTopLayer = GetTopLayer (&m_matDst, (int)sqrt ((double)m_iMinReduceArea));
    buildPyramid (m_matDst, m_TemplData.vecPyramid, iTopLayer);
    s_TemplData* templData = &m_TemplData;
    templData->iBorderColor = mean (m_matDst).val[0] < 128 ? 255 : 0;
    int iSize = templData->vecPyramid.size ();
    templData->resize (iSize);

    for (int i = 0; i < iSize; i++)
    {
        double invArea = 1. / ((double)templData->vecPyramid[i].rows * templData->vecPyramid[i].cols);
        cv::Scalar templMean, templSdv;
        double templNorm = 0, templSum2 = 0;

        meanStdDev (templData->vecPyramid[i], templMean, templSdv);
        templNorm = templSdv[0] * templSdv[0] + templSdv[1] * templSdv[1] + templSdv[2] * templSdv[2] + templSdv[3] * templSdv[3];

        if (templNorm < DBL_EPSILON)
        {
            templData->vecResultEqual1[i] = TRUE;
        }
        templSum2 = templNorm + templMean[0] * templMean[0] + templMean[1] * templMean[1] + templMean[2] * templMean[2] + templMean[3] * templMean[3];


        templSum2 /= invArea;
        templNorm = std::sqrt (templNorm);
        templNorm /= std::sqrt (invArea); // care of accuracy here


        templData->vecInvArea[i] = invArea;
        templData->vecTemplMean[i] = templMean;
        templData->vecTemplNorm[i] = templNorm;
    }
    templData->bIsPatternLearned = TRUE;
}

BOOL measure_bc::Match ()
{
    if (m_matSrc.empty () || m_matDst.empty ())
        return FALSE;
    if ((m_matDst.cols < m_matSrc.cols && m_matDst.rows > m_matSrc.rows) || (m_matDst.cols > m_matSrc.cols && m_matDst.rows < m_matSrc.rows))
        return FALSE;
    if (m_matDst.size ().area () > m_matSrc.size ().area ())
        return FALSE;
    if (!m_TemplData.bIsPatternLearned)
        return FALSE;
    //UpdateData (1);



    //決定金字塔層數 總共為1 + iLayer層
    int iTopLayer = GetTopLayer (&m_matDst, (int)sqrt ((double)m_iMinReduceArea));
    //建立金字塔
    vector<Mat> vecMatSrcPyr;
    if (0)
    {
        Mat matNewSrc = 255 - m_matSrc;
        buildPyramid (matNewSrc, vecMatSrcPyr, iTopLayer);
        namedWindow("1",0);
        imshow ("1", matNewSrc);
        moveWindow ("1", 0, 0);
        waitKey(0);
    }
    else
        buildPyramid (m_matSrc, vecMatSrcPyr, iTopLayer);

    s_TemplData* pTemplData = &m_TemplData;

    //第一階段以最頂層找出大致角度與ROI
    double dAngleStep = atan (2.0 / max (pTemplData->vecPyramid[iTopLayer].cols, pTemplData->vecPyramid[iTopLayer].rows)) * R2D;


    vector<double> vecAngles;

    if (m_bToleranceRange)
    {
        if (m_dTolerance1 >= m_dTolerance2 || m_dTolerance3 >= m_dTolerance4)
        {
            AfxMessageBox (L"角度範圍設定異常，左值須小於右值");
            return FALSE;
        }
        for (double dAngle = m_dTolerance1; dAngle < m_dTolerance2 + dAngleStep; dAngle += dAngleStep)
            vecAngles.push_back (dAngle);
        for (double dAngle = m_dTolerance3; dAngle < m_dTolerance4 + dAngleStep; dAngle += dAngleStep)
            vecAngles.push_back (dAngle);
    }
    else
    {
        if (m_dToleranceAngle < VISION_TOLERANCE)
            vecAngles.push_back (0.0);
        else
        {
            for (double dAngle = 0; dAngle < m_dToleranceAngle + dAngleStep; dAngle += dAngleStep)
                vecAngles.push_back (dAngle);

            for (double dAngle = -dAngleStep; dAngle > -m_dToleranceAngle - dAngleStep; dAngle -= dAngleStep)
                vecAngles.push_back (dAngle);
        }
    }



    int iTopSrcW = vecMatSrcPyr[iTopLayer].cols, iTopSrcH = vecMatSrcPyr[iTopLayer].rows;
    Point2f ptCenter ((iTopSrcW - 1) / 2.0f, (iTopSrcH - 1) / 2.0f);


    int iSize = (int)vecAngles.size ();
    //vector<s_MatchParameter> vecMatchParameter (iSize * (m_iMaxPos + MATCH_CANDIDATE_NUM));
    vector<s_MatchParameter> vecMatchParameter;
    //Caculate lowest score at every layer
    vector<double> vecLayerScore (iTopLayer + 1, m_dScore);
    for (int iLayer = 1; iLayer <= iTopLayer; iLayer++)
        vecLayerScore[iLayer] = vecLayerScore[iLayer - 1] * 0.9;



    for (int i = 0; i < iSize; i++)
    {
        Mat matRotatedSrc, matR = getRotationMatrix2D (ptCenter, vecAngles[i], 1);
        Mat matResult;
        Point ptMaxLoc;
        double dValue, dMaxVal;
        //double dRotate = clock ();
        Size sizeBest = GetBestRotationSize (vecMatSrcPyr[iTopLayer].size (), pTemplData->vecPyramid[iTopLayer].size (), vecAngles[i]);

        float fTranslationX = (sizeBest.width - 1) / 2.0f - ptCenter.x;
        float fTranslationY = (sizeBest.height - 1) / 2.0f - ptCenter.y;
        matR.at<double> (0, 2) += fTranslationX;
        matR.at<double> (1, 2) += fTranslationY;
        warpAffine (vecMatSrcPyr[iTopLayer], matRotatedSrc, matR, sizeBest, INTER_LINEAR, BORDER_CONSTANT, Scalar (pTemplData->iBorderColor));

        MatchTemplate (matRotatedSrc, pTemplData, matResult, iTopLayer, FALSE);
        //matchTemplate (matRotatedSrc, pTemplData->vecPyramid[iTopLayer], matResult, CV_TM_CCOEFF_NORMED);

        minMaxLoc (matResult, 0, &dMaxVal, 0, &ptMaxLoc);
        if (dMaxVal < vecLayerScore[iTopLayer])
            continue;
        vecMatchParameter.push_back (s_MatchParameter (Point2f (ptMaxLoc.x - fTranslationX, ptMaxLoc.y - fTranslationY), dMaxVal, vecAngles[i]));

        for (int j = 0; j < m_iMaxPos + MATCH_CANDIDATE_NUM - 1; j++)
        {
            ptMaxLoc = GetNextMaxLoc (matResult, ptMaxLoc, -1, pTemplData->vecPyramid[iTopLayer].cols, pTemplData->vecPyramid[iTopLayer].rows, dValue, m_dMaxOverlap);
            if (dValue < vecLayerScore[iTopLayer])
                continue;
            vecMatchParameter.push_back (s_MatchParameter (Point2f (ptMaxLoc.x - fTranslationX, ptMaxLoc.y - fTranslationY), dValue, vecAngles[i]));

        }
    }
    sort (vecMatchParameter.begin (), vecMatchParameter.end (), compareScoreBig2Small);
    //FilterWithScore (&vecMatchParameter, m_dScore - 0.05*iTopLayer);

    //record rotated rectangle、ROI and angle
    int iMatchSize = (int)vecMatchParameter.size ();
    int iDstW = pTemplData->vecPyramid[iTopLayer].cols, iDstH = pTemplData->vecPyramid[iTopLayer].rows;



    for (int i = 0; i < iMatchSize; i++)
    {
        Point2f ptLT, ptRT, ptRB, ptLB;
        double dRAngle = -vecMatchParameter[i].dMatchAngle * D2R;
        ptLT = ptRotatePt2f (vecMatchParameter[i].pt, ptCenter, dRAngle);
        ptRT = Point2f (ptLT.x + iDstW * (float)cos (dRAngle), ptLT.y - iDstW * (float)sin (dRAngle));
        ptLB = Point2f (ptLT.x + iDstH * (float)sin (dRAngle), ptLT.y + iDstH * (float)cos (dRAngle));
        ptRB = Point2f (ptRT.x + iDstH * (float)sin (dRAngle), ptRT.y + iDstH * (float)cos (dRAngle));
        //紀錄旋轉矩形
        Point2f ptRectCenter = Point2f ((ptLT.x + ptRT.x + ptLB.x + ptRB.x) / 4.0f, (ptLT.y + ptRT.y + ptLB.y + ptRB.y) / 4.0f);
        vecMatchParameter[i].rectR = RotatedRect (ptRectCenter, pTemplData->vecPyramid[iTopLayer].size (), (float)vecMatchParameter[i].dMatchAngle);
    }
    //紀錄旋轉矩形
    //FilterWithRotatedRect (&vecMatchParameter, CV_TM_CCOEFF_NORMED, m_dMaxOverlap * m_dMaxOverlap);


    //顯示第一層結果
    if (m_bDebugMode)
    {
        int iDebugScale = 1;

        Mat matShow, matResize;
        resize (vecMatSrcPyr[iTopLayer], matResize, vecMatSrcPyr[iTopLayer].size () * iDebugScale);
        cvtColor (matResize, matShow, CV_GRAY2BGR);
        iMatchSize = (int)vecMatchParameter.size ();
        string str = format ("Toplayer, Candidate:%d", iMatchSize);
        for (int i = 0; i < iMatchSize; i++)
        {
            Point2f ptLT, ptRT, ptRB, ptLB;
            double dRAngle = -vecMatchParameter[i].dMatchAngle * D2R;
            ptLT = ptRotatePt2f (vecMatchParameter[i].pt, ptCenter, dRAngle);
            ptRT = Point2f (ptLT.x + iDstW * (float)cos (dRAngle), ptLT.y - iDstW * (float)sin (dRAngle));
            ptLB = Point2f (ptLT.x + iDstH * (float)sin (dRAngle), ptLT.y + iDstH * (float)cos (dRAngle));
            ptRB = Point2f (ptRT.x + iDstH * (float)sin (dRAngle), ptRT.y + iDstH * (float)cos (dRAngle));
            line (matShow, ptLT * iDebugScale, ptLB * iDebugScale, Scalar (0, 255, 0));
            line (matShow, ptLB * iDebugScale, ptRB * iDebugScale, Scalar (0, 255, 0));
            line (matShow, ptRB * iDebugScale, ptRT * iDebugScale, Scalar (0, 255, 0));
            line (matShow, ptRT * iDebugScale, ptLT * iDebugScale, Scalar (0, 255, 0));
            circle (matShow, ptLT * iDebugScale, 1, Scalar (0, 0, 255));

            string strText = format ("%d", i);
            putText (matShow, strText, ptLT *iDebugScale, FONT_HERSHEY_PLAIN, 1, Scalar (0, 255, 0));
            imshow (str, matShow);

        }
        imshow (str, matShow);
        moveWindow (str, 0, 0);
    }

    //顯示第一層結果
    int a = 0;
    //第一階段結束
    BOOL bSubPixelEstimation = FALSE;
    int iStopLayer = 0;
    //int iSearchSize = min (m_iMaxPos + MATCH_CANDIDATE_NUM, (int)vecMatchParameter.size ());//可能不需要搜尋到全部 太浪費時間
    vector<s_MatchParameter> vecAllResult;
    for (int i = 0; i < (int)vecMatchParameter.size (); i++)
    {
        double dRAngle = -vecMatchParameter[i].dMatchAngle * D2R;
        Point2f ptLT = ptRotatePt2f (vecMatchParameter[i].pt, ptCenter, dRAngle);

        double dAngleStep = atan (2.0 / max (iDstW, iDstH)) * R2D;//min改為max
        vecMatchParameter[i].dAngleStart = vecMatchParameter[i].dMatchAngle - dAngleStep;
        vecMatchParameter[i].dAngleEnd = vecMatchParameter[i].dMatchAngle + dAngleStep;

        if (iTopLayer <= iStopLayer)
        {
            vecMatchParameter[i].pt = Point2d (ptLT * ((iTopLayer == 0) ? 1 : 2));
            vecAllResult.push_back (vecMatchParameter[i]);
        }
        else
        {
            for (int iLayer = iTopLayer - 1; iLayer >= iStopLayer; iLayer--)
            {
                //搜尋角度
                dAngleStep = atan (2.0 / max (pTemplData->vecPyramid[iLayer].cols, pTemplData->vecPyramid[iLayer].rows)) * R2D;//min改為max
                vector<double> vecAngles;
                //double dAngleS = vecMatchParameter[i].dAngleStart, dAngleE = vecMatchParameter[i].dAngleEnd;
                double dMatchedAngle = vecMatchParameter[i].dMatchAngle;
                if (m_bToleranceRange)
                {
                    for (int i = -1; i <= 1; i++)
                        vecAngles.push_back (dMatchedAngle + dAngleStep * i);
                }
                else
                {
                    if (m_dToleranceAngle < VISION_TOLERANCE)
                        vecAngles.push_back (0.0);
                    else
                        for (int i = -1; i <= 1; i++)
                            vecAngles.push_back (dMatchedAngle + dAngleStep * i);
                }
                Point2f ptSrcCenter ((vecMatSrcPyr[iLayer].cols - 1) / 2.0f, (vecMatSrcPyr[iLayer].rows - 1) / 2.0f);
                iSize = (int)vecAngles.size ();
                vector<s_MatchParameter> vecNewMatchParameter (iSize);
                int iMaxScoreIndex = 0;
                double dBigValue = -1;
                for (int j = 0; j < iSize; j++)
                {
                    Mat matResult, matRotatedSrc;
                    double dMaxValue = 0;
                    Point ptMaxLoc;
                    GetRotatedROI (vecMatSrcPyr[iLayer], pTemplData->vecPyramid[iLayer].size (), ptLT * 2, vecAngles[j], matRotatedSrc);
                    MatchTemplate (matRotatedSrc, pTemplData, matResult, iLayer, TRUE);
                    //matchTemplate (matRotatedSrc, pTemplData->vecPyramid[iLayer], matResult, CV_TM_CCOEFF_NORMED);
                    minMaxLoc (matResult, 0, &dMaxValue, 0, &ptMaxLoc);
                    vecNewMatchParameter[j] = s_MatchParameter (ptMaxLoc, dMaxValue, vecAngles[j]);

                    if (vecNewMatchParameter[j].dMatchScore > dBigValue)
                    {
                        iMaxScoreIndex = j;
                        dBigValue = vecNewMatchParameter[j].dMatchScore;
                    }
                    //次像素估計
                    if (ptMaxLoc.x == 0 || ptMaxLoc.y == 0 || ptMaxLoc.x == matResult.cols - 1 || ptMaxLoc.y == matResult.rows - 1)
                        vecNewMatchParameter[j].bPosOnBorder = TRUE;
                    if (!vecNewMatchParameter[j].bPosOnBorder)
                    {
                        for (int y = -1; y <= 1; y++)
                            for (int x = -1; x <= 1; x++)
                                vecNewMatchParameter[j].vecResult[x + 1][y + 1] = matResult.at<float> (ptMaxLoc + Point (x, y));
                    }
                    //次像素估計
                }
                if (vecNewMatchParameter[iMaxScoreIndex].dMatchScore < vecLayerScore[iLayer])
                    break;
                //次像素估計
                if (bSubPixelEstimation
                        && iLayer == 0
                        && (!vecNewMatchParameter[iMaxScoreIndex].bPosOnBorder)
                        && iMaxScoreIndex != 0
                        && iMaxScoreIndex != 2)
                {
                    double dNewX = 0, dNewY = 0, dNewAngle = 0;
                    SubPixEsimation ( &vecNewMatchParameter, &dNewX, &dNewY, &dNewAngle, dAngleStep, iMaxScoreIndex);
                    vecNewMatchParameter[iMaxScoreIndex].pt = Point2d (dNewX, dNewY);
                    vecNewMatchParameter[iMaxScoreIndex].dMatchAngle = dNewAngle;
                }
                //次像素估計

                double dNewMatchAngle = vecNewMatchParameter[iMaxScoreIndex].dMatchAngle;

                //讓坐標系回到旋轉時(GetRotatedROI)的(0, 0)
                Point2f ptPaddingLT = ptRotatePt2f (ptLT * 2, ptSrcCenter, dNewMatchAngle * D2R) - Point2f (3, 3);
                Point2f pt (vecNewMatchParameter[iMaxScoreIndex].pt.x + ptPaddingLT.x, vecNewMatchParameter[iMaxScoreIndex].pt.y + ptPaddingLT.y);
                //再旋轉
                pt = ptRotatePt2f (pt, ptSrcCenter, -dNewMatchAngle * D2R);

                if (iLayer == iStopLayer)
                {
                    vecNewMatchParameter[iMaxScoreIndex].pt = pt * (iStopLayer == 0 ? 1 : 2);
                    vecAllResult.push_back (vecNewMatchParameter[iMaxScoreIndex]);
                }
                else
                {
                    //更新MatchAngle ptLT
                    vecMatchParameter[i].dMatchAngle = dNewMatchAngle;
                    vecMatchParameter[i].dAngleStart = vecMatchParameter[i].dMatchAngle - dAngleStep / 2;
                    vecMatchParameter[i].dAngleEnd = vecMatchParameter[i].dMatchAngle + dAngleStep / 2;
                    ptLT = pt;
                }
            }

        }
    }
    FilterWithScore (&vecAllResult, m_dScore);

    //最後再次濾掉重疊
    iDstW = pTemplData->vecPyramid[iStopLayer].cols, iDstH = pTemplData->vecPyramid[iStopLayer].rows;

    for (int i = 0; i < (int)vecAllResult.size (); i++)
    {
        Point2f ptLT, ptRT, ptRB, ptLB;
        double dRAngle = -vecAllResult[i].dMatchAngle * D2R;
        ptLT = vecAllResult[i].pt;
        ptRT = Point2f (ptLT.x + iDstW * (float)cos (dRAngle), ptLT.y - iDstW * (float)sin (dRAngle));
        ptLB = Point2f (ptLT.x + iDstH * (float)sin (dRAngle), ptLT.y + iDstH * (float)cos (dRAngle));
        ptRB = Point2f (ptRT.x + iDstH * (float)sin (dRAngle), ptRT.y + iDstH * (float)cos (dRAngle));
        //紀錄旋轉矩形
        Point2f ptRectCenter = Point2f ((ptLT.x + ptRT.x + ptLB.x + ptRB.x) / 4.0f, (ptLT.y + ptRT.y + ptLB.y + ptRB.y) / 4.0f);
        vecAllResult[i].rectR = RotatedRect (ptRectCenter, pTemplData->vecPyramid[iStopLayer].size (), (float)vecAllResult[i].dMatchAngle);
    }
    FilterWithRotatedRect (&vecAllResult, CV_TM_CCOEFF_NORMED, m_dMaxOverlap);
    //最後再次濾掉重疊

    //根據分數排序
    sort (vecAllResult.begin (), vecAllResult.end (), compareScoreBig2Small);
    //m_strExecureTime.Format (L"%s : %d ms", m_strLanExecutionTime, int (clock () - d1));
    //m_statusBar.SetPaneText (0, m_strExecureTime);

    m_vecSingleTargetData.clear ();
    //  m_listMsg.DeleteAllItems ();
    iMatchSize = (int)vecAllResult.size ();
    if (vecAllResult.size () == 0)
        return FALSE;
    int iW = pTemplData->vecPyramid[0].cols, iH = pTemplData->vecPyramid[0].rows;


    for (int i = 0; i < iMatchSize; i++)
    {
        s_SingleTargetMatch sstm;
        Point2d offsetxy = Point2d(offset_x, offset_y);


        double dRAngle = -vecAllResult[i].dMatchAngle * D2R;

        sstm.ptLT = vecAllResult[i].pt ;

        sstm.ptRT = Point2d (sstm.ptLT.x + iW * cos (dRAngle), sstm.ptLT.y - iW * sin (dRAngle));
        sstm.ptLB = Point2d (sstm.ptLT.x + iH * sin (dRAngle), sstm.ptLT.y + iH * cos (dRAngle)) ;
        sstm.ptRB = Point2d (sstm.ptRT.x + iH * sin (dRAngle), sstm.ptRT.y + iH * cos (dRAngle)) ;
        sstm.ptCenter = Point2d ((sstm.ptLT.x + sstm.ptRT.x + sstm.ptRB.x + sstm.ptLB.x) / 4, (sstm.ptLT.y + sstm.ptRT.y + sstm.ptRB.y + sstm.ptLB.y) / 4) ;
        sstm.dMatchedAngle = -vecAllResult[i].dMatchAngle;
        sstm.dMatchScore = vecAllResult[i].dMatchScore;

        if (sstm.dMatchedAngle < -180)
            sstm.dMatchedAngle += 360;
        if (sstm.dMatchedAngle > 180)
            sstm.dMatchedAngle -= 360;


        sstm.ptLT = sstm.ptLT + offsetxy ;
        sstm.ptRT = sstm.ptRT+ offsetxy ;
        sstm.ptLB = sstm.ptLB+ offsetxy ;
        sstm.ptRB = sstm.ptRB + offsetxy ;
        sstm.ptCenter = sstm.ptCenter+ offsetxy ;

        //sstm.result_rc;
        //sstm.result_rc.setTopLeft( QPointF(sstm.ptLT.x,  sstm.ptLT.y ));

        // sstm.result_rc.setTopRight( QPointF(sstm.ptRT.x,  sstm.ptRT.y ));

        // sstm.result_rc.setBottomLeft( QPointF(sstm.ptLB.x,  sstm.ptLB.y ));
        //sstm.result_rc.setBottomRight(QPointF(sstm.ptRB.x,  sstm.ptRB.y ));

        m_vecSingleTargetData.push_back (sstm);



        //Test Subpixel
        /*Point2d ptLT = vecAllResult[i].ptSubPixel;
            Point2d ptRT = Point2d (sstm.ptLT.x + iW * cos (dRAngle), sstm.ptLT.y - iW * sin (dRAngle));
            Point2d ptLB = Point2d (sstm.ptLT.x + iH * sin (dRAngle), sstm.ptLT.y + iH * cos (dRAngle));
            Point2d ptRB = Point2d (sstm.ptRT.x + iH * sin (dRAngle), sstm.ptRT.y + iH * cos (dRAngle));
            Point2d ptCenter = Point2d ((sstm.ptLT.x + sstm.ptRT.x + sstm.ptRB.x + sstm.ptLB.x) / 4, (sstm.ptLT.y + sstm.ptRT.y + sstm.ptRB.y + sstm.ptLB.y) / 4);
            CString strDiff;strDiff.Format (L"Diff(x, y):%.3f, %.3f", ptCenter.x - sstm.ptCenter.x, ptCenter.y - sstm.ptCenter.y);
            AfxMessageBox (strDiff);*/
        //Test Subpixel
        //存出MATCH ROI
        //OutputRoi (sstm);
        if (i + 1 == m_iMaxPos)
            break;
    }
    //sort (m_vecSingleTargetData.begin (), m_vecSingleTargetData.end (), compareMatchResultByPosX);
    // m_listMsg.DeleteAllItems ();

    //    for (int i = 0 ; i < (int)m_vecSingleTargetData.size (); i++)
    //    {
    //        s_SingleTargetMatch sstm = m_vecSingleTargetData[i];

    //       // std::cout<<i<<"->Score:"<<sstm.dMatchScore<<" MatchedAngle:"<<sstm.dMatchedAngle<<std::endl;
    //        //Msg
    //        //        CString str (L"");
    //        //        m_listMsg.InsertItem (i, str);
    //        //        m_listMsg.SetCheck (i);
    //        //        str.Format (L"%d", i);
    //        //        m_listMsg.SetItemText (i, SUBITEM_INDEX, str);
    //        //        str.Format (L"%.3f", sstm.dMatchScore);
    //        //        m_listMsg.SetItemText (i, SUBITEM_SCORE, str);
    //        //        str.Format (L"%.3f", sstm.dMatchedAngle);
    //        //        m_listMsg.SetItemText (i, SUBITEM_ANGLE, str);
    //        //        str.Format (L"%.3f", sstm.ptCenter.x);
    //        //        m_listMsg.SetItemText (i, SUBITEM_POS_X, str);
    //        //        str.Format (L"%.3f", sstm.ptCenter.y);
    //        //        m_listMsg.SetItemText (i, SUBITEM_POS_Y, str);
    //        //Msg
    //    }

    // cout<<"---------------7---------------->"<<endl;
    //sort (m_vecSingleTargetData.begin (), m_vecSingleTargetData.end (), compareMatchResultByPos);
    // m_bShowResult = TRUE;

    //RefreshSrcView ();



}


cv::Mat cal_mat_estimateRigidTransform(cv::Point2f* src, cv::Point2f* dst, int num_point)
{

    Mat warpMat_xiangji_dao_jizuobiao = getAffineTransform(src, dst);
    return warpMat_xiangji_dao_jizuobiao;
}
void cal_point_estimateRigidTransform(cv::Point2f *src, cv::Point2f *dst, int num_point,cv::Mat &warpMat_xiangji_dao_jizuobiao,int width, int height)
{

    vector<Point2f> vt_src;
    vector<Point2f> vt_dst;
    for (int i = 0; i < 3; i++)
    {
        vt_src.push_back(src[i]);
    }

    warpAffine(cv::Mat(vt_src), cv::Mat(vt_dst), warpMat_xiangji_dao_jizuobiao, Size(1, 3), CV_INTER_LINEAR /*CV_INTER_CUBIC*/);
    for (int i = 0; i < 3; i++)
    {
        dst[i] = vt_dst[i];
    }
}


void measure_bc::set_dx_pt( MeasureUnit & m_obj_A , QRectF template_rc_chushihua, s_SingleTargetMatch guocheng_4_pt ,int flag )
{

    cv::Point2f src[4];
    cv::Point2f dst[4];
    src[0].x = template_rc_chushihua.topLeft().x();
    src[0].y = template_rc_chushihua.topLeft().y();

    src[1].x =  template_rc_chushihua.bottomLeft().x();
    src[1].y = template_rc_chushihua.bottomLeft().y();

    src[2].x =  template_rc_chushihua.bottomRight().x();
    src[2].y = template_rc_chushihua.bottomRight().y();

    src[3].x =  template_rc_chushihua.topRight().x();
    src[3].y =  template_rc_chushihua.topRight().y();



    dst[0] =  guocheng_4_pt.ptLT;
    dst[1] =  guocheng_4_pt.ptLB;;
    dst[2] =  guocheng_4_pt.ptRB;
    dst[3] = guocheng_4_pt.ptRT;


    tsfIPM = cv::getPerspectiveTransform(src, dst);
    tsfIPMInv = tsfIPM.inv();

    if(flag)
    {
        vector<cv::Point2f> ps;
        vector<cv::Point2f> psOut;

        ps.push_back(Point2f( m_obj_A.cfg.pt1.x, m_obj_A.cfg.pt1.y ));
        ps.push_back(Point2f( m_obj_A.cfg.pt2.x, m_obj_A.cfg.pt2.y));
        cv::perspectiveTransform(ps, psOut, tsfIPM);
        m_obj_A.cfg.pt1 = cv::Point( cvRound( psOut[0].x  ) , cvRound( psOut[0].y  ));//起点，用cv替代
        m_obj_A.cfg.pt2 = cv::Point(  cvRound( psOut[1].x  ) , cvRound( psOut[1].y  ));//起点，用cv替代

    }

}

void measure_bc::set_roi_offset(double _offset_x, double _offset_y)
{

    offset_x =_offset_x;
    offset_y = _offset_y;

}

void measure_bc::OutputRoi (s_SingleTargetMatch sstm)
{
    //if (!m_ckOutputROI.GetCheck ())
    //	return;
    Rect rect (sstm.ptLT, sstm.ptRB);
    for (int i = 1 ; i < 50 ; i++)
    {
        String strName = format (".\\xxx\\roi%d.bmp", i);
        if (::PathFileExists (CString (strName.c_str ())))
            continue;
        imwrite (strName, m_matSrc (rect));
        break;
    }
}

