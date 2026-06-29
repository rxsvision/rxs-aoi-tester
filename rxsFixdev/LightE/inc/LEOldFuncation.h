#pragma once
#ifdef CONFOCAL_DLL
#else
#define CONFOCAL_DLL extern "C" _declspec(dllimport)
#endif
/********************新版本DLL不再推荐使用函数，以下函数为旧版本DLL函数接口，新版本DLL兼容以下
函数，客户新开发程序推荐使用新的替代函数******************************************************/
/*
函数功能：[仅E系列控制器支持]设置外触发源类型,请使用LE_SetTriggerMode函数设置对应触发模式代替。
参数：
type-[in]外触发源类型，0-编码器触发；1-外部单端24V信号。
specHandle-[in]控制器句柄。
返回值：设置成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetTriggerSourceType(int type, int specHandle);

/*
函数功能：[仅E系列控制器支持]获取外触发源类型。
参数：
specHandle-[in]控制器句柄。
返回值：0-编码器触发，1-外部单端24V信号。
*/
CONFOCAL_DLL int __stdcall LE_GetTriggerSourceType(int specHandle);
/*函数功能：获取控制器指定通道量程，单位为μm,建议使用LE_GetDistanceRangeLimit代替。*/
CONFOCAL_DLL int __stdcall LE_GetDistanceRange(float &dRange, int specHandle, int channel = 1);
/*函数功能：[只能在控制器空闲状态下调用]开启指定通道采集指定数量的高度测量结果任务。*/
CONFOCAL_DLL int __stdcall LE_StartGetValues(double *pRstHigh, int pts, int specHandle, int channel = 1);

/*函数功能：[只能在控制器空闲状态下调用]开启指定通道采集指定数量的高度测量结果、波形数据任务。*/
CONFOCAL_DLL int __stdcall LE_StartGetValuesIntensity(double *pRstHigh, int pts, int specHandle, int *pPrdItn, float *pUnitRa, int channel = 1);

/*函数功能：获取指定数量的测量结果、感光度、峰像素位置数据。*/
CONFOCAL_DLL int __stdcall LE_StartGetValuesPeakWave(double *pRstHigh, int pts, int specHandle, float *pUnitRa, float *pPeakWave, int channel = 1);

/*函数功能：[只能在控制器空闲状态下调用]获取指定数量的测量结果、波形曲线、感光度、峰像素位置数据。*/
CONFOCAL_DLL int __stdcall LE_StartGetValuesIntensityPeakWave(double *pRstHigh, int pts, int specHandle, int *pPrdItn, float *pUnitRa,
	float *pPeakWave, int channel = 1);

/*
函数功能：[只能在控制器空闲状态下调用]开启指定通道采集指定数量的高度测量结果任务,同时需要给定回
调函数指针，当采集任务完成或者被用户主动停止后，DLL内部将执行给定回调函数。
*/
CONFOCAL_DLL int __stdcall LE_StartGetValuesEx(double *pRstHigh, int pts, int specHandle, CBCaptureComplete pFunction, int executeMode = 0, int channel = 1);

/*
函数功能：[只能在控制器空闲状态下调用]开启指定通道采集指定数量的高度测量结果、能量曲线数据、感光
度数据任务，当采集任务完成或者被用户主动停止后，DLL内部将执行给定回调函数。
*/
CONFOCAL_DLL int __stdcall LE_StartGetValuesIntensityEx(double *pRstHigh, int pts, int specHandle, int *pPrdItn, float *pUnitRa,
	CBCaptureComplete pFunction, int executeMode = 0, int channel = 1);

/*
函数功能：[只能在控制器空闲状态下调用]获取指定数量的测量结果、感光度、峰像素位置数据,当采集任务
完成或者被用户主动停止后，DLL内部将执行给定回调函数。
*/
CONFOCAL_DLL int __stdcall LE_StartGetValuesPeakWaveEx(double *pRstHigh, int pts, int specHandle, float *pUnitRa, float *pPeakWave,
	CBCaptureComplete pFunction, int executeMode = 0, int channel = 1);

