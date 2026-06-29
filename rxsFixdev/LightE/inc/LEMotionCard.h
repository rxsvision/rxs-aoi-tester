#pragma once
#ifdef CONFOCAL_DLL
#else
#define CONFOCAL_DLL extern "C" _declspec(dllimport)
#endif
#include"LEMDataType.h"

/*
函数功能：获取已打开的运动控制卡信息[注意该函数只能在LE_InitDLL执行成功后执行才有效]。
参数：
pHandle-[in,out]保存已打开的运动控制卡句柄。
pSN-[in,out]保存已打开的运动控制卡SN号，每个SN号最大占用32个字节，即pSN的长度需大于等于32*maxCount。
maxCount-[in]最多可获取的运动控制卡信息数量。
返回值：0-没有已打开的运动控制卡；大于0的数表示实际已打开的运动控制卡数量。
*/
CONFOCAL_DLL int __stdcall LE_GetMotionCardsMessage(int *pHandle,char *pSN,int maxCount);

/*
函数功能：初始化指定轴。
参数：
axisParam-[in]轴初始化参数结构体。
axis-[in]轴号。
handle-[in]已打开卡句柄。
返回值：1-执行成功，其他返回值请参考返回值参数表
*/
CONFOCAL_DLL int __stdcall LE_InitAxis(AxisParam axisParam, int axis, int handle = 0);

/*
函数功能：单轴复位找原点。
参数：
axis-[in]轴号。
mode-[in]回零方式：0-原点开关回零（初始回零方向正向）；1-原点开关回零（初始回零方向负向）；2-正限位开关触发；3-负限位开关触发；
4-单 Z 相 编码上升沿触发（初始回零方向正向）；5-单 Z 相 编码上升沿触发（初始回零方向负向）。
lowVel-[in]基本搜寻速度，单位mm/s。
highVel-[in]低速（两次搜寻时需要），单位mm/s。
offset-[in]原点偏移，单位mm。
handle-[in]已打开卡句柄。
返回值：true-执行成功；false-执行失败。
*/
CONFOCAL_DLL bool __stdcall LE_AxisOrigin(int axis, char mode, float lowVel, float highVel, float offset, int handle = 0);



/*
函数功能：单轴速度模式运动。
参数：
axis-[in]轴号。
vel-[in]目标速度，单位mm/s，当此值设置为非零时开始运动，若设置为0时停止运动。
handle-[in]已打开卡句柄。
返回值：无
*/
CONFOCAL_DLL int __stdcall LE_MoveVelocity(int axis, float vel, int handle = 0);

/*
函数功能：单轴速度模式运动。
参数：
axis-[in]轴号。
vel-[in]目标速度，当此值设置为非零时开始运动，若设置为0时停止运动，单位um/s。
handle-[in]已打开卡句柄。
返回值：无
*/
CONFOCAL_DLL int __stdcall LE_MoveVelocityUM(int axis, float vel, int handle = 0);

/*
函数功能：单轴速度模式运动。
参数：
axis-[in]轴号。
vel-[in]目标速度，当此值设置为非零时开始运动，若设置为0时停止运动，单位mm/s。
handle-[in]已打开卡句柄。
返回值：无
*/
CONFOCAL_DLL int __stdcall LE_MoveVelocityMM(int axis, float vel, int handle = 0);

/*
函数功能：绝对直线运动
参数：
axis-[in]轴号。
endPos-[in]终点位置，单位为pulse。
handle-[in]已打开卡句柄。
返回值：无
*/
CONFOCAL_DLL int __stdcall LE_MoveAbsolute(int axis, float endPos, int handle = 0);

/*
函数功能：绝对直线运动
参数：
axis-[in]轴号。
endPos-[in]终点位置，单位为um。
handle-[in]已打开卡句柄。
返回值：无
*/
CONFOCAL_DLL int __stdcall LE_MoveAbsoluteUM(int axis, float endPos, int handle = 0);

