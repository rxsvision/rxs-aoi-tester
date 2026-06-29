#pragma once
#include "rxsHrdwdal.h"
#include <stdio.h>
#include <string>

#pragma warning(disable : 4996)
#define RXSAPI			extern "C" __declspec(dllexport)


#ifdef _MYX86
#include "../zmotion/x86/zauxdll2.h"
#include "../zmotion/x86/zmotion.h"

#pragma comment(lib,"..\\zmotion\\x86\\zmotion.lib")
#pragma comment(lib,"..\\zmotion\\x86\\zauxdll.lib")

#else
#include "../zmotion/x64/zauxdll2.h"
#include "../zmotion/x64/zmotion.h"

#pragma comment(lib,"..\\zmotion\\x64\\zmotion.lib")
#pragma comment(lib,"..\\zmotion\\x64\\zauxdll.lib")

#endif


ZMC_HANDLE       g_handle = NULL;				//控制器链接句柄


ZMotionDal::ZMotionDal() {
	g_handle = 0;
}


bool ZMotionDal::AutoSearchIP() {
	//可以不用这个
	int32 iresult;
	char   buffer[256];
	iresult = ZAux_SearchEthlist(buffer, 10230, 100);
	return iresult == ERR_OK;
}


short ZMotionDal::baseCfgSet(const char* cfgStr) {

	return 0;
}

bool ZMotionDal::BASfileUploader(const char* path) {
	//下载BAS程序到ROM
	int32 ret = ZAux_BasDown(g_handle, path, 1);
	if (ret != 0)
	{
		//("DownLoad File Failed!");
		return false;
	}
	return true;
}

void ZMotionDal::resourceCfg() {
	AxNum = 3;
	DiNum = 23;
	DoNum = 12;

	//设置轴的类型 脉冲轴类型
	int32 iret = ZAux_Direct_SetAtype(g_handle, 0, 1);
	iret = ZAux_Direct_SetAtype(g_handle, 1, 1);
	iret = ZAux_Direct_SetAtype(g_handle, 2, 1);
	//设定脉冲模式及逻辑方向（脉冲+方向）	
	iret = ZAux_Direct_SetInvertStep(g_handle, 0, 0);
	iret = ZAux_Direct_SetInvertStep(g_handle, 1, 0);
	iret = ZAux_Direct_SetInvertStep(g_handle, 2, 0);

	//设置脉冲当量	1表示以一个脉冲为单位 ，设置为1MM的脉冲个数，这度量单位为MM
	iret = ZAux_Direct_SetUnits(g_handle, 0, 1000);
	iret = ZAux_Direct_SetUnits(g_handle, 1, 1000);
	iret = ZAux_Direct_SetUnits(g_handle, 2, 1000);

	//0-1是高速的，做位置比较用
	iret = ZAux_Direct_SetDatumIn(g_handle, 0, 2);//0轴 in2 原点
	iret = ZAux_Direct_SetFwdIn(g_handle, 0, 3); //映射正限位输入对应到输入口 2
	iret = ZAux_Direct_SetRevIn(g_handle, 0, 4); //映射负限位输入对应到输 入口 3

	iret = ZAux_Direct_SetDatumIn(g_handle, 1, 5);//1轴 in3 原点
	iret = ZAux_Direct_SetFwdIn(g_handle, 1, 6); //映射正限位输入对应到输入口 6
	iret = ZAux_Direct_SetRevIn(g_handle, 1, 7); //映射负限位输入对应到输 入口 7

	iret = ZAux_Direct_SetDatumIn(g_handle, 2, 8);//2轴 in4 原点
	iret = ZAux_Direct_SetFwdIn(g_handle, 2, 9); //映射正限位输入对应到输入口 9
	iret = ZAux_Direct_SetRevIn(g_handle, 2, 10); //映射负限位输入对应到输 入口10

	//+/-限位输入状态反转
	iret = ZAux_Direct_SetInvertIn(g_handle, 2, 0);
	iret = ZAux_Direct_SetInvertIn(g_handle, 5, 0);
	iret = ZAux_Direct_SetInvertIn(g_handle, 8, 0);

	iret = ZAux_Direct_SetInvertIn(g_handle, 3, 0);
	iret = ZAux_Direct_SetInvertIn(g_handle, 4, 0);
	iret = ZAux_Direct_SetInvertIn(g_handle, 6, 0);
	iret = ZAux_Direct_SetInvertIn(g_handle, 7, 0);
	iret = ZAux_Direct_SetInvertIn(g_handle, 9, 0);
	iret = ZAux_Direct_SetInvertIn(g_handle, 10, 0);


	//轴驱动报警信号定义到输入口
	ZAux_Direct_SetAlmIn(g_handle, 0, 24);
	ZAux_Direct_SetAlmIn(g_handle, 1, 25);
	ZAux_Direct_SetAlmIn(g_handle, 2, 26);
	//轴驱动报警信号 
	ZAux_Direct_SetInvertIn(g_handle, 24, 1);
	ZAux_Direct_SetInvertIn(g_handle, 25, 1);
	ZAux_Direct_SetInvertIn(g_handle, 26, 0);//设置输入 0 低电平有效
}

