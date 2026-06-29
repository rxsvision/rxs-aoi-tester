#include "rxsHrdwdal.h"
#include "proteced.h"
#include <string>
#include <iostream>
#include <stdio.h>
#include "Auboi5/Auboi5example.h"

#pragma warning(disable : 4996)
#define RXSAPI			extern "C" __declspec(dllexport)

#define ROBOT_ADDR "192.168.15.100"
#define ROBOT_PORT 8899


//机械臂控制上下文句柄
RSHD g_rshd = -1;

AuboRobotDal::~AuboRobotDal() {
	//反初始化接口库
	rs_uninitialize();
}

RXSAPI short AuboRobotDal::Open(char* CfgPath) {
	std::vector<std::string> ipport = stringSplit(CfgPath, ',');
	//登录服务器
	if (example_login(g_rshd, ipport[0].c_str(), atoi(ipport[1].c_str()))) {
		//启动机械臂(必须连接真实机械臂）
		example_robotStartup(g_rshd);
		//机械臂控制柜IO测试(必须连接真实机械臂）
		example_boardIO(g_rshd);
		//机械臂工具端IO测试(必须连接真实机械臂）
		example_ToolIO(g_rshd);
		//工程启动
		//rs_project_startup(g_rshd);

		ClearStatusError(7);
		return 0;
	}
	return -1;
}

RXSAPI void AuboRobotDal::Close() {
	//关闭机械臂（必须连接真实机械臂）
	example_robotShutdown(g_rshd);
	//诊断信息
	example_get_diagnosis(g_rshd);
	//工程停止
	rs_project_stop(g_rshd);
	//退出登录
	example_logout(g_rshd);
}

RXSAPI void AuboRobotDal::ClearPos(unsigned AxisNo) {

}

//清除 状态 及错误，SOE 1为清除 状态，2 为清除错误，3 为两者都清除
RXSAPI bool AuboRobotDal::ClearStatusError(u8 SOE) {

	int axisstatus = 0;
	for (unsigned x = 0; x < AxNum; x++) {

		if (axisstatus > 0) {

		}
	}
	return true;
}

RXSAPI void AuboRobotDal::JogMove(unsigned AxisNo, byte dir, AxisInfo& Axi) {

}

RXSAPI void AuboRobotDal::PpMoveAbs(unsigned AxisNo, AxisInfo& Axi) {

}

RXSAPI void AuboRobotDal::PpMoveRel(unsigned AxisNo, AxisInfo& Axi) {

}

RXSAPI void AuboRobotDal::GroupParamSet(unsigned mAxisNo, AxisInfo& Axi) {

}

RXSAPI void AuboRobotDal::GroupMoveClose(unsigned mAxisNo, AxisInfo& Axi) {

}

RXSAPI void AuboRobotDal::GroupMove(unsigned AxisNo, AxisInfo& Axi) {

}

RXSAPI bool AuboRobotDal::Servon(unsigned AxisNo, bool OnOff) {
	return true;
}

RXSAPI void AuboRobotDal::AxStop(unsigned AxisNo, bool isSmooth) {

}


RXSAPI bool AuboRobotDal::SetOutput(unsigned oSernum, bool Value) {
	char buff[20] = { 0 };
	sprintf(buff, "U_DO_%2d", oSernum);
	if (RS_SUCC == rs_set_board_io_status_by_name(g_rshd, RobotBoardUserDO, buff, IO_STATUS_VALID)) {
		return true;
	}
	//if (RS_SUCC == rs_set_board_io_status_by_name(g_rshd, RobotBoardUserDI, USER_DI_00, IO_STATUS_VALID)) {

	//}
	return false;
}

RXSAPI	int AuboRobotDal::GetInput(unsigned index) {
	double status = 0;
	if (index < 0) {

	}
	else {
		char buff[20] = { 0 };
		sprintf(buff, "U_DI_%2d", index);
		if (RS_SUCC == rs_get_board_io_status_by_name(g_rshd, RobotBoardUserDI, buff, &status)) {
			return status;
		}
	}
	return 0;
}

RXSAPI int AuboRobotDal::GetOutput(unsigned index) {
	double status = 0;
	if (index < 0) {

	}
	else {
		char buff[20] = { 0 };
		sprintf(buff, "U_DO_%2d", index);
		if (RS_SUCC == rs_get_board_io_status_by_name(g_rshd, RobotBoardUserDO, buff, &status)) {

		}
	}
	return 0;
}

RXSAPI void AuboRobotDal::AxHome(unsigned AxisNo, AxisInfo& Axi) {

}


RXSAPI void AuboRobotDal::GetAxStatus(unsigned AxisNo, AxisInfo& Axi) {
	int axisstatus = 0;
	int status = 0;

}

RXSAPI void AuboRobotDal::GetAxVelPos(unsigned AxisNo, AxisInfo& Axi) {

	float position = 0;

}

RXSAPI void AuboRobotDal::SetSoftLimit(unsigned AxisNo, long Ppos, long Npos) {

}


void AuboRobotDal::PosCompareSet(unsigned AxisNo, PoscmpSetparam& Pcsp, PoscmpPilist& PointList) {

}
