#pragma once
#include "rxsHrdwdal.h"
#include "proteced.h"
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <atomic>
#include <mutex>
#include <memory>
#include <thread>
#include <vector>
#include <cmath>
#include <atomic>

//通过 附加包含的 引用方式应用了 头文件
#include "CHRocodileLib.h"
#include "CHRocodileLibErrorDef.h"
#include "CHRocodileLibPluginDef.h"
#include "CHRocodileLibSpecialFunc.h"

#ifdef _MYX86
#pragma comment(lib,"../Precitec/win/Win32/CHRocodile.lib")
#else
#pragma comment(lib,"../Precitec/win/x64/CHRocodile.lib")
#endif

#pragma warning(disable : 4996)

//#define SAMPLERATE			4000.0f
//#define SAMPLERATE			70000.0f
#define PZ_CONVERT			-1000.0f


using namespace std;

Conn_h hCHR;
std::vector<rxsPointCouldp> fr;
std::vector<double> aSamples;


Precitec::Precitec() {
	Sensorinfo.devType = DevType::Confocal;
	CtrInfo = new ControlInfo;
	sp = { 0.0f,0.0f };
	ep = { 0.0f,0.0f };
	dir = 0;
	//编码器触发间隔10
	CtrInfo[0].lines = 10;

#ifdef _WIN32
	SetLibLogFileDirectory(L"./SysLog/", 500, 50);
#else
	SetLibLogFileDirectory(".", 500, 50);
#endif
	InsertGetParams("采样率", 0);
}

Precitec::~Precitec() {
	Sensorinfo.ptrGrabResult = 0;
	Sensorinfo.BuffSize = 0;
	CloseConnection(hCHR);
	if (CtrInfo != 0) {
		delete CtrInfo;
		CtrInfo = 0;
	}
}


bool Precitec::ResetConnect() {
	Stop();
	close();
	if (Sensorinfo.DevConnMethod.earth.Port == 0 || Sensorinfo.DevConnMethod.earth.Ip[0] == '0') {
		return false;
	}
	string ns = Sensorinfo.DevConnMethod.earth.Ip;
	ns += ',';
	char iport[6];
	sprintf_s(iport, "%d", Sensorinfo.DevConnMethod.earth.Port);
	ns += iport;
	return (Open((char*)ns.c_str()) == 0);
}

short Precitec::open(const char* tsPath) {
	if (tsPath == 0) {
		return -1;
	}
	//当作时采集时候的起始偏移量与总点数偏移量，根据实际的丢脉冲数情况设置
	CtrInfo[0].offsetLine = 5;
	std::string fp = tsPath;
	std::vector<std::string> spp = stringSplit(fp, ',');
	int pl = spp.size();
	if (pl < 1) { return 110; }
	string strConnectionInfo = spp[0];
	Sensorinfo.DevConnMethod.earth.Port = stoi(spp[1].c_str());
	memcpy(Sensorinfo.DevConnMethod.earth.Ip, strConnectionInfo.c_str(), strConnectionInfo.length());
	if (Sensorinfo.DevConnMethod.earth.Port == 0 || Sensorinfo.DevConnMethod.earth.Ip[0] == '0') { return -3; }
	//errCode = OpenConnection(strConnectionInfo.c_str(), CHR_2_Device, Connection_Synchronous, 0, &hCHR);
	int32_t dt = CHR_2_Device;//CHR_Compact_Device
	if (pl > 2) {
		dt = stoi(spp[2].c_str());
	}
	errCode = OpenConnection(strConnectionInfo.c_str(), dt, Connection_Synchronous, 0, &hCHR);
	if (!CHR_SUCCESS(errCode))
	{
		return errCode;
	}
	Cmd_h hCmd;
	//测量模式
	NewCommand(CmdID_Measuring_Method, false, &hCmd);
	AddCommandIntArg(hCmd, 0);
	errCode = ExecCommand(hCHR, hCmd, nullptr);
	if (!CHR_SUCCESS(errCode)) {
		std::cout << "Error in setting measuring mode: " << errCode << std::endl;
	}
	// set to only detect one peaks
	NewCommand(CmdID_Number_Of_Peaks, false, &hCmd);
	AddCommandIntArg(hCmd, 1);
	errCode = ExecCommand(hCHR, hCmd, nullptr);
	if (!CHR_SUCCESS(errCode)) {
		std::cout << "Error in setting number of peaks: " << errCode << std::endl;
	}
	// Select optical probe
	int32_t probe = 0;
	if (pl > 3) {
		probe = stoi(spp[3].c_str());
	}
	NewCommand(CmdID_Optical_Probe, false, &hCmd);
	AddCommandIntArg(hCmd, probe);
	errCode = ExecCommand(hCHR, hCmd, nullptr);
	if (!CHR_SUCCESS(errCode)) {
		std::cout << "Error in setting optical probe: " << errCode << std::endl;
	}
	float samphz = 4000.0f;
	if (pl > 4) {
		samphz = atof(spp[4].c_str());
	}
	NewCommand(CmdID_Scan_Rate, false, &hCmd);
	AddCommandFloatArg(hCmd, samphz);
	errCode = ExecCommand(hCHR, hCmd, nullptr);
	if (!CHR_SUCCESS(errCode)) {
		std::cout << "Error in setting scan rate: " << errCode << std::endl;
	}

	//设置输出信号需要的内容
	int an[] = { 83,65, 256, 257 };
	//int an[] = { 83,65, 260, 257 };//260 中值滤波
	NewCommand(CmdID_Output_Signals, false, &hCmd);
	AddCommandIntArrayArg(hCmd, an, 4);
	errCode = ExecCommand(hCHR, hCmd, nullptr);
	if (!CHR_SUCCESS(errCode)) {
		std::cout << "Error in setting output signals: " << errCode << std::endl;
	}
	stop();
	errCode = 0;
	return 0;
}