RXSAPI short ZMotionDal::Open(char* CfgPath) {
	char   IP[128];
	int32 iresult;
	memset(IP, 0, 128);
	//strcpy(IP, "127.0.0.1");//本地模拟调试
	strcpy(IP, "192.168.0.11");
	if (NULL != g_handle)
	{
		ZAux_Close(g_handle);
		g_handle = NULL;
	}
	iresult = ZAux_OpenEth(IP, &g_handle);
	if (ERR_SUCCESS != iresult)
	{
		g_handle = NULL;
		return -1;
	}

	ClearStatusError(7);
	resourceCfg();
	return 0;
}

RXSAPI void ZMotionDal::Close() {
	if (NULL != g_handle)
	{
		ZAux_Close(g_handle);
		g_handle = NULL;
	}
}

RXSAPI void ZMotionDal::ClearPos(unsigned AxisNo) {
	int32 ret = ZAux_Direct_SetDpos(g_handle, AxisNo, 0);//设置零点
}

//清除 状态 及错误，SOE 1为清除 状态，2 为清除错误，3 为两者都清除
RXSAPI bool ZMotionDal::ClearStatusError(u8 SOE) {
	int32 ret = 0;
	int axisstatus = 0;
	for (unsigned x = 0; x < AxNum; x++) {
		ret = ZAux_Direct_GetAxisStatus(g_handle, x, &axisstatus);
		if (axisstatus > 0) {
			//if ((SOE & 1) == 1) {
			//	ret = ZAux_BusCmd_DriveClear(g_handle, x, 0);
			//}
			//if ((SOE & 2) == 2) {
			//	ret = ZAux_BusCmd_DriveClear(g_handle, x, 1);
			//}
			//if ((SOE & 4) == 4) {
			//	ret = ZAux_BusCmd_DriveClear(g_handle, x, 2);
			//}
			ret = ZAux_Direct_SetErrormask(g_handle, x, 0);
		}
	}
	return true;
}

RXSAPI void ZMotionDal::JogMove(unsigned AxisNo, byte dir, AxisInfo& Axi) {
	int32 ret = 0;

	ret = ZAux_Direct_SetErrormask(g_handle, AxisNo, 0);

	//设定速度，加减速
	ret = ZAux_Direct_SetLspeed(g_handle, AxisNo, Axi.MotionParam.velStart);
	ret = ZAux_Direct_SetSpeed(g_handle, AxisNo, Axi.TageVel);
	ret = ZAux_Direct_SetAccel(g_handle, AxisNo, Axi.MotionParam.acc);
	ret = ZAux_Direct_SetDecel(g_handle, AxisNo, Axi.MotionParam.dec);
	ret = ZAux_Direct_SetFastDec(g_handle, AxisNo, Axi.MotionParam.dec * 10);  //设置轴的快减减速度 

	//设定S曲线时间 设置为0表示梯形加减速 
	ret = ZAux_Direct_SetSramp(g_handle, AxisNo, 0);
	ret = ZAux_Direct_Single_Vmove(g_handle, AxisNo, (dir == '+') ? 1 : -1);
}

