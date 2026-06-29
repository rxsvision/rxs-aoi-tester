
#include "rxsFixdal.h"
#include <sstream>
#include "../rxsImgProcess/imgProcess.h"
#include "protected.h"
#include <thread>
#include "protected_rxsComputerTransferManger.h"

using namespace std;
#define TMSOUTPOINTS			100000000

#define ResultReportManger			(*(rxsResultReportGrpsManger*)resultCollection)


CompoundGather::CompoundGather() {
	dir = 0xFF;
	refCount = 0;
	memset(&crcd, 0, sizeof(Crcdm));
	resultCollection = 0;
	VariableAssignment();
	isNVFC = true;
}

void CompoundGather::ResultReportGrpSet(void* ras) {
	resultCollection = ras;
}

bool CompoundGather::isCompeleted() {
	return (refCount == 0);
}

bool CompoundGather::creteSensorDal(const char* key) {
	string val = key;
	if (val == "Keyence")
	{
		const char* val1 = (*FacParam)["激光参数1"];
		const char* val2 = (*FacParam)["激光参数2"];
		std::stringstream ss;
		ss << val1 << "," << val2;
		std::string S = ss.str();
		ImageableSensorUnit* imgableSensor = new KeyenceSpectrum;
		imgableSensor->Open((char*)S.c_str());
		PushImageableGrp(val.c_str(), imgableSensor);
	}
	else if (val == "Precitec")
	{
		const char* val1 = (*FacParam)["PrecitecIp"];
		const char* val2 = (*FacParam)["PrecitecPort"];
		const char* val3 = (*FacParam)["PrecitecDevt"];//普雷斯特的设备型号
		const char* val4 = (*FacParam)["PrecitecProb"];//校验表
		const char* val5 = (*FacParam)["PrecitecSampHZ"];//采样频率
		std::stringstream ss;
		ss << val1 << "," << val2 << "," << val3 << "," << val4 << "," << val5;
		std::string S = ss.str();
		ImageableSensorUnit* imgableSensor = new Precitec;
		imgableSensor->Open((char*)S.c_str());
		PushImageableGrp(val.c_str(), imgableSensor);
	}
	else if (val == "KeyenceLineLsr") {

	}
	else if (val == "NVTLiner") {
		const char* val1 = (*FacParam)["翌视IP1"];
		//注意传入  顺序0-X轴 , 1 -Y轴
		//std::string S = ",";
		//S += val1;
		std::string S = val1;
		S += ",";

		ImageableSensorUnit* imgableSensor = new NVTLineLsr;
		imgableSensor->Open((char*)S.c_str());
		PushImageableGrp(val.c_str(), imgableSensor);
	}
	else if (val == "HkCream") {
		ImageableSensorUnit* imgableSensor = new HkCream_ch120;
		imgableSensor->Open(0);
		PushImageableGrp(val.c_str(), imgableSensor);
	}
	else if (val == "SSSR") {
		ImageableSensorUnit* imgableSensor = new SSZN_SR8060;
		std::string S = (*FacParam)["深视IP"];
		S += ",";
		S += (*FacParam)["深视配置"];
		imgableSensor->Open(S.c_str());
		PushImageableGrp(val.c_str(), imgableSensor);
	}
	else {
		val += ":未知的（暂未支持）传感器!";
		SysLogRecord(val.c_str());
		return false;
	}
	return true;
}

bool CompoundGather::SensorCreater() {
	string val = (*FacParam)["传感器"];
	if (val.length() == 0) {
		val += "设备参数:【传感器】未设置有效值!";
		SysLogRecord(val.c_str());
		return false;
	}
	bool ret = true;
	vector<string> sGrps = stringSplit(val, ',');
	for (size_t m = 0; m < sGrps.size(); m++) {
		if (!creteSensorDal(sGrps[m].c_str())) {
			ret = false;
		}
	}
	return ret;
}

bool CompoundGather::LightSourceCreater() {


	return true;
}

bool CompoundGather::LensCreater() {


	return true;
}