/*
函数功能：绝对直线运动
参数：
axis-[in]轴号。
endPos-[in]终点位置，单位为mm。
handle-[in]已打开卡句柄。
返回值：无
*/
CONFOCAL_DLL int __stdcall LE_MoveAbsoluteMM(int axis, float endPos, int handle = 0);

/*
函数功能：多轴插补运动
参数：
axisCount-[in]:参与的轴数量。
axisArray-[in]:参与的轴序号，数组大小跟axisCount要一致。
pTargetPos-[in]目标位置，数组大小跟axisCount要一致，对应axisArray的轴顺序终点位置，单位为pulse。
blockEnable-[in]:是否为阻塞模式。
handle-[in]已打开卡句柄。
返回值：无
*/
CONFOCAL_DLL int __stdcall LE_MoveMultiAxis(int axisCount, int *axisArray, float *pTargetPos, bool blockEnable, int handle = 0);

/*
函数功能：多轴插补运动uffe
参数：
axisCount-[in]:参与的轴数量。
axisArray-[in]:参与的轴序号，数组大小跟axisCount要一致。
pTargetPos-[in]目标位置，数组大小跟axisCount要一致，对应axisArray的轴顺序终点位置，单位为um。
blockEnable-[in]:是否为阻塞模式。
handle-[in]已打开卡句柄。
返回值：无
*/
CONFOCAL_DLL int __stdcall LE_MoveMultiAxisUM(int axisCount, int *axisArray, float *pTargetPos, bool blockEnable, int handle = 0);

/*
函数功能：多轴插补运动
参数：
axisCount-[in]:参与的轴数量。
axisArray-[in]:参与的轴序号，数组大小跟axisCount要一致。
pTargetPos-[in]目标位置，数组大小跟axisCount要一致，对应axisArray的轴顺序终点位置，单位为mm。
blockEnable-[in]:是否为阻塞模式。
handle-[in]已打开卡句柄。
返回值：无
*/
CONFOCAL_DLL int __stdcall LE_MoveMultiAxisMM(int axisCount, int *axisArray, float *pTargetPos, bool blockEnable, int handle = 0);

/*
函数功能：相对直线运动
参数：
axis-[in]轴号。
endPos-[in]相对位移，单位为pulse。
handle-[in]已打开卡句柄。
返回值：无
*/
CONFOCAL_DLL int __stdcall LE_MoveRelative(int axis, float endPos, int handle = 0);

/*
函数功能：相对直线运动
参数：
axis-[in]轴号。
endPos-[in]相对位移，单位为um。
handle-[in]已打开卡句柄。
返回值：无
*/
CONFOCAL_DLL int __stdcall LE_MoveRelativeUM(int axis, float endPos, int handle = 0);

/*
函数功能：相对直线运动
参数：
axis-[in]轴号。
endPos-[in]相对位移，单位为mm。
handle-[in]已打开卡句柄。
返回值：无
*/
CONFOCAL_DLL int __stdcall LE_MoveRelativeMM(int axis, float endPos, int handle = 0);

/*
函数功能：检查指定轴是否运动完成,同时可选择是否读取轴当前的位置。
参数：
axis-[in]轴号。
curPos-[out]保存轴当前位置,单位um。
handle-[in]已打开卡句柄。
返回值：1-表示运动中，0-表示等待空闲或运动已完成状态。
*/
CONFOCAL_DLL int __stdcall LE_CheckMoveFinishUM(int axis, float &curPos, int handle = 0);

/*
函数功能：设置轴加速度
参数：
axis-[in]轴号。
acc-[in]加速度，单位mm/s^2。
handle-[in]已打开卡句柄。
返回值：1-设置成功。
*/
CONFOCAL_DLL int __stdcall LE_SetAcc(int axis, float acc, int handle = 0);

/*
函数功能：获取轴加速度。
参数：
axis-[in]轴号。
acc-[out]用于存储获取到的加速度，单位mm/s^2。
handle-[in]已打开卡句柄。
返回值：1-设置成功。
*/
CONFOCAL_DLL int __stdcall LE_GetAcc(int axis, float &acc, int handle = 0);

