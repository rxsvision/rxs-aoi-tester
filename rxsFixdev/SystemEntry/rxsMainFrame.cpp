// rxsMainFrame.cpp : 定义静态库的函数。
//

#include "rxsMainFrame.h"
#include <sstream>
#include "stdio.h"
#include <malloc.h>
#include <thread>
#include "private.h"

#include <direct.h>
#include <time.h>
#include <io.h>
#include <fcntl.h>


#if _MYX86
#pragma comment(lib,"../inkType/x86/inkType.lib")
#else
#pragma comment(lib,"../inkType/x64/inkType.lib")
#endif // 0


using namespace std;
using namespace inkType;

swGtxparam* swGtxper;

rxsMainFrame::rxsMainFrame()
{
	try {
		char mDate[14] = { 0 };
		char Pathbuffer[512] = { 0 };
		const char* r = getcwd(Pathbuffer, 512);
		swGtxper = new swGtxparam;
		Wrkpd = 0;
		errCode = 0;
		gSct = 0;
		gRct = 0;
		srCircle = 5;
		(*swGtxper).Systemptr = this;
		(*swGtxper).sysFlag = 0;
		(*swGtxper).sysExpFlag = 0;
		(*swGtxper).MotionGpioSer = 0;
		(*swGtxper).Wrkprocess = 0;
		(*swGtxper).FactoryParamer = 0;
		(*swGtxper).WorkParamer = 0;
		(*swGtxper).uipp = { 0,0,0,0 };
		int l = strlen(Pathbuffer) + 1;
		(*swGtxper).CurModulePath = (char*)malloc(l);
		if ((*swGtxper).CurModulePath != 0)
			memcpy((*swGtxper).CurModulePath, Pathbuffer, l);

		GetLocaltime(mDate);
		sprintf(Pathbuffer, "%s\\SysLog", (*swGtxper).CurModulePath);
		if (access(Pathbuffer, 0) == -1)
		{
			l = mkdir(Pathbuffer);
		}
		sprintf(Pathbuffer, "%s\\SysLog\\%s.txt", (*swGtxper).CurModulePath, mDate);
		(*swGtxper).SytemLoger.PushParam(Pathbuffer);
		memset(Pathbuffer, 0, 512);
		sprintf(Pathbuffer, "%s\\SysConfig", Pathbuffer);
		if (access(Pathbuffer, 0) == -1)
		{
			l = mkdir(Pathbuffer);
		}


		//调试使用
		int i = 0;
		while (true) {
			if (i == 10) {
				break;
			}
			_sleep(1000);
			i += 1;
		}

		sprintf(Pathbuffer, "%s\\SysConfig\\AppConfig.ink", (*swGtxper).CurModulePath);
		if (!AppConfigGet(Pathbuffer))
		{
			SysLogRecord("错误：请确认系统文件存在！");
			errCode = -1;
		}
		(*swGtxper).UserMange = new UserManger();
		(*swGtxper).FactoryParamer = new FactoryConfigManger();
		userManger = (UserManger*)(swGtxper)->UserMange;
		FacParam = (FactoryConfigManger*)(swGtxper)->FactoryParamer;
		SysLogRecord("系统构造执行完成!");
	}
	catch (exception& e) {
		std::string str = "构造执行异常!";
		str += e.what();
		SysLogRecord(str.c_str());
	}
}

rxsMainFrame::~rxsMainFrame()
{
	if (swGtxper != 0) {
		if ((swGtxper)->sysFlag != 0)
			Close();
		if ((*swGtxper).MotionGpioSer != 0) {
			delete ((*swGtxper).MotionGpioSer);
			(*swGtxper).MotionGpioSer = 0;
		}
		if ((*swGtxper).FactoryParamer != 0) {
			delete (FactoryConfigManger*)(*swGtxper).FactoryParamer;
			(*swGtxper).FactoryParamer = 0;
		}
		if ((*swGtxper).UserMange != 0) {
			delete (UserManger*)(*swGtxper).UserMange;
			(*swGtxper).UserMange = 0;
		}
		if ((*swGtxper).WorkParamer != 0) {
			delete (WorkParamManger*)(*swGtxper).WorkParamer;
			(*swGtxper).WorkParamer = 0;
		}
		(*swGtxper).WrkparamsDesc.clear();
		(*swGtxper).UserparamsDesc.clear();
		(*swGtxper).AxGrpsMapIndex.clear();
		if ((*swGtxper).CurModulePath != 0) {
			free((*swGtxper).CurModulePath);
			(*swGtxper).CurModulePath = 0;
		}
		(swGtxper)->Dipsq.clear();
		(swGtxper)->Dopsq.clear();
		(swGtxper)->Axpsq.clear();

		if ((swGtxper)->imgableGrpMgr != 0) {
			delete (swGtxper)->imgableGrpMgr;
			(swGtxper)->imgableGrpMgr = 0;
		}
		delete (swGtxper);
		swGtxper = 0;
	}
}

