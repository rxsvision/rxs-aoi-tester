/*
文件版本：V1.80.1.X
最后修改日期：20230617
修改人：long
*/
#pragma once
#ifdef CONFOCAL_DLL
#else
#define CONFOCAL_DLL extern "C" _declspec(dllimport)
#endif
#include"LEDataType.h"
/*
函数功能：数据采集任务完成或被主动停止采集任务后DLL内部执行的回调函数类型。
参数：
capturedCount-保存当前任务已采集到的数据数量。
返回值：无。
*/
typedef void(*CBCaptureComplete)(int capturedCount);
/*
函数功能：获取当前DLL的版本号。
参数：
pDllVersion-[out]保存获取到的DLL版本号，格式类似于“V1.80.0.0”。
len-[in]pDllVersion的存储长度，需大于等于32。
返回值：无。
*/
CONFOCAL_DLL void __stdcall LE_GetDLLVersion(wchar_t *pDllVersion,int len);

/*
函数功能：选择所使用的控制器类型，本函数必须在LE_InitDLL前调用，一旦调用LE_InitDLL初始化成功后，
本函数将不能再调用，除非调用LE_UnInitDLL进行反初始化才可以。
参数：
dcType-[in]选择使用的控制器类型，0-PC版USB 3.0接口1代，1-PC版以太网接口，2-PC版USB 3.0接口2代；3-嵌入式控制器E系列；
4-嵌入式控制器G系列；DLL内部默认为USB 3.0接口2代。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SelectDeviceType(int dcType);

/*
函数功能：获取当前所使用的控制器类型，本函数可以在任意位置调用。
参数：
返回值：
0-USB 3.0接口1代；
1-以太网接口；
2-USB 3.0接口2代；
3-嵌入式控制器E系列；
4-嵌入式控制器G系列。
*/
CONFOCAL_DLL int __stdcall LE_GetDeviceType();

/*
函数功能：初始化控制器DLL，调用本函数前需插入匹配的加密狗，使用LE_InitDLL进行初始化成功后，必须
在应用程序关闭之前调用一次且只能调用一次LE_UnInitDLL释放初始化所获得的资源。
参数：
返回值：
1-初始化成功；
-29-未找到匹配的加密狗；
-30-USB一代控制器初始化失败，请检查是否已安装USB一代最新版驱动；
-31-以太网控制器初始化失败，请检查是否已安装以太网版本驱动；
-32-USB二代控制器初始化失败，请检查是否已安装USB二代最新版驱动；
其他返回值请参考返回值定义表，返回值类型在20190725由bool改成int类型。
*/
CONFOCAL_DLL int __stdcall LE_InitDLL();

/*
函数功能：反初始化控制器DLL。
参数：
返回值：成功返回true，失败返回false。
*/
CONFOCAL_DLL bool __stdcall LE_UnInitDLL();

/*
函数功能：[调用时不能有控制器被打开]设置期望连接嵌入式的控制器数量，最大数量为4，
所以调用此函数时请确保没有控制器为打开状态，否则可能会出现控制器状态异常的现象。
参数：
返回值：成功返回1，失败返回-10。
*/
CONFOCAL_DLL int __stdcall LE_SetEBDSpecCount(int t_devCnt);

/*
函数功能：[调用时不能有控制器被打开]获取控制器数量，此函数每次执行都会重新枚举一次控制器数量，所
以调用此函数时请确保没有控制器为打开状态，否则可能会出现控制器状态异常的现象。
参数：
返回值：成功返回获取到的数量，失败返回-10。
*/
CONFOCAL_DLL int __stdcall LE_GetSpecCount();

/*
函数功能：获取已连接电脑的所有控制器的序列号。
参数：
pSns-[out]保存所有控制器序列号的数组，每个控制器序列号固定占用32个数组元素，pSns数组长度需大于等
于32*count。
count-[in]控制器的数量,最大为10个。
返回值：成功返回true，失败返回false。
*/
CONFOCAL_DLL bool __stdcall LE_GetSpecSN(wchar_t *pSns, int count);

/*
函数功能：获取控制器SN号，名称、固件版本号等信息，每个控制器的每一项属性固定占用32个数组元素，例
如第一个控制器SN号的长度只有12个字符，那第二个控制器的SN号第一个字符存储的位置为pSNs[1*32]，其他
属性也是如此。
参数：
pSns-[out]保存获取到的序列号数组，长度需大于等于32*count。
pName-[out]保存获取到的控制器名称数组，长度需大于等于32*count。
pFMVersion-[out]保存获取到的固件数组，长度需大于等于32*count。
count-[in]期望获取信息的控制器数量。
返回值：成功返回true；失败返回false。
*/
CONFOCAL_DLL bool __stdcall LE_GetSpecMsg(wchar_t *pSns, wchar_t *pName, wchar_t *pFMVersion, int count);

/*
 函数功能：打开指定序列号控制器。
 参数：
 pSn-[in]控制器序列号，长度为32的字符串。
 specHandle-[out]打开成功后传出的控制器句柄。
 返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_Open(wchar_t *pSn, int &specHandle);

/*
函数功能：关闭指定句柄控制器。
参数： 
specHandle-[in]需关闭的控制器句柄。
返回值：成功返回1，其他返回值请参考返回值定义表	。
*/
CONFOCAL_DLL int __stdcall LE_Close(int &specHandle);