bool Precitec::grabContinuity(bool isAnsy, int gCount) {
	fr.clear();
	aSamples.clear();
	errCode = FlushConnectionBuffer(hCHR);
	if (!CHR_SUCCESS(errCode)) { std::cout << "FlushConnectionBuffer()->" << errCode << std::endl; }
	LibSize_t nSize = 0;
	LibSize_t nSampleCount = 0;
	if (dir == 1) {
		nSampleCount = (ep.x - sp.x) * 1000 / CtrInfo[0].lines;
	}
	else if (dir == 2) {
		nSampleCount = (ep.y - sp.y) * 1000  / CtrInfo[0].lines;
	}
	else {
		return false;
	}
	if (nSampleCount < 0) {
		nSampleCount = 0 - nSampleCount;
	}
	nSampleCount -= ((nSampleCount / 10000.0f) * 30);
	Cmd_h hCmd;
	NewCommand(CmdID_Start_Data_Stream, false, &hCmd);
	errCode = ExecCommand(hCHR, hCmd, nullptr);
	if (!CHR_SUCCESS(errCode)) {
		std::cout << "Error in starting data stream: " << errCode << std::endl;
	}
	//获取最小的buff 大小
	errCode = ActivateAutoBufferMode(hCHR, nullptr, nSampleCount, &nSize);
	if (!CHR_SUCCESS(errCode)) { std::cout << "1.ActivateAutoBufferMode()->" << errCode << std::endl; }
	aSamples.resize(size_t(nSize) / sizeof(double));
	double* pData = aSamples.data();
	errCode = ActivateAutoBufferMode(hCHR, pData, nSampleCount, nullptr);
	if (!CHR_SUCCESS(errCode)) { std::cout << "2.ActivateAutoBufferMode()->" << errCode << std::endl; }
	return true;
}

void Precitec::GetSensorparam() {
	Cmd_h hCmd;
	Rsp_h hRsp;
	// download spectrum from CHRocodile device , spectrum type: 0, raw spectrum; 1, processed spectrum in confocal mode; 2, FT spectrum;
	const int8_t* pTemp = nullptr;
	int32_t nLength = 0;
	NewCommand(CmdID_Download_Spectrum, false, &hCmd);
	AddCommandIntArg(hCmd, Spectrum_Confocal);
	auto nRes = ExecCommand(hCHR, hCmd, &hRsp);
	// spectrum download response, parameters:
	// the first: spectrum type, second: start channel index, third: channel count
	// fourth: exposure number, fifth: micromters per bin (interferometric mode),
	// sixth: Block exponent of spectrum data, seventh: spectrum data
	if (CHR_SUCCESS(nRes))
		nRes = GetResponseBlobArg(hRsp, 6, &pTemp, &nLength);
	if (!CHR_SUCCESS(nRes))
		std::cout << "Error in spectrum downloading!" << std::endl;
	else
	{
		// Spectrum is in short format
		int32_t nSpecLength = nLength / 2;
		std::cout << "Receive spectrum length: " << std::to_string(nSpecLength) << std::endl;
	}
}

