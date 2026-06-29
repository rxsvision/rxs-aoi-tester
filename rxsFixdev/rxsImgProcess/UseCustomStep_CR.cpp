#include "private.h"
#pragma warning(disable : 4996)


extern HrdWrkSyetemEnv hrdSyetemEnver;



ColorRecognitionStep::ColorRecognitionStep(int id) :StepBlockBase(id) {
	_type = StepBlockBase::COLORREG_UCM_PROCESS;
	outResultType = BLKOUTRESULTYPE_DATASET;
};

ColorRecognitionStep::ColorRecognitionStep(int id, const char* name) :StepBlockBase(id, name) {
	_type = StepBlockBase::COLORREG_UCM_PROCESS;
	outResultType = BLKOUTRESULTYPE_DATASET;
};




void ColorRecognitionStep::ColorRecognitionParamSet(ColorRecogParam crp) {
	int len = sizeof(ColorRecogParam);
	InParamAlloc(&crp, len);
	return;
}


ColorRecogParam* ColorRecognitionStep::ColorRecognitionParamGet() {
	ColorRecogParam* ipg = (ColorRecogParam*)InParamsGet();
	if (ipg == 0) {
		ColorRecogParam scp;
		memset(&scp, 0, sizeof(ColorRecogParam));
		ColorRecognitionParamSet(scp);
		ipg = (ColorRecogParam*)InParamsGet();
		if (ipg == 0) { return 0; }
	}
	return ipg;
}

void ColorRecognitionStep::rpicGather(const char* savedir) {
	ColorRecogParam* ucp = (ColorRecogParam*)InParamsGet();
	for (int i = 0; i < hrdSyetemEnver.hrd_lightSource->ColorNum; i++) {
		if (i == 4 || i == 5 || i == 6) { continue; }
		int ci = i + 1;
		CST_LightSource::ColorIndex cis[4] = { (CST_LightSource::ColorIndex)ci,(CST_LightSource::ColorIndex)ci,(CST_LightSource::ColorIndex)ci,(CST_LightSource::ColorIndex)ci };
		hrdSyetemEnver.hrd_lightSource->brightnessONOFF(cis);
		for (short x = 0; x < 4; x++) {
			hrdSyetemEnver.hrd_lightSource->brightnessSet(x, cis[x], ucp->lightVal);
		}
		_sleep(170);
		ImageableSensorUnit* hrdSensor = hrdSyetemEnver.pSystemer->Wrkpd->iabGather->Selected(ucp->selectedCream);
		if (hrdSensor != 0) {
			rxsImg rr = { 0,0,0,0,0 };
			hrdSensor->Grab(false, true);
			rr.Data = hrdSensor->DataGet(rr.Len);
			rr.cols = hrdSensor->CtrInfo[0].Width;
			rr.rows = hrdSensor->CtrInfo[0].Height;
			rr.Channels = 1;
			Mat img(rr.rows, rr.cols, CV_MAKETYPE(CV_8U, rr.Channels), rr.Data, rr.cols * rr.Channels);
			//White = 1, Blue = 2, Green = 3, Yellow = 4, RED_730 = 5, RED_850 = 6, UV = 7, RED = 8
			std::string fname = savedir;
			fname += "/";
			fname += std::to_string(ci);
			imwrite(fname + ".jpg", img);
		}
		else {
			break;
		}
		_sleep(20);
	}
}

short ColorRecognitionStep::Run() {
	if ((_type & SBTYPE::UCUSTOM_PROCESS) != SBTYPE::UCUSTOM_PROCESS)
	{
		return -98;
	}
	if (hrdSyetemEnver.rtk != 0) {
		ColorRecogParam* crp = (ColorRecogParam*)InParamsGet();
		rpicGather(crp->objDir);
		//内部集成算法处理调用
		rr.clear();
		std::string filed[2] = { "编号","结果" };
		rr.SetFiled(filed, 2, "颜色检测");
		hrdSyetemEnver.rtk->colorReg(crp->rootDir, crp->objDir, &rr);
		return 0;
	}
	else {
		return -2;
	}
	return -10;
}

bool ColorRecognitionStep::InputParamStringCfgSet(std::string* p, int l) {
	int index = 0;
	ColorRecogParam *crp = ColorRecognitionParamGet();
	//memset(&crp, 0, sizeof(ColorRecogParam));
	std::string val = p[index];
	crp->srcLinker.id = atoi(val.c_str());
	val = p[index += 1];
	crp->srcLinker.outChoose = atoi(val.c_str());

	val = p[index += 1];
	crp->lightVal = atoi(val.c_str());

	val = p[index += 1];
	memcpy(crp->objDir, val.c_str(), val.length());

	val = p[index += 1];
	memcpy(crp->rootDir, val.c_str(), val.length());

	val = p[index += 1];
	memcpy(crp->selectedCream, val.c_str(), val.length());

	val = p[index += 1];
	crp->roiEnable = (val == "1") ? true : false;
	val = p[index += 1];
	crp->roiArea.x = atoi(val.c_str());
	val = p[index += 1];
	crp->roiArea.y = atoi(val.c_str());
	val = p[index += 1];
	crp->roiArea.w = atoi(val.c_str());
	val = p[index += 1];
	crp->roiArea.h = atoi(val.c_str());
	val = p[index += 1];
	crp->roiArea.Cx = atoi(val.c_str());
	val = p[index += 1];
	crp->roiArea.Cy = atoi(val.c_str());
	val = p[index += 1];
	crp->roiArea.Angle = atof(val.c_str());
	return true;
}

std::string ColorRecognitionStep::InputParamSaveStringGet() {
	ColorRecogParam* crp = (ColorRecogParam*)InParamsGet();
	std::string result = std::to_string(crp->srcLinker.id) + "," + std::to_string(crp->srcLinker.outChoose) + ",";
	result += (std::to_string(crp->lightVal) + ",") + crp->objDir;
	result += ",";
	result += crp->rootDir;
	result += ",";
	result += crp->selectedCream;
	result += ",";
	result += (crp->roiEnable ? "1," : "0,");
	result += (std::to_string(crp->roiArea.x) + ",");
	result += (std::to_string(crp->roiArea.y) + ",");
	result += (std::to_string(crp->roiArea.w) + ",");
	result += (std::to_string(crp->roiArea.h) + ",");
	result += (std::to_string(crp->roiArea.Cx) + ",");
	result += (std::to_string(crp->roiArea.Cy) + ",");
	result += std::to_string(crp->roiArea.Angle);
	return result;
}