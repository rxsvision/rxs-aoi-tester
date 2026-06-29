
#include "rxsMainFrame.h"
#include "private.h"
#include <thread>
#include <time.h>
#include <chrono>

using namespace std;

extern swGtxparam* swGtxper;

#define motionCombiner		((MotionCombiner*)mCombiner)
#define devLocServer		((devCommunicationsServer *)devcommserver)

WorkProcess::WorkProcess() {
	iabGather = 0;
	IgnoreFlag = 0;
	mCombiner = 0;
	userManger = 0;
	FacParam = 0;
	WrkParam = 0;
	mainFrameptr = 0;
	baseFlag = 0;
	step = 0;
	trd_wp = 0;
	trd_rp = 0;
	runCircle = 20;
	devcommserver = new devCommunicationsServer();
}

WorkProcess::~WorkProcess() {
	if (baseFlag != 0) {
		baseFlag = 0;
	}
	if (swGtxper != 0 && swGtxper->imgableGrpMgr != 0)
	{
		delete swGtxper->imgableGrpMgr;
		swGtxper->imgableGrpMgr = 0;
	}
	if (iabGather != 0) {
		iabGather->Close();
		delete iabGather;
		iabGather = 0;
	}
	if (mCombiner != 0) {
		delete motionCombiner;
		mCombiner = 0;
	}
	if (trd_wp != 0) {
		((thread*)trd_wp)->join();
		delete ((thread*)trd_wp);
		trd_wp = 0;
	}
	inSignalScanClose();
	if (devcommserver != 0) {
		delete (devCommunicationsServer*)devcommserver;
		devcommserver = 0;
	}
}

void WorkProcess::Ready( void** SysStatus) {
	userManger = (UserManger*)swGtxper->UserMange;
	FacParam = (FactoryConfigManger*)swGtxper->FactoryParamer;
	//MotionGpio = (MotionGpioSerialize*)swGtxper->MotionGpioSer;
	WrkParam = (WorkParamManger*)swGtxper->WorkParamer;
	VariableAssignment();
	//mainFrameptr = (rxsMainFrame*)swGtxper->Systemptr;
	//MotionGpio = swGtxper->MotionGpioSer;
	(*SysStatus) = &sysStatus;
	inSignalScanOpen();
	InitLoader();
}

void WorkProcess::Close() {
	baseFlag = 0;
	userManger = 0;
	FacParam = 0;
	WrkParam = 0;
	mainFrameptr = 0;
	if (devcommserver != 0) {
		delete (devCommunicationsServer*)devcommserver;
		devcommserver = 0;
	}
	inSignalScanClose();
}


bool WorkProcess::SoftTiggerSet(const char* key, u8 source, SoftTiggerEvent func) {
	swGtxper->SoftTigger.TiggerEventSet(key, source, func);
	return true;
}

bool WorkProcess::MotionCombinerOpen(const char* cfgstr) {
	mCombiner = new MotionCombiner(MotionGpio);
	if (mCombiner == 0) {
		return false;
	}
	if (cfgstr != 0) {
		((MotionCombiner*)mCombiner)->ImportCfgString(cfgstr);
	}
	else {
		kvpdChr* val = FacParam->KvpRead("运动组合");
		if (val != 0 && val->val != 0) {
			string tv = val->val;
			vector<string> sv = stringSplit(tv, ',');
			for (int m = 0; m < sv.size(); m++) {
				val = FacParam->KvpRead(sv[m].c_str());
				if (val != 0 && val->val != 0) {
					motionCombiner->ImportCfgString(sv[m], val->val);
				}
			}
		}
	}
	return true;
}

bool WorkProcess::ImageableHasOpen(int& noWich) {
	if (swGtxper == 0) { return false; }
	if (swGtxper->imgableGrpMgr == 0) {
		noWich = -1;
		return false;
	}
	if (swGtxper->imgableGrpMgr->Count() > 0) {
		return swGtxper->imgableGrpMgr->HasOpen(noWich);
	}
	noWich = -2;
	return true;
}