/*
函数功能：设置点到点直线运动速度，vel的单位为mm/s
参数：
axis-[in]轴号。
vel-[in]速度，单位mm/s。
handle-[in]已打开卡句柄。
返回值：1-设置成功。
*/
CONFOCAL_DLL int __stdcall LE_SetVel(int axis, float vel, int handle = 0);

/*
函数功能：获取点到点直线运动速度单位为mm/s。
参数：
axis-[in]轴号。
vel-[out]用于存储获取到的速度，单位mm/s。
handle-[in]已打开卡句柄。
返回值：1-设置成功。
*/
CONFOCAL_DLL int __stdcall LE_GetVel(int axis, float &vel, int handle = 0);

/*
函数功能：设置点到点直线运动速度。
参数：
axis-[in]轴号。
vel-[in]速度，单位pulse/s。
handle-[in]已打开卡句柄。
返回值：1-设置成功。
*/
CONFOCAL_DLL int __stdcall LE_SetVelPulse(int axis, float velP, int handle = 0);

/*
函数功能：获取点到点直线运动速度。
参数：
axis-[in]轴号。
vel-[out]用于存储获取到的速度，单位pulse/s。
handle-[in]已打开卡句柄。
返回值：1-设置成功。
*/
CONFOCAL_DLL int __stdcall LE_GetVelPulse(int axis, float &velP, int handle = 0);

/*
函数功能：获取轴限位开关状态。
参数：
axis-[in]轴号。
handle-[in]已打开卡句柄。
返回值：0-表示未触发限位；1- 表示轴正限位；2- 表示轴负限位；3- 表示轴正负都限位。
*/
CONFOCAL_DLL int __stdcall LE_GetAxisStatus(int axis, int handle = 0);

/*
函数功能：获取轴当前位置。
参数：
axis-[in]轴号。
axisPosUM-[in]用于存储获取到的位置值，单位为pulse。
handle-[in]已打开卡句柄。
返回值：1-获取成功。
*/
CONFOCAL_DLL int __stdcall LE_GetPos(int axis, int &axisPos, int handle = 0);

/*
函数功能：获取轴当前位置。
参数：
axis-[in]轴号。
axisPosUM-[in]用于存储获取到的位置值，单位为um。
handle-[in]已打开卡句柄。
返回值：1-获取成功。
*/
CONFOCAL_DLL int __stdcall LE_GetPosUM(int axis, float &axisPosUM, int handle = 0);

/*
函数功能：获取轴当前位置。
参数：
axis-[in]轴号。
axisPosMM-[out]用于存储获取到的位置值，单位为mm。
handle-[in]已打开卡句柄。
返回值：1-获取成功。
*/
CONFOCAL_DLL int __stdcall LE_GetPosMM(int axis, float &axisPosMM, int handle = 0);

/*
函数功能：清除运控卡缓存区中的指令。
参数：
bufferIdx-[in]缓冲区编号。
handle-[in]已打开卡句柄。
返回值：1-清除成功。
*/
CONFOCAL_DLL int __stdcall LE_ClearFIFO(int bufferIdx, int handle = 0);

/*
函数功能：紧急停止指定轴的运动，调用此函数后若要重新运动该轴需重新初始化该轴，并设置速度与加速度。
参数：
axis-[in]轴号。
handle-[in]已打开卡句柄。
返回值：1-停止成功。
*/
CONFOCAL_DLL int __stdcall LE_StopMove(int axis, int handle = 0);

/*
函数功能：检查指定轴是否运动完成。
参数：
axis-[in]轴号。
handle-[in]已打开卡句柄。
返回值：0-表示完成，1-表示运动中。
*/
CONFOCAL_DLL int __stdcall LE_CheckStop(int axis, int handle = 0);

/*
函数功能：等待指定轴运动完成。
参数：
axis-[in]轴号。
handle-[in]已打开卡句柄。
返回值：1-等待成功。
*/
CONFOCAL_DLL int __stdcall LE_WaitAxisMoveFinish(int axis, int timeOut, int handle = 0);