/*
函数功能：[只能在控制器空闲状态下调用]设置指定控制器采样频率，注意控制器采样频率设置可能会影响到
曝光时间，比如说当前控制器的曝光时间为1.5ms的时候，如果设置采样频率为1000Hz的时候，控制器内部会
优先满足采样频率，从而内部强行降低曝光时间至0.88ms（1000Hz频率下曝光时间能设置的理论上限为1ms，
然后再减去0.12ms的计算时间就等于0.88ms）左右，G系列控制器不受此限制。
参数：
sampleHz-[in]待设置采样频率。
specHandle-[in]控制器句柄。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetSampleFrequency(int sampleHz, int specHandle);

/*
函数功能：获取控制器采样频率。
参数：
sampleHz-[out]待获取采样频率。
specHandle-[in]控制器句柄。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetSampleFrequency(int &sampleHz, int specHandle);

/*
函数功能：获取控制器曝光时间范围，单位ms。
参数：
eptMin-[out]保存获取到的曝光时间范围最小值。
eptMax-[out]保存获取到的曝光时间范围最大值。
specHandle-[in]控制器句柄。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetExposureRange(float *eptMin, float *eptMax, int specHandle);

/*
函数功能：设置曝光时间，单位为ms。
参数：
expTime-[in]待设置曝光时间。
specHandle-[in]控制器句柄。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetExposureTime(float expTime, int specHandle);

/*
函数功能：获取曝光时间,单位为ms。
参数：
expTime-[out]保存获取到的曝光时间值。
specHandle-[in]控制器句柄。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetExposureTime(float &expTime, int specHandle);

/*
函数功能：获取控制器增益范围。
参数：
gainMin-[out]保存获取到的增益范围最小值。
gainMax-[out]保存获取到的增益范围最大值。
specHandle-[in]控制器句柄。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetGainRange(float *gainMin, float *gainMax, int specHandle);

/*
函数功能：设置控制器增益。
参数：
ga-[in]待设置增益值。
specHandle-[in]控制器句柄。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetGain(float ga, int specHandle);

/*
函数功能：获取控制器增益。
参数： 
ga-[out]保存获取到的增益值。
specHandle-[in]控制器句柄。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetGain(float &ga, int specHandle);

/*
函数功能：获取通道光斑中心位置,及通道光斑所占行数。
参数： 
offsetLine-[out]保持获取到的能量中心位置。
lines-[out]保存获取到的光斑所占行数。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetPosOffset(int &offsetLine, int &lines, int specHandle, int channel = 1);

/*
函数功能：设置控制器触发模式。
参数：
triggerMode-[in]触发模式：0-表示连续获取模式;2-表示硬件触发模式1;3-表示硬件触发模式2（此模式支持一个触发信号采集多帧数据
，需控制器支持该功能,控制器上有IN2+、IN2-的端子标识即表示控制器支持此功能）;4-编码器分频触发模式。
specHandle-[in]控制器句柄。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetTriggerMode(int triggerMode, int specHandle);

/*
函数功能：获取控制器触发模式。
参数：
triggerMode-[out]保存获取到的触发模式：0-表示连续获取模式;2-表示硬件触发模式1;3-表示硬件触发模式2（此模式支持一个触发信号采集多帧数据
，需控制器支持该功能,控制器上有IN2+、IN2-的端子标识即表示控制器支持此功能）;4-编码器分频触发模式。
specHandle-[in]控制器句柄。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetTriggerMode(int &triggerMode, int specHandle);

/*
函数功能：设置控制器外触发模式下的触发源，只能在硬件触发模式修改，且只在硬件触发模式下起作用。
参数：
sourceMode-[in]待设置触发源类型，0-上升沿触发，1-下降沿触发，当使用边沿触发时，传感器按外触发信
号频率来采集数据，2-高电平触发，3-低电平触发，当使用有效电平来触发时，在信号有效时控制器按设定的
频率连续采集数据。
specHandle-[in]控制器句柄。
返回值：1-设置成功；其他返回值请参照返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetTriggerSource(int sourceMode, int specHandle);

/*
函数功能：获取控制器外触发模式下的触发源。
参数：
sourceMode-[out]保存获取到的触发源类型，含义同LE_SetTriggerSource中第一个参数保持一致。
specHandle-[in]控制器句柄。
返回值：1-获取成功；其他返回值请参照返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetTriggerSource(int &sourceMode, int specHandle);

/*
函数功能：[只能在控制器空闲状态下调用]通过文件载入控制器所有通道配置数据并启用所有通道。
参数：
filePath-[in]目标文件路径字符数组，允许最大长度2000，路径末尾必须有空字符（即ASCII码为0的字符）
结尾,文件格式为.dcfg。
specHandle-[in]控制器句柄。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int  __stdcall LE_LoadDeviceConfigureFromFile(wchar_t *filePath, int specHandle);

/*
函数功能：获取指定控制器通道数量。
参数：
specHandle-[in]控制器句柄。
返回值：成功返回获取到的大于等于0的通道数量；负数-获取失败，含义请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetChannels(int specHandle);

/*
函数功能：[只能在控制器空闲状态下调用]开启或关闭指定控制器指定通道，此函数只能在指定控制器无采集
任务时才能执行成功。
参数：
specHandle-[in]控制器句柄。
channel-[in]通道编号，当前默认为1。
bEnable-[in]true表示启用通道，false表示禁用通道。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_EnableChannel(int specHandle, int channel = 1, bool bEnable = true);

/*
函数功能：[只能在控制器空闲状态下调用]通过文件载入指定控制器所有通道高度校准数据，20210202修改。
参数：
filePath-[in]目标文件路径字符数组，允许最大长度2000，路径末尾必须有空字符（即ASCII码为0的字符）
结尾，文件格式为.hwc。
specHandle-[in]控制器句柄。
channel-[in]预留。
pFiberSN-[in]光纤序列号，目前默认为空，调用函数时无需输入参数。
pLensSN-[in]探头序列号，目前默认为空，调用时无需输入参数。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_LoadLWCalibrationData(wchar_t *filePath, int specHandle, int channel = 1, wchar_t *pFiberSN = 0, wchar_t *pLensSN = 0);

/*
函数功能：将当前控制器所有通道计算参数保存到指定文件中，注意保存之前必须至少成功载入过一次参数
20210202修改为保存所有通道参数。
参数：
filePath-[in]目标文件路径字符数组，允许最大长度2000，路径末尾必须有空字符（即ASCII码为0的字符）
结尾,文件格式为.dcfg，嵌入式控制器此参数可传入空字符数组。
specHandle-[in]控制器句柄。
channel-[in]预留。
pFiberSN - [in]光纤序列号，目前默认为空，调用函数时无需输入参数。
pLensSN - [in]探头序列号，目前默认为空，调用时无需输入参数。
返回值：[in]成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SaveParamToFile(wchar_t *filePath, int specHandle, int channel = 1, wchar_t *pFiberSN = 0, wchar_t *pLensSN = 0);

/*
函数功能：[只能在控制器空闲状态下调用]采集控制器已启用通道的dark数据，采集前请先用黑色盖子盖住对
应通道探头，直到采集完成。
参数：
specHandle-[in]控制器句柄。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_CaptureChannelsIntensityCaliData(int specHandle);

/*
函数功能：获取控制器通道当前Dark完成进度，单位为%，当开启了多通道时，所有通道Dark进度是一致的只
需获取一个通道进度即可。
参数：	
rate-[out]保存获取到的进度比例，单位为%。
specHandle - [in]控制器句柄。
channel-[in]通道序号。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetDarkRate(int *rate, int specHandle, int channel = 1);

/*
函数功能：获得指定通道单帧波形曲线数据长度及最大能达到的能量值。
参数：
specHandle-[in]控制器句柄。
length-[out]保存获取到的单帧波形曲线数据数组的长度，不同类型控制器长度可能不一样。
maxValue-[out]保存波形曲线数据中波峰能达到的最大能量值。
channel-[in]通道序号。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetChannelIntensityMsg(int specHandle, int *length, int *maxValue, int channel = 1);

/*
函数功能：设置测量到无效数据时的处理方式。
参数：
prdMod-[in]待设置数据处理模式,0-不处理，1-保持，2-改为量程内最小值，3-改为量程内最大值,4-拟合，
当选择此项后，通道内部的拟合参数会影响拟合效果。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
pFiberSN-[in]光纤序列号，目前默认为空，调用函数时无需输入参数。
pLensSN-[in]探头序列号，目前默认为空，调用时无需输入参数。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetInvalidDataPrdModel(int prdMod, int specHandle, int channel = 1, wchar_t *pFiberSN = 0, wchar_t *pLensSN = 0);

/*
函数功能：获取测量到无效数据的处理方式。
参数：
prdMod-[out]保存获取到的数据处理模式,0-不处理，1-保持，2-改为量程内最小值，3-改为量程内最大值，4-拟合。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
pFiberSN-[in]光纤序列号，目前默认为空，调用函数时无需输入参数。
pLensSN-[in]探头序列号，目前默认为空，调用时无需输入参数。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetInvalidDataPrdModel(int &prdMod, int specHandle, int channel = 1, wchar_t *pFiberSN = 0, wchar_t *pLensSN = 0);

/*
函数功能：设置无效数据使用拟合来处理的参数结构体。
参数：
IDFParam-[in]待设置的无效数据拟合相关参数结构体。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetInvalidDataFitParam(InvalidDataFitParam IDFParam, int specHandle, int channel = 1);

/*
函数功能：获取无效数据使用拟合来处理的参数结构体。
参数：
IDFParam-[out]保存获取到的无效数据拟合相关参数结构体。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetInvalidDataFitParam(InvalidDataFitParam &IDFParam, int specHandle, int channel = 1);

/*
函数功能：设置通道取峰模式(仅作用于高度或位移数据类型)。
参数：
CPeakMode-[in]待设置取峰模式，0-取最优峰；1-取最近峰；2-取最远峰；3-第n峰；4-倒数第n峰。
specHandle-[in]控制器句柄。
channel-[in]通道号。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetCapturePeakMode(int CPeakMode, int specHandle, int channel = 1);

/*
函数功能：获取通道取峰模式。
参数：
specHandle-[in]控制器句柄。
channel-[in]通道号。
返回值：小于0表示获取失败；0-取最优峰；1-取最近峰；2-取最远锋；3-第n峰；4-倒数第n峰,注意
嵌入式控制器暂不支持“最优峰”模式。
*/
CONFOCAL_DLL int __stdcall LE_GetCapturePeakMode(int specHandle, int channel = 1);