/*
函数功能：[只能在控制器空闲状态下调用]获取指定数量的测量结果、波形曲线、感光度、峰像素位置数据，
当采集任务完成或者被用户主动停止后，DLL内部将执行给定回调函数。
*/
CONFOCAL_DLL int __stdcall LE_StartGetValuesIntensityPeakWaveEx(double *pRstHigh, int pts, int specHandle, int *pPrdItn, float *pUnitRa,
	float *pPeakWave, CBCaptureComplete pFunction, int executeMode = 0, int channel = 1);

/********以下为旧版本函数不推荐使用，适用于单通道采集与多通道采集，当使用多通道采集时，可以通过
下列函数设置需要获取的数据类型及保存的位置，然后通过LE_StartGetChannelsValues函数来同
步启用多个通道的采集任务*********/
/*函数功能：[只能在控制器空闲状态下调用]设置指定控制器通道高度数据采集相关参数，但不开启采集任务。*/
CONFOCAL_DLL int __stdcall LE_SetChannelGetValuesParam(double *pRstHigh, int pts, int specHandle, int channel = 1);

/*函数功能：[只能在控制器空闲状态下调用]设置指定控制器通道高度数据、波形曲线数据、及感光度等数据采
集相关参数，但不开启采集任务。*/
CONFOCAL_DLL int __stdcall LE_SetChannelGetValuesIntensityParam(double *pRstHigh, int pts, int specHandle, int *pPrdItn, float *pUnitRa, int channel = 1);

/*函数功能：[只能在控制器空闲状态下调用]设置指定控制器通道高度数据、波形曲线数据、感光度、像素位置
数据采集相关参数，但不开启采集任务。*/
CONFOCAL_DLL int __stdcall LE_SetChannelGetValuesIntensityPeakWaveParam(double *pRstHigh, int pts, int specHandle, int *pPrdItn, float *pUnitRa,
	float *pPeakWave, int channel = 1);


/*
函数功能：[只能在控制器空闲状态下调用]设置指定控制器通道高度数据、波形曲线数据、感光度、像素位置
数据采集相关参数，但不开启采集任务。
*/
CONFOCAL_DLL int __stdcall LE_SetChannelGetValuesPeakWaveParam(double *pRstHigh, int pts, int specHandle, float *pUnitRa, float *pPeakWave, int channel = 1);

/*************************旧版本单边测厚数据获取相关函数************************/
/*函数功能：[只能在控制器空闲状态下调用]设置指定通道厚度数据采集相关参数，但不开启采集任务。*/
CONFOCAL_DLL int __stdcall LE_SetChannelGetThicknessParam(double *pRstTK, int pts, int specHandle, int channel = 1);

/*
函数功能：[只能在控制器空闲状态下调用]设置指定通道厚度数据、波形曲线数据、及感光度等数据采集相关
参数，但不开启采集任务。
*/
CONFOCAL_DLL int __stdcall LE_SetChannelGetThicknessItnParam(double *pRstTK, int pts, int specHandle, int *pPrdItn, float *pUnitRa, int channel = 1);

/*
函数功能：[只能在控制器空闲状态下调用]设置指定控制器通道厚度数据、上下表面等高度数据采集相关参数
，但不开启采集任务。
*/
CONFOCAL_DLL int __stdcall LE_SetChannelGetThicknessDTParam(double *pRstTK, double *pDistance1, double *pDistance2, int pts, int specHandle, int channel = 1);

/*
函数功能：[只能在控制器空闲状态下调用]设置指定控制器通道厚度数据、上下表面高度数据、波形曲线数据
、及感光度等数据采集相关参数，但不开启采集任务。
*/
CONFOCAL_DLL int __stdcall LE_SetChannelGetThicknessDTItnParam(double *pRstTK, double *pDistance1, double *pDistance2, int pts, int specHandle, int *pPrdItn,
	float *pUnitRa, int channel = 1);

/*
函数功能：[只能在控制器空闲状态下调用]设置指定控制器通道厚度数据、上下表面高度数据、波形曲线数据
、感光度、像素位置数据采集相关参数，但不开启采集任务。
*/
CONFOCAL_DLL int __stdcall LE_SetChannelGetThicknessDTItnPeakWaveParam(double *pRstTK, double *pDistance1, double *pDistance2, int pts, int specHandle,
	int *pPrdItn, float *pUnitRa, float *pPeakWave1, float *pPeakWave2, int channel = 1);