/*
函数功能：清零指定轴的位置。
参数：
axis-[in]轴号。
handle-[in]已打开卡句柄。
返回值：1-设置成功。
*/
CONFOCAL_DLL int __stdcall LE_ClearAxisPos(int axis, int handle = 0);

/*
函数功能：读取指定卡的锁存计数。
参数：
axis-[in]轴号。
handle-[in]已打开卡句柄。
返回值：锁存个数。
*/
CONFOCAL_DLL int __stdcall  LE_ReadPosLockCount(int handle = 0);

/*
函数功能：清空指定卡的锁存计数。
参数：
handle-[in]已打开卡句柄。
返回值：true-成功；false-失败。
*/
CONFOCAL_DLL bool __stdcall  LE_ClearPosLockCount(int handle = 0);

/*
函数功能：设置位置锁存相关参数。
参数：
llParam-[in]位置锁存相关参数对象。
handle-[in]已打开卡句柄。
返回值：true-成功；false-失败。
*/
CONFOCAL_DLL bool __stdcall  LE_SetPosLockParam(LocationLatchParam & llParam, int handle = 0);

/*
函数功能：启动或停止位置锁存。
参数：
isEnable-[in]false-停止；true-启动。
handle-[in]已打开卡句柄。
返回值：true-成功；false-失败。
*/
CONFOCAL_DLL bool __stdcall  LE_EnablePosLock(int onOff, int handle = 0);

/*
函数功能：获取位置锁存相关参数。
参数：
llParam-[in]保存位置锁存相关参数。
handle-[in]已打开卡句柄。
返回值：true-成功；false-失败。
*/
CONFOCAL_DLL bool __stdcall  LE_GetPosLockParam(LocationLatchParam & llParam, int handle = 0);

/*
函数功能：设置运控卡指定地址的值。
参数：
loc-[in]地址。
val-[out]需设置的值。
axis-[in]轴号。
handle-[in]已打开卡句柄。
返回值：true-成功；false-失败。
*/
CONFOCAL_DLL bool __stdcall  LE_SetLocVal(int loc, int val, int axis = 0, int handle = 0, bool bSmall = true);

/*
函数功能：获取运控卡指定地址的值。
参数：
loc-[in]地址。
val-[out]保存获取到的值。
axis-[in]轴号。
handle-[in]已打开卡句柄。
返回值：true-成功；false-失败。
*/
CONFOCAL_DLL bool __stdcall  LE_GetLocVal(int loc, int &val, int axis = 0, int handle = 0);

/*
函数功能：设置轴当前位置所对应的坐标，单位为pulse。
参数：
axis-[in]轴号。
pos-[in]需设置到轴内部的坐标，单位pulse。
handle-[in]已打开卡句柄。
返回值：1-设置成功。
*/
CONFOCAL_DLL int __stdcall LE_SetAxisPos(int axis, int pos, int handle = 0);

///*
//函数功能：设置分频输出参数。
//参数：
//LinearCmpDivParam-[in]分频输出相关参数对象。
//handle-[in]已打开卡句柄。
//返回值：true-成功；false-失败。
//*/
//CONFOCAL_DLL bool __stdcall  LE_SetLinearCompParam(LinearCmpDivParam & lCParam, int handle = 0);
//
///*
//函数功能：获取分频输出参数。
//参数：
//LinearCmpDivParam-[in]分频输出相关参数对象。
//handle-[in]已打开卡句柄。
//返回值：true-成功；false-失败。
//*/
//CONFOCAL_DLL bool __stdcall  LE_GetLinearCompParam(LinearCmpDivParam & lCParam, int handle = 0);

/*
函数功能：启动或停止分频输出。
参数：
isEnable-[in]false-停止；true-启动。
handle-[in]已打开卡句柄。
返回值：true-成功；false-失败。
*/
CONFOCAL_DLL bool __stdcall  LE_EnableLinearComp(bool isEnable, int handle = 0);

