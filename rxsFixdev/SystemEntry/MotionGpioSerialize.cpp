
#include "public.h"
#include "private.h"

#include <malloc.h>
#include <thread>

using namespace std;
extern swGtxparam* swGtxper;


MotionGpioSerialize::MotionGpioSerialize() {
	AxGrps = 0;
	DiGrps = 0;
	DoGrps = 0;
	dpThreader = 0;
	inSideMode = 0;
	MU = 0;
	GrpMovePushIndex = 0;
	AxNum = (*swGtxper).AxGrpsMapIndex.size();
	DiNum = (*swGtxper).DinGrps.size();
	DoNum = (*swGtxper).DoutGrps.size();
	ready();
}

MotionGpioSerialize::~MotionGpioSerialize() {
	swGtxper->sysFlag &= (~4);
	if (AxGrps != 0) {
		free(AxGrps);
		AxGrps = 0;
	}
	if (DiGrps != 0) {
		free(DiGrps);
		DiGrps = 0;
	}
	if (DoGrps != 0) {
		free(DoGrps);
		DoGrps = 0;
	}
	((thread*)dpThreader)->join();
	if (dpThreader != 0) {
		delete (thread*)dpThreader;
		dpThreader = 0;
	}
}

AxisInfo* MotionGpioSerialize::operator[](unsigned index) {
	if (index < AxNum) {
		return &AxGrps[index];
	}
	return 0;
}

AxisInfo* MotionGpioSerialize::operator[](const char* AxName) {
	unsigned axI = 0;
	return AxNameToTageInfo(AxName, axI);
}

AxisInfo* MotionGpioSerialize::AxGrpsGet() {
	return AxGrps;
}

dioPort* MotionGpioSerialize::DoGrpsGet() {
	return DoGrps;
}

dioPort* MotionGpioSerialize::DiGrpsGet() {
	return DiGrps;
}

void MotionGpioSerialize::ready() {
	int i = 0;

	swGtxper->Dipsq;
	swGtxper->Dopsq;
	swGtxper->Axpsq;

	if (AxNum > 0) {
		AxGrps = (AxisInfo*)malloc(sizeof(AxisInfo) * AxNum);
		if (AxGrps != 0)
			memset(AxGrps, 0, sizeof(AxisInfo) * AxNum);
		if (AxGrps != 0) {
			for (auto it = (*swGtxper).AxGrpsMapIndex.begin(); it != (*swGtxper).AxGrpsMapIndex.end(); it++) {
				//it->second 为值
				int mi = (*swGtxper).AxGrpsMapIndex[it->first];
				AxGrps[mi].AxName = (char*)it->first.c_str();
			}
			//for(vector<string>::iterator i= swGtxper->Axpsq.begin();i!= swGtxper->Axpsq.end();i++){

			//}
		}
	}
	if (DiNum > 0) {
		DiGrps = (dioPort*)malloc(sizeof(dioPort) * DiNum);
		if (DiGrps != 0)
			memset(DiGrps, 0, sizeof(dioPort) * DiNum);
		i = 0;
		const char* key = 0;
		for (vector<string>::iterator itr = swGtxper->Dipsq.begin(); itr != swGtxper->Dipsq.end(); itr++) {
			key = itr->c_str();
			swGtxper->DinGrps[key].ioName = key;
			if (DiGrps != 0)
				memcpy(&DiGrps[i], &(swGtxper->DinGrps[key]), sizeof(dioPort));
			i += 1;
		}
	}

	if (DoNum > 0) {
		DoGrps = (dioPort*)malloc(sizeof(dioPort) * DoNum);
		if (DoGrps != 0)
			memset(DoGrps, 0, sizeof(dioPort) * DoNum);
		i = 0;
		const char* key = 0;
		for (vector<string>::iterator itr = swGtxper->Dopsq.begin(); itr != swGtxper->Dopsq.end(); itr++) {
			key = itr->c_str();
			swGtxper->DoutGrps[key].ioName = key;
			if (DoGrps != 0)
				memcpy(&DoGrps[i], &(swGtxper->DoutGrps[key]), sizeof(dioPort));
			i += 1;
		}
	}
}

