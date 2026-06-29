#include "private.h"



FormatConvertStep::FormatConvertStep(int id) :StepBlockBase(id) {
	_type = StepBlockBase::CONVERT_DEPIMG_PC;
	outResultType = BLKOUTRESULTYPE_NONE;
}

FormatConvertStep::FormatConvertStep(int id, const char* name) : StepBlockBase(id, name) {
	_type = StepBlockBase::CONVERT_DEPIMG_PC;
	outResultType = BLKOUTRESULTYPE_NONE;
}


void FormatConvertStep::FormatConvertParamSet(FormatConvertParam) {


}



FormatConvertParam* FormatConvertStep::FormatConvertParamGet() {

	return 0;
}



short FormatConvertStep::Run() {

	return 0;
}


bool FormatConvertStep::InputParamStringCfgSet(std::string* p, int l)
{
	return true;

}



std::string FormatConvertStep::InputParamSaveStringGet() {

	return "";
}


