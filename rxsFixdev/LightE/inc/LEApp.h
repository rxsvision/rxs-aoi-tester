#pragma once
#ifdef CONFOCAL_DLL
#else
#define CONFOCAL_DLL extern "C" _declspec(dllimport)
#endif
#include"LEDataType.h"
/*
函数功能：滤波函数，用户给定数据，函数执行完后可获得处理后的数据，并且支持过滤给定有效高度范围外
的值不做处理。
参数：
pData-[in,out]待处理数据指针或数组。
count-[in]pData的长度。
filterParam-[in]滤波参数。
RanageLow-[in]有效数据的下限。
RanageHigh-[in]有效数据的上限。
返回值：1-执行成功；其他返回值请参照返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_FilterEx(double *pData, int count, FilterParam filterParam, double RanageLow, double RanageHigh);

/*
函数功能：使用给定高度阈值进行去极值处理,极值数据默认使用拟合填充。
参数：
pRscH-[in,out]需进行去极值的源数据。
count-[in]pRscH的数据长度。
EDParam-[in]去极值参数结构体。
RanageLow-[in]有效数据的下边界。
RanageHeight-[in]有效数据的上边界。
exportFstDrv-[out]去极值计算出的一阶导数，算法调试用。
exportSecDrv-[in]去极值计算出的二阶导数，算法调试用。
返回值：成功返回1,其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_ExtremeDataProcessEx(double *pRscH, int count, ExtremeDataParam EDParam, double RanageLow, double RanageHeight, double *exportFstDrv = 0, double *exportSecDrv = 0);

/*
函数功能：一段轨迹内使用自动寻找算法计算n点胶高。
参数：
pRscData-[in,out]原始高度数据数组,注意计算完胶高后，对数据的滤波去极值等处理会作用在此输入数据上。
count-[in]原始高度数据数组长度。
PRVal1-[out]保存计算出来的左边胶水厚度及其他相关信息。
PRVal2-[out]保存计算出来的右边胶水厚度及其他相关信息。
GCParam-[in]计算使用参数结构体。
glueCount-[in]希望计算的胶水厚度数量，默认为2，当扫描出的高度截面曲线包含2个点的胶高时，此参数输
入2，当扫描出的高度截面曲线包含1个点的胶高时，此参数输入1，此参数会决定DLL内部所调用的算法，请根
据实际情况设置，否则可能会导致计算结果不准。
返回值：返回已找到的胶水厚度数量。
*/
CONFOCAL_DLL int __stdcall LE_RoundHoleGlueCalc(double *pRscData, int count, ProductResult &PRVal1, ProductResult &PRVal2, GlueCalcParam &GCParam, int glueCount = 2);
/*
函数功能：无效数据拟合函数。
参数：
pRscData-[in,out]需进行拟合的源数据。
count-[in]pRscData的数据长度。
RanageLow-[in]有效数据的下边界。
RanageHeight-[in]有效数据的上边界。
IDFParam-[in]数据拟合参数结构体。
返回值：成功返回1,其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_InvalidDataFit(double *pRscData, int count, float RanageLow, float RanageHeight, InvalidDataFitParam &IDFParam);
/*
函数功能：小数据段过滤函数。
参数：
pRscData-[in,out]需进行过滤的源数据。
count-[in]pRscData的数据长度。
continueCountThd-[in]数据段最少有效点数，小于等于此数量的数据段将被过滤。
RanageLow-[in]有效数据的下边界。
RanageHeight-[in]有效数据的上边界。
markValue-[in]待过滤数据填充的标记值。
返回值：成功返回1,其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_DeleteDataByContinueCount(double *pRscData, int count, int continueCountThd, float RanageLow, float RanageHeight, double markValue);
/*
函数功能：有效数据段两端翘起圆弧数据专用优化函数。
参数：
pRscH-[in,out]需进行优化的源数据。
count-[in]pRscH的数据长度。
prcParam-[in]数据优化参数结构体。
RanageLow-[in]有效数据的下边界。
RanageHeight-[in]有效数据的上边界。
返回值：成功返回1,其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_AbnormalDataProcess(double *pRscH, int count, EDPParam prcParam, MarkDataFitParam MDFParam, float RanageLow, float RanageHeight);
/*
函数功能：对指定数据做滑动平均，注意对于给定数据范围外的数据不做任何处理。
参数：
pRscData-[in,out]需进行优化的源数据。
count-[in]pRscData的数据长度。
avgCnt-[in]滑动平均因子，例如设置为5，则此函数内部将每个数据以其相邻5个点的平均作为新值。
RanageLow-[in]有效数据的下边界。
RanageHeight-[in]有效数据的上边界。
返回值：成功返回true,失败返回false。
*/
CONFOCAL_DLL bool __stdcall LE_MoveAverage(double *pRscData, int count, int avgCnt, float RanageLow, float RanageHeight);

/*
函数功能：获取指定应用测量功能是否有使用权限。
参数：
appIdx-[in]需获取的应用测量功能编号，参数可选范围为0~9。
返回值：0-无使用权限,1-有使用权限，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetAppSwitch(int appIdx);
/*
函数功能：通过趋势判断寻找最大值和最小值。
参数：
pData-[in]原始数据。
count-[in]pData数组长度。
limit0-[in]数据有效范围下限。
limit1-[in]数据有效范围上限。
FP-[in]数据滤波参数，根据数据实际情况选择。
mVal-[out]保存计算出的结果
avgCount-[out]计算结果时是否取参考点做平均，1-不做平均，3-在最小值或最大值的左右各取一个参考数据计算平均值后输出。
processResourceData-[in]是否直接对原始数据做滤波，true-当滤波功能开启后，此函数计算完成会直接改变pData数组内的数据，
false-不论滤波功能是否开启，此函数执行完成后都不会改变pData数组内的数据，最大最小值除外。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_CalcMinimaxVal(double* pData, int count, float limit0, float limit1, FilterParam FP, MinimaxVal &mVal, int avgCount = 3, bool processResourceData=false);
/*
函数功能：使用给定高度阈值进行去极值处理。
参数：
pRscH-[in,out]需进行去极值的源数据。
count-[in]pRscH的数据长度。
EDParam-[in]去极值参数结构体。
MDFP-[in]极值数据填充模式
RanageLow-[in]有效数据的下边界。
RanageHeight-[in]有效数据的上边界。
exportFstDrv-[out]去极值计算出的一阶导数，算法调试用。
exportSecDrv-[in]去极值计算出的二阶导数，算法调试用。
返回值：成功返回1,其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_ExtremeDataProcessExEx(double *pRscH, int count, ExtremeDataParam EDParam,MarkDataFitParam MDFP, double RanageLow, double RanageHeight, 
	double *exportFstDrv = 0, double *exportSecDrv = 0);