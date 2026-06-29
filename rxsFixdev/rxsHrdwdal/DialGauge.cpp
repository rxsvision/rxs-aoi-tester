#include "rxsHrdwdal.h"



DialGauge::DialGauge() {
	val = 0.0f;
}


DialGauge::~DialGauge() {

}

short DialGauge::open(const char* tsPath) {
	if (tsPath == 0) {
		return -1;
	}


	return 0;
}

bool DialGauge::ResetConnect() {
	return true;
}


bool DialGauge::grabContinuity(bool isAnsy, int gCount){
	return true;
}

void DialGauge::GetSensorparam() {

}

void DialGauge::SetSensorparam() {

}

void DialGauge::close() {

}

int DialGauge::IsCollectionCompleted(unsigned int gCount) {
	return 0;
}

void* DialGauge::DataGet(unsigned& outcount) {
	return 0;
}

void DialGauge::stop() {

}

bool DialGauge::TiggerTaber(short wich, bool isClose) {
	return true;
}

bool DialGauge::expActive(short fno, void* param)
{
	return true;
}

void DialGauge::CoordinateConvert() {
}

bool DialGauge::WorldCoordinateSet(u8 wich, float x, float y, float z) {
	return true;
}
