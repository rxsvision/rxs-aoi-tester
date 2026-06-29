#include "private.h"

extern HrdWrkSyetemEnv hrdSyetemEnver;




ImgPreProcessStep::ImgPreProcessStep(int id)
	:StepBlockBase(id)
{

}



ImgPreProcessStep::ImgPreProcessStep(int id, const char* name)
	: StepBlockBase(id, name)
{
	_type = StepBlockBase::IMG_PREV_PROCESS;
	outResultType = BLKOUTRESULTYPE_IMG;
}


ImgPreProcessStep::~ImgPreProcessStep() {
	_type = StepBlockBase::IMG_PREV_PROCESS;
	outResultType = BLKOUTRESULTYPE_IMG;
}



void ImgPreProcessStep::PrevProcessInputParamSet(PrevProcessParam) {

}

PrevProcessParam* ImgPreProcessStep::PrevProcessInputParamGet() {

	return 0;
}

short ImgPreProcessStep::Run() {
	return 0;
}

bool ImgPreProcessStep::InputParamStringCfgSet(std::string* p, int l) {

	return true;
}

std::string ImgPreProcessStep::InputParamSaveStringGet() {

	return "";
}














typedef struct inexpparam {
	std::vector<std::string> fns;
	std::vector<std::string> fps;
}InExparam;




PointCouldPreProcessStep::PointCouldPreProcessStep(int id)
	:StepBlockBase(id)
{
	_type = StepBlockBase::PC_PREPROCESS;
	outResultType = BLKOUTRESULTYPE_PC;
	insp = new InExparam;
}


PointCouldPreProcessStep::PointCouldPreProcessStep(int id, const char* name) 
	: StepBlockBase(id, name)
{
	_type = StepBlockBase::PC_PREPROCESS;
	outResultType = BLKOUTRESULTYPE_PC;
	insp = new InExparam;
}


PointCouldPreProcessStep::~PointCouldPreProcessStep() {
	if (insp != 0) {
		delete (InExparam*)insp;
		insp = 0;
	}
}

short PointCouldPreProcessStep::Run() {
	if (hrdSyetemEnver.rtk == 0) {
		return -1;
	}


	//bool ret = hrdSyetemEnver.rtk->preprocessFunction();
	//return ret ? 0 : -2;

	return 0;
}


void PointCouldPreProcessStep::funNameParamSet(const char* n, const char* ps) {
	((InExparam*)insp)->fns.push_back(n);
	((InExparam*)insp)->fps.push_back(ps);
	int psl = ((InExparam*)insp)->fps.size();
	int ns = ((InExparam*)insp)->fns.size();
	PointCouldPreProParam* scp = (PointCouldPreProParam*)InParamsGet();
	scp->funcNum = psl;
}

void PointCouldPreProcessStep::funNameParamGet(int i, std::string& name, std::string& Ps) {
	int ps = ((InExparam*)insp)->fps.size();
	int ns = ((InExparam*)insp)->fns.size();
	if (i < ps && (ps == ns)) {
		name = ((InExparam*)insp)->fns[i];
		Ps = ((InExparam*)insp)->fps[i];
	}
	else {
		return;
	}
}

void PointCouldPreProcessStep::funNameParamClr() {
	((InExparam*)insp)->fns.clear();
	((InExparam*)insp)->fps.clear();
	PointCouldPreProParam* scp = (PointCouldPreProParam*)InParamsGet();
	scp->funcNum = 0;
}

bool PointCouldPreProcessStep::InputParamStringCfgSet(std::string* p, int l) {
	int index = 0;
	PointCouldPreProParam* scp = ParamGet<PointCouldPreProParam>();
	std::string val = p[index];
	scp->SinMethhod = atoi(val.c_str());
	val = p[index += 1];
	scp->srcLinker.id = atoi(val.c_str());
	val = p[index += 1];
	scp->srcLinker.outChoose = atoi(val.c_str());
	val = p[index += 1];
	scp->dir = atoi(val.c_str());
	val = p[index += 1];
	scp->funcNum = atoi(val.c_str());
	if (scp->funcNum > 0) {
		const char* n = 0;
		const char* Ps = 0;
		for (int s = 0; s < scp->funcNum; s++) {
			n = p[index += 1].c_str();
			Ps = p[index += 1].c_str();
			funNameParamSet(n, Ps);
		}
	}
	int ps = ((InExparam*)insp)->fps.size();
	int ns = ((InExparam*)insp)->fns.size();
	return (scp->funcNum == ps && (ps == ns));
}

std::string PointCouldPreProcessStep::InputParamSaveStringGet() {
	PointCouldPreProParam* scp = ParamGet<PointCouldPreProParam>();
	stringstream result;
	int dir = scp->dir;
	int sinm = scp->SinMethhod;
	result << std::to_string(sinm) << "," << std::to_string(scp->srcLinker.id) << "," << std::to_string(scp->srcLinker.outChoose) << "," << std::to_string(dir);
	result << "," << scp->funcNum << ",";
	if (scp->funcNum > 0) {
		for (int x = 0; x < scp->funcNum; x++) {
			result << ((InExparam*)insp)->fns[x] << "," << ((InExparam*)insp)->fps[x] << ",";
		}
	}
	std::string ret = result.str();
	ret.pop_back();
	return ret;
}


