/*
函数功能：通道测量高度结果时，设置取峰模式为“第n峰”或“倒数第n峰”时的目标峰序号。
参数：
peakIdx-[in]待设置峰序号，最小可设置为1。
specHandle-[in]控制器句柄。
channel-[in]通道号。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetDistancePeakIdx(int peakIdx, int specHandle, int channel = 1);

/*
函数功能：获取通道测量高度结果时，“第n峰”或“倒数第n峰”时的目标峰序号。
参数：
specHandle-[in]控制器句柄。
channel-[in]通道号。
返回值：大于等于1的值表示目标峰序号，负值表示获取失败，含义请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetDistancePeakIdx(int specHandle, int channel = 1);

/*
函数功能：获取指定控制器当前实际的采样频率。
参数：
spFrequency-[out]保存获取到的采样频率，控制器如果在采集数据则输出实时帧率，如果控制器未在采集数
据则输出-1。
specHandle - [in]控制器句柄。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetCurrentSampleFrequency(int *spFrequency, int specHandle);

/*
函数功能：获取指定控制器最大能达到的采样频率。
参数：
maxSPFrequency-[out]保存获取到的最大采样频率。
specHandle - [in]控制器句柄。
返回值：1-获取成功；其他返回值请参照返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetLimitSampleFrequency(int *maxSPFrequency, int specHandle);

/*
函数功能：设置自动调节曝光、增益功能相关参数。
参数：
pAEGParam-[in]待设置的参数结构体。
specHandle-[in]控制器句柄。
返回值：1-设置成功；其他返回值请参照返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetAutoEptGainParam(AEGParam *pAEGParam, int specHandle);

/*
函数功能：获取自动调节曝光、增益功能相关参数。
参数：
pAEGParam-[out]存储获取到的参数结构体。
specHandle-[in]控制器句柄。
返回值：1-设置成功；其他返回值请参照返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetAutoEptGainParam(AEGParam *pAEGParam, int specHandle);

/*
函数功能：[只能在控制器空闲状态下调用]设置指定控制器当前与软件连接的通讯超时时间，默认设置为3s，
即如果软件出现意外情况崩溃或者卡死时，控制器等待的时间，如果超过指定时间控制器仍未与软件通讯上则
控制器将主动断开与软件的连接，此时软件要继续访问控制器采集数据需重新打开连接控制器，该功能通常用
于二次开发时调试程序使用，调试程序时该时间需设置大一些，这样在程序进入断点中断时控制器不至于断开
与软件的连接，发布程序请将该时间设置为3s钟左右，如果设置时间过长会导致软件意外结束时，需至少等待
大于通讯超时时间的设置值才能重新打开控制器，或者控制器需要断电重启，此参数仅作用与以太网控制器。
参数：	
hTime-[in]待设置通讯超时时间，单位为ms。
specHandle-[in]控制器句柄。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetHeartTime(int hTime, int specHandle);

/*
函数功能：获取指定控制器通讯超时时间,此参数仅以太网控制器支持。
参数：	
hTime-[out]保存获取到的通讯超时时间，单位为ms。
specHandle-[in]控制器句柄。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetHeartTime(int *hTime, int specHandle);

/*
函数功能：[只能在控制器空闲状态下调用]获取指定控制器一个或多个通道的高度测量结果,函数执行后控制
器才开始采集数据，采集完成后函数才返回执行状态，并且结果值存储在pValue中，注意此函数每一个通道只
能获取一个高度结果，支持获取多个通道，例如channels的值为2时，函数执行完后pValue保存2个高度结果，
分别为pCHNumber[0]通道和pCHNumber[1]通道的高度结果。
参数：
pValue-[out]保存获取到的测量结果，如果需要获取多个通道结果时，所有结果按通道顺序依次保存。
specHandle-[in]控制器句柄。
pCHNumber-[in]期望获取数据的通道编号数组
channels-[in]pCHIdx数组的长度，期望获取结果值的通道数量。
data0-[in]预留参数。
data1-[in]预留参数。
pPrdItn-[out]保存channels个通道的波形曲线数据数组,数组长度为channels*length（length代表单帧光谱
的长度，可以通过LE_GetChannelIntensityMsg函数获得），每个通道保存一帧，如果不需要获取此项数据则
此参数传入0或空指针。
pUnitRa-[out]保存channels个通道的感光度数组，长度等于channels，如果不需要获取此项数据则此参数传
入0或空指针。
pPeakWave-[out]保存channels个通道的波峰像素位置数组，长度等于channels，如果不需要获取此项数据则
此参数传入0或空指针。
pPeakItn-[out]保存channels个通道的波峰光强数组，长度等于channels，如果不需要获取此项数据则此参数
传入0或空指针。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetSingleValueRTEx(double *pValue, int specHandle,int *pCHNumber, int channels = 1,
	int data0 = 0, int data1 = 0, int *pPrdItn = 0, float* pUnitRa = 0, float *pPeakWave = 0, int *pPeakItn = 0);
/*
函数功能：[只能在控制器空闲状态下调用]获取指定控制器一个或多个通道的厚度测量结果,函数执行后控制
器才开始采集数据，采集完成后函数才返回执行状态，并且结果值存储在pTKValue、pDistance1、pDistance2
中，注意此函数每一个通道只能获取tkCount个结果，支持获取多个通道，例如channels的值为2，tkCount值
为1时，函数执行完后pTKValue保存2*1个厚度结果，分别为pCHNumber[0]通道的第一层厚度和pCHNumber[1]通
道的第一层厚度结果，同理pDistance1、pDistance2存储情况也是一样。
参数：
pTKValue-[out]获取到的厚度测量结果，该数组长度需大于等于tkCount*channels。
tkCount-[in]期望获取的层数。
specHandle-[in]控制器句柄。
pCHNumber-[in]期望获取数据的通道编号数组。
channels-[in]pCHNumber数组的长度，期望获取结果值的通道数量。
pDistance1-[out]获取到的每一层上表面测量结果，该数组长度需大于等于tkCount*channels，如果不需要该
值则参数输入0或空指针，假设tkCount=2，channels=2，则第二个通道的上表面测量结果存储的起始地址为
pDistance1[1*tkCount]。
pDistance2-[out]获取到的每一层下表面测量结果，该数组长度需大于等于tkCount*channels，如果不需要该
值则参数输入0或空指针,内部存储顺序与pDistance1相同。
data0-[in]预留参数。
data1-[in]预留参数。
pPrdItn-[out]保存channels个通道的波形曲线数据数组,数组长度为channels*length（length代表单帧光谱
的长度，可以通过LE_GetChannelIntensityMsg函数获得），每个通道保存一帧，如果不需要获取此项数据则
此参数传入0或空指针。
pUnitRa-[out]保存channels个通道的感光度数组，长度等于channels，如果不需要获取此项数据则此参数传
入0或空指针。
pPeakWave1-[out]保存channels个通道的上表面波峰像素位置数组，长度等于channels*tkCount，如果不需要
获取此项数据则此参数传入0或空指针。
pPeakWave2-[out]保存channels个通道的下表面波峰像素位置数组，长度等于channels*tkCount，如果不需要
获取此项数据则此参数传入0或空指针,注意pPeakWave1和pPeakWave2要么2个都获取，要么都不获取。
pPeakItn1-[out]保存channels个通道的上表面波峰光强数组，长度等于channels*tkCount，如果不需要获取
此项数据则此参数传入0或空指针。
pPeakItn2-[out]保存channels个通道的下表面波峰光强数组，长度等于channels*tkCount，如果不需要获取
此项数据则此参数传入0或空指针,注意pPeakItn1和pPeakItn2要么2个都获取，要么都不获取。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetSingleTKValueRTEx(double *pTKValue, int tkCount, int specHandle, int *pCHNumber,
	int channels = 1, double *pDistance1 = 0, double *pDistance2 = 0, int data0 = 0, int data1 = 0, int *pPrdItn = 0, 
	float* pUnitRa = 0, float *pPeakWave1 = 0, float *pPeakWave2 = 0, int *pPeakItn1 = 0, int *pPeakItn2 = 0);

/**********************************************************************************************/
/*
函数功能：获取指定控制器通道状态。
参数：
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：
0-控制器已完成上一次采集任务或无采集任务，等待开启采集任务指令；
1-采集数据中。
其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetDeviceStatus(int specHandle, int channel = 1);

/*
函数功能：查询指定通道已采集到的数据量,兼容任务模式和缓存模式。
参数：
values-[out]保存查询到的已采集的数据量。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetCapturedPoints(int &values, int specHandle, int channel = 1);

/*
函数功能：主动停止指定控制器数据采集任务，目前因多通道产品采集数据时需同开同停，故此函数即便在使
用多通道产品的时候也只需要调用一次即可将所用通道的采集任务停掉,兼容任务模式和缓存模式。
参数：
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_StopGetPoints(int specHandle, int channel = 1);

/*
函数功能：[只能在控制器空闲状态下调用]设置指定通道目标峰数,传感器配置文件内出厂已设置好，一般不
需要修改。
参数：
peaks-[in]待写入的“目标峰数”。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetTargetPeaks(int peaks, int specHandle, int channel = 1);

/*
函数功能：获取指定通道目标峰数。
参数：
peaks-[out]保存获取的“目标峰数”参数。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetTargetPeaks(int &peaks, int specHandle, int channel = 1);


/***************************************单边测厚相关函数***************************************/
/*
函数功能：设置单层或多层厚度上下表面峰序号。
参数：
pTKPeakIdxs-[in]待设置的单层或多层厚度上下表面峰序号定义结构体数组。
tkCount-[in]期望设置层数。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetTKPeakIdxs(TKPeakIdx *pTKPeakIdxs,int tkCount, int specHandle, int channel = 1);

/*
函数功能：获取指定通道多层厚度上下表面峰序号定义，tkCount为希望获取的层数，注意此函数只能按顺序
输出多层的上下表面峰序号定义，例如tkCount为2的时候，输出的是第一第二层厚度的定义。
参数：
pTKPeakIdxs-[out]保存获取到的多层厚度上下表面峰序号定义结构体
tkCount-[in]期望获取层数，目前最大为5层。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetTKPeakIdxs(TKPeakIdx *pTKPeakIdxs, int tkCount, int specHandle, int channel = 1);

/*
函数功能：设置指定层厚度上下表面选峰参数。
参数：
TKPeakParam-[in]待设置厚度上下表面选峰参数。
thicknessIdx-[in]期望设置的层序号，最小为0。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetTKPeakParam(ThicknessPeakParam TKPeakParam, int thicknessIdx, int specHandle, int channel = 1);

/*
函数功能：获取单层或多层厚度上下表面选峰参数，注意此函数只能按顺序
输出多层的上下表面峰序号定义，例如count为2的时候，输出的是第一第二层厚度的定义。
参数：
pTKPeakParam-[out]保存获取到的多层厚度上下表面选峰参数。
count-[in]期望获取层数，目前最大为5层。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetTKPeakParam(ThicknessPeakParam *pTKPeakParam, int count, int specHandle, int channel = 1);

/*
函数功能：通过文件载入指定通道厚度校准数据。
参数：
count-[out]载入成功后保存指定通道包含的材质校准数据数量。
filePath-[in]目标文件路径字符数组，允许最大长度2000，路径末尾必须有空字符（即ASCII码为0的字符）
结尾，文件格式为.tkc。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
pFiberSN-[in]光纤序列号，目前默认为空，调用函数时无需输入参数。
pLensSN-[in]探头序列号，目前默认为空，调用时无需输入参数。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_LoadTKCalibrationData(int &count,wchar_t *filePath, int specHandle, int channel = 1, wchar_t *pFiberSN = 0, wchar_t *pLensSN = 0);

/*
函数功能：获得当前通道包含的所有厚度校准数据材质名称。
参数：
materialName-[out]保存获取到的材质名称列表字符数组,长度为count*20。
count-[in]最大为LE_LoadTKCalibrationData获得的材质校准数据数量。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
pFiberSN-[in]光纤序列号，目前默认为空，调用函数时无需输入参数。
pLensSN-[in]探头序列号，目前默认为空，调用时无需输入参数。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetMaterialsName(wchar_t *materialsName, int count, int specHandle, int channel = 1, wchar_t *pFiberSN = 0, wchar_t *pLensSN = 0);

/*
函数功能：选择指定名称的材质厚度校准数据作为第一层厚度的校准数据。
参数：
materialName-[in]待选择材质名称,名称长度最大为20个双字节字符。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
pFiberSN-[in]光纤序列号，目前默认为空，调用函数时无需输入参数。
pLensSN-[in]探头序列号，目前默认为空，调用时无需输入参数。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SelectTKCaliData(wchar_t *materialName, int specHandle, int channel = 1, wchar_t *pFiberSN = 0, wchar_t *pLensSN = 0);

/*
函数功能：选择单层或按顺序选择多层指定名称的材质厚度校准数据。
参数：
materialNames-[in]待选择材质名称数组,单个名称长度最大为20个双字节字符，目前最多一次选择5层，数组
长度为20*count。
count-[in]期望选择层数，注意此函数选择时层数是按递增顺序的，例如count=3，则分别选择1、2、3层的厚
度校准数据。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SelectTKCaliDataEx(wchar_t *materialNames,int count, int specHandle, int channel = 1);

/*
函数功能：[只能在控制器空闲状态下调用]在线校准已知厚度样品的单边测厚系数。
参数：
thickness-[in]待校准材质厚度,单位μm。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
pFiberSN-[in]光纤序列号，目前默认为空，调用函数时无需输入参数。
pLensSN-[in]探头序列号，目前默认为空，调用时无需输入参数。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_CaliTKRa(float thickness, int specHandle, int channel = 1, wchar_t *pFiberSN = 0, wchar_t *pLensSN = 0);

/*
函数功能：[只能在控制器空闲状态下调用]在线校准已知单层或多层厚度样品的单边测厚系数。
参数：
pThickness-[in]待校准材质单层或多层厚度，数组长度为count。
pTKIdx-[in]待校准的层序号，数组长度为count。
count-[in]期望校准层数。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_CaliTKRaEx(float *pThickness, int *pTKIdx, int count, int specHandle, int channel);

/*
函数功能：将当前使用的第一层单边测厚系数存入指定文件。
参数：
filePath-[in]待保存文件路径，允许最大长度2000，路径末尾必须有空字符（即ASCII码为0的字符）结尾。
materialName-[in]待保存材质名称,单个名称长度最大为20个双字节字符，数组长度为20。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
pFiberSN-[in]光纤序列号，目前默认为空，调用函数时无需输入参数。
pLensSN-[in]探头序列号，目前默认为空，调用时无需输入参数。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SaveTKRa(wchar_t *filePath,wchar_t *materialName, int specHandle, int channel = 1, wchar_t *pFiberSN = 0, wchar_t *pLensSN = 0);

/*
函数功能：将当前单层或多层使用的单边测厚系数存入指定文件。
参数：
filePath-[in]待保存文件路径，允许最大长度2000，路径末尾必须有空字符（即ASCII码为0的字符）结尾。
materialNames-[in]单层或多层待保存材质名称,单个名称长度最大为20个双字节字符，数组长度为20*count
，目前最多一次保存5层。
pTKIdx-[in]待保存的层序号，数组长度为count。
count-[in]期望校准层数。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SaveTKRaEx(wchar_t *filePath, wchar_t *materialNames, int *pTKIdx, int count, int specHandle, int channel = 1);

/**********************************************************************************************/
/********************************************************进阶功能、二次校准相关****************/
/*
函数功能：通过给定像素位置数组以及高度数组计算传感器高度二次校准数据，像素及高度数组由用户提供。
参数：	
pWave-[in]像素数组。
pDistance-[in]高度数组。
count-[in]像素与高度数组长度。
bSave-[in]计算完后是否直接保存到已加载的高度校准文件。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_CalcDistanceCalibrationDataSecond(float *pWave, double *pDistance, int count,bool bSave, int specHandle, int channel = 1);

/*
函数功能：保存高度二次校准数据。
参数：
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：1-保存成功；其他返回值请参照返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SaveDistanceSecondCalibrationData(int specHandle, int channel = 1);

/*
函数功能：获取指定通道的高度校准数据数组。
参数：	
pDistance-[out]保存获取到的高度数组。
count-[in]pDistance的数组长度,通常等于传感器的像素长度。
calibrationType-[in]要获取的高度校准数据类型，0表示获取传感器出厂校准数组，1表示获取传感器二次校
准数组。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetDistanceCalibrationData(double *pDistance, int count, int calibrationType, int specHandle, int channel = 1);

/*
函数功能：设置指定通道高度模式时所使用的校准数据类型,未调用此函数时默认使用出厂校准数据。
参数：	
dataType-[in]0表示使用出厂校准数据，1表示使用用户二次校准数据。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：1-设置成功；其他返回值请参照返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetUseDistancesDataType(int dataType,int specHandle, int channel = 1);

/*
函数功能：获取指定通道高度模式时所使用的校准数据类型,未调用此函数时默认使用出厂校准数据。
参数：	
dataType-[out]保存获取到的校准数据类型。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：1-获取成功；其他返回值请参照返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetUseDistanceDataType(int *dataType, int specHandle, int channel = 1);

/******************应用测量与数据处理功能开关、参数设置相关函数*****************************/
/*
函数功能：开启或关闭指定通道结果数据处理功能。
参数：	
funcationIdx-[in]数据处理或功能编号，0-滤波功能，1-去极值功能，其他请参考数据处理功能定义表。
bUse-[in]为true时开启，为false时关闭。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：1-获取成功；其他返回值请参照返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_EnableAppFuncation(int funcationIdx, bool bUse, int specHandle, int channel);

/*
函数功能：设置结果数据滤波功能参数。
参数：	
filterParam-[in]滤波参数结构体。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：1-设置成功；其他返回值请参照返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetFilterParam(FilterParam filterParam, int specHandle, int channel);

/*
函数功能：获取结果数据滤波功能参数。
参数：	
filterParam-[out]用来保存获取到的滤波参数，注意指针filterParam只包含一个元素。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：1-获取成功；其他返回值请参照返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetFilterParam(FilterParam *filterParam, int specHandle, int channel);

/*
函数功能：设置结果数据去极值功能参数。
参数：	
extremeDataParam-[in]去极值参数结构体。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：1-设置成功；其他返回值请参照返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetExtremeParam(ExtremeDataParam extremeDataParam, int specHandle, int channel);

/*
函数功能：获取结果数据去极值功能参数。
参数：	
extremeDataParam-[out]用来保存获取到的去极值参数，注意指针extremeDataParam只包含一个元素。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：1-获取成功；其他返回值请参照返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetExtremeParam(ExtremeDataParam *extremeDataParam, int specHandle, int channel);

/*
函数功能：获取结果数据丢帧插补功能是否开启。
参数：	
sta-[out]用来保存获取到的丢帧插补功能状态，true-已开启，false-已关闭。
specHandle-[in]控制器句柄。
返回值：1-获取成功；其他返回值请参照返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetFillLostFrameStatus(bool *sta, int specHandle);

/*
函数功能：启用或禁用结果数据丢帧插补功能。
参数：	
sta-[in]true:启用控制器丢帧插补功能，false-禁用控制器丢帧插补功能，设置对象为控制器。
specHandle-[in]控制器句柄。
返回值：1-获取成功；其他返回值请参照返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_EnableFillLostFrameStatus(bool sta, int specHandle);

/*
函数功能：设置将小段数据设为指定值功能的参数20220424修改，bDeleteData、markValue参数改成预留。
参数：
bDeleteData-[in]预留。
continueCountThd-[in]有效数据段需至少包含大于此参数数量的数据，设为0时，此功能不启用，大
于1时功能启用。
markValue-[in]预留。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：设置成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetDeleteDataByContinueCountParam(bool bDeleteData, int continueCountThd, double markValue, int specHandle, int channel);

/*
函数功能：获取将小段数据设为指定值功能的参数20220424修改，bDeleteData、markValue参数改成预留。
参数：
bDeleteData-[out]预留。
continueCountThd-[out]有效数据段需至少包含大于此参数数量的数据。
markValue-[out]无效数据段填充值。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：获取成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetDeleteDataByContinueCountParam(bool &bDeleteData, int &continueCountThd, double &markValue, int specHandle, int channel);

/*
函数功能：设置边缘圆弧数据优化参数，以通道为单位设置。
参数：
param-[in]待设置参数。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：设置成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetEDPParam(EDPParam param, int specHandle, int channel);

/*
函数功能：获取边缘圆弧数据优化参数，以通道为单位设置。
参数：
param-[out]将获取到的参数保存到此变量。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：获取成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetEDPParam(EDPParam &param, int specHandle, int channel);

/*
函数功能：设置被标记数据通用填充参数。
参数：
param-[in]待设置参数。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：设置成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetMarkDataFitParam(MarkDataFitParam param, int specHandle, int channel);

/*
函数功能：获取被标记数据通用填充参数。
参数：
param-[out]将获取到的参数保存到此变量。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：获取成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetMarkDataFitParam(MarkDataFitParam &param, int specHandle, int channel);

/*
函数功能：使用给定的byte数据和宽、高生成指定路径和名称的BMP图像。
参数：
pData-[out]图像数据指针。
width-[in]图像数据分辨率宽度。
height-[in]图像数据分辨率高度。
filename-[in]BMP文件路径和名称。
返回值：获取成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SaveByteToBMP(char *pData, int width, int height,wchar_t* filename);

/*
函数功能：设置单边测厚的工作模式。
参数：
mode-[in]单边测厚模式，0-主动校准模式；1-折射率模式。
specHandle-[in]控制器句柄。
返回值：设置成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetTKMode(char mode,int specHandle);

/*
函数功能：获取单边测厚的工作模式。
参数：
specHandle-[in]控制器句柄。
返回值：0-主动校准模式；1-折射率模式，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL char __stdcall LE_GetTKMode(int specHandle);

/*
函数功能：设置单边测厚折射率模式下多层的折射率参数，设置只能按层的顺序设置。
参数：
pRft-[in]多层的折射率相关参数。
count-[in]pRft的数组长度，即要设置参数的层数，注意长度单位为/RefractiveParam，而不是字节，
例如count=3，则pRft包含3个RefractiveParam元素，设置第1~3层的参数。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：设置成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetRefractive(RefractiveParam *pRft, int count, int specHandle, int channel);

/*
函数功能：获取单边测厚折射率模式下多层的折射率参数。
参数：
pRft-[out]保存多层的折射率相关参数。
count-[in]pRft的最大长度，注意长度单位为/RefractiveParam。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
返回值：读取成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetRefractive(RefractiveParam *pRft, int count, int specHandle, int channel);

/*
函数功能：自动标定单对或多对探头对射测厚时各探头的值。
参数：
pDCCHIdx-[in]单对或多对对射探头所属的设备句柄和通道编号。
pLens1Rst-[out]保存count对对射探头中探头1的高度读数。
pLens2Rst-[out]保存count对对射探头中探头2的高度读数。
count-[in]pDCCHIdx数组、pLens1Rst数组、pLens2Rst数组各自的元素数量。
AEG-[in]是否使用自动曝光增益模式。
返回值：标定成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_AutoCalibrationLensPosInTK(DCCHIdx *pDCCHIdx, int count, double *pLens1Rst, double *pLens2Rst,bool AEG);

/*
函数功能：设置单点模式下平均次数,20210204新增。
参数：
avg-[in]平均次数。
specHandle-[in]控制器句柄。
返回值：设置成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetSinglePointAvg(int avg, int specHandle);

/*
函数功能：获取单点模式下平均次数,20210204新增。
参数：
avg-[out]保存获取到的平均次数。
specHandle-[in]控制器句柄。
返回值：获取成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetSinglePointAvg(int &avg, int specHandle);

/*
函数功能：清空控制器数据缓存。
参数：
specHandle-[in]控制器句柄。
返回值：1-修改成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_ClearBuffer(int specHandle);

/*
函数功能：切换控制器数据采集模式，以及获取的数据类型。
参数：
mode-[in]工作模式，0-采集任务模式；1-实时获取及缓存模式。
specHandle-[in]控制器句柄。
返回值：1-选择成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetCaptureDataMode(int mode, int specHandle);
/*
函数功能：获取控制器数据采集模式，以及获取的数据类型。
参数：
mode-[in]工作模式，0-采集任务模式；1-实时获取及缓存模式。
specHandle-[in]控制器句柄。
返回值：1-选择成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetCaptureDataMode(int &mode,int specHandle);

/*
函数功能：切换控制器采集的数据类型。
参数：
type-[in]测量数据类型，0-高度数据；1-厚度数据。
tkCount-[in]预留。
specHandle-[in]控制器句柄。
返回值：1-选择成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetMeasureType(int type, int tkCount, int specHandle, int channel = 1);
/*
函数功能：获取控制器采集的数据类型。
参数：
type-[in]测量数据类型，0-高度数据；1-厚度数据。
tkCount-[out]预留。
specHandle-[in]控制器句柄。
返回值：1-选择成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetMeasureType(int &type,int &tkCount, int specHandle, int channel = 1);

/*
函数功能：[只能在控制器工作模式为1时调用]获取指定通道最后pts个测量结果，其中有多种附加类型数据可按
需要选择是否获取，数据类型包含高度测量结果、感光度、峰像素位置、波形曲线数据，波峰强度，实际调用时
函数内部会根据给定参数指针是否为空指针来判断是否需要保存该数据到调用方给定的内存中，如果采集的是多
通道的结果，则数据存储方式为第一个通道储存在0~pts-1内存中,第二个通道存储在pts~2*pts-1内存中注意此
函数不会对传入的数组进行清零初始化操作。
参数：
pRstHigh-[out]保存采集到的测量结果数据数组，数组长度为pts。
pts-[in]期望采集的测量结果数量。
specHandle-[in]控制器句柄。
channel-[in]通道编号
pUnitRa-[out]感光度数组，数组长度为pts，如果不需要获取此项数据则此参数传入0或空指针。
pPeakWave-[out]峰像素位置数组，数组长度为pts，如果不需要获取此项数据则此参数传入0或空指针。
pPrdItn-[out]能量曲线数据数组，数组长度为pts*length（length代表单帧能量曲线的长度，可以通
过LE_GetChannelIntensityMsg函数获得），如果不需要获取此项数据则此参数传入0或空指针。
pPeakItn-[out]保存每个高度数据的波峰位置的光强度，如果高度数据为有效值时则记录其波峰位置的光强度
，如果高度数据为无效数据时则记录该光谱曲线的最高光强度作为此数据波峰光强度，数组长度为pts
如果不需要此功能则此参数传入0或空指针。
pEncoder1-[out]保存锁存的轴0编码器的位置（仅嵌入式控制器支持）。
pEncoder2-[out]保存锁存的轴1编码器的位置（仅嵌入式控制器支持）。
pEncoder3-[out]保存锁存的轴2编码器的位置（仅嵌入式控制器支持）。
pEncoder4-[out]保存锁存的轴3编码器的位置（仅嵌入式控制器支持）。
pEncoder5-[out]保存锁存的轴4编码器的位置（仅嵌入式控制器支持）。
pEncoder6-[out]保存锁存的轴5编码器的位置（仅嵌入式控制器支持）。
data0-[in]预留参数，默认为0，目前不起作用。
返回值：0-未采集到数据，大于零时表示实际采集到的数据数量，负值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetLastDistances(double *pRstHigh, int pts, int specHandle, int channel = 1, float *pUnitRa = 0, float *pPeakWave = 0,
	int *pPrdItn = 0, int *pPeakItn = 0, float *pEncoder1 = 0, float *pEncoder2 = 0, float *pEncoder3 = 0, float*pEncoder4 = 0, float*pEncoder5 = 0,
	float*pEncoder6 = 0,int data0 = 0);

/*
函数功能：[只能在控制器工作模式为1时调用]获取指定通道最后pts个测量结果，其中包含多层（目前版
本只支持一层）厚度数据、多层上下表面高度数据、感光度、多层上下表面波峰像素位置数据、波形曲线
数据，具体需采集哪些数据由给定的指针或数组首地址是否等于0或为空来决定，如果采集的是多通道的结
果，则厚度结果数据存储方式为第一个通道储存在0~（pts*tkCount）-1内存中,第二个通道存储在
pts*tkCount~（2*pts*tkCount）-1内存中，其他类型数据存储方式请参考“光谱共焦传感器SDK使用说明”
，注意此函数不会对传入的数组进行清零初始化操作。
参数：
pRstTK-[out]保存采集到的厚度数据数组，长度等于pts*tkCount。
pts-[in]期望获取厚度数量，注意一次采集多层结果仅表示一个数量。
tkCount-[in]单帧能量数据中期望获取的层数，最小为1层，目前最大为5层。
specHandle-[in]控制器句柄。
channel-[in]通道编号。
pDistance1-[out]保存采集到的上表面高度数组，长度等于pts*tkCount，如果不需要获取此项数据
则此参数传入0或空指针。
pDistance2-[out]保存采集到的下表面高度数组，长度等于pts*tkCount，如果不需要获取此项数据
则此参数传入0或空指针,注意pDistance1和pDistance1要么2个都获取，要么都不获取。
pUnitRa-[out]感光度数组，长度等于pts*channels，如果不需要获取此项数据则此参数传入0或空指针。
pPeakWave1-[out]保存上表面波峰像素位置数组，长度等于pts*tkCount，如果不需要获取此项数据
则此参数传入0或空指针。
pPeakWave2-[out]保存下表面波峰像素位置数组，长度等于pts*tkCount，如果不需要获取此项数据
则此参数传入0或空指针,注意pPeakWave1和pPeakWave2要么2个都获取，要么都不获取。
pPrdItn-[out]pts个点波形曲线数据数组,数组长度为pts*length（length代表单帧光谱的长度，可
以通过LE_GetChannelIntensityMsg函数获得），如果不需要获取此项数据则此参数传入0或空指针。
pPeakItn1-[out]保存上表面波峰光强值，功能暂未实现。
pPeakItn2-[out]保存下表面波峰光强值，功能暂未实现。
pEncoder1-[out]保存锁存的轴0编码器的位置（仅嵌入式控制器支持）。
pEncoder2-[out]保存锁存的轴1编码器的位置（仅嵌入式控制器支持）。
pEncoder3-[out]保存锁存的轴2编码器的位置（仅嵌入式控制器支持）。
pEncoder4-[out]保存锁存的轴3编码器的位置（仅嵌入式控制器支持）。
pEncoder5-[out]保存锁存的轴4编码器的位置（仅嵌入式控制器支持）。
pEncoder6-[out]保存锁存的轴5编码器的位置（仅嵌入式控制器支持）。
data0-[out]预留。
返回值：0-未采集到数据，大于零时表示实际采集到的数据数量，负值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetLastThicknesses(double *pRstTK, int pts, int tkCount, int specHandle, int channel = 1, double *pDistance1 = 0,
	double *pDistance2 = 0, float *pUnitRa = 0, float *pPeakWave1 = 0, float *pPeakWave2 = 0, int *pPrdItn = 0, int *pPeakItn1 = 0, int *pPeakItn2 = 0,
	float *pEncoder1 = 0, float *pEncoder2 = 0,float *pEncoder3 = 0, float*pEncoder4 = 0, float*pEncoder5 = 0, float*pEncoder6 = 0, int data0 = 0);

/*
函数功能：开始存储测量结果到控制器缓冲区。
参数：
specHandle-[in]控制器句柄。
返回值：1-开始成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_StartStoreDataToBuffer(int specHandle);

/*
函数功能：[只能在控制器工作模式为1时调用]获取指定通道缓冲区内pts个测量结果，按先进先出的原则获取，
其中有多种附加类型数据可按需要选择是否获取，数据类型包含高度测量结果、感光度、峰像素位置、波形曲
线数据，波峰强度，实际调用时函数内部会根据给定参数指针是否为空指针来判断是否需要保存该数据到调用
方给定的内存中，如果采集的是多通道的结果，则数据存储方式为第一个通道储存在0~pts-1内存中,第二个通
道存储在pts~2*pts-1内存中注意此
函数不会对传入的数组进行清零初始化操作。
参数：
pRstHigh-[out]保存采集到的测量结果数据数组，数组长度为pts。
pts-[in]期望采集的测量结果数量。
specHandle-[in]控制器句柄。
channel-[in]通道编号
pUnitRa-[out]感光度数组，数组长度为pts，如果不需要获取此项数据则此参数传入0或空指针。
pPeakWave-[out]峰像素位置数组，数组长度为pts，如果不需要获取此项数据则此参数传入0或空指针。
pPrdItn-[out]保存波形曲线数据数组,数组长度为pts*length（length代表单帧光谱的长度，可
	    以通过LE_GetChannelIntensityMsg函数获得），如果不需要获取此项数据则此参数传入0或空指针。
pPeakItn-[out]保存每个高度数据的波峰位置的光强度，如果高度数据为有效值时则记录其波峰位置的光强度
，如果高度数据为无效数据时则记录该光谱曲线的最高光强度作为此数据波峰光强度，数组长度为pts
如果不需要此功能则此参数传入0或空指针。
pEncoder1-[out]保存锁存的轴0编码器的位置。
pEncoder2-[out]保存锁存的轴1编码器的位置。
pEncoder3-[out]保存锁存的轴2编码器的位置。
pEncoder4-[out]保存锁存的轴3编码器的位置。
pEncoder5-[out]保存锁存的轴4编码器的位置（仅E系列、G系列控制器支持）。
pEncoder6-[out]保存锁存的轴5编码器的位置（仅E系列控制器支持）。
data0-[in]预留。
返回值：0-未采集到数据，大于零时表示实际采集到的数据数量，负值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetStoredDistances(double *pRstHigh, int pts, int specHandle, int channel = 1, float *pUnitRa = 0, float *pPeakWave = 0,
	int *pPrdItn = 0, int *pPeakItn = 0, float *pEncoder1 = 0, float *pEncoder2 = 0, float *pEncoder3 = 0, float*pEncoder4 = 0, float*pEncoder5 = 0,
	float*pEncoder6 = 0, int data0 = 0);

/*
函数功能：[只能在控制器工作模式为1时调用]获取指定通道缓冲区内pts个测量结果，按先进先出的原则获取，
其中包含多层厚度数据、多层上下表面高度数据、感光度、多层上下表面波峰像素位置
数据、波形曲线数据，具体需采集哪些数据由给定的指针或数组首地址是否等于0或为空来决定，厚度结果数据
pRstTK存储方式为第一层储存在0~pts-1内存中,第二层存储在pts~（2*pts-1)内存中，pDistance1、pDistance2、
pPeakWave1、pPeakWave2数据存储格式与pRstTK相同，注意此函数不会对传入的数组进行清零初始化操作。
参数：
pRstTK-[out]保存采集到的厚度数据数组，长度等于pts*tkCount。
pts-[in]期望获取厚度数量，注意一次采集多层结果仅表示一个数量。
tkCount-[in]单帧能量数据中期望获取的层数，目前固定为1层。
specHandle-[in]控制器句柄。
channel-[in]通道编号。
pDistance1-[out]保存采集到的上表面高度数组，长度等于pts*tkCount，如果不需要获取此项数据
则此参数传入0或空指针。
pDistance2-[out]保存采集到的下表面高度数组，长度等于pts*tkCount，如果不需要获取此项数据
则此参数传入0或空指针,注意pDistance1和pDistance1要么2个都获取，要么都不获取。
pUnitRa-[out]感光度数组，长度等于pts*channels，如果不需要获取此项数据则此参数传入0或空指针。
pPeakWave1-[out]保存上表面波峰像素位置数组，长度等于pts*tkCount，如果不需要获取此项数据
则此参数传入0或空指针。
pPeakWave2-[out]保存下表面波峰像素位置数组，长度等于pts*tkCount，如果不需要获取此项数据
则此参数传入0或空指针,注意pPeakWave1和pPeakWave2要么2个都获取，要么都不获取。
pPrdItn-[out]预留
pPeakItn1-[out]保存上表面波峰光强值，长度等于pts*tkCount，如果不需要获取此项数据
则此参数传入0或空指针,注意pPeakWave1和pPeakWave2要么2个都获取，要么都不获取。
pPeakItn2-[out]保存下表面波峰光强值，长度等于pts*tkCount，如果不需要获取此项数据
则此参数传入0或空指针,注意pPeakWave1和pPeakWave2要么2个都获取，要么都不获取。
pEncoder1-[out]保存锁存的轴0编码器的位置。
pEncoder2-[out]保存锁存的轴1编码器的位置。
pEncoder3-[out]保存锁存的轴2编码器的位置。
pEncoder4-[out]保存锁存的轴3编码器的位置。
pEncoder5-[out]保存锁存的轴4编码器的位置（仅E系列、G系列控制器支持）。
pEncoder6-[out]保存锁存的轴5编码器的位置（仅E系列控制器支持）。
data0-[out]预留。
返回值：0-未采集到数据，大于零时表示实际采集到的数据数量，负值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetStoredThicknesses(double *pRstTK, int pts, int tkCount, int specHandle, int channel = 1, double *pDistance1 = 0,
	double *pDistance2 = 0, float *pUnitRa = 0, float *pPeakWave1 = 0, float *pPeakWave2 = 0, int *pPrdItn = 0, int *pPeakItn1 = 0, int *pPeakItn2 = 0,
	float *pEncoder1 = 0, float *pEncoder2 = 0, float *pEncoder3 = 0, float*pEncoder4 = 0, float*pEncoder5 = 0, float*pEncoder6 = 0, int data0 = 0);

/*
函数功能：通过密码来登入用户权限，目前仅嵌入式控制器支持该功能。
参数：
pwd-[in]密码，最小为6字节，最大为128字节，支持数字字母,注意登陆普通用户可以不输入密码。
len-[in]pwd数组或指针的长度。
userGlass-[in]待登陆用户权限1-普通用户；2-预留用户；3-工程师；4-管理员；5-超级管理员。
specHandle-[in]控制器句柄。
返回值：1-登陆成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_Login(unsigned char *pwd, int len, int userGlass, int specHandle);

/*
函数功能：查询当前用户权限。
参数：
specHandle-[in]控制器句柄。
返回值：,1-普通用户；2-预留用户；3-工程师；4-管理员；5-超级管理员。
*/
CONFOCAL_DLL int __stdcall LE_QueryUserType(int specHandle);

