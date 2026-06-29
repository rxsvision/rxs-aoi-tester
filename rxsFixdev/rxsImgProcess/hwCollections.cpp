#include "private.h"

extern HrdWrkSyetemEnv hrdSyetemEnver;


FocusingLensStep::FocusingLensStep(int id) :StepBlockBase(id) {
	_type = StepBlockBase::HW_FOCUSINGLENS;
	outResultType = (BLKOUTRESULTYPE_NONE);
	adjVal = 0;
	dms = 0;
}

FocusingLensStep::FocusingLensStep(int id, const char* name) : StepBlockBase(id, name) {
	_type = StepBlockBase::HW_FOCUSINGLENS;
	outResultType = (BLKOUTRESULTYPE_NONE);
	adjVal = 0;
	dms = 0;
}

unsigned short FocusingLensStep::AdjValue() {
	return adjVal;
}


void FocusingLensStep::AdjValue(unsigned short v) {
	adjVal = v;
}

unsigned FocusingLensStep::DmsValue() {
	return dms;
}

void FocusingLensStep::DmsValue(unsigned v) {
	dms = v;
}

short FocusingLensStep::Run() {
	AdjustFocal* af = (AdjustFocal*)(hrdSyetemEnver.pSystemer->Wrkpd)->HrdWareObjectGet(0);
	af->Set(adjVal);
	if (dms > 0) {
		_sleep(dms);
	}
	return 0;
}

bool FocusingLensStep::InputParamStringCfgSet(std::string* p, int l) {
	adjVal = atoi(p[0].c_str());
	if (l > 1) {
		dms = atoi(p[1].c_str());
	}
	return true;
}

std::string FocusingLensStep::InputParamSaveStringGet() {
	std::stringstream ss;
	ss << std::to_string(adjVal) << "," << std::to_string(dms);
	return ss.str();
}







LightSourceStep::LightSourceStep(int id) :StepBlockBase(id) {
	_type = StepBlockBase::HW_LIGHTSOURCE;
	outResultType = (BLKOUTRESULTYPE_NONE);
	lstype = 0;
}



LightSourceStep::LightSourceStep(int id, const char* name) : StepBlockBase(id, name) {
	_type = StepBlockBase::HW_LIGHTSOURCE;
	outResultType = (BLKOUTRESULTYPE_NONE);
	lstype = 0;
}

short LightSourceStep::Run() {
	msclspParam* lspp = (msclspParam*)InParamsGet();
	if (lstype == 0) {
		CST_TZls* hrd_tzLs = (CST_TZls*)hrdSyetemEnver.pSystemer->Wrkpd->HrdWareObjectGet(1);
		for (int i = 0; i < msclsp.size(); i++) {
			lsValueSet lvs = msclsp[i];
			hrd_tzLs->brightnessDSet(lvs.ChannelNo, lvs.Value);
		}
		if (lspp->dms > 0) {
			_sleep(lspp->dms);
		}
	}
	else if (lstype == 1) {
		CST_LightSource* hrd_lightSource = (CST_LightSource*)hrdSyetemEnver.pSystemer->Wrkpd->HrdWareObjectGet(2);
		CST_LightSource::ColorIndex cis[4] = { CST_LightSource::None,CST_LightSource::None,CST_LightSource::None,CST_LightSource::None };
		for (int i = 0; i < msclsp.size(); i++) {
			lsValueSet lvs = msclsp[i];
			cis[i] = lvs.curCi;
			hrd_lightSource->brightnessSet(i, cis[i], lvs.Value);
		}
		if (lspp->dms > 0) {
			_sleep(lspp->dms);
		}
	}
	return 0;
}

bool LightSourceStep::InputParamStringCfgSet(std::string* p, int l) {
	int index = 0;
	lstype = atoi(p[index].c_str());
	msclspParam* lspp = ParamGet<msclspParam>();
	lspp->setNums = atoi(p[index += 1].c_str());
	for (int x = 0; x < lspp->setNums; x++) {
		lsValueSet lvs = {
			atoi(p[index += 1].c_str()),
			atoi(p[index += 1].c_str()),
			CST_LightSource::ColorIndex(atoi(p[index += 1].c_str()))
		};
		msclsp.push_back(lvs);
	}
	if (lspp->setNums > 0 && msclsp.size() > 0) {
		lspp->setNums = msclsp.size();
		lspp->lsVs = &msclsp[0];
	}
	return true;
}


std::string LightSourceStep::InputParamSaveStringGet() {
	std::stringstream ss;
	ss << std::to_string(lstype) << "," << std::to_string(msclsp.size());
	for (int i = 0; i < msclsp.size(); i++)
	{
		int cn = msclsp[i].ChannelNo;
		ss << "," << std::to_string(cn) << "," << msclsp[i].Value << "," << msclsp[i].curCi;
	}
	return ss.str();
}


u8 LightSourceStep::lsType() {
	return lstype;
}


void LightSourceStep::lsType(u8 v) {
	lstype = v;
}


void LightSourceStep::LSParamClr() {
	msclsp.clear();
	msclspParam* p = (msclspParam*)InParamsGet();
	p->setNums = 0;
	p->lsVs = 0;
}


void LightSourceStep::mscSet(lsValueSet v) {
	msclsp.push_back(v);
	msclspParam* p = (msclspParam*)InParamsGet();
	p->setNums = msclsp.size();
	p->lsVs = &msclsp[0];
}