
#include "rxsMainFrame.h"
#include "private.h"
#include <thread>


#pragma warning(disable : 4996)

using namespace std;


#define		ParamGrp		((vector<Vpd> *)ParamGprs)



ImageableSensorUnit::ImageableSensorUnit(unsigned index)
{
	errCode = 0;
	ParamGprs = new vector<Vpd>;
	memset(&Sensorinfo, 0, sizeof(ImgableSensorInfo));
	Sensorinfo.ID = index;
	CofigPath = 0;
	sts = 0;
	slectedChannelno = 0;
	StatusFlagSet(INIT);
}

ImageableSensorUnit::~ImageableSensorUnit()
{
	Close();
	BuffClr();
	if (CofigPath != 0) {
		delete CofigPath;
		CofigPath = 0;
	}
	if (Sensorinfo.ExtendData != 0) {
		free(Sensorinfo.ExtendData);
		Sensorinfo.ExtendData = 0;
	}
	if (ParamGprs != 0) {
		delete ParamGrp;
		ParamGprs = 0;
	}
}

void ImageableSensorUnit::SetCfgpath(char* cfgfile) {
	if (cfgfile != 0) {
		if (CofigPath != 0)
			delete CofigPath;

		(CofigPath) = cfgfile;
	}
}
void ImageableSensorUnit::BuffClr() {
	if (Sensorinfo.ptrGrabResult != 0) {
		if (aloct == 0)
			delete[](char*)Sensorinfo.ptrGrabResult;
		else if (aloct == 1)
			delete[](int*)Sensorinfo.ptrGrabResult;
		else if (aloct == 2)
			delete[](float*)Sensorinfo.ptrGrabResult;
		else if (aloct == 3)
			delete[](double*)Sensorinfo.ptrGrabResult;
		Sensorinfo.ptrGrabResult = 0;
	}
	Sensorinfo.BuffSize = 0;
}

//type = 0 char; = 1 int; = 2 float ; = 3 double;
void ImageableSensorUnit::allocBuff(unsigned uSize, u8 type) {
	if (uSize > 0) {
		aloct = type;
		Sensorinfo.BuffSize = uSize;
		if (type == 0) {
			if (Sensorinfo.ptrGrabResult != 0)
				delete[](char*)Sensorinfo.ptrGrabResult;
			Sensorinfo.ptrGrabResult = new char[Sensorinfo.BuffSize];
			memset(Sensorinfo.ptrGrabResult, 0, Sensorinfo.BuffSize);
		}
		else if (type == 1)
		{
			if (Sensorinfo.ptrGrabResult != 0)
				delete[](int*)Sensorinfo.ptrGrabResult;
			Sensorinfo.ptrGrabResult = new int[Sensorinfo.BuffSize];
			memset(Sensorinfo.ptrGrabResult, 0, Sensorinfo.BuffSize * sizeof(int));
		}
		else if (type == 2)
		{
			if (Sensorinfo.ptrGrabResult != 0)
				delete[](float*)Sensorinfo.ptrGrabResult;
			Sensorinfo.ptrGrabResult = new float[Sensorinfo.BuffSize];
			memset(Sensorinfo.ptrGrabResult, 0, Sensorinfo.BuffSize * sizeof(float));
		}
		else if (type == 3)
		{
			if (Sensorinfo.ptrGrabResult != 0)
				delete[](double*)Sensorinfo.ptrGrabResult;
			Sensorinfo.ptrGrabResult = new double[Sensorinfo.BuffSize];
			memset(Sensorinfo.ptrGrabResult, 0, Sensorinfo.BuffSize * sizeof(double));
		}
	}
}

void ImageableSensorUnit::SetExtendData(int size) {
	if (Sensorinfo.ExtendData == 0) {
		Sensorinfo.ExtendData = malloc(size);
		return;
	}
	free(Sensorinfo.ExtendData);
	Sensorinfo.ExtendData = 0;
	SetExtendData(size);
}

short ImageableSensorUnit::Open(const char* tsPath) {
	short r = open(tsPath);
	if (r == 0) {
		StatusFlagSet(ON);
	}
	return r;
}

bool ImageableSensorUnit::Grab(bool isAnsy, unsigned OneOrContinuityNums) {
	bool r = grabContinuity(isAnsy, OneOrContinuityNums);
	if (r) {
		StatusFlagSet(GRABING);
	}
	return r;
}

void ImageableSensorUnit::Stop() {
	stop();
	StatusFlagClr(GRABING);
}

void ImageableSensorUnit::Close() {
	close();
	StatusFlagClr(ON);
}


bool ImageableSensorUnit::ResetConnect() { return true; }
void ImageableSensorUnit::GetSensorparam() {}
void ImageableSensorUnit::SetSensorparam() {}
void ImageableSensorUnit::SpecParamSet(const char* pkey, float v) {};
void ImageableSensorUnit::SpecParamSet(const char* pkey, int v) {};


int ImageableSensorUnit::IsCollectionCompleted(unsigned int gCount) { return 0; }
void* ImageableSensorUnit::DataGet(unsigned& outcount) { return 0; }
rxsPointCouldp* ImageableSensorUnit::DataGetPointCould(unsigned& outcount) { return 0; }

