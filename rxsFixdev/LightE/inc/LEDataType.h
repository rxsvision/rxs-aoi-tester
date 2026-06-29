#pragma once
#define APP_FILTER 0		//滤波功能编号
#define APP_EXTREME 1		//去极值功能编号
//应用测量功能宏定义
#define APP_THICKNESS_LENS 0		//镜片对射测厚功能编号
//滤波参数结构体
typedef struct FilterParam
{
	//是否启用滤波功能
	char use;	
	//滤波方式，0为1*3近似高斯滤波,1为1*3中值滤波，3-为滑动平均
	char mode;	
	//2项预留参数
	char data0;							
	char data1;
	//滤波次数
	int count;							
}FilterParam, *pFilterParam;
//去极值参数结构体
typedef struct ExtremeDataParam
{
	//是否启用去极值功能
	char use;
	//去极值模式,模式0-使用目标点与参考点做高度差判断是否极值；模式1-使用二阶导数来判断是否极值，
	//适应性更好，优先推荐此模式。
	char mode;
	//20220506改为预留，数据填充模式统一使用MarkDataFitParam数据结构中的参数。
	char fittedMode;
	//预留
	char data0;
	//数据是否有效判断点数,默认为10，最小可设置3
	int refPoint;
	//待判断点与参考点的高度差允许的最大波动范围，默认为10um。
	float HThreshold;
	//待判断点有效时左右有效参考点占总参考点数量的最低比例通常为0.6，最大可设置1
	float validRa;
	//20220506改为预留
	int data1;
	//20220506改为预留，数据填充模式统一使用MarkDataFitParam数据结构中的参数。
	int data2;
}ExtremeDataParam, *pExtremeDataParam;
//圆孔胶水计算参数结构体
typedef struct GlueCalcParam
{
	//寻找胶水模式，0-自动寻找最高点胶水厚度，适用于扫描高度截面包含2个上表面的数据，1-直接找两
	//边最大值作为胶水高度，然后计算胶水厚度，此模式适用于扫描高度截面不包含2个上表面的数据，默
	//认为0。
	int mode;
	//导脚过滤占孔深的百分比,一道胶默认为5%,二道胶默认为20%
	int refRa;
	//平面斜率变化率阈值，默认为0.1，可设范围0.1~0.5
	float planeRa;
	//曲面斜率变化率阈值，默认为0.6，可设范围0.1~5,一道胶经验值为0.3，二道胶经验值为0.6
	float glueRa;
	//滤波次数，默认为20，可根据原始数据是否有很多毛刺来加大或减小，一道胶经验值为10，二道胶经验值为20
	int smooth;
	//判断一段连贯数据是否为有效数据的最少数据点数，默认为20，可根据实际测试时干扰数据段的长短来
	//调节，通常要设置为大于干扰数据长度的值。
	int subsectionCnt;
	//产品扫描时的倾斜角度，默认为0，如果实际测量时使产品倾斜了则将倾斜角度填入此参数
	float RtAngle;
	//相邻数据点之间的物理间距，单位μm	，默认为1um，按实际设置
	float step;
	//（一道胶计算专用）判断与上表面相平胶水时的斜率变化率阈值，默认为0.2
	float slopRanage;
	//（一道胶计算专用）曲面斜率最小值，默认为0.1
	float slopRaThd;
	//（一道胶计算专用）曲面最少有效点数量，默认为30
	int slopeValidPts;
	//是否启用导脚过滤，0-表示不开启，1-表示开启，默认启用
	char footFilter;
	//自动倾斜修正开关，0-表示不开启，1-表示开启，默认不开启，此功能目前效果欠佳
	char titleCorrection;
	//（一道胶计算专用）胶水与上表面相平判断开关，0-表示不开启，1-表示开启,注意此判断功能仅限于一道胶测量，测量二道胶时禁止开启
	char judgeGlueFlatPlane;
	//预留
	char data0;
}GlueCalcParam, *pGlueCalcParam;
//圆孔胶水计算结果结构体
typedef struct ProductResult
{
	//计算出的胶水厚度
	float glueThickness;
	//找到的最高点胶水读数
	float glue;
	//圆孔底部高度读数
	float bottom;
	//圆孔上表面高度读数
	float top;
	//寻找到的胶水最高点数据下标
	int gluePos;
	//寻找到的圆孔上表面第一个数据下标
	int topPos;
	//寻找的圆孔底部第一个数据下标
	int bottomPos;
}ProductResult, *pProductResult;
//单边测厚单层上下表面峰序号结构体
typedef struct TKPeakIdx
{
	//上表面的序号
	char top;
	//下表面的序号，注意下表面的序号必须大于上表面
	char bottom;
	//层编号，最小为1
	char idx;
	//预留
	char data1;
}TKPeakIdx, *pTKPeakIdx;
//数据拟合功能相关参数结构体
typedef struct InvalidDataFitParam
{
	//数据拟合算法，0-直线拟合法
	char mode;
	//预留参数0
	char data0;
	//预留参数1
	char data1;
	//预留参数2
	char data2;
	//允许拟合单段无效数据最大长度，大于0的参数为实际限制长度，20220506兼容嵌入式参数修改
	int limitCount;
}InvalidDataFitParam, *pInvalidDataFitParam;
//自动曝光参数结构体
typedef struct AutoEptGainParam
{
	//自动曝光增益模式，各设置值对应的模式为：0-不启用自动曝光；1-智能模式；2-快速增强模式；
	//3-快速减弱模式；4-缓慢增强和减弱模式：默认为0。
	char AEGMode;			
	//预留参数0
	char data0;
	//预留参数1
	char data1;
	//预留参数2
	char data2;
	//缓慢调节时曝光实际或增益调节的幅度,默认为20%
	float AEGRa;	
	//单个像素光强的有效范围,百分比形式
	float VIRangeRa0;					
	//单个像素光强的有效范围,百分比形式
	float VIRangeRa1;	
	//自动曝光时间及增益参考帧数
	int AEGRefFps;									
}AEGParam, *pAEGParam;
//数据段末端数据优化结构体
typedef struct EdgeDataProcessParam
{
	//需优化数据角度阈值，数据段整体小于此角度时不优化，大于此角度时才优化，单位为°
	float angle;
	//允许修正的异常数据最长长度，此参数设置为0-不限制长度；设为大于0的数值x时，当异常数据
	//段长度小于x则优化，大于x则不优化。
	int invalidCount;
	//参考点1
	int refCount1;
	//是否启用该功能，0-不启用；1-启用
	char use;
	//预留参数0
	char data0;
	//预留参数1
	char data1;
	//预留参数3
	char data3;
}EDPParam, *pEDPParam;
//被标记数据通用填充方式，通常由数据处理类算法来标记异常值
typedef struct MarkDataFitParam
{
	//填充模式，0-直线拟合；1-设为量程最小值；2-设为量程最大值；3-使用自定义值填充；
	//4-设为无效标记值-2147483640；5-曲线拟合(暂未实现)。
	int mode;
	//当填充模式为3，即设置为自定义填充时，所填入的值
	float markData;
}MarkDataFitParam, *pMarkDataFitParam;
//使用折射率测试透明产品厚度参数
typedef struct RefractiveParam
{
	//黄光，587.56nm波长处折射率
	float nd;
	//青光，486.13nm波长处折射率
	float nF;
	//红光，656.27nm波长处折射率
	float nC;	
	//阿贝数
	float vd;	
	//预留
	float data0;
	//预留
	float data1;		
}RefractiveParam, *pRefractiveParam;
//双探头对射测厚自动标定结构体
typedef struct DeviceChannelIdx
{
	//探头1设备句柄
	int dc1;
	//探头1通道编号
	int ch1;
	//探头2设备句柄
	int dc2;
	//探头2通道编号
	int ch2;			
}DCCHIdx, *pDCCHIdx;

