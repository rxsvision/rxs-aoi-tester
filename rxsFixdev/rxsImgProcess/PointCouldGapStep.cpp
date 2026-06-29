#include "private.h"

extern HrdWrkSyetemEnv hrdSyetemEnver;

PointCouldGapStep::PointCouldGapStep(int id) : StepBlockBase(id) {
	_type = StepBlockBase::PC_GAP;
	outResultType = BLKOUTRESULTYPE_DATASET;
	ps = new std::vector<Xscstp>;
}

PointCouldGapStep::PointCouldGapStep(int id, const char* name) : StepBlockBase(id, name) {
	_type = StepBlockBase::PC_GAP;
	outResultType = BLKOUTRESULTYPE_DATASET;
	ps = new std::vector<Xscstp>;
}

PointCouldGapStep::~PointCouldGapStep() {
	if (ps != 0) {
		delete (std::vector<Xscstp>*)ps;
		ps = 0;
	}
}

short PointCouldGapStep::Run() {
	PointCouldGapParam* pcgp = (PointCouldGapParam*)InParamsGet();
	if (pcgp == 0) { return -4000; }
	StepBlockBase* prev = fromSrc(pcgp->srcLinker.id);
	if (prev == 0) { return -201; }
	if ((prev->OutType() & BLKOUTRESULTYPE_PC) != BLKOUTRESULTYPE_PC) { return -202; }
	const CloudEntity* ce;
	if (prev->blkType() == StepBlockBase::ROIGRPMANGER) {
		prev->GetResult(&ce, pcgp->srcLinker.outChoose);
	}
	else {
		prev->GetResult(&ce);
	}
	if (ce->pNums < 1 || ce->Entiy == 0) { return -203; }
	if (pcgp->pNum > 0) {
		std::vector<float> xs;
		std::vector<float> coplanarityThreshold;
		std::vector<float> skewThreshold;
		for (int x = 0; x < pcgp->pNum; x++) {
			xs.push_back(pcgp->p[x].xs);
			coplanarityThreshold.push_back(pcgp->p[x].coplanarityThreshold);
			skewThreshold.push_back(pcgp->p[x].skewThreshold);
		}
		hrdSyetemEnver.rtk->GapDenoise(*(CloudEntity*)ce, xs, coplanarityThreshold, skewThreshold, pcgp->range, pcgp->dir);
		return 0;
	}
	return 0;
}


bool PointCouldGapStep::InputParamStringCfgSet(std::string* p, int l) {
	int index = 0;
	PointCouldGapParam* scp = ParamGet<PointCouldGapParam>();
	((std::vector<Xscstp>*)ps)->clear();
	std::string val = p[index];
	scp->srcLinker.id = atoi(val.c_str());
	val = p[index += 1];
	scp->srcLinker.outChoose = atoi(val.c_str());
	val = p[index += 1];
	scp->dir = atoi(val.c_str());
	val = p[index += 1];
	scp->range = atoi(val.c_str());
	val = p[index += 1];
	scp->pNum = atoi(val.c_str());
	if (scp->pNum > 0) {
		for (int s = 0; s < scp->pNum; s++) {
			Xscstp pval = { atof(p[index += 1].c_str()),atof(p[index += 1].c_str()) ,atof(p[index += 1].c_str()) };
			((std::vector<Xscstp>*)ps)->push_back(pval);
		}
		scp->p = &(*(std::vector<Xscstp>*)ps)[0];
	}
	return true;
}

std::string PointCouldGapStep::InputParamSaveStringGet() {
	PointCouldGapParam* pcdp = (PointCouldGapParam*)InParamsGet();
	if (pcdp == 0) { return ""; }
	stringstream result;
	int dir = pcdp->dir;
	result << std::to_string(pcdp->srcLinker.id) << "," << std::to_string(pcdp->srcLinker.outChoose) << "," << std::to_string(dir);
	result << "," << std::to_string(pcdp->range) << "," << pcdp->pNum << ",";
	if (pcdp->pNum > 0) {
		for (int x = 0; x < pcdp->pNum; x++) {
			result << pcdp->p[x].xs << "," << pcdp->p[x].coplanarityThreshold << "," << pcdp->p[x].skewThreshold << ",";
		}
	}
	std::string ret = result.str();
	ret.pop_back();
	return ret;
}

void PointCouldGapStep::XscstpClr() {
	((std::vector<Xscstp>*)ps)->clear();
	PointCouldGapParam* pcgp = (PointCouldGapParam*)InParamsGet();
	if (pcgp != 0) {
		pcgp->p = 0;
		pcgp->pNum = 0;
	}
}

void PointCouldGapStep::XscstpPush(Xscstp* p) {
	PointCouldGapParam* scp = ParamGet<PointCouldGapParam>();
	((std::vector<Xscstp>*)ps)->push_back(*p);
	scp->pNum = ((std::vector<Xscstp>*)ps)->size();
	scp->p = &(*(std::vector<Xscstp>*)ps)[0];
}