bool ImageableSensorUnit::TiggerTaber(short wich, bool isClose) { return false; }
bool ImageableSensorUnit::expActive(short fno, void* param) { return false; }
bool ImageableSensorUnit::DinGet(unsigned diIndex) { return false; }
void ImageableSensorUnit::DoutSet(unsigned doindex) {}
void ImageableSensorUnit::FrontendDisplayWndPush(void* sd) {};


void ImageableSensorUnit::CoordinateConvert() {}
bool ImageableSensorUnit::WorldCoordinateSet(u8 wich, float x, float y, float z) { return false; }

void ImageableSensorUnit::ChannelSelected(int channelNo) {
	slectedChannelno = channelNo;
}

void ImageableSensorUnit::ChannelSelectedGet(int& ChannelNo) {
	ChannelNo = slectedChannelno;
}

bool ImageableSensorUnit::HasOpen() {
	int v = (int)ON;
	return (((sts & v) == v) && hasConnectioned());
}

void ImageableSensorUnit::InsertGetParams(const char* name, const char* val) {
	vector<Vpd>::iterator itr = std::find(ParamGrp->begin(), ParamGrp->end(), name);
	if (itr != ParamGrp->end()) {
		return;
	}
	string v = (val == 0 ? "" : val);
	Vpd nv = { name, v };

	ParamGrp->push_back(nv);
}

bool ImageableSensorUnit::UpdateGetParams(const char* name, const char* val) {

	vector<Vpd>::iterator itr = std::find(ParamGrp->begin(), ParamGrp->end(), name);
	if (itr == ParamGrp->end()) {
		return false;
	}
	itr->des = val;
	return true;
}

void ImageableSensorUnit::StatusFlagSet(DevStatus ds) {
	sts |= (int)ds;
}

void ImageableSensorUnit::StatusFlagClr(DevStatus ds) {
	sts &= (~((int)ds));
}

bool ImageableSensorUnit::isGrabing() {
	return ((sts & DevStatus::GRABING) == DevStatus::GRABING);
}

short ImageableSensorUnit::open(const char* tsPath) { return true; }
void ImageableSensorUnit::close() {}
void ImageableSensorUnit::stop() {}
bool ImageableSensorUnit::grabContinuity(bool isAnsy, int gCount) { return true; }
bool ImageableSensorUnit::hasConnectioned() { return true; }

/*
* 以下运动板卡的接口
*/
MotionUnit::MotionUnit(unsigned serno) {
	Serno = serno;
	EsData = NULL;
	errCode = 0;
	AxNum = 0;
	DiNum = 0;
	DoNum = 0;
	UnitType = muType::UNKNOW;
}

MotionUnit::~MotionUnit() {}

short MotionUnit::Open(char* tsPath) { return true; }
void MotionUnit::ClearPos(unsigned AxisNo) {}
bool MotionUnit::ClearStatusError(u8 SOE) { return true; }
void MotionUnit::JogMove(unsigned AxisNo, byte dir, AxisInfo& Axi) {}
void MotionUnit::PpMoveAbs(unsigned AxisNo, AxisInfo& Axi) {}
void MotionUnit::PpMoveRel(unsigned AxisNo, AxisInfo& Axi) {}
void MotionUnit::GroupMove(unsigned AxisNo, AxisInfo& Axi) {}
void MotionUnit::GroupParamSet(unsigned mAxisNo, AxisInfo& Axi) {}
void MotionUnit::GroupMoveClose(unsigned mAxisNo, AxisInfo& Axi) {}
bool MotionUnit::Servon(unsigned AxisNo, bool OnOff) { return true; }
void MotionUnit::AxStop(unsigned AxisNo, bool isSmooth) {}
void MotionUnit::AxHome(unsigned AxisNo, AxisInfo& Axi) {}
void MotionUnit::GetAxStatus(unsigned AxisNo, AxisInfo& Axi) {}
void MotionUnit::GetAxVelPos(unsigned AxisNo, AxisInfo& Axi) {}
bool MotionUnit::SetOutput(unsigned oSernum, bool Value) { return true; }
int MotionUnit::GetInput(unsigned index) { return 0; }
int MotionUnit::GetOutput(unsigned index) { return 0; }
void MotionUnit::Close() {}

void MotionUnit::SetSoftLimit(unsigned AxisNo, long Ppos, long Npos) {}
void MotionUnit::MoveOnHome(unsigned AxisNo) {}


void MotionUnit::PosCompareSet(unsigned AxisNo, PoscmpSetparam&, PoscmpPilist&) {}




/*
以下设备硬件基类接口对象
*/

FHWObject::FHWObject() {
	flag = 0;
}

FHWObject::~FHWObject() {

}

short FHWObject::Open(const char* cmdline) {
	if ((flag & 0x80) != 0x80)
	{
		Init();
		flag |= 0x80;
	}

	short ret = Link(cmdline);
	if (ret == 0) {
		flag |= 1;
	}
	return ret;
}

int FHWObject::Request(void* d, unsigned len) {
	flag |= 2;
	int ret = Writer(d, len);
	flag &= (~2);
	return ret;
}

void* FHWObject::Response(unsigned& len) {
	flag |= 4;
	void* ret = Reader(len);
	flag &= (~4);
	return ret;
}

void FHWObject::Init() {}
void* FHWObject::Reader(unsigned& len) { return 0; }
int FHWObject::Writer(void*, unsigned len) { return 0; }
short FHWObject::Link(const char* cmdline) { return 0; }
short FHWObject::unLink() { return 0; }

bool FHWObject::Close() {
	return (unLink() == 0);
}


