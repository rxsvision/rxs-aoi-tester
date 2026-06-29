#include "rxsHrdwdal.h"
#include "stdio.h"

#include <locale.h>
#include <iostream>
#include <string>

typedef void(*CBCaptureComplete)(int capturedCount);//老版 LEOldFuncation.h 中使用

#include "../LightE/inc/LEDataType.h"
#include "../LightE/inc/LEOldFuncation.h"
#include "../LightE/inc/LEApp.h"
#include "../LightE/inc/ConfocalDLL.h"
#include "../LightE/inc/LEReturnValue.h"


#ifdef _MYX86
#pragma comment(lib,"..\\LightE\\x86\\ConfocalDLL.lib")
#else
#pragma comment(lib,"..\\LightE\\x64\\ConfocalDLL_x64.lib")
#endif


#pragma warning(disable : 4996)
#define RXSAPI			extern "C" __declspec(dllexport)

using namespace std;

LightE_AOCC::LightE_AOCC() {
	//0 - PC版USB 3.0接口1代,1 - PC版以太网接口,2 - PC版USB 3.0接口2代,3 - 嵌入式控制器E系列,4 - 嵌入式控制器G系列
	int m_iDeviceType = 4;
	CtrInfo = 0;
	ctrCount = 0;
	errCode = 0;
	sctr = 0;
	slectedChannelno = 0;
	Sensorinfo.devType = ImageableSensorUnit::DevType::Confocal;
	LE_SelectDeviceType(m_iDeviceType);
	CtrInit();
}

void LightE_AOCC::CtrInit() {
	errCode = LE_InitDLL();
	if (errCode == RT_SUCCESS)
	{
		//获取已连接的控制器数量
		ctrCount = LE_GetSpecCount();
		if (ctrCount > 0) {
			CtrInfo = (ControlInfo*)malloc(sizeof(ControlInfo) * ctrCount);
			if (CtrInfo != 0)
				memset(CtrInfo, 0, sizeof(ControlInfo) * ctrCount);
		}
	}
}

wstring s2ws(const string& s)
{
	//curLocale="C"
	string curLocale = setlocale(LC_ALL, NULL);
	setlocale(LC_ALL, "chs");
	const char* source = s.c_str();
	size_t charNum = s.size() + 1;
	//cout << "s.size():" << s.size() << endl;//7：多字节字符串"ABC我们"有7个字节

	wchar_t* dest = new wchar_t[charNum];
	mbstowcs_s(NULL, dest, charNum, source, _TRUNCATE);
	wstring result = dest;
	delete[] dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}

short LightE_AOCC::open(const char* tsPath) {
	if (errCode == RT_SUCCESS && ctrCount > 0) {
		std::string fp = tsPath;
		int x = fp.find(',');
		if (x < 0) { return 110; }
		//string fp1 = fp.substr(0, x);
		//string fp2 = fp.substr((x + 1), fp.length() - x - 1);
		//wstring dcfgFile = s2ws(fp1.c_str());
		//wstring hwcFile = s2ws(fp2.c_str());
		wchar_t m_pSpecSN[32];
		wchar_t pName[32];
		wchar_t pFMVersion[32];
		if (!LE_GetSpecMsg(m_pSpecSN, pName, pFMVersion, ctrCount)) { return 111; }
		//打开指定序列号控制器
		errCode = LE_Open(&m_pSpecSN[0], CtrInfo[0].devID);
		if (errCode != RT_SUCCESS) { return errCode; }

		//通常调试模式下心跳时间设置大一些，防止调试的过程中与控制器连接中断，release模式一般心跳时间设置为3s
#ifdef _DEBUG
		LE_SetHeartTime(30000, CtrInfo[0].devID);
#else
		LE_SetHeartTime(3000, CtrInfo[0].devID);
#endif
		//errCode = LE_LoadDeviceConfigureFromFile((wchar_t*)dcfgFile.c_str(), CtrInfo[0].devID);
		//if (errCode != RT_SUCCESS) { return errCode; }
		CtrInfo[0].ChannelNum = LE_GetChannels(CtrInfo[0].devID);
		//通过文件载入通道高度校准数据
		//errCode = LE_LoadLWCalibrationData((wchar_t*)hwcFile.c_str(), CtrInfo[0].devID);
		//if (errCode != RT_SUCCESS) { return errCode; }
		//Selected(0, 1);
		CtrInfo[0].CaptureMode = 1;
		errCode = LE_SetCaptureDataMode(CtrInfo[0].CaptureMode, CtrInfo[0].devID);
		GetSensorparam();
		return 0;
	}
	return errCode;
}

