#include "private.h"

extern HrdWrkSyetemEnv hrdSyetemEnver;


PointCouldLengthStep::PointCouldLengthStep(int id) : StepBlockBase(id) {
	_type = StepBlockBase::PC_LENGTH;
	outResultType = BLKOUTRESULTYPE_DATASET;
}

PointCouldLengthStep::PointCouldLengthStep(int id, const char* name) : StepBlockBase(id, name) {
	_type = StepBlockBase::PC_LENGTH;
	outResultType = BLKOUTRESULTYPE_DATASET;
}


short PointCouldLengthStep::Run() {
	PointCouldLenParam* pclp = (PointCouldLenParam*)InParamsGet();
	if (pclp == 0) { return -4000; }
	StepBlockBase* prev = fromSrc(pclp->srcLinker.id);

	rr.clear();
	if (pclp->ispredict) {
		std::string fileds[] = { "预测长度","PASS" };
		rr.SetFiled(fileds, 2, "长度测量");


		//hrdSyetemEnver.rtk->predictLength(, pclp->Params.Plp.pModelin);
	}
	else {
		std::string fileds[] = { "长度","PASS" };
		rr.SetFiled(fileds, 2, "长度测量");
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

		rxsPointCouldp bondp[2] = { {0,0,0},{0,0,0} };
		StepBlockBase* bunSbb = fromSrc(pclp->Params.Mlp.boundLinker.id);
		rxsResultReport* rxspp = 0;
		bunSbb->GetResult(&rxspp);
		if (rxspp->RowCount() != 2) { return -4002; }

		bondp[0] = getBp(rxspp, 0);
		bondp[1] = getBp(rxspp, 1);
		CloudEntity ce_b = { &bondp[0],2 };
		float rv = 0;	
		if (hrdSyetemEnver.rtk->lengthOfBound(*(CloudEntity*)ce, ce_b, pclp->Params.Mlp.consider_y, rv, pclp->Params.Mlp.dir)) {
			int rc = rr.AddRow() - 1;
			rr.SetValue("长度", std::to_string(rv), rc);
			return 0;
		}
	}
	return -1;
}


bool PointCouldLengthStep::InputParamStringCfgSet(std::string* p, int l) {
	int index = 0;
	PointCouldLenParam* scp = ParamGet<PointCouldLenParam>();
	std::string val = p[index];
	scp->ispredict = (val == "1");
	val = p[index += 1];
	scp->srcLinker.id = atoi(val.c_str());
	val = p[index += 1];
	scp->srcLinker.outChoose = atoi(val.c_str());
	if (scp->ispredict) {
		val = p[index += 1];
		//scp->Params.Plp.pModelin = val.c_str();
		val = p[index += 1];
		scp->Params.Plp.pNum = atoi(val.c_str());
		if (scp->Params.Plp.pNum > 0) {
			for (int y = 0; y < scp->Params.Plp.pNum; y++) {
				val = p[index += 1];
				scp->Params.Plp.LenGrp = 0;
			}
		}
	}
	else {
		val = p[index += 1];
		scp->Params.Mlp.dir = atoi(val.c_str());
		val = p[index += 1];
		scp->Params.Mlp.consider_y = (val == "1");
		val = p[index += 1];
		scp->Params.Mlp.fixedBound = (val == "1");
		val = p[index += 1];
		scp->Params.Mlp.boundLinker.id = atoi(val.c_str());
		val = p[index += 1];
		scp->Params.Mlp.boundLinker.outChoose = atoi(val.c_str());
		scp->Params.Mlp.Bound[0] = { atof(p[index += 1].c_str()) ,atof(p[index += 1].c_str()),atof(p[index += 1].c_str()) };
		scp->Params.Mlp.Bound[1] = { atof(p[index += 1].c_str()) ,atof(p[index += 1].c_str()),atof(p[index += 1].c_str()) };
	}
	return true;
}

std::string PointCouldLengthStep::InputParamSaveStringGet() {
	PointCouldLenParam* pcdp = (PointCouldLenParam*)InParamsGet();
	if (pcdp == 0) { return ""; }
	stringstream result;
	std::string r;
	result << (pcdp->ispredict ? "1," : "0,") << std::to_string(pcdp->srcLinker.id) << "," << std::to_string(pcdp->srcLinker.outChoose);
	if (pcdp->ispredict) {
		result << std::to_string(pcdp->Params.Plp.pNum) << ",";
		for (int i = 0; i < pcdp->Params.Plp.pNum; i++) {
			result << "," << std::to_string(pcdp->Params.Plp.LenGrp[i]) << ",";
		}
	}
	else {
		int dir = pcdp->Params.Mlp.dir;
		result << "," << std::to_string(dir) << "," << pcdp->Params.Mlp.consider_y << "," << (pcdp->Params.Mlp.fixedBound ? "1," : "0,");
		result << pcdp->Params.Mlp.boundLinker.id << "," << pcdp->Params.Mlp.boundLinker.outChoose << ",";
		result << std::to_string(pcdp->Params.Mlp.Bound[0].x) << ",";
		result << std::to_string(pcdp->Params.Mlp.Bound[0].y) << ",";
		result << std::to_string(pcdp->Params.Mlp.Bound[0].z) << ",";
		result << std::to_string(pcdp->Params.Mlp.Bound[1].x) << ",";
		result << std::to_string(pcdp->Params.Mlp.Bound[1].y) << ",";
		result << std::to_string(pcdp->Params.Mlp.Bound[1].z) << ",";
	}
	r = result.str();
	r.pop_back();
	return r;
}