/*
函数功能：清除轴错误。
参数：
axis-[in]轴号。
handle-[in]已打开卡句柄。
返回值：1-执行成功。
*/
CONFOCAL_DLL void __stdcall LE_ClearAxisError(int axis, int handle = 0);

/*
函数功能：获取轴已锁存的坐标，内部锁存采用先进先出的机制。
参数：
pData-[in,out]保存已锁存的坐标。
maxCnt-[in]pData的长度。
realCount-[in]实际获取到的位置数量
handle-[in]已打开卡句柄。
返回值：1-获取成功。
*/
CONFOCAL_DLL bool __stdcall LE_GetLatchPos(float *pData, int maxCnt, int &realCount, int handle = 0);

/*
函数功能：设置电子齿轮运动中从动轴与主动轴之间的比例。
参数：
mainAxis-[in]主动轴号。
subAxis-[in]从动轴轴号。
Ra-[in]从动轴比例。
sourceType-[in]从动轴跟随类型。
handle-[in]已打开卡句柄。
返回值：1-执行成功。
*/
CONFOCAL_DLL bool __stdcall LE_SetGearFree(int mainAxis, int subAxis, double Ra, int sourceType, int handle = 0);

/*
函数功能：解除电子齿轮运动。
参数：
subAxis-[in]从动轴轴号。
handle-[in]已打开卡句柄。
返回值：1-执行成功。
*/
CONFOCAL_DLL bool __stdcall LE_ReleaseGearFree(int subAxis, int handle = 0);

/*
函数功能：获取指定组号的输出信号状态，按bit的值来显示，1-开启，0-关闭。
参数：
val-[in]保存获取到的值。
group-[in]输出信号组号，如果不支持按组号读取的板卡则此函数不适用
axis-[in]轴号。
handle-[in]已打开卡句柄。
返回值：1-执行成功。
*/
CONFOCAL_DLL bool __stdcall LE_GetOutputSingleBit(short &val, int group, int axis, int handle);
/*
函数功能：设置指定组号的输出信号状态，按bit的值来设置，1-开启，0-关闭。
参数：
val-[in]待设置值。
group-[in]输出信号组号，如果不支持按组号写入的板卡则此函数不适用
axis-[in]轴号。
handle-[in]已打开卡句柄。
返回值：1-执行成功。
*/
CONFOCAL_DLL bool __stdcall LE_SetOutputSingleBit(short val, int group = 1, int axis = 0, int handle = 0);

/*
函数功能：设置指定DO口高低电平状态。
参数：
val-[in]1：高电平，0：低电平。
idx-[in]待设置的通用输出编号，最小编号为0。
group-[in]输出信号组号，如果不支持按组号写入的板卡则此函数不适用
handle-[in]已打开卡句柄。
返回值：1-执行成功。
*/
CONFOCAL_DLL bool __stdcall LE_SetDOBit(short val, int idx, int group = 1, int handle = 0);

/*
函数功能：获取指定DO口高低电平状态。
参数：
val-[in]1：高电平，0：低电平。
group-[in]输出信号组号，如果不支持按组号写入的板卡则此函数不适用
handle-[in]已打开卡句柄。
返回值：1-执行成功。
*/
CONFOCAL_DLL bool __stdcall LE_GetDOBit(short &val, int idx, int group = 1, int handle = 0);

/*
函数功能：读取指定DI口高低电平状态。
参数：
val-[in]1：高电平，0：低电平。
idx-[in]待获取的通用输入编号，最小编号为0。
group-[in]输出信号组号，如果不支持按组号写入的板卡则此函数不适用
handle-[in]已打开卡句柄。
返回值：1-执行成功。
*/
CONFOCAL_DLL bool __stdcall LE_GetDIBit(short &val, int idx, int group = 1, int handle = 0);

/*
函数功能：获取轴错误状态。
参数：
axis-[in]轴号。
axisError-[out]保存获取到的状态值。
handle-[in]已打开卡句柄。
返回值：1-执行成功。
*/
CONFOCAL_DLL bool __stdcall LE_GetAxisError(int axis, int &axisError, int handle = 0);