//分频触发预设参数
typedef struct FrequencyPresetParam
{
	//轴计数是否使能，0-禁能；1-使能（仅E系列支持）。
	unsigned char enableCount;
	//20220609修改为预留，原功能迁移到FNormalParam结构体中。
	unsigned char SPSta;
	//20220610修改为传感器编码器加计数方向，0表示A相超前于B相，1表示A相落后于B相（注意该参数设置仅对E型控制器有效）。
	unsigned char data0;
	//预留
	unsigned char data2;
	//编码器触发信号电平宽度，E系列和G系列控制器默认为1，设置的值需大于等于控制器要求的外触发信号宽度，分频卡默认设置为70us。
	unsigned short OutLevelWidth;
	//预留
	unsigned short data1;
}FPresetParam, *pFPresetParam;
//分频触发常用参数
typedef struct FrequencyNormalParam
{
	//触发方向， 0：正向；1：负向；2-双向。20210911为了兼容嵌入式修改
	unsigned char dir;
	//20220609修改为是否启用从指定位置开始输出触发信号采集数据模式，0-禁用；1-启用，注意若使用此功能，则每次修改开始触发位置时此参数都应该设置为1
	unsigned char data0;
	//预留
	unsigned char data1;
	//预留
	unsigned char data2;
	//分频系数
	int step;
	//从指定位置开始输出触发信号模式的起点位置，在其他条件都满足的情况下，当设置为正向触发时，轴运动到大于等于该位置
	//时开始输出触发信号，当设置为负向触发时，轴运动到小于等于该位置时开始输出触发信号。
	int startPos;
}FNormalParam, *pFNormalParam;
//计算出的最大值和最小值的结果数据及数据下标
typedef struct MinimaxVal
{
	//最小值
	double minVal;
	//最大值
	double maxVal;
	//最小值下标
	int minIdx;
	//最大值下标
	int maxIdx;
}MinimaxVal, *pMinimaxVal;
//对射测厚同一控制器探头组合20220518新增
typedef struct ThicknessCalibration
{
	//组合01通道序号，序号从1开始。
	unsigned int chIdx01;
	//组合02通道序号，序号从1开始。
	unsigned int chIdx02;
	//两通道当前校准的对射厚度值，单位μm。
	float caliDTkVal;
	//校准时通道01内部保存的位移值，单位μm。(用户校准时无需输入)
	float caliDT01Val;
	//校准时通道02内部保存的位移值，单位μm。(用户校准时无需输入)
	float caliDT02Val;
}ThicknessCalibration, *pThicknessCalibration;

