#include "private.h"

extern HrdWrkSyetemEnv hrdSyetemEnver;


roiGrpMangerStep::roiGrpMangerStep(int id) :StepBlockBase(id) {
	_type = StepBlockBase::ROIGRPMANGER;
	outResultType = BLKOUTRESULTYPE_IMG | BLKOUTRESULTYPE_PC;
}

roiGrpMangerStep::roiGrpMangerStep(int id, const char* name) : StepBlockBase(id, name) {
	_type = StepBlockBase::ROIGRPMANGER;
	outResultType = BLKOUTRESULTYPE_IMG | BLKOUTRESULTYPE_PC;
}

roiGrpMangerStep::~roiGrpMangerStep() {
	MultinClr();
	size_t nums = ep.outRoiPC.size();
	for (size_t i = 0; i < nums; i++) {
		if (ep.outRoiPC[i].Entiy != 0) {
			free(ep.outRoiPC[i].Entiy);
			ep.outRoiPC[i].Entiy = 0;
			ep.outRoiPC[i].pNums = 0;
		}
	}
	ep.outRoiPC.clear();
}

void roiGrpMangerStep::outClr() {
	for (size_t i = 0; i < ep.outRoiPC.size(); i++) {
		if (ep.outRoiPC[i].Entiy != 0) {
			free(ep.outRoiPC[i].Entiy);
			ep.outRoiPC[i].Entiy = 0;
			ep.outRoiPC[i].pNums = 0;
		}
	}
	ep.outRoiPC.clear();
}

void roiGrpMangerStep::roiAreaSet(roiRect* nv, int num) {
	roiMangeParam* rmp = (roiMangeParam*)InParamsGet();
	ep.roiAreas.clear();
	rmp->roiNums = num;
	for (int i = 0; i < num; i++) {
		ep.roiAreas.push_back(nv[i]);
	}
	rmp->roiArea = &ep.roiAreas[0];
}

void roiGrpMangerStep::roiAreaSet(roiRect& rr) {
	roiMangeParam* rmp = (roiMangeParam*)InParamsGet();
	if (rmp == 0) { return; }
	ep.roiAreas.push_back(rr);
	rmp->roiArea = &ep.roiAreas[0];
}

bool roiGrpMangerStep::InputParamStringCfgSet(std::string* p, int l) {
	roiMangeParam* rmp = ParamGet<roiMangeParam>();
	int index = 0;
	std::string val = p[index];
	rmp->srcLinker.id = atoi(val.c_str());
	val = p[index += 1];
	rmp->srcLinker.outChoose = atoi(val.c_str());
	rmp->roiNums = atoi(p[index += 1].c_str());
	for (int s = 0; s < rmp->roiNums; s++) {
		roiRect rr;
		rr.Flag = atoi(p[index += 1].c_str());
		rr.SORC = { atof(p[index += 1].c_str()) ,atof(p[index += 1].c_str()) ,atof(p[index += 1].c_str()) };
		rr.End = { atof(p[index += 1].c_str()) ,atof(p[index += 1].c_str()) ,atof(p[index += 1].c_str()) };
		rr.LWH = { atof(p[index += 1].c_str()) ,atof(p[index += 1].c_str()) ,atof(p[index += 1].c_str()) };
		rr.rName = p[index += 1];
		index += 1;
		rr.forRi = (p[index] == "_" ? "" : p[index]);
		ep.roiAreas.push_back(rr);
	}
	if (ep.roiAreas.size() > 0) {
		rmp->roiArea = &ep.roiAreas[0];
		rmp->roiNums = ep.roiAreas.size();
	}
	else {
		rmp->roiArea = 0;
	}
	return true;
}

std::string roiGrpMangerStep::InputParamSaveStringGet() {
	roiMangeParam* rmp = ParamGet<roiMangeParam>();
	stringstream result;
	//int sinm = rmp->SinMethod;
	//if (rmp->SinMethod == 1) {
		result <</* std::to_string(sinm) << "," << */std::to_string(rmp->srcLinker.id) << "," << std::to_string(rmp->srcLinker.outChoose);
	//}
	//else if (rmp->SinMethod == 2) {
	//	result << std::to_string(sinm) << "," << rmp->path;
	//}
	//else {
	//	result << std::to_string(sinm);
	//}
	result << "," << std::to_string(rmp->roiNums) << ",";
	for (size_t i = 0; i < rmp->roiNums; i++) {
		int flag = rmp->roiArea[i].Flag;
		result << std::to_string(flag) << "," << rmp->roiArea[i].SORC.x << "," << rmp->roiArea[i].SORC.y << "," << rmp->roiArea[i].SORC.z << ",";
		result << rmp->roiArea[i].End.x << "," << rmp->roiArea[i].End.y << "," << rmp->roiArea[i].End.z << ",";
		result << rmp->roiArea[i].LWH.x << "," << rmp->roiArea[i].LWH.y << "," << rmp->roiArea[i].LWH.z << ",";
		result << rmp->roiArea[i].rName << "," << ((rmp->roiArea[i].forRi.length() == 0) ? "_" : rmp->roiArea[i].forRi) << ",";
	}
	std::string r = result.str();
	r.pop_back();
	return r;
}

const char* roiGrpMangerStep::GetCanOutSelecter(int& index) {
	roiMangeParam* rmp = (roiMangeParam*)InParamsGet();
	if (rmp == 0) {
		index = -1;
		return 0;
	}
	if (index < ep.roiAreas.size()) {
		return ep.roiAreas[index].rName.c_str();
	}
	return 0;
}

void* roiGrpMangerStep::MultResultOut(int index) {
	if (index < 0) {
		return &outCe;
	}
	if (index < ep.outRoiPC.size()) {
		return &ep.outRoiPC[index];
	}
	return 0;
}

short roiGrpMangerStep::Run() {
	roiMangeParam* rmp = (roiMangeParam*)InParamsGet();
	if (rmp == 0) {
		return -1;
	}

	StepBlockBase* prev = fromSrc(rmp->srcLinker.id);
	if (prev == 0) { return -201; }
	if ((prev->OutType() & BLKOUTRESULTYPE_PC) != BLKOUTRESULTYPE_PC) { return -202; }
	const CloudEntity* ce;
	prev->GetResult(&ce);
	if (ce->pNums < 1 || ce->Entiy == 0) { return -203; }
	outCe = *ce;

	for (int x = 0; x < rmp->roiNums; x++) {
		CloudEntity roice = hrdSyetemEnver.rtk->RoiAreaGet(rmp->roiArea[x].Flag, outCe, rmp->roiArea[x].SORC, rmp->roiArea[x].End);
		ep.outRoiPC.push_back(roice);
	}
	return 0;
}