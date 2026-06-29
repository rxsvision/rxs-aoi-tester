#include "private.h"
extern HrdWrkSyetemEnv hrdSyetemEnver;


PointCouldArclenStep::PointCouldArclenStep(int id) :StepBlockBase(id) {
	_type = StepBlockBase::PC_ARC;
	outResultType = BLKOUTRESULTYPE_DATASET;
}

PointCouldArclenStep::PointCouldArclenStep(int id, const char* name) : StepBlockBase(id, name) {
	_type = StepBlockBase::PC_ARC;
	outResultType = BLKOUTRESULTYPE_DATASET;
}

//void PointCouldArclenStep::PointCouldArclenParamSet(PointCouldArclenParam pcalp) {
//	int len = sizeof(PointCouldArclenParam);
//	InParamAlloc(&pcalp, len);
//}
//
//PointCouldArclenParam* PointCouldArclenStep::PointCouldArclenParamGet() {
//	PointCouldArclenParam* pcalp = (PointCouldArclenParam*)InParamsGet();
//	if (pcalp == 0) {
//		PointCouldArclenParam isp = { 0,0,0 };
//		PointCouldArclenParamSet(isp);
//		pcalp = (PointCouldArclenParam*)InParamsGet();
//		if (pcalp == 0) { return 0; }
//	}
//	return pcalp;
//}


bool PointCouldArclenStep::InputParamStringCfgSet(std::string* p, int l) {
	int index = 0;
	//PointCouldArclenParam* scp = PointCouldArclenParamGet();
	PointCouldArclenParam* scp = ParamGet<PointCouldArclenParam>();
	std::string val = p[index];
	scp->srcLinker.id = atoi(val.c_str());
	val = p[index += 1];
	scp->srcLinker.outChoose = atoi(val.c_str());
	val = p[index += 1];
	scp->dir = atoi(val.c_str());
	val = p[index += 1];
	scp->isDebug = (val == "true");
	return true;
}

std::string PointCouldArclenStep::InputParamSaveStringGet() {
	PointCouldArclenParam* pcalp = (PointCouldArclenParam*)InParamsGet();
	if (pcalp == 0) { return ""; }
	stringstream result;
	int dir = pcalp->dir;
	result << std::to_string(pcalp->srcLinker.id) << "," << std::to_string(pcalp->srcLinker.outChoose) << "," << std::to_string(dir) << "," << std::to_string(pcalp->isDebug);
	return result.str();
}


short PointCouldArclenStep::Run() {
	PointCouldArclenParam* pcalp = (PointCouldArclenParam*)InParamsGet();
	if (pcalp == 0) {
		return -101;
	}
	StepBlockBase* prev = fromSrc(pcalp->srcLinker.id);
	if (prev == 0) { return -201; }
	if ((prev->OutType() & BLKOUTRESULTYPE_PC) != BLKOUTRESULTYPE_PC) { return -202; }
	hrdSyetemEnver.rtk->isDebug = pcalp->isDebug;
	const CloudEntity* ce;
	prev->GetResult(&ce);
	if (ce->pNums < 1 || ce->Entiy == 0) { return -203; }
	rr.clear();
	std::string fileds[] = { "ID","弧长(mm)","投影长(mm)","轮廓度","PASS" };
	rr.SetFiled(fileds, 5, "弧长测量");
	if (!hrdSyetemEnver.rtk->fitBSpline(ce->Entiy, ce->pNums, &rr, pcalp->dir)) {
		return -1;
	}
	hrdSyetemEnver.pSystemer->FrontProcessMsgSender(CBC_VISUAL_UPDSSR, (void*)&_id);
	return 0;
}

