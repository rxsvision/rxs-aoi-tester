#include "private.h"

#define			InImgGrpsPtr		((std::vector<rxsImg>*)MultinGrps)
#define			InMatGrpsPtr		((std::vector<cv::Mat>*)MultinGrps)

//extern HrdWrkSyetemEnv hrdSyetemEnver;

StepBlockBase::StepBlockBase(int id) {
	_type = COLORREG_UCM_PROCESS;
	sbName = 0;
	_id = id;
	tfpDependency = 0;
	inparams = 0;
	inplen = 0;
	inGrpsType = 0;
	MultinGrps = 0;
	memset(&outImg, 0, sizeof(rxsImg));
	memset(&RenderPos, 0, sizeof(rxsPointI));
	status = 0;
}

StepBlockBase::StepBlockBase(int id, const char* Name) {
	status = 0;
	_type = COLORREG_UCM_PROCESS;
	_id = id;
	tfpDependency = 0;
	sbName = 0;
	inparams = 0;
	inplen = 0;
	inGrpsType = 0;
	MultinGrps = 0;
	memset(&outImg, 0, sizeof(rxsImg));
	memset(&RenderPos, 0, sizeof(rxsPointI));
	if (Name == 0) { return; }
	int len = strlen(Name);
	if (len < 1) { return; }
	sbName = (char*)malloc(len + 1);
	if (sbName != 0) {
		sbName[len] = 0;
		memcpy(sbName, Name, len);
	}
}

StepBlockBase::StepBlockBase(int id, unsigned bsType, const char* Name) {
	status = 0;
	_id = id;
	_type = bsType;
	tfpDependency = 0;
	sbName = 0;
	inparams = 0;
	inplen = 0;
	inGrpsType = 0;
	MultinGrps = 0;
	memset(&outImg, 0, sizeof(rxsImg));
	if (Name == 0) { return; }
	int len = strlen(Name);
	if (len < 1) { return; }
	sbName = (char*)malloc(len + 1);
	if (sbName != 0) {
		sbName[len] = 0;
		memcpy(sbName, Name, len);
	}
}


StepBlockBase::~StepBlockBase() {
	Release();
	if (sbName != 0) {
		free(sbName);
		sbName = 0;
	}
	if (outImg.Data != 0) {
		free(outImg.Data);
		memset(&outImg, 0, sizeof(rxsImg));
	}
	if (inparams != 0) {
		free(inparams);
		inparams = 0;
	}
	if (MultinGrps != 0) {
		if (inGrpsType == 1) {
			std::vector<rxsImg>* A = (std::vector<rxsImg>*)MultinGrps;
			size_t l = A->size();
			for (size_t i = 0; i < l; i++) {
				rxsImg* ri = &(*A)[i];
				if (ri == 0) { continue; }
				if (ri->Data != 0) {
					free(ri->Data);
					ri->Data = 0;
				}
			}
			((std::vector<rxsImg>*)MultinGrps)->clear();
			delete (std::vector<rxsImg>*)MultinGrps;
		}
		else if (inGrpsType == 2) {
			std::vector<CloudEntity>* A = (std::vector<CloudEntity>*)MultinGrps;
			size_t l = A->size();
			for (size_t i = 0; i < l; i++) {
				CloudEntity* ce = &(*A)[i];
				if (ce == 0) { continue; }
				if (ce->Entiy != 0) {
					free(ce->Entiy);
					ce->Entiy = 0;
				}
			}
			((std::vector<CloudEntity>*)MultinGrps)->clear();
			delete (std::vector<CloudEntity>*)MultinGrps;
		}
		else if (inGrpsType == 3) {
			//std::vector<cv::Mat>* A = (std::vector<cv::Mat>*)MultinGrps;
			//size_t l = A->size();
			//for (size_t i = 0; i < l; i++) {
			//	cv::Mat* mat = &(*A)[i];
			//	if (mat == 0) { continue; }
			//	mat->release();
			//}
			((std::vector<cv::Mat>*)MultinGrps)->clear();
			delete (std::vector<cv::Mat>*)MultinGrps;
		}
		MultinGrps = 0;
	}
}

int StepBlockBase::ID() { return _id; }
unsigned StepBlockBase::blkType() { return _type; }
unsigned StepBlockBase::OutType() { return outResultType; }

void StepBlockBase::MultinGrpsAlloc(u8 isPCorImg) {
	if (isPCorImg == 2) {
		inGrpsType = 2;//pc
		MultinGrps = new std::vector<CloudEntity>;
	}
	else if (isPCorImg == 1) {
		inGrpsType = 1;//img
		MultinGrps = new std::vector<rxsImg>;
	}
	else if (isPCorImg == 3) {
		inGrpsType = 3;//cv::Mat
		MultinGrps = new std::vector<cv::Mat>;
	}
}