RXSAPI void ZMotionDal::PpMoveAbs(unsigned AxisNo, AxisInfo& Axi) {
	int32 ret = 0;

	ret = ZAux_Direct_SetErrormask(g_handle, AxisNo, 0);

	//设定速度，加减速
	ret = ZAux_Direct_SetLspeed(g_handle, AxisNo, Axi.MotionParam.velStart);
	ret = ZAux_Direct_SetSpeed(g_handle, AxisNo, Axi.TageVel);
	ret = ZAux_Direct_SetAccel(g_handle, AxisNo, Axi.MotionParam.acc);
	ret = ZAux_Direct_SetDecel(g_handle, AxisNo, Axi.MotionParam.dec);
	ret = ZAux_Direct_SetFastDec(g_handle, AxisNo, Axi.MotionParam.dec * 10);//设置轴的快减减速度 
	//设定S曲线时间 设置为0表示梯形加减速 
	ret = ZAux_Direct_SetSramp(g_handle, AxisNo, 0);
	ret = ZAux_Direct_Single_MoveAbs(g_handle, AxisNo, Axi.TagePos);
}

RXSAPI void ZMotionDal::PpMoveRel(unsigned AxisNo, AxisInfo& Axi) {
	int32 ret = 0;
	ret = ZAux_Direct_SetErrormask(g_handle, AxisNo, 0);	
	//设定速度，加减速
	ret = ZAux_Direct_SetLspeed(g_handle, AxisNo, Axi.MotionParam.velStart);
	ret = ZAux_Direct_SetSpeed(g_handle, AxisNo, Axi.TageVel);
	ret = ZAux_Direct_SetAccel(g_handle, AxisNo, Axi.MotionParam.acc);
	ret = ZAux_Direct_SetDecel(g_handle, AxisNo, Axi.MotionParam.dec);
	ret = ZAux_Direct_SetFastDec(g_handle, AxisNo, Axi.MotionParam.dec * 10);//设置轴的快减减速度 
	//设定S曲线时间 设置为0表示梯形加减速 
	ret = ZAux_Direct_SetSramp(g_handle, AxisNo, 0);
	ret = ZAux_Direct_Single_Move(g_handle, AxisNo, Axi.TagePos);
}

RXSAPI void ZMotionDal::GroupParamSet(unsigned mAxisNo, AxisInfo& Axi) {
	int32 ret = 0;
	if (Axi.Mode == MotionModel::GROUP_LINE_INTERPOLATION_MASTER) {

	}
	else if (Axi.Mode == MotionModel::GROUP_ARC_INTERPOLATION_MASTER) {

	}
	else if (Axi.Mode == MotionModel::GROUP_GEAR_MASTER) {
		if (Axi.GrpData.SalveTable == 0) { return; }
		//设置开启同步连接，轴 1 跟着轴 0 走
		ret = ZAux_Direct_Connect(g_handle, 1, Axi.GrpData.MasterNo, Axi.GrpData.SalveTable[0].AxNo);

	}
	else if (Axi.Mode == MotionModel::GROUP_FOLLOW_MASTER) {

	}
	else
	{
		return;
	}
}

RXSAPI void ZMotionDal::GroupMoveClose(unsigned mAxisNo, AxisInfo& Axi) {
	int32 ret = 0;
	if (Axi.Mode == MotionModel::GROUP_LINE_INTERPOLATION_MASTER || Axi.Mode == MotionModel::GROUP_ARC_INTERPOLATION_MASTER) {
		//存在可能连续插补运动先，先解除掉
		ret = ZAux_Direct_SetMerge(g_handle, mAxisNo, 0);
	}
	else if (Axi.Mode == MotionModel::GROUP_GEAR_MASTER) {

	}
	else if (Axi.Mode == MotionModel::GROUP_FOLLOW_MASTER) {

	}
	else
	{
		return;
	}
}

