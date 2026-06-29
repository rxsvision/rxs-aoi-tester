#include "private.h"

extern HrdWrkSyetemEnv hrdSyetemEnver;

PointCouldEmplenStep::PointCouldEmplenStep(int id) : StepBlockBase(id) {
	_type = StepBlockBase::PC_EMPLEN;
	outResultType = BLKOUTRESULTYPE_DATASET;
}

PointCouldEmplenStep::PointCouldEmplenStep(int id, const char* name) : StepBlockBase(id, name) {
	_type = StepBlockBase::PC_EMPLEN;
	outResultType = BLKOUTRESULTYPE_DATASET;
}


short PointCouldEmplenStep::Run() {
	PointCouldEmplenParam* pclp = (PointCouldEmplenParam*)InParamsGet();
	if (pclp == 0) { return -4000; }
	StepBlockBase* prev = fromSrc(pclp->srcLinker.id);
	if (prev == 0) { return -201; }
	if ((prev->OutType() & BLKOUTRESULTYPE_PC) != BLKOUTRESULTYPE_PC) { return -202; }
	const CloudEntity* ce;
	if (prev->blkType() == StepBlockBase::ROIGRPMANGER) {
		prev->GetResult(&ce, pclp->srcLinker.outChoose);
	}
	else {
		prev->GetResult(&ce);
	}
	if (ce->pNums < 1 || ce->Entiy == 0) { return -203; }
	float rv = 0;
	if (pclp->isShapnesmate) {
		rv = hrdSyetemEnver.rtk->sharpnessEstimate(*(CloudEntity*)ce, pclp->spm_order, pclp->dir);
	}
	else {
		hrdSyetemEnver.rtk->emptyLength(*(CloudEntity*)ce, pclp->rang, rv, pclp->dir);
	}
}

bool PointCouldEmplenStep::InputParamStringCfgSet(std::string* p, int l) {
	int index = 0;
	PointCouldEmplenParam* scp = ParamGet<PointCouldEmplenParam>();
	std::string val = p[index];
	scp->srcLinker.id = atoi(val.c_str());
	val = p[index += 1];
	scp->srcLinker.outChoose = atoi(val.c_str());
	val = p[index += 1];
	scp->dir = atoi(val.c_str());
	val = p[index += 1];
	scp->rang = atoi(val.c_str());
	val = p[index += 1];
	scp->isShapnesmate = (val == "1");
	val = p[index += 1];
	scp->spm_order = atof(val.c_str());
	return true;
}

std::string PointCouldEmplenStep::InputParamSaveStringGet() {
	PointCouldEmplenParam* pclp = ParamGet<PointCouldEmplenParam>();
	std::stringstream ss;
	int dir = pclp->dir;
	ss << std::to_string(pclp->srcLinker.id) << "," << std::to_string(pclp->srcLinker.outChoose) << "," << std::to_string(dir) << "," << pclp->rang;
	ss << (pclp->isShapnesmate ? "1," : "0,") << pclp->spm_order;
	return ss.str();
}