/*
函数功能：[只能在控制器空闲状态下调用]设置指定控制器通道厚度数据、上下表面高度数据、波形曲线数据
、感光度、波峰像素位置数据采集相关参数，但不开启采集任务。
*/
CONFOCAL_DLL int __stdcall LE_SetChannelGetThicknessDTPeakWaveParam(double *pRstTK, double *pDistance1, double *pDistance2, int pts,
	int specHandle, float *pUnitRa, float *pPeakWave1, float *pPeakWave2, int channel = 1);

/*
函数功能：[只能在控制器空闲状态下调用]获取指定数量的一种或多种类型数据，数据类型包含高度测量结果
、感光度、峰像素位置、波形曲线数据，实际调用时函数内部会根据给定参数指针是否为0来判断是否需要保
存到调用方给定的内存中，当采集任务完成或者被用户主动停止后如果此函数调用时给定了回调函数指针，那
么DLL内部将执行回调函数，此函数一旦调用成功则控制器将清空缓存开始进入数据采集状态,注意此函数不会
对传入的数组进行清零初始化操作。
参数：
pRstHigh-[out]保存采集到的测量结果数据数组，数组长度为pts。
pts-[in]期望采集测量结果点数。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
pUnitRa-[out]pts个点的感光度数组，数组长度为pts，如果不需要获取此项数据则此参数传入0或空指针。
pPeakWave-[out]pts个点峰像素位置数组，数组长度为pts，如果不需要获取此项数据则此参数传入0或空指针。
pPrdItn-[out]pts个点能量曲线数据数组，数组长度为pts*length（length代表单帧能量曲线的长度，可以通
过LE_GetChannelIntensityMsg函数获得），如果不需要获取此项数据则此参数传入0或空指针（嵌入式控制器暂
不支持通过此接口获取）。
pFunction-[in]采集任务完成或者被用户主动停止后执行的回调函数指针，如果不需要此功能则此参数传入0
或空指针。
executeMode-[in]回调函数在DLL内部执行方式，0-表示同步执行，DLL内部将调用并等到回调函数执行完成后
，当前采集任务才算完成，目前只支持此模式，其他异步等模式将后续集成开放。
data0-[in]预留参数，默认为0，目前不起作用。
返回值：
1-开始采集指令成功。
-1当前设备上一次采集任务尚未完成。
-12-给定指针参数无效,无法开启新的采集任务。
*/
CONFOCAL_DLL int __stdcall LE_StartGetAllValues(double *pRstHigh, int pts, int specHandle, int channel = 1, float *pUnitRa = 0, float *pPeakWave = 0,
	int *pPrdItn = 0, CBCaptureComplete pFunction = 0, int executeMode = 0, int data0 = 0);
