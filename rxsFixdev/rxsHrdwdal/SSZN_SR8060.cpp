#include "rxsHrdwdal.h"
#include "proteced.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "SR7Link.h"
#include <thread>


#define	SSZN_YRATE			0.012
#define SSZN_ZRATE			0.00001


std::vector<rxsPointCouldp> PCResult;



SSZN_SR8060::SSZN_SR8060() {
    Sensorinfo.devType = ImageableSensorUnit::LineLsr;
    Sensorinfo.ID = 0;
    reT = 0;
    CtrInfo = new ControlInfo;
    memset(CtrInfo, 0, sizeof(ControlInfo));
    mDispH = 2000;
    bCamBOnline = false;
    mProfileData = 0;//批处理数据缓存--当前显示范围
    mIntensityData = 0;//灰度数据缓存
    mEncoderData = 0;//编码器数据缓存
    H_Up = 100;
    H_Down = -100;
    HOffset = 0;
}

SSZN_SR8060::~SSZN_SR8060() {
    close();
    delete CtrInfo;
    CtrInfo = 0;
    if (mProfileData != 0) {
        delete[] mProfileData;
        mProfileData = 0;
    }
    if (mIntensityData != 0) {
        delete[] mIntensityData;
        mIntensityData = 0;
    }
    if (mEncoderData != 0) {
        delete[] mEncoderData;
        mEncoderData = 0;
    }
}


short SSZN_SR8060::open(const char* tsPath) {
    if (tsPath == 0) {
        return -1;
    }
    HOffset = 0;
    std::vector<std::string> cps = stringSplit(tsPath, ',');
    std::vector<std::string> ip = stringSplit(cps[0], '.');

    SR7IF_ETHERNET_CONFIG SREthernetConFig;
    SREthernetConFig.abyIpAddress[0] = std::atoi(ip[0].c_str());
    SREthernetConFig.abyIpAddress[1] = std::atoi(ip[1].c_str());
    SREthernetConFig.abyIpAddress[2] = std::atoi(ip[2].c_str());
    SREthernetConFig.abyIpAddress[3] = std::atoi(ip[3].c_str());

    reT = SR7IF_EthernetOpen(Sensorinfo.ID, &SREthernetConFig);
    if (reT < 0) { return reT; }
    if (cps.size() > 1) {
        LoadCfg(cps[1].c_str());
    }
    //相机B是否在线
    reT = SR7IF_GetOnlineCameraB(Sensorinfo.ID);
    if (reT == 0)
    {
        bCamBOnline = true;
    }

    GetSensorparam();
    int MAXECODER = 1;
    int mDataC = 0;
    if (bCamBOnline)
    {
        mDataC = mDispH* 6400;
        MAXECODER = 2;
    }
    else {
        mDataC = mDispH * CtrInfo->Width;
    }

    if (mProfileData != 0) {
        delete[] mProfileData;
        mProfileData = 0;
    }
    mProfileData = new int[mDataC];

    if (mIntensityData != 0) {
        delete[] mIntensityData;
        mIntensityData = 0;
    }
    mIntensityData = new unsigned char[mDataC];

    if (mEncoderData != 0) {
        delete[] mEncoderData;
        mEncoderData = 0;
    }
    mEncoderData = new unsigned int[mDispH * MAXECODER];
    return 0;
}

void SSZN_SR8060::close() {
    bCamBOnline = false;
    //停止批处理
    reT = SR7IF_StopMeasure(Sensorinfo.ID);
    reT = SR7IF_CommClose(Sensorinfo.ID);
}


void SSZN_SR8060::stop() {
    reT = SR7IF_StopMeasure(Sensorinfo.ID);
}