void CompoundGather::PointDataComputerAndSave(CompoundGather::Crcdm cdm, u8 dir) {
	rxsPointCouldp* srcPs = cdm.pointResult;
	rxsPointCouldp* rpc = new rxsPointCouldp[cdm.getPointNums];
	unsigned pns = cdm.getPointNums;
	double space = cdm.Len / cdm.getPointNums;
	int crID = cdm.ID;
	if (rpc != 0) {
		memset(rpc, 0, sizeof(rxsPointCouldp) * pns);
	}
	else {
		SysLogRecord("PointDataComputerAndSave异常-rpc内存分配失败!", CBC_STR_E_MSG);
		return;
	}
	WorkProcessDal* wrkpdr = ((WorkProcessDal*)(pSystem->Wrkpd));
	rxsPoint Spos = { cdm.Sx ,cdm.Sy };
	CalibrationPC_DataGet(rpc, srcPs, pns, space, dir, Spos);
	wrkpdr->rtkInParamSet(crID, dir, mp);
	CloudEntity ce = { rpc, pns };
	pSystem->FrontProcessMsgSender(10, &ce);
	imgTaskFlowerProcesser* tfp = (*wrkpdr->rxsImgTPC)[mp.Flower];
	PointCouldSourceStep* sbb = 0;
	WorkProcessDal* wrpd = WorkProcessDalGet();
	rxsToolKit* rtk = (rxsToolKit*)wrpd->HrdWareObjectGet(10);
	if (tfp == 0) {
		wrkpdr->runRecordp.EndGatherInFac = true;
		pSystem->FrontProcessMsgSender(CBC_STR_E_MSG, (void*)"当前流程集中，无此流程名，请设定对应的流程名！");
		goto curEnd;
	}
	if (tfp->Ready() != 0) {
		wrkpdr->runRecordp.EndGatherInFac = true;
		pSystem->FrontProcessMsgSender(CBC_STR_E_MSG, (void*)"流程执行准备失败，请查看状态!");
		goto curEnd;
	}
	if ((sbb = (PointCouldSourceStep*)tfp->StepBlockGet("输入源")) == 0) {
		wrkpdr->runRecordp.EndGatherInFac = true;
		pSystem->FrontProcessMsgSender(CBC_STR_E_MSG, (void*)"流程中无【输入源】块无法压入!");
		goto curEnd;
	}
	rtk->CouldCopy_ZtoXY(ce.Entiy, ce.pNums, (dir == 0));
	sbb->PushIn(ce);
	wrkpdr->runRecordp.EndGatherInFac = true;
	if (tfp->Run() != 0) {
		pSystem->FrontProcessMsgSender(CBC_STR_E_MSG, (void*)"流程执行失败，请注意参数配置是否正确!");
	}
	pSystem->FrontProcessMsgSender(12, 0, true);
curEnd:
	//需要等待前端执行反馈，是否已经显示完成，然后在释放点云内容
	delete[] rpc;
	rpc = 0;
}


bool CompoundGather::CalibrationPC_DataGet(rxsPointCouldp* rpc, rxsPointCouldp* srcPs, unsigned& PointNums, double space, u8 dir, rxsPoint spos) {
	if (rpc == 0 || srcPs == 0) {
		SysLogRecord("点云数据转换失败,必要参数为空NULL!");
		return false;
	}
	bool inv = (space < 0.00f);
	const char* ps = (*FacParam)["点间距"];
	if (ps != 0) {
		double pps = std::atof(ps);
		if ((int)(pps * 10000) == 0) {
			space = 0.008f;
		}
		else {
			space = pps;
		}
	}
	else {
		space = 0.008f;
	}
	if (inv) {
		space = -space;
	}
	GatherDataFilterConvert(rpc, (rxsPointCouldp*)srcPs, PointNums, space, dir, spos);
	return true;
}