typedef struct _pcbdolinp {
	std::string funName;
	std::string funParams;
}Pcdolinp;


PointCouldBoundolStep::PointCouldBoundolStep(int id)
	:StepBlockBase(id)
{
	_type = StepBlockBase::PC_BOUNDOL;
	outResultType = BLKOUTRESULTYPE_POINTSET;
	insp = new Pcdolinp;
}


PointCouldBoundolStep::PointCouldBoundolStep(int id, const char* name)
	: StepBlockBase(id, name)
{
	_type = StepBlockBase::PC_BOUNDOL;
	outResultType = BLKOUTRESULTYPE_POINTSET;
	insp = new Pcdolinp;
}



PointCouldBoundolStep::~PointCouldBoundolStep() {
	if (insp != 0) {
		delete (Pcdolinp*)insp;
		insp = 0;
	}
}

short PointCouldBoundolStep::Run() {
	PointCouldBoundolParam* p = (PointCouldBoundolParam*)InParamsGet();
	if (p == 0) { return -4000; }
	StepBlockBase* prev = fromSrc(p->srcLinker.id);
	if (prev == 0) { return -201; }
	if ((prev->OutType() & BLKOUTRESULTYPE_PC) != BLKOUTRESULTYPE_PC) { return -202; }
	if (p->isRoifp == 1) {
		rxsPointCouldp rp[2] = { {0,0,0} ,{0,0,0} };
		char lr[2] = { 'L','R' };
		rr.clear();
		std::string fileds[] = { "ID","X","Y","Z" };
		rr.SetFiled(fileds, 4, "∂‘∆Î’“µ„");
		int r = 0;
		for (int x = 0; x < 2; x++) {
			const CloudEntity* ce;
			prev->GetResult(&ce, p->srcLinker.outChoose + x);
			if (ce->pNums < 1 || ce->Entiy == 0) { return -203; }
			rp[x] = hrdSyetemEnver.rtk->roiPC_LRPointGet(*(CloudEntity*)ce, lr[x]);
			int no = rr.AddRow();
			r = no - 1;
			rr.SetValue("ID", std::to_string(no), r);
			rr.SetValue("X", std::to_string(rp[x].x), r);
			rr.SetValue("Y", std::to_string(rp[x].y), r);
			rr.SetValue("Z", std::to_string(rp[x].z), r);
		}
		hrdSyetemEnver.pSystemer->FrontProcessMsgSender(CBC_VISUAL_UPDSSR, (void*)&_id);
	}
	else {
	}
	return  0;
}


void PointCouldBoundolStep::FunNameSet(const char* name) {
	PointCouldBoundolParam* p = (PointCouldBoundolParam*)InParamsGet();
	if (p == 0) {
		return;
	}
	std::string Name = name;
	funnSet(&p->funName, &Name);
}

void PointCouldBoundolStep::FunParamsSet(const char* params) {
	PointCouldBoundolParam* p = (PointCouldBoundolParam*)InParamsGet();
	if (p == 0) {
		return;
	}
	std::string Params = params;
	funpSet(&p->funParams, &Params);
}



void PointCouldBoundolStep::funnSet(const char** t, void* p) {
	((Pcdolinp*)insp)->funName = *(std::string*)p;
	*t = (char *)((Pcdolinp*)insp)->funName.c_str();
}

void PointCouldBoundolStep::funpSet(const char** t, void* p) {
	((Pcdolinp*)insp)->funParams = *(std::string*)p;
	*t = (char*)((Pcdolinp*)insp)->funParams.c_str();
}

bool PointCouldBoundolStep::InputParamStringCfgSet(std::string* p, int l) {
	int index = 0;
	PointCouldBoundolParam* scp = ParamGet<PointCouldBoundolParam>();
	std::string val = p[index];
	scp->srcLinker.id = atoi(val.c_str());
	val = p[index += 1];
	scp->srcLinker.outChoose = atoi(val.c_str());
	val = p[index += 1];
	scp->isRoifp = atoi(val.c_str());
	val = p[index += 1];
	scp->dir = atoi(val.c_str());
	val = p[index += 1];
	funnSet(&scp->funName, &val);
	val = p[index += 1];
	funpSet(&scp->funParams, &val);
	return true;
}

std::string PointCouldBoundolStep::InputParamSaveStringGet() {
	PointCouldBoundolParam* scp = ParamGet<PointCouldBoundolParam>();
	stringstream result;
	int dir = scp->dir;
	int isroifp = scp->isRoifp;
	result << std::to_string(scp->srcLinker.id) << "," << std::to_string(scp->srcLinker.outChoose) << "," << std::to_string(isroifp) << "," << std::to_string(dir);
	result << "," << (scp->funName == 0 ? "_" : scp->funName) << "," << (scp->funParams == 0 ? "_" : scp->funParams);
	return result.str();
}