void StepBlockBase::MultinClr() {
	if (inGrpsType == 1) {
		std::vector<rxsImg>* A = (std::vector<rxsImg>*)MultinGrps;
		size_t l = A->size();
		for (size_t i = 0; i < l; i++) {
			rxsImg* ri = &(*A)[i];
			if (ri == 0) { continue; }
			if (ri->Data != 0) {
				free(ri->Data);
				ri->Data = 0;
			}
		}
		((std::vector<rxsImg>*)MultinGrps)->clear();
	}
	else if (inGrpsType == 2) {
		std::vector<CloudEntity>* A = (std::vector<CloudEntity>*)MultinGrps;
		size_t l = A->size();
		for (size_t i = 0; i < l; i++) {
			CloudEntity* ce = &(*A)[i];
			if (ce == 0) { continue; }
			if (ce->Entiy != 0) {
				free(ce->Entiy);
				ce->Entiy = 0;
			}
		}
		((std::vector<CloudEntity>*)MultinGrps)->clear();
	}
	else if (inGrpsType == 3) {
		std::vector<cv::Mat>* A = (std::vector<cv::Mat>*)MultinGrps;
		size_t l = A->size();
		for (size_t i = 0; i < l; i++) {
			//cv::Mat* mat = &(*A)[i];
			//if (mat == 0) { continue; }
			//mat->release();
		}
		((std::vector<cv::Mat>*)MultinGrps)->clear();
	}
}

void* StepBlockBase::MultResultOut(int index) { return 0; }

void* StepBlockBase::MultinGet(int index) {
	if (inGrpsType == 2) {
		std::vector<CloudEntity>* A = (std::vector<CloudEntity> *)MultinGrps;
		if (index < A->size()) {
			return &((*A)[index]);
		}
	}
	else if (inGrpsType == 1) {
		std::vector<rxsImg>* A = (std::vector<rxsImg> *)MultinGrps;
		if (index < A->size()) {
			return &((*A)[index]);
		}
	}
	else if (inGrpsType == 3) {
		std::vector<cv::Mat>* A = (std::vector<cv::Mat> *)MultinGrps;
		if (index < A->size()) {
			return &((*A)[index]);
		}
	}
	return 0;
}

void StepBlockBase::operator()(StepBlockBase* sbb) {
	sbb->_type = _type;
	sbb->_id = _id;
	sbb->rr = rr();
	sbb->inGrpsType = inGrpsType;
	sbb->RenderPos = RenderPos;
	sbb->tfpDependency = tfpDependency;
	if (sbb->sbName != 0) {
		free(sbb->sbName);
		sbb->sbName = 0;
	}
	if (sbName != 0) {
		int len = strlen(sbName);
		sbb->sbName = (char*)malloc(len + 1);
		if (sbb->sbName != 0) {
			sbb->sbName[len] = 0;
			memcpy(sbb->sbName, sbName, len);
		}
	}

	if (sbb->inparams != 0) {
		free(sbb->inparams);
		sbb->inparams = 0;
		sbb->inplen = 0;
	}
	if (inparams != 0) {
		sbb->inparams = malloc(inplen);
		if (sbb->inparams != 0) {
			memcpy(sbb->inparams, inparams, inplen);
			sbb->inplen = inplen;
		}
	}
	if (inGrpsType == 1) {
		if (sbb->MultinGrps != 0) {
			std::vector<rxsImg>* sbbA = (std::vector<rxsImg>*)sbb->MultinGrps;
			if (sbbA->size() > 0) {
				sbb->MultinClr();
			}
		}
		if (MultinGrps != 0) {
			*((std::vector<rxsImg>*)sbb->MultinGrps) = *((std::vector<rxsImg>*)MultinGrps);
		}
	}
	else if (inGrpsType == 2) {
		if (sbb->MultinGrps != 0) {
			std::vector<CloudEntity>* sbbA = (std::vector<CloudEntity>*)sbb->MultinGrps;
			if (sbbA->size() > 0) {
				sbb->MultinClr();
			}
		}
		if (MultinGrps != 0) {
			*((std::vector<CloudEntity>*)sbb->MultinGrps) = *((std::vector<CloudEntity>*)MultinGrps);
		}
	}

	if (sbb->outImg.Data != 0) {
		free(sbb->outImg.Data);
		memset(&sbb->outImg, 0, sizeof(rxsImg));
	}
	sbb->outimgAlloc(&outImg);
}

const char* StepBlockBase::Name() {
	if (sbName == 0) { return 0; }
	return (const char*)sbName;
}

void StepBlockBase::Name(const char* nName) {
	if (sbName != 0) {
		free(sbName);
	}
	int nlen = strlen(nName);
	sbName = (char*)malloc(nlen + 1);
	if (sbName == 0) { return; }
	sbName[nlen] = 0;
	memcpy(sbName, nName, nlen);
}