void CompoundGather::LineDataComputerAndSave(CompoundGather::Crcdm cdm, u8 dir) {
	rxsPointCouldp* srcPs = new rxsPointCouldp[cdm.getPointNums];
	//rxsComputerTransfer* ctp = 0;
	unsigned pns = cdm.getPointNums;
	//double space = cdm.Len / cdm.getPointNums;
	int crID = cdm.ID;
	if (srcPs != 0)
		memcpy(srcPs, cdm.pointResult, sizeof(rxsPointCouldp) * cdm.getPointNums);

	WorkProcessDal* wrkpdr = ((WorkProcessDal*)(pSystem->Wrkpd));
	wrkpdr->rtkInParamSet(crID, dir, mp);
	CloudEntity ce = { srcPs, pns };
	pSystem->FrontProcessMsgSender(10, &ce);
	imgTaskFlowerProcesser* tfp = (*wrkpdr->rxsImgTPC)[mp.Flower];
	StepBlockBase* sbb = 0;
	if (tfp == 0) {
		wrkpdr->runRecordp.EndGatherInFac = true;
		pSystem->FrontProcessMsgSender(CBC_STR_E_MSG, (void*)"当前流程集中，无此流程名，请设定对应的流程名！");
		goto curEnd;
	}
	if (tfp->Ready() != 0) {
		wrkpdr->runRecordp.EndGatherInFac = true;
		pSystem->FrontProcessMsgSender(CBC_STR_E_MSG, (void*)"流程执行准备失败，请查看状态!");
		goto curEnd;
	}
	if ((sbb = tfp->StepBlockGet("输入源")) == 0) {
		wrkpdr->runRecordp.EndGatherInFac = true;
		pSystem->FrontProcessMsgSender(CBC_STR_E_MSG, (void*)"流程中无【输入源】块无法压入!");
		goto curEnd;
	}
	sbb->PushIn(ce);
	wrkpdr->runRecordp.EndGatherInFac = true;
	if (tfp->Run() != 0) {
		pSystem->FrontProcessMsgSender(CBC_STR_E_MSG, (void*)"流程执行失败，请注意参数配置是否正确!");
	}
	pSystem->FrontProcessMsgSender(12, 0, true);
curEnd:
	//需要等待前端执行反馈，是否已经显示完成，然后在释放点云内容
	delete[] srcPs;
	srcPs = 0;
}

void CompoundGather::runGatherDataCalculatingAndSave(CompoundGather::Crcdm cdm, u8 dir) {
	refCount += 1;
	ImageableSensorUnit* curOpter = CurSelectedGet();
	if (curOpter->Sensorinfo.devType == ImageableSensorUnit::DevType::LineConfocal ||
		curOpter->Sensorinfo.devType == ImageableSensorUnit::DevType::LineLsr) {
		//线激光/线光谱的处理
		LineDataComputerAndSave(cdm, dir);
	}
	else if (curOpter->Sensorinfo.devType == ImageableSensorUnit::DevType::PointLsr ||
		curOpter->Sensorinfo.devType == ImageableSensorUnit::DevType::Confocal) {
		//点激光/点光谱的处理
		PointDataComputerAndSave(cdm, dir);
	}
	else if (curOpter->Sensorinfo.devType == ImageableSensorUnit::DevType::Cream) {
		//相机
		//CreamDataComputerAndSave(curOpter, cdm, dir);
	}
	refCount -= 1;
}




short CompoundGather::EncTiggerTaber(ImageableSensorUnit* isu, u8 dir) {
	kvpdChr* v = 0;
	v = FacParam->KvpRead("触发模式");
	if (v != 0 && v->val != 0) {
		isu->CtrInfo[0].offsetLine = atoi(v->val);
	}
	else {
		SysLogRecord("设备参数-触发模式-值获取错误!", CBC_STR_E_MSG);
		return -11;
	}
	v = FacParam->KvpRead("触发间隔");
	if (v != 0 && v->val != 0) {
		isu->CtrInfo[0].lines = atoi(v->val);
	}
	else {
		SysLogRecord("设备参数-触发间隔-值获取错误!", CBC_STR_E_MSG);
		return -12;
	}
	if (dir == 0) {
		//X方向编码器触发切换
		MotionGpio->SetDoutput("编码器方向", true);
		isu->TiggerTaber(3);
		return 0;
	}
	if (dir == 1) {
		//Y方向编码器触发切换
		MotionGpio->SetDoutput("编码器方向", false);
		isu->TiggerTaber(4);
		return 0;
	}
	return -10;
}