bool MotionGpioSerialize::isMotionPrevOk() {
	if (AxNum < 1) {
		string msg = "移动失败!轴数为0";
		(*swGtxper).SytemLoger.Write((char*)msg.c_str());
		return false;
	}
	return true;
}

bool MotionGpioSerialize::AxNamePrevOk(const char* AxName) {
	if (AxName == 0) {
		string msg = "移动失败!轴名称参数为NULL";
		(*swGtxper).SytemLoger.Write((char*)msg.c_str());
		return false;
	}
	return true;
}

AxisInfo* MotionGpioSerialize::AxNameToTageInfo(const char* AxName, unsigned& AxNo) {
	AxNo = swGtxper->AxGrpsMapIndex[AxName];
	if (AxNo < AxNum) {
		//AxNo += 1;
		return &AxGrps[AxNo];
	}
	return 0;
}

u8 MotionGpioSerialize::HomeModeAdj() {

	return 0;
}

short MotionGpioSerialize::Open(MotionUnit* mcu, unsigned  mNum) {
	u8 x = 0;
	MU = mcu;
	if (MU == 0) { return -1; }
	if (MU->Open(0) == 0) {
		x += 1;
	}
	swGtxper->sysFlag |= 4;
	dpThreader = new std::thread(&MotionGpioSerialize::RunSignalProcess, this);
	return x;
}

void MotionGpioSerialize::Close() {
	swGtxper->sysFlag &= (~4);
	if (MU != 0) {
		MU->Close();
	}
	for (unsigned short i = 0; i < AxNum; i++) {
		if (AxGrps[i].GrpData.SalveTable != 0) {
			ClearAxExtraData(i + 1);
		}
	}
}

short MotionGpioSerialize::RunSignalProcess() {
	while ((swGtxper->sysFlag & 4) == 4) {
		for (unsigned i = 0; i < AxNum; i++) {
			MU->GetAxVelPos(i, AxGrps[i]);
			MU->GetAxStatus(i, AxGrps[i]);

			if (inSideMode) {
				AxSignalStsdeal(i);
			}
			else {

			}
		}
		GetioSignal();
		_sleep(5);
	}
	return 0;
}

void MotionGpioSerialize::AxOrgHome(unsigned AxisNo) {
	unsigned AxIndex = AxisNo - 1;
	if (MU == 0) { return; }
	MU->AxStop(AxisNo, true);
	AxGrps[AxIndex].Mode = PP;
	if (AxGrps[AxIndex].Status.OnOrg)
	{
		AxGrps[AxIndex].TageVel = AxGrps[AxIndex].Rethome.Vel;
		AxGrps[AxIndex].TagePos = AxGrps[AxIndex].Rethome.ZeroCorrectpos;
		MU->PpMoveRel(AxisNo, AxGrps[AxIndex]);
		AxGrps[AxIndex].Rethome.Flag = 6;
	}
	else {
		AxGrps[AxIndex].TageVel = AxGrps[AxIndex].Rethome.Vel;
		if (AxGrps[AxIndex].Rethome.Ispostive) {
			AxGrps[AxIndex].TagePos = 1000000000;
		}
		else {
			AxGrps[AxIndex].TagePos = -1000000000;
		}
		MU->PpMoveAbs(AxisNo, AxGrps[AxIndex]);
		AxGrps[AxIndex].Rethome.Flag = 1;
	}
}