/*
函数功能：修改用户密码,较高权限用户可以修改同级或较低权限用户的密码。
参数：
newPwd-[in]待修改密码，最小为6字节，最大为128字节。
len-[in]pwd数组或指针的长度。
userGlass-[in]待修改用户权限1-普通用户；2-预留用户；3-工程师；4-管理员；5-超级管理员。
specHandle-[in]控制器句柄。
返回值：1-修改成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_ChangePassword(unsigned char *newPwd, int len, int userGlass, int specHandle);

/*
函数功能：获取编码器分频触发参数。
参数：
sta-[out]保存获取到的触发信号输出状态，0-禁用触发信号输出，1-启用触发信号输出。
axis-[out]保存获取到的轴号。
dir-[out]保存获取到的触发方向， 0：正向；1：负向；2-双向。
step-[out]保存获取到的分频间隔，单位pulse。
specHandle-[in]控制器句柄。
返回值：1-操作成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_FQ_GetEncoderFrequencyParam(unsigned char &sta, unsigned char &axis, unsigned char &dir, int &step, int specHandle);

/*
函数功能：设置编码器分频触发参数，[此函数执行后会重置指定轴的
分频触发开始位置为轴当前机械坐标位置]。
参数：
sta-[in]0-禁用触发信号输出，1-启用触发信号输出。
axis-[in]轴号，G系列控制器可设范围0~4；E系列控制器G系列控制器可选范围0~4；E系列控制器可选范围0~5；分频触发卡可选范围0~3；分频触发卡可设范围0~3。
dir-[in]触发方向，0：正向；1：负向；2-双向。
step-[in]分频间隔，单位pulse。
specHandle-[in]控制器句柄。
返回值：1-操作成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_FQ_SetEncoderFrequencyParam(unsigned char sta, unsigned char axis, unsigned char dir, int step, int specHandle);

/*
函数功能：获取编码器输出触发信号是否使能，同时获取当前触发轴号。
参数：
sta-[out]保存获取到的触发信号输出状态。
axis-[out]保存获取到的当前触发轴号。
specHandle-[in]控制器句柄。
返回值：1-获取成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_FQ_GetOutStatusAndAxis(unsigned char &sta, unsigned char &axis, int specHandle);

/*
函数功能：设置编码器输出触发信号是否使能，同时设置当前触发轴号。
参数：
sta-[in]0-禁用触发信号输出，1-启用触发信号输出。
axis-[in]当前触发轴号，G系列控制器可设范围0~4；E系列控制器G系列控制器可选范围0~4；E系列控制器可选范围0~5；分频触发卡可选范围0~3；分频触发卡可设范围0~3。
specHandle-[in]控制器句柄。
返回值：1-设置成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_FQ_SetOutStatusAndAxis(unsigned char sta, unsigned char axis, int specHandle);

/*
函数功能：读取某个轴的编码器位置值，单位脉冲。
参数：
axis-[in]轴号，G系列控制器可选范围0~4；E系列控制器可选范围0~5；分频触发卡可选范围0~3。
specHandle-[in]控制器句柄。
返回值：获取到的编码器位置值。
*/
CONFOCAL_DLL int __stdcall LE_FQ_GetABCount(unsigned char axis, int specHandle);