void CompoundGather::ReadyParamSet(void* param) {
	memset(&crcd, 0, sizeof(Crcdm));
	mp = *(Movepath*)param;
	crcd.Sx = mp.Sx;
	crcd.Sy = mp.Sy;
	crcd.ID = mp.id;
	if (memcmp(&mp.Flower, "NVFC-AUTO-", 11) == 0) {
		if (mp.Ey == mp.Sy) {
			crcd.Len = mp.Ex - mp.Sx;
			dir = 0;
		}
		else {
			crcd.Len = mp.Ey - mp.Sy;
			dir = 1;
		}
		isNVFC = false;
	}
	else {
		if (mp.Ey == mp.Sy) {
			crcd.Len = mp.Ex - mp.Sx;
		}
		else {
			crcd.Len = mp.Ey - mp.Sy;
		}
	}
}

bool CompoundGather::GatherPrevProcess(ImageableSensorUnit* g) {
	string cursn = curSelectedName();
	if (cursn == "Keyence") {
		g->WorldCoordinateSet(dir, mp.Sx, mp.Sy, mp.Sz);
		g->WorldCoordinateSet((dir | 128), mp.Ex, mp.Ey, mp.Ez);
		EncTiggerTaber(g, dir);
		g->Grab(true, 0);
	}
	else if (cursn == "NVTLiner") {
		if (dir == 0) {//X
			g->ChannelSelected(0);
			g->TiggerTaber(2);
			//imgableSensor->WorldCoordinateSet(dir, mp.Sx, mp.Sy, mp.Sz);
			//_sleep(50);
			//MotionGpio->SetDoutput("线扫X", true);
			//_sleep(50);
			g->Grab(true, 100000);
			_sleep(900);
			MotionGpio->SetDoutput("线扫X", true);
		}
		else {
			g->ChannelSelected(1);
			g->TiggerTaber(2);
			//imgableSensor->WorldCoordinateSet(dir, mp.Sx, mp.Sy, mp.Sz);
			g->Grab(true, 100000);
			_sleep(900);
			MotionGpio->SetDoutput("线扫Y", true);
		}
	}
	else if (cursn == "Precitec") {
		kvpdChr* v = 0;
		v = FacParam->KvpRead("触发间隔");
		if (v != 0 && v->val != 0) {
			g->CtrInfo[0].lines = atoi(v->val);
		}
		else {
			SysLogRecord("设备参数-触发间隔-值获取错误!");
			//return -12;
			return false;
		}
		if (dir == 0) {
			//实际编码器方向与电机运动方向反了，所以取个反
			g->WorldCoordinateSet(dir, -mp.Sx, mp.Sy, mp.Sz);
			g->WorldCoordinateSet((dir | 128), -mp.Ex, mp.Ey, mp.Ez);
			g->TiggerTaber(3);
		}
		else if (dir == 1) {
			g->WorldCoordinateSet(dir, mp.Sx, mp.Sy, mp.Sz);
			g->WorldCoordinateSet((dir | 128), mp.Ex, mp.Ey, mp.Ez);
			g->TiggerTaber(4);
		}
		g->Grab(true, 0);
	}
	else {
		g->Grab(true, 100000);
	}

	kvpdChr* val = FacParam->KvpRead("触发等待");
	if (val != 0 && val->val != 0) {
		int tms = atoi(val->val);
		std::this_thread::sleep_for(std::chrono::microseconds(tms));
	}
	return true;
}