RXSAPIS void rxsMainFrame::Close() {
	SysLogRecord("系统关闭,准备退出!");
	WorkProcess* wrkp = (WorkProcess*)swGtxper->Wrkprocess;
	if (wrkp != 0) {
		wrkp->Stop(false);
		wrkp->Close();
	}
	swGtxper->sysFlag = 0;
	FacParam->Save();
	if (gSct != 0) {
		((thread*)gSct)->join();
	}
	if (gRct != 0) {
		((thread*)gRct)->join();
	}
	if ((swGtxper)->MotionGpioSer != 0) {
		(swGtxper)->MotionGpioSer->Close();
	}
	if (gSct != 0) {
		delete (thread*)gSct;
		gSct = 0;
	}
	if (gRct != 0) {
		delete (thread*)gRct;
		gRct = 0;
	}
}

const char* rxsMainFrame::AppPath() {
	return swGtxper->CurModulePath;
}

void rxsMainFrame::defaultSysParamInit() {
	sysParm.runScanms = 50;
	sysParm.checkScanms = 3;
	sysParm.MotionInpCheckTime = 50;
	sysParm.Vel1 = 10;
	sysParm.Vel2 = 20;
}

void rxsMainFrame::appSegdataAnalysisVec(u8 isW_T, unsigned SegNum, void* Reader) {
	char* pNext = 0;
	char* value = 0;
	unsigned ID = 0;
	SysConfigManger* SysCfgReader = (SysConfigManger*)Reader;
	unsigned xn = 0;
	std::vector<Kvpd>* wu = 0;

	if (isW_T == 1) {
		wu = &(*swGtxper).WrkparamsDesc;
	}
	else {
		wu = &(*swGtxper).UserparamsDesc;
	}

	for (unsigned i = 0; i < SegNum; i++) {
		Kvpd uwd;
		value = (char*)(*SysCfgReader)[i];
		while ((pNext = strtok(value, ":")) != NULL)
		{
			if (xn == 0) {
				uwd.key = pNext;
			}
			else {
				uwd.des = pNext;
			}
			xn += 1;
			value = NULL;
		}
		(*wu).push_back(uwd);
		xn = 0;
	}
}

void rxsMainFrame::appSegdataAnalysisSid(u8 isD_i_o, unsigned SegNum, void* Reader) {
	char* pNext = 0, * value = 0;
	SysConfigManger* SysCfgReader = (SysConfigManger*)Reader;
	unsigned xn = 0;
	map<string, dioPort>* inout = 0;
	vector<string>* oq = 0;
	if (isD_i_o == 1) {
		inout = &(*swGtxper).DinGrps;
		oq = &(swGtxper)->Dipsq;
	}
	else {
		inout = &(*swGtxper).DoutGrps;
		oq = &(swGtxper)->Dopsq;
	}
	for (unsigned i = 0; i < SegNum; i++) {
		string key;
		dioPort ioport;
		memset(&ioport, 0, sizeof(dioPort));
		value = (char*)(*SysCfgReader)[i];
		while ((pNext = strtok(value, ":")) != NULL)
		{
			if (xn == 0) {
				key = pNext;
			}
			else {
				ioport.index = atoi(pNext);
			}
			xn += 1;
			value = NULL;
		}
		(*inout)[key] = (ioport);
		oq->push_back(key);
		xn = 0;
	}
}

