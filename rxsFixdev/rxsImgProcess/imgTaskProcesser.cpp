#include "private.h"
#include <thread>
#include <malloc.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <io.h>


#define		pEnventNameMap		((std::map<std::string, imgTaskProcessEnvt> *)pemaps)

extern HrdWrkSyetemEnv hrdSyetemEnver;


imgTaskFlowerProcesser::imgTaskFlowerProcesser(const char* flowerName) {
	rrpmg = 0;
	totalID = 1;
	pSystemer = 0;
	rrpmg = hrdSyetemEnver.rrpmg;
	pSystemer = hrdSyetemEnver.pSystemer;
	linkLister = new LinkLister();
	outlinkTable = new std::vector<cuOutLinker*>;
	pemaps = new std::map<std::string, imgTaskProcessEnvt>;
	tkCollection = new std::vector<StepBlockBase*>;
	int len = strlen(flowerName);
	locFlowerName = (char*)malloc(len + 1);
	if (locFlowerName == 0) { return; }
	locFlowerName[len] = 0;
	memcpy(locFlowerName, flowerName, len);
}

imgTaskFlowerProcesser::~imgTaskFlowerProcesser() {
	if (pemaps != 0) {
		delete (std::map<std::string, imgTaskProcessEnvt>*)pemaps;
		pemaps = 0;
	}
	if (locFlowerName != 0) {
		free(locFlowerName);
		locFlowerName = 0;
	}
	Clear();
	if (tkCollection != 0) {
		delete (std::vector<StepBlockBase*>*)tkCollection;
		tkCollection = 0;
	}
	if (outlinkTable != 0) {
		delete (std::vector<StepBlockBase*>*)outlinkTable;
	}
	if (linkLister != 0) {
		delete (LinkLister*)linkLister;
		linkLister = 0;
	}
}

int imgTaskFlowerProcesser::CanUseID() {
	return totalID;
}

void imgTaskFlowerProcesser::CanUseID(int setVal) {
	totalID = setVal;
}


void imgTaskFlowerProcesser::Clear() {
	if (tkCollection != 0) {
		std::vector<StepBlockBase*>* tkc = (std::vector<StepBlockBase*>*)tkCollection;
		for (size_t i = 0; i < tkc->size(); i++) {
			//需要根据几种具体的类型 delete
			StepBlockBase* sbb = (*tkc)[i];
			if (sbb != 0) {
				delete sbb;
				(*tkc)[i] = 0;
			}
		}
		(*tkc).clear();
	}
	if (outlinkTable != 0) {
		std::vector<cuOutLinker*>* A = (std::vector<cuOutLinker*> *)outlinkTable;
		for (size_t j = 0; j < A->size(); j++) {
			cuOutLinker *cuolr = (*A)[j];
			if (cuolr != 0) {
				delete cuolr;
				(*A)[j] = 0;
			}
		}
		(*A).clear();
	}
	if (linkLister != 0) {
		((LinkLister*)linkLister)->Clear();
	}
}



void imgTaskFlowerProcesser::ProcessEnvtSet(const char* taskName, imgTaskProcessEnvt func) {
	(*pEnventNameMap)[taskName] = func;
}


const char* imgTaskFlowerProcesser::FlowerName() {
	return  (const char*)locFlowerName;
}


short imgTaskFlowerProcesser::Ready() {
	std::vector<StepBlockBase*>* tkc = (std::vector<StepBlockBase*>*)tkCollection;
	StepBlockBase* cur = 0;
	for (size_t i = 0; i < tkc->size(); i++) {
		cur = (*tkc)[i];
		if (cur == 0) { continue; }
		cur->RunReady();
	}
	return 0;
}

void imgTaskFlowerProcesser::RunResourceEndProcess() {
	std::vector<StepBlockBase*>* tkc = (std::vector<StepBlockBase*>*)tkCollection;
	int l = tkc->size();
	StepBlockBase* cur = 0;
	for (size_t i = 0; i < l; i++) {
		cur = (*tkc)[i];
		unsigned ot = cur->OutType();
		if ((ot & BLKOUTRESULTYPE_DATASET) == BLKOUTRESULTYPE_DATASET) {
			rxsResultReport* rrr = 0;
			cur->GetResult(&rrr);
			if (rrr->RowCount() > 0 && rrpmg != 0) {
				rrpmg->ResultReportPush(*rrr);
				rrr->RowsClr();
			}
		}
	}
}