//其中 Flag  标志 各数值含义 3 为一段到位之后第二段位移到位 6 为 原点回原点中, 8 为 原点补偿中，9为需要清零位置
void MotionGpioSerialize::AxSignalStsdeal(unsigned aindex) {
	long bm = 1 << aindex, ax = 0;
	unsigned short axisid = aindex + 1;
	if (AxGrps[aindex].Rethome.Flag != 0) {//回原点中的检测
		if (AxGrps[aindex].Status.OnLimtP)
		{
			AxGrps[aindex].TagePos = -1000000000;
			AxGrps[aindex].TageVel = AxGrps[aindex].Rethome.Vel;
			MU->PpMoveAbs(axisid, AxGrps[aindex]);
			AxGrps[aindex].Rethome.Flag = 1;
			return;
		}
		if (AxGrps[aindex].Status.OnLimtN)
		{
			AxGrps[aindex].TagePos = 1000000000;
			AxGrps[aindex].TageVel = AxGrps[aindex].Rethome.Vel;
			MU->PpMoveAbs(axisid, AxGrps[aindex]);
			AxGrps[aindex].Rethome.Flag = 1;
			return;
		}
		if (AxGrps[aindex].Status.IsInp || AxGrps[aindex].Status.IsMotionFinsh) {//对应轴运动静止
			if ((AxGrps[aindex].Rethome.Flag == 6 || AxGrps[aindex].Rethome.Flag == 3)) {//此处与下面不能优化在同一个地方
				AxGrps[aindex].Rethome.Flag = 2;
				return;
			}
			if (AxGrps[aindex].Rethome.Flag == 2) {//此处与上面不能优化在同一个地方
				AxGrps[aindex].TageVel = AxGrps[aindex].Rethome.ZeroCorrectvel;
				AxGrps[aindex].TagePos = 0 - AxGrps[aindex].Rethome.ZeroCorrectpos * 2;
				MU->AxHome(axisid, AxGrps[aindex]);
				return;
			}

			if (AxGrps[aindex].Rethome.Flag == 8) {//原点补偿运动，在第八步的时候前置条件已经准备完毕	
				if (AxGrps[aindex].LimitPos.Homeoffset != 0) {//补偿前置条件准备
					AxGrps[aindex].TageVel = AxGrps[aindex].Rethome.ZeroCorrectvel;
					AxGrps[aindex].TagePos = AxGrps[aindex].LimitPos.Homeoffset;
					MU->PpMoveRel(axisid, AxGrps[aindex]);
				}
				AxGrps[aindex].Rethome.Flag = 9;
				return;
			}
			if (AxGrps[aindex].Rethome.Flag == 9)
			{
				MU->ClearPos(axisid);
				AxGrps[aindex].Rethome.Flag = 10;
				return;
			}
			if (AxGrps[aindex].Rethome.Flag == 10) {
				if (abs((int)AxGrps[aindex].Status.EncPos) < 2 && (int)(AxGrps[aindex].Status.prfPos * 1000) == 0)
				{
					AxGrps[aindex].Rethome.Flag = 0;
					//并设置单轴软限位
					if (AxGrps[aindex].LimitPos.Positive != 0 && AxGrps[aindex].LimitPos.Negative != 0)
					{
						MU->SetSoftLimit(axisid, AxGrps[aindex].LimitPos.Positive, AxGrps[aindex].LimitPos.Negative);
					}
					return;
				}
				AxGrps[aindex].Rethome.Flag = 9;
				return;
			}
			if (AxGrps[aindex].Rethome.Flag == 4) {//此处不要与上面 1 段的时候优化在一起，解决需要延迟问题
				AxGrps[aindex].TageVel = AxGrps[aindex].Rethome.Vel;
				AxGrps[aindex].TagePos = AxGrps[aindex].Rethome.ZeroCorrectpos;
				MU->PpMoveRel(axisid, AxGrps[aindex]);
				AxGrps[aindex].Rethome.Flag = 3;
				return;
			}
		}
		if (AxGrps[aindex].Rethome.Flag < 3) {
			if (AxGrps[aindex].Rethome.Flag == 2 && AxGrps[aindex].Status.OnOrg) {
				MU->AxStop(axisid, false);
				MU->ClearStatusError(3);
				AxGrps[aindex].Rethome.Flag = 8;
				MU->MoveOnHome(aindex);
				return;
			}
			if (AxGrps[aindex].Rethome.Flag == 1 && AxGrps[aindex].Status.OnOrg) {
				MU->AxStop(axisid, true);
				AxGrps[aindex].Rethome.Flag = 4;//此处不要与上面 flag = 4 段的时候优化在一起，解决需要延迟问题
				return;
			}
		}
	}
}

void MotionGpioSerialize::GetConverval(unsigned short Axisindex) {}

