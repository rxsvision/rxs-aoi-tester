#include "private.h"


PointCouldFilterStep::PointCouldFilterStep(int id) :StepBlockBase(id) {
	_type = StepBlockBase::PC_FILTER;
	outResultType = BLKOUTRESULTYPE_PC;
}

PointCouldFilterStep::PointCouldFilterStep(int id, const char* name) : StepBlockBase(id, name) {
	_type = StepBlockBase::PC_FILTER;
	outResultType = BLKOUTRESULTYPE_PC;
}

void PointCouldFilterStep::PointCouldFilterParamSet(PointCouldFilterParam) {


}

PointCouldFilterParam* PointCouldFilterStep::PointCouldFilterParamGet() {

	return 0;
}

short PointCouldFilterStep::Run() {


	return 0;
}


bool PointCouldFilterStep::InputParamStringCfgSet(std::string* p, int l) {

	return true;
}

std::string PointCouldFilterStep::InputParamSaveStringGet() {
	return "";
}