bool SSZN_SR8060::grabContinuity(bool isAnsy, int gCount) {
    PCResult.clear();
    HOffset = 0;
    if (Sensorinfo.Mode == 0) {
        //非IO下的触发
        reT = SR7IF_StartMeasure(Sensorinfo.ID);
    }
    else if (Sensorinfo.Mode == 1) {
        //外部IO触发
        reT = SR7IF_StartIOTriggerMeasure(Sensorinfo.ID, gCount);
    }
    else if (Sensorinfo.Mode == 2)
    {
        //回调方式
        //reT = SR7IF_SetBatchOneTimeDataHandler(Sensorinfo.ID, BatchOneTimeCallBack);
        //reT = SR7IF_TriggerOneBatch(Sensorinfo.ID);
    }
    else if (Sensorinfo.Mode == 3) {
        //回调方式
        //reT = SR7IF_SetBatchOneTimeDataHandler(Sensorinfo.ID, BatchOneTimeCallBack);
        //int m_BatchWait = 1;//立即开始处理，=1 外部触发信号才开始处理
        //reT = SR7IF_StartMeasureWithCallback(Sensorinfo.ID, m_BatchWait);
    }
    return (reT == 0);
}


bool SSZN_SR8060::ResetConnect() {
    return true;
}

void SSZN_SR8060::GetSensorparam() {
    //批处理行数、轮廓宽度、X间距获取
    int mBatchH = SR7IF_ProfilePointSetCount(Sensorinfo.ID, NULL);
    CtrInfo->Width = SR7IF_ProfileDataWidth(Sensorinfo.ID, NULL);
    CtrInfo->vfRate = SR7IF_ProfileData_XPitch(Sensorinfo.ID, NULL);

    const char* _version = SR7IF_GetModels(Sensorinfo.ID);   //型号获取
    std::string str_Version;
    str_Version = _version;
    double m_dHeightRange = 1500;
    if (str_Version == "SR7050" || str_Version == "SR7060D")
    {
        m_dHeightRange = 3;
    }
    else if (str_Version == "SR7080")
    {
        m_dHeightRange = 9;
    }
    else if (str_Version == "SR7140")
    {
        m_dHeightRange = 15;
    }
    else if (str_Version == "SR7240")
    {
        m_dHeightRange = 24;
    }
    else if (str_Version == "SR7400")
    {
        m_dHeightRange = 60;
    }
    else if (str_Version == "SR7300")
    {
        m_dHeightRange = 150;
    }
    else if (str_Version == "SR6060")
    {
        m_dHeightRange = 15;
    }
    else if (str_Version == "SR6030")
    {
        m_dHeightRange = 8;
    }
    else if (str_Version == "SR6070")
    {
        m_dHeightRange = 16;
    }
    else if (str_Version == "SR6071")
    {
        m_dHeightRange = 40;
    }
    else if (str_Version == "SR6130")
    {
        m_dHeightRange = 100;
    }
    else if (str_Version == "SR6260")
    {
        m_dHeightRange = 240;
    }
    else if (str_Version == "SR8020")
    {
        m_dHeightRange = 6;
    }
    else if (str_Version == "SR8060")
    {
        m_dHeightRange = 20;
    }
    else if (str_Version == "SR7900")
    {
        m_dHeightRange = 450;
    }
    else if (str_Version == "SR7060")
    {
        m_dHeightRange = 6;
    }
    else if (str_Version == "SR71600")
    {
        m_dHeightRange = 1500;
    }
    H_Up = m_dHeightRange;
    H_Down = -m_dHeightRange;
}

void SSZN_SR8060::SetSensorparam() {
}

void SSZN_SR8060::resultConvert() {
    int tempH = 0;
    int index = 0;

    for (int r = 0; r < mDispH; r++) {
        for (int c = 0; c < CtrInfo->Width; c++) {
            index = r * CtrInfo->Width + c;
            tempH = mProfileData[index] * SSZN_ZRATE;
            if (tempH > H_Up) { continue; }
            if (tempH < H_Down) { continue; }
            rxsPointCouldp pcp = { (HOffset + r) * SSZN_YRATE,c * CtrInfo->vfRate,tempH };
            PCResult.push_back(pcp);
        }
    }
    HOffset += mDispH;
}