void rxsMainFrame::appSegdataAnalysisAlc(unsigned SegNum, void* Reader) {
	char* pNext = 0, * value = 0, * key = 0;
	unsigned xn = 0, ID = 0;
	SysConfigManger* SysCfgReader = (SysConfigManger*)Reader;
	for (unsigned i = 0; i < SegNum; i++) {
		value = (char*)(*SysCfgReader)[i];
		key = 0;
		pNext = 0;
		while ((pNext = strtok(value, ":")) != NULL)
		{
			if (xn == 0) {
				key = pNext;
			}
			else {
				ID = atoi(pNext);
			}
			xn += 1;
			value = NULL;
		}
		if (key != 0) {
			(*swGtxper).AxGrpsMapIndex[key] = ID;
			(swGtxper)->Axpsq.push_back(key);
		}
		xn = 0;
	}
}

bool rxsMainFrame::AppConfigGet(char* path) {
	char* value = 0;
	char* pNext = 0;
	unsigned xn = 0;
	SysConfigManger	SysCfgReader;
	if (SysCfgReader.Open(path) == 0) {
		unsigned SegNum = SysCfgReader.ReadSegSet((char*)"SysConfig");
		if (SegNum > 0) {
			if ((value = (char*)SysCfgReader[(char*)"BpFile"]) != NULL)
				(*swGtxper).bpFile = value;
			if ((value = (char*)SysCfgReader[(char*)"UnAttended"]) != NULL)
				sysParm.UnAttended = atoi(value);
			if ((value = (char*)SysCfgReader[(char*)"runScanms"]) != NULL)
				sysParm.runScanms = atoi(value);
			if ((value = (char*)SysCfgReader[(char*)"checkScanms"]) != NULL)
				sysParm.checkScanms = atoi(value);
			if ((value = (char*)SysCfgReader[(char*)"MotionInpCheckTime"]) != NULL)
				sysParm.MotionInpCheckTime = atoi(value);
			if ((value = (char*)SysCfgReader[(char*)"PhyCoorVp"]) != NULL)
				sysParm.PhyMacCoorLp = atoi(value);
			if ((value = (char*)SysCfgReader[(char*)"KeyCode"]) != NULL)
				(*swGtxper).keyCode = value;
			xn += 1;
		}
		if ((SegNum = SysCfgReader.ReadSegSet((char*)"Axlloc")) > 0) {
			appSegdataAnalysisAlc(SegNum, &SysCfgReader);
			xn += 1;
		}
		if ((SegNum = SysCfgReader.ReadSegSet((char*)"InPort")) > 0) {
			appSegdataAnalysisSid(1, SegNum, &SysCfgReader);
			xn += 1;
		}
		if ((SegNum = SysCfgReader.ReadSegSet((char*)"OutPort")) > 0) {
			appSegdataAnalysisSid(0, SegNum, &SysCfgReader);
			xn += 1;
		}
		if ((SegNum = SysCfgReader.ReadSegSet((char*)"WRKRAD")) > 0) {
			appSegdataAnalysisVec(1, SegNum, &SysCfgReader);
			xn += 1;
		}
		if ((SegNum = SysCfgReader.ReadSegSet((char*)"USERAD")) > 0) {
			appSegdataAnalysisVec(0, SegNum, &SysCfgReader);
			xn += 1;
		}
		return xn == 6 ? true : false;
	}
	return false;
}

bool rxsMainFrame::SysInit() {
	char Pathbuffer[512] = { 0 };
	if ((*swGtxper).bpFile.Length() > 0) {
		memset(Pathbuffer, 0, 512);
		sprintf(Pathbuffer, "%s\\SysConfig\\%s", (*swGtxper).CurModulePath, (*swGtxper).bpFile.chrString());
		if (FacParam->Ready(Pathbuffer))
		{
			return true;
		}
		else {
			SysLogRecord("错误：请确认文件存在或路径配置正确！");
			errCode = -3;
		}
	}
	else {
		SysLogRecord("错误：基础参数BpFile未配置！");
		errCode = -4;
	}
	return true;
}

char* rxsMainFrame::MsgText() {
	return (*swGtxper).MsgText.chrString();
}

int rxsMainFrame::ErrCode() {
	return errCode;
}

void rxsMainFrame::FontMessageProcessCB(uiProcessCallBack func, void* font_obj) {
	(swGtxper)->uipp.fntPtr = font_obj;
	(swGtxper)->uipp.uiProcesser = func;
}

