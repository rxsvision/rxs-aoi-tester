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

short ColorRecognitionStep::Run() {
	if (hrdSyetemEnver.rtk2d != 0) {
		ColorRecogParam* crp = (ColorRecogParam*)InParamsGet();
		rr.clear();
		std::string filed[2] = { "编号","结果" };
		rr.SetFiled(filed, 2, "颜色检测");
		std::string v = hrdSyetemEnver.rtk2d->useModel_ColorReg(modPath, gatherPath);
		int r = rr.AddRow();
		rr.SetValue("编号", std::to_string(r), r - 1);
		rr.SetValue("结果", v, r - 1);
		return 0;
	}
	else {
		return -2;
	}
	return -10;
}

bool ColorRecognitionStep::InputParamStringCfgSet(std::string* p, int l) {
	int index = 0;
	ColorRecogParam* crp = ParamGet<ColorRecogParam>();
	std::string val = p[index];
	crp->srcLinker.id = atoi(val.c_str());
	val = p[index += 1];
	crp->srcLinker.outChoose = atoi(val.c_str());

	if ((index += 1) < l) {
		val = p[index];
		sampPath = (val == "\x09" ? "" : stringSubreplace(val, "\x1F", " "));
	}
	if ((index += 1) < l) {
		val = p[index];
		modPath = (val == "\x09" ? "" : stringSubreplace(val, "\x1F", " "));
	}

	if ((index += 1) < l) {
		val = p[index];
		gatherPath = (val == "\x09" ? "" : stringSubreplace(val, "\x1F", " "));
	}

	if ((index += 1) < l) {
		val = p[index];
		crp->roiEnable = (val == "1") ? true : false;
	}
	if ((index += 1) < l) {
		val = p[index];
		crp->roiArea.x = atoi(val.c_str());
	}
	if ((index += 1) < l) {
		val = p[index];
		crp->roiArea.y = atoi(val.c_str());
	}
	if ((index += 1) < l) {
		val = p[index];
		crp->roiArea.w = atoi(val.c_str());
	}
	if ((index += 1) < l) {
		val = p[index];
		crp->roiArea.h = atoi(val.c_str());
	}
	if ((index += 1) < l) {
		val = p[index];
		crp->roiArea.Cx = atoi(val.c_str());
	}
	if ((index += 1) < l) {
		val = p[index];
		crp->roiArea.Cy = atoi(val.c_str());
	}
	if ((index += 1) < l) {
		val = p[index];
		crp->roiArea.Angle = atof(val.c_str());
	}
	crp->gatherPath = gatherPath.c_str();
	crp->modelPath = modPath.c_str();
	crp->samplePath = sampPath.c_str();
	return true;
}

std::string ColorRecognitionStep::InputParamSaveStringGet() {
	ColorRecogParam* crp = (ColorRecogParam*)InParamsGet();
	std::stringstream ss;
	ss << std::to_string(crp->srcLinker.id) << "," << std::to_string(crp->srcLinker.outChoose) << ",";
	ss << (sampPath.length() > 0 ? stringSubreplace(sampPath, " ", "\x1F") : "\x09") << ",";
	ss << (modPath.length() > 0 ? stringSubreplace(modPath, " ", "\x1F") : "\x09");
	ss << "," << (gatherPath.length() > 0 ? stringSubreplace(gatherPath, " ", "\x1F") : "\x09") << ",";
	ss << (crp->roiEnable ? "1," : "0,");
	ss << std::to_string(crp->roiArea.x) << "," << std::to_string(crp->roiArea.y) << ",";
	ss << std::to_string(crp->roiArea.w) << "," << std::to_string(crp->roiArea.h) << ",";
	ss << std::to_string(crp->roiArea.Cx) << "," << std::to_string(crp->roiArea.Cy) << ",";
	ss << std::to_string(crp->roiArea.Angle);
	return ss.str();
}


bool ColorRecognitionStep::modeTrain(const char* sample, const char* savPath) {
	ColorRecogParam* crp = (ColorRecogParam*)InParamsGet();
	sampPath = sample;
	modPath = savPath;
	crp->samplePath = sampPath.c_str();
	crp->modelPath = modPath.c_str();
	return hrdSyetemEnver.rtk2d->trainModel_ColorReg(sampPath, modPath);
}

void ColorRecognitionStep::pathSet(const char* sample, const char* savPath, const char* gatPath) {
	ColorRecogParam* crp = (ColorRecogParam*)InParamsGet();
	sampPath = sample;
	modPath = savPath;
	gatherPath = gatPath;
	crp->samplePath = sampPath.c_str();
	crp->modelPath = modPath.c_str();
	crp->gatherPath = gatherPath.c_str();
}
