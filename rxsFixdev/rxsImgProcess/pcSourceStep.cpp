#include "private.h"


#define MOTIONINPDMS		180

extern HrdWrkSyetemEnv hrdSyetemEnver;


PointCouldSourceStep::PointCouldSourceStep(int id) :StepBlockBase(id)
{
	_type = StepBlockBase::PC_IN_SOURCE;
	outResultType = BLKOUTRESULTYPE_PC;
	curRunIab = 0;
	mGpio = 0;
	memset(&outCe, 0, sizeof(CloudEntity));
	MultinGrpsAlloc(2);
}


PointCouldSourceStep::PointCouldSourceStep(int id, const char* name) :StepBlockBase(id, name)
{
	_type = StepBlockBase::PC_IN_SOURCE;
	outResultType = BLKOUTRESULTYPE_PC;
	curRunIab = 0;
	mGpio = 0;
	memset(&outCe, 0, sizeof(CloudEntity));
	MultinGrpsAlloc(2);
}


PointCouldSourceStep::~PointCouldSourceStep() {
	PointCouldSourceParam* pcsp = (PointCouldSourceParam*)InParamsGet();
	if (pcsp == 0) { return; }
	if (pcsp->sType == 0) {
		MotionMovParam* mmp = &pcsp->pcParams.Lpcsp.mmp;
		if (mmp != 0) {
			if (mmp->AxisLister != 0) {
				free(mmp->AxisLister);
				mmp->AxisLister = 0;
				mmp->AxNum = 0;
			}
		}
	}
}


void PointCouldSourceStep::PointCouldSourceParamSet(PointCouldSourceParam pcsp) {
	int len = sizeof(PointCouldSourceParam);
	InParamAlloc(&pcsp, len);
	InParamUpdateRefush();
}

PointCouldSourceParam* PointCouldSourceStep::PointCouldSourceParamGet() {
	PointCouldSourceParam* pcsp = (PointCouldSourceParam*)InParamsGet();
	if (pcsp == 0) {
		PointCouldSourceParam isp = { 0,0,0 };
		PointCouldSourceParamSet(isp);
		pcsp = (PointCouldSourceParam*)InParamsGet();
		if (pcsp == 0) { return 0; }
	}
	return pcsp;
}

bool PointCouldSourceStep::InputParamStringCfgSet(std::string* p, int l) {
	if (p == 0) { return true; }
	if (l > 0) {
		int index = 0;
		PointCouldSourceParam* pcsp = PointCouldSourceParamGet();
		pcsp->outFlag = atoi(p[index].c_str());
		pcsp->sType = atoi(p[index+=1].c_str());
		if (pcsp->sType == 0) {
			pcsp->pcParams.Lpcsp.method = p[index += 1].c_str()[0];
			locSensorSelectedSet(p[index += 1].c_str());
			pcsp->pcParams.Lpcsp.selectedSensor = sn.c_str();
			if (l > 3) {
				locSensorCfgPathSet(p[index += 1].c_str());
				pcsp->pcParams.Lpcsp.cfgPath = path.c_str();
			}
			if (l < 5) { return true; }
			pcsp->pcParams.Lpcsp.mmp.AxNum = atoi(p[index += 1].c_str());
			MotionMovParam* mmp = &pcsp->pcParams.Lpcsp.mmp;
			if (mmp->AxNum < 1) { return false; }
			mmp->AxisLister = (AxMoveparam*)malloc(mmp->AxNum * sizeof(AxMoveparam));
			if (mmp->AxisLister == 0) { return false; }
			//std::vector<std::string>* ns = (std::vector<std::string> *)names;
			names.clear();
			for (unsigned i = 0; i < mmp->AxNum; i++) {
				names.push_back(p[index += 1]);
			}
			for (unsigned i = 0; i < mmp->AxNum; i++) {
				mmp->AxisLister[i].AxName = names[i].c_str();//*注意此处存在问题，每一次push 都会导致 源指针变化
				mmp->AxisLister[i].method = p[index += 1].c_str()[0];
				mmp->AxisLister[i].waitInp = -1;
				mmp->AxisLister[i].acc = atof(p[index += 1].c_str());
				mmp->AxisLister[i].dec = atof(p[index += 1].c_str());
				mmp->AxisLister[i].TageVel = atof(p[index += 1].c_str());
				mmp->AxisLister[i].TagePos = atof(p[index += 1].c_str());
			}
		}
		else if (pcsp->sType == 1) {
			LocalPathSet(p[index += 1].c_str());
		}		
		if ((index += 1) < l) {
			outSavePathSet(p[index].c_str());
		}
		return true;
	}
	return false;
}