void rxsMainFrame::FrontProcessMsgSender(int msgNo, void* param, bool Ansy) {
	swGtxparam* wrkpGtxer = swGtxper;
	if (wrkpGtxer->uipp.uiProcesser != 0) {
		if (wrkpGtxer->uipp.fntPtr == 0) {
			SysLogRecord("前端消息推送失败,前端对象未配置!");
			return;
		}
		try {
			wrkpGtxer->uipp.msgNo = msgNo;
			wrkpGtxer->uipp.param = param;
			if (wrkpGtxer->uipp.msgNo == 0) {
				wrkpGtxer->uipp.uiProcesser(sysStatus, wrkpGtxer->uipp.msgNo, wrkpGtxer->uipp.fntPtr);
			}
			else {
				wrkpGtxer->uipp.uiProcesser(wrkpGtxer->uipp.param, wrkpGtxer->uipp.msgNo, wrkpGtxer->uipp.fntPtr);
			}
		}
		catch (...) {
			SysLogRecord("前端消息推送异常,请检查相关代码!");
		}
		return;
	}
	SysLogRecord("前端消息推送失败,回调未配置!");
}

bool rxsMainFrame::Ready(MotionUnit* motionDal, WorkProcess* wrkProcessDal) {
	try {
		if (errCode != 0) { return false; }
		SysLogRecord("系统进入Ready");
		(*swGtxper).sysFlag |= 3;
		(*swGtxper).WorkParamer = new WorkParamManger();
		swGtxper->MotionGpioSer = new MotionGpioSerialize();
		errCode = swGtxper->MotionGpioSer->Open(motionDal);
		MotionGpio = swGtxper->MotionGpioSer;

		swGtxper->Wrkprocess = wrkProcessDal;
		Wrkpd = wrkProcessDal;
		stringstream msg;
		msg << "Ready-序列化运动操作Open:" << errCode << " 个!";
		SysLogRecord(msg.str().c_str());
		errCode = 0;
		if (!SysInit()) {
			SysLogRecord("系统初始化失败，请查看系统配置排除问题!");
			return false;
		}
		else {
			WorkProcess* WrkProcesser = (WorkProcess*)(*swGtxper).Wrkprocess;
			MotionGpioSerialize* motionGpio = swGtxper->MotionGpioSer;
			if (WrkProcesser != 0 && motionGpio != 0) {
				WrkProcesser->Ready((void**)&sysStatus);
				WrkProcesser->FacParamLoadSet(motionGpio->AxGrpsGet(), motionGpio->AxNum);
			}
			else {
				SysLogRecord("核心类初始化为NULL异常！");
			}
		}

		WrkProparam = (WorkParamManger*)(*swGtxper).WorkParamer;
		gSct = new std::thread(&rxsMainFrame::ScanCore, this);
		gRct = new std::thread(&rxsMainFrame::RunCore, this);
		sysStatus->FixStatusSet(SystemInfo::FixStaus::RDY);
		return true;
	}
	catch (exception& exmsg) {
		(*swGtxper).MsgText = (char*)"Ready异常:";
		(*swGtxper).MsgText + (char*)exmsg.what();
		SysLogRecord((*swGtxper).MsgText.chrString());
	}
	return false;
}

RXSAPIS bool rxsMainFrame::AxServon(const char* AxName, bool OnOff) {
	MotionGpioSerialize* motionGpio = swGtxper->MotionGpioSer;
	if (motionGpio == 0) { errCode = -51; return false; }
	SystemInfo::ALAMTYPE at;
	if ((*sysStatus).isAlam(at)) {
		errCode = -10;
		return false;
	}
	if ((*sysStatus).CurFixStatus() == SystemInfo::RDY) {
		motionGpio->AxServon(OnOff, AxName);
		return true;
	}
	return true;
}

RXSAPIS short rxsMainFrame::AxHome(const char* AxName, bool ignoreAlam) {
	MotionGpioSerialize* motionGpio = swGtxper->MotionGpioSer;
	if (motionGpio == 0) { errCode = -51; return errCode; }
	if (ignoreAlam) {
		motionGpio->AxHome(AxName);
	}
	else {
		SystemInfo::ALAMTYPE at;
		if ((*sysStatus).isAlam(at)) {
			return -1;
		}
		if ((*sysStatus).CurFixStatus() == SystemInfo::RDY) {
			motionGpio->AxHome(AxName);
			return 0;
		}
		return -2;
	}
	return 0;
}

