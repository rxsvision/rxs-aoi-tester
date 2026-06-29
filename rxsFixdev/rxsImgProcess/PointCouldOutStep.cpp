#include "private.h"


PointCouldOutStep::PointCouldOutStep(int id) :StepBlockBase(id) {
	_type = StepBlockBase::PC_OUT;
	outResultType = BLKOUTRESULTYPE_NONE;
}


PointCouldOutStep::PointCouldOutStep(int id, const char* name) : StepBlockBase(id, name) {
	_type = StepBlockBase::PC_OUT;
	outResultType = BLKOUTRESULTYPE_NONE;
}


void PointCouldOutStep::PointCouldOutParamSet(PointCouldOutParam) {

}


PointCouldOutParam* PointCouldOutStep::PointCouldOutParamGet() {

	return 0;
}


short PointCouldOutStep::Run() {
	return 0;
}


bool PointCouldOutStep::InputParamStringCfgSet(std::string* p, int l) {
	return true;
}


std::string PointCouldOutStep::InputParamSaveStringGet() {
	return "";
}