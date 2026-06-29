#pragma once
#include "rxsFixdal.h"
#include "protected_rxsComputerTransferManger.h"
#include <thread>
//#include <mutex>
#include <iostream>
#include <chrono>
#include "protected.h"
#include <fstream>
#include <cstdlib>
#include <sys/stat.h>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

#pragma warning(disable : 4996)



#define MOTIONINPDMS		80
#define LS_DMS_CREAM		200

#define rxsTk				(*(rxsToolKit*)rxsTK)
#define RoiManger			(*(rxsRoiAreaManger*)rxsRoiGrps)
#define	imgableCgather		((CompoundGather*)iabGather)



//后期优化掉本本变量和对应类
#define	CompTransManger			(*(rxsComputerTransferManger*)rxsCT)

using namespace std;


//#define				JUST_ROBORT_RUN			1

#ifdef JUST_ROBORT_RUN
//myRobort	robort;
AuboRobotDal robort;
#endif // JUST_ROBORT_RUN


std::chrono::system_clock::time_point startTimep;
std::chrono::system_clock::time_point endTimep;



WorkProcessDal::WorkProcessDal() {
	rxsImgTPC = 0;
	memset(&runRecordp, 0, sizeof(RuningRecordParam));
	calScpNums = 0;
	memset(&RunGr, 0, sizeof(RunGatherRecord));
	rxsReportGrps = new rxsResultReportGrpsManger;
	rxsRoiGrps = new rxsRoiAreaManger;
	rxsTK = new rxsToolKit;
	rxsRRGA = new ResultReportGrpAssemble;
	rxsReportGrps->MergeMode(true);
	rxsCT = new rxsComputerTransferManger(
		(rxsRoiAreaManger*)rxsRoiGrps,
		rxsReportGrps,
		(rxsToolKit*)rxsTK);
	
}

void WorkProcessDal::Close() {
	pSystem->WorkStop(false);
	//lightSource.Close();
	tzg.Close();
	const char* curPath = WrkParam->CurFilePath();
	FacParam->KvpUpdater("当前加工程式", curPath, 0);
	MotionGpio->SetDoutput("Z刹车打开", false);
	MotionGpio->AxServon(false);
	rxsReportGrps->ResultReportContentClr();

	if (rxsCT != 0) {
		delete ((rxsComputerTransferManger*)rxsCT);
		rxsCT = 0;
	}
	if (rxsTK != 0) {
		delete (rxsToolKit*)rxsTK;
		rxsTK = 0;
	}
	if (rxsReportGrps != 0) {
		delete rxsReportGrps;
		rxsReportGrps = 0;
	}
	if (rxsRoiGrps != 0) {
		delete (rxsRoiAreaManger*)rxsRoiGrps;
		rxsRoiGrps = 0;
	}
	if (rxsRRGA != 0) {
		delete rxsRRGA;
		rxsRRGA = 0;
	}
	if (rxsImgTPC != 0) {
		delete rxsImgTPC;
		rxsImgTPC = 0;
	}
	return WorkProcess::Close();
}

void WorkProcessDal::AlamRed(bool on) {
	MotionGpio->SetDoutput("三色红", on);
}

void WorkProcessDal::AlamGreen(bool on) {
	MotionGpio->SetDoutput("三色绿", on);
}

void WorkProcessDal::AlamYellow(bool on) {
	MotionGpio->SetDoutput("三色黄", on);
}


bool WorkProcessDal::FacParamLoadSet(AxisInfo* axiGrps, unsigned axiNum) {
	int colsNum = 0;
	const char* val = 0;
	for (unsigned x = 0; x < axiNum; x++) {
		colsNum = FacParam->SelectedRow(x);
		if (colsNum < 10) { return false; }
		if ((val = (*FacParam)[(unsigned)0]) != 0) {
			axiGrps[x].RunAcc = std::stod(val);
			axiGrps[x].MotionParam.acc = axiGrps[x].RunAcc;
		}
		if ((val = (*FacParam)[1]) != 0) {
			axiGrps[x].RunDec = std::stod(val);
			axiGrps[x].MotionParam.dec = axiGrps[x].RunDec;
		}
		if ((val = (*FacParam)[2]) != 0) {
			axiGrps[x].RunVel = std::stod(val);
		}
		if ((val = (*FacParam)[3]) != 0) {
			axiGrps[x].Rethome.Vel = std::stod(val);
		}
		if ((val = (*FacParam)[4]) != 0) {
			axiGrps[x].Rethome.ZeroCorrectvel = std::stod(val);
		}
		if ((val = (*FacParam)[5]) != 0) {
			axiGrps[x].Rethome.Mode = std::stoi(val);
		}
		if ((val = (*FacParam)[6]) != 0) {
			axiGrps[x].Rethome.ZeroCorrectpos = std::stol(val);
		}
		if ((val = (*FacParam)[7]) != 0) {
			axiGrps[x].Rethome.Ispostive = (val[0] == '1');
		}
		if ((val = (*FacParam)[8]) != 0) {
			axiGrps[x].Alpha = std::stod(val);
		}
		if ((val = (*FacParam)[9]) != 0) {
			axiGrps[x].Rethome.Offset = std::stof(val);
		}
	}
	kvpdChr* v = FacParam->KvpRead("报表模板");
	if (v != 0 && v->val != 0) {
		vector<string> vs = stringSplit(v->val, ',');
		for (unsigned m = 0; m < vs.size(); m++) {
			kvpdChr* nv = FacParam->KvpRead(vs[m].c_str());
			if (nv != 0 && nv->val != 0) {
				vector<string> fileds = stringSplit(nv->val, ',');
				rxsReportGrps->ResultReportTempSet(vs[m], &fileds[0], fileds.size());
			}
		}
	}
	v = FacParam->KvpRead("当前加工程式");
	if (v != 0 && v->val != 0) {
		if (!WorkProReloader(v->val)) {
			SysLogRecord("当前默认加工程式打开失败！");
		}
	}
	else {
		SysLogRecord("当前默认加工程式路径不存在！");
	}
	return true;
}

void WorkProcessDal::DialGaugeOpen(bool on) {
	if (on) {
		if (DigitalDG.HasOpen()) {
			DigitalDG.Close();
		}
		DigitalDG.Open();
	}
	else {
		DigitalDG.Close();
	}
}

float WorkProcessDal::DialGaugeReadVal() {


	DigitalDG.Grab(true, true);
	return *(float*)DigitalDG.Sensorinfo.ptrGrabResult;
}


ImageableSensorUnit* WorkProcessDal::ImageableSensorDefaultGet() {
	const char* igabn = (*FacParam)["传感器"];
	if (igabn == 0) { return 0; }
	return ImageableSensorGet(igabn);
}

short WorkProcessDal::SensorOpen(bool isReopen) {
	string msg = "";
	const char* val1 = (*FacParam)["激光参数1"];
	const char* val2 = (*FacParam)["激光参数2"];
	std::string S = val1;
	S += ",";
	S += val2;
	short ret = -1;
	const char* igabn = (*FacParam)["传感器"];
	if (igabn == 0) { return -10; }
	ImageableSensorUnit* imgableSensor = ImageableSensorGet(igabn);
	if (isReopen) {
		if (imgableSensor->HasOpen()) {
			imgableSensor->Close();
		}
		ret = imgableSensor->ResetConnect() ? 0 : -1;
	}
	else {
		if (imgableSensor->HasOpen()) {
			imgableSensor->Close();
			ret = imgableSensor->ResetConnect() ? 0 : -1;
		}
		else {
			ret = imgableSensor->Open((char*)S.c_str());
		}
	}
	if (ret == 0) {
		SysLogRecord("光谱仪打开成功!");
	}
	else {
		msg = "光谱仪打开失败：" + ret;
		SysLogRecord(msg.c_str(),CBC_STR_W_MSG);
	}
	return ret;
}

void WorkProcessDal::Zsaftypos(bool chkDone) {
	kvpdChr* val = FacParam->KvpRead("Z安全位");
	if (val != 0 && val->val != 0) {
		float f = std::stof(val->val);
		const char* axName = "AOI-Z";
		AxMoveparam ampi = { axName, 'A', 0, f ,0.0,0.0 };
		MotionGpio->AxMove(ampi);
		if (chkDone) {
			_sleep(MOTIONINPDMS);
			while (!MotionGpio->IsMotionFinsh("AOI-Z")) {
				_sleep(MOTIONINPDMS);
			}
		}
	}
}

short WorkProcessDal::InitLoader() {
	MotionGpio->AxServon(true);
	MotionGpio->SetDoutput("Z刹车打开", true);
	MotionCombinerOpen();
	CompoundGather* igCG = new CompoundGather;
	ImageableGatherOpen(igCG);
	imgableCgather->ResultReportGrpSet(rxsReportGrps);

	const char* val = (*FacParam)["CST光源"];
	//if (val != 0) {
	//	//"COM7,115200,8,1,NONE"
	//	lightSource.Open(val);
	//}
	//else {
	//	SysLogRecord("CST光源-参数获取失败!");
	//}

	tzg.Open("COM1,19200,8,1,NONE");
	//adjF.Open("COM1,9600,8,1,NONE");
	//SoftTiggerSet("高速触发", 0, HSpeedTiggerEnvent);
	CheckResultReportSave(false);
	if (!rxsTk.Ready(pSystem)) {
		SysLogRecord("内部算法库加载失败!");
	}
	rxsImgTPC = new imgTaskProcessCollection(rxsReportGrps, pSystem, rxsTK);
	//LightClose(-1, CST_LightSource::None);
	ImgProcessFlowerLoad();
	GatherSrcPicSave(false);

	val = (*FacParam)["服务启动方式"];
	if (val != 0 && val[0] == '1')
	{
		LocServerOn(true);
	}

#ifdef JUST_ROBORT_RUN
	//robort.Open("192.168.0.121,9786");
	robort.Open((char*)"192.168.15.100,8899");
	sysStatus.HadRestFinsh();
#endif // JUST_ROBORT_RUN

	return 0;
}


bool WorkProcessDal::GetResult_OK_NG(rxsResultReport& rrp) {
	bool result = true;
	MeasurementJudgment* mj;
	rxsResultReport* rrr = 0;
	int len = rxsReportGrps->Count();
	for (int index = 1; index < len; index++) {
		rrr = rxsReportGrps->ResultReportGet(index);
		std::string af[1] = { "标准比对" };
		rrr->SetFiledAppend(af, 1);
		float curv = 0;
		int rc = rrp.RowCount();
		int x = 0;
		for (; x < rc; x++) {
			const char* v = rrp.GetValue(x, "名");
			if (v != 0) {
				std::string cmpval = v;
				if ((mj = WrkParam->MeasurementStandGet(v)) != 0 && mj->enable)
				{
					v = rrp.GetValue(x, "值");
					if (v != 0) {
						curv = atof(v);
						if (!WrkParam->MeasurementResultJudgment(*mj, curv)) {
							rrr->SetValue("标准比对", "NG", x);
							result = false;
						}
						else {
							rrr->SetValue("标准比对", "OK", x);
						}
						continue;
					}
				}
			}
			rrr->SetValue("标准比对", "OK", x);
		}
	}
	return result;
}