RXSAPI void ZMotionDal::GroupMove(unsigned AxisNo, AxisInfo& Axi) {
	int32 ret = 0;
	int tNums = Axi.GrpData.SalveCount + 1;
	if (tNums < 2) { return; }
	if (Axi.Mode == MotionModel::GROUP_LINE_INTERPOLATION_MASTER) {
		
		int* axislist = new int[tNums];
		float* poslist = new float[tNums];
		axislist[0] = Axi.GrpData.MasterNo;
		poslist[0] = Axi.TagePos;
		for (int x = 1; x < tNums; x++) {
			axislist[x] = Axi.GrpData.SalveTable[x - 1].AxNo;
			poslist[x] = ((GrpExtrenData*)Axi.GrpData.SalveTable[x - 1].ExtrenData)->TgaePos;
		}

		ret = ZAux_Direct_SetMerge(g_handle, Axi.GrpData.MasterNo, 1);
		ret = ZAux_Direct_SetSpeed(g_handle, axislist[0], Axi.TageVel); //设置插补速度 100，设置在主轴上
		ret = ZAux_Direct_SetAccel(g_handle, axislist[0], Axi.MotionParam.acc); //设置插补加速度 500，设置在主轴上
		ret = ZAux_Direct_SetDecel(g_handle, axislist[0], Axi.MotionParam.dec); //设置插补减速度 500，设置在主轴上
		ret = ZAux_Direct_MoveAbs(g_handle, tNums, axislist, poslist); //调用运动，轴 0,1,2 走到对应的绝对位置

		delete[] axislist;
		delete[] poslist;
		axislist = 0;
		poslist = 0;
	}
	else if (Axi.Mode == MotionModel::GROUP_ARC_INTERPOLATION_MASTER) {
			GrpExtrenData ged;
			int* axislist = new int[tNums];
			axislist[0] = Axi.GrpData.MasterNo;
			for (int x = 1; x < tNums; x++) {
				axislist[x] = Axi.GrpData.SalveTable[x - 1].AxNo;
			}
			ged.ArcParam.center = ((GrpExtrenData*)Axi.GrpData.SalveTable[0].ExtrenData)->ArcParam.center;
			ged.ArcParam.ends = ((GrpExtrenData*)Axi.GrpData.SalveTable[0].ExtrenData)->ArcParam.ends;
			
			ret = ZAux_Direct_SetSpeed(g_handle, axislist[0], Axi.TageVel); //设置插补速度 100，设置在主轴上
			ret = ZAux_Direct_SetAccel(g_handle, axislist[0], Axi.MotionParam.acc); //设置插补加速度 500，设置在主轴上
			ret = ZAux_Direct_SetDecel(g_handle, axislist[0], Axi.MotionParam.dec); //设置插补减速度 500，设置在主轴上s
			ret = ZAux_Direct_SetMerge(g_handle, Axi.GrpData.MasterNo, 1);
			ret = ZAux_Direct_SetCornerMode(g_handle, Axi.GrpData.MasterNo, 8);//开启小圆限速
			ret = ZAux_Direct_SetFullSpRadius(g_handle, Axi.GrpData.MasterNo, 60);//设置开始减速角度
			ret = ZAux_Direct_SetForceSpeed(g_handle, Axi.GrpData.MasterNo, 100);//设置等比减速时速度
			ret = ZAux_Direct_MoveCirc(g_handle, tNums, axislist, ged.ArcParam.ends.x, ged.ArcParam.ends.y, ged.ArcParam.center.x, ged.ArcParam.center.y, 0);

			delete[] axislist;
			axislist = 0;
	}
	else if (Axi.Mode == MotionModel::GROUP_GEAR_MASTER) {

	}
	else if (Axi.Mode == MotionModel::GROUP_FOLLOW_MASTER) {

	}
	else
	{
		return;
	}
}