void Precitec::SetSensorparam() {
	Cmd_h hCmd;


	//Set Averaging to 1 (data written to file) :
	NewCommand(CmdID_Data_Average, false, &hCmd);
	AddCommandIntArg(hCmd, 1);
	errCode = ExecCommand(hCHR, hCmd, nullptr);
	if (!CHR_SUCCESS(errCode)) {
		std::cout << "Error in setting data average: " << errCode << std::endl;
	}
}

void Precitec::close() {
	if (HasOpen()) {
		DeactivateAutoBufferMode(hCHR);
	}
	Sensorinfo.ptrGrabResult = 0;
	Sensorinfo.BuffSize = 0;
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	CloseConnection(hCHR);
}

int Precitec::IsCollectionCompleted(unsigned int gCount) {
	rxsPoint spr = sp;
	u8 rdir = dir;
	//while (isGrabing() && ((errCode = GetAutoBufferStatus(hCHR)) == Auto_Buffer_Saving))
	while ((errCode = GetAutoBufferStatus(hCHR)) == Auto_Buffer_Saving)
	{
		_sleep(30);
	}
	int64_t nSampleCountRead = 0;
	std::cout << "Auto buffer save status: " << GetAutoBufferStatus(hCHR) << std::endl;
	errCode = GetAutoBufferSavedSampleCount(hCHR, &nSampleCountRead);
	if (nSampleCountRead < 1) { return -1; }
	if (CHR_SUCCESS(errCode))
	{
		double* pData = aSamples.data();
		if (rdir == 1) {
			for (unsigned x = 0; x < aSamples.size();) {
				//pData[x];// sample counter output by device
				x += 1;
				//pData[x];// axis encoder counter
				x += 1;// distance in micrometer
				//rxsPointCouldp rxsp = { spr.x + (x * CtrInfo[0].lines),spr.y,pData[x] };
				rxsPointCouldp rxsp = { spr.x ,spr.y,(pData[x] == 0.0f) ? -99.9f : pData[x] / PZ_CONVERT };
				fr.push_back(rxsp);
				x += 1;
				//pData[x];// intensity in percent
				x += 1;
			}
		}
		else if (rdir == 2) {
			for (unsigned x = 0; x < aSamples.size();) {
				//pData[x];// sample counter output by device
				x += 1;
				//pData[x];// axis encoder counter
				x += 1;// distance in micrometer
				//rxsPointCouldp rxsp = { spr.x,spr.y + (x * CtrInfo[0].lines),pData[x] };
				rxsPointCouldp rxsp = { spr.x,spr.y ,(pData[x] == 0.0f) ? -99.9f : pData[x] / PZ_CONVERT };
				fr.push_back(rxsp);
				x += 1;
				//pData[x];// intensity in percent
				x += 1;
			}
		}
	}
	return fr.size();
}

void* Precitec::DataGet(unsigned& outcount) {
	Sensorinfo.BuffSize = fr.size();
	Sensorinfo.ptrGrabResult = fr.data();
	return Sensorinfo.ptrGrabResult;
}

void Precitec::stop() {
	Cmd_h hCmd;
	int nEnable = 0;
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	errCode = DeactivateAutoBufferMode(hCHR);
	NewCommand(CmdID_Stop_Data_Stream, false, &hCmd);
	errCode = ExecCommand(hCHR, hCmd, nullptr);
	if (!CHR_SUCCESS(errCode)) {
		std::cout << "Error in starting data stream: " << errCode << std::endl;
	}
	NewCommand(CmdID_Encoder_Trigger_Enabled, false, &hCmd);
	AddCommandIntArg(hCmd, nEnable);
	errCode = ExecCommand(hCHR, hCmd, nullptr);
	if (!CHR_SUCCESS(errCode)) {
		std::cout << "Error in enabling encoder trigger: " << errCode << std::endl;
	}
	memset(&sp, 0, sizeof(rxsPoint));
	memset(&ep, 0, sizeof(rxsPoint));
	dir = 0;
	NewCommand(CmdID_Device_Trigger_Mode, false, &hCmd);
	AddCommandIntArg(hCmd, Device_Trigger_Mode_Free_Run);
	errCode = ExecCommand(hCHR, hCmd, nullptr);

}