/*
函数功能：[只能在控制器空闲状态下调用]获取指定数量的一种或多种类型数据，数据类型包含高度测量结果
、感光度、峰像素位置、波形曲线数据，波峰强度，实际调用时函数内部会根据给定参数指针是否为0来判断
是否需要保存到调用方给定的内存中，当采集任务完成或者被用户主动停止后如果此函数调用时给定了回调函
数指针，那么DLL内部将执行回调函数，此函数一旦调用成功则控制器将清空缓存开始进入数据采集状态,注意
此函数不会对传入的数组进行清零初始化操作。
参数：
pRstHigh-[out]保存采集到的测量结果数据数组，数组长度为pts。
pts-[in]期望采集测量结果点数。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
pUnitRa-[out]pts个点的感光度数组，数组长度为pts，如果不需要获取此项数据则此参数传入0或空指针。
pPeakWave-[out]pts个点峰像素位置数组，数组长度为pts，如果不需要获取此项数据则此参数传入0或空指针。
pPrdItn-[out]pts个点能量曲线数据数组，数组长度为pts*length（length代表单帧能量曲线的长度，可以通
过LE_GetChannelIntensityMsg函数获得），如果不需要获取此项数据则此参数传入0或空指针（嵌入式控制器暂
不支持通过此接口获取）。
pPeakItn-[out]保存每个高度数据的波峰位置的光强度，如果高度数据为有效值时则记录其波峰位置的光强度
，如果高度数据为无效数据时则记录该光谱曲线的最高光强度作为此数据波峰光强度，数组长度为pts如果不需
要此功能则此参数传入0或空指针。
pEncoder1-[out]保存锁存的轴0编码器的位置。
pEncoder2-[out]保存锁存的轴1编码器的位置。
pEncoder3-[out]保存锁存的轴2编码器的位置。
pEncoder4-[out]保存锁存的轴3编码器的位置。
pEncoder5-[out]保存锁存的轴4编码器的位置（仅嵌入式控制器支持）。
pEncoder6-[out]保存锁存的轴5编码器的位置（仅嵌入式控制器支持）。
pFunction-[in]采集任务完成或者被用户主动停止后执行的回调函数指针，如果不需要此功能则此参数传入0
或空指针。
executeMode-[in]回调函数在DLL内部执行方式，0-表示同步执行，DLL内部将调用并等到回调函数执行完成后
，当前采集任务才算完成，目前只支持此模式，其他异步等模式将后续集成开放。
data0-[in]预留参数，默认为0，目前不起作用。
返回值：
1-开始采集指令成功。
-1当前设备上一次采集任务尚未完成。
-12-给定指针参数无效,无法开启新的采集任务。
*/
CONFOCAL_DLL int __stdcall LE_StartGetAllValuesEx(double *pRstHigh, int pts, int specHandle, int channel = 1, float *pUnitRa = 0, float *pPeakWave = 0,
	int *pPrdItn = 0, int *pPeakItn = 0, float *pEncoder1 = 0, float *pEncoder2 = 0, float *pEncoder3 = 0, float*pEncoder4 = 0, float*pEncoder5 = 0,
	float*pEncoder6 = 0, CBCaptureComplete pFunction = 0, int executeMode = 0, int data0 = 0);
/**********************************************************************************************/

/**********适用于单通道采集与多通道采集，当使用多通道采集时，可以通过下列函数设置需要获取的数据
类型及保存的位置，然后通过LE_StartGetChannelsValues函数来同步启用多个通道的采集任务**********/
/*
函数功能：[只能在控制器空闲状态下调用]设置指定控制器指定通道采集多种类型数据，数据类型包含高度
测量结果、感光度、峰像素位置数据、波形曲线、波峰光强值、编码器数据，具体需采集哪些数据由给定的
指针或数组首地址是否等于0或为空来决定，此函数只设置好保存数据的数组及期望采集数据量，如果使用多
个通道同时采集数据则此函数需调用多次分别设置各通道的参数，需使用
LE_StartGetChannelsValues或LE_StartGetChannelsValuesEx来统一开启一个或多个通道的采集任务。
参数：
pRstHigh-[out]保存采集到的测量结果数据数组，数组长度为pts。
pts-[in]期望采集测量结果点数。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
pUnitRa-[out]pts个点的感光度数组，数组长度为pts，如果不需要获取此项数据则此参数传入0或空指针。
pPeakWave-[out]pts个点峰像素位置数组，数组长度为pts，如果不需要获取此项数据则此参数传入0或空指针。
pPrdItn-[out]pts个点能量曲线数据数组，数组长度为pts*length（length代表单帧能量曲线的长度，可以通
过LE_GetChannelIntensityMsg函数获得），如果不需要获取此项数据则此参数传入0或空指针（嵌入式控制器暂
不支持通过此接口获取）。
pPeakItn-[out]保存每个高度数据的波峰位置的光强度，如果高度数据为有效值时则记录其波峰位置的光强度
，如果高度数据为无效数据时则记录该光谱曲线的最高光强度作为此数据波峰光强度，数组长度为pts如果不需
要此功能则此参数传入0或空指针。
pEncoder1-[out]保存锁存的轴0编码器的位置。
pEncoder2-[out]保存锁存的轴1编码器的位置。
pEncoder3-[out]保存锁存的轴2编码器的位置。
pEncoder4-[out]保存锁存的轴3编码器的位置。
pEncoder5-[out]保存锁存的轴4编码器的位置（仅嵌入式控制器支持）。
pEncoder6-[out]保存锁存的轴5编码器的位置（仅嵌入式控制器支持）。
data0-[in]预留。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetChannelGetAllValuesExEx(double *pRstHigh, int pts, int specHandle, int channel = 1, float *pUnitRa = 0, float *pPeakWave = 0, int *pPrdItn = 0
	, int *pPeakItn = 0, float *pEncoder1 = 0, float *pEncoder2 = 0, float *pEncoder3 = 0, float*pEncoder4 = 0, float*pEncoder5 = 0, float*pEncoder6 = 0, int data0 = 0);

/*
函数功能：[只能在控制器空闲状态下调用]设置指定控制器指定通道获取指定数量的一种或多种类型数据，数
据类型包含高度测量结果、波形曲线、感光度、峰像素位置数据，实际调用时函数内部会根据给定参数指针是
否为0来判断该类型数据是否需要保存到调用方给定的内存中，此函数只设置好保存数据的数组及期望采集数
据量，如果使用多个通道同时采集数据则此函数需调用多次分别设置各通道的参数，需使用
LE_StartGetChannelsValues或LE_StartGetChannelsValuesEx来统一开启一个或多个通道的采集任务。
参数：
pRstHigh-[out]保存采集到的测量结果数据数组，数组长度为pts。
pts-[in]期望采集测量结果点数。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
pUnitRa-[out]pts个点的感光度数组，数组长度为pts，如果不需要获取此项数据则此参数传入0或空指针。
pPeakWave-[out]pts个点峰像素位置数组，数组长度为pts，如果不需要获取此项数据则此参数传入0或空指针。
pPrdItn-[out]pts个点能量曲线数据数组，数组长度为pts*length（length代表单帧能量曲线的长度，可以通
过LE_GetChannelIntensityMsg函数获得），如果不需要获取此项数据则此参数传入0或空指针。
data0-[in]预留参数，默认为0，目前不起作用。
返回值：
1-开始采集指令成功。
-1当前设备上一次采集任务尚未完成。
-12-给定指针参数无效,无法开启新的采集任务。
*/
CONFOCAL_DLL int __stdcall LE_SetChannelGetAllValues(double *pRstHigh, int pts, int specHandle, int channel = 1, float *pUnitRa = 0, float *pPeakWave = 0,
	int *pPrdItn = 0, int data0 = 0);