bool WorkProcess::ImageableGatherOpen(ImageableGather* myIG) {
	if (swGtxper == 0) { return false; }
	swGtxper->imgableGrpMgr = new ImageableGrpManger;
	if (myIG == 0) {
		iabGather = new ImageableGather;
	}
	else {
		iabGather = myIG;
	}	
	return iabGather->Prepare();
}

ImageableSensorUnit* WorkProcess::ImageableSensorGet(const char* name) {
	if (swGtxper == 0) { return 0; }
	if (name == 0) { return 0; }
	if (swGtxper->imgableGrpMgr == 0) { return 0; }
	return swGtxper->imgableGrpMgr->Selected(name);
}

const char* WorkProcess::ImageableSensorForeach(bool& isStart, ImageableSensorUnit::DevType& isuType) {
	if (swGtxper == 0) { return 0; }
	if (swGtxper->imgableGrpMgr == 0) { return 0; }
	return swGtxper->imgableGrpMgr->GrpsNameForeach(isStart, isuType);
}

bool WorkProcess::SoftTigger(const char* key) {
	if (swGtxper->SoftTigger.PortNumGet() > 0) {




		return true;
	}
	return false;
}

//s==0  全灭，==1  亮绿灯， ==2 亮黄灯， == 3 红灯
void WorkProcess::AlamLight(unsigned char s, bool on) {
	if (s == 0) {
		AlamGreen(false);
		AlamRed(false);
		AlamYellow(false);
		return;
	}
	if (s == 1) {
		AlamGreen(true);
		AlamRed(false);
		AlamYellow(false);
		return;
	}
	if (s == 2) {
		AlamGreen(false);
		AlamRed(false);
		AlamYellow(true);
		return;
	}
	if (s == 3) {
		AlamGreen(false);
		AlamRed(true);
		AlamYellow(false);
		return;
	}
}

void WorkProcess::inSignalScanOpen() {
	inSignalScanClose();
	baseFlag |= 2;
	trd_rp = new thread(&WorkProcess::RuningProcess, this);
}

void WorkProcess::inSignalScanClose() {
	baseFlag &= (~2);
	if (trd_rp != 0) {
		((thread*)trd_rp)->join();
		delete (thread*)trd_rp;
		trd_rp = 0;
	}
}

bool WorkProcess::reLoadMotionParamSet() {
	return FacParamLoadSet(MotionGpio->AxGrpsGet(), MotionGpio->AxNum);
}

void WorkProcess::RuningProcess() {
	while ((baseFlag & 2) == 2) {
		if ((swGtxper->sysFlag & 8) == 8) {
			swGtxper->sysFlag &= (~8);
			if (!swGtxper->funcNo.ignoreAlam) {
				SystemInfo::ALAMTYPE at;
				if (sysStatus.isAlam(at)) {
					SysLogRecord("功能号调用失败，当前处于报警状态！");
					return;
				}
			}
			FunctionNumCaller(swGtxper->funcNo.No, swGtxper->funcNo.Paramstr.chrString());
		}
		InSignlProcess();
		_sleep(runCircle);
	}
}

void WorkProcess::IgnoreFlagSet(unsigned int iv) {
	IgnoreFlag |= iv;
}

void WorkProcess::IgnoreFlagClr(unsigned int iv) {
	IgnoreFlag &= (~iv);
}

bool WorkProcess::HasIgnoreStatus(unsigned int iv) {
	return ((IgnoreFlag & iv) == iv);
}

short WorkProcess::Start(const char* cmdLine) {
	step = 1;
	baseFlag |= 1;
	if (!sysStatus.HadRestOk()) {
		pSystem->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"设备未复位，请先进行设备复位!");
		return -1;
	}
	if (!StartReady(cmdLine)) {
		pSystem->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"设备加工开始失败，请先处理前置条件!");
		return -2;
	}
	thread trd_wrkp([this]() {
		WorkRuning();
		});
	trd_wrkp.detach();
	sysStatus.FixStatusSet(SystemInfo::FixStaus::RUNING);
	return 0;
}