/*
函数功能：设置某个轴的编码器位置值，单位脉冲。
参数：
axis-[in]轴号，G系列控制器可选范围0~4；E系列控制器可选范围0~5；分频触发卡可选范围0~3。
count-[in]待设置的位置，单位为脉冲。
specHandle-[in]控制器句柄。
返回值：1-操作成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_FQ_SetABCount(unsigned char axis, int count, int specHandle);

/*
函数功能：读取某个轴的Z相计数值，单位脉冲。
参数：
axis-[in]轴号，G系列控制器可选范围0~4；E系列控制器可选范围0~5；分频触发卡可选范围0~3。
specHandle-[in]控制器句柄。
返回值：获取到的Z相计数值。
*/
CONFOCAL_DLL int __stdcall LE_FQ_GetZCount(unsigned char axis, int specHandle);

/*
函数功能：读取某个轴的编码器分频触发预设参数。
参数：
FPP-[out]保存获取到的预设参数。
axis-[in]轴号，G系列控制器可选范围0~4；E系列控制器可选范围0~5；分频触发卡可选范围0~3。
specHandle-[in]控制器句柄。
返回值：1-操作成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_FQ_GetPresetParam(FPresetParam &FPP, unsigned char axis, int specHandle);

/*
函数功能：设置某个轴的编码器分频触发预设参数。
参数：
FPP-[in]保存获取到的预设参数。
axis-[in]轴号，G系列控制器可选范围0~4；E系列控制器可选范围0~5；分频触发卡可选范围0~3。
specHandle-[in]控制器句柄。
返回值：1-操作成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_FQ_SetPresetParam(FPresetParam &FPP, unsigned char axis, int specHandle);

/*
函数功能：获取某个轴的编码器分频触发常用参数。
参数：
FNP-[in]保存获取到的常用参数。
axis-[in]轴号，G系列控制器可选范围0~4；E系列控制器可选范围0~5；分频触发卡可选范围0~3。
specHandle-[in]控制器句柄。
返回值：1-操作成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_FQ_GetNormalParam(FNormalParam &FNP, unsigned char axis, int specHandle);

/*
函数功能：设置某个轴的编码器分频触发常用参数。
参数：
FNP-[in]待设置的常用参数。
axis-[in]轴号，G系列控制器可选范围0~4；E系列控制器可选范围0~5；分频触发卡可选范围0~3。
specHandle-[in]控制器句柄。
返回值：1-操作成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_FQ_SetNormalParam(FNormalParam &FNP, unsigned char axis, int specHandle);

/*
函数功能：重置某个轴的编码器分频触发开始位置为轴当前位置。
参数：
axis-[in]轴号，G系列控制器可选范围0~4；E系列控制器可选范围0~5；分频触发卡可选范围0~3。
specHandle-[in]控制器句柄。
返回值：1-操作成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_FQ_Reset(unsigned char axis, int specHandle);

/*
函数功能：[仅支持E、G型控制器]设置某个轴的编码器第一个负向分频间隔触发偏移值，此值不能大于分频系数，单位脉冲。
参数：
axis-[in]轴号，G系列控制器可选范围0~4；E系列控制器可选范围0~5；分频触发卡可选范围0~3。
offset-[in]待设置的位置，单位为脉冲。
specHandle-[in]控制器句柄。
返回值：1-操作成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_FQ_SetEncoderOffset(unsigned char axis, int offset, int specHandle);

/*
函数功能：[仅支持E、G型控制器]读取某个轴的编码器第一个负向分频间隔触发偏移值，单位脉冲。
参数：
axis-[in]轴号，G系列控制器可选范围0~4；E系列控制器可选范围0~5；分频触发卡可选范围0~3。
specHandle-[in]控制器句柄。
返回值：获取到的负向分频间隔触发偏移值。
*/
CONFOCAL_DLL int __stdcall LE_FQ_GetEncoderOffset(unsigned char axis, int specHandle);

