#pragma once
#include "rxsHrdwdal.h"
#include "proteced.h"
#include <stdio.h>
#include <Windows.h>
#include <process.h>
#include <conio.h>
#include "MvCameraControl.h"
#include <iostream>


HkCream_ch120::HkCream_ch120() {
    Sensorinfo.devType = DevType::Cream;
    DevList = new MV_CC_DEVICE_INFO_LIST;
    Stream = NULL;
    nRet = MV_CC_Initialize();
    unsigned version = MV_CC_GetSDKVersion();
    unsigned int nTLayerType = MV_GIGE_DEVICE | MV_USB_DEVICE | MV_GENTL_GIGE_DEVICE | MV_GENTL_CAMERALINK_DEVICE | MV_GENTL_CXP_DEVICE | MV_GENTL_XOF_DEVICE;
    nRet = MV_CC_EnumDevices(nTLayerType, (MV_CC_DEVICE_INFO_LIST*)DevList);
    int dnum = ((MV_CC_DEVICE_INFO_LIST*)DevList)->nDeviceNum;
    CtrInfo = new ControlInfo[dnum];
    if (CtrInfo != 0)
        memset(CtrInfo, 0, sizeof(ControlInfo) * dnum);
}

HkCream_ch120::~HkCream_ch120() {
    if (DevList != 0) {
        delete ((MV_CC_DEVICE_INFO_LIST*)DevList);
        DevList = 0;
    }
    if (CtrInfo != 0) {
        delete[](ControlInfo*)CtrInfo;
        CtrInfo = 0;
    }
    MV_CC_Finalize();
}


short HkCream_ch120::open(const char* tsPath) {
    MV_CC_DEVICE_INFO_LIST* pstDevList = (MV_CC_DEVICE_INFO_LIST*)DevList;
    if (pstDevList == 0 || pstDevList->nDeviceNum < 1) {
        return -1;
    }
    for (unsigned x = 0; x < pstDevList->nDeviceNum; x++) {
        MV_CC_DEVICE_INFO* pstDeviceInfo = pstDevList->pDeviceInfo[x];
        if (NULL == pstDeviceInfo)
        {
            return MV_E_PARAMETER;
        }
        if (CtrInfo[x].DevHandle != 0)
        {
            return MV_E_CALLORDER;
        }
        nRet = MV_CC_CreateHandle(&CtrInfo[x].DevHandle, pstDeviceInfo);
        if (MV_OK != nRet)
        {
            return nRet;
        }
        nRet = MV_CC_OpenDevice(CtrInfo[x].DevHandle);
        if (MV_OK != nRet)
        {
            nRet = MV_CC_DestroyHandle(CtrInfo[x].DevHandle);
            CtrInfo[x].DevHandle = NULL;
        }

        nRet = MV_CC_GetDeviceInfo(CtrInfo[x].DevHandle, pstDeviceInfo);
        slectedChannelno = x;
        GetSensorparam();
        allocBuff(Sensorinfo.BuffSize);
        nRet = MV_CC_StartGrabbing(CtrInfo[slectedChannelno].DevHandle);
    }
    slectedChannelno = 0;
    return nRet;
}

void HkCream_ch120::close() {
    MV_CC_DEVICE_INFO_LIST* pstDevList = (MV_CC_DEVICE_INFO_LIST*)DevList;
    if (pstDevList == 0 || pstDevList->nDeviceNum < 1 || CtrInfo == 0) {
        return;
    }
    for (int x = 0; x < pstDevList->nDeviceNum; x++) {
        if (CtrInfo[x].DevHandle != 0)
        {
            nRet = MV_CC_StopGrabbing(CtrInfo[x].DevHandle);
            nRet = MV_CC_CloseDevice(CtrInfo[x].DevHandle);
            nRet = MV_CC_DestroyHandle(CtrInfo[x].DevHandle);
            CtrInfo[x].DevHandle = 0;
        }
    }
}

void HkCream_ch120::stop() {
    //nRet = MV_CC_StopGrabbing(CtrInfo[slectedChannelno].DevHandle);
}

bool HkCream_ch120::hasConnectioned() {
    return MV_CC_IsDeviceConnected(CtrInfo[slectedChannelno].DevHandle);
}


bool HkCream_ch120::grabContinuity(bool isAnsy, int gCount) {
    if (CtrInfo[slectedChannelno].DevHandle == 0) { return false; }
    if (Sensorinfo.ptrGrabResult == 0) { return false; }
    if (gCount < 2) {
        MV_FRAME_OUT stImageInfo = { 0 };
        nRet = MV_CC_GetImageBuffer(CtrInfo[slectedChannelno].DevHandle, &stImageInfo, 500);
        stImageInfo.stFrameInfo.nFrameLen;
        stImageInfo.stFrameInfo.enPixelType;
        //CtrInfo[slectedChannelno].Width = stImageInfo.stFrameInfo.nWidth;
        //CtrInfo[slectedChannelno].Height = stImageInfo.stFrameInfo.nHeight;
        //Sensorinfo.BuffSize = stImageInfo.stFrameInfo.nWidth * stImageInfo.stFrameInfo.nHeight;
        //allocBuff(Sensorinfo.BuffSize);
        if (stImageInfo.pBufAddr == 0) { return false; }
        memcpy(Sensorinfo.ptrGrabResult, stImageInfo.pBufAddr, Sensorinfo.BuffSize);
        MV_CC_FreeImageBuffer(CtrInfo[slectedChannelno].DevHandle, &stImageInfo);
        return true;
    }
    else {

    }
    return false;
}