void imgTaskFlowerProcesser::linkInsert(StepBlockBase* sbb) {
	std::vector<cuOutLinker*>* A = (std::vector<cuOutLinker*>*) outlinkTable;
	std::vector<StepBlockBase*>* tkc = (std::vector<StepBlockBase*>*)tkCollection;
	((LinkLister*)linkLister)->Insert(sbb, tkc->size() - 1);
	cuOutLinker* cuolr = new cuOutLinker;
	cuolr->id = sbb->ID();
	cuolr->Name = sbb->Name();
	cuolr->outType = sbb->OutType();
	A->push_back(cuolr);
}



bool imgTaskFlowerProcesser::HadFinshed() {
	std::vector<StepBlockBase*>* tkc = (std::vector<StepBlockBase*>*)tkCollection;
	int l = tkc->size();
	StepBlockBase* cur = 0;
	for (size_t i = 0; i < l; i++) {
		cur = (*tkc)[i];
		if (cur->blkType() == StepBlockBase::IMG_OUT || cur->blkType() == StepBlockBase::IMG_OUT) {
			continue;
		}
		if (!cur->HadCompleted()) {
			return false;
		}
		else {
		}
	}
	return true;
}


void imgTaskFlowerProcesser::ResultReportSummary() {
	std::vector<StepBlockBase*>* tkc = (std::vector<StepBlockBase*>*)tkCollection;
	int l = tkc->size();
	StepBlockBase* cur = 0;
	for (size_t i = 0; i < l; i++) {
		cur = (*tkc)[i];
		unsigned ot = cur->OutType();
		if ((ot & BLKOUTRESULTYPE_DATASET) == BLKOUTRESULTYPE_DATASET) {
			rxsResultReport* rrr = 0;
			cur->GetResult(&rrr);
			if (rrr->RowCount() > 0 && rrpmg != 0) {
				rrpmg->ResultReportPush(*rrr);
				rrr->RowsClr();
			}
		}
	}
}

void imgTaskFlowerProcesser::RunResourceRelease() {
	std::vector<StepBlockBase*>* tkc = (std::vector<StepBlockBase*>*)tkCollection;
	StepBlockBase* cur = 0;
	for (size_t i = 0; i < tkc->size(); i++) {
		cur = (*tkc)[i];
		if (cur == 0) { continue; }
		cur->RunReady();
	}
}


int imgTaskFlowerProcesser::Run() {
	//if (Ready() != 0) { return -99; }
	int ercode = 0;
	std::vector<StepBlockBase*>* tkc = (std::vector<StepBlockBase*>*)tkCollection;
	int l = tkc->size();
	StepBlockBase* cur = 0;
	for (size_t i = 0; i < l; i++) {
		cur = (*tkc)[i];
		if (cur->Run() != 0) {
			int errid = cur->ID();
			pSystemer->FrontProcessMsgSender(CBC_VISUAL_ERR, (int*)&errid);
			return errid;
		}
		unsigned ot = cur->OutType();
		if ((ot & BLKOUTRESULTYPE_IMG) == BLKOUTRESULTYPE_IMG) {
			if (i == (l - 1) && pSystemer != 0) {
				const rxsImg* ri = 0;
				cur->GetResult(&ri);
				if (ri != 0 && ri->Data != 0) {
					pSystemer->FrontProcessMsgSender(CBC_IMGSHOW, (void*)ri);
				}
				else {
					pSystemer->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"无有效结果图片输出!");
				}
				break;
			}
		}
	}
	std::cout << "run Finshed" << std::endl;
	RunResourceEndProcess();
	return 0;
}