/*
函数功能：[只能在控制器空闲状态下调用]指定控制器清空缓存并开启指定控制器多通道采集任务。
参数：
specHandle-[in]控制器句柄。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_StartGetChannelsValues(int specHandle);

/*
函数功能：[只能在控制器空闲状态下调用]指定控制器清空缓存并开启指定控制器多通道采集任务,同时需要
给定回调函数指针，当采集任务完成或者被用户主动停止后，DLL内部将执行给定回调函数。
参数：
specHandle-[in]控制器句柄；
pFunction-[in]采集任务完成或者被用户主动停止后执行的回调函数指针，capturedCount保存了当前任务已
采集数据数量。
executeMode-[in]回调函数在DLL内部执行方式，0-表示同步执行，DLL内部将调用并等到回调函数执行完成后
，当前采集任务才算完成，目前只支持此模式，其他异步等模式将后续集成开放。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_StartGetChannelsValuesEx(int specHandle, CBCaptureComplete pFunction, int executeMode = 0);

/*
函数功能：[只能在控制器空闲状态下调用]设置指定控制器指定通道采集多种类型数据，其中包含第一层厚度
数据、上下表面高度数据、感光度、上下表面波峰像素位置数据、波形曲线数据，具体需采集哪些数据由给定
的指针或数组首地址是否等于0或为空来决定，此函数只设置好保存数据的数组及期望采集数据量，如果使用
多个通道同时采集数据则此函数需调用多次分别设置各通道的参数，需使用LE_StartGetChannelsValues或
LE_StartGetChannelsValuesEx来统一开启一个或多个通道的采集任务。
参数：
pRstTK-[out]保存采集到的厚度数据数组。
pts-[in]期望获取厚度点数。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
pDistance1-[out]保存采集到的上表面高度数组，如果不需要获取此项数据则此参数传入0或空指针。
pDistance2-[out]保存采集到的下表面高度数组，如果不需要获取此项数据则此参数传入0或空指针,注意
pDistance1和pDistance1是否获取需保持一致，即要么2个都获取，要么都不获取。
pUnitRa-[out]pts个点感光度数组，如果不需要获取此项数据则此参数传入0或空指针。
pPeakWave1-[out]保存上表面波峰像素位置数组，数组长度为pts，如果不需要获取此项数据则此参数传入0或
空指针。
pPeakWave2-[out]保存下表面波峰像素位置数组，数组长度为pts，如果不需要获取此项数据则此参数传入0或
空指针,注意pPeakWave1和pPeakWave2是否获取需保持一致，即要么2个都获取，要么都不获取。
pPrdItn-[out]pts个点波形曲线数据数组,数组长度为pts*length（length代表单帧光谱的长度，可以通过
LE_GetChannelIntensityMsg函数获得），如果不需要获取此项数据则此参数传入0或空指针。
data0-[out]预留。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetChannelGetThicknessAllValuesParam(double *pRstTK, int pts, int specHandle, int channel = 1, double *pDistance1 = 0,
	double *pDistance2 = 0, float *pUnitRa = 0, float *pPeakWave1 = 0, float *pPeakWave2 = 0, int *pPrdItn = 0, float* data0 = 0);

/*
函数功能：[只能在控制器空闲状态下调用]设置指定控制器指定通道采集多种类型数据，其中包含多层厚度数
据、多层上下表面高度数据、感光度、多层上下表面波峰像素位置数据、波形曲线数据，具体需采集哪些数据
由给定的指针或数组首地址是否等于0或为空来决定，此函数只设置好保存数据的数组及期望采集数据量，如
果使用多个通道同时采集数据则此函数需调用多次分别设置各通道的参数，需使用
LE_StartGetChannelsValues或LE_StartGetChannelsValuesEx来统一开启一个或多个通道的采集任务。
参数：
pRstTK-[out]保存采集到的厚度数据数组，长度等于pts*tkCount。
pts-[in]期望获取厚度点数。
tkCount-[in]单帧能量数据中期望获取的层数，最小为1层，目前最大为5层。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
pDistance1-[out]保存采集到的上表面高度数组，长度等于pts*tkCount，如果不需要获取此项数据则此参数
传入0或空指针。
pDistance2-[out]保存采集到的下表面高度数组，长度等于pts*tkCount，如果不需要获取此项数据则此参数
传入0或空指针,注意pDistance1和pDistance1要么2个都获取，要么都不获取。
即要么2个都获取，要么都不获取。
pUnitRa-[out]pts个点感光度数组，如果不需要获取此项数据则此参数传入0或空指针。
pPeakWave1-[out]保存上表面波峰像素位置数组，长度等于pts*tkCount，如果不需要获取此项数据则此参数
传入0或空指针。
pPeakWave2-[out]保存下表面波峰像素位置数组，长度等于pts*tkCount，如果不需要获取此项数据则此参数
传入0或空指针,注意pPeakWave1和pPeakWave2要么2个都获取，要么都不获取。
pPrdItn-[out]pts个点波形曲线数据数组,数组长度为pts*length（length代表单帧光谱的长度，可以通过
LE_GetChannelIntensityMsg函数获得），如果不需要获取此项数据则此参数传入0或空指针。
data0-[out]预留。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetChannelGetThicknessAllValuesParamEx(double *pRstTK, int pts, int tkCount, int specHandle, int channel = 1, double *pDistance1 = 0,
	double *pDistance2 = 0, float *pUnitRa = 0, float *pPeakWave1 = 0, float *pPeakWave2 = 0, int *pPrdItn = 0, float* data0 = 0);

/*
函数功能：[只能在控制器空闲状态下调用]设置指定控制器指定通道采集多种类型数据，其中包含多层厚度数
据、多层上下表面高度数据、感光度、多层上下表面波峰像素位置数据、波形曲线数据、编码器数据，具体需
采集哪些数据由给定的指针或数组首地址是否等于0或为空来决定，此函数只设置好保存数据的数组及期望采
集数据量，如果使用多个通道同时采集数据则此函数需调用多次分别设置各通道的参数，需使用
LE_StartGetChannelsValues或LE_StartGetChannelsValuesEx来统一开启一个或多个通道的采集任务。
参数：
pRstTK-[out]保存采集到的厚度数据数组，长度等于pts*tkCount。
pts-[in]期望获取厚度点数。
tkCount-[in]单帧能量数据中期望获取的层数，最小为1层，目前最大为5层。
specHandle-[in]控制器句柄。
channel-[in]通道序号。
pDistance1-[out]保存采集到的上表面高度数组，长度等于pts*tkCount，如果不需要获取此项数据则此参数
传入0或空指针。
pDistance2-[out]保存采集到的下表面高度数组，长度等于pts*tkCount，如果不需要获取此项数据则此参数
传入0或空指针,注意pDistance1和pDistance1要么2个都获取，要么都不获取。
即要么2个都获取，要么都不获取。
pUnitRa-[out]pts个点感光度数组，如果不需要获取此项数据则此参数传入0或空指针。
pPeakWave1-[out]保存上表面波峰像素位置数组，长度等于pts*tkCount，如果不需要获取此项数据则此参数
传入0或空指针。
pPeakWave2-[out]保存下表面波峰像素位置数组，长度等于pts*tkCount，如果不需要获取此项数据则此参数
传入0或空指针,注意pPeakWave1和pPeakWave2要么2个都获取，要么都不获取。
pPrdItn-[out]pts个点波形曲线数据数组,数组长度为pts*length（length代表单帧光谱的长度，可以通过
LE_GetChannelIntensityMsg函数获得），如果不需要获取此项数据则此参数传入0或空指针。
pEncoder1-[out]保存锁存的轴0编码器的位置。
pEncoder2-[out]保存锁存的轴1编码器的位置。
pEncoder3-[out]保存锁存的轴2编码器的位置。
pEncoder4-[out]保存锁存的轴3编码器的位置。
pEncoder5-[out]保存锁存的轴4编码器的位置（仅嵌入式控制器支持）。
pEncoder6-[out]保存锁存的轴5编码器的位置（仅嵌入式控制器支持）。
data0-[out]预留。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_SetChannelGetThicknessValuesExEx(double *pRstTK, int pts, int tkCount, int specHandle, int channel = 1, double *pDistance1 = 0,
	double *pDistance2 = 0, float *pUnitRa = 0, float *pPeakWave1 = 0, float *pPeakWave2 = 0, int *pPrdItn = 0, float *pEncoder1 = 0, float *pEncoder2 = 0,
	float *pEncoder3 = 0, float*pEncoder4 = 0, float*pEncoder5 = 0, float*pEncoder6 = 0, float* data0 = 0);

/*
函数功能：[只能在控制器空闲状态下调用]获取指定控制器一个或多个通道的高度测量结果,函数执行后控制
器才开始采集数据，采集完成后函数才返回执行状态，并且结果值存储在pValue中，注意此函数每一个通道只
能获取一个高度结果，支持获取多个通道，例如channels的值为2时，函数执行完后pValue保存2个高度结果，
分别为已启用的第一个通道和第二个通道的高度结果，推荐使用LE_GetSingleValueRTEx替代。
参数：
pValue-[out]保存获取到的测量结果，如果需要获取多个通道结果时，所有结果按通道顺序依次保存。
specHandle-[in]控制器句柄。
channels-[in]需获取结果值的通道数量，例如该参数为1时表示获取第一个通道的结果值，该参数为2时表示
获取已启用通道的第一、第二两个通道的结果值，依此类推。
data0-[in]预留参数。
data1-[in]预留参数。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetSingleValueRT(double *pValue, int specHandle, int channels = 1, int data0 = 0, int data1 = 0);
/*
函数功能：[只能在控制器空闲状态下调用]获取指定控制器一个或多个通道的厚度测量结果,函数执行后控制
器才开始采集数据，采集完成后函数才返回执行状态，并且结果值存储在pTKValue、pDistance1、pDistance2
中，注意此函数每一个通道只能获取tkCount个结果，支持获取多个通道，例如channels的值为2，tkCount值
为1时，函数执行完后pTKValue保存2*1个厚度结果，分别为已启用的第一个通道的第一层厚度和第二个通道的
第一层厚度结果，同理pDistance1、pDistance2存储情况也是一样推荐使用LE_GetSingleTKValueRTEx替代。
参数：
pTKValue-[out]获取到的厚度测量结果，该数组长度需大于等于tkCount*channels。
tkCount-[in]期望获取的层数。
specHandle-[in]控制器句柄。
channels-[in]需获取结果值的通道数量，例如该参数为1时表示获取第一个通道的结果值，该参数为2时表示
获取已启用通道的第一、第二两个通道的结果值，依此类推。
pDistance1-[out]获取到的每一层上表面测量结果，该数组长度需大于等于tkCount*channels，如果不需要该
值则参数输入0或空指针，假设tkCount=2，channels=2，则第二个通道的上表面测量结果存储的起始地址为
pDistance1[1*tkCount]。
pDistance2-[out]获取到的每一层下表面测量结果，该数组长度需大于等于tkCount*channels，如果不需要该
值则参数输入0或空指针,内部存储顺序与pDistance1相同。
data0-[in]预留参数。
data1-[in]预留参数。
返回值：成功返回1，其他返回值请参考返回值定义表。
*/
CONFOCAL_DLL int __stdcall LE_GetSingleTKValueRT(double *pTKValue, int tkCount, int specHandle, int channels = 1,
	double *pDistance1 = 0, double *pDistance2 = 0, int data0 = 0, int data1 = 0);

