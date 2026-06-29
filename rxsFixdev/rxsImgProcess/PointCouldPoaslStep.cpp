#include "private.h"
extern HrdWrkSyetemEnv hrdSyetemEnver;


PointCouldPoaslStep::PointCouldPoaslStep(int id) :StepBlockBase(id) {
	_type = StepBlockBase::PC_POA_SL;
	outResultType = BLKOUTRESULTYPE_DATASET;
}

PointCouldPoaslStep::PointCouldPoaslStep(int id, const char* name) : StepBlockBase(id, name) {
	_type = StepBlockBase::PC_POA_SL;
	outResultType = BLKOUTRESULTYPE_DATASET;
}


bool PointCouldPoaslStep::InputParamStringCfgSet(std::string* p, int l) {
	int index = 0;
	PointCouldPoaslParam* scp = ParamGet<PointCouldPoaslParam>();
	std::string val = p[index];
	scp->srcLinker.id = atoi(val.c_str());
	val = p[index += 1];
	scp->srcLinker.outChoose = atoi(val.c_str());
	val = p[index += 1];
	scp->tp_s_or_l = atoi(val.c_str());
	val = p[index += 1];
	scp->roiEnable = (val == "1") ? true : false;
	val = p[index += 1];
	scp->roiArea.x = atoi(val.c_str());
	val = p[index += 1];
	scp->roiArea.y = atoi(val.c_str());
	val = p[index += 1];
	scp->roiArea.w = atoi(val.c_str());
	val = p[index += 1];
	scp->roiArea.h = atoi(val.c_str());
	val = p[index += 1];
	scp->roiArea.Cx = atoi(val.c_str());
	val = p[index += 1];
	scp->roiArea.Cy = atoi(val.c_str());
	val = p[index += 1];
	scp->roiArea.Angle = atof(val.c_str());
	return true;
}

std::string PointCouldPoaslStep::InputParamSaveStringGet() {
	PointCouldPoaslParam* p = (PointCouldPoaslParam*)InParamsGet();
	if (p == 0) { return ""; }
	stringstream result;
	int m = p->tp_s_or_l;
	result << std::to_string(p->srcLinker.id) << "," << std::to_string(p->srcLinker.outChoose) << "," << std::to_string(m) << ",";
	result << (p->roiEnable ? "1," : "0,");
	result << std::to_string(p->roiArea.x) << ",";
	result << std::to_string(p->roiArea.y) << ",";
	result << std::to_string(p->roiArea.w) << ",";
	result << std::to_string(p->roiArea.h) << ",";
	result << std::to_string(p->roiArea.Cx) << ",";
	result << std::to_string(p->roiArea.Cy) << ",";
	result << std::to_string(p->roiArea.Angle);
	return result.str();
}


short PointCouldPoaslStep::Run() {
	PointCouldPoaslParam* p = (PointCouldPoaslParam*)InParamsGet();
	if (p == 0) {
		return -5000;
	}
	StepBlockBase* prev = fromSrc(p->srcLinker.id);
	if (prev == 0) { return -201; }
	if ((prev->OutType() & BLKOUTRESULTYPE_PC) != BLKOUTRESULTYPE_PC) { return -5002; }
	const CloudEntity* ce;
	prev->GetResult(&ce);
	if (ce->pNums < 1 || ce->Entiy == 0) { return -5003; }
	rr.clear();
	if (p->tp_s_or_l == 1) {
		std::string fileds[] = { "ID","±àºÅ","ÏßÂÖÀª¶È","PASS" };
		rr.SetFiled(fileds, 5, "ÂÖÀª¶È²âÁ¿");
	}
	else if (p->tp_s_or_l == 2) {
		std::string fileds[] = { "ID","±àºÅ","ÃæÂÖÀª¶È","PASS" };
		rr.SetFiled(fileds, 5, "ÂÖÀª¶È²âÁ¿");
		hrdSyetemEnver.rtk->surfaceProfile(ce->Entiy, ce->pNums, &rr);
	}
	return 0;
}
