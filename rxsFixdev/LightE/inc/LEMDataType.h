#pragma once
//轴初始化参数结构体
typedef struct AxisParam
{
	//加速度极限，单位pulse/s^2
	float accLm;
	//减速度极限，单位pulse/s^2
	float decLm;
	//速度极限，单位pulse/s
	float velLm;
	//运控卡与光栅尺或编码器脉冲反馈比例，默认为1。
	float ratio;
	//驱动器细分数
	int subdivide;
	//卡输出脉冲宽度,单位ns。
	int pulWidth;
	//丝杆螺距或直线电机磁极距，单位mm
	float pitch;
	//卡内部位置反馈是使用编码器或指令位置，此参数为1时表示使用编码器反馈，为0时表示使用指令反馈。
	char cEncoder=0;
	//轴原点、限位信号是否高电平有效，此参数为1时表示高电平有效、为0时表示低电平有效。
	char cSwitchHLevel=0;
	//编码器计数方向，0-正向；1-负向
	char data0;
	//方向信号有效电平，0-高电平有效；1-低电平有效
	char data1;
	//加速度平滑因子
	float data3;
}AxisParam, *pAxisParam;
//位置锁存参数结构体
typedef struct LocationLatchParam
{
	//指捕获的编码器位置源掩码(支持的是1维还是二维...)
	int capAxMask;
	// 触发源序号，（可以理解为锁存信号输入的所在轴号）
	int trigIndex;
	// 触发电平，0：上升，1：下降沿
	int trigLev;
	// 滤波时间常数,单位:微秒,取值范围[0,255]
	int filter;
	//以下为预留参数
	float data0;
	float data1;
	char data2;
	char data3;
	char data4;
	char data5;
	int data6;
}LocationLatchParam, *pLocationLatchParam;

//分频输出参数结构体
typedef struct LinearCmpDivParam
{
	//待分频的轴号
	int axis;
	// 比较源：1:编码器位置  0：规划位置
	int src;		
	// 比较方向， 0：正向；1：负向；2-双向。20210911为了兼容嵌入式修改
	int dir;	
	// 启动位置，只支持一维
	int  startPos;	
	// 位置间隔
	int gateDistance;		
	// 输出的脉冲宽度，单位us
	int gateTime;		
	//以下为预留参数
	char data0;		//0-不使用指定位置触发模式；1-使用指定位置触发模式
	char data1;		//保留
	char data2;		//保留
	char data3;		//保留
}LinearCmpDivParam, *pLinearCmpDivParam;