void SSZN_SR8060::resultConvert(int* pd, unsigned lineNums) {
    float tempH = 0;
    int index = 0;

    for (int r = 0; r < lineNums; r++) {
        for (int c = 0; c < CtrInfo->Width; c++) {
            index = r * CtrInfo->Width + c;
            tempH = pd[index] * SSZN_ZRATE;
            if (tempH > H_Up) { continue; }
            if (tempH < H_Down) { continue; }
            rxsPointCouldp pcp = { (HOffset + r) * SSZN_YRATE,c * CtrInfo->vfRate,tempH };
            PCResult.push_back(pcp);
        }
    }
    HOffset += lineNums;
}


int SSZN_SR8060::IsCollectionCompleted(unsigned int gCount) {
    int mEncoderN = 1;//主控一个相机A
    int mTmpH = 1000;//行数应设置为小于显示的高度mDispH
    int* TmpBatchPoint = new int[mTmpH * CtrInfo->Width];//当前批次高度数据缓存
    unsigned char* TmpGrayData = new unsigned char[mTmpH * CtrInfo->Width];//当前批次灰度数据缓存
    unsigned int* FrameLoss = new unsigned int[mTmpH * mEncoderN];//批处理过快掉帧数量数据缓存
    long long* FrameId = new long long[mTmpH];//帧编号数据缓存
    unsigned int* Encoder = new unsigned int[mTmpH * mEncoderN];//编码器数据缓存
    long long BatchPoint_CurNo = 0;     //当前批处理编号
    long long OverFlowStartId = 0;      //溢出起始帧号
    long long m_BatchPoint_CurNo = 0;   //当前总行数
    int FrameLossID = 0;                //丢帧数
    int EncoderID = 0;                  //编码器值

    bool bError = false;
    do {
        //SR7IF_SetBatchRollProfilePoint
        /* 接收数据---当前批次高度数据、灰度数据、编码器数据、帧编号、掉帧数量数据 */
        int m_curBatchPoint = SR7IF_GetBatchRollData(Sensorinfo.ID, NULL, TmpBatchPoint, TmpGrayData, Encoder, FrameId, FrameLoss, 500);
        if (m_curBatchPoint < 0)
        {
            if (m_curBatchPoint == SR7IF_ERROR_MODE)
            {
                //setTextInfo(_T("当前为非循环模式"));
                bError = true;
                break;
            }
            else if (m_curBatchPoint == SR7IF_NORMAL_STOP)
            {
                //setTextInfo(_T("外部IO或其他因素导致批处理正常停止"));
                bError = true;
                break;
            }
            else if (m_curBatchPoint == SR7IF_ERROR_ABORT) {
                bError = true;
                break;
            }
            else
            {
                int EthErrCnt, UserErrCnt;
                SR7IF_GetBatchRollError(0, &EthErrCnt, &UserErrCnt);
                if (m_curBatchPoint == SR7IF_ERROR_ROLL_DATA_OVERFLOW)
                {
                    //strTmp.Format(_T("数据获取过慢，数据覆盖.网络原因溢出量 %d, 用户原因溢出量 %d, 溢出起始帧号 %d， 丢帧数 %d  编码器值 %d"),
                    //    EthErrCnt, UserErrCnt, OverFlowStartId, FrameLossID, EncoderID);
                }
                else if (m_curBatchPoint == SR7IF_ERROR_ROLL_BUSY)
                {
                    //stmp = _T("busy");
                }
                continue;
            }
        }
        if (m_curBatchPoint == 0)
            continue;


        resultConvert(TmpBatchPoint, m_curBatchPoint);

        ////上一次最后一行对应的帧等信息
        //int TmpID = m_curBatchPoint - 1;
        //OverFlowStartId = FrameId[TmpID];
        //FrameLossID = FrameLoss[TmpID];
        //EncoderID = Encoder[TmpID];

        ////数据拷贝显示
        //int TmpN = m_curBatchPoint * CtrInfo->Width;
        //if (BatchPoint_CurNo < mDispH)
        //{
        //    if (BatchPoint_CurNo + m_curBatchPoint > mDispH)
        //    {
        //        int TMPP = int(CtrInfo->Width * (mDispH - BatchPoint_CurNo));

        //        /* 高度 */
        //        memcpy(&mProfileData[BatchPoint_CurNo * CtrInfo->Width], TmpBatchPoint, sizeof(int) * TMPP);

        //        ///* 灰度 */
        //        //memcpy(&mIntensityData[BatchPoint_CurNo * CtrInfo->Width], TmpGrayData, TMPP);

        //        ///* 编码器 */
        //        //memcpy(&mEncoderData[BatchPoint_CurNo * mEncoderN], Encoder,sizeof(unsigned int) * (mDispH - BatchPoint_CurNo) * mEncoderN);

        //        //显示 mDispH - BatchPoint_CurNo 行数据
        //        resultConvert();


        //        //多出的行数重新排列
        //        int TMPPT = int(BatchPoint_CurNo + m_curBatchPoint - mDispH);
        //        int TTTT = TMPPT * CtrInfo->Width;
        //        int Tmpx = (mDispH - TMPPT) * CtrInfo->Width;

        //        /* 高度 */
        //        memcpy(&mProfileData[0], &mProfileData[TTTT], sizeof(int) * Tmpx);
        //        memcpy(&mProfileData[Tmpx], &TmpBatchPoint[TMPP], sizeof(int) * TTTT);

        //        ///* 灰度 */
        //        //memcpy(&mIntensityData[0], &mIntensityData[TTTT], Tmpx);
        //        //memcpy(&mIntensityData[Tmpx], &TmpGrayData[TMPP], TTTT);

        //        ///* 编码器 */
        //        //memcpy(&mEncoderData[0], &mEncoderData[TMPPT * mEncoderN],(mDispH - TMPPT) * mEncoderN * sizeof(unsigned int));
        //        //memcpy(&mEncoderData[(mDispH - TMPPT) * mEncoderN],&Encoder[(mDispH - BatchPoint_CurNo) * mEncoderN],TMPPT * mEncoderN * sizeof(unsigned int));

        //        resultConvert();

        //        BatchPoint_CurNo += m_curBatchPoint;
        //        m_BatchPoint_CurNo = BatchPoint_CurNo;

        //        //显示当前接收的总行数
        //        //strTotalC.Format(_T("当前总行数:%d"), m_BatchPoint_CurNo);
        //        continue;
        //    }

        //    /* 高度 */
        //    memcpy(&mProfileData[BatchPoint_CurNo * CtrInfo->Width], TmpBatchPoint, sizeof(int) * TmpN);
        //    ///* 灰度 */
        //    //memcpy(&mIntensityData[BatchPoint_CurNo * CtrInfo->Width], TmpGrayData, TmpN);
        //    ///* 编码器 */
        //    //memcpy(&mEncoderData[BatchPoint_CurNo * mEncoderN], Encoder,m_curBatchPoint * mEncoderN * sizeof(unsigned int));
        //}
        //else
        //{
        //    /* 重新排列 */
        //    int TTTT = (mDispH - m_curBatchPoint) * CtrInfo->Width;
        //    memcpy(&mProfileData[0], &mProfileData[TmpN], sizeof(int) * TTTT);
        //    memcpy(&mProfileData[TTTT], TmpBatchPoint, sizeof(int) * TmpN);

        //    ///* 灰度 */
        //    //memcpy(&mIntensityData[0], &mIntensityData[TmpN], TTTT);
        //    //memcpy(&mIntensityData[TTTT], TmpGrayData, TmpN);

        //    ///* 编码器 */
        //    //memcpy(&mEncoderData[0], &mEncoderData[m_curBatchPoint * mEncoderN], sizeof(unsigned int) * (mDispH - m_curBatchPoint));
        //    //memcpy(&mIntensityData[(mDispH - m_curBatchPoint) * mEncoderN], Encoder, sizeof(unsigned int) * m_curBatchPoint * mEncoderN);
        //}

        //resultConvert();

        //BatchPoint_CurNo += m_curBatchPoint;
        //m_BatchPoint_CurNo = BatchPoint_CurNo;

        //显示当前接收的总行数
        //strTotalC.Format(_T("当前总行数:%d"), m_BatchPoint_CurNo);

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    } while (true);

    if (bError)  //因错误导致循环退出
    {
        SR7IF_StopMeasure(Sensorinfo.ID);
    }
    if (TmpBatchPoint)
    {
        delete[] TmpBatchPoint;
        TmpBatchPoint = NULL;
    }
    if (TmpGrayData)
    {
        delete[] TmpGrayData;
        TmpGrayData = NULL;
    }
    if (Encoder)
    {
        delete[] Encoder;
        Encoder = NULL;
    }
    if (FrameLoss)
    {
        delete[] FrameLoss;
        FrameLoss = NULL;
    }
    if (FrameId)
    {
        delete[] FrameId;
        FrameId = NULL;
    }
    return PCResult.size();
}