//[仅支持G系列控制器]嵌入式控制器Modbus数据交换通讯参数，20220927新增
typedef struct ModbusTcpParam
{
	//启动通讯开关，0：关闭，1：开启
	unsigned int enable ;
	//modbus服务器地址，20230406修改定义,例如IP地址为192.168.1.100，则ipAddress[0]=192
	//;ipAddress[1]=168;ipAddress[2]=1;ipAddress[3]=100
	unsigned char ipAddress[64];
	//modbus服务器地址端口
	unsigned int port ;
	//传感器数据写入的寄存器首地址
	unsigned int wrAddress ;
	//控制器ID
	unsigned int data01 ;
	//预留02
	unsigned int data02 ;
	//预留03
	unsigned int data03 ;
	//预留04
	unsigned int data04 ;
}ModbusTcpParam, * pModbusTcpParam;
//单边测厚不同厚度之间的校准系数结构体
typedef struct DifThicknessCalibration
{
	//当前档厚度值
	unsigned int standardTK;
	//预留
	unsigned int data0;
	//校准系数
	double calibrationK;
}DifThicknessCalibration, * pDifThicknessCalibration;
//单边测厚单层上下表面峰序号及取峰方式结构体
typedef struct ThicknessPeakParam
{
	//上表面的序号
	char top;
	//下表面的序号，注意取峰模式为正序时，下表面的序号必须大于上表面
	char bottom;
	//上表面取峰模式，0-正序；1-倒序
	char topCaptureMode;
	//下表面取峰模式，0-正序；1-倒序
	char bottomCaptureMode;
}ThicknessPeakParam, *pThicknessPeakParam;
//使用折射率测试透明产品厚度参数
typedef struct RefractiveParamSimpli
{
	//黄光，587.56nm波长处折射率
	float nd;
	//青光，486.13nm波长处折射率
	float nF;
	//红光，656.27nm波长处折射率
	float nC;
	//阿贝数
	float vd;
}RefractiveParamSimpli, * pRefractiveParamSimpli;
//20230302
//控制器信息结构体
typedef struct LEDeviceInfomation
{
	//控制器类型
	unsigned int type;
	//端口号
	unsigned int port;
	//型号名称
	char name[32];
	//序列号
	char SN[32];
	//固件及应用程序版本
	char FMVersion[32];
	//IP地址
	char IP[32];
}LEDeviceInfomation, *pLEDeviceInfomation;