#pragma once

#include "rxsFixdal.h"
#include "protected.h"
#include <map>

#define isuGrpsPtr		((map<string,ImageableSensorUnit *> *)objGrps)
#define isuGrps			(*(isuGrpsPtr))
#define isuGrpsIterPtr		((isuGrpsIter*)iter)


typedef std::map<std::string, ImageableSensorUnit*>::iterator	isuGrpsIter;

using namespace std;



ImageableGrpManger::ImageableGrpManger()
{
	slectedName = 0;
	objGrps = new map<string, ImageableSensorUnit*>;
	iter = 0;
}

ImageableGrpManger::~ImageableGrpManger()
{
	if (objGrps != 0)
	{
		delete isuGrpsPtr;
		objGrps = 0;
	}
}

void ImageableGrpManger::Push(std::string name, ImageableSensorUnit* isu) {
	isuGrpsIter a = isuGrps.find(name);
	if (a == isuGrps.end()) {
		isuGrps[name] = isu;
	}
}

void ImageableGrpManger::Remove(std::string name) {
	isuGrpsIter a = isuGrps.find(name);
	if (a != isuGrps.end()) {
		isuGrps.erase(name);
	}
}

int ImageableGrpManger::Count() {
	return isuGrps.size();
}

ImageableSensorUnit* ImageableGrpManger::Selected(const char* name) {
	isuGrpsIter a = isuGrps.find(name);
	if (a != isuGrps.end()) {
		slectedName = a->first.c_str();
		return a->second;
	}
	else {
		slectedName = 0;
	}
	return 0;
}


const char* ImageableGrpManger::GrpsNameForeach() {
	const char* v = 0;
	if (iter == 0) {
		isuGrpsIter A = isuGrps.begin();
		iter = &A;
		v = (*isuGrpsIterPtr)->first.c_str();
		(*isuGrpsIterPtr)++;
		return v;
	}
	if (*isuGrpsIterPtr != isuGrps.end()) {
		v = (*isuGrpsIterPtr)->first.c_str();
		(*isuGrpsIterPtr)++;
		return v;
	}
	isuGrpsIter B = isuGrps.begin();
	iter = &B;
	return 0;
}





ImageableGather::ImageableGather(WorkParamManger* wrkpm, FactoryConfigManger* facpm, ImageableGrpManger *igm) {
	wrkParam = wrkpm;
	facParam = facpm;
	ImageableGrps = igm;
	curOpter = 0;
	Method = 0;
}

ImageableGather::~ImageableGather() {
	wrkParam = 0;
	facParam = 0;
	ImageableGrps = 0;
	curOpter = 0;
	Method = 0;
}

short ImageableGather::GatherReady(const char* name) {
	curOpter = ImageableGrps->Selected(name);
	if (curOpter == 0)
	{
		return -1;
	}
	short errCode = 0;
	return GatherReady(curOpter, errCode) ? 0 : errCode;
}

short ImageableGather::Gather(u8 method) {
	if (curOpter == 0)
	{
		return -1;
	}
	short errCode = 0;
	Method = method;
	return Gather(curOpter, errCode) ? 0 : errCode;
}

void* ImageableGather::DataGet(unsigned &count) {
	if (curOpter == 0)
	{
		return 0;
	}
	if (Method == 1) {
		//异步采集
		if (curOpter->IsCollectionCompleted(0) > 0) {
			return curOpter->DataGet(count);
		}
	}
	else if(Method ==2){
		//同步采集
		return curOpter->DataGet(count);
	}
	else {
		//采集方式无效
		return 0;
	}
}

bool ImageableGather::Gather(ImageableSensorUnit* g, short& errCode) {
	g->GrabTiggerContinuity();
	return true;
}

bool ImageableGather::GatherReady(ImageableSensorUnit* g, short& errCode) { return true; }