/*******************************************************************************************************/
/*
函数功能：开关控制器指定通道的光源。
参数：
isOn-[in]true-打开光源，false-关闭光源。
channel-[in]通道编号。
specHandle-[in]控制器句柄。
返回值：1-操作成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_TurnOnLight(bool isOn, int specHandle, int channel = 1);

/*
函数功能：获取控制器指定通道的光源状态。
参数：
isOn-[out]true-光源已打开，false-光源已关闭。
channel-[in]通道编号。
specHandle-[in]控制器句柄。
返回值：1-操作成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetLightStatus(bool &isOn, int specHandle, int channel = 1);

/*
函数功能：[仅USB2代控制器支持此功能]设置GPIO输出信号模式。
参数：
val-[in]待设置的GPIO输出模式，0-闪光灯模式，1-普通模式（此模式下可通过软件指令
LE_SetConfocalGPIOOutStatus控制out信号状态）。
specHandle-[in]控制器句柄。
返回值：1-操作成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetConfocalGPIOOutMode(unsigned char val, int specHandle);

/*
函数功能：[仅USB2代控制器支持此功能]获取GPIO输出信号模式。
参数：
val-[out]保存获取到的GPIO输出模式，0-闪光灯模式，1-普通模式（此模式下可通过软件指令
LE_SetConfocalGPIOOutStatus控制out信号状态）。
specHandle-[in]控制器句柄。
返回值：1-操作成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetConfocalGPIOOutMode(unsigned char &val, int specHandle);

/*
函数功能：[仅USB2代控制器支持此功能]设置GPIO输出信号状态。
参数：
state-[in]待设置的GPIO输出状态，0-断开，1-闭合。
specHandle-[in]控制器句柄。
返回值：1-操作成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetConfocalGPIOOutStatus(unsigned char state, int specHandle);

/*
函数功能：[仅USB2代控制器支持此功能]获取GPIO输出信号状态。
参数：
state-[out]保存获取到的GPIO输出状态，0-断开，1-闭合。
specHandle-[in]控制器句柄。
返回值：1-操作成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetConfocalGPIOOutStatus(unsigned char &state, int specHandle);

/*
函数功能：设置一个触发信号采集的数据量。
参数：
state-[in]待设置的值，可设范围1~10000。
specHandle-[in]控制器句柄。
返回值：1-操作成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetMultiplicationFrequency(unsigned short count, int specHandle);

/*
函数功能：获取一个触发信号采集的数据量。
参数：
state-[out]保存获取到的值。
specHandle-[in]控制器句柄。
返回值：1-操作成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetMultiplicationFrequency(unsigned short &count, int specHandle);

/*
函数功能：将给定波峰像素位置数组转换为位移数据数组。
参数：
peakPoses-[in]波峰像素位置数组。
pDistance-[out]换算出的位移数据数组。
count-[in]peakPoses及pDistance的长度。
specHandle-[in]控制器句柄。
channel-[in]通道编号。
返回值：1-操作成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_ConvertPeakPosToDistance(float *peakPoses, double *pDistance, int count, int specHandle, int channel);

/*
函数功能：启用或禁用在线获取数据线程。
参数：
isEnable-[in]true-启用；false-禁用。
specHandle-[in]控制器句柄。
返回值：1-操作成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_EnableOnlineThread(bool isEnable, int specHandle);

/*
函数功能：获取在线获取数据线程的状态。
参数：
isEnable-[out]保存获取到的值。
specHandle-[in]控制器句柄。
返回值：1-操作成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetOnlineThread(bool &isEnable, int specHandle);

/*
函数功能：在线校准指定组合对射测厚探头的厚度值。
参数：
idx-[in]期望校准的探头组合的序号。
isTurnOnLight-[in]是否交替开关通道光源。
specHandle-[in]控制器句柄。
返回值：1-操作成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_CaliOppositeThickness(unsigned short idx, bool isTurnOnLight, int specHandle);

/*
函数功能：获取指定序号的探头组合参数。
参数：
dtkCali-[in]保存获取到的探头组合参数。
idx-[in]待获取探头组合参数的序号，参数范围0~3。
specHandle-[in]控制器句柄。
返回值：1-操作成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetOppositeThicknessParam(ThicknessCalibration &dtkCali, unsigned short idx, int specHandle);
/*
函数功能：设置指定序号的探头组合参数。
参数：
dtkCali-[in]待设置的探头组合参数。
idx-[in]待设置探头组合参数的序号，参数范围0~3。
specHandle-[in]控制器句柄。
返回值：1-操作成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetOppositeThicknessParam(ThicknessCalibration dtkCali, unsigned short idx, int specHandle);

/*
函数功能：同时获取一个控制器多组对射测厚探头的厚度值，注意此函数执行一次，每个组合仅输出一组厚度、上表面、下表面的结果。
参数：
pDTkVal-[out]保存对射测厚厚度值数组，长度需大于等于combinations，单位μm。
combinations-[in]期望获取的对射探头组合数量。
pDT1-[out]保存探头1的高度偏移值(相对于对射测厚校准时记录的探头1高度值)数组，长度需大于等于combinations，单位μm。
pDT2-[out]保存探头2的高度偏移值(相对于对射测厚校准时记录的探头2高度值)数组，长度需大于等于combinations，单位μm。
avgCount-[i]每个通道单次数据采集结果平均次数。
turnOnLightCount-[i]采集数据过程中，光源交替开关次数，最终的平均次数 = turnOnLightCount * avgCount。
isTurnOnLight-[in]是否交替开关通道光源，true：交替开关；false：不交替开关。
specHandle-[in]控制器句柄。
pPrdItn-[out]保存combinations个组合的光谱曲线数组，数组长度应大于等于“单帧光谱曲线长度*2*combinations”，如果不需
要获取此项数据则此参数传入0或空指针。
pUnitRa-[out]感光度数组，数组长度应大于等于“2*combinations”，如果不需要获取此项数据则此参数传入0或空指针。
pPeakWave-[out]峰像素位置数组，数组长度应大于等于“2*combinations”，如果不需要获取此项数据则此参数传入0或空指针。
pPeakItn-[out]保存每个组合中每个通道的波峰位置的光强度，如果高度数据为有效值时则记录其波峰位置的光强度
，如果高度数据为无效数据时则记录该光谱曲线的最高光强度作为此数据波峰光强度，数组长度应大于等于“2*combinations”
如果不需要此功能则此参数传入0或空指针。
返回值：1-操作成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetOppositeThicknessValue(double* pDTkVal, unsigned short combinations, unsigned short avgCount, unsigned short turnOnLightCount
	, bool isTurnOnLight, int specHandle, double* pDT1 = 0, double* pDT2 = 0, int *pPrdItn = 0, float* pUnitRa = 0, float *pPeakWave = 0, int *pPeakItn = 0);

/*
函数功能：获取控制器指定通道出厂高度量程的上边界跟下边界，单位为μm。
参数：
dRangeTop-[out]保存获取到的量程上边界。
dRangeBottom-[out]保存获取到的量程下边界。
specHandle-[in]控制器句柄。
channel-[in]通道编号，当前默认为1。
返回值：1-获取成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetDistanceRangeLimit(float &dRangeTop, float &dRangeBottom, int specHandle, int channel = 1);
/*
函数功能：[仅支持E、G型控制器,且需要超级管理员权限]设置控制器指定通道出厂高度量程的上边界跟下边界，单位为μm。
参数：
dRangeTop-[out]保存获取到的量程上边界。
dRangeBottom-[out]保存获取到的量程下边界。
specHandle-[in]控制器句柄。
channel-[in]通道编号，当前默认为1。
返回值：1-获取成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetDistanceRangeLimit(float dRangeTop, float dRangeBottom, int specHandle, int channel = 1);

/*
函数功能：[仅支持E、G型控制器]读取高度读数零点位置值。
参数：
value-[out]保存读取到的的高度读数零点位置值，单位μm。
specHandle-[in]控制器句柄。
channel-[in]通道编号，当前默认为1。
返回值：1-获取成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetDistanceZeroValue(float &value, int specHandle, int channel = 1);
/*
函数功能：[仅支持E、G型控制器]设置高度读数零点位置值，例如设置值为40μm，内部计算的原始高度读数为100μm，则实际输出的高度读数为60μm。
参数：
value-[out]待设置的高度读数零点位置值，单位μm。
specHandle-[in]控制器句柄。
channel-[in]通道编号，当前默认为1。
返回值：1-获取成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetDistanceZeroValue(float value, int specHandle, int channel = 1);

/*
函数功能：[仅支持E、G型控制器]读取高度量程使用范围最大值。
参数：
value-[out]保存读取到的量程使用范围最大值，单位μm。
specHandle-[in]控制器句柄。
channel-[in]通道编号，当前默认为1。
返回值：1-获取成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetVoltageRangeMaxValue(float& value, int specHandle, int channel = 1);
/*
函数功能：[仅支持E、G型控制器]设置高度量程使用范围最大值。
参数：
value-[out]待设置的量程使用范围最大值，单位μm。
specHandle-[in]控制器句柄。
channel-[in]通道编号，当前默认为1。
返回值：1-获取成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetVoltageRangeMaxValue(float value, int specHandle, int channel = 1);

/*
函数功能：[仅支持E、G型控制器]读取高度量程使用范围最小值。
参数：
value-[out]保存读取到的量程使用范围最小值，单位μm。
specHandle-[in]控制器句柄。
channel-[in]通道编号，当前默认为1。
返回值：1-获取成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetVoltageRangeMinValue(float& value, int specHandle, int channel = 1);
/*
函数功能：[仅支持E、G型控制器]设置高度量程使用范围最小值。
参数：
value-[out]待设置的量程使用范围最小值，单位μm。
specHandle-[in]控制器句柄。
channel-[in]通道编号，当前默认为1。
返回值：1-设置成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetVoltageRangeMinValue(float value, int specHandle, int channel = 1);


/*
函数功能：[仅支持E、G型控制器]读取通道modbus通讯参数。
参数：
ModbusTcpParam-[out]保存读取到的modbus通讯参数结构体。
specHandle-[in]控制器句柄。
channel-[in]通道编号，当前默认为1。
返回值：1-获取成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetModbusTcpParam(ModbusTcpParam& MTParam, int specHandle, int channel = 1);
/*
函数功能：[仅支持E、G型控制器]设置通道modbus通讯参数。
参数：
ModbusTcpParam-[out]待设置的modbus通讯参数结构体。
specHandle-[in]控制器句柄。
channel-[in]通道编号，当前默认为1。
返回值：1-设置成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetModbusTcpParam(ModbusTcpParam& MTParam, int specHandle, int channel = 1);
/*
函数功能：保存厚度比例系数文件。
参数：
pTkUpperVal-[in]厚度值上限，单位μm。
pTkLowerVal-[in]厚度值下限，单位μm。
pK-[in]厚度值斜率。  
pOffset-[in]厚度值偏移值，单位μm。
specHandle-[in]控制器句柄。
channel-[in]通道编号，当前默认为1。
返回值：1-设置成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetThicknessCalibrationData(float* pTkUpperVal, float* pTkLowerVal, float* pK, float* pOffset, int pCount, int specHandle, int channel = 1);
/*
函数功能：获取厚度比例系数文件数据个数。
参数：
specHandle-[in]控制器句柄。
channel-[in]通道编号，当前默认为1。
返回值：厚度比例系数文件数据个数。
*/
CONFOCAL_DLL int __stdcall LE_GetThicknessCalibrationDataCount(int specHandle, int channel = 1);
/*
函数功能：获取厚度比例系数文件。
参数：
pTkUpperVal-[in]厚度值上限，单位μm。
pTkLowerVal-[in]厚度值下限，单位μm。
pK-[in]厚度值斜率。
pOffset-[in]厚度值偏移值，单位μm。
count-[in]获取的校准数据点个数。
specHandle-[in]控制器句柄。
channel-[in]通道编号，当前默认为1。
返回值：实际获取到的数量。
*/
CONFOCAL_DLL int __stdcall LE_GetThicknessCalibrationData(float* pTkUpperVal, float* pTkLowerVal, float* pK, float* pOffset, int count, int specHandle, int channel = 1);