void* SSZN_SR8060::DataGet(unsigned& outcount) {
    outcount = PCResult.size();
    if (outcount > 0) {
        return &PCResult[0];
    }
    return 0;
}

rxsPointCouldp* SSZN_SR8060::DataGetPointCould(unsigned& num) {
    num = PCResult.size();
    if (num > 0) {
        return &PCResult[0];
    }
    return 0;
}

bool SSZN_SR8060::TiggerTaber(short wich, bool isClose) {
    return true;
}

bool SSZN_SR8060::expActive(short fno, void* param) {
    return true;
}

void SSZN_SR8060::CoordinateConvert() {

}

/// <summary>
/// wich 最高位 bit7 指出 是否终点,其他位指示是哪一个轴
/// </summary>
bool SSZN_SR8060::WorldCoordinateSet(u8 wich, float x, float y, float z) {
    return true;
}

void SSZN_SR8060::LoadCfg(const char* path)
{
    if (path == 0) {
        return;
    }
    std::string content;
    std::ifstream file(path);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            content += line;
        }
        file.close();
    }
    if (content.length() < 1) { return; }
    reT = SR7IF_LoadParameters(Sensorinfo.ID, content.c_str(), content.length());
}


//void BatchOneTimeCallBack(const void* info, const SR7IF_Data* data) {
//    const SR7IF_STR_CALLBACK_INFO* conInfo = (const SR7IF_STR_CALLBACK_INFO*)info;
//    if (conInfo->returnStatus != SR7IF_OK)
//    {
//        return;
//    }
//    if (conInfo->BatchPoints != 0)
//    {
//        //DataCallModel->mBatchTimes++;
//    }
//    //高度数据获取--相机A
//    const int* mTmpData = SR7IF_GetBatchProfilePoint(data, 0);
//    int mNumP = conInfo->BatchPoints * conInfo->xPoints;
//    if (mTmpData != NULL)
//    {
//        memset(catchPoints, -1000000000, mNumP);
//        memcpy(catchPoints, &mTmpData[0], mNumP * sizeof(int));
//        pcResult.push_back(catchPoints);
//    }
//
//    //灰度数据获取--相机A
//    int mNumG = sizeof(unsigned char) * conInfo->BatchPoints * conInfo->xPoints;
//    const unsigned char* mTmpGraydata = SR7IF_GetBatchIntensityPoint(data, 0);
//    if (mTmpGraydata != NULL)
//    {
//        //memset(&DataCallModel->mIntensityData[0][0], 0, mNumG);
//        //memcpy(&DataCallModel->mIntensityData[0][0], &mTmpGraydata[0], mNumG);
//    }
//
//    //编码器数据获取--相机A
//    int mNumE = sizeof(unsigned int) * conInfo->BatchPoints;
//    const unsigned int* mTmpEncoderdata = SR7IF_GetBatchEncoderPoint(data, 0);
//    if (mTmpEncoderdata != NULL)
//    {
//        //memset(&DataCallModel->mEncoderData[0][0], 0, mNumE);
//        //memcpy(&DataCallModel->mEncoderData[0][0], &mTmpEncoderdata[0], mNumE);
//    }
//}