short imgTaskFlowerProcesser::RunAnsy() {
	//if (Ready() != 0) { return -99; }
	auto fun = [=]() {
		//std::vector<StepBlockBase*> ntkc = (*(std::vector<StepBlockBase*>*)tkCollection);
		//int l = ntkc.size();
		//StepBlockBase* cur = 0;
		//for (int i = 0; i < l; i++) {
		//	cur = ntkc[i];
		//	if (cur->Run() != 0) {
		//		int errid = cur->ID();
		//		pSystemer->FrontProcessMsgSender(CBC_VISUAL_ERR, (int*)&errid);
		//		return;
		//	}
		//	unsigned ot = cur->OutType();
		//	if ((ot & BLKOUTRESULTYPE_DATASET) == BLKOUTRESULTYPE_DATASET) {
		//		rxsResultReport* rrr = 0;
		//		cur->GetResult(&rrr);
		//		if (rrr->RowCount() > 0 && rrpmg != 0) {
		//			rrpmg->ResultReportPush(*rrr);
		//			if (pSystemer != 0)
		//				pSystemer->FrontProcessMsgSender(CBC_VISUAL_UPD, 0);
		//		}
		//	}
		//	if ((ot & BLKOUTRESULTYPE_IMG) == BLKOUTRESULTYPE_IMG) {
		//		if (i == (l - 1) && pSystemer != 0) {
		//			const rxsImg* ri = 0;
		//			cur->GetResult(&ri);
		//			if (ri != 0 && ri->Data != 0) {
		//				pSystemer->FrontProcessMsgSender(CBC_IMGSHOW, (void*)ri);
		//			}
		//			else {
		//				pSystemer->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"无有效结果图片输出!");
		//			}
		//			break;
		//		}
		//	}
		//}
		Run();
	};
	std::thread sthr(fun);
	sthr.detach();
	return 0;
}

bool imgTaskFlowerProcesser::operator ==(imgTaskFlowerProcesser* dest) {
	int l = strlen((char*)dest->locFlowerName);
	bool mv = (memcmp(dest->locFlowerName, locFlowerName, l) == 0);
	if (!mv) { return false; }

	std::vector<StepBlockBase*>* tkc = (std::vector<StepBlockBase*>*)tkCollection;
	std::vector<StepBlockBase*>* dtkc = (std::vector<StepBlockBase*>*)(dest->tkCollection);
	mv = (tkc->size() == dtkc->size());
	if (!mv) { return false; }

	//各个步骤快 也需要完全相等
	mv = ((*tkc) == (*dtkc));
	if (!mv) { return false; }
	return mv;
}

int imgTaskFlowerProcesser::index(const int id) {
	std::vector<StepBlockBase*>* tkc = (std::vector<StepBlockBase*>*)tkCollection;
	StepBlockBase* sbb = 0;
	for (unsigned x = 0; x < tkc->size(); x++) {
		sbb = (*tkc)[x];
		if (sbb->ID() == id) {
			return x;
		}
	}
	return -1;
}


StepBlockBase* imgTaskFlowerProcesser::operator[](const int id) {
	std::vector<StepBlockBase*>* tkc = (std::vector<StepBlockBase*>*)tkCollection;
	StepBlockBase* sbb = 0;
	for (unsigned x = 0; x < tkc->size(); x++) {
		sbb = (*tkc)[x];
		if (sbb->ID() == id) {
			return sbb->self();
		}
	}
	return 0;
}

StepBlockBase* imgTaskFlowerProcesser::StepBlockGet(int index) {
	std::vector<StepBlockBase*>* tkc = (std::vector<StepBlockBase*>*)tkCollection;
	int l = tkc->size();
	if (index < l) {
		return (*tkc)[index]->self();
	}
	return 0;
}

StepBlockBase* imgTaskFlowerProcesser::StepBlockGet(const char* sbName, int id) {
	std::vector<StepBlockBase*>* tkc = (std::vector<StepBlockBase*>*)tkCollection;
	int len = strlen(sbName);
	for (size_t i = 0; i < tkc->size(); i++) {
		StepBlockBase* sbb = (*tkc)[i];
		const char* v = sbb->Name();
		if (id == sbb->ID() && memcmp(sbName, v, len) == 0) {
			return sbb->self();
		}
	}
	return 0;
}

StepBlockBase* imgTaskFlowerProcesser::StepTypeFind(StepBlockBase::SBTYPE tt) {
	std::vector<StepBlockBase*>* tkc = (std::vector<StepBlockBase*>*)tkCollection;
	for (size_t i = 0; i < tkc->size(); i++) {
		StepBlockBase* sbb = (*tkc)[i];
		const char* v = sbb->Name();
		if (tt == sbb->blkType()) {
			return sbb->self();
		}
	}
	return 0;
}

