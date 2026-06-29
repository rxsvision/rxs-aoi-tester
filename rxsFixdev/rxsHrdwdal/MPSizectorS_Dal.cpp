#include "rxsHrdwdal.h"
#include "proteced.h"
#include <iostream>
#include <stdio.h>

#include <string>
#ifndef _WIN32
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <locale.h>
#define Sleep(a) usleep(1000 * a)
#define scanf_s scanf
#define system(a) printf("Press Enter to continue.\n\n");fgetc(stdin);fgetc(stdin)
#define sprintf_s snprintf
#else
#include "windows.h"
#endif

#include "../SizectorAPI/MPSizectorAPI.h"
#include <time.h> 

#pragma comment(lib,"../SizectorAPI/MPSizectorAPI.lib")


IMPSizector* Sensor = MPSizectorFactory::GetInstance();


MPSizectorDal::MPSizectorDal() {
	//print API version
	printf("API Version : V%2.2f", GetSizectorAPIVersion());

	//check device count
	DeviceCount = Sensor->GetDeviceCount();
	if (DeviceCount <= 0)
	{
		printf("No Device Found!\r\n");
		
	}
}

MPSizectorDal::~MPSizectorDal() {
	delete Sensor;
}




short MPSizectorDal::open(const char* tsPath) {
	int choice = 0;

	bool rtv;
	MPDeviceInfo DeviceInfo;
	for (int i = 0; i < DeviceCount; i++)
	{
		DeviceInfo = Sensor->GetDeviceInfo(i);
#ifndef _WIN32
		printf("  -Name : %ls\r\n", DeviceInfo.DeviceName);
		printf("  -Serial Number : %ls\r\n", DeviceInfo.DeviceSerialNumber);
#else
		wprintf(L"  -Name : %s\r\n", DeviceInfo.DeviceName);
		wprintf(L"  -Serial Number : %s\r\n", DeviceInfo.DeviceSerialNumber);
#endif
		printf("  -FW Version : %.2f\r\n", (float)DeviceInfo.FWVersion);
		printf("  -Minimum SDK Version : %.2f\r\n", DeviceInfo.MinimumSDKVersion);
		printf("  -Sensor Width : %d\r\n", DeviceInfo.SensorWidth);
		printf("  -Sensor Height : %d\r\n", DeviceInfo.SensorHeight);
		printf("  -Sensor Resolution : %d\r\n", DeviceInfo.SensorResolution);
		printf("  -X Max : %.2f\r\n", DeviceInfo.XMax);
		printf("  -Y Max : %.2f\r\n", DeviceInfo.YMax);
		printf("  -Z Max : %.2f\r\n", DeviceInfo.ZMax);
		printf("  -Device Interface : %s\r\n", DeviceInfo.IsUSB30 ? "USB3.0" : "USB2.0");
	}

	rtv = Sensor->Open(Sensor->GetDeviceInfo(choice));
	if (!rtv)
	{
		printf("\r\nOpen device failed.\r\n\r\n");
		return -1;
	}

	bool isInit = false;
	Sensor->GetInitState(&isInit);
	while (!isInit)
	{
		Sleep(1000);
		Sensor->GetInitState(&isInit);
	}






	return 0;
}

void MPSizectorDal::close() {

}

void MPSizectorDal::stop() {

}

bool MPSizectorDal::grabContinuity(bool isAnsy, int gCount) {


	return false;
}




bool MPSizectorDal::ResetConnect() {
	return true;
}

void MPSizectorDal::GetSensorparam() {

}

void MPSizectorDal::SetSensorparam() {

}

int MPSizectorDal::IsCollectionCompleted(unsigned int gCount) {


	return 0;
}

void* MPSizectorDal::DataGet(unsigned& outcount) {


	return 0;
}

rxsPointCouldp* MPSizectorDal::DataGetPointCould(unsigned& num) {


	return 0;
}

void MPSizectorDal::CoordinateConvert() {

}

bool MPSizectorDal::WorldCoordinateSet(u8 wich, float x, float y, float z) {
	return true;
}