void MotionGpioSerialize::ConvertIOBuff(int iv, int ov) {
	int maxV = DiNum > DoNum ? DiNum : DoNum;
	int cmpv = 0;
	for (unsigned x = 0; x < maxV; x++) {
		cmpv = 1 << x;
		if (x < DiNum) {
			DiGrps[x].curStatus = ((iv & cmpv) == cmpv);
		}

		if (x < DoNum) {
			DoGrps[x].curStatus = ((ov & cmpv) == cmpv);
		}
	}
}

void MotionGpioSerialize::GetioSignal() {
	if (MU == 0) { return; }
	int iv = MU->GetInput(INTMAXVAL);
	int ov = MU->GetOutput(INTMAXVAL);
	ConvertIOBuff(iv, ov);
}

bool MotionGpioSerialize::AxHome(const char* AxName) {
	if (AxName == 0) {
		swGtxper->SytemLoger.Write((char*)"AxName == 0 进行了全部轴回零操作!");
		//全轴回零
		if (inSideMode) {

		}
		else {
			for (unsigned x = 0; x < AxNum; x++) {
				AxGrps[x].Rethome.Flag = 1;
			}
			MU->AxHome(0, *AxGrps);
		}
		return true;
	}
	else {
		string msg = AxName;
		swGtxper->SytemLoger.Write((char*)(msg + "进行了回零操作!").c_str());
		if (!isMotionPrevOk()) {
			return true;
		}
		AxisInfo* A = 0;
		unsigned AxNo = 0;
		if ((A = AxNameToTageInfo(AxName, AxNo)) != 0) {
			if (inSideMode) {
				AxOrgHome(AxNo);
			}
			else {
				A->Rethome.Flag = 1;
				MU->AxHome(AxNo, *A);
			}
			return true;
		}
	}
	return true;
}

bool MotionGpioSerialize::AxStop(const char* AxName) {
	if (MU == 0) { return true; }
	if (AxName == 0) {
		//全轴停止
		MU->AxStop(INTMAXVAL, false);
		return true;
	}
	unsigned AxNo = 0;
	AxisInfo* axi = AxNameToTageInfo(AxName, AxNo);
	MU->AxStop(AxNo, false);
	return true;
}

bool MotionGpioSerialize::AxServon(bool on_off, const char* AxName) {
	if (MU == 0) { return false; }
	if (AxName == 0) {
		//全轴使能
		MU->Servon(INTMAXVAL, on_off);
		return true;
	}
	AxisInfo* A = 0;
	unsigned AxNo = 0;
	if ((A = AxNameToTageInfo(AxName, AxNo)) != 0) {
		MU->Servon(AxNo, on_off);
		return true;
	}
	return false;
}

bool MotionGpioSerialize::IsMotionFinsh(const char* AxName) {
	if (!isMotionPrevOk()) {
		return false;
	}
	unsigned AxNo = 0;
	AxisInfo* A = AxNameToTageInfo(AxName, AxNo);
	if (A == 0) { return false; }
	return (A->Status.IsMotionFinsh);
}

bool MotionGpioSerialize::IsMotionInp(const char* AxName) {
	if (!isMotionPrevOk()) {
		return false;
	}
	unsigned AxNo = 0;
	AxisInfo* A = AxNameToTageInfo(AxName, AxNo);
	if (A == 0) { return false; }
	return (A->Status.IsInp);
}


bool MotionGpioSerialize::IsOrgFinshed(const char* AxName) {
	if (AxName == 0) {
		for (unsigned x = 0; x < AxNum; x++) {
			if (AxGrps[x].Rethome.Flag != 0) {
				return false;
			}
		}
		return true;
	}
	unsigned AxNo = 0;
	AxisInfo* A = AxNameToTageInfo(AxName, AxNo);
	if (A == 0) { return false; }
	return (A->Rethome.Flag == 0);
}