RXSAPIS short rxsMainFrame::AxMove(const char* AxName, u8 method, bool ignoreAlam) {
	MotionGpioSerialize* motionGpio = swGtxper->MotionGpioSer;
	if (motionGpio == 0) { errCode = -51; return errCode; }
	if (ignoreAlam) {
		motionGpio->AxMove(AxName, method, 0, 0);
	}
	else {
		SystemInfo::ALAMTYPE at;
		if ((*sysStatus).isAlam(at)) {
			return -1;
		}
		SystemInfo::FixStaus fs = (*sysStatus).CurFixStatus();
		if (fs == SystemInfo::RESETING) {
			return -2;
		}
		if (fs == SystemInfo::PAUSEING) {
			return -3;
		}
		if (fs == SystemInfo::WORKING) {
			return -4;
		}
		motionGpio->AxMove(AxName, method, 0, 0);
	}
	return 0;
}

RXSAPIS short rxsMainFrame::AxStop(const char* AxName, bool ignoreAlam) {
	MotionGpioSerialize* motionGpio = swGtxper->MotionGpioSer;
	if (motionGpio == 0) { return -51; }
	return motionGpio->AxStop(AxName) ? 0 : -1;
}

RXSAPIS bool rxsMainFrame::InSignelGet(const char* key) {
	if (key == 0) {
		SysLogRecord("输出切换失败，端口key为NULL！");
		return false;
	}
	MotionGpioSerialize* motionGpio = swGtxper->MotionGpioSer;
	if (motionGpio == 0) { errCode = -51; return false; }
	return  motionGpio->GetDinput(key);
}

RXSAPIS bool rxsMainFrame::OutSignelSet(const char* key, bool on_off) {
	MotionGpioSerialize* motionGpio = swGtxper->MotionGpioSer;
	if (motionGpio == 0) { errCode = -51; return false; }
	return motionGpio->SetDoutput(key, on_off);
}

RXSAPIS void rxsMainFrame::OutSignelTiggle(const char* key) {
	if (key == 0) {
		SysLogRecord("输出切换失败，端口key为NULL！");
		return;
	}
	dioPort curOut = (*swGtxper).DoutGrps[key];
	MotionGpioSerialize* motionGpio = swGtxper->MotionGpioSer;
	if (motionGpio == 0) { errCode = -51; return; }
	dioPort* mapOut = motionGpio->DoGrpsGet();
	motionGpio->SetDoutput(curOut.index, !mapOut[curOut.index].curStatus);
}

short rxsMainFrame::WorkStart(const char* cmdLine) {
	WorkProcess* WrkProcesser = (WorkProcess*)(swGtxper)->Wrkprocess;
	if (WrkProcesser == 0) {
		return -1;
	}
	SystemInfo::ALAMTYPE at;
	if ((*sysStatus).isAlam(at)) {
		return -2;
	}
	if ((*sysStatus).CurFixStatus() != SystemInfo::RDY) {
		return -3;
	}
	return WrkProcesser->Start(cmdLine) ? 0 : -3;
}

short rxsMainFrame::WorkStop(bool isBatchEnd) {
	try {
		WorkProcess* WrkProcesser = (WorkProcess*)(swGtxper)->Wrkprocess;
		if (WrkProcesser == 0) {
			SysLogRecord("加工流为空，请确认已经正确挂载!");
			return -1;
		}
		return WrkProcesser->Stop(isBatchEnd) ? 0 : -4;
	}
	catch (exception  emsg) {
		(*swGtxper).MsgText = (char*)"WorkStop异常:";
		(*swGtxper).MsgText + (char*)emsg.what();
		SysLogRecord((*swGtxper).MsgText.chrString());
	}
	return -101;
}

short rxsMainFrame::WorkPause() {
	WorkProcess* WrkProcesser = (WorkProcess*)(swGtxper)->Wrkprocess;
	if (WrkProcesser == 0) {
		SysLogRecord("加工流为空，请确认已经正确挂载!");
		return -1;
	}
	return WrkProcesser->Pause() ? 0 : -5;
}