std::string PointCouldSourceStep::InputParamSaveStringGet() {
	PointCouldSourceParam* pcsp = (PointCouldSourceParam*)InParamsGet();
	if (pcsp == 0) { return ""; }
	stringstream result;
	result << std::to_string(pcsp->outFlag) << "," << std::to_string(pcsp->sType);
	if (pcsp->sType == 0) {
		string cfgp = pcsp->pcParams.Lpcsp.cfgPath == 0 ? "" : pcsp->pcParams.Lpcsp.cfgPath;
		result << "," << std::to_string(pcsp->pcParams.Lpcsp.method) << ",";
		result << (pcsp->pcParams.Lpcsp.selectedSensor == 0 ? "\x09" : pcsp->pcParams.Lpcsp.selectedSensor);
		result << "," << (cfgp.length() > 0 ? cfgp : "\x09");
		result << "," << std::to_string(pcsp->pcParams.Lpcsp.mmp.AxNum) << ",";
		if (pcsp->pcParams.Lpcsp.mmp.AxNum > 0) {
			//std::vector<std::string>* ns = (std::vector<std::string> *)names;
			for (int i = 0; i < pcsp->pcParams.Lpcsp.mmp.AxNum; i++) {
				result << names[i] << ",";
			}
			for (int i = 0; i < pcsp->pcParams.Lpcsp.mmp.AxNum; i++) {
				AxMoveparam amps = pcsp->pcParams.Lpcsp.mmp.AxisLister[i];
				result << amps.method << "," << amps.acc << "," << amps.dec << "," << amps.TageVel << "," << amps.TagePos;
				if (i < (pcsp->pcParams.Lpcsp.mmp.AxNum - 1)) {
					result << ",";
				}
			}
		}
	}
	else if (pcsp->sType == 1) {
		if (pcsp->pcParams.locPath != 0) {
			result << "," << pcsp->pcParams.locPath;
		}
	}
	else {

	}
	if (osfp.length() > 1) {
		result << "," << osfp;
	}
	else {
		result << ",\x09";
	}
	return result.str();
}

void PointCouldSourceStep::InParamUpdateRefush() {
	PointCouldSourceParam* ngv = (PointCouldSourceParam*)InParamsGet();
	if (ngv != 0) {
		if (ngv->sType == 0) {
			if (ngv->pcParams.Lpcsp.selectedSensor != 0) {
				locSensorSelectedSet(ngv->pcParams.Lpcsp.selectedSensor);
				ngv->pcParams.Lpcsp.selectedSensor = sn.c_str();
			}

			if (ngv->pcParams.Lpcsp.cfgPath != 0) {
				locSensorCfgPathSet(ngv->pcParams.Lpcsp.cfgPath);
				ngv->pcParams.Lpcsp.cfgPath = path.c_str();
			}
		}
		else if (ngv->sType == 1) {
			if (ngv->pcParams.locPath != 0) {
				LocalPathSet(ngv->pcParams.locPath);
				ngv->pcParams.locPath = path.c_str();
			}
		}
	}
}