bool HkCream_ch120::ResetConnect() {
    if (hasConnectioned()) {
        return true;
    }
    Close();
    Open(0);
    return true;
}

void HkCream_ch120::GetSensorparam() {
    if (statusIsNotOk()) { return; }
    MVCC_FLOATVALUE pFloatValue = { 0 };
    MVCC_ENUMVALUE stEnumValue = { 0 };
    MVCC_ENUMENTRY stPixelFormatInfo = { 0 };
    MVCC_INTVALUE_EX intValex;

    int iVal = 0;
    double dVal = 0;
    float fVal = 0;
    nRet = MV_CC_GetFloatValue(CtrInfo[slectedChannelno].DevHandle, "Gain", &pFloatValue);
    CtrInfo[slectedChannelno].gain = pFloatValue.fCurValue;

    nRet = MV_CC_GetFloatValue(CtrInfo[slectedChannelno].DevHandle, "ExposureTime", &pFloatValue);
    CtrInfo[slectedChannelno].expTime = pFloatValue.fCurValue;

    nRet = MV_CC_GetEnumValue(CtrInfo[slectedChannelno].DevHandle, "PixelFormat", &stEnumValue);
    nRet = MV_CC_GetEnumEntrySymbolic(CtrInfo[slectedChannelno].DevHandle, "PixelFormat", &stPixelFormatInfo);

    nRet = MV_CC_GetIntValueEx(CtrInfo[slectedChannelno].DevHandle, "Width", &intValex);
    CtrInfo[slectedChannelno].Width = intValex.nCurValue;
    nRet = MV_CC_GetIntValueEx(CtrInfo[slectedChannelno].DevHandle, "Height", &intValex);
    CtrInfo[slectedChannelno].Height = intValex.nCurValue;
    Sensorinfo.BuffSize = CtrInfo[slectedChannelno].Width * CtrInfo[slectedChannelno].Height;
}

void HkCream_ch120::SetSensorparam() {
    if (statusIsNotOk()) { return; }
    nRet = MV_CC_SetFloatValue(CtrInfo[slectedChannelno].DevHandle, "Gain", CtrInfo[slectedChannelno].gain);
    nRet = MV_CC_SetEnumValue(CtrInfo[slectedChannelno].DevHandle, "ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
    nRet = MV_CC_SetFloatValue(CtrInfo[slectedChannelno].DevHandle, "ExposureTime", CtrInfo[slectedChannelno].expTime);
}

int HkCream_ch120::IsCollectionCompleted(unsigned int gCount) {
    return -1;
}

void* HkCream_ch120::DataGet(unsigned& outcount) {
    if (statusIsNotOk()) { return 0; }
    if (CtrInfo[slectedChannelno].DevHandle == 0) { return 0; }
    outcount = Sensorinfo.BuffSize;
    return Sensorinfo.ptrGrabResult;
}

bool HkCream_ch120::TiggerTaber(short wich, bool isClose) {
    return false;
}

bool HkCream_ch120::expActive(short fno, void* param) {
    return false;
}

void HkCream_ch120::CoordinateConvert() {

}

bool HkCream_ch120::WorldCoordinateSet(u8 wich, float x, float y, float z) {
    return false;
}

void HkCream_ch120::FrontendDisplayWndPush(void* hwnd) {
    if (statusIsNotOk()) { return; }
    MV_DISPLAY_FRAME_INFO stDisplayInfo = { 0 };
    MV_FRAME_OUT stImageInfo = { 0 };
    nRet = MV_CC_GetImageBuffer(CtrInfo[slectedChannelno].DevHandle, &stImageInfo, 500);
    stDisplayInfo.hWnd = hwnd;
    stDisplayInfo.pData = stImageInfo.pBufAddr;
    stDisplayInfo.nDataLen = stImageInfo.stFrameInfo.nFrameLen;
    stDisplayInfo.nWidth = stImageInfo.stFrameInfo.nWidth;
    stDisplayInfo.nHeight = stImageInfo.stFrameInfo.nHeight;
    stDisplayInfo.enPixelType = stImageInfo.stFrameInfo.enPixelType;
    nRet = MV_CC_DisplayOneFrame(CtrInfo[slectedChannelno].DevHandle, &stDisplayInfo);
    nRet = MV_CC_FreeImageBuffer(CtrInfo[slectedChannelno].DevHandle, &stImageInfo);
}

bool HkCream_ch120::statusIsNotOk() {
    MV_CC_DEVICE_INFO_LIST* pstDevList = (MV_CC_DEVICE_INFO_LIST*)DevList;
    if (pstDevList == 0 || pstDevList->nDeviceNum < 1 || CtrInfo == 0) {
        return true;
    }
    return false;
}

void HkCream_ch120::SpecParamSet(const char* pkey, float v) {
    MV_CC_SetFloatValue(CtrInfo[slectedChannelno].DevHandle, pkey, v);
}

void HkCream_ch120::SpecParamSet(const char* pkey, int v) {
    MV_CC_SetIntValue(CtrInfo[slectedChannelno].DevHandle, pkey, v);
}