bool WorkProcess::Stop(bool isBatchEnd) {
	if (isBatchEnd) {
		//批次完成--不立即切换状态

		return StopBatchCompleted();
	}
	else {
		//立即停止作业，状态变换切换
		baseFlag &= (~1);
		bool sr = StopJob();
		sysStatus.FixStatusSet(SystemInfo::RDY);
		if (trd_wp != 0) {
			((thread*)trd_wp)->join();
			if (trd_wp != 0) {
				delete (thread*)trd_wp;
				trd_wp = 0;
			}
		}
		return sr;
	}
}

void WorkProcess::WorkRuning() {
	while ((baseFlag & 1) == 1) {
		int ret = WorkProcesser(step);
		if (ret != 0) {
			//
			//设备状态设置

		}
		_sleep(runCircle);
	}
}

const char* WorkProcess::motionCombinerForeach(bool& isStart) {
	if (mCombiner != 0) {
		const char* V = motionCombiner->Foreach(isStart);
		isStart = false;
		return V;
	}
	else {
		SysLogRecord("运动组合器-Foreach 失败:请先打开在操作!");
	}
	return 0;
}

const char* WorkProcess::motionCombinerSalveForeach(const char* name, unsigned index) {
	if (mCombiner != 0) {
		MotionGrpCombiner* mgc = motionCombiner->Get(name);
		if (mgc->hasFillSalve && index < mgc->hasFillSalve) {
			return mgc->SalveAxList[index].c_str();
		}
	}
	else {
		SysLogRecord("运动组合器-SalveForeach 失败:请先打开在操作!");
	}
	return 0;
}

bool WorkProcess::motionCombinerCfgSet(const char* name, const char* content) {
	if (mCombiner != 0) {
		motionCombiner->ImportCfgString(name, content);
	}
	else {
		SysLogRecord("运动组合器-CfgSet 失败:请先打开在操作!");
	}
	return false;
}


void WorkProcess::motionCombinerSave() {
	if (mCombiner != 0) {
		const char* V = 0;
		string names;
		bool isStart = true;
		while ((V = motionCombiner->Foreach(isStart)) != 0) {
			names += V;
			names += ',';
			string ss = motionCombiner->GetSaveCfgString(V);
			FacParam->KvpUpdater(V, ss.c_str(), "运动组合器中的组合名对应的内容");
			isStart = false;
		}
		if (names.length() > 1) {
			names.pop_back();
			FacParam->KvpUpdater("运动组合", names.c_str(), "运动组合器的组合名称组");
		}
		return;
	}
	SysLogRecord("运动组合器-Save 失败:请先打开在操作!");
}

void WorkProcess::motionCombinerAdd(const char* name, MotionModel mm, const char* masterName, unsigned SalveNum)
{
	if (mCombiner != 0) {
		if (motionCombiner->Creater(name, mm, masterName, 0, SalveNum)) {
			return;
		}
		SysLogRecord("运动组合器-Add 失败!");
		return;
	}
	SysLogRecord("运动组合器-Add 失败:请先打开在操作!");
}

void WorkProcess::motionCombinerAppened(const char* name, const char* SalveName)
{
	if (mCombiner != 0) {
		if (motionCombiner->Appened(name, SalveName)) {
			return;
		}
		SysLogRecord("运动组合器-Appened 失败!");
		return;
	}
	SysLogRecord("运动组合器-Appened 失败:请先打开在操作!");
}