void StepBlockBase::DependencyMount(void* A) {
	tfpDependency = A;
}

StepBlockBase* StepBlockBase::fromSrc(int id) {
	if (tfpDependency == 0) { return 0; }
	return (*(imgTaskFlowerProcesser*)tfpDependency)[id];
}


void StepBlockBase::outimgAlloc(rxsImg* dai) {
	if (dai == 0) { return; }
	if (dai->Data == 0) { return; }
	void* data = malloc(dai->Len);
	if (data == 0) { return; }
	outImg = *dai;
	outImg.Data = data;
	memcpy(outImg.Data, dai->Data, dai->Len);
}

void StepBlockBase::InParamAlloc(void* c, int len) {
	if (inparams != 0) {
		if (len != inplen) {
			free(inparams);
			inparams = 0;
		}
		else {
			memcpy(inparams, c, inplen);
			return;
		}
	}
	inplen = len;
	inparams = malloc(inplen);
	if (inparams == 0) {
		inplen = 0;
		return;
	}
	memcpy(inparams, c, inplen);
}

void* StepBlockBase::InParamsGet() {
	return inparams;
}

void StepBlockBase::UpdateRenderPos(rxsPointI pos) {
	RenderPos = pos;
}

rxsPointI* StepBlockBase::GetRenderPos() {
	return &RenderPos;
}

void StepBlockBase::PushIn(rxsImg inimg) {
	rxsImg cp = inimg;
	cp.Data = malloc(cp.Len);
	if (cp.Data == 0) { return; }
	memcpy(cp.Data, inimg.Data, cp.Len);
	InImgGrpsPtr->push_back(inimg);
}

void StepBlockBase::PushIn(const char* imgPath, int flag) {
	Mat img = imread(imgPath, flag);
	rxsImg inimg = { img.channels(),img.data,img.rows * img.cols * img.channels() ,img.rows,img.cols };
	PushIn(inimg);
}

void StepBlockBase::PushIn(CloudEntity ce) {
	CloudEntity CE = ce;
	int tl = CE.pNums * sizeof(rxsPointCouldp);
	CE.Entiy = (rxsPointCouldp*)malloc(tl);
	if (CE.Entiy == 0) { return; }
	memcpy(CE.Entiy, ce.Entiy, tl);
	std::vector<CloudEntity>*  MinG = (std::vector<CloudEntity>*)MultinGrps;
	MinG->push_back(CE);
}

void StepBlockBase::PushIn(void* p) {
	cv::Mat A;
	((cv::Mat*)p)->copyTo(A);
	InMatGrpsPtr->push_back(A);
}

bool StepBlockBase::GetResult(rxsResultReport** rrr) {
	*rrr = &rr;
	return true;
}

//bool StepBlockBase::GetResult(const CloudEntity** oce, int index) {
//	*oce = &outCe;
//	return true;
//}
//
//bool StepBlockBase::GetResult(const rxsImg** ri, int index) {
//	*ri = &outImg;
//	return true;
//}
bool StepBlockBase::GetResult(const CloudEntity** oce, int index) {
	if (index < -1) {
		*oce = &outCe;
	}
	else {
		*oce = (const CloudEntity*)MultResultOut(index);
	}
	return true;
}

bool StepBlockBase::GetResult(const rxsImg** ri, int index) {
	if (index < -1) {
		*ri = &outImg;
	}
	else {
		*ri = (const rxsImg*)MultResultOut(index);
	}
	return true;
}

void StepBlockBase::ResultClr() {
	if (outImg.Data != 0) {
		free(outImg.Data);
		outImg.Data = 0;
	}
	memset(&outImg, 0, sizeof(rxsImg));
}

const char* StepBlockBase::GetCanOutSelecter(int& index) { index = -1; return 0; }

bool StepBlockBase::operator ==(StepBlockBase* sbb) {
	bool v = false;

	v = (_id == sbb->_id);
	if (!v) { return false; }

	v = (_type == sbb->_type);
	if (!v) { return false; }

	int len = strlen(sbb->sbName);
	v = (memcmp(sbName, sbb->sbName, len) == 0);
	if (!v) { return false; }

	v = (memcmp(&RenderPos, &sbb->RenderPos, sizeof(rxsPointI)) == 0);
	if (!v) { return false; }

	return true;
}

StepBlockBase* StepBlockBase::self() {
	return this;
}

bool StepBlockBase::InputParamStringCfgSet(std::string* p, int l) { return true; }

std::string StepBlockBase::InputParamSaveStringGet() { return ""; }

void StepBlockBase::Release() {}

short StepBlockBase::Run() {
	return 0;
}


void StepBlockBase::RunReady() {
	status = 0;
	MultinClr();
	ResultClr();
	rr.clear();
}

bool StepBlockBase::HadCompleted() {
	return !((status & 1) == 1);
}