bool WorkProcessDal::WorkProParamConditionCheck(bool isLoad_or_Save, bool be_or_after) {
	if (isLoad_or_Save) {
		const char* Val = 0;
		if (be_or_after) {
			RoiManger.Clear();
			rxsImgTPC->Clear();
			rxsReportGrps->ResultReportClr();
			CompTransManger.Clear();
		}
		else {
			if ((Val = WrkParam->AppendedContentGet("CTGM")) != 0) {

				CompTransManger.CfgStringSet(Val);
			}
			else {
				SysLogRecord("无算法流程设置！");
			}
			Kvpdchr Val;
			WrkParam->Selected(1);
			WrkParam->DataReaderKvpd(3, Val);
			if (Val.val != 0) {
				rxsImgTPC->Load(Val.val);
			}

			//------------以下----后期优化掉内容-------------
			rxsResultReport* rpp = rxsReportGrps->ResultReportGet(0);
			if (rpp != 0) {
				std::string rnnn = rpp->Name();
				if (rnnn == "间隙断差报表") {
					WrkParam->Selected(1);
					WrkParam->DataReaderKvpd(4, Val);
					if (Val.key != 0 && Val.val != 0) {
						rpp->outFilter(-1);
						std::vector<std::string> fi = stringSplit(Val.val, ',');
						for (int mn = 0; mn < fi.size(); mn++) {
							rpp->outFilter(atoi(fi[mn].c_str()));
						}
					}
				}
			}
			//------------以上-----后期优化掉内容-----------
		}
	}
	return true;
}

bool WorkProcessDal::StartReady(const char* cmdLine) {
#ifdef JUST_ROBORT_RUN
	//robort.workStart();
#else

	runRecordp.repRunCnt = 1;
	runRecordp.repStableDms = 0;
	if (cmdLine != 0) {
		vector<string> r = stringSplit(cmdLine, '|');
		int  l = r.size();
		if (l > 0) {
			runRecordp.repRunCnt = atoi(r[0].c_str());
			runRecordp.repStableDms = 1000;
			std::string v = (*FacParam)["重复稳定延时"];
			if (v.length() > 0) {
				runRecordp.repStableDms = atoi(v.c_str());
			}
		}
	}
	rxsReportGrps->ResultReportContentClr();
	rxsRRGA->Clr();
	CheckVisualOpen(false);
	if (CompTransManger.Count() < 1) {
		SysLogRecord("请先配置加工计算流程或重新加载加工程式！", CBC_STR_W_MSG);
		return false;
	}
	if (runRecordp.repRunCnt == 1) {
		runRecordp.repStableDms = 100;
		std::string v = (*FacParam)["运行稳定时"];
		if (v.length() > 0) {
			runRecordp.repStableDms = atoi(v.c_str());
		}
		IgnoreFlagClr(GRRTEST);
	}
	else {
		//根据 重复 次数 自动开启GRR测试
		//IgnoreFlagSet(GRRTEST);
	}
#endif // JUST_ROBORT_RUN
	return true;
}


bool WorkProcessDal::StopBatchCompleted() {
	rxsTk.inAutoRun = false;
	return true;
}

bool WorkProcessDal::StopJob() {
	rxsTk.inAutoRun = false;
	LightClose(-1, CST_LightSource::None);
	runRecordp.MmpRecordIndex = 0;
	runRecordp.ScpRecordIndex = 0;
	endTimep = std::chrono::system_clock::now();
	return true;
}

bool WorkProcessDal::Pause() {
	return true;
}

bool WorkProcessDal::Goon(char* cmdLine) {
	return true;
}

bool WorkProcessDal::ResetPrevProcess(void* Axng, unsigned& num) {
	rxsReportGrps->ResultReportContentClr();
	RPP_AX_ORG(Axng, "AOI-Z");
	return true;
}

bool WorkProcessDal::ResetFinsh(bool isResting, unsigned& cnt) {
	if (cnt == 0) {
		cnt += 1;
		MotionGpio->AxHome("AOI-X");
		MotionGpio->AxHome("AOI-Y");
		return false;
	}
	return true;
}

void WorkProcessDal::InSignlProcess() {

}

void WorkProcessDal::EmgStopdeal() {
	rxsTk.inAutoRun = false;
}

bool WorkProcessDal::AutoClearAlam() {
	return true;
}

short WorkProcessDal::FunctionNumCaller(int funcNo, const char* paramStr) {
	switch (funcNo)
	{
	case 0:
		break;
	case 1:
		break;
	default:
		break;
	}
	return 0;
}

bool WorkProcessDal::HpcmpCfgopen(Scanpoint* scp) {
	//高速位置比较模式触发设置
	for (; (scp = (Scanpoint*)WrkParam->DataReaderMsp(runRecordp.ScpRecordIndex)) != 0; runRecordp.ScpRecordIndex++) {
		PoscmpPilist pclis;
		PoscmpSetparam psp;
		pclis.ID = 0;
		pclis.PointNums = 1;
		psp.isHightSpeed = true;
		psp.Cmpdir = 3;
		psp.DoutNo = 0;
		psp.DoutStatus = 1;
		//点位比较设置 填充
		MotionGpio->PosCmpSet("AOI-X", psp, pclis);
		{
			//完毕之后将当前这一段采集进行处理并推送到前端
			thread pPpcDeal([this]() {

				//获取采集结果
				//imgableSensor;

				});
		}
		WrkParam->Selected(4);
	}
	return false;
}

void WorkProcessDal::HpcmpCfgclose() {

}

short WorkProcessDal::moveGatherPrevDeal(bool inRunning) {
	while (!runRecordp.EndGatherInFac) {
		if (inRunning && sysStatus.CurFixStatus() != SystemInfo::FixStaus::RUNING) {
			return 1;
		}
		_sleep(10);
	}
	runRecordp.EndGatherInFac = false;
	return 0;
}


short WorkProcessDal::XmoveGather(Movepath* mp, Scanpoint* scp, bool inRunning) {
	if (moveGatherPrevDeal(inRunning) != 0)
		return 1;
	imgableCgather->dir = 0;
	imgableCgather->ReadyParamSet(mp);
	imgableCgather->GatherReady();
	MotionGpio->AxMove("AOI-X", 'A', mp->speed, mp->Ex, mp->acc, mp->dec);
	thread sthr([this]() {
		if (imgableCgather->Gather(0)) {

		}
		else {
			runRecordp.EndGatherInFac = true;
		}
		});
	sthr.detach();
	_sleep(MOTIONINPDMS);
	while (!MotionGpio->IsMotionFinsh("AOI-X") || !MotionGpio->IsMotionFinsh("AOI-Z")) {
		if (inRunning && sysStatus.CurFixStatus() != SystemInfo::FixStaus::RUNING) {
			return 1;
		}
		_sleep(MOTIONINPDMS);
	}
	return imgableCgather->GatherStop();
}

short WorkProcessDal::YmoveGather(Movepath* mp, Scanpoint* scp, bool inRunning) {
	if (moveGatherPrevDeal(inRunning) != 0)
		return 1;
	imgableCgather->dir = 1;
	imgableCgather->ReadyParamSet(mp);
	imgableCgather->GatherReady();
	MotionGpio->AxMove("AOI-Y", 'A', mp->speed, mp->Ey, mp->acc, mp->dec);

	thread sthr([this]() {
		if (imgableCgather->Gather(0)) {
		}
		else {
			runRecordp.EndGatherInFac = true;
		}
		});
	sthr.detach();

	_sleep(MOTIONINPDMS);
	while (!MotionGpio->IsMotionFinsh("AOI-Y") || !MotionGpio->IsMotionFinsh("AOI-Z")) {
		if (inRunning && sysStatus.CurFixStatus() != SystemInfo::FixStaus::RUNING) {
			return 1;
		}
		_sleep(MOTIONINPDMS);
	}
	return imgableCgather->GatherStop();
}


void WorkProcessDal::CreamSrcPicSave(int crID, rxsImg* img) {
	kvpdChr* val = FacParam->KvpRead("数据保存");
	string path = "C:\\Users\\Administrator\\Desktop\\";
	char fName[30];
	memset(fName, 0, 30);
	if (val->val != 0) { path = val->val; }

	if (product.Name.length() > 0) {
		path += (product.Name + "\\");
	}
	if (!std::experimental::filesystem::exists(path)) {
		//不存在则创建
		std::experimental::filesystem::create_directory(path);
	}

	DateTimeGet(fName, 2);
	path += std::to_string(crID) + "_" + fName;
	path += ".jpg";
	imgprocToolbase srim(img);
	srim.Save(path.c_str());
}


short WorkProcessDal::CreamFlowerProcess(Movepath* mp)
{
	if (!HasIgnoreStatus(TD_CAL)) {
		imgTaskFlowerProcesser* itfp = rxsImgTPC->Selected(mp->Flower);
		if (0 == itfp) {
			SysLogRecord("CreamDataComputerAndSave-选中流程载入失败!请确认配置正确!");
			return -3;
		}
		if (!rxsImgTPC->Run()) {
			pSystem->FrontProcessMsgSender(20, (void*)"CreamFlowerProcess-图像采集运算流程异常!");
		}
		itfp->RunResourceRelease();
		return 0;
	}
	else {
		ImageableSensorUnit* isu = imgableCgather->Selected("HkCream");
		//不进行流程计算，则是仅采集图片
		LightOpen(-1, CST_LightSource::RED);
		_sleep(LS_DMS_CREAM);
		rxsImg spic = { 0,0,0,0,0 };
		isu->Grab(false, true);
		spic.Data = isu->DataGet(spic.Len);
		spic.cols = isu->CtrInfo[0].Width;
		spic.rows = isu->CtrInfo[0].Height;
		spic.Channels = 1;
		pSystem->FrontProcessMsgSender(CBC_IMGSHOW, &spic);
	}
	return 0;
}

short WorkProcessDal::CreamFlyingRacketAndProcess(Movepath* mp, Scanpoint* scp) {
	if (HasIgnoreStatus(ONlYMMOVE)) {
		return 0;
	}
	//是否存在结束位置
	if (((int)(mp->Ex * 1000) == 0) && ((int)(mp->Ey * 1000) == 0)) {
		SysLogRecord("飞拍设置下-结束位置未设置?", CBC_STR_W_MSG);
		return -1;
	}
	onlyMove(mp, scp);
	imgableCgather->crcd.Sx = mp->Sx;
	imgableCgather->crcd.Sy = mp->Sy;
	imgableCgather->crcd.Len = mp->Ey - mp->Sy;
	imgableCgather->crcd.ID = mp->id;
	CreamFlowerProcess(mp);
	return 0;
}