void PointCouldSourceStep::AxListCreater(int AxNums, AxMoveparam* amps) {
	if (AxNums < 1) { return; }
	PointCouldSourceParam* pcsp = (PointCouldSourceParam*)InParamsGet();
	if (pcsp == 0) { return; }
	if (pcsp->sType != 0) { return; }
	MotionMovParam* mmp = &pcsp->pcParams.Lpcsp.mmp;
	if (mmp->AxisLister != 0) {
		free(mmp->AxisLister);
		mmp->AxNum = 0;
		mmp->AxisLister = 0;
	}
	mmp->AxNum = AxNums;
	mmp->AxisLister = (AxMoveparam*)malloc(AxNums * sizeof(AxMoveparam));
	if (mmp->AxisLister == 0) { return; }
	memcpy(mmp->AxisLister, amps, AxNums * sizeof(AxMoveparam));
	//std::vector<std::string>* ns = (std::vector<std::string> *)names;
	names.clear();
	for (size_t i = 0; i < AxNums; i++) {
		names.push_back(amps[i].AxName);
	}
	for (size_t i = 0; i < AxNums; i++) {
		mmp->AxisLister[i].AxName = names[i].c_str();
	}
}

void PointCouldSourceStep::locSensorCfgPathSet(const char* newPath) {
	if (newPath[0] == '\x09') {
		path.clear();
	}
	else {
		path = newPath;

	}
}

void PointCouldSourceStep::locSensorSelectedSet(const char* name) {
	sn = name;
}

void PointCouldSourceStep::LocalPathSet(const char* newpath) {
	path = newpath;
}

void PointCouldSourceStep::outSavePathSet(const char* fp) {
	if (fp == 0) {
		osfp = "";
	}
	else if (fp[0] == '\x09') {
		osfp = "";
	}
	else {
		osfp = fp;
		if (osfp.length() > 0) {
			char ec = osfp[osfp.length() - 1];
			if (ec != '\\' && ec != '/') {
				osfp = osfp.append("/");
			}
		}
	}
	PointCouldSourceParam* pcsp = (PointCouldSourceParam*)InParamsGet();
	pcsp->outSavePath = osfp.c_str();
}

void PointCouldSourceStep::PointDataComputer(rxsPointCouldp* pointResult, unsigned getPointNums, float Len, rxsPoint Spos, u8 dir) {
	rxsPointCouldp* srcPs = pointResult;
	if (outCe.Entiy != 0) {
		delete[] outCe.Entiy;
		outCe.Entiy = 0;
	}
	outCe.Entiy = new rxsPointCouldp[getPointNums];
	double space = Len / getPointNums;
	if (outCe.Entiy != 0) {
		memset(outCe.Entiy, 0, sizeof(rxsPointCouldp) * getPointNums);
	}
	else {
		hrdSyetemEnver.pSystemer->SysLogRecord("PointDataComputerAndSave异常-rpc内存分配失败!");
		return;
	}
	bool inv = (space < 0.00f);
	const char* ps = (*hrdSyetemEnver.pSystemer->FacParam)["点间距"];
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
	PointGatherDataFilterConvert(outCe.Entiy, srcPs, outCe.pNums, space, dir, Spos);
}


bool PointCouldSourceStep::PointGatherDataFilterConvert(rxsPointCouldp* rpc, rxsPointCouldp* srcPs, unsigned long& PointNums, double space, u8 dir, rxsPoint spos) {
	ImageableSensorUnit* curOpter = (ImageableSensorUnit*)curRunIab;
	if (curOpter->Sensorinfo.devType == ImageableSensorUnit::DevType::PointLsr || curOpter->Sensorinfo.devType == ImageableSensorUnit::DevType::Confocal) {
		AxisInfo* xAxi = (*(MotionGpioSerialize*)mGpio)["AOI-X"];
		AxisInfo* yAxi = (*(MotionGpioSerialize*)mGpio)["AOI-Y"];
		int j = 0;
		Kvpdchr val = { 0 };
		float cmpVal = -20.0f;

		//WrkParam->Selected(2);
		//if (WrkParam->DataReaderKvpd((unsigned)0, val)) {
		//	if (val.val != 0) {
		//		cmpVal = atof(val.val);
		//	}
		//}
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
	}
	return true;
}