bool WorkProcess::WorkProReloader(const char* path) {
	if (path == 0) {
		return false;
	}
	WorkProParamConditionCheck(true, true);
	if (WrkParam->Load(path)) {
		if (WorkProParamConditionCheck(true, false)) {
			if (sysStatus.CurFixStatus() != SystemInfo::INIT) {
				pSystem->FrontProcessMsgSender(0, 0);
			}
			return true;
		}
	}
	return false;
}

const char* WorkProcess::LocServerAccessForeach(int& index) {
	return devLocServer->ClientForeach(index);
}

int WorkProcess::LocServerSend(const char* tage, const char* content, int len) {
	if (!devLocServer->hasOpen()) {
		SysLogRecord("设备服务未开启!请先打开或配置!", CBC_STR_W_MSG);
		return -1;
	}
	std::string c = content;
	return devLocServer->SendContent(tage, c);
}

int WorkProcess::LocServerSend(const char* tip, int tPort, const char* content, int len) {
	if (!devLocServer->hasOpen()) {
		SysLogRecord("设备服务未开启!请先打开或配置!", CBC_STR_W_MSG);
		return -1;
	}
	if (tip < 0) {
		std::string c = content;
		return devLocServer->SendContent(0, c);
	}
	std::string c = content;
	return devLocServer->SendContent(tip, tPort, c);
}


const char* WorkProcess::LocServerReceive(const char* tage) {
	if (!devLocServer->hasOpen()) {
		SysLogRecord("设备服务未开启!请先打开或配置!", CBC_STR_W_MSG);
		return 0;
	}
	return devLocServer->RecviceContent(tage);
}

const char* WorkProcess::LocServerReceive(const char* tip, int tPort) {
	if (!devLocServer->hasOpen()) {
		SysLogRecord("设备服务未开启!请先打开或配置!", CBC_STR_W_MSG);
		return 0;
	}
	return devLocServer->RecviceContent(tip, tPort);
}

void WorkProcess::LocServerMsgClr(const char* tage) {
	if (!devLocServer->hasOpen()) {
		SysLogRecord("设备服务未开启!请先打开或配置!", CBC_STR_W_MSG);
		return;
	}
	devLocServer->RecviceContentClr(tage);
}


bool WorkProcess::LocServerOn(bool on) {
	if (on) {
		devLocServer->Close();
		kvpdChr* val = FacParam->KvpRead("服务IP");
		if (val == 0 || val->val == 0) { return false; }
		std::string ipport = val->val;
		val = FacParam->KvpRead("服务端口");
		if (val == 0 || val->val == 0) { return false; }
		ipport += ",";
		ipport += val->val;
		return (devLocServer->Open(ipport) == 0);
	}
	else {
		devLocServer->Close();
	}
	return true;
}

void* WorkProcess::HrdWareObjectGet(int index) { return 0; }





void WorkProcess::AlamRed(bool on) {}
void WorkProcess::AlamGreen(bool on) {}
void WorkProcess::AlamYellow(bool on) {}
short WorkProcess::FunctionNumCaller(int funcNo, const char* paramStr) { return 0; }


short WorkProcess::InitLoader() { return 0; }
void WorkProcess::InSignlProcess() {}
void WorkProcess::EmgStopdeal() {}
bool WorkProcess::AutoClearAlam() { return true; }
bool WorkProcess::FacParamLoadSet(AxisInfo* axiGrps, unsigned axiNum) { return true; }
bool WorkProcess::ResetPrevProcess(void* AxNameGrps, unsigned& Axnum) { return true; }
bool WorkProcess::ResetFinsh(bool, unsigned& cnt) { return true; }
bool WorkProcess::StartReady(const char* cmdLine) { return false; }

bool WorkProcess::StopBatchCompleted() { return true; }
bool WorkProcess::StopJob() { return true; }
bool WorkProcess::Pause() { return false; }
bool WorkProcess::Goon(char* cmdLine) { return false; }
int WorkProcess::WorkProcesser(int wrkStep) { return 0; }
bool WorkProcess::WorkProParamConditionCheck(bool, bool) { return true; }