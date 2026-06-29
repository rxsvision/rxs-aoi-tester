
#include "private.h"


extern HrdWrkSyetemEnv hrdSyetemEnver;

typedef struct _mpls {
    std::string name;
    std::string pls;
}MPLS;


UserCustomStep::UserCustomStep(int id) :StepBlockBase(id) {
    _type = StepBlockBase::IMG_UCUSTOM_PROCESS;
    outResultType = (BLKOUTRESULTYPE_IMG | BLKOUTRESULTYPE_DATASET);
    p = new MPLS;
};

UserCustomStep::UserCustomStep(int id, const char* name) : StepBlockBase(id, name) {
    _type = StepBlockBase::IMG_UCUSTOM_PROCESS;
    outResultType = (BLKOUTRESULTYPE_IMG | BLKOUTRESULTYPE_DATASET);
    p = new MPLS;
};


bool UserCustomStep::InputParamStringCfgSet(std::string* p, int l) {
    int index = 0;
    UseCustomParam* ucp = ParamGet<UseCustomParam>();
    ucp->srcLinker.id = atoi(p[index].c_str());
    ucp->srcLinker.outChoose = atoi(p[index += 1].c_str());
    int ll = l - 1;
    if (index < ll) {
        ((MPLS*)p)->name = p[index += 1];
        ucp->wichMethod = ((MPLS*)p)->name.c_str();
    }
    if (index < ll) {
        ((MPLS*)p)->pls = p[index += 1];
        ucp->params = ((MPLS*)p)->pls.c_str();
    }
    return true;
}


std::string UserCustomStep::InputParamSaveStringGet() {
    UseCustomParam* ucp = (UseCustomParam*)InParamsGet();
    if (ucp == 0) { return ""; }
    stringstream result;
    result << ucp->srcLinker.id << "," << ucp->srcLinker.outChoose << "," << ucp->wichMethod << "," << ucp->params;
    return result.str();
}


void UserCustomStep::Release()
{
    if (p != 0) {
        delete (MPLS*)p;
        p = 0;
    }
}


short UserCustomStep::Run() {

    //hrdSyetemEnver.rtk2d->dimple();

    return 0;
}


const char* UserCustomStep::foreachMethod(int& index) {


    return 0;
}


void UserCustomStep::CallSet(const char* mname, const char* paramlist) {
    UseCustomParam* ucp = (UseCustomParam*)InParamsGet();
    if (ucp == 0) {
        return;
    }
    ((MPLS*)p)->name = mname;
    ((MPLS*)p)->pls = paramlist;
    ucp->wichMethod = ((MPLS*)p)->name.c_str();
    ucp->params = ((MPLS*)p)->pls.c_str();
}