short PointCouldSourceStep::Run() {
	rxsPointCouldp* srcPs = 0;
	u8 dir = 0;
	float movLen = 0;
	PointCouldSourceParam* sp = (PointCouldSourceParam*)InParamsGet();
	if (sp == 0) {
		return -101;
	}
	if (sp->sType == 0) {
		mGpio = hrdSyetemEnver.pSystemer->MotionGpio;
		if (mGpio == 0) { return -104; }
		ImageableGather* iabg = hrdSyetemEnver.pSystemer->Wrkpd->iabGather;
		if (iabg == 0) { return -102; }
		ImageableSensorUnit* curOpter = iabg->Selected(sp->pcParams.Lpcsp.selectedSensor);
		if (curOpter == 0) { return -103; }
		curRunIab = curOpter;
		AxMoveparam ampmr = sp->pcParams.Lpcsp.mmp.AxisLister[0];
		Movepath mp;
		memset(&mp, 0, sizeof(Movepath));
		memcpy(mp.Flower, "NVFC-AUTO-", 11);
		AxisInfo* aix = (*(MotionGpioSerialize*)mGpio)["AOI-X"];
		AxisInfo* aiy = (*(MotionGpioSerialize*)mGpio)["AOI-Y"];
		if (memcmp(ampmr.AxName, "AOI-X", 6) == 0) {
			mp.Sx = aix->Status.EncPos;
			mp.Sy = aiy->Status.EncPos;
			mp.Ex = ampmr.TagePos;
			mp.Ey = aiy->Status.EncPos;
			mp.id = 0;
			dir = 0;
			movLen = mp.Ex - mp.Sx;
		}
		else if (memcmp(ampmr.AxName, "AOI-Y", 6) == 0) {
			mp.Sx = aix->Status.EncPos;
			mp.Sy = aiy->Status.EncPos;
			mp.Ex = aix->Status.EncPos;
			mp.Ey = ampmr.TagePos;
			mp.id = 1;
			dir = 1;
			movLen = mp.Ey - mp.Sy;
		}
		iabg->ReadyParamSet(&mp);
		iabg->GatherReady();
		((MotionGpioSerialize*)mGpio)->AxMove(ampmr);
		thread sthr([=]()
			{
				iabg->Gather(0);
			});
		sthr.detach();
		_sleep(MOTIONINPDMS);
		SystemInfo* sysStatus = hrdSyetemEnver.pSystemer->sysStatus;
		for (int i = 0; i < sp->pcParams.Lpcsp.mmp.AxNum; i++) {
			ampmr = sp->pcParams.Lpcsp.mmp.AxisLister[i];
			while (!((MotionGpioSerialize*)mGpio)->IsMotionFinsh(ampmr.AxName)) {
				_sleep(MOTIONINPDMS);
			}
		}
		iabg->GatherStop();
		if (curOpter->Sensorinfo.devType == ImageableSensorUnit::DevType::LineConfocal ||
			curOpter->Sensorinfo.devType == ImageableSensorUnit::DevType::LineLsr) {
			//线激光/线光谱的处理
			//LineDataComputerAndSave(cdm, dir);
		}
		else if (curOpter->Sensorinfo.devType == ImageableSensorUnit::DevType::PointLsr ||
			curOpter->Sensorinfo.devType == ImageableSensorUnit::DevType::Confocal) {
			_sleep(50);
			rxsPoint Spos = { mp.Sx ,mp.Sy };
			srcPs = (rxsPointCouldp*)iabg->GatherResultGet(outCe.pNums);
			PointDataComputer(srcPs, outCe.pNums, movLen, Spos, dir);
		}
		else {
			return -105;
		}
	}
	else if (sp->sType == 1) {
		sp->pcParams.locPath;
	}
	else {//sdk in
		CloudEntity* rr = (CloudEntity*)MultinGet(0);
		if (rr == 0) { return -106; }
		outCe = *rr;
	}
	if ((sp->outFlag & 4) == 4) {
		int d = dir;
		const char* flname = ((imgTaskFlowerProcesser*)tfpDependency)->FlowerName();
		std::string pf = osfp + flname + "_" + std::to_string(d) + "_.pcd";
		pcdFileSave(outCe.Entiy, outCe.pNums, pf.c_str());
	}
	return 0;
}