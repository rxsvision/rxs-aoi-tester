#pragma once
#include "rxsHrdwdal.h"
#include <stdio.h>
#include <string>

#pragma warning(disable : 4996)
#define RXSAPI			extern "C" __declspec(dllexport)



void robotMotion_Dal::resourceCfg() {

}

RXSAPI short robotMotion_Dal::Open(char* CfgPath) {


	ClearStatusError(7);
	resourceCfg();
	return 0;
}

RXSAPI void robotMotion_Dal::Close() {

}

RXSAPI void robotMotion_Dal::ClearPos(unsigned AxisNo) {

}

//清除 状态 及错误，SOE 1为清除 状态，2 为清除错误，3 为两者都清除
RXSAPI bool robotMotion_Dal::ClearStatusError(u8 SOE) {

	int axisstatus = 0;
	for (unsigned x = 0; x < AxNum; x++) {

		if (axisstatus > 0) {

		}
	}
	return true;
}

RXSAPI void robotMotion_Dal::JogMove(unsigned AxisNo, byte dir, AxisInfo& Axi) {

}

RXSAPI void robotMotion_Dal::PpMoveAbs(unsigned AxisNo, AxisInfo& Axi) {

}

RXSAPI void robotMotion_Dal::PpMoveRel(unsigned AxisNo, AxisInfo& Axi) {

}

RXSAPI void robotMotion_Dal::GroupParamSet(unsigned mAxisNo, AxisInfo& Axi) {

}

RXSAPI void robotMotion_Dal::GroupMoveClose(unsigned mAxisNo, AxisInfo& Axi) {

}

RXSAPI void robotMotion_Dal::GroupMove(unsigned AxisNo, AxisInfo& Axi) {

}

RXSAPI bool robotMotion_Dal::Servon(unsigned AxisNo, bool OnOff) {
	return true;
}

RXSAPI void robotMotion_Dal::AxStop(unsigned AxisNo, bool isSmooth) {

}


RXSAPI bool robotMotion_Dal::SetOutput(unsigned oSernum, bool Value) {
	return true;
}

RXSAPI	int robotMotion_Dal::GetInput(unsigned index) {
	return 0;
}

RXSAPI int robotMotion_Dal::GetOutput(unsigned index) {
	return 0;
}

RXSAPI void robotMotion_Dal::AxHome(unsigned AxisNo, AxisInfo& Axi) {

}


RXSAPI void robotMotion_Dal::GetAxStatus(unsigned AxisNo, AxisInfo& Axi) {
	int axisstatus = 0;
	int status = 0;

}

RXSAPI void robotMotion_Dal::GetAxVelPos(unsigned AxisNo, AxisInfo& Axi) {

	float position = 0;

}

RXSAPI void robotMotion_Dal::SetSoftLimit(unsigned AxisNo, long Ppos, long Npos) {

}


void robotMotion_Dal::PosCompareSet(unsigned AxisNo, PoscmpSetparam& Pcsp, PoscmpPilist& PointList) {

}
