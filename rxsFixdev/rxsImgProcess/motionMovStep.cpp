#include "private.h"


extern HrdWrkSyetemEnv hrdSyetemEnver;



MotionMovStep::MotionMovStep(int id) :StepBlockBase(id)
{
	_type = StepBlockBase::MOTION_MOV;
	outResultType = BLKOUTRESULTYPE_NONE;
	names = new std::vector<std::string>;
}

MotionMovStep::MotionMovStep(int id, const char* name) :StepBlockBase(id, name)
{
	_type = StepBlockBase::MOTION_MOV;
	outResultType = BLKOUTRESULTYPE_NONE;
	names = new std::vector<std::string>;
}

void MotionMovStep::Release() {
	MotionMovParam* mmp = MotionMovParamParamGet();
	if (mmp != 0) {
		if (mmp->AxisLister != 0) {
			free(mmp->AxisLister);
			mmp->AxisLister = 0;
			mmp->AxNum = 0;
		}
	}
	((std::vector<std::string> *)names)->clear();
	delete (std::vector<std::string> *)names;
	names = 0;
}

void MotionMovStep::MotionMovParamSet(MotionMovParam mmp) {
	int len = sizeof(MotionMovParam);
	InParamAlloc(&mmp, len);
}

MotionMovParam* MotionMovStep::MotionMovParamParamGet() {
	MotionMovParam* mmp = (MotionMovParam*)InParamsGet();
	if (mmp == 0) {
		MotionMovParam isp;
		memset(&isp, 0, sizeof(MotionMovParam));
		MotionMovParamSet(isp);
		mmp = (MotionMovParam*)InParamsGet();
		if (mmp == 0) { return 0; }
	}
	return mmp;
}

short MotionMovStep::Run() {
	MotionMovParam* mmp = (MotionMovParam*)InParamsGet();
	if (mmp == 0) { return -1; }
	if (mmp->AxNum < 1) { return -2; }
	for (unsigned x = 0; x < mmp->AxNum; x++) {
		AxMoveparam amps = mmp->AxisLister[x];
		hrdSyetemEnver.pSystemer->MotionGpio->AxMove(amps);
		if (amps.waitInp < 0) {
			_sleep(100);
			while (!hrdSyetemEnver.pSystemer->MotionGpio->IsMotionFinsh(amps.AxName))
			{
				_sleep(100);
			}
		}
		else if (amps.waitInp > 0) {
			_sleep(amps.waitInp);
		}
	}
	return 0;
}


void MotionMovStep::AxListCreater(int AxNums, AxMoveparam* amps) {
	if (AxNums < 1) { return; }
	MotionMovParam* mmp = MotionMovParamParamGet();
	if (mmp->AxisLister != 0) {
		free(mmp->AxisLister);
		mmp->AxNum = 0;
		mmp->AxisLister = 0;
	}
	mmp->AxNum = AxNums;
	mmp->AxisLister = (AxMoveparam*)malloc(AxNums * sizeof(AxMoveparam));
	if (mmp->AxisLister == 0) { return; }
	memcpy(mmp->AxisLister, amps, AxNums * sizeof(AxMoveparam));
	std::vector<std::string>* ns = (std::vector<std::string> *)names;
	ns->clear();
	for (size_t i = 0; i < AxNums; i++) {
		ns->push_back(amps[i].AxName);
	}
	for (size_t i = 0; i < AxNums; i++) {
		mmp->AxisLister[i].AxName = (*ns)[i].c_str();
	}
}


bool MotionMovStep::InputParamStringCfgSet(std::string* p, int l) {
	int index = 0;
	MotionMovParam* mmp = MotionMovParamParamGet();
	mmp->AxNum = atoi(p[index].c_str());
	if (mmp->AxNum < 1) { return false; }
	mmp->AxisLister = (AxMoveparam*)malloc(mmp->AxNum * sizeof(AxMoveparam));
	if (mmp->AxisLister == 0) { return false; }
	std::vector<std::string>* ns = (std::vector<std::string> *)names;
	ns->clear();
	for (unsigned i = 0; i < mmp->AxNum; i++) {
		ns->push_back(p[index += 1]);
	}
	for (unsigned i = 0; i < mmp->AxNum; i++) {
		mmp->AxisLister[i].AxName = (*ns)[i].c_str();//*注意此处存在问题，每一次push 都会导致 源指针变化
		mmp->AxisLister[i].method = p[index += 1].c_str()[0];
		mmp->AxisLister[i].waitInp = atoi(p[index += 1].c_str());
		mmp->AxisLister[i].acc = atof(p[index += 1].c_str());
		mmp->AxisLister[i].dec = atof(p[index += 1].c_str());
		mmp->AxisLister[i].TagePos = atof(p[index += 1].c_str());
		mmp->AxisLister[i].TageVel = atof(p[index += 1].c_str());
	}
	return true;
}

std::string MotionMovStep::InputParamSaveStringGet() {
	MotionMovParam* mmp = (MotionMovParam*)InParamsGet();
	if (mmp == 0) { return ""; }
	stringstream result;
	result << mmp->AxNum << ",";
	std::vector<std::string>* ns = (std::vector<std::string> *)names;
	for (int i = 0; i < mmp->AxNum; i++) {
		result << (*ns)[i] << ",";
	}
	for (int i = 0; i < mmp->AxNum; i++) {
		AxMoveparam amps = mmp->AxisLister[i];
		result << amps.method << "," << amps.waitInp << "," << amps.acc << "," << amps.dec << "," << amps.TagePos << "," << amps.TageVel;
		if (i < (mmp->AxNum - 1)) {
			result << ",";
		}
	}
	return result.str();
}