bool CompoundGather::GatherEndProcess(ImageableSensorUnit* g) {
	string cursn = curSelectedName();
	if (cursn == "NVTLiner") {
		if (dir == 0) { //X方向
			g->ChannelSelected(0);
			//MotionGpio->SetDoutput("线扫X", true);
			MotionGpio->SetDoutput("线扫X", false);//LVM-2730
			g->Stop();
		}
		if (dir == 1) { //Y方向
			g->ChannelSelected(1);
			MotionGpio->SetDoutput("线扫Y", false);//LVM-2730
			g->Stop();
		}
	}
	else
	{
		g->Stop();
	}
	return true;
}

void* CompoundGather::GatherResultGet(unsigned long& count) {
	count = crcd.getPointNums;
	return crcd.pointResult;
}

bool CompoundGather::Gather(ImageableSensorUnit* g) {
	if (g == 0) {
		g = CurSelectedGet();
	}
	if (g->Sensorinfo.devType == ImageableSensorUnit::Cream) {
		//return CreamTiggerProcess(g) ? 0 : errCode();
		return true;
	}
	else {
		char rt[35];
		memset(rt, 0, 35);
		stringstream msg;
		DateTimeGet(rt, 2);
		msg << "数据采集开始:" << rt;
		SysLogRecord(msg.str().c_str());
		crcd.getPointNums = g->IsCollectionCompleted(TMSOUTPOINTS);
		crcd.isFinshed = true;
		msg.clear();
		if (crcd.getPointNums < 1) {
			stringstream ss;
			ss << "运动路径点" << crcd.ID << "未能采集到数据!";
			SysLogRecord(ss.str().c_str(), CBC_STR_W_MSG);
			return false;
		}
		crcd.pointResult = (rxsPointCouldp*)g->DataGet(crcd.getPointNums);
		DateTimeGet(rt, 2);
		msg << "数据传输结-" << rt << ",运动路径点:" << crcd.ID << ",大小点数:" << crcd.getPointNums;
		SysLogRecord(msg.str().c_str(), CBC_STR_W_MSG);
		if (crcd.pointResult == 0) {
			return false;
		}
		if (isNVFC) {
			runGatherDataCalculatingAndSave(crcd, dir);
		}
		return true;
	}
}

bool CompoundGather::GatherDataFilterConvert(rxsPointCouldp* rpc, rxsPointCouldp* srcPs, unsigned& PointNums, double space, u8 dir, rxsPoint spos) {
	ImageableSensorUnit* curOpter = CurSelectedGet();
	if (curOpter->Sensorinfo.devType == ImageableSensorUnit::DevType::LineConfocal || curOpter->Sensorinfo.devType == ImageableSensorUnit::DevType::LineLsr) {
		//线激光/线光谱的处理
		return true;
	}
	else if (curOpter->Sensorinfo.devType == ImageableSensorUnit::DevType::PointLsr || curOpter->Sensorinfo.devType == ImageableSensorUnit::DevType::Confocal) {
		AxisInfo* xAxi = (*MotionGpio)["AOI-X"];
		AxisInfo* yAxi = (*MotionGpio)["AOI-Y"];
		int j = 0;
		Kvpdchr val = { 0 };
		float cmpVal = -20.0f;

		WrkParam->Selected(2);
		if (WrkParam->DataReaderKvpd((unsigned)0, val)) {
			if (val.val != 0) {
				cmpVal = atof(val.val);
			}
		}
		for (unsigned x = 0; x < PointNums; x++) {
			if (srcPs[x].z < cmpVal || isnan(srcPs[x].z)) {
				continue;
			}
			if (dir == 0) {
				rpc[j].x = (spos.x + x * space);
				rpc[j].y = yAxi->Status.EncPos;
			}
			else if (dir == 1) {
				rpc[j].x = xAxi->Status.EncPos;
				rpc[j].y = (spos.y + x * space);
			}
			else
			{
				//rpc[x].x = ;
				//rpc[x].y = ;
			}
			rpc[j].z = srcPs[x].z;
			j += 1;
		}
		PointNums = j;
		return true;
	}
	else {
		//相机
		return true;
	}
}