short WorkProcessDal::CreamGatherAndProcess(Movepath* mp, Scanpoint* scp) {
	if (HasIgnoreStatus(ONlYMMOVE)) {
		return 0;
	}
	if (mp->Ez > 0.1) {
		//mp->Ez 复用为可调焦距的 设定
		adjF.Set(mp->Ez);
	}
	imgableCgather->crcd.Sx = mp->Sx;
	imgableCgather->crcd.Sy = mp->Sy;
	imgableCgather->crcd.Len = mp->Ey - mp->Sy;
	imgableCgather->crcd.ID = mp->id;
	CreamFlowerProcess(mp);
	return 0;
}

short WorkProcessDal::MoveToStartPoint(Movepath* mp, bool inRunning) {
	//运动到起点---以及采集前补偿--
	MotionGpio->AxMove("AOI-X", 'A', 0, mp->Sx);
	MotionGpio->AxMove("AOI-Y", 'A', 0, mp->Sy);
	MotionGpio->AxMove("AOI-Z", 'A', 0, mp->Sz);

	_sleep(MOTIONINPDMS);
	while (!MotionGpio->IsMotionFinsh("AOI-X") || !MotionGpio->IsMotionFinsh("AOI-Y") || !MotionGpio->IsMotionFinsh("AOI-Z")) {
		if (inRunning && sysStatus.CurFixStatus() != SystemInfo::FixStaus::RUNING) {
			return 1;
		}
		_sleep(MOTIONINPDMS);
	}

	if (runRecordp.repStableDms > 0) {
		_sleep(runRecordp.repStableDms);
	}
	return 0;
}

void WorkProcessDal::GetMoveMethodAndSet(u8 m, bool& isOnlyMove, bool& isSegm, MotionModel& mm, void* AxLn, Movepath* mp) {
	isOnlyMove = false;
	isSegm = false;
	if ((m & 0x80) == 0x80) {
		isOnlyMove = true;
	}
	u8 src = m;
	m &= 0x7F;
	u8 n = (m >> 4);
	u8 mpv = 0;
	string AXS[3] = { "AOI-X","AOI-Y" ,"AOI-Z" };
	if (n == 1) {//直线插补
		mm = MotionModel::GROUP_LINE_INTERPOLATION_MASTER;
		for (int x = 0; x < 3; x++) {
			mpv = (1 << x);
			if ((m & mpv) == mpv) {
				(*(string*)AxLn) = AXS[x];
				MotionGpio->GroupMoveOpen(AXS[x].c_str(), MotionModel::GROUP_LINE_INTERPOLATION_MASTER, 2);

				ZMotionDal::GrpExtrenData ged = { 0 };
				ged.TgaePos = mp->Ez;
				MotionGpio->GroupMoveSalveCfgPush(AXS[x].c_str(), "AOI-Z", &ged, sizeof(ZMotionDal::GrpExtrenData));

				if (x == 0) {
					ged.TgaePos = mp->Ey;
					MotionGpio->GroupMoveSalveCfgPush(AXS[x].c_str(), "AOI-Y", &ged, sizeof(ZMotionDal::GrpExtrenData));
				}
				else if (x == 1) {
					ged.TgaePos = mp->Ex;
					MotionGpio->GroupMoveSalveCfgPush(AXS[x].c_str(), "AOI-X", &ged, sizeof(ZMotionDal::GrpExtrenData));
				}
				else {
					//Z
				}
				break;
			}
		}
	}
	else if (n == 2) {//圆弧插补
		mm = MotionModel::GROUP_ARC_INTERPOLATION_MASTER;
		float dx = mp->Ex - mp->Sx;
		float dy = mp->Ey - mp->Sy;
		for (int x = 0; x < 3; x++) {
			mpv = (1 << x);
			if ((m & mpv) == mpv) {
				(*(string*)AxLn) = AXS[x];
				MotionGpio->GroupMoveOpen(AXS[x].c_str(), MotionModel::GROUP_ARC_INTERPOLATION_MASTER, 1);

				ZMotionDal::GrpExtrenData ged;
				ged.ArcParam.center.x = mp->Sx + dx / 2;
				ged.ArcParam.center.y = mp->Sy + dy / 2;
				ged.ArcParam.ends.x = mp->Ex;
				ged.ArcParam.ends.y = mp->Ey;
				if (x == 0) {
					MotionGpio->GroupMoveSalveCfgPush(AXS[x].c_str(), "AOI-Y", &ged, sizeof(ZMotionDal::GrpExtrenData));
				}
				else if (x == 1) {
					MotionGpio->GroupMoveSalveCfgPush(AXS[x].c_str(), "AOI-X", &ged, sizeof(ZMotionDal::GrpExtrenData));
				}
				else {

				}
				break;
			}
		}
	}
	else if (n == 7) {//分段式
		isSegm = true;
	}
	else {
		mm = MotionModel::PP;
	}
}


short WorkProcessDal::onlyMove(Movepath* mp, Scanpoint* scp) {
	MotionGpio->AxMove("AOI-X", 'A', mp->speed, mp->Ex);
	MotionGpio->AxMove("AOI-Y", 'A', mp->speed, mp->Ey);
	MotionGpio->AxMove("AOI-Z", 'A', mp->speed, mp->Ez);
	_sleep(MOTIONINPDMS);
	while (!MotionGpio->IsMotionFinsh("AOI-X") || !MotionGpio->IsMotionFinsh("AOI-Y") || !MotionGpio->IsMotionFinsh("AOI-Z")) {
		if (sysStatus.CurFixStatus() != SystemInfo::FixStaus::RUNING) {
			return 1;
		}
		_sleep(MOTIONINPDMS);
	}
	return 0;
}


short WorkProcessDal::GroupDirScanerProcess(Movepath* mp, Scanpoint* sp, MotionModel mm, const char* Axln) {
	//主轴运动
	if (memcmp(Axln, "AOI-X", 6) == 0)
		MotionGpio->AxMove(Axln, 'G', 0, mp->Ex, mp->acc, mp->dec);
	else if (memcmp(Axln, "AOI-Y", 6) == 0)
		MotionGpio->AxMove(Axln, 'G', 0, mp->Ey, mp->acc, mp->dec);
	else
		return -1;

	_sleep(MOTIONINPDMS);
	while (!MotionGpio->IsMotionFinsh(Axln)) {
		if (sysStatus.CurFixStatus() != SystemInfo::FixStaus::RUNING) {
			return 1;
		}
		_sleep(MOTIONINPDMS);
	}
	//到位后停止插补运动
	MotionGpio->GroupMoveCancel(Axln);
	return 0;
}


short WorkProcessDal::SegmentedScanerProcess(Movepath* mp, Scanpoint* sp) {
	HpcmpCfgopen(sp);

	while (!MotionGpio->IsMotionFinsh("AOI-X") || !MotionGpio->IsMotionFinsh("AOI-Y") || !MotionGpio->IsMotionFinsh("AOI-Z")) {
		if (sysStatus.CurFixStatus() != SystemInfo::FixStaus::RUNING) {
			return 1;
		}
		_sleep(MOTIONINPDMS);
	}

	HpcmpCfgclose();
	return 0;
}

short WorkProcessDal::MovePathProcess(Movepath* mp) {
	Scanpoint* scp = 0;
	short retVal = 0;
	bool justMove = false;
	bool isSegm = false;
	MotionModel MM;
	string Axln;
	MoveToStartPoint(mp, false);
	if (mp->Type == 0xFF) {
		//2D 定点位拍照
		return CreamGatherAndProcess(mp, scp);
	}
	else if (mp->Type == 0xFE) {
		//2D 飞拍处理
		return CreamFlyingRacketAndProcess(mp, scp);
	}
	runRecordp.tcpRecvNums += 1;
	//--3D--的目标传感器切换
	Kvpdchr Val;
	WrkParam->Selected(2);
	WrkParam->DataReaderKvpd(2, Val);
	ImageableSensorUnit* isu = imgableCgather->Selected(Val.val);
	if (isu == 0) {
		SysLogRecord("不存在的传感器，请确认创建对应传感器!", CBC_STR_E_MSG);
		return -20;
	}
	else {
		if (!isu->HasOpen()) {
			SysLogRecord("传感器未能成功打开，请确认相关配置/操作正确!", CBC_STR_E_MSG);
			return -21;
		}
	}

	GetMoveMethodAndSet(mp->Type, justMove, isSegm, MM, &Axln, mp);
	if (HasIgnoreStatus(ONlYMMOVE)) {
		justMove = true;
	}
	if (justMove) {
		//仅运动
		if ((retVal = onlyMove(mp, scp)) != 0) {
			SysLogRecord("仅运动执行完成-失败!");
			return retVal;
		}
	}
	else if (isSegm) {
		//分段方式，高速位置比较输出触发
		if ((retVal = SegmentedScanerProcess(mp, scp)) != 0) {
			SysLogRecord("分段比较模式执行完成-失败!");
			return retVal;
		}
	}
	else {
		if (MM == MotionModel::PP) {
			//点位运动
			if ((mp->Type & 1) == 1) {// X-方向
				WrkParam->Selected(4);
				retVal = XmoveGather(mp, scp);
			}
			else if ((mp->Type & 2) == 2) {//Y-方向
				WrkParam->Selected(4);
				retVal = YmoveGather(mp, scp);
			}
			else if ((mp->Type & 4) == 4) {//Z-方向

			}
			else if ((mp->Type & 3) == 3) {//XY组合-群组

			}
			else if ((mp->Type & 7) == 7) {//XY组合-群组

			}
		}
		else {
			//群组运动
			if ((retVal = GroupDirScanerProcess(mp, scp, MM, Axln.c_str())) != 0) {
				SysLogRecord("群组运动执行完成-失败!");
				return retVal;
			}
		}
	}

	if (sysStatus.CurFixStatus() != SystemInfo::FixStaus::RUNING) {
		return 1;
	}
	return retVal;
}