StepBlockBase* imgTaskFlowerProcesser::StepBlockGet(const char* sbName) {
	std::vector<StepBlockBase*>* tkc = (std::vector<StepBlockBase*>*)tkCollection;
	int len = strlen(sbName);
	for (size_t i = 0; i < tkc->size(); i++) {
		StepBlockBase* sbb = (*tkc)[i];
		const char* v = sbb->Name();
		if (memcmp(sbName, v, len) == 0) {
			return sbb->self();
		}
	}
	return 0;
}

imgTaskFlowerProcesser* imgTaskFlowerProcesser::self() {
	return this;
}

void imgTaskFlowerProcesser::CopyTo(imgTaskFlowerProcesser* itfp) {
	itfp->rrpmg = rrpmg;
	itfp->pSystemer = pSystemer;
	itfp->totalID = totalID;
	std::vector<StepBlockBase*>* tkc = (std::vector<StepBlockBase*>*)tkCollection;
}


void imgTaskFlowerProcesser::ResultReportGrpSet(rxsResultReportGrpsManger* ras, rxsMainFrame* fSystemer) {
	rrpmg = ras;
	pSystemer = fSystemer;
}

int imgTaskFlowerProcesser::StepBlockCount() {
	std::vector<StepBlockBase*>* tkc = (std::vector<StepBlockBase*>*)tkCollection;
	return tkc->size();
}

void imgTaskFlowerProcesser::StepBlockRemove(StepBlockBase* v) {
	if (v == 0) { return; }
	std::vector<StepBlockBase*>* tkc = (std::vector<StepBlockBase*>*)tkCollection;
	std::vector<StepBlockBase*>::iterator itr = std::find(tkc->begin(), tkc->end(), v);
	if (itr == tkc->end()) { return; }
	std::vector<cuOutLinker*>* A = (std::vector<cuOutLinker*>*) outlinkTable;
	std::vector<int> delIndex;
	for (int i = 0; i < A->size(); i++) {
		cuOutLinker* v = (*A)[i];
		if (v->id == (*itr)->ID() && v->Name == (*itr)->Name()) {
			delIndex.push_back(i);
		}
	}
	std::vector<cuOutLinker*>::iterator Aitr = A->begin();
	tkc->erase(itr);
	for (int j = 0; j < delIndex.size(); j++) {
		A->erase(Aitr + delIndex[j]);
	}
	((LinkLister*)linkLister)->Remove(v->ID());
}


void imgTaskFlowerProcesser::StepBlockRemove(const char* blkName, int id) {
	if (blkName == 0) { return; }
	StepBlockBase* A = StepBlockGet(blkName, id);
	StepBlockRemove(A);
}


void imgTaskFlowerProcesser::Linker(int sid, int TageID) {
	if (linkLister == 0) {
		return;
	}
	((LinkLister*)linkLister)->LinkerPush(sid, TageID);
}

int* imgTaskFlowerProcesser::Linker(int curid, unsigned& nums) {
	nums = 0;
	if (linkLister == 0) {
		return 0;
	}
	return ((LinkLister*)linkLister)->NextStepID(curid, nums);
}

void imgTaskFlowerProcesser::Save(const char* fp) {
	rxsVisualizationDescriptionFile vdf(this, (LinkLister*)linkLister);
	vdf.Save(fp);
}

bool imgTaskFlowerProcesser::Load(const char* fp) {
	Clear();
	ResultReportGrpSet(hrdSyetemEnver.rrpmg, hrdSyetemEnver.pSystemer);
	rxsVisualizationDescriptionFile vdf(this, (LinkLister*)linkLister);
	return vdf.reLoader(fp);
}

bool imgTaskFlowerProcesser::getValer(int cid, u8 outType) {


	return true;
}


short imgTaskFlowerProcesser::SignalBlkTester(StepBlockBase* A) {
	if (A == 0) { return -1000; }
	return A->Run();
}


short imgTaskFlowerProcesser::SignalBlkTester(int id, const char* path, int flag) {
	StepBlockBase* A = (*this)[id];
	A->PushIn(path, flag);
	return SignalBlkTester(A);
}



cuOutLinker* imgTaskFlowerProcesser::ForeachCanUseOutLinker(int index) {
	std::vector<cuOutLinker*>* A = (std::vector<cuOutLinker*>*) outlinkTable;
	if (index < A->size())
		return (*A)[index];
	return 0;
}