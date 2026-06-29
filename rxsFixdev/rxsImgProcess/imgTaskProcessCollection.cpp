#include "private.h"
#include "protected_rxsToolKit.h"
#include <thread>
#include <malloc.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <io.h>


#define		ImgTaskCollection	((std::vector<imgTaskFlowerProcesser*>*)imgTaskGrps)

HrdWrkSyetemEnv hrdSyetemEnver;



imgTaskProcessCollection::imgTaskProcessCollection(rxsResultReportGrpsManger* ras, rxsMainFrame* fSystemer, void* rtk) {
	imgTaskGrps = new std::vector<imgTaskFlowerProcesser*>;
	curItp = 0;
	hrdSyetemEnver.pSystemer = fSystemer;
	hrdSyetemEnver.rrpmg = ras;
	hrdSyetemEnver.pSystemer->SysLogRecord("imgTaskProcessCollection-构造执行完成!");
	hrdSyetemEnver.rtk = (rxsToolKit*)rtk;
	hrdSyetemEnver.rtk2d = new rxsToolKit2D;
}

imgTaskProcessCollection::~imgTaskProcessCollection() {
	if (hrdSyetemEnver.rtk2d != 0) {
		delete hrdSyetemEnver.rtk2d;
		hrdSyetemEnver.rtk2d = 0;
	}
	if (imgTaskGrps != 0) {
		int l = ImgTaskCollection->size();
		for (unsigned x = 0; x < l; x++) {
			delete (*ImgTaskCollection)[x];
			(*ImgTaskCollection)[x] = 0;
		}
		(*ImgTaskCollection).clear();
		delete (std::vector<imgTaskFlowerProcesser*> *)imgTaskGrps;
		imgTaskGrps = 0;
	}
	curItp = 0;
}

int imgTaskProcessCollection::Count() {
	return ImgTaskCollection->size();
}

imgTaskFlowerProcesser* imgTaskProcessCollection::operator[](int index) {
	int l = ImgTaskCollection->size();
	if (l > index) {
		return (*ImgTaskCollection)[index];
	}
	return 0;
}

imgTaskFlowerProcesser* imgTaskProcessCollection::operator[](const char* tskName) {
	if (tskName == 0) { return 0; }
	int l = ImgTaskCollection->size();
	std::string ss = tskName;
	imgTaskFlowerProcesser* temp = 0;
	for (unsigned x = 0; x < l; x++) {
		temp = (*ImgTaskCollection)[x];
		if (ss == temp->FlowerName()) {
			return temp;
		}
	}
	return 0;
}

imgTaskFlowerProcesser* imgTaskProcessCollection::Selected(int index) {
	curItp = (*this)[index];
	return curItp;
}

imgTaskFlowerProcesser* imgTaskProcessCollection::Selected(const char* tskName) {
	curItp = (*this)[tskName];
	return curItp;
}

void imgTaskProcessCollection::TaskFlowerProcessPush(const char* tskName)
{
	imgTaskFlowerProcesser* ipd1 = new imgTaskFlowerProcesser(tskName);
	ImgTaskCollection->push_back(ipd1);
}

void imgTaskProcessCollection::TaskFlowerProcessPush(imgTaskFlowerProcesser* ipd) {
	imgTaskFlowerProcesser* ipdx = new imgTaskFlowerProcesser(ipd->FlowerName());
	ipd->CopyTo(ipdx);
	ImgTaskCollection->push_back(ipdx);
}

bool imgTaskProcessCollection::TaskFlowerProcessRemove(imgTaskFlowerProcesser* itp) {
	std::vector<imgTaskFlowerProcesser*>::iterator has = std::find(ImgTaskCollection->begin(), ImgTaskCollection->end(), itp);
	if (has != ImgTaskCollection->end()) {
		ImgTaskCollection->erase(has);
	}
	return true;
}

