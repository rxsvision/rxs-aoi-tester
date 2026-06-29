#include "private.h"




extern HrdWrkSyetemEnv hrdSyetemEnver;



PointCouldDeformationStep::PointCouldDeformationStep(int id) :StepBlockBase(id) {
	_type = StepBlockBase::PC_DEFORMATION;
	outResultType = BLKOUTRESULTYPE_DATASET;
	modefn = new std::string;
}

PointCouldDeformationStep::PointCouldDeformationStep(int id, const char* name) : StepBlockBase(id, name) {
	_type = StepBlockBase::PC_DEFORMATION;
	outResultType = BLKOUTRESULTYPE_DATASET;
	modefn = new std::string;
}


PointCouldDeformationStep::~PointCouldDeformationStep() {
	if (modefn != 0) {
		delete (std::string*)modefn;
		modefn = 0;
	}
}

bool PointCouldDeformationStep::InputParamStringCfgSet(std::string* p, int l) {
	int index = 0;
	PointCouldDeformationParam* scp = ParamGet<PointCouldDeformationParam>();
	std::string val = p[index];
	scp->srcLinker.id = atoi(val.c_str());
	val = p[index += 1];
	scp->srcLinker.outChoose = atoi(val.c_str());
	val = p[index += 1];
	scp->dir = atoi(val.c_str());
	val = p[index += 1];
	ModelFilePathSet(val.c_str());
	val = p[index += 1];
	scp->order_deformation = atof(val.c_str());
	val = p[index += 1];
	scp->boundLinker.id = atoi(val.c_str());
	val = p[index += 1];
	scp->boundLinker.outChoose = atoi(val.c_str());
	val = p[index += 1];
	scp->ModeBound[0].x = atof(val.c_str());
	val = p[index += 1];
	scp->ModeBound[0].y = atof(val.c_str());
	val = p[index += 1];
	scp->ModeBound[0].z = atof(val.c_str());
	val = p[index += 1];
	scp->ModeBound[1].x = atof(val.c_str());
	val = p[index += 1];
	scp->ModeBound[1].y = atof(val.c_str());
	val = p[index += 1];
	scp->ModeBound[1].z = atof(val.c_str());
	return true;
}

std::string PointCouldDeformationStep::InputParamSaveStringGet() {
	PointCouldDeformationParam* pcdp = (PointCouldDeformationParam*)InParamsGet();
	if (pcdp == 0) { return ""; }
	stringstream result;
	int dir = pcdp->dir;
	result << std::to_string(pcdp->srcLinker.id) << "," << std::to_string(pcdp->srcLinker.outChoose) << "," << std::to_string(dir) << "," << pcdp->modefile;
	result << "," << std::to_string(pcdp->order_deformation) << "," << std::to_string(pcdp->boundLinker.id) << "," << std::to_string(pcdp->boundLinker.outChoose) << ",";
	result << std::to_string(pcdp->ModeBound[0].x) << "," << std::to_string(pcdp->ModeBound[0].y) << "," << std::to_string(pcdp->ModeBound[0].z) << ",";
	result << std::to_string(pcdp->ModeBound[1].x) << "," << std::to_string(pcdp->ModeBound[1].y) << "," << std::to_string(pcdp->ModeBound[1].z);
	return result.str();
}

void PointCouldDeformationStep::ModelFilePathSet(const char* fp) {
	if (fp == 0) { return; }
	(*(std::string*)modefn) = fp;
	PointCouldDeformationParam* pcdp = (PointCouldDeformationParam*)InParamsGet();
	pcdp->modefile = ((std::string*)modefn)->c_str();
}



short PointCouldDeformationStep::Run() {
	PointCouldDeformationParam* pcdp = (PointCouldDeformationParam*)InParamsGet();
	if (pcdp == 0) { return -4000; }
	StepBlockBase* prev = fromSrc(pcdp->srcLinker.id);
	if (prev == 0) { return -201; }
	if ((prev->OutType() & BLKOUTRESULTYPE_PC) != BLKOUTRESULTYPE_PC) { return -202; }
	const CloudEntity* ce;
	if (prev->blkType() == StepBlockBase::ROIGRPMANGER) {
		prev->GetResult(&ce, pcdp->srcLinker.outChoose);
	}
	else {
		prev->GetResult(&ce);
	}
	if (ce->pNums < 1 || ce->Entiy == 0) { return -203; }
	rr.clear();
	std::string fileds[] = { "变形度","变形类型","PASS" };
	rr.SetFiled(fileds, 3, "变形度检测");

	CloudEntity model = { 0,0 };
	rxsPointCouldp bondp[2] = { {0,0,0},{0,0,0} };

	StepBlockBase* bunSbb = fromSrc(pcdp->boundLinker.id);
	rxsResultReport* rxspp = 0;
	bunSbb->GetResult(&rxspp);
	if (rxspp->RowCount() != 2) { return -4002; }

	bondp[0] = getBp(rxspp, 0);
	bondp[1] = getBp(rxspp, 1);
	CloudEntity b_ce = { &bondp[0],2 };
	float rv = 0;
	model = hrdSyetemEnver.rtk->ModelReaderAndProcess((*(std::string*)modefn), pcdp->ModeBound);
	if (!hrdSyetemEnver.rtk->deformOfLaptop(model, *(CloudEntity*)ce, b_ce, pcdp->order_deformation, rv, pcdp->dir)) {
		free(model.Entiy);
		return -4001;
	}
	int ri = rr.AddRow() - 1;
	rr.SetValue("变形度", std::to_string(rv), ri);
	hrdSyetemEnver.pSystemer->FrontProcessMsgSender(CBC_VISUAL_UPDSSR, (void*)&_id);
	free(model.Entiy);
	return 0;
}