RXSAPI bool ZMotionDal::Servon(unsigned AxisNo, bool OnOff) {
	int32 ret = 0;
	if (AxisNo == INTMAXVAL) {
		for (unsigned x = 0; x < AxNum; x++)
			ret = ZAux_Direct_SetOp(g_handle, 12 + x, OnOff ? 1 : 0);

		return true;
	}
	ret = ZAux_Direct_SetOp(g_handle, 12 + AxisNo, OnOff ? 1 : 0);
	return true;
}

RXSAPI void ZMotionDal::AxStop(unsigned AxisNo, bool isSmooth) {
	int32 ret = 0;
	if (NULL == g_handle) { return; }
	if (AxisNo == INTMAXVAL) {
		for (unsigned x = 0; x < AxNum; x++) {
			ret = ZAux_Direct_Single_Cancel(g_handle, x, 2);
		}
		return;
	}
	ret = ZAux_Direct_Single_Cancel(g_handle, AxisNo, 2);
}


RXSAPI bool ZMotionDal::SetOutput(unsigned oSernum, bool Value) {
	int32 ret = ZAux_Direct_SetOp(g_handle, oSernum, Value ? 1 : 0);

	//安bit 方式输出 -- modbus 操作
	uint8 iostate = 0;
	//ZAux_Modbus_Set0x(g_handle, 20000, 4, &iostate);//MODBUS_BIT(20000)映射到控制输出口 ，关闭输出0-3 二进制0 
	return true;
}

RXSAPI	int ZMotionDal::GetInput(unsigned index) {
	uint32 iostatus = 0;
	int32 ret = ZMC_GetInAll(g_handle, 0, DiNum, &iostatus);
	if (index == INTMAXVAL) {
		return iostatus;
	}
	else {
		uint32 cmpv = 1 << index;
		return ((iostatus & cmpv) == cmpv) ? 1 : 0;
	}
}

RXSAPI int ZMotionDal::GetOutput(unsigned index) {
	uint32 outStatus = 0;
	int32 ret = ZMC_GetOutAll(g_handle, 0, DoNum, &outStatus);
	if (index == INTMAXVAL) {
		return outStatus;
	}
	uint32 cmpv = 1 << index;
	return ((outStatus & cmpv) == cmpv) ? 1 : 0;
}

RXSAPI void ZMotionDal::AxHome(unsigned AxisNo, AxisInfo& Axi) {
	//存在可能连续插补运动先，先解除掉
	int32 ret = ZAux_Direct_SetMerge(g_handle, AxisNo, 0);
	//设定速度，加减速
	ret = ZAux_Direct_SetLspeed(g_handle, AxisNo, Axi.MotionParam.velStart);
	ret = ZAux_Direct_SetSpeed(g_handle, AxisNo, Axi.Rethome.Vel);
	ret = ZAux_Direct_SetAccel(g_handle, AxisNo, Axi.MotionParam.acc);
	ret = ZAux_Direct_SetDecel(g_handle, AxisNo, Axi.MotionParam.dec);
	ret = ZAux_Direct_SetCreep(g_handle, AxisNo, Axi.Rethome.ZeroCorrectvel);

	//ZMC系列认为OFF时碰到了原点信号（常闭） ，如果是常开传感器则需要反转输入口，ECI系列的不需要反转
	//ret = ZAux_Direct_SetInvertIn(g_handle, m_datumin, 1);
	//回零	

	//ret = ZAux_Direct_Single_Datum(g_handle, AxisNo, Axi.Rethome.Mode); //模式 3+ 10(碰限位反找存在问题)
	ret = ZAux_Direct_UserDatum(g_handle, AxisNo, Axi.Rethome.Mode, Axi.Rethome.Vel, Axi.Rethome.ZeroCorrectvel, Axi.Rethome.Offset);
}