void WorkProcessDal::SpecFileCut() {
	intptr_t   hFile = 0;
	struct _finddata_t fileinfo;
	string p;
	int l = 0;
	char fName[30];
	memset(fName, 0, 30);
	DateTimeGet(fName, 2);
	std::string tagePath = "E:\\PicBkup\\";
	tagePath += fName;
	tagePath += "\\";
	if (!std::experimental::filesystem::exists(tagePath)) {
		//不存在则创建
		std::experimental::filesystem::create_directory(tagePath);
	}
	std::string curp = pSystem->AppPath();
	int result = 0;
	if ((hFile = _findfirst(p.assign(curp).append("\\*.bmp").c_str(), &fileinfo)) != -1)//若查找成功，则进入
	{
		do
		{
			if ((fileinfo.attrib & _A_SUBDIR))
			{
			}
			else
			{
				result = rename(((curp + "\\") + fileinfo.name).c_str(), (tagePath + fileinfo.name).c_str());
				if (result != 0) {
					SysLogRecord("加工前图片文件剪切失败!");
				}
			}
			l++;
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}


int WorkProcessDal::WorkProcess() {
	Movepath* mp = 0;
	short retv = 0;
	runRecordp.MmpRecordIndex = 0;
	runRecordp.ScpRecordIndex = 0;
	runRecordp.EndGatherInFac = true;
	runRecordp.tcpRecvNums = 0;
	WrkParam->Selected(3);
	for (; (mp = (Movepath*)WrkParam->DataReaderMsp(runRecordp.MmpRecordIndex)) != 0; runRecordp.MmpRecordIndex++) {
		if (sysStatus.CurFixStatus() != SystemInfo::FixStaus::RUNING) {
			return 1;
		}
		if ((retv = MovePathProcess(mp)) < 0) {
			switch (retv)
			{
			case -201:
				SysLogRecord("方向参数错误!", CBC_STR_E_MSG);
				break;
			default:
				break;
			}
		}
		WrkParam->Selected(3);
	}

	if (HasIgnoreStatus(AUTOEXIT)) {//忽略自动退出
		if (HasIgnoreStatus(ONlYMMOVE)) {
			return 0;
		}
		//等待实际采集完后副本拷贝完成
		while (!runRecordp.EndGatherInFac) {
			_sleep(50);
		}
	}
	else {//需要自动退出
		Zsaftypos(false);
		if (HasIgnoreStatus(ONlYMMOVE)) {
			OutMatterPos_Move();
			return 0;
		}
		//等待实际采集完后副本拷贝完成
		while (!runRecordp.EndGatherInFac) {
			_sleep(50);
		}
		OutMatterPos_Move();
	}
	if (rxsTk.notCalculating) {
		return 0;
	}
	SysLogRecord("流程结束等待计算结果中....!", CBC_STR_W_MSG);
	do {
		_sleep(200);
		if (sysStatus.CurFixStatus() != SystemInfo::FixStaus::RUNING) {
			SysLogRecord("加工停止，退出等待!", CBC_STR_W_MSG);
			return 1;
		}
	} while (!CompTransManger.isCompleted() || !rxsImgTPC->HadFinshed() || !imgableCgather->isCompeleted());
	SysLogRecord("全部计算已经完成!", CBC_STR_W_MSG);
	return 0;
}

typedef struct {
	int ColIndex;
	double Tolerance;
}GrrSCF;

typedef struct {
	double maxVal;
	double minVal;
}GrrCalr;


void WorkProcessDal::GRRTester(u8 method, bool needGrr) {
	if (needGrr) {
		WrkParam->Selected(1);
		Kvpdchr val;
		WrkParam->DataReaderKvpd("GRR列", val);
		if (val.val == 0) {
			return;
		}
		//需要读取输出的文件名及路径，得到文件中的内容
		const char* fp = GetSaveReportFile();
		rxsResultReport* rpp = 0;
		if (rpp == 0) {
			SysLogRecord("需操作的目标报表为空，未能读入报表数据!");
			return;
		}
		std::vector<GrrSCF>  grrSCF;
		std::vector<std::string> ps = stringSplit(val.val, '|');
		for (size_t i = 0; i < ps.size(); i++) {
			std::vector<std::string> psuu = stringSplit(ps[0], ',');
			grrSCF.push_back(GrrSCF{ atoi(psuu[0].c_str()), atof(psuu[1].c_str()) });
		}
		std::vector<GrrCalr>grrColValGrp(grrSCF.size(), { -10000.0f,-10000.0f });
		double zjv = 0.0f;
		unsigned rc = rpp->RowCount();
		for (unsigned k = 0; k < rc; k++) {
			for (int m = 0; m < grrSCF.size(); m++) {
				const char* dtVal = rpp->GetValue(k, grrSCF[m].ColIndex);
				if (dtVal != 0) {//需要加入是否为 数字判断 
					zjv = atof(dtVal);
					if (zjv > grrColValGrp[m].maxVal) {
						grrColValGrp[m].maxVal = zjv;
					}
					if (grrColValGrp[m].minVal > zjv) {
						grrColValGrp[m].minVal = zjv;
					}
				}
			}
		}
		double difVal;
		std::vector<double> GrrVal;
		for (rsize_t i = 0; i < grrColValGrp.size(); i++) {
			difVal = grrColValGrp[i].maxVal - grrColValGrp[i].minVal;
			GrrVal.push_back(difVal / grrSCF[i].Tolerance);
		}
	}
}

int WorkProcessDal::WorkProcesser(int wrkStep) {

#ifdef JUST_ROBORT_RUN
	//ImageableSensorUnit* isu = imgableCgather->Selected("HkCream");
	//if (isu == 0) {
	//	pSystem->FrontProcessMsgSender(20, (void*)"不存在的传感器，请确认创建对应传感器!");
	//	return -20;
	//}
	//else {
	//	if (!isu->HasOpen()) {
	//		pSystem->FrontProcessMsgSender(21, (void*)"传感器未能成功打开，请确认相关配置/操作正确!");
	//		return -21;
	//	}
	//}
	//int execi = 0;
	//while (true) {
	//	int wich = 0;
	//	_sleep(80);
	//	if (robort.isWorkEnd(wich)) {
	//		break;
	//	}
	//	if (wich > 0) {
	//		if (wich != execi) {
	//			pSystem->FrontProcessMsgSender(13, 0);
	//			Movepath mvp;
	//			memset(mvp.Flower, 0, FLOWER_LEN);
	//			memcpy(mvp.Flower, "缺陷检测VM", 11);
	//			CreamFlowerProcess(&mvp, isu);
	//			robort.ImgProcessFinsh();
	//			execi += 1;
	//		}
	//	}
	//}
	//pSystem->WorkStop(false);
	//pSystem->FrontProcessMsgSender(CBC_WRKSTOP, 0);

#else
	startTimep = std::chrono::system_clock::now();
	rxsTk.inAutoRun = true;
	while ((runRecordp.repRunCnt--) > 0) {
		try {
			short ret = 0;
			LocServerMsgClr();
			pSystem->FrontProcessMsgSender(13, 0);
			ret = WorkProcess();
			if (!HasIgnoreStatus(CHKRR_SAVE)) {
				TcpServerRepResultGet(runRecordp.tcpRecvNums);
				SaveCheckResultReport();
			}
			//2D 原图存储开启
			if (!HasIgnoreStatus(TDGSRC_SAVE)) {
				SpecFileCut();
			}
		}
		catch (std::exception& e) {
			std::string msg = "WorkProcesser...流程异常=>\r\n";
			msg += e.what();
			SysLogRecord(msg.c_str());
		}
		if (runRecordp.repStableDms > 0) {
			_sleep(runRecordp.repStableDms);
		}
	}
	pSystem->WorkStop(false);
	if (!HasIgnoreStatus(GRRTEST)) {
		u8 	rrpOutMethod = 0;
		const char* rpov = (*FacParam)["GRR报表方式"];//报表保存方式==0 默认在重复GRR模式下多分合一输出，==1 单份一份数据一份报表输出
		if (rpov != 0) {
			rrpOutMethod = atoi(rpov);
		}
		GRRTester(rrpOutMethod, true);
	}
	pSystem->FrontProcessMsgSender(CBC_WRKSTOP, 0);

#endif // JUST_ROBORT_RUN

	return 0;
}

int WorkProcessDal::HSpeedTiggerEnvent(const char*, unsigned) {
	return 0;
}


void WorkProcessDal::TabMotionMode(const char* AxName, u8 Mode) {
	//目前主要是 点位 和 直线插补
}


short WorkProcessDal::TiggerTaber(int wich) {
	ImageableSensorUnit* imgableSensor = imgableCgather->CurSelectedGet();
	if (imgableSensor) {
		if (wich == 0) {//连续触发
			imgableSensor->TiggerTaber(0);
			return 0;
		}
		if (wich == 1) {//外部触发
			imgableSensor->TiggerTaber(1);
			return 0;
		}

		if (wich == 3) {//编码器X
			return imgableCgather->EncTiggerTaber(imgableSensor, 0);
		}
		if (wich == 4) {//编码器Y
			return imgableCgather->EncTiggerTaber(imgableSensor, 1);
		}
		return 8;
	}
	return -9;
}

void WorkProcessDal::flushResult(const char* path, const char* contData, unsigned cdLen, bool isAppend) {
	FILE* file = 0;
	if (isAppend) {
		file = fopen(path, "a");
	}
	else {
		file = fopen(path, "wt");
	}
	if (file != NULL) {
		fwrite(contData, 1, cdLen, file);
		fclose(file);
	}
}

bool WorkProcessDal::SaveTestdata(int pNums, rxsPointCouldp* pcs, double* srcZdatas, u8 x_or_y, const char* fn) {
	stringstream cds;
	cds << fixed << setprecision(5);
	double* xy = new double[pNums];
	_STDP stdp;
	if (srcZdatas == 0) {
		SysLogRecord("原采集数据为NULL！");
		return false;
	}
	for (unsigned x = 0; x < pNums; x++) {
		cds << pcs[x].x << "," << pcs[x].y << "," << pcs[x].z << "," << srcZdatas[x] << "\n";
		if (x_or_y == 0)
			xy[x] = pcs[x].x;
		else {
			xy[x] = pcs[x].y;
		}
	}
	StraightnessGet(pNums, xy, srcZdatas, &stdp);
	std::stringstream ss;
	ss << std::fixed << std::setprecision(5);
	ss << "K:" << stdp.k << ",B:" << stdp.b << ",+公差:" << stdp.pDiff << ",-公差:" << stdp.nDiff << ",直线度:" << stdp.fDiffVal;
	SysLogRecord(ss.str().c_str());

	string c = cds.str();
	flushResult(fn, c.c_str(), c.length(), true);
	delete[] xy;
	return true;
}

void WorkProcessDal::CheckVisualOpen(bool on) {
	if (on) {
		rxsTk.isDebug = true;
	}
	else {
		rxsTk.isDebug = false;
	}
}

void WorkProcessDal::OnlyShowNoCalculation(bool on) {
	if (on) {
		rxsTk.notCalculating = true;
		//
	}
	else {
		rxsTk.notCalculating = false;
	}
}

void WorkProcessDal::OnlyMoveNoScan(bool on) {
	if (on) {
		IgnoreFlagSet(IgnoreType::ONlYMMOVE);
	}
	else {
		IgnoreFlagClr(IgnoreType::ONlYMMOVE);
	}
}


void WorkProcessDal::GatherZValueFilter(bool on) {
	if (on)
		IgnoreFlagClr(IgnoreType::ZVALFILTER);
	else {
		IgnoreFlagSet(IgnoreType::ZVALFILTER);
	}
}

long WorkProcessDal::getCycleTime() {
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTimep - startTimep);
	long long  diffs = duration.count();
	return diffs;
}

void WorkProcessDal::rtkInParamSet(int crID, u8 dir, Movepath& mp) {
	kvpdChr* val = FacParam->KvpRead("数据保存");
	string path = "C:\\Users\\Administrator\\Desktop\\";
	char fName[30];
	memset(fName, 0, 30);
	if (val->val != 0) { path = val->val; }
	DateTimeGet(fName, 2);

	string dirMap[3] = { "_X","_Y","_XY" };
	if (product.Name.length() > 0) {
		path += (product.Name + "\\");
	}
	if (!std::experimental::filesystem::exists(path)) {
		//不存在则创建
		std::experimental::filesystem::create_directory(path);
	}
	std::stringstream outFn;
	outFn << fName << "_" << crID << dirMap[dir] << ".pcd";
	string fname = path + outFn.str();
	rxsResultReport* rpp = rxsReportGrps->ResultReportGet(0);
	if (rpp != 0) {
		int ri = rpp->AddRow() - 1;
		rpp->SetValue("ID", std::to_string(crID), ri);
		rpp->SetValue("输出文件", outFn.str(), ri);
	}
	rxsTk.SavePathSet(fname);
	rxsTk.setExparam(&mp);
}


void WorkProcessDal::CheckResultClr() {
	rxsReportGrps->ResultReportContentClr();
}

short WorkProcessDal::HasCheckResult(const char* rrName) {
	rxsResultReport* rpp = 0;
	if (rrName == 0) {
		rpp = rxsReportGrps->ResultReportGet(0);
	}
	else {
		rpp = rxsReportGrps->ResultReportGet(rrName);
	}
	if (rpp != 0) {
		return rpp->RowCount();
	}
	return -1;
}

void WorkProcessDal::CheckResultReportSave(bool on) {
	if (on) {
		IgnoreFlagClr(CHKRR_SAVE);
	}
	else {
		IgnoreFlagSet(CHKRR_SAVE);
	}
}

void WorkProcessDal::AutoOutMatterOpen(bool on) {
	if (on) {
		IgnoreFlagClr(AUTOEXIT);
	}
	else {
		IgnoreFlagSet(AUTOEXIT);//忽略自动退出
	}
}

void WorkProcessDal::GatherSrcPicSave(bool on) {
	if (on) {
		IgnoreFlagClr(TDGSRC_SAVE);
	}
	else {
		IgnoreFlagSet(TDGSRC_SAVE);//忽略2D采集原图保存
	}
}

const char* WorkProcessDal::GetSaveReportFile() {
	string fpath = "E:\\";
	kvpdChr* val = FacParam->KvpRead("报告路径");
	if (val != 0 && val->val != 0) {
		fpath = val->val;
	}
	std::string fName;
	bool isAppend = false;

	Kvpdchr fnval;
	WrkParam->Selected(1);
	WrkParam->DataReaderKvpd(1, fnval);
	if (fnval.key != 0 && fnval.val != 0) {
		fName = fnval.val;
		isAppend = true;
	}

	std::stringstream ss;
	ss << fpath << ((product.Name.length() > 0) ? product.Name : "P") << "_rep_" << fName << ".csv";
	return ss.str().c_str();
}


void WorkProcessDal::SaveCheckResultReport() {
	rxsImgTPC->ResultReportAssember();
	rxsResultReport srm;
	if (rxsReportGrps->SingleReportMerge(srm)) {
		product.curPass = GetResult_OK_NG(srm);
		if (product.curPass) {
			product.okNum += 1;
		}
		else {
			product.ngNum += 1;
		}
	}
	else {
		SysLogRecord("报表单一输出合并失败,未进行统计!");
		return;
	}
	std::string fp = GetSaveReportFile();
	std::stringstream ssct;
	std::string scanCode;
	rxsResultReport* result = rxsReportGrps->ResultReportGet("条码内容");
	if (result != 0 && result->RowCount() > 0) {
		scanCode = result->GetValue(0, "条码");
	}
	if (access(fp.c_str(), 0) == 0) {
		//报表存在
		ssct << (scanCode.length() > 0 ? scanCode : "code") << ",";
		rxsResultReport* rrp = 0;
		int index = 1;
		while ((rrp = rxsReportGrps->ResultReportGet(index)) != 0) {
			int rc = rrp->RowCount();
			for (int i = 0; i < rc; i++) {
				const char* val = rrp->GetValue(i, "值");
				if (val != 0) {
					ssct << val << ",";
				}
			}
			index += 1;
		}
		ssct << (product.curPass ? "PASS" : "FAILD") << "\n";
		flushResult(fp.c_str(), ssct.str().c_str(), ssct.str().length(), true);
	}
	else {
		//不存在则创建 并写入头
		ofstream out_data;
		out_data.open(fp.c_str(), ios::app);
		std::stringstream remaphead;
		//列头	
		out_data << "条码,";
		remaphead << "条码,";
		rxsResultReport* rrp = 0;

		int index = 1;
		while ((rrp = rxsReportGrps->ResultReportGet(index)) != 0) {
			int rc = rrp->RowCount();
			for (int i = 0; i < rc; i++) {
				const char* val = rrp->GetValue(i, "名");
				const char* rrmpval = 0;
				bool enb = false;
				if (val != 0) {
					out_data << val << ",";
					//remaphead << rxsReportGrps->RemapColumnHeadGet(val) << ",";
					rrmpval = WrkParam->rrpThGet(val, &enb);
					if (!enb) {
						remaphead << val << ",";
					}
					else {
						remaphead << rrmpval << ",";
					}
					val = rrp->GetValue(i, "值");
					ssct << val << ",";
				}
			}
			index += 1;
		}
		out_data << "结果" << "\n" << remaphead.str() << "结果" << "\n";
		out_data << (scanCode.length() > 0 ? scanCode : "code") << "," << ssct.str() << (product.curPass ? "PASS" : "FAILD") << "\n";
		out_data.close();
	}
}


const char* WorkProcessDal::ResultReportFieldForech(unsigned index) {
	unsigned Num = 0;
	std::string* A = rxsReportGrps->AllFieldGet(Num);
	if (index < Num)
		return A[index].c_str();

	return 0;
}

const char* WorkProcessDal::ResultReportTempNameForech(unsigned index) {
	if (index == 0) {
		return rxsReportGrps->ResultReportTempNameForech();
	}
	return rxsReportGrps->ResultReportTempNameForech(false);
}

const char* WorkProcessDal::ResultReportTempFieldForech(const char* tpName, unsigned index) {
	if (tpName == 0) {
		SysLogRecord("输出报告模板遍历失败，tpName为空错误!", CBC_STR_E_MSG);
		return 0;
	}
	unsigned nums = 0;
	string* fs = rxsReportGrps->ResultReportTempFiledGet(tpName, nums);
	if (index < nums) {
		return fs[index].c_str();
	}
	return 0;
}

void WorkProcessDal::ResultReportTempFieldAppends(const char* tpName, const char* Filed) {
	if (tpName == 0 || Filed == 0) {
		SysLogRecord("输出报告追加字段失败，必要参数两个名称存在空错误!", CBC_STR_E_MSG);
		return;
	}
	if (!rxsReportGrps->TempExist(tpName)) {
		kvpdChr* v = FacParam->KvpRead("报表模板");
		string nsv = tpName;
		if (v != 0 && v->val != 0) {
			nsv += ",";
			nsv += v->val;
		}
		FacParam->KvpUpdater("报表模板", nsv.c_str(), "不同的报表输出模板名称集合!");
	}
	rxsReportGrps->ResultReportTempAppends(tpName, Filed);
	unsigned nums = 0;
	string* tmpFileds = rxsReportGrps->ResultReportTempFiledGet(tpName, nums);
	if (nums > 0) {
		string content;
		nums -= 1;
		for (unsigned n = 0; n < nums; n++) {
			content += tmpFileds[n] + ",";
		}
		content += tmpFileds[nums];
		FacParam->KvpUpdater(tpName, content.c_str(), "报表输出模板的字段集合!");
	}
}

void WorkProcessDal::ResultReportCreater(const char* tempName, const char* rrName) {
	if (tempName == 0 || rrName == 0) {
		SysLogRecord("输出报告创建失败，必要参数两个名称存在空错误!", CBC_STR_E_MSG);
		return;
	}
	rxsReportGrps->ResultReportAdd(tempName, rrName);
}

const char* WorkProcessDal::ResultReportForech(unsigned index) {
	rxsResultReport* V = rxsReportGrps->ResultReportGet(index);
	if (V == 0) { return 0; }
	return V->Name();
}

const char* WorkProcessDal::ResultReportFiledsGet(const char* rrName, unsigned index) {
	if (rrName == 0) {
		SysLogRecord("输出报告字段遍历失败，rrName为空错误!", CBC_STR_E_MSG);
		return 0;
	}
	rxsResultReport* V = rxsReportGrps->ResultReportGet(rrName);
	if (V == 0) { return 0; }
	return V->GetFiled(index);
}

unsigned WorkProcessDal::ResultReportRowCount(const char* rrName) {
	if (rrName == 0) {
		SysLogRecord("输出报告行数获取失败，rrName为空错误!", CBC_STR_E_MSG);
		return 0;
	}
	rxsResultReport* V = rxsReportGrps->ResultReportGet(rrName);
	if (V == 0) { return 0; }
	return V->RowCount();
}

const char* WorkProcessDal::ResultReportFiledValGet(const char* rrName, unsigned rowIndex, unsigned index) {
	if (rrName == 0) {
		SysLogRecord("输出报告字段取值失败，rrName为空错误!", CBC_STR_E_MSG);
		return 0;
	}
	rxsResultReport* V = rxsReportGrps->ResultReportGet(rrName);
	if (V == 0) { return 0; }
	return V->GetValue(rowIndex,index);
}

const char* WorkProcessDal::ResultReportFiledValGet(const char* rrName, unsigned rowIndex, const char* filedName) {
	if (rrName == 0 || filedName == 0) {
		return 0;
	}
	rxsResultReport* V = rxsReportGrps->ResultReportGet(rrName);
	if (V == 0) { return 0; }
	return V->GetValue(rowIndex, filedName);
}

const char* WorkProcessDal::ToolKitForech(unsigned index) {
	unsigned Num = 0;
	std::string* A = rxsTk.ToolsNameGet(Num);
	if (index < Num)
		return A[index].c_str();

	return 0;
}


bool WorkProcessDal::RoiMangerAreaSet(roiGrpMangerStep* rgms, int index, int sx, int sy, int w, int h) {
	if (rgms == 0) {
		return false;
	}
	roiRect rrt = { 1 << 1,{sx,sy,0.0},{w,h,0.0},{w,h,0.0},"", std::to_string(index) + rgms->Name() };
	rgms->roiAreaSet(rrt);
	return true;
}


void WorkProcessDal::RoiAreaSet(const char* roiName, u8 flag, rxsPointCouldp tps[3], rxsPointCouldp* pdEnt, unsigned nums) {
	if (roiName == 0) {
		SysLogRecord("ROI区域设置失败，ROI名称参数为NULL！");
		return;
	}
	RoiManger.Add(roiName, tps, pdEnt, nums, flag);
}

void WorkProcessDal::RoiReomve(const char* roiName) {
	if (roiName == 0) {
		SysLogRecord("ROI区域Remove失败，ROI名称参数为NULL！");
		return;
	}
	RoiManger.Remove(roiName);
}

void WorkProcessDal::RoiClr() {
	RoiManger.Clear();
}

roiRect WorkProcessDal::RoiConvertAbstoref(int absRefIndex, int srcIndex) {
	//roiRect ref = RoiManger.Get(absRefIndex)->Area;
	//roiRect src = RoiManger.Get(srcIndex)->Area;
	roiRect out = { 0,{0,0,0},{0,0,0},{0,0,0},"","" };
	//short ret = RoiConvertAbsToRel(ref, src, out);
	return out;
}

const char* WorkProcessDal::RoiAreaForech(unsigned index, unsigned& id, u8& Type, rxsPointCouldp bp[3]) {
	int l = RoiManger.Count();
	if (index < l) {
		//RoiArea* ra = RoiManger.Get(index);
		//id = ra->id;
		//Type = ra->Area.Flag;
		//bp[0] = ra->Area.SORC;
		//bp[1] = ra->Area.End;
		//bp[2] = ra->Area.LWH;
		//return ra->Area.rName.c_str();
	}
	return 0;
}

void WorkProcessDal::InMatterPos_Move() {
	while (!MotionGpio->IsMotionFinsh("AOI-Z"))
	{
		if (sysStatus.CurFixStatus() != SystemInfo::FixStaus::RUNING) {
			return;
		}
		_sleep(MOTIONINPDMS);
	}
	WrkParam->Selected(2);
	Kvpdchr val;
	//if (WrkParam->DataReaderKvpd("装料位置", val) && val.val != 0) {
	if (WrkParam->DataReaderKvpd(3, val) && val.val != 0 && val.val[0] != 0) {
		std::string vf = val.val;
		if (vf.length() < 1) { return; }
		float f = std::stof(vf);
		MotionGpio->AxMove("AOI-Y", 'A', 0, f);
	}
}

void WorkProcessDal::OutMatterPos_Move() {
	while (!MotionGpio->IsMotionFinsh("AOI-Z"))
	{
		if (sysStatus.CurFixStatus() != SystemInfo::FixStaus::RUNING) {
			return;
		}
		_sleep(MOTIONINPDMS);
	}
	WrkParam->Selected(2);
	Kvpdchr val;
	//if (WrkParam->DataReaderKvpd("出料位置", val) && val.val != 0) {
	if (WrkParam->DataReaderKvpd(4, val) && val.val != 0 && val.val[0] != 0) {
		std::string vf = val.val;
		if (vf.length() < 1) { return; }
		float f = std::stof(vf);
		MotionGpio->AxMove("AOI-Y", 'A', 0, f);
	}
}

void WorkProcessDal::NoCalculationTwo(bool isOn) {
	if (isOn) {
		IgnoreFlagSet(TD_CAL);
	}
	else {
		IgnoreFlagClr(TD_CAL);
	}
}


//光源控制
void WorkProcessDal::LightOpen(int Cno, CST_LightSource::ColorIndex ci) {
	if (Cno < 0) {
		CST_LightSource::ColorIndex cis[4] = { ci,ci,ci,ci };
		lightSource.brightnessONOFF(cis);

		for (int x = 0; x < tzg.Cnum; x++) {
			//Cno 0...X对应 A....H
			tzg.brightnessDSet(x + 65, 255);
		}
	}
	else {
		CST_LightSource::Partition* A = lightSource.PartitionStatus();
		CST_LightSource::ColorIndex cis[4] = {
			A[0].isOn ? A[0].curCi : CST_LightSource::None,
			A[1].isOn ? A[1].curCi : CST_LightSource::None,
			A[2].isOn ? A[2].curCi : CST_LightSource::None,
			A[3].isOn ? A[3].curCi : CST_LightSource::None,
		};
		cis[Cno] = ci;
		lightSource.brightnessONOFF(cis);

		//Cno 0...X对应 A....H
		tzg.brightnessDSet(Cno + 65, 255);
	}
}

void WorkProcessDal::LightClose(int Cno, CST_LightSource::ColorIndex ci) {
	if (Cno < 0) {
		CST_LightSource::ColorIndex cis[4] = { CST_LightSource::None,CST_LightSource::None,CST_LightSource::None,CST_LightSource::None };
		lightSource.brightnessONOFF(cis);

		for (int x = 0; x < tzg.Cnum; x++) {
			tzg.brightnessDSet(x + 65, 0);
		}

	}
	else {
		CST_LightSource::Partition* A = lightSource.PartitionStatus();
		CST_LightSource::ColorIndex cis[4] = {
			A[0].isOn ? A[0].curCi : CST_LightSource::None,
			A[1].isOn ? A[1].curCi : CST_LightSource::None,
			A[2].isOn ? A[2].curCi : CST_LightSource::None,
			A[3].isOn ? A[3].curCi : CST_LightSource::None,
		};
		cis[Cno] = CST_LightSource::None;
		lightSource.brightnessONOFF(cis);

		tzg.brightnessDSet(Cno + 65, 0);
	}
}

void WorkProcessDal::LightRegulate(int Cno, CST_LightSource::ColorIndex ci, u8 value) {
	if (Cno < 0) {
		for (short x = 0; x < 4; x++) {
			lightSource.brightnessSet(x, ci, value);

			tzg.brightnessDSet(x + 65, value);
		}
	}
	else {
		CST_LightSource::Partition* A = lightSource.PartitionStatus();
		if (A[Cno].isOn) {
			if (A[Cno].curCi == ci) {
				lightSource.brightnessSet(Cno, ci, value);
			}
			else {//与当前不一致 要切换

			}
		}
		tzg.brightnessDSet(Cno + 65, value);
	}
}

int WorkProcessDal::AutoLightIntensityTakephoto(const char* vals, CST_LightSource::ColorIndex ci) {
	ImageableSensorUnit* curisu = iabGather->Selected("HkCream");
	kvpdChr* val = FacParam->KvpRead("数据保存");
	string path = "C:\\Users\\Administrator\\Desktop\\";
	if (val->val != 0) { path = val->val; }
	LightOpen(-1, ci);
	std::vector<string> rs = stringSplit(vals, ',');
	for (int i = 0; i < rs.size(); i++) {
		int v = atoi(rs[i].c_str());
		LightRegulate(-1, ci, v);
		_sleep(LS_DMS_CREAM);
		rxsImg spic = { 0,0,0,0,0 };
		curisu->Grab(false, true);
		spic.Data = curisu->DataGet(spic.Len);
		spic.cols = curisu->CtrInfo[0].Width;
		spic.rows = curisu->CtrInfo[0].Height;
		spic.Channels = 1;

		imgprocToolbase itb(&spic);
		stringstream sname;
		sname << path << i << "_" << v << ".bmp";
		itb.Save(sname.str().c_str());
	}
	return 0;
}


int WorkProcessDal::AutoSevenLightColorTakephoto(u8 liv) {
	CST_LightSource::ColorIndex ci[4] = {
	CST_LightSource::ColorIndex::None,
	CST_LightSource::ColorIndex::None,
	CST_LightSource::ColorIndex::None,
	CST_LightSource::ColorIndex::None
	};
	ImageableSensorUnit* curisu = iabGather->Selected("HkCream");
	kvpdChr* val = FacParam->KvpRead("数据保存");
	string path = "C:\\Users\\Administrator\\Desktop\\";
	if (val->val != 0) { path = val->val; }
	lightSource.brightnessONOFF(ci);
	for (int x = 0; x < lightSource.ColorNum; x++) {
		memset(ci, (x + 1), sizeof(CST_LightSource::ColorIndex) * 4);
		lightSource.brightnessONOFF(ci);
		LightRegulate(-1, CST_LightSource::ColorIndex(x + 1), liv);
		_sleep(LS_DMS_CREAM);
		rxsImg spic = { 0,0,0,0,0 };
		curisu->Grab(false, true);
		spic.Data = curisu->DataGet(spic.Len);
		spic.cols = curisu->CtrInfo[0].Width;
		spic.rows = curisu->CtrInfo[0].Height;
		spic.Channels = 1;

		imgprocToolbase itb(&spic);
		stringstream sname;
		sname << path << x << "_" << ".bmp";
		itb.Save(sname.str().c_str());
	}
	return 0;
}

int WorkProcessDal::AutoAllLightTakephoto(int starti) {
	CST_LightSource::ColorIndex off[4] = {
		CST_LightSource::ColorIndex::None,
		CST_LightSource::ColorIndex::None,
		CST_LightSource::ColorIndex::None,
		CST_LightSource::ColorIndex::None
	};
	unsigned pscount = 0;
	ImageableSensorUnit* curisu = iabGather->Selected("HkCream");
	kvpdChr* val = FacParam->KvpRead("数据保存");
	string path = "C:\\Users\\Administrator\\Desktop\\";
	if (val->val != 0) { path = val->val; }

	int mod = 0;
	int cip = 0;
	std::vector<int> v;
	int i = starti;
	lightSource.brightnessONOFF(off);
	while (i < lightSource.ColorNum * lightSource.ColorNum * lightSource.ColorNum * lightSource.ColorNum) {

		v.clear();
		int nv = i;
		while (true) {
			mod = nv % lightSource.ColorNum;
			cip = nv / lightSource.ColorNum;
			v.push_back(mod);
			if (cip == 0) {
				break;
			}
			nv = cip;

			if (v.size() > 3) {
				break;
			}

		}
		CST_LightSource::ColorIndex cis[4] = {
			CST_LightSource::ColorIndex::None,
			CST_LightSource::ColorIndex::None,
			CST_LightSource::ColorIndex::None,
			CST_LightSource::ColorIndex::None
		};

		for (int civs = 0; civs < v.size(); civs++) {
			cis[civs] = (CST_LightSource::ColorIndex)v[civs];
		}

		lightSource.brightnessONOFF(cis);

		_sleep(LS_DMS_CREAM);
		rxsImg spic = { 0,0,0,0,0 };
		curisu->Grab(false, true);
		spic.Data = curisu->DataGet(spic.Len);
		spic.cols = curisu->CtrInfo[0].Width;
		spic.rows = curisu->CtrInfo[0].Height;
		spic.Channels = 1;

		imgprocToolbase itb(&spic);
		stringstream sname;
		sname << path << i << "_" << cis[3] << "-" << cis[2] << "-" << cis[1] << "-" << cis[0] << ".bmp";
		itb.Save(sname.str().c_str());
		i += 1;
	}
	return i;
}

void WorkProcessDal::LightInfoGet(short& Anum, u8* valss) {
	u8 v = 0;
	for (short x = 0; x < 4; x++) {
		v = lightSource.brightnessGet(x);
	}
}


const char* WorkProcessDal::imgToolsForeach(bool& isStrat) {
	if (rxsImgTPC == 0) { return 0; }
	return rxsImgTPC->Foreach(isStrat);
}

short WorkProcessDal::imgProcessTester(const char* tkName) {
	string val = (*FacParam)["2D相机"];
	if (val.length() == 0) {
		val = "设备参数:【2D相机】未设置有效值!";
		SysLogRecord(val.c_str(), CBC_STR_E_MSG);
		return -11;
	}
	ImageableSensorUnit* isu = imgableCgather->Selected(val.c_str());
	if (rxsImgTPC->Selected(tkName) == 0) {
		val = tkName;
		val += ":获取失败!为NULL错误!";
		SysLogRecord(val.c_str(), CBC_STR_E_MSG);
		return -12;
	}
	rxsImgTPC->Run();
	return 0;
}

short WorkProcessDal::ImgProcessFlowerSave(const char* dirPath) {
	if (dirPath == 0) {
		string val = (*FacParam)["可视流程"];
		if (val.length() == 0) {
			val = "设备参数:【可视流程】未设置有效值,将采用默认值!";
			SysLogRecord(val.c_str());
			val = "E:\\myImgFlower\\Save\\";
		}
		dirPath = val.c_str();

	}
	else {
		FacParam->KvpUpdater("可视流程", dirPath, 0);
	}
	return rxsImgTPC->Save(dirPath);
}

short WorkProcessDal::ImgProcessFlowerLoad(const char* dirPath) {
	if (dirPath == 0) {
		string val = (*FacParam)["可视流程"];
		if (val.length() == 0) {
			val = "设备参数:【可视流程】未设置有效值,将采用默认值!";
			SysLogRecord(val.c_str());
			val = "E:\\myImgFlower\\";
		}
		dirPath = val.c_str();
		return rxsImgTPC->Load(dirPath);
	}
	FacParam->KvpUpdater("可视流程", dirPath, 0);
	return rxsImgTPC->Load(dirPath);
}

void WorkProcessDal::modelConvert(const char* inpcd, const char* outpcd) {
	rxsTk.initModelNake(inpcd, outpcd);
}


void WorkProcessDal::oneKeyModelTrain(std::string path) {
	if (path.length() < 1) {

	}

	std::vector<std::vector<float>> X;
	std::vector<float> Y;
	rxsTk.trainModel(X, Y);
}

bool WorkProcessDal::AlgorithmTester(std::string aName, std::string params) {

	return true;
}

void WorkProcessDal::SynchronousRoiToFlower(const char* curSelFlower) {
	imgTaskFlowerProcesser* tfp = (*rxsImgTPC)[curSelFlower];
	if (tfp == 0) {
		return;
	}
	roiGrpMangerStep* rgms = (roiGrpMangerStep*)tfp->StepTypeFind(StepBlockBase::ROIGRPMANGER);
	roiMangeParam* rmp = rgms->ParamGet<roiMangeParam>();

	PointCouldDeformationStep* pcds = (PointCouldDeformationStep*)tfp->StepTypeFind(StepBlockBase::PC_DEFORMATION);
	PointCouldDeformationParam* pcdp = pcds->ParamGet<PointCouldDeformationParam>();

	unsigned index = 0;
	RoiArea* ri = 0;
	while ((ri = RoiManger.Get(index)) != 0) {
		char l_or_r = ri->Area.rName.at(0);
		if (l_or_r == 'L') {
			if (ri->Area.rName.find("model") == string::npos) {
				*(rmp->roiArea) = ri->Area;
			}
			else {
				pcdp->ModeBound[0] = { 0,0,0 };
			}
		}
		else if (l_or_r == 'R') {
			if (ri->Area.rName.find("model") == string::npos) {
				*(rmp->roiArea) = ri->Area;
			}
			else {
				pcdp->ModeBound[1] = { 0,0,0 };
			}
		}
		index += 1;
	}
	RoiManger.Clear();
}

void WorkProcessDal::GetLineContourMatchingPC(const char* pcFileName, int mpIndex) {
	WrkParam->Selected(3);
	Movepath* mp = (Movepath*)WrkParam->DataReaderMsp(mpIndex);
	if (mp == 0) {
		SysLogRecord("请确认加工程式已经正确加载，未能能从ID取到对应的加工路径内容！", CBC_STR_E_MSG);
		return;
	}
	kvpdChr* val = FacParam->KvpRead("数据保存");
	string path = "C:\\Users\\Administrator\\Desktop\\";
	if (val->val != 0) { path = val->val; }
	std::string fpp = path + pcFileName;

	imgTaskFlowerProcesser* tfp = (*rxsImgTPC)[mp->Flower];
	roiGrpMangerStep* rgms = (roiGrpMangerStep*)tfp->StepTypeFind(StepBlockBase::ROIGRPMANGER);
	CloudEntity ce = rxsTk.pcdReader(fpp);
	rgms->PushIn(ce);
	free(ce.Entiy);
	tfp->Ready();
	tfp->Run();
}

bool WorkProcessDal::roiPcFindPoint(u8 methodType, CloudEntity& ce, rxsPointCouldp& outPoint) {
	if (methodType == 'L') {
		outPoint = rxsTk.roiPC_LRPointGet(ce, methodType);
		return true;
	}
	if (methodType == 'R') {
		outPoint = rxsTk.roiPC_LRPointGet(ce, methodType);
		return true;
	}
	return false;
}


int WorkProcessDal::indexFindFrom(CloudEntity& ce, rxsPointCouldp tagePoint) {
	int rIndex = -1;
	if (ce.pNums > 0) {
		float rDif = -1.0f;
		float zDif = 0.0f;
		for (int x = 0; x < ce.pNums; x++) {
			zDif = ce.Entiy[x].x - tagePoint.x;
			if (std::abs(zDif) < std::abs(rDif)) {
				rDif = zDif;
				rIndex = x;
			}
		}
	}
	return rIndex;
}

void WorkProcessDal::fileCopy(const char* from, const char* toTage) {
	ifstream ifs(from, ifstream::in | ios::binary);
	if (!ifs.good())
	{
		cout << "文件不存在，请重试 " << from << endl;
		return;
	}
	ofstream ofs(toTage, ios::trunc | ios::binary);
	if (!ofs.good())
	{
		cout << "目标文件无权限或打开失败 " << endl;
		return;
	}

	ifs.seekg(0, ifs.end);
	int length = ifs.tellg();
	cout << length << endl;
	ifs.seekg(0, ifs.beg);
	// 3. 创建内存缓存区
	char* buffer = new char[length];
	// 4. 读取图片
	ifs.read(buffer, length);
	ofs.write(buffer, ifs.gcount());
	delete[] buffer;
	ifs.close();
	ofs.close();
}

void WorkProcessDal::twoDGetDataFromPic() {
	LocServerMsgClr();
	auto func = [=]() {
		SysLogRecord("进入自动跑图测试!");
		std::vector<std::string> flodps;
		intptr_t   hFile = 0;
		struct _finddata_t fileinfo;
		std::string srcpath = "E:\\PicBkup\\";
		if ((hFile = _findfirst((srcpath + ("\\*")).c_str(), &fileinfo)) != -1)
		{
			do {
				if ((fileinfo.attrib & _A_SUBDIR))
				{
					if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {

						flodps.push_back(srcpath + fileinfo.name);
					}
				}
			} while (_findnext(hFile, &fileinfo) == 0);
			_findclose(hFile);
		}
		if (flodps.size() < 1) {
			SysLogRecord("自动跑图-未找到相关文件夹");
			return;
		}
		std::string recvfp;
		for (int x = 0; x < flodps.size(); x++) {
			rxsReportGrps->ResultReportContentClr();
			startTimep = std::chrono::system_clock::now();
			recvfp = flodps[x];
			string p;
			int l = 0;
			if ((hFile = _findfirst(p.assign(recvfp).append("\\*").c_str(), &fileinfo)) != -1)
			{
				do {
					if ((fileinfo.attrib & _A_SUBDIR) || (fileinfo.attrib & _A_SYSTEM) || (fileinfo.attrib & _A_HIDDEN))
					{
						continue;
					}
					else
					{
						string fp = recvfp;
						string tage = pSystem->AppPath();
						fp = fp.append("\\").append(fileinfo.name);
						tage = tage.append("\\");
						tage.append(fileinfo.name);
						fileCopy(fp.c_str(), tage.c_str());
					}
					l++;
				} while (_findnext(hFile, &fileinfo) == 0);
				_findclose(hFile);
			}

			runRecordp.MmpRecordIndex = 0;
			runRecordp.ScpRecordIndex = 0;
			runRecordp.EndGatherInFac = true;
			sysStatus.FixStatusSet(SystemInfo::FixStaus::RUNING);
			WrkParam->Selected(3);
			Movepath* mp = 0;
			for (; (mp = (Movepath*)WrkParam->DataReaderMsp(runRecordp.MmpRecordIndex)) != 0; runRecordp.MmpRecordIndex++) {
				imgTaskFlowerProcesser* tfp = (*rxsImgTPC)[mp->Flower];
				tfp->Ready();
				for (int m = 0; m < tfp->StepBlockCount(); m++) {
					StepBlockBase* sbb = tfp->StepBlockGet(m);
					if (sbb == 0) {
						continue;
					}
					if (sbb->blkType() == StepBlockBase::SBTYPE::THRD_PROCESS || sbb->blkType() == StepBlockBase::SBTYPE::TCP_SERVERRECV) {
						sbb->Run();
						_sleep(50);
					}
				}

				while (!rxsImgTPC->HadFinshed() || !imgableCgather->isCompeleted()) {
					if (sysStatus.CurFixStatus() != SystemInfo::FixStaus::RUNING) {
						SysLogRecord("加工停止，自动跑图任务退出..!", CBC_STR_W_MSG);
						return;
					}
					_sleep(200);
				}
				WrkParam->Selected(3);
			}
			pSystem->FrontProcessMsgSender(CBC_RELOADPAGE, 0);
			if (!HasIgnoreStatus(CHKRR_SAVE)) {
				TcpServerRepResultGet(runRecordp.MmpRecordIndex);
				SaveCheckResultReport();
			}
			pSystem->WorkStop(false);
			pSystem->FrontProcessMsgSender(CBC_WRKSTOP, 0);
			_sleep(500);
		}
		SysLogRecord("自动跑图测试-全部执行完毕!");
	};
	std::thread thr(func);
	thr.detach();
}


void WorkProcessDal::virtualRunTester() {
	LocServerMsgClr();
	kvpdChr* val = FacParam->KvpRead("数据保存");
	string path = "C:\\Users\\Administrator\\Desktop\\";
	if (val->val != 0) { path = val->val; }
	auto func = [=]() {
		runRecordp.MmpRecordIndex = 0;
		runRecordp.ScpRecordIndex = 0;
		runRecordp.EndGatherInFac = true;
		sysStatus.FixStatusSet(SystemInfo::FixStaus::RUNING);
		WrkParam->Selected(3);
		Movepath* mp = 0;
		std::string fnp;
		for (; (mp = (Movepath*)WrkParam->DataReaderMsp(runRecordp.MmpRecordIndex)) != 0; runRecordp.MmpRecordIndex++) {
			bool isok = false;
			imgTaskFlowerProcesser* tfp = (*rxsImgTPC)[mp->Flower];
			tfp->Ready();
			for (int m = 0; m < tfp->StepBlockCount(); m++) {
				StepBlockBase* sbb = tfp->StepBlockGet(m);
				if (sbb == 0) {
					continue;
				}
				if ((sbb->blkType() == StepBlockBase::SBTYPE::PC_IN_SOURCE) && (sbb->Name() == std::string("输入源"))) {
					fnp = (path + mp->Flower) + "_" + std::to_string(sbb->ID()) + "_.pcd";
					CloudEntity ce = rxsTk.pcdReader(fnp);
					((PointCouldSourceStep*)sbb)->PushIn(ce);
					isok = true;
					free(ce.Entiy);
					_sleep(50);
				}
			}
			if (isok) {
				tfp->Run();
			}
			while (!rxsImgTPC->HadFinshed() || !imgableCgather->isCompeleted()) {
				if (sysStatus.CurFixStatus() != SystemInfo::FixStaus::RUNING) {
					SysLogRecord("加工停止，虚拟运行结束..!", CBC_STR_W_MSG);
					return;
				}
				_sleep(200);
			}
			WrkParam->Selected(3);
		}
		pSystem->FrontProcessMsgSender(CBC_RELOADPAGE, 0);
		if (!HasIgnoreStatus(CHKRR_SAVE)) {			
			TcpServerRepResultGet(runRecordp.MmpRecordIndex);
			SaveCheckResultReport();
		}
		pSystem->WorkStop(false);
		pSystem->FrontProcessMsgSender(CBC_WRKSTOP, 0);
		_sleep(500);
	};
	std::thread thr(func);
	thr.detach();
}


void WorkProcessDal::TcpServerRepResultGet(int mNus) {
	Kvpdchr Val;
	WrkParam->Selected(1);
	WrkParam->DataReaderKvpd(5, Val);
	if (Val.val == 0 ) {
		SysLogRecord("Tcp服务报告获取客户端列表未配置内容!", CBC_STR_N_MSG);
		return;
	}
	if (strlen(Val.val) < 8) {
		SysLogRecord("Tcp服务报告获取客户端列表未配置-IP不正确!", CBC_STR_E_MSG);
		return;
	}
	std::vector<std::string> rrvs;
	const char* ctxt = 0;
	std::vector<std::string> line;
	SysLogRecord("tcpServer结果获取中....", CBC_STR_N_MSG);
	int tms = 200;
	kvpdChr* val = FacParam->KvpRead("TCP接收超时");
	if (val->val != 0) { tms = atoi(val->val) / 100; }
	int releNums = 0;
	rxsResultReport* mainrr = rxsReportGrps->ResultReportGet(0);
	while (true) {
		_sleep(100);
		ctxt = LocServerReceive(Val.val, -1);
		if (ctxt == 0) {
			//是否超时判断
			if (tms == 0) {
				SysLogRecord("tcpServer结果获取超时退出", CBC_STR_E_MSG);
				return;
			}
			tms -= 1;
			continue;
		}
		line = stringSplit(ctxt, '\n');
		if ((line.size() < mNus)) {
			//是否超时判断
			if (tms == 0) {
				SysLogRecord("tcpServer结果获取超时退出", CBC_STR_E_MSG);
				return;
			}
			line.clear();
			tms -= 1;
			continue;
		}
		for (int x = 0; x < line.size(); x++) {
			std::vector<std::string> act = stringSplit(line[x], ',');
			releNums = act.size();
			rrvs.insert(rrvs.end(), act.begin(), act.end());
			if (sysStatus.CurFixStatus() != SystemInfo::FixStaus::RUNING) {
				SysLogRecord("加工停止,tcpServer结果获取终止退出..!", CBC_STR_W_MSG);
				return;
			}
		}
		break;
	}
	if (rrvs.size() > 0) {

		rxsResultReport nrr;
		std::vector<std::string> sv;
		std::string fnams[3] = { "NO","名","值" };
		nrr.SetFiled(fnams, 3, "TCP结果");
		int r = 0;
		for (int y = 0; y < rrvs.size(); y++) {
			sv = stringSplit(rrvs[y], ':');
			r = nrr.AddRow() - 1;
			nrr.SetValue("NO", std::to_string(r), r);
			nrr.SetValue("名", sv[0], r);
			nrr.SetValue("值", sv[1], r);

			if (y % releNums == 0) {
				mainrr->SetValue("内P长宽", sv[1], y / releNums);
				std::string kid = sv[0].substr(5);
				std::string fname = sv[0].substr(7) + "_0_";
				mainrr->SetValue("编号", kid.c_str(), y / releNums);
				mainrr->SetValue("文件名", fname.c_str(), y / releNums);
			}
			if (y % releNums == 1)
			{
				mainrr->SetValue("变形度", sv[1], y / releNums);
			}
			if (y % releNums == 2) {
				mainrr->SetValue("锋锐度", sv[1], y / releNums);
			}
			if (y % releNums == 3) {
				mainrr->SetValue("预测长", sv[1], y / releNums);
			}
		}
		rxsReportGrps->ResultReportPush(nrr);
		SysLogRecord("tcpServer结果报表转换完成", CBC_STR_N_MSG);
		return;
	}
	SysLogRecord("tcpServer获取结果长度为0", CBC_STR_W_MSG);
}

void WorkProcessDal::ShowOpenDir(const char* fp) {
	std::stringstream ss;
	std::string ps = "explorer /select,";
	ss << ps << fp;
	system(ss.str().c_str());
}

bool SearchFile(std::string path, std::string sfn, std::string& outfpn) {
	intptr_t   hFile = 0;
	struct _finddata_t fileinfo;
	int l = 0;
	std::string searchtage = path;
	if ((hFile = _findfirst((searchtage.append("*").append(sfn)).c_str(), &fileinfo)) != -1)
	{
		do {
			if ((fileinfo.attrib & _A_SUBDIR) || (fileinfo.attrib & _A_SYSTEM) || (fileinfo.attrib & _A_HIDDEN))
			{
				continue;
			}
			else
			{
				outfpn = path.append(fileinfo.name);
				return true;
			}
			l++;
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	return false;
}

void WorkProcessDal::PointCould_DeformTecher(u8 wich, int id, u8 dir, const char* flrn) {

	if (dir > 3) {
		SysLogRecord("对应路径【Type】模式错误-请先修正,或当前为非3D内容!", CBC_STR_W_MSG);
		return;
	}
	dir -= 1;
	string idimap[3] = { "X","Y","XY" };
	std::stringstream cmd;
	std::stringstream ss;
	if (wich == 0) {
		Kvpdchr fnval;
		WrkParam->Selected(1);
		WrkParam->DataReaderKvpd(6, fnval);
		if (fnval.val == 0) {
			SysLogRecord("模型示教未配置模型路径设定!", CBC_STR_W_MSG);
			return;
		}
		ss << id << idimap[dir] << flrn;
		std::string modn = ss.str();
		cmd << "techDeform,model_" << modn << "," << fnval.val << modn << ".pcd";
	}
	else if (wich == 1) {
		kvpdChr* val = FacParam->KvpRead("数据保存");
		if (val == 0 || val->val == 0) {
			SysLogRecord("点云示教未配置扫描保存路径，请先设定!", CBC_STR_W_MSG);
			return;
		}
		std::string tfn;
		std::string sf = flrn;
		sf += "*";
		sf += ".pcd";
		if (!SearchFile(val->val, sf.c_str(), tfn)) {
			SysLogRecord("目标点云目录下无文件，请先扫描采集!", CBC_STR_W_MSG);
			return;
		}
		string dirMap[3] = { "_X","_Y","_XY" };
		ss << id << idimap[dir] << flrn;
		cmd << "techDeform," << ss.str() << "," << tfn;
	}
	else {
		SysLogRecord("无效的示教选项!", CBC_STR_E_MSG);
		return;
	}
	std::string cmdstr = cmd.str();
	if (cmdstr.length() > 0) {
		LocServerSend(0, cmdstr.c_str(), cmdstr.length());
	}
}


void WorkProcessDal::PointCould_MatchShow(const char* id, u8 dir, const char* fn) {
	if (fn == 0) {
		SysLogRecord("文件设定为空，请确认报表中对应输出字段存在!", CBC_STR_W_MSG);
		return;
	}
	kvpdChr* val = FacParam->KvpRead("数据保存");
	if (val == 0 || val->val == 0) {
		SysLogRecord("点云示教未配置扫描保存路径，请先设定!", CBC_STR_W_MSG);
		return;
	}
	std::stringstream ss;
	ss << "validDeform," << id << "," << val->val << fn << ".pcd";
	std::string cmdstr = ss.str();
	if (cmdstr.length() > 0) {
		LocServerSend(0, cmdstr.c_str(), cmdstr.length());
	}
}

void* WorkProcessDal::HrdWareObjectGet(int index) {
	switch (index)
	{
	case 0:
		return &adjF;
	case 1:
		return &tzg;
	case 2:
		return &lightSource;
	case 10:
		return rxsTK;
	default:
		break;
	}
	SysLogRecord("无才对应的硬件索引，请确认调用索引正确!");
	return 0;
}