bool MotionGpioSerialize::AxMove(const char* AxName, u8 method, float TageVel, float TagePos, float acc, float dec) {
	if (!isMotionPrevOk()) {
		return false;
	}
	if (!AxNamePrevOk(AxName)) {
		return false;
	}
	if (AxGrps == 0) {
		string msg = AxName;
		msg += "移动失败!严重错误:轴组信息为NULL是否已经正确分配？";
		(*swGtxper).SytemLoger.Write((char*)msg.c_str());
		return false;
	}
	string key = AxName;
	int mi = ((*swGtxper).AxGrpsMapIndex)[key];
	AxisInfo* axi = &AxGrps[mi];
	if (acc > 0.00001f) {
		axi->MotionParam.acc = acc;
	}
	else {
		axi->MotionParam.acc = axi->RunAcc;
	}
	if (dec > 0.00001f) {
		axi->MotionParam.dec = dec;
	}
	else {
		axi->MotionParam.dec = axi->RunDec;
	}
	if (TageVel != 0.0f) {
		axi->TageVel = TageVel;
	}
	else {
		axi->TageVel = axi->RunVel;
	}
	if (TagePos != 0.0f) {
		axi->TagePos = TagePos;
	}
	if (method == 'A') {
		MU->PpMoveAbs(mi, AxGrps[mi]);
		return true;
	}
	if (method == 'R') {
		MU->PpMoveRel(mi, AxGrps[mi]);
		return true;
	}
	if (method == '+' || method == '-') {
		if (!AxGrps[mi].Status.IsMotionFinsh) { return true; }
		MU->JogMove(mi, method, AxGrps[mi]);
		return true;
	}
	//其他则群组方式运动
	if (AxGrps[mi].Mode == GROUPSALVER) {
		string msg = AxName;
		msg += "该轴当前群组从轴无法运动，请运行对应主轴!";
		(*swGtxper).SytemLoger.Write((char*)msg.c_str());
		return false;
	}
	if (AxGrps[mi].GrpData.SalveCount < 1 || AxGrps[mi].GrpData.SalveTable == 0) {
		string msg = AxName;
		msg += ",错误:该轴当前群组运动从轴参数未设置!";
		(*swGtxper).SytemLoger.Write((char*)msg.c_str());
		return false;
	}
	MU->GroupParamSet(mi, *axi);
	MU->GroupMove(mi, *axi);
	return true;
}

bool MotionGpioSerialize::AxMove(AxMoveparam mvp) {
	return AxMove(mvp.AxName, mvp.method, mvp.TageVel, mvp.TagePos, mvp.acc, mvp.dec);
}

bool MotionGpioSerialize::MoveForRobort(AxMoveparam* grp, int nums) {
	if (grp == 0) { return false; }
	if (nums < 1) { return false; }
	

	return true;
}


void MotionGpioSerialize::ClearAxExtraData(unsigned AxisNo) {
	if (AxisNo > 0) {
		AxisNo -= 1;
		for (unsigned i = 0; i < AxGrps[AxisNo].GrpData.SalveCount; i++) {
			if (AxGrps[AxisNo].GrpData.SalveTable[i].ExtrenData != 0) {
				free(AxGrps[AxisNo].GrpData.SalveTable[i].ExtrenData);
				AxGrps[AxisNo].GrpData.SalveTable[i].ExtrenData = 0;
			}
			
		}
		AxGrps[AxisNo].GrpData.SalveCount = 0;
		free(AxGrps[AxisNo].GrpData.SalveTable);
		AxGrps[AxisNo].GrpData.SalveTable = 0;
	}
}

bool MotionGpioSerialize::SetDoutput(unsigned serIndex, bool on_or_off) {
	if (MU == 0) { return false; }
	return MU->SetOutput(serIndex, on_or_off);
}

bool MotionGpioSerialize::SetDoutput(const char* key, bool on_or_off) {
	if (key == 0) {
		return false;
	}
	dioPort out = (swGtxper->DoutGrps[key]);
	dioPort mapOut = DoGrps[out.index];
	if (mapOut.isLogicOp) {
		return SetDoutput(mapOut.index, !on_or_off);
	}
	else {
		return SetDoutput(mapOut.index, on_or_off);
	}
}

u8 MotionGpioSerialize::GetDinput(unsigned serIndex) {
	if (MU == 0) { return false; }
	return MU->GetInput(serIndex);
}