RXSAPI void ZMotionDal::GetAxStatus(unsigned AxisNo, AxisInfo& Axi) {
	int axisstatus = 0;
	int status = 0;
	int32 ret = ZAux_Direct_GetAxisStatus(g_handle, AxisNo, &axisstatus);
	if (axisstatus == 0)   //正常
	{
		//str = "轴正常";
	}
	else
	{

	}
	if (axisstatus & (1 << 22))   //ALM 报警
	{
		//str = "伺服报警";
		Axi.Status.IsAlam = true;
	}
	else {
		Axi.Status.IsAlam = false;
	}

	if (axisstatus & (1 << 4))   //正向限位
	{
		//str += " 正限位报警";
		Axi.Status.OnLimtP = true;
	}
	else {
		Axi.Status.OnLimtP = false;
	}

	if (axisstatus & (1 << 5))   //正向限位
	{
		//str += " 负限位报警";
		Axi.Status.OnLimtN = true;
	}
	else {
		Axi.Status.OnLimtN = false;
	}
	ret = ZAux_Direct_GetIfIdle(g_handle, AxisNo, &status);//判断当前轴状态
	if (status == -1)
	{
		//("当前状态：停  止");
		Axi.Status.IsMotionFinsh = true;
	}
	else
	{
		//("当前状态：运动中");
		Axi.Status.IsMotionFinsh = false;
	}

	//此处需要注意，在内部模式下，此处不是依靠此来完成判断的
	// //ZAux_Direct_Single_Datum 函数才回生效 ZAux_Direct_GetHomeStatus，不然都是 0
	//uint32 orgStatus;
	//ret = ZAux_Direct_GetHomeStatus(g_handle, AxisNo, &orgStatus);
	//if (orgStatus == 0) {
	//	Axi.Rethome.Flag = 0;
	//}
	//else {
	//	Axi.Rethome.Flag = 1;
	//}


	uint32 orgInv = 0;
	ret = ZAux_Direct_GetIn(g_handle, 2 + AxisNo * 3, &orgInv);
	if (orgInv == 1) {
		Axi.Status.OnOrg = true;

		if (Axi.Rethome.Flag > 0) {
			Axi.Rethome.Flag = 0;
		}

	}
	else {
		Axi.Status.OnOrg = false;
	}
	ret = ZAux_Direct_GetOp(g_handle, 12 + AxisNo, &orgInv);
	if (orgInv == 1) {
		Axi.Status.OnServ = true;
	}
	else {
		Axi.Status.OnServ = false;
	}
}

RXSAPI void ZMotionDal::GetAxVelPos(unsigned AxisNo, AxisInfo& Axi) {

	float position = 0;
	int32 ret = ZAux_Direct_GetDpos(g_handle, AxisNo, &position);
	ret = Axi.Status.prfPos = position;

	//ret = ZAux_Direct_GetEncoder(g_handle, AxisNo, &position);
	ret = ZAux_Direct_GetMpos(g_handle, AxisNo, &position);
	Axi.Status.EncPos = position;

	float NowSp = 0;
	ret = ZAux_Direct_GetVpSpeed(g_handle, AxisNo, &NowSp);
	Axi.Status.prfVel = NowSp;
	ret = ZAux_Direct_GetMspeed(g_handle, AxisNo, &NowSp);
	Axi.Status.EncVel = NowSp;
}

RXSAPI void ZMotionDal::SetSoftLimit(unsigned AxisNo, long Ppos, long Npos) {

}


void ZMotionDal::PosCompareSet(unsigned AxisNo, PoscmpSetparam& Pcsp, PoscmpPilist& PointList) {
	//清除前面的比较输出指令
	int32 iret = ZAux_Direct_HwPswitch2(g_handle, AxisNo, 2, 0, 0, 0, 0, 0, 0);
	//将比较点填入TABLE
	iret = ZAux_Direct_SetTable(g_handle, PointList.ID, PointList.PointNums, PointList.PointArray);
	if (iret != ERR_OK)
	{
		return;
	}
	iret = ZAux_Direct_HwPswitch2(g_handle, AxisNo, 1, Pcsp.DoutNo, Pcsp.DoutStatus, 0, 10, 0, 0);
	if (iret != ERR_OK) {
		return;
	}
}