bool LightE_AOCC::ResetConnect() {
	close();
	CtrInit();
	std::string tp;
	Open(tp.c_str());
	return true;
}

void LightE_AOCC::GetSensorparam() {
	int tkCount = 0;
	errCode = LE_GetExposureTime(CtrInfo[sctr].expTime, CtrInfo[sctr].devID);
	errCode = LE_GetPosOffset(CtrInfo[sctr].offsetLine, CtrInfo[sctr].lines, CtrInfo[sctr].devID, slectedChannelno);
	errCode = LE_GetGain(CtrInfo[sctr].gain, CtrInfo[sctr].devID);
	errCode = LE_GetSampleFrequency(CtrInfo[sctr].sampleFreq, CtrInfo[sctr].devID);
	errCode = LE_GetMeasureType(CtrInfo[sctr].MeasureType, tkCount, CtrInfo[sctr].devID);
	errCode = LE_GetTriggerMode(CtrInfo[sctr].TiggerMode, CtrInfo[sctr].devID);
	errCode = LE_GetCaptureDataMode(CtrInfo[sctr].CaptureMode, CtrInfo[sctr].devID);
	if (CtrInfo[sctr].TiggerMode == 4) {
		u8 sts = 0;
		u8 wich = 0;
		errCode = LE_FQ_GetOutStatusAndAxis(sts, wich, CtrInfo[sctr].devID);
		CtrInfo[sctr].TiggerMode = 4 + wich;
	}
}

void LightE_AOCC::SetSensorparam() {
	errCode = LE_SetSampleFrequency(CtrInfo[sctr].sampleFreq, CtrInfo[sctr].devID);
	errCode = LE_SetExposureTime(CtrInfo[sctr].expTime, CtrInfo[sctr].devID);
	errCode = LE_SetGain(CtrInfo[sctr].gain, CtrInfo[sctr].devID);
	errCode = LE_SetCaptureDataMode(CtrInfo[sctr].CaptureMode, CtrInfo[sctr].devID);
	errCode = LE_SetMeasureType(CtrInfo[sctr].MeasureType, 1, CtrInfo[sctr].devID, slectedChannelno);
}

void LightE_AOCC::close() {
	//关闭控制器
	for (unsigned x = 0; x < ctrCount; x++) {
		LE_Close(CtrInfo[x].devID);
	}
	//反初始化控制器DLL，该步骤很重要，不做可能会引起内存错误
	LE_UnInitDLL();
	if (CtrInfo != 0) {
		free(CtrInfo);
		CtrInfo = 0;
	}
	ctrCount = 0;
}

bool LightE_AOCC::GrabTigger(unsigned int gCount) {

	return true;
}

bool LightE_AOCC::GrabTiggerContinuity(unsigned int gCount) {
	Sensorinfo.Mode = 1;
	if (CtrInfo[sctr].CaptureMode == 1) {
		//实时缓存模式采集
		errCode = LE_StartStoreDataToBuffer(CtrInfo[sctr].devID);
		return (errCode == RT_SUCCESS);
	}
	else if (CtrInfo[sctr].CaptureMode == 0) {
		//采集任务模式采集

		return true;
	}
	else {
		errCode = LE_StartGetAllValues((double*)Sensorinfo.ptrGrabResult, Sensorinfo.BuffSize, CtrInfo[sctr].devID);
		if (1 == errCode)
		{
			_sleep(5);//采集任务开启后，延时5ms再去查询设备状态
			bool bFinished = false;
			do
			{
				errCode = LE_GetDeviceStatus(CtrInfo[sctr].devID);
				if (1 != errCode)
					bFinished = true;
				_sleep(5);
			} while (!bFinished);
			if (0 == errCode)
				return true;
		}
	}
	return false;
}