u8 MotionGpioSerialize::GetDinput(const char* key) {
	if (key == 0) {
		return false;
	}
	dioPort in = (swGtxper->DinGrps[key]);
	u8 r = GetDinput(in.index);
	if (r < 2) {
		return in.isLogicOp == 0 ? 1 : 0;
	}
	return r;
}

bool MotionGpioSerialize::PosCmpSet(const char* AxName, PoscmpSetparam psp, PoscmpPilist pclist) {
	if (!isMotionPrevOk()) {
		return false;
	}
	unsigned AxNo = 0;
	AxisInfo* A = AxNameToTageInfo(AxName, AxNo);
	if (A == 0) { return false; }
	MU->PosCompareSet(AxNo, psp, pclist);
	return true;
}

bool MotionGpioSerialize::GroupMoveOpen(const char* masterAxName, MotionModel mm, unsigned SalveNum) {
	if (!isMotionPrevOk()) {
		return false;
	}
	unsigned AxNo = 0;
	AxisInfo* A = AxNameToTageInfo(masterAxName, AxNo);
	if (A == 0) { return false; }
	A->Mode = mm;
	A->GrpData.MasterNo = AxNo;
	A->GrpData.SalveCount = SalveNum;
	int ts = sizeof(SalveParam) * SalveNum;
	A->GrpData.SalveTable = (SalveParam*)malloc(ts);
	if (A->GrpData.SalveTable == 0) {
		return false;
	}
	memset(A->GrpData.SalveTable, 0, ts);
	GrpMovePushIndex = 0;
	return true;
}


bool MotionGpioSerialize::GroupMoveSalveCfgPush(const char* masterAxName, const char* salveAxName, void* suData, int suSize) {
	if (!isMotionPrevOk()) {
		return false;
	}
	unsigned AxNo = 0, SAxNo = 0;
	AxisInfo* A = AxNameToTageInfo(masterAxName, AxNo);
	if (A->GrpData.SalveTable == 0) {
		return false;
	}
	AxisInfo* SA = AxNameToTageInfo(salveAxName, SAxNo);

	A->GrpData.SalveTable[GrpMovePushIndex].AxNo = SAxNo;
	A->GrpData.SalveTable[GrpMovePushIndex].ExtrenData = malloc(suSize);
	if (A->GrpData.SalveTable[GrpMovePushIndex].ExtrenData == 0) {
		return false;
	}
	memcpy(A->GrpData.SalveTable[GrpMovePushIndex].ExtrenData, suData, suSize);
	SA->GrpData.MasterNo = AxNo;
	SA->Mode = MotionModel::GROUPSALVER;
	GrpMovePushIndex += 1;
	return true;
}

void MotionGpioSerialize::GroupMoveCancel(const char* masterAxName) {
	if (MU == 0) { return; }
	if (!isMotionPrevOk()) {
		return;
	}
	unsigned AxNo = 0;
	AxisInfo* A = AxNameToTageInfo(masterAxName, AxNo);
	if (A == 0) { return; }
	A->GrpData.MasterNo = 0;
	if (A->GrpData.SalveTable == 0) { return; }
	for (unsigned x = 0; x < A->GrpData.SalveCount; x++) {
		A->GrpData.SalveTable[x].AxNo = 0;
		free(A->GrpData.SalveTable[x].ExtrenData);
		A->GrpData.SalveTable[x].ExtrenData = 0;
	}
	free(A->GrpData.SalveTable);
	A->GrpData.SalveTable = 0;
	A->GrpData.SalveCount = 0;
	MU->GroupMoveClose(AxNo, *A);
	A->Mode = MotionModel::PP;
}

bool MotionGpioSerialize::SetAoutput(unsigned serIndex, bool on_or_off) {
	return false;
}

bool MotionGpioSerialize::SetAoutput(char* key, bool on_or_off) {
	return false;
}

float MotionGpioSerialize::GetAinput(unsigned serIndex) {
	return 0.0f;
}

float MotionGpioSerialize::GetAinput(char* key) {
	return 0.0f;
}