/*
函数功能：获取指定通道包含的高度校准数据数量。
参数：
specHandle-[in]控制器句柄。
channel-[in]通道编号。
返回值：实际获取到的数量。
*/
CONFOCAL_DLL int __stdcall LE_GetAllCalibrationSnListCount(int specHandle, int channel = 1);

/*
函数功能：获取指定通道包含的所有高度校准数据的“探头型号及SN”，每一个“探头型号及SN”固定
占用32个数组元素。
参数：
distanceCalibrationSNList-[out]保存获取到的“探头型号及SN”的数组，长度需大于等于count*32。
count-[in]最大可保存的“探头型号及SN”数量。
specHandle-[in]控制器句柄。
channel-[in]通道编号。
返回值：实际获取到的“探头型号及SN”数量。
*/
CONFOCAL_DLL int __stdcall LE_GetAllCalibrationSnList(unsigned char* distanceCalibrationSNList, int count, int specHandle, int channel = 1);

/*
函数功能：切换控制器内部当前使用的移校校准文件编号。
参数：
calibNum-[in]期望切换的文件编号。
specHandle-[in]控制器句柄。
channel-[in]通道编号。
返回值：1-设置成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SelectCalibrationOfNum(int calibNum, int specHandle, int channel = 1);

/*
函数功能：获取通道当前使用的高度校准数据的“探头型号及SN”。
参数：
distanceCalibrationSN-[out]保存获取到的“探头型号及SN”。
specHandle-[in]控制器句柄。
channel-[in]通道编号。
返回值：1-获取成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetCurCalibrationSnName(unsigned char* distanceCalibrationSN, int specHandle, int channel = 1);

/*
函数功能：设置unsigned short型参数。
参数：
paramIdx-[in]待设置参数序号。
pValue-[in]待设置参数数组。
count-[in]pValue数组长度。
specHandle-[in]控制器句柄。
channel-[in]通道序号或IO端口序号。
返回值：1-设置成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetParamsUnsignedShort(unsigned int paramIdx, unsigned short *pValue, int count, int specHandle, int channel = 1);
/*
函数功能：读取unsigned short型参数。
参数：
paramIdx-[in]待获取参数序号。
pValue-[in]保存获取到的参数数组。
count-[in]pValue数组长度，也是期望或获取的数据数量。
specHandle-[in]控制器句柄。
channel-[in]通道序号或IO端口序号。
返回值：1-获取成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetParamsUnsignedShort(unsigned int paramIdx, unsigned short *pValue, int count, int specHandle, int channel = 1);

/*
函数功能：设置unsigned int型参数。
参数：同LE_SetParamsUnsignedShort参数说明。
返回值：1-设置成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetParamsUnsignedInt(unsigned int paramIdx, unsigned int *pValue, int count, int specHandle, int channel = 1);
/*
函数功能：读取unsigned int型参数。
参数：同LE_GetParamsUnsignedShort参数说明。
返回值：1-获取成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetParamsUnsignedInt(unsigned int paramIdx, unsigned int *pValue, int count, int specHandle, int channel = 1);


/*
函数功能：设置unsigned int型参数。
参数：同LE_SetParamsUnsignedShort参数说明。
返回值：1-设置成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetParamsInt(unsigned int paramIdx, int *pValue, int count, int specHandle, int channel = 1);
/*
函数功能：读取unsigned int型参数。
参数：同LE_GetParamsUnsignedShort参数说明。
返回值：1-获取成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetParamsInt(unsigned int paramIdx, int *pValue, int count, int specHandle, int channel = 1);


/*
函数功能：设置unsigned char型参数。
参数：同LE_SetParamsUnsignedShort参数说明。
返回值：1-设置成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetParamsUnsignedChar(unsigned int paramIdx, unsigned char *pValue, int count, int specHandle, int channel = 1);
/*
函数功能：读取unsigned char型参数。
参数：同LE_GetParamsUnsignedShort参数说明。
返回值：1-获取成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetParamsUnsignedChar(unsigned int paramIdx, unsigned char *pValue, int count, int specHandle, int channel = 1);

/*
函数功能：设置double型参数。
参数：同LE_SetParamsUnsignedShort参数说明。
返回值：1-设置成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetParamsDouble(unsigned int paramIdx, double *pValue, int count, int specHandle, int channel = 1);
/*
函数功能：读取double型参数。
参数：同LE_GetParamsUnsignedShort参数说明。
返回值：1-获取成功，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetParamsDouble(unsigned int paramIdx, double *pValue, int count, int specHandle, int channel = 1);
/*
函数功能：执行指定的指令操作，区分于参数读写命令。
参数：
specHandle-[in]控制器句柄。
channel-[in]通道编号。
cmd-[in]待执行指令类型。
data0-[in]预留参数。
返回值：执行成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_Command(int specHandle, int channel, unsigned int  cmd, void* data0=0);
/*******************************************************************************************************/