/*
函数功能：重置探针信号计数。
参数：
axis-[in]轴号。
handle-[in]已打开卡句柄。
返回值：1-执行成功。
*/
CONFOCAL_DLL bool __stdcall LE_ResetProCount(int axis, int handle = 0);

/*
函数功能：获取探针信号计数。
参数：
axis-[in]轴号。
count-[out]保存获取到的值。
handle-[in]已打开卡句柄。
返回值：1-执行成功。
*/
CONFOCAL_DLL bool __stdcall LE_GetProCount(int axis, int &count, int handle = 0);

/*
函数功能：卡复位（机械坐标，位置全部复位）。
参数：
handle-[in]已打开卡句柄。
返回值：1-执行成功。
*/
CONFOCAL_DLL bool __stdcall LE_ResetCard(int handle = 0);

/*
函数功能：写入当前分频参数到Flash。
参数：
handle-[in]已打开卡句柄。
返回值：1-执行成功。
*/
CONFOCAL_DLL bool __stdcall LE_SaveLCParamFlash(int handle = 0);

/*
函数功能：获取当前分频状态。
参数：
psSts-[in]0 未启动比较 1 比较输出中
handle-[in]已打开卡句柄。
返回值：1-执行成功。
*/
CONFOCAL_DLL bool __stdcall LE_GetLinearCompStatus(int &psSts, int handle = 0);

/*
函数功能：设置编码器模式。
参数：
axis-[in]轴号，取值 0~N
encMode-[in]16 位编码器的模式
Bit7:0 分频系数，数值为~255。对应分频数值 1~256 （默认为0）

Bit9:8 轴位置反馈信号类型
00：外部编码器输入
01: 内部指令位置

Bit11:10 编码器信号类型（外部）（待确认）
00：AB相，相位差
01：脉冲+方向
10：正脉冲+负脉冲
11：保留

Bit12 输入 A、B 交换（外部）（A相超越B相为正，是切换这项吗？）
0：不交换，：交换

Bit13 输入 A 取反（外部）（A取反是什么意思？对应的是A的有效电平吗？0-代表什么电平？）
0：不取反，：取反

Bit14 输入 B 取反（外部）（B取反是什么意思？对应的是B的有效电平吗？0-代表什么电平？）
0：不取反，：取反

Bti15 编码器饱和  (对应什么内容，用简单直白一点的方式描述)
0：最大最小值翻转，1：不翻转
PS:对于步进开环板卡，默认编码器的模
式为 0x0100 (默认使用内部脉冲）
handle-[in]已打开卡句柄。
返回值：1-执行成功。
*/
CONFOCAL_DLL bool __stdcall LE_SetEncMode(int axis, short encMode, int handle = 0);

/*
函数功能：读取编码器模式
参数：
axis-[in]编码器通道序号，取值 0~N
encMode-[in]16 位编码器的模式
handle-[in]已打开卡句柄。
返回值：1-执行成功。
*/
CONFOCAL_DLL bool __stdcall LE_GetEncMode(int axis, short &encMode, int handle = 0);

/*
函数功能：设置螺距误差补偿参数。
参数：
num-[in]补偿的段数,取值范围[2,128]
startPos-[in]补偿起始位置，终止位置为 startPos + cmpLen
cmpLen-[in]补偿长度
pCompPos-[in]正向补偿值
pCompNeg-[in]负向补偿值
handle-[in]已打开卡句柄。
返回值：true-成功；false-失败。
*/
CONFOCAL_DLL bool __stdcall LE_SetLeadScrewCompPara(int axis, short num, long startPos, long cmpLen, short* pCompPos, short* pCompNeg, int handle = 0);

/*
函数功能：使能或禁止螺距位置补偿。
参数：
handle-[in]已打开卡句柄。
返回值：true-成功；false-失败。
*/
CONFOCAL_DLL bool __stdcall LE_EnableLeadScrew(int axis, short enable, int handle = 0);