short rxsMainFrame::WorkGoon() {
	WorkProcess* WrkProcesser = (WorkProcess*)(swGtxper)->Wrkprocess;
	if (WrkProcesser == 0) {
		SysLogRecord("加工流为空，请确认已经正确挂载!");
		return -1;
	}
	return WrkProcesser->Goon(0) ? 0 : -6;
}

short rxsMainFrame::SysReset() {
	(*sysStatus).FixStatusSet(SystemInfo::RESETING);
	(*swGtxper).sysFlag |= 0x80;
	return 0;
}

RXSAPIS AxisInfo* rxsMainFrame::AxInfo(unsigned& GrpNum) {
	GrpNum = (*swGtxper).AxGrpsMapIndex.size();
	if (GrpNum > 0) {
		return swGtxper->MotionGpioSer->AxGrpsGet();
	}
	return 0;
}

RXSAPIS dioPort* rxsMainFrame::DinGrp(unsigned& GrpNum) {
	MotionGpioSerialize* motionGpio = swGtxper->MotionGpioSer;
	if ((GrpNum = motionGpio->DiNum) > 0) {
		return motionGpio->DiGrpsGet();
	}
	return 0;
}

RXSAPIS dioPort* rxsMainFrame::DoutGrp(unsigned& GrpsNum) {
	MotionGpioSerialize* motionGpio = swGtxper->MotionGpioSer;
	if ((GrpsNum = motionGpio->DoNum) > 0) {
		return motionGpio->DoGrpsGet();
	}
	return 0;
}

void rxsMainFrame::ScanCore()
{
	while (((swGtxper)->sysFlag & 1) == 1) {
		_sleep(srCircle);
		try {
			MotionGpioSerialize* motionGpio = swGtxper->MotionGpioSer;
			WorkProcess* WrkProcesser = (WorkProcess*)(swGtxper)->Wrkprocess;
			if (WrkProcesser == 0) {
				continue;
			}
			if (motionGpio == 0) {
				continue;
			}
			if (((*swGtxper).sysFlag & 0x80) == 0x80) {//复位预处理
				(*swGtxper).sysFlag &= (~0x80);
				unsigned rAxnum;
				vector<string>A;
				if (!WrkProcesser->ResetPrevProcess(&A, rAxnum)) {
					errCode = -101;
					WrkProcesser->AlamLight(3);
					continue;
				}
				if ((rAxnum = A.size()) > 0) {
					for (unsigned x = 0; x < rAxnum; x++) {
						motionGpio->AxHome(A[x].c_str());
					}
				}
				(*swGtxper).sysFlag |= 0x40;//复位预处理完成进入下一步,复位处理
			}
		}
		catch (exception ex) {
			if (((*swGtxper).sysExpFlag & 2) != 2) {
				SysLogRecord(ex.what());
				(*swGtxper).sysExpFlag |= 2;
			}
		}
	}
}

void rxsMainFrame::RunCore() {
	unsigned HomeCirCnt = 0;
	while (((*swGtxper).sysFlag & 2) == 2) {
		try {
			WorkProcess* WrkProcesser = (WorkProcess*)(swGtxper)->Wrkprocess;
			MotionGpioSerialize* motionGpio = (MotionGpioSerialize*)(swGtxper)->MotionGpioSer;
			if (WrkProcesser != 0) {
				if (((*swGtxper).sysFlag & 0x40) == 0x40) {//复位预处理完成后，各州回原点操作
					if (motionGpio != 0) {
						if (!motionGpio->IsOrgFinshed()) {
							continue;
						}
					}
					else {
						SysLogRecord("复位操作:motionGpio == 0请注意！");
					}
					//复位完全完成后，清零标记。
					if (WrkProcesser->ResetFinsh(true, HomeCirCnt)) {
						(*swGtxper).sysFlag &= (~0x40);
						HomeCirCnt = 0;
						sysStatus->FixStatusSet(SystemInfo::RDY);
						sysStatus->HadRestFinsh();
					}
				}
			}
		}
		catch (exception ex) {
			if (((*swGtxper).sysExpFlag & 1) != 1) {
				SysLogRecord(ex.what());
				(*swGtxper).sysExpFlag |= 1;
			}
		}
		_sleep(srCircle);
	}
}