const char* imgTaskProcessCollection::Foreach(bool& isStart) {
	static int index = 0;
	std::vector<imgTaskFlowerProcesser*>::iterator itp;
	if (isStart) {
		itp = ImgTaskCollection->begin();
		index = 1;
		isStart = false;
	}
	else {
		itp = (ImgTaskCollection->begin() + index);
		index += 1;
	}

	if (itp != ImgTaskCollection->end()) {
		return (*itp)->FlowerName();
	}
	index = 0;
	return 0;
}

void imgTaskProcessCollection::Clear() {
	if (imgTaskGrps != 0) {
		int l = ImgTaskCollection->size();
		if (l == 0) { return; }
		for (unsigned x = 0; x < l; x++) {
			delete (*ImgTaskCollection)[x];
		}
		ImgTaskCollection->clear();
	}
}

short imgTaskProcessCollection::Save(const char* dirPath) {
	if (imgTaskGrps != 0) {
		int l = ImgTaskCollection->size();
		if (l == 0) { return -1; }
		for (unsigned x = 0; x < l; x++) {
			imgTaskFlowerProcesser* curItfper = (*ImgTaskCollection)[x];
			stringstream ss;
			ss << dirPath << curItfper->FlowerName() << ".ifwer";
			curItfper->Save(ss.str().c_str());
		}

	}
	return 0;
}


void imgTaskProcessCollection::anasyloader(int cui, const char* fp, int len) {
	int i = len - 1;
	int ei = -1;
	int si = -1;
	while (i--) {
		if (fp[i] == '.') {
			ei = i - 1;
			continue;
		}
		if (fp[i] == '\\' || fp[i] == '/') {
			si = i + 1;
			break;
		}
	}
	i = ei - si + 1;
	if (i > 0) {
		char* name = (char*)malloc(i + 1);
		if (name != 0) {
			name[i] = 0;
			memcpy(name, &fp[si], i);
			linCreater(name, fp);
			free(name);
			return;
		}
	}
	linCreater((std::to_string(cui) + "noName").c_str(), fp);
}

void imgTaskProcessCollection::linCreater(const char* name, const char* fp) {
	imgTaskFlowerProcesser* itfptr = new imgTaskFlowerProcesser(name);
	itfptr->ResultReportGrpSet(hrdSyetemEnver.rrpmg, hrdSyetemEnver.pSystemer);
	if (itfptr->Load(fp)) {
		ImgTaskCollection->push_back(itfptr);
		return;
	}
	delete itfptr;
	itfptr = 0;
}

short imgTaskProcessCollection::Load(const char* dirPath) {
	Clear();
	intptr_t   hFile = 0;
	struct _finddata_t fileinfo;
	string p;
	int l = 0;
	if ((hFile = _findfirst(p.assign(dirPath).append("\\*").c_str(), &fileinfo)) != -1)//若查找成功，则进入
	{
		do
		{
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				//文件名不等于"."&&文件名不等于".."
				//判断时，两者都要忽略，不然就无限递归跳不出去了！
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
					//getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
				}
			}
			else
			{
				string fp = (p.assign(dirPath).append(fileinfo.name));
				anasyloader(l, fp.c_str(), fp.length());
			}
			l++;
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	return 0;
}


bool imgTaskProcessCollection::Run(bool isAnsy) {
	int ercode = 0;
	if (curItp == 0) { return false; }
	curItp->Ready();
	if (isAnsy) {
		ercode = curItp->RunAnsy();
	}
	else {
		ercode = curItp->Run();
	}
	return (ercode == 0);
}

bool imgTaskProcessCollection::HadFinshed() {
	int rc = ImgTaskCollection->size();
	for (int s = 0; s < rc; s++) {
		if (!(*ImgTaskCollection)[s]->HadFinshed()) {
			return false;
		}
	}
	return true;
}

//
void imgTaskProcessCollection::ResultReportAssember() {
	int rc = ImgTaskCollection->size();
	for (int s = 0; s < rc; s++) {
		(*ImgTaskCollection)[s]->ResultReportSummary();
	}
}