int LightE_AOCC::IsCollectionCompleted(unsigned int gCount) {
	int cnt = 0;
	int curCount = 0;
	int outcount = 0;
	do
	{
		errCode = LE_GetCapturedPoints(curCount, CtrInfo[sctr].devID, slectedChannelno);
		++cnt;
		if (!isGrabing()) {//主动停止采集
			break;
		}
		////采集超时判断，如果触发模式为外触发模式则采集完成时间可能不确定，
		//// 请按实际情况来决定是否需要去除此判断。
		//if (cnt > gCount * 2)
		//{
		//	//超时跳出等待循环前先主动停止控制器采集任务。
		//	LE_StopGetPoints(CtrInfo[sctr].devID, slectedChannelno);
		//	Sensorinfo.Mode = 0;
		//	return -1;
		//}
		_sleep(10);
	} while (curCount < gCount);
	errCode = LE_StopGetPoints(CtrInfo[sctr].devID, slectedChannelno);
	errCode = LE_GetCapturedPoints(curCount, CtrInfo[sctr].devID, slectedChannelno);
	Sensorinfo.Mode = 0;
	if (curCount >= gCount)
		outcount = gCount;
	else
		if (curCount > 0 && curCount < gCount)
			outcount = curCount;

	return outcount;
}

void LightE_AOCC::ThinknessGet(unsigned outcount) {
	BuffClr();
	allocBuff(outcount, 3);
	//只获取单层厚度结果数据。
	errCode = LE_GetStoredThicknesses((double*)Sensorinfo.ptrGrabResult, Sensorinfo.BuffSize, 1, CtrInfo[sctr].devID);
}

void LightE_AOCC::HeightDataGet(unsigned outcount) {
	BuffClr();
	allocBuff(outcount, 3);
	//只获取高度结果数据。
	int getedCount = LE_GetStoredDistances((double*)Sensorinfo.ptrGrabResult, Sensorinfo.BuffSize, CtrInfo[sctr].devID);
}


void LightE_AOCC::stop() {
	if (Sensorinfo.Mode == 1) {
		Sensorinfo.Mode = 0;
	}
}

void* LightE_AOCC::DataGet(unsigned &outcount) {
	if (CtrInfo[sctr].MeasureType == 0) {
		HeightDataGet(outcount);
	}
	else if (CtrInfo[sctr].MeasureType == 1) {
		ThinknessGet(outcount);
	}
	return (double*)Sensorinfo.ptrGrabResult;
}

bool LightE_AOCC::TiggerTaber(short wich, bool isClose) {
	if (isClose) {
		if (CtrInfo[sctr].TiggerMode > 3) {
			errCode = (LE_FQ_SetOutStatusAndAxis(0, wich, CtrInfo[sctr].devID) == RT_SUCCESS) ? 1 : 0;
			return true;
		}
		return false;
	}
	if (wich == 0) {//连续触发
		errCode = LE_SetTriggerMode(0, CtrInfo[sctr].devID);
		if (errCode == 1) {
			CtrInfo[sctr].TiggerMode = 0;
			return true;
		}
		return false;
	}
	if (wich == 1) {//外部触发
		errCode = LE_SetTriggerMode(3, CtrInfo[sctr].devID);
		if (errCode == 1) {
			CtrInfo[sctr].TiggerMode = 3;
			return true;
		}
		return false;
	}
	//编码器触发
	errCode = LE_SetTriggerMode(4, CtrInfo[sctr].devID);
	if (errCode != 1) {
		return false;
	}
	errCode = 0;
	wich = wich - 2;//对齐到对应编码器
	CtrInfo[sctr].TiggerMode = 4 + wich;
	errCode = (LE_FQ_SetOutStatusAndAxis(1, wich, CtrInfo[sctr].devID) == RT_SUCCESS) ? 1 : 0;
	return (errCode == 1);
}

bool LightE_AOCC::expActive(short fno, void* param) {
	switch (fno)
	{
	case 1:
		return Dark();
		break;
	case 2:
		//OrgAndRange();
		break;
	default:
		break;
	}
	return false;
}

void LightE_AOCC::CoordinateConvert() {}
bool LightE_AOCC::WorldCoordinateSet(u8 wich, float x, float y, float z) { return true; }


bool LightE_AOCC::Dark() {
	errCode = LE_CaptureChannelsIntensityCaliData(CtrInfo[sctr].devID);

	return (errCode == 1);
}

bool LightE_AOCC::OrgAndRange(float zVal, float upVal, float downVal) {
	errCode = LE_SetDistanceZeroValue(zVal, CtrInfo[sctr].devID, slectedChannelno);
	if (upVal != 0.0f && downVal != 0.0f)
		errCode = LE_SetDistanceRangeLimit(upVal, downVal, CtrInfo[sctr].devID, slectedChannelno);


	return (errCode == 1);
}

bool LightE_AOCC::grabContinuity(bool isAnsy, int gCount) {
	return GrabTiggerContinuity(10000);
}