bool Precitec::TiggerTaber(short wich, bool isClose) {
	Cmd_h hCmd;
	if (GetAutoBufferStatus(hCHR) == Auto_Buffer_Saving) {
		DeactivateAutoBufferMode(hCHR);
	}

	if (wich == 0) {
		//设置设备返回到自由运行模式
		NewCommand(CmdID_Device_Trigger_Mode, false, &hCmd);
		AddCommandIntArg(hCmd, Device_Trigger_Mode_Free_Run);
		errCode = ExecCommand(hCHR, hCmd, nullptr);
		return CHR_SUCCESS(errCode);
	}

	if (wich == 1) {//外部触发
		return false;
	}

	if (wich > 2 && wich < 5) {
		//其他---编码器其触发
		int nStartPos = 0;
		int nStopPos = 0;
		int nAxis = 0;
		if (dir == 1) {
			nStartPos = sp.x * 1000;
			nStopPos = ep.x * 1000;
			nAxis = 0;
		}
		else if (dir == 2) {
			nStartPos = sp.y * 1000;
			nStopPos = ep.y * 1000;
			nAxis = 1;
		}
		else {
			nAxis = wich - 3;
		}
		//清零编码器计数
		int nIniPos = 0;
		NewCommand(CmdID_Encoder_Counter, false, &hCmd);
		AddCommandIntArg(hCmd, nAxis);
		AddCommandIntArg(hCmd, nIniPos);
		errCode = ExecCommand(hCHR, hCmd, nullptr);
		if (!CHR_SUCCESS(errCode)) {
			std::cout << "Error in setting encoder counter: " << errCode << std::endl;
			return false;
		}
		//使能编码器触发
		int nEnable = 1;
		NewCommand(CmdID_Encoder_Trigger_Enabled, false, &hCmd);
		AddCommandIntArg(hCmd, nEnable);
		errCode = ExecCommand(hCHR, hCmd, nullptr);
		if (!CHR_SUCCESS(errCode)) {
			std::cout << "Error in enabling encoder trigger: " << errCode << std::endl;
			return false;
		}

		int l = nStopPos - nStartPos;
		float nInterval = CtrInfo[0].lines;
		int nReturnTrigger = 1;
		if (nStopPos > 0) {
			nStartPos = CtrInfo[0].offsetLine;
		}
		else {
			nStartPos = -CtrInfo[0].offsetLine;
		}
		if (l > 0) {
			nStopPos = nStartPos + l;

		}
		else {
			nStopPos = nStartPos - l;
			nInterval = -nInterval;
		}

		NewCommand(CmdID_Encoder_Trigger_Property, false, &hCmd);
		AddCommandIntArg(hCmd, nAxis);
		AddCommandIntArg(hCmd, nStartPos);
		AddCommandIntArg(hCmd, nStopPos);
		AddCommandFloatArg(hCmd, nInterval);
		AddCommandIntArg(hCmd, nReturnTrigger);
		errCode = ExecCommand(hCHR, hCmd, nullptr);
		if (!CHR_SUCCESS(errCode)) {
			std::cout << "Error in setting trigger Property: " << errCode << std::endl;
			return false;
		}
		//set device to "trigger each" mode
		NewCommand(CmdID_Device_Trigger_Mode, false, &hCmd);
		AddCommandIntArg(hCmd, Device_Trigger_Mode_Trigger_Each);
		errCode = ExecCommand(hCHR, hCmd, nullptr);
		return CHR_SUCCESS(errCode);
	}
	return false;
}

bool Precitec::expActive(short fno, void* param) {
	return true;
}

void Precitec::CoordinateConvert() {

}

bool Precitec::WorldCoordinateSet(u8 wich, float x, float y, float z) {
	bool isEnd = ((wich & 128) == 128);
	wich = (wich << 1);
	wich = (wich >> 1);
	if (wich == 0) {//x-轴
		dir = 1;
		if (isEnd) {
			ep.x = x;
		}
		else {
			sp.x = x;
			sp.y = y;
		}
	}
	else if (wich == 1) {//y-轴
		dir = 2;
		if (isEnd) {
			ep.y = y;
		}
		else {
			sp.x = x;
			sp.y = y;
		}
	}
	else {
		return false;
	}
	return true;
}