#pragma once
#include "rxsMainFrame.h"
#include "private.h"
#include <thread>
#include <mutex>

using namespace std;

extern swGtxparam* swGtxper;
typedef std::map<std::string, ImageableSensorUnit*>::iterator	isuGrpsIter;


ImageableGrpManger::ImageableGrpManger()
{
	slectedName = 0;
}

ImageableGrpManger::~ImageableGrpManger()
{
	if (isuGrps.size() > 0) {
		for (isuGrpsIter itr = isuGrps.begin(); itr != isuGrps.end(); itr++) {
			if (itr->second != 0) {
				itr->second->Close();
				delete itr->second;
				itr->second = 0;
			}
		}
		isuGrps.clear();
	}
}

bool ImageableGrpManger::Push(std::string name, ImageableSensorUnit* isu) {
	if (name.length() < 1 || isu == 0) { return false; }
	if (isuGrps.size() > 0) {
		isuGrpsIter a = isuGrps.find(name);
		if (a == isuGrps.end()) {
			isuGrps[name] = isu;
		}
		else {
			return true;
		}
	}
	else {
		isuGrps[name] = isu;
	}
	return true;
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
	return 0;
}

const char* ImageableGrpManger::SelectedName() {
	return slectedName;
}

bool ImageableGrpManger::HasOpen(int& index) {
	bool ret = true;
	for (iter = isuGrps.begin(); iter != isuGrps.end(); iter++) {
		if (!(ret = iter->second->HasOpen()))
		{
			break;
		}
	}
	iter = isuGrps.end();
	return ret;
}


const char* ImageableGrpManger::GrpsNameForeach(bool& isStart, ImageableSensorUnit::DevType& isuType) {
	const char* v = 0;
	if (isStart) {
		iter = isuGrps.begin();
		isStart = false;
	}
	if (iter != isuGrps.end()) {
		v = iter->first.c_str();
		isuType = iter->second->Sensorinfo.devType;
		iter++;
		return v;
	}
	return 0;
}


ImageableGather::ImageableGather() {
	Gxter = 0;
	WrkParam = 0;
	FacParam = 0;
	curOpter = 0;
	Method = 0;
	ercode = 0;
	MotionGpio = swGtxper->MotionGpioSer;
	curTxopter = 0;
	attenData = 0;
	Init();
}

ImageableGather::~ImageableGather() {
	WrkParam = 0;
	FacParam = 0;
	curOpter = 0;
	Method = 0;
	Gxter = 0;
}

void ImageableGather::AllocAttenData(int size) {
	if (attenData != 0) {
		free(attenData);
		attenData = 0;
	}
	attenData = malloc(size);
	if (attenData == 0) { return; }
	memset(attenData, 0, size);
}

void* ImageableGather::AttenDataGet() {
	return attenData;
}

short ImageableGather::errCode() {
	return ercode;
}

void ImageableGather::errCodeSet(short code) {
	ercode = code;
}

bool ImageableGather::Prepare() {
	if (swGtxper == 0) {
		SysLogRecord("可成像传感器:Ready失败，NULL 参数!");
		return false;
	}
	WrkParam = (WorkParamManger*)(swGtxper->WorkParamer);
	FacParam = (FactoryConfigManger*)(swGtxper->FactoryParamer);
	//mGpio = swGtxper->MotionGpioSer;
	return SensorCreater();
}

void ImageableGather::PushImageableGrp(const char* name, ImageableSensorUnit* isu) {
	if (name == 0 || isu == 0) { return; }
	if (swGtxper->imgableGrpMgr == 0) { return; }
	swGtxper->imgableGrpMgr->Push(name, isu);
	curOpter = swGtxper->imgableGrpMgr->Selected(name);
}

ImageableSensorUnit* ImageableGather::Selected(const char* sName) {
	if (sName == 0) {
		SysLogRecord("选中失败! sName为NULL！");
		return 0;
	}
	std::string cursn = sName;
	SysLogRecord((cursn + ":传感器被选中!").c_str());
	curOpter = swGtxper->imgableGrpMgr->Selected(sName);
	return curOpter;
}

const char* ImageableGather::curSelectedName() {
	return swGtxper->imgableGrpMgr->SelectedName();
}

ImageableSensorUnit* ImageableGather::CurSelectedGet() {
	return curOpter;
}

void ImageableGather::ReadyParamSet(void* param) {}

short ImageableGather::GatherReady() {
	if (curOpter == 0)
	{
		SysLogRecord("GatherReady失败,当前采集器对象为NULL，请确认当前已经选中！");
		return -2;
	}
	ercode = 0;
	GatherPrevProcess(curOpter);
	return (ercode == 0) ? 0 : ercode;
}

short ImageableGather::GatherStop() {
	if (curOpter == 0)
	{
		SysLogRecord("GatherStop失败，当前采集器对象为NULL，请确认当前已经选中！");
		return -1;
	}
	ercode = 0;
	GatherEndProcess(curOpter);
	return  (ercode == 0) ? 0 : ercode;
}

void* ImageableGather::GatherResultGet(unsigned long& count) {
	if (curOpter == 0)
	{
		SysLogRecord("当前采集器对象为NULL！");
		return 0;
	}
	//if (Method == 1) {
	//	//异步采集
	//	if (curOpter->IsCollectionCompleted(0) > 0) {
	//		return curOpter->DataGet((unsigned&)count);
	//	}
	//}
	//else if (Method == 2) {
	//	//同步采集
	//	return curOpter->DataGet((unsigned &)count);
	//}
	//else {
	//	//采集方式无效
	//	return 0;
	//}
	//SysLogRecord("采集方式无效,DataGet()失败！");
	return curOpter->DataGet((unsigned&)count);
}

bool ImageableGather::GatherAsynchronous(ImageableSensorUnit* g) {
	curTxopter = g;
	thread trd_wrkp([this]() {
		Gather(curTxopter);
		});
	trd_wrkp.detach();
	return true;
}

bool ImageableGather::Gather(ImageableSensorUnit* g) {
	if (g == 0) {
		g = CurSelectedGet();
	}
	ercode = 0;
	return g->Grab(true, true); 
}
void ImageableGather::Init() {};
bool ImageableGather::GatherPrevProcess(ImageableSensorUnit* g) { return true; }
bool ImageableGather::GatherEndProcess(ImageableSensorUnit*) { return true; }

bool ImageableGather::SensorCreater() { return false; }
void ImageableGather::Close() {
	if (attenData != 0) {
		free(attenData);
		attenData = 0;
	}
}