/*
函数功能：设置指定控制器外触发模式下分频触发比例,注意该功能需控制器本身支持该功能才可使用!
*/
CONFOCAL_DLL int __stdcall LE_SetFractionalFrequency(int specHandle, int counterVal = 1);

/*函数功能：获取指定控制器外触发模式下分频触发比例,注意该功能需控制器本身支持该功能才可使用!*/
CONFOCAL_DLL int __stdcall LE_GetFractionalFrequency(int specHandle);

/*
函数功能：清除指定控制器外触发模式下分频计数，例如设置分频比例为5时，已发送了3个触发信号时，希望
控制器对触发信号重新计数时可调用该函数,注意该功能需控制器本身支持该功能才可使用!
*/
CONFOCAL_DLL int __stdcall LE_ClearFractionalCountesy(int specHandle);

/**********************************新版本废弃函数，只保留接口以便兼容旧版本DLL,函数执行将不做任
何操作，若之前有使用到可直接删除相关代码******************************************************/
/*函数功能：设置通道光斑中心位置,及单个通道光斑所占行数,当前函数于20190701已停止使用。*/
CONFOCAL_DLL int __stdcall LE_SetPosOffset(int offsetLine, int lines, int specHandle, int channel);

/*
函数功能：采集控制器指定通道dark数据，采集前请先用黑色盖子盖住对应通道探头，直到采集完成，当前函
数已于20170701停止使用，请使用LE_CaptureChannelsIntensityCaliData函数进行dark操作。
*/
CONFOCAL_DLL int __stdcall LE_CaptureIntensityCaliData(int specHandle, int channel = 1);
/*函数功能：获取控制器指定通道调试数据是否导出开关,旧版函数，只保留接口，函数调用不起任何作用。*/
CONFOCAL_DLL int __stdcall LE_GetDebugDataExportState(int &exportSta, int specHandle);

/*函数功能：设置控制器指定通道调试数据是否导出开关,旧版函数，只保留接口，函数调用不起任何作用。*/
CONFOCAL_DLL int __stdcall LE_SetDebugDataExportState(int exportSta, int specHandle);

/*
函数功能：开启或关闭DLL内部自动调节曝光、增益功能。
备注：20200918废除，即使调用也不启任何作用！！！
*/
CONFOCAL_DLL int __stdcall LE_SetAutoEptGain(bool bAuto, int specHandle);

/*
函数功能：获取DLL内部自动调节曝光、增益功能状态。
备注：20200918废除，即使调用也不启任何作用！！！
*/
CONFOCAL_DLL int __stdcall LE_GetAutoEptGainSta(bool &bAuto, int specHandle);

/*
函数功能：[只能在控制器空闲状态下调用]通过文件载入单通道配置数据,LE_LoadDeviceConfigureFromFile
函数调用后已载入所有通道参数，当前函数仅在某通道参数修改乱了之后需重新加载文件内参数使用，一般无
需使用。
备注：20210110废除。
*/
CONFOCAL_DLL int  __stdcall LE_LoadChannelConfigureFromFile(wchar_t *filePath, int specHandle, int channel = 1, wchar_t *pFiberSN = 0, wchar_t *pLensSN = 0);
