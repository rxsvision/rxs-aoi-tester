#ifndef __MPSIZECTOR_API_H__
#define __MPSIZECTOR_API_H__


/////////////////////////////////////////////////////////////////////////
/// MPSizector API
/////////////////////////////////////////////////////////////////////////

//Parameters

/// <summary>
///内部使用，DLL导出的标识。\n
///Internal use, the sign for DLL export。
/// </summary>
#ifndef _WIN32
#define DLL_EXPORT
#else
#define DLL_EXPORT __declspec(dllexport)
#endif

/// <summary>
/// MPDeviceInfo中DeviceName的缓冲区长度。\n
/// The DeviceName buffer size of MPDeviceInfo.
/// </summary>
#define MP_PRODUCT_NAME_MAX_LEN 256

/// <summary>
/// MPDeviceInfo中DeviceSerialNumber的缓冲区长度。\n
/// The DeviceSerialNumber buffer size of MPDeviceInfo.
/// </summary>
#define MP_PRODUCT_SERIAL_NUMBER_MAX_LEN 256

/// <summary>
/// API版本号。\n
/// API Version.
/// </summary>
#define MP_SIZECTOR_API_VER 3.07f

//Data structure

/// <summary>
/// 用于存储设备信息的结构体。\n
/// Device info structure.
/// </summary>
struct MPDeviceInfo
{
	///设备名称.\n
	///Device name.
	wchar_t DeviceName[MP_PRODUCT_NAME_MAX_LEN];
	///设备序列号。该序列号为设备的惟一序列号。\n
	///Unique serial number for the device.
	wchar_t DeviceSerialNumber[MP_PRODUCT_SERIAL_NUMBER_MAX_LEN];
	///设备的固件版本号 \n
	///Version of device firmware
	unsigned int FWVersion;
	///设备所需要的最低SDK版本\n
	///The minimum version required by device
	float MinimumSDKVersion;
	///设备的X方向像素分辨率\n
	///The pixel resolution of X direction
	unsigned int SensorWidth;
	///设备的Y方向像素分辨率\n
	///The pixel resolution of Y direction
	unsigned int SensorHeight;
	///设备的像素分辨率\n
	///The pixel resolution of device
	unsigned int SensorResolution;
	///设备的X方向物理坐标最大值(mm)\n
	///The maximum X coordinate value of device, in mm.
	float XMax;
	///设备的Y方向物理坐标最大值(mm)\n
	///The maximum Y coordinate value of device, in mm.
	float YMax;
	///设备的Z方向物理坐标最大值(mm)\n
	///The maximum Z coordinate value of device, in mm.
	float ZMax;
	///设备是否通过USB3.0连接。\n
	///Whether device is connected through USB 3.0.
	bool IsUSB30;

};

/// <summary>
/// 三维数据单像素基本结构体。\n
/// Basic struct to represent one 3D point data.
/// </summary>
struct MP3DDataStruct
{
	/// 像素灰度值。\n Gray scale value of the point.
	unsigned char Gray;
	/// 像素掩码值。当且仅当为0时，此点为有效点。\n
    /// Mask value of the point. This point is valid if and only if Mask == 0.
	unsigned char Mask;
	/// X 坐标，单位mm。\n X Coordinate in mm.
	float X;
	/// Y 坐标，单位mm。\n Y Coordinate in mm. 
	float Y;
	/// Z 坐标，单位mm。\n Z Coordinate in mm. 
	float Z;
};

/// <summary>
/// 一帧(一次拍摄)的<b>未托管</b>三维数据基本结构体。 \n
/// Basic struct to represent UNMANAGED 3D data of one frame (i.e. one-time scan).
/// </summary>
struct MP3DFrame
{
	///设备的X方向像素分辨率\n
	///The pixel resolution of X direction
	unsigned int SensorWidth;
	///设备的Y方向像素分辨率\n
	///The pixel resolution of Y direction
	unsigned int SensorHeight;
	///设备的像素分辨率\n
	///The pixel resolution of device
	unsigned int SensorResolution;
	///设备的X方向物理坐标最大值(mm)\n
	///The maximum X coordinate value of device, in mm.
	float XMax;
	///设备的Y方向物理坐标最大值(mm)\n
	///The maximum Y coordinate value of device, in mm.
	float YMax;
	///设备的Z方向物理坐标最大值(mm)\n
	///The maximum Z coordinate value of device, in mm.
	float ZMax;
	/// 三维点云数据的数组指针。\n Pointer to the array of 3D data.
	MP3DDataStruct* Data;
};

/// <summary>
/// 一帧(一次拍摄)的<b>未托管</b>二维数据基本结构体\n
/// Basic struct to represent UNMANAGED 2D data of one frame (i.e. one-time scan). 
/// </summary>
struct MP2DFrame
{
	///设备的X方向像素分辨率\n
	///The pixel resolution of X direction
	unsigned int SensorWidth;
	///设备的Y方向像素分辨率\n
	///The pixel resolution of Y direction
	unsigned int SensorHeight;
	///设备的像素分辨率\n
	///The pixel resolution of device
	unsigned int SensorResolution;
	///设备的X方向物理坐标最大值(mm)\n
	///The maximum X coordinate value of device, in mm.
	float XMax;
	///设备的Y方向物理坐标最大值(mm)\n
	///The maximum Y coordinate value of device, in mm.
	float YMax;
	///设备的Z方向物理坐标最大值(mm)\n
	///The maximum Z coordinate value of device, in mm.
	float ZMax;
	/// 灰度数据的数组指针。\n Pointer to the array of 3D data.
	unsigned char* Data;
};

/// <summary>
/// 深度图像结构体, 数据以一维数组存储。任意像素点(i, j)对应三维坐标为：\n
/// Range image struct, data is stored as 1-dimensional array. 3D coordinates of any pixel(i, j) is：\n
/// X = XOffset + i * PixelInterval \n
/// Y = YOffset + j * PixelInterval \n
/// Z = ZOffset + RangeData[i + j * Width] * ZInterval = ZRawData[i + j * Width] \n
/// </summary>
struct RangeImage
{
	///高度数据数组指针(mm)\n
	///Height data array, in mm.
	float* ZRawData;
	///以unsigned short表示的高度数据数组指针(mm). \n
	///Height data array stored as unsigned short type, in mm.
	unsigned short* RangeData;
	///像素灰度值数值指针。\n Gray scale value array.
	unsigned char* Textures;
	///掩码数据指针，指示对应高度数据是否为有效值。\n
	///Pointer to the mask data, indicate whether the height data is valid.
	bool* Mask;
	///图像的X方向像素长度\n
	///The pixel resolution of X direction
	unsigned int Width;
	///图像的Y方向像素长度\n
	///The pixel resolution of Y direction
	unsigned int Height;
	///图像的X方向偏移(mm).\n
	///The offset of X direction, in mm
	float XOffset;
	///图像的Y方向偏移(mm).\n
	///The offset of Y direction, in mm
	float YOffset;
	///图像的Z方向偏移(mm).\n
	///The offset of Z direction, in mm
	float ZOffset;
	///图像的X和Y方向像素间隔(mm)\n
	///The pixel interval of X and Y direction, in mm
	float PixelInterval;
	///图像的Z方向像素间隔(mm)\n
	///The pixel interval of Z direction, in mm
	float ZInterval;
};


class CallBack3DBase
{
public:
	virtual void Call(unsigned char param) = 0;
};

template <class T>
class CallBack3DItem : public CallBack3DBase
{
private:
	void (T::*CallBackFn_Ptr)(unsigned char);
	T* Obj_Ptr;

public:
	void Call(unsigned char param)
	{
		if (!Obj_Ptr)
			return;

		(Obj_Ptr->*CallBackFn_Ptr)(param);
	}

	CallBack3DItem(void (T::*_CallBackFn_Ptr)(unsigned char), T* _Obj_Ptr):CallBackFn_Ptr(_CallBackFn_Ptr), Obj_Ptr(_Obj_Ptr) {}
};

class CallBack2DBase
{
public:
	virtual void Call(unsigned char param1, unsigned char param2) = 0;
};

template <class T>
class CallBack2DItem : public CallBack2DBase
{
private:
	void (T::*CallBackFn_Ptr)(unsigned char, unsigned char);
	T* Obj_Ptr;

public:
	void Call(unsigned char param1, unsigned char param2)
	{
		if (!Obj_Ptr)
			return;

		(Obj_Ptr->*CallBackFn_Ptr)(param1, param2);
	}

	CallBack2DItem(void (T::*_CallBackFn_Ptr)(unsigned char, unsigned char), T* _Obj_Ptr):CallBackFn_Ptr(_CallBackFn_Ptr), Obj_Ptr(_Obj_Ptr) {}
};

//enum

/// <summary>
/// 设备工作模式枚举。\n
/// The enum of device working mode.
/// </summary>
enum WorkingModeType
{
	/// 标准3D拍摄工作模式。\n
    /// Standard 3D capture working mode.\n
	WorkingMode3D = 0,
	/// 快速3D拍摄工作模式。\n
    /// Fast 3D capture working mode.\n
	WorkingMode3DFast = 2,
	/// 精准3D拍摄工作模式。 \n
	/// Precise 3D capture working mose.\n
	WorkingMode3DPrecise = 4,
	/// 超准3D拍摄工作模式。 \n
	/// Super precise 3D capture working mose.\n
	WorkingMode3DSuperPrecise = 6,
	/// 无照明2D拍摄工作模式。\n
    /// 2D capture with no light projection working mode.\n
	WorkingMode2DBlack = 10,
	/// 带有照明的2D拍摄工作模式。\n
    /// 2D capture with full light projection working mode. Data returned is MP2DFrame or MP2DFrameManaged.\n
	WorkingMode2DWhite = 11,
	/// 带有格栅投影的2D拍摄工作模式。\n
    /// 2D cpature with grid projection working mode. Data returned is MP2DFrame or MP2DFrameManaged.\n
	WorkingMode2DGrid = 12
};

/// <summary>
/// 设备触发方式枚举。 \n  
/// The enum of trigger mode.
/// </summary>
enum TriggerSourceType
{
	/// 无触发，仅能通过软件发送命令触发。\n
    /// No trigger. The device will only start a snap when recieving a software trigger.
	TriggerSourceNoTrigger = 0,
	/// 内部连续触发，设备将会在一次拍摄、计算、传输结束后立刻开始下一次拍摄。\n
    /// Internal keep-running trigger. The device will start next snap when last snap is finished automatically.
	TriggerSourceKeepRunning = 1,
	/// 外部触发，通过IO输入引脚I0信号触发。\n
    /// External trigger by input pin I0. The device will start a snap when recieving a rising edge from I0.
	TriggerSourceExtI0 = 2,
	/// 外部触发，通过IO输入引脚I1信号触发。\n
    /// External trigger by input pin I1. The device will start a snap when recieving a rising edge from I1.
	TriggerSourceExtI1 = 3,
	/// 保留 \n Reserved
	TriggerSourceExtI2 = 4,
	/// 保留 \n Reserved
	TriggerSourceExtI3 = 5,
	/// 保留 \n Reserved
	TriggerSourceExtI4 = 6,
	/// 保留 \n Reserved
	TriggerSourceExtI5 = 7,
	/// 保留 \n Reserved
	TriggerSourceExtI6 = 8,
	/// 保留 \n Reserved
	TriggerSourceExtI7 = 9
};

/// <summary>
/// 连接事件类型枚举。\n
/// Connection event enum.
/// </summary>
enum ConnectionEventType
{
	/// 设备通过USB3.0建立连接\n
    /// Device is connected through USB3.0.
	ConnectionEventReconnectedU3 = 0,
	/// 设备通过USB2.0建立连接\n
    /// Device is connected through USB2.0.
	ConnectionEventReconnectedU2 = 1,
	/// 设备断开连接\n
    /// Deviced is disconnected
	ConnectionEventDisconnected = 2,
};

/// <summary>
/// 转换坐标为2D深度图的方法枚举类型 \n
/// Method of convert depth image enum
/// </summary>
enum Convert3DTargetType
{
    X = 0,
    Y = 1,
    Z = 2
};

/// <summary>
/// 诊断信息类型 \n
/// Diagnostics Info type enum
/// </summary>
enum DiagnosticsInfoType
{
	DiagInfoBlankMessage = 0,
	DiagInfoConnectionLost = 1,
	DiagInfoConnectSuccess = 2,
	DiagInfoConnectFail = 3,
	DiagInfoReconnectSuccess = 4,
	DiagInfoReconnectFail = 5,
	DiagInfoCloseConnection = 6,
	DiagInfoTransferThreadExit = 7,
	DiagInfoConnectionError = 8,

	DiagInfoHeartBeat = 10,
	DiagInfoSnapStart = 11,
	DiagInfoTransferStart = 12,
	DiagInfoRACK = 13,
	DiagInfoACK = 14,
	DiagInfoNAK = 15,


    DiagInfoSoftwareTriggerSent = 50,
    DiagInfoCommandFailure = 51,
    DiagInfo2DCallBackSet = 52,
    DiagInfo2DCallBackEnable = 53,
    DiagInfo2DCallBackDisable = 54,
    DiagInfo3DCallBackSet = 55,
    DiagInfo3DCallBackEnable = 56,
    DiagInfo3DCallBackDisable = 57,
    DiagInfoConnectionEventCallBackSet = 58,

	DiagInfoHoldStateSet = 60,
	DiagInfoWorkingModeSet = 61,
	DiagInfoLEDCurrentSet = 62,
	DiagInfoExposureTimeSet = 63,
	DiagInfoTriggerSourceSet = 64,
	DiagInfoOFunctionSelSet = 65,
	DiagInfoOValueSet = 66,
	DiagInfoValidPointThresholdSet = 67,
	DiagInfoOverExposureFilterThresholdSet = 68,
	DiagInfoPreProcessLoopNumSet = 69,

	DiagInfoSoftwareReset = 80,
	DiagInfoDefaultSettingsSave = 90,



	DiagInfoDataPacket = 100,
	DiagInfoEndPacket = 101,
	DiagInfoStart2DFrame = 102,
	DiagInfoStart3DFrame = 103,
	DiagInfoFinish2DFrame = 104,
	DiagInfoFinish3DFrame = 105,
	DiagInfoPacketError = 106,
	DiagInfoSpecialData = 107,
	
	DiagInfoSnap2DEnter = 108,
	DiagInfoSnap3DEnter = 109,
	DiagInfoSnap2DReturn = 110,
	DiagInfoSnap3DReturn = 111,
	DiagInfoSnap2DTimeOut = 112,
	DiagInfoSnap3DTimeOut = 113,
	DiagInfoCallBack2DEnter = 114,
	DiagInfoCallBack3DEnter = 115,
	DiagInfoCallBack2DExit = 116,
	DiagInfoCallBack3DExit = 117,

	DiagInfoDebugRegisters = 150
};


//callback function type define

/// <summary>
/// 二维数据获取回调函数类型。\n
/// Callback function type for acquiring 2D data from device.
/// </summary>
/// <param name="BufferIndex">
/// 由回调提供的当前帧在缓存中的Index。缓存为调用Set2DDataCallBack的返回值。\n
/// The index of current frame stored in buffer, provided by call bacck. The buffer is allocated by calling Set2DDataCallBack.
/// </param>
/// <param name="PicIndex">
/// 保留功能\n
/// Reserved
/// </param>
typedef void (*MPSizector2DDataCallBackType)(unsigned char BufferIndex, unsigned char PicIndex);

/// <summary>
/// 三维数据获取回调函数类型。\n
/// Callback function type for acquiring 3D data from device.
/// </summary>
/// <param name="BufferIndex">
/// 由回调提供的当前帧在缓存中的Index。缓存为调用Set3DDataCallBack的返回值。\n
/// The index of current frame stored in buffer, provided by call bacck. The buffer is allocated by calling Set3DDataCallBack.
/// </param>
typedef void (*MPSizector3DDataCallBackType)(unsigned char BufferIndex);


/// <summary>
/// 连接状态改变回调函数类型。\n
/// Callback function type for connection status changing.
/// </summary>
/// <param name="ConnectionEvent">
/// 连接事件。\n
/// Connection event.
/// </param>
typedef void (*MPSizectorConnectionEventCallBackType) (ConnectionEventType ConnectionEvent);



/// <summary>
/// 诊断信息回调函数类型。\n
/// Callback function type for diagnostics.
/// </summary>
typedef void (*MPSizectorDiagnosticsCallBackType)(DiagnosticsInfoType DiagInfo, unsigned int Param0, unsigned int Param1, unsigned int Param2, unsigned int Param3, unsigned int Param4, unsigned int Param5, unsigned int Param6, unsigned int Param7);


//Class Interface

    /// <summary>
    /// MPSizector接口类, 是用户进行3D相机所有控制的接口。\n
    /// Interface class of MPSizector. It is used for all control functions of 3D camera.
    /// </summary>
class DLL_EXPORT IMPSizector
{
public:

	//Manager operation

	/// <summary>
    /// 刷新连接的设备列表.类初始化时会自动调用。必须调用后才能通过GetDeviceCount和GetDeviceInfo得到最新的设备信息。\n
    /// Update the connected device list. It is called when the MPSizector is initiallized. User need to call this function before calling GetDeviceCount and GetDeviceInfo to avoid device change.
    /// </summary>
    /// <returns>
    /// 是否成功。\n
    /// If success.
    /// </returns>
	virtual bool UpdateDeviceList(void) = 0;
	/// <summary>
    /// 获取连接设备的数量。\n
    /// Get the number of connected device(s)
    /// </summary>
    /// <returns>
    /// 设备数量\n
    /// Number of device(s)
    /// </returns>
	virtual int GetDeviceCount(void) = 0;
	/// <summary>
	/// 获取设备信息。\n
	/// Get the device infomation of selected device.
	/// </summary>
	/// <param name="Index">
	/// \[输入\]:对应设备的序号，从0开始，小于GetDeviceCount的返回值\n
	/// [INPUT]:Index of the device. Start from 0, and must be smaller than the return value of GetDeviceCount();
	/// </param>
	/// <returns>
	/// 对应设备的信息\n
	/// Device Information
	/// </returns>
	virtual MPDeviceInfo GetDeviceInfo(int Index) = 0;

	//Connection operations

	/// <summary>
    /// 根据设备信息打开设备。\n
    /// Open device according to input device info.
    /// </summary>
    /// <param name="DeviceInfo">
    /// \[输入\]:设备信息\n
    /// [INPUT]: Device Info.
    /// </param>
    /// <returns>
    /// 是否成功\n
    /// If Success.
    /// </returns>
	virtual bool Open(MPDeviceInfo DeviceInfo) = 0;
	/// <summary>
    /// 关闭设备。\n
    /// Close device.
    /// </summary>
    /// <returns>
    /// 是否成功\n
    /// If Success.
    /// </returns>
	virtual bool Close() = 0;
	/// <summary>
    /// 手动重连设备。当AutoReconnect属性为true时，该函数会被自动调用。\n
    /// Reconnect device. If the AutoReconnect property is true, this function will be called automatically.
    /// </summary>
    /// <returns>
    /// 是否成功。\n
    /// If success.
    /// </returns>
	virtual bool Reconnect(void) = 0;	//if connection lost, use reconnect
	/// <summary>
    /// 设备是否已连接。\n
    /// If the device is connected.
	/// <returns>
    /// 是否连接。\n
    /// If connected.
    /// </returns>
    /// </summary>
	virtual bool IsConnected(void) = 0; 
	/// <summary>
    /// 获取当前已连接的设备信息.\n
    /// Get currently connected device info.
	/// <returns>
    /// 当前已连接的设备信息。\n
    /// Currently connected device info.
    /// </returns>
    /// </summary>
	virtual MPDeviceInfo GetCurrentDeviceInfo(void) = 0;
	/// <summary>
    /// 设置断线是否自动重连设备\n
    /// Set if reconnect the device automatically if disconnected.
    /// </summary>
	virtual void SetAutoReconnect(bool AutoReconnect) = 0;
	/// <summary>
    /// 获取是否通过USB3接口连接\n
    /// Get the state that if the device is connected via USB3.0
    /// </summary>
	virtual bool IsUSB3(void) = 0;

	/// <summary>
    /// 设置是否使能高级通讯协议\n
    /// Set the enable state of advanced protocal
    /// </summary>
	virtual bool SetAdvancedProtocalEnable(bool AdvancedProtocalEnable) = 0;


	//Device Change Callback
	/// <summary>
    /// 注册设备连接状态变化回调函数。\n
    /// Register callback function for device connection status changing.
    /// </summary>
    /// <param name="CallBackFn">
    /// \[输入\]:已定义的回调函数\n
    /// [INPUT]:Call back function.
    /// </param>
    /// <returns>
    /// 是否成功。\n
    /// If Success.
    /// </returns>
	virtual bool SetDeviceConnectionEventCallBack(MPSizectorConnectionEventCallBackType CallBackFn) = 0;

	//Diagnostics Callback
	/// <summary>
    /// 注册诊断信息回调函数。\n
    /// Register callback function for diagnostics.
    /// </summary>
    /// <param name="CallBackFn">
    /// \[输入\]:已定义的回调函数\n
    /// [INPUT]:Call back function.
    /// </param>
    /// <returns>
    /// 是否成功。\n
    /// If Success.
    /// </returns>
	virtual bool SetDiagnosticsCallBack(MPSizectorDiagnosticsCallBackType CallBackFn) = 0;

	//Request to send debug registers value through Diagnostics callback
	virtual bool RequestForDebugRegisters(void) = 0;

	//Capture Control

	/// <summary>
    /// 发送一次软件触发信号。\n
    /// Send software trigger to the device.
    /// </summary>
    /// <returns>
    /// 是否成功。\n
    /// If Success.
    /// </returns>
	virtual bool FireSoftwareTrigger(void) = 0;
	/// <summary>
    /// 进行一次同步3D拍摄，获取<b>非托管</b>3D数组。【注意】非托管指针需要编写非安全代码。\n
    /// Perform one-time synchronized "trigger-recieve data" process to get unmanaged 3D array.**Notice**Please write unsafe code clip to handle unmanaged array.
    /// </summary>
    /// <param name="SoftwareTrigger">
    /// \[输入\]:是否发送软件触发信号。\n
    /// [INPUT]: If use software trigger.
    /// </param>
    /// <param name="Buffer">
    /// \[输出\]:非托管数据缓存指针。\n
    /// [OUTPUT]: Unmanaged Pointer to acquired data buffer.
    /// </param>
    /// <param name="TimeOutMS">
    /// \[输入\]:设置超时时长(ms)\n
    /// [INPUT]: Set time-out in millisecond.
    /// </param>
    /// <returns>
    /// 是否成功。\n
    /// If success.
    /// </returns>
	virtual bool Snap3D(bool SoftwareTrigger, MP3DFrame* Buffer, unsigned int TimeOutMS) = 0;
    /// <summary>
    /// 进行一次同步2D拍摄，获取<b>非托管</b>2D数组。【注意】非托管指针需要编写非安全代码。\n
    /// Perform one-time synchronized "trigger-recieve data" process to get unmanaged 2D array.**Notice**Please write unsafe code clip to handle unmanaged array.
    /// </summary>
    /// <param name="SoftwareTrigger">
    /// \[输入\]:是否发送软件触发信号。\n
    /// [INPUT]: If use software trigger.
    /// </param>
    /// <param name="Buffer">
    /// \[输出\]:非托管数据缓存指针。\n
    /// [OUTPUT]: Managed Pointer to acquired data buffer.
    /// </param>
    /// <param name="TimeOutMS">
    /// \[输入\]:设置超时时长(ms)\n
    /// [INPUT]: Set time-out in millisecond.
    /// </param>
    /// <returns>
    /// 是否成功。\n
    /// If success.
    /// </returns>
	virtual bool Snap2D(bool SoftwareTrigger, MP2DFrame* Buffer, unsigned int TimeOutMS) = 0;

	//Callback Setting

	/// <summary>
    /// 为获取<b>非托管</b>3D数据注册回调函数，并分配数据缓冲区。【注意】非托管指针需要编写非安全代码。\n
    /// Register callback function for recieving unmanaged 3D data, and allocate buffer. **Notice**Please write unsafe code clip to handle unmanaged array.
    /// </summary>
    /// <param name="CallBackFn">
    /// \[输入\]:已定义的回调函数。\n
    /// [INPUT]:Defined callback function.
    /// </param>
    /// <param name="BufferSize">
    /// \[输入\]:缓存数组大小。 \n
    /// [INPUT]:Size of the buffer for storing data.
    /// </param>
    /// <returns>
    /// 获得的<b>非托管</b>3D数组指针\n
    /// Acquired UNMANAGED 3D array pointer.
    /// </returns>
	virtual MP3DFrame* Set3DDataCallBack(MPSizector3DDataCallBackType CallBackFn, unsigned char BufferSize) = 0;
	/// <summary>
    /// 为获取<b>非托管</b>3D数据注册回调成员函数指针，并分配数据缓冲区(之前调用分配的数据将删除，不可再访问)。【注意】非托管指针需要编写非安全代码。\n
    /// Register callback member function pointer for recieving unmanaged 3D data, and allocate buffer(previous allocated buffer will be deleted and can't be access any more). **Notice**Please write unsafe code clip to handle unmanaged array.
    /// </summary>
    /// <param name="CallBackFn_Ptr">
    /// \[输入\]:已定义的回调成员函数指针。\n
    /// [INPUT]:Defined callback member function pointer.
    /// </param>
	/// <param name="Obj_Ptr">
    /// \[输入\]:对象指针。\n
    /// [INPUT]:Object pointer.
    /// </param>
    /// <param name="BufferSize">
    /// \[输入\]:缓存数组大小。 \n
    /// [INPUT]:Size of the buffer for storing data.
    /// </param>
    /// <returns>
    /// 获得的<b>非托管</b>3D数组指针\n
    /// Acquired UNMANAGED 3D array pointer.
    /// </returns>
	template <class T> MP3DFrame* Set3DDataCallBack(void (T::*CallBackFn_Ptr)(unsigned char), T* Obj_Ptr, unsigned char BufferSize)
	{
		if (m_CallBack3DBase)
			delete m_CallBack3DBase;

		m_CallBack3DBase = new CallBack3DItem<T>(CallBackFn_Ptr, Obj_Ptr);
		return Set3DDataCallBack(m_CallBack3DBase, BufferSize);
	}
	/// <summary>
    /// 使能3D数据回调。 \n
    /// Enable 3D data recieved callback.
    /// </summary>
	virtual bool Enable3DDataCallBack(void) = 0;
	/// <summary>
    /// 禁用3D数据回调。 \n
    /// Disable 3D data recieved callback.
    /// </summary>
	virtual bool Disable3DDataCallBack(void) = 0;
	//2D Callback setting unmanaged. buffer is allocated in the C++ DLL
    /// <summary>
    /// 为获取<b>非托管</b>2D数据注册回调函数，并分配数据缓冲区。【注意】非托管指针需要编写非安全代码。\n
    /// Register callback function for recieving unmanaged 2D data, and allocate buffer.**Notice**Please write unsafe code clip to handle unmanaged array.
    /// </summary>
    /// <param name="CallBackFn">
    /// \[输入\]:已定义的回调函数。\n
    /// [INPUT]:Defined callback function.
    /// </param>
    /// <param name="BufferSize">
    /// \[输入\]:缓存数组大小。\n
    /// [INPUT]:Size of the buffer for storing data.
    /// </param>
    /// <returns>
    /// 获得的<b>非托管</b>2D数组\n
    /// Acquired MANAGED 2D array.
    /// </returns>
	virtual MP2DFrame* Set2DDataCallBack(MPSizector2DDataCallBackType CallBackFn, unsigned char BufferSize) = 0;
	//2D Callback setting unmanaged. buffer is allocated in the C++ DLL
    /// <summary>
    /// 为获取<b>非托管</b>2D数据注册回调成员函数指针，并分配数据缓冲区(之前调用分配的数据将删除，不可再访问)。【注意】非托管指针需要编写非安全代码。\n
    /// Register callback member function pointer for recieving unmanaged 2D data, and allocate buffer(previous allocated buffer will be deleted and can't be access any more).**Notice**Please write unsafe code clip to handle unmanaged array.
    /// </summary>
    /// <param name="CallBackFn_Ptr">
    /// \[输入\]:已定义的回调成员函数指针。\n
    /// [INPUT]:Defined callback member function pointer.
    /// </param>
	/// <param name="Obj_Ptr">
    /// \[输入\]:对象指针。\n
    /// [INPUT]:Object pointer.
    /// </param>
    /// <param name="BufferSize">
    /// \[输入\]:缓存数组大小。\n
    /// [INPUT]:Size of the buffer for storing data.
    /// </param>
    /// <returns>
    /// 获得的<b>非托管</b>2D数组\n
    /// Acquired MANAGED 2D array.
    /// </returns>
	template <class T> MP2DFrame* Set2DDataCallBack(void (T::*CallBackFn_Ptr)(unsigned char, unsigned char), T* Obj_Ptr, unsigned char BufferSize)
	{
		if (m_CallBack2DBase)
			delete m_CallBack2DBase;

		m_CallBack2DBase = new CallBack2DItem<T>(CallBackFn_Ptr, Obj_Ptr);
		return Set2DDataCallBack(m_CallBack2DBase, BufferSize);
	}
	/// <summary>
    /// 使能2D数据回调。 \n
    /// Enable 2D data recieved callback.
    /// </summary>
	virtual bool Enable2DDataCallBack(void) = 0;
	/// <summary>
    /// 禁用2D数据回调。 \n
    /// Disable 2D data recieved callback.
    /// </summary>
	virtual bool Disable2DDataCallBack(void) = 0;

	//Camera Setting

	/// <summary>
    /// 获取暂停状态。当此属性被设置为true时，设备将在执行完成最后一次拍摄传输之后，屏蔽包括软件触发、自动连续触发、硬件触发在内的一切触发信号。\n
    /// Get the hold state. If the HoldState property is set to be true, the device will refuse all kinds of trigger signals including software trigger, keep running trigger, hardware trigger after the last capture operation.
    /// </summary>
	virtual bool GetHoldState(bool* HoldState) = 0;
	/// <summary>
    /// 设置暂停状态。当此属性被设置为true时，设备将在执行完成最后一次拍摄传输之后，屏蔽包括软件触发、自动连续触发、硬件触发在内的一切触发信号。\n
    /// Set the hold state. If the HoldState property is set to be true, the device will refuse all kinds of trigger signals including software trigger, keep running trigger, hardware trigger after the last capture operation.
    /// </summary>
	virtual bool SetHoldState(bool HoldState) = 0;
	/// <summary>
    /// 获取设备的工作模式。可设置设备工作在3D拍摄或2D拍摄模式下。详见WorkingModeType结构体说明。更改工作模式后的首次拍摄将需要额外的2s等待时间。
    /// Get the working mode of device. The device can be set to work in 3D mode or 2D mode. See the description of structure WorkingModeType. The first snap after changing working mode will consume additional 2s waiting time.
    /// </summary>
	virtual bool GetWorkingMode(WorkingModeType* WorkingMode) = 0;
	/// <summary>
    /// 设置设备的工作模式。可设置设备工作在3D拍摄或2D拍摄模式下。详见WorkingModeType结构体说明。更改工作模式后的首次拍摄将需要额外的2s等待时间。
    /// Set the working mode of device. The device can be set to work in 3D mode or 2D mode. See the description of structure WorkingModeType. The first snap after changing working mode will consume additional 2s waiting time.
    /// </summary>
	virtual bool SetWorkingMode(WorkingModeType WorkingMode) = 0;
	/// <summary>
    /// 获取设备照明强度。范围从0（最暗）至255（最亮）\n
    /// Get the light projection intensity. Range from 0(no light) to 255(most bright).
    /// </summary>
	virtual bool GetLEDCurrent(unsigned char* LEDCurrent) = 0;
	/// <summary>
    /// 设置设备照明强度。范围从0（最暗）至255（最亮）\n
    /// Set the light projection intensity. Range from 0(no light) to 255(most bright).
    /// </summary>
	virtual bool SetLEDCurrent(unsigned char LEDCurrent) = 0;
	/// <summary>
    /// 获取曝光时长(us)。建议的曝光时长为500~10000us。当照明强度设置为最大值仍然过暗时，应增大曝光时长。注意，更改曝光时长之后的第一次拍摄，将需要额外的2s等待时间。\n
    /// Get the exposure time of the camera in microsecond. The first snap after changing exposure time will need additional 2s waiting time.
    /// </summary>
	virtual bool GetExposureTime(unsigned short* ExposureTime) = 0;
	/// <summary>
    /// 设置曝光时长(us)。建议的曝光时长为500~10000us。当照明强度设置为最大值仍然过暗时，应增大曝光时长。注意，更改曝光时长之后的第一次拍摄，将需要额外的2s等待时间。\n
    /// Set the exposure time of the camera in microsecond. The first snap after changing exposure time will need additional 2s waiting time.
    /// </summary>
	virtual bool SetExposureTime(unsigned short ExposureTime) = 0;
	/// <summary>
    /// 获取设备的触发方式，可设置为仅软件触发、自动连续触发、外部触发。 详见TriggerSourceType说明。\n
    /// Get the trigger mode of the device. Can be configured to triggered by software trigger only, keep running auto trigger and external trigger. See the illustration of TriggerSourceType for more info.
    /// </summary>
	virtual bool GetTriggerSource(TriggerSourceType* TriggerSource) = 0;
	/// <summary>
    /// 设置设备的触发方式，可设置为仅软件触发、自动连续触发、外部触发。 详见TriggerSourceType说明。\n
    /// Set the trigger mode of the device. Can be configured to triggered by software trigger only, keep running auto trigger and external trigger. See the illustration of TriggerSourceType for more info.
    /// </summary>
	virtual bool SetTriggerSource(TriggerSourceType TriggerSource) = 0;
	/// <summary>
    /// 获取输出管脚的特殊功能使能设置。以位存储，1为使能，0为禁用。\n
    /// Bit 0: Output 0管脚特殊功能使能状态。特殊功能为设备初始化完成。该Bit使能后，Output 0的输出为：0为未完成初始化，1为完成初始化。\n
    /// Bit 1: Output 1管脚特殊功能使能状态。特殊功能为运行状态。该Bit使能后，Output 1的输出为：0为空闲，1为忙碌（包括曝光、计算和传输过程）。\n
    /// Bit 2: Output 2管脚特殊功能使能状态。特殊功能为曝光状态。该Bit使能后，Output 2的输出为：0为当前不在曝光，1为当前在曝光。 \n
    /// 
    /// Get the special function enable state of output pins. Stored in bits. Each bit stands for the enable state of the special function for each pin, 1 for enable, 0 for disable.\n
    /// Bit 0: Output 0 special function(initialization state) enable state. If this bit is set to 1, the output value of Output 0 is: 0 for initialization not finished, 1 for finished. \n
    /// Bit 1: Output 1 special function(running state) enable state. If this bit is set to 1, the output value of Output 1 is: 0 for idle, 1 for busy (including exposure, calculation and data transfer. \n
    /// Bit 2: Output 2 special function(exposure state) enable state. If this bit is set to 1, the output value of Output 2 is: 0 for not under exposure, 1 for under exposure.
    /// </summary>
	virtual bool GetOutputSpecialFunction(unsigned char* SpecialFunctionActivated) = 0;
	/// <summary>
    /// 设置输出管脚的特殊功能使能设置。以位存储，1为使能，0为禁用。\n
    /// Bit 0: Output 0管脚特殊功能使能状态。特殊功能为设备初始化完成。该Bit使能后，Output 0的输出为：0为未完成初始化，1为完成初始化。\n
    /// Bit 1: Output 1管脚特殊功能使能状态。特殊功能为运行状态。该Bit使能后，Output 1的输出为：0为空闲，1为忙碌（包括曝光、计算和传输过程）。\n
    /// Bit 2: Output 2管脚特殊功能使能状态。特殊功能为曝光状态。该Bit使能后，Output 2的输出为：0为当前不在曝光，1为当前在曝光。 \n
    /// 
    /// Set the special function enable state of output pins. Stored in bits. Each bit stands for the enable state of the special function for each pin, 1 for enable, 0 for disable.\n
    /// Bit 0: Output 0 special function(initialization state) enable state. If this bit is set to 1, the output value of Output 0 is: 0 for initialization not finished, 1 for finished. \n
    /// Bit 1: Output 1 special function(running state) enable state. If this bit is set to 1, the output value of Output 1 is: 0 for idle, 1 for busy (including exposure, calculation and data transfer. \n
    /// Bit 2: Output 2 special function(exposure state) enable state. If this bit is set to 1, the output value of Output 2 is: 0 for not under exposure, 1 for under exposure.
    /// </summary>
	virtual bool SetOutputSpecialFunction(unsigned char SpecialFunctionActivated) = 0;
	/// <summary>
    /// 获取由用户控制的输出管脚的状态，以位的形式存储。\n
    /// Bit X: Output X管脚的输出状态。0为关断(高阻态)，1为导通。\n
    /// 注意，当对应管脚的特殊功能被使能后，此属性中该管脚对应的位的值将与输出状态无关（包括读和写 )。\n
    /// 
    /// Get the state of output pins controlled by user. Stored in bits.\n
    /// Bit X: The state of Output X pin. 0 stands for disable(high resistance), 1 stands for enable(conducting).\n
    /// Note, when the special function is activated for specific pin, the value of this pin in "Output" property is nolonger related with the real state of the pin.
    /// </summary>
	virtual bool GetOutput(unsigned char* State) = 0;
	/// <summary>
    /// 设置由用户控制的输出管脚的状态，以位的形式存储。\n
    /// Bit X: Output X管脚的输出状态。0为关断(高阻态)，1为导通。\n
    /// 注意，当对应管脚的特殊功能被使能后，此属性中该管脚对应的位的值将与输出状态无关（包括读和写 )。\n
    /// 
    /// Set the state of output pins controlled by user. Stored in bits.\n
    /// Bit X: The state of Output X pin. 0 stands for disable(high resistance), 1 stands for enable(conducting).\n
    /// Note, when the special function is activated for specific pin, the value of this pin in "Output" property is nolonger related with the real state of the pin.
    /// </summary>
	virtual bool SetOutput(unsigned char State) = 0;
	/// <summary>
    /// 获取输入管脚的值，以位的形式存储。\n
    /// Bit X: Input X管脚的当前值。0为无输入(低电平），1为有输入(高电平)。\n
    /// 
    /// Get the value of input pins. Stored in bits.\n
    /// Bit X: The current value of Input X pin. 0 for no input(low voltage level). 1 for input signal detected(high voltage level).
    /// </summary>
	virtual bool GetInput(unsigned char* State) = 0;
	/// <summary>
    /// 设备软重启。启动过程将消耗约10s时间，并将默认设置覆盖当前设置。\n
    /// Soft reset the device. The soft reset procedure takes about 10s. Current setting will be replaced by default setting after soft reset.
    /// </summary>
    /// <returns>
    /// 是否成功。\n
    /// If success.
    /// </returns>
	virtual bool SoftwareReset(void) = 0;
	/// <summary>
    /// 获取设备是否已经初始化完毕。\n
    /// Get the state that if the device has completed initialization.
    /// </summary>
	virtual bool GetInitState(bool* IsInitDone) = 0;
	/// <summary>
    /// 获取设备的初始化状态编码。\n
    /// Get the state code initialization.
    /// </summary>
	virtual bool GetInitStateCode(unsigned int* StateCode) = 0;
	/// <summary>
    /// 获取设备是否正在忙碌。\n
    /// Get the state if the device is busy.
    /// </summary>
	virtual bool GetDeviceState(unsigned int* State) = 0;
	/// <summary>
    /// 获取修改3D重构算法中"有效点阈值"参数。有效点阈值为判断3D数据点是否有效的阈值。更大的有效点阈值将滤除更多的飞点和毛刺，更小的有效点阈值将提升图像的拍摄完整度。建议的有效点阈值的设置范围为3~50 \n
    /// 注意，调用SaveDefaultSettings后，会将该参数写入默认值。下次设备上电时将加载上次保存的默认值。\n
    /// Get the "valid point threshold" parameter in reconstruction algorithm. The valid point threshold is the threshold to judge whether a 3D data point is valid or not. Larger valid point threshold will filter more unreliable data points, while smaller valid point threshold will result a more complete 3D result. It is susggested to set this value between 3~50. 
    /// Note, the value of this property will be saved in FlashROM of device as default value after called SaveDefaultSettings. The initial value of this property will be loaded from FlashROM next time power up.
    /// </summary>
	virtual bool GetValidPointThreshold(unsigned char* ValidPointThreshold) = 0;
	/// <summary>
    /// 设置修改3D重构算法中"有效点阈值"参数。有效点阈值为判断3D数据点是否有效的阈值。更大的有效点阈值将滤除更多的飞点和毛刺，更小的有效点阈值将提升图像的拍摄完整度。建议的有效点阈值的设置范围为3~50 \n
    /// 注意，调用SaveDefaultSettings后，会将该参数写入默认值。下次设备上电时将加载上次保存的默认值。\n
    /// Set the "valid point threshold" parameter in reconstruction algorithm. The valid point threshold is the threshold to judge whether a 3D data point is valid or not. Larger valid point threshold will filter more unreliable data points, while smaller valid point threshold will result a more complete 3D result. It is susggested to set this value between 3~50. 
    /// Note, the value of this property will be saved in FlashROM of device as default value after called SaveDefaultSettings. The initial value of this property will be loaded from FlashROM next time power up.
    /// </summary>
	virtual bool SetValidPointThreshold(unsigned char ValidPointThreshold) = 0;
	/// <summary>
    /// 获取过曝滤除阈值。该参数决定因过曝而剔除点的过曝程度阈值。 \n
	/// 有效值为： （1）0~16， 指定的值，值越大，剔除的过曝光点越多，值为0时，不剔除过曝点。（2）254，使用针对不同算法的默认值。（3）255，关闭过曝剔除功能，不剔除过曝点。
	/// 无效值将导致设置操作失败。
    /// 注意，调用SaveDefaultSettings后，会将该参数写入默认值。下次设备上电时将加载上次保存的默认值。\n
    /// Get the over exposure filter threshold. This parameter determines the over exposure degree threshold value for removing over exposure data points. \n
	/// Validate values: (1) 0~16, specified threshold value. Higher value will result more high exposure points being removed, while 0 will not remove any. (2) 254, default value automatically applied for different algorithms. (3) 255, functionality disabled, do not remove any over exposure point.
    /// Other input value will result the setting operation fail.
	/// Note, the value of this property will be saved in FlashROM of device as default value after called SaveDefaultSettings. The initial value of this property will be loaded from FlashROM next time power up.
    /// </summary>
	virtual bool GetOverExposureFilterThreshold(unsigned char* OverExposureFilterThreshold) = 0;
	/// <summary>
    /// 设置过曝滤除阈值。该参数决定因过曝而剔除点的过曝程度阈值。 \n
	/// 有效值为： （1）0~16， 指定的值，值越大，剔除的过曝光点越多，值为0时，不剔除过曝点。（2）254，使用针对不同算法的默认值。（3）255，关闭过曝剔除功能，不剔除过曝点。
	/// 无效值将导致设置操作失败。
    /// 注意，调用SaveDefaultSettings后，会将该参数写入默认值。下次设备上电时将加载上次保存的默认值。\n
    /// Set the over exposure filter threshold. This parameter determines the over exposure degree threshold value for removing over exposure data points. \n
	/// Validate values: (1) 0~16, specified threshold value. Higher value will result more high exposure points being removed, while 0 will not remove any. (2) 254, default value automatically applied for different algorithms. (3) 255, functionality disabled, do not remove any over exposure point.
    /// Other input value will result the setting operation fail.
	/// Note, the value of this property will be saved in FlashROM of device as default value after called SaveDefaultSettings. The initial value of this property will be loaded from FlashROM next time power up.
    /// </summary>
	virtual bool SetOverExposureFilterThreshold(unsigned char OverExposureFilterThreshold) = 0;
	/// <summary>
    /// 获取预处理迭代次数。预处理包含滤波、祛除飞点，以及修补空洞的功能，迭代次数越多，效果越显著。增加预处理次数可显著提高重复性。该参数范围为0~5，设置为范围外的值将导致设置失败。 \n
    /// Get the loop number of pre-process. Pre-process include the functionality of filter, remove blur, and mending. Higher loop number will result more notable effect. \n
	/// Increase the loop number can also provide better repeatance. The range of this parameter is between 0 to 5. Setting this parameter out of range will result failure of setting.
    /// </summary>
	virtual bool GetPreProcessLoopNum(unsigned char* PreProcessLoopNum) = 0;
	/// <summary>
    /// 设置预处理迭代次数。预处理包含滤波、祛除飞点，以及修补空洞的功能，迭代次数越多，效果越显著。增加预处理次数可显著提高重复性。该参数范围为0~5，设置为范围外的值将导致设置失败。 \n
    /// Set the loop number of pre-process. Pre-process include the functionality of filter, remove blur, and mending. Higher loop number will result more notable effect. \n
	/// Increase the loop number can also provide better repeatance. The range of this parameter is between 0 to 5. Setting this parameter out of range will result failure of setting.
    /// </summary>
	virtual bool SetPreProcessLoopNum(unsigned char PreProcessLoopNum) = 0;

	//Default Settings, must call SaveDefaultSettings after changed. The saving process takes about 2s. 
	/// <summary>
    /// 将默认设置从FlashROM中读取到设备的默认值寄存器中。设备启动时会自行执行该命令。\n
    /// Load default values from FlashROM to device default value registers. This command is automatically executed when device power up.
    /// <returns>
    /// 是否成功。\n
    /// If success.
    /// </returns>
    /// </summary>
	virtual bool LoadDefaultSettings(void) = 0;
	/// <summary>
    /// 将设备的默认值寄存器中的值写入FlashROM。注意，当用户修改默认值寄存器后，必须调用该函数，才能将默认值保存。进行多个默认值寄存器的更改，只需要在所有的默认值寄存器更改结束后调用该函数一次。 \n
    /// Save the default value registers into FlashROM. This function must be called to save the default values after user changed the default value. If multiple default values are changed, this function only need to be called once after all changes done.
    /// <returns>
    /// 是否成功。\n
    /// If success.
    /// </returns>
    /// </summary>

	virtual bool SaveDefaultSettings(void) = 0;
	/// <summary>
    /// 获取设备默认暂停状态。\n
    /// Get the default hold state of device.
    /// </summary>
	virtual bool GetDefaultHoldState(bool* HoldState) = 0;
	/// <summary>
    /// 设置设备默认暂停状态。\n
    /// Set the default hold state of device.
    /// </summary>
	virtual bool SetDefaultHoldState(bool HoldState) = 0;
	/// <summary>
    /// 获取设备默认工作模式\n
    /// Get the default working mode of device.
    /// </summary>
	virtual bool GetDefaultWorkingMode(WorkingModeType* WorkingMode) = 0;
	/// <summary>
    /// 设置设备默认工作模式\n
    /// Set the default working mode of device.
    /// </summary>
	virtual bool SetDefaultWorkingMode(WorkingModeType WorkingMode) = 0;
	/// <summary>
    /// 获取设备默认照明强度。\n
    /// Get the default light projection intensity of device.
    /// </summary>
	virtual bool GetDefaultLEDCurrent(unsigned char* LEDCurrent) = 0;
	/// <summary>
    /// 设置设备默认照明强度。\n
    /// Set the default light projection intensity of device.
    /// </summary>
	virtual bool SetDefaultLEDCurrent(unsigned char LEDCurrent) = 0;
	/// <summary>
    /// 获取设备默认曝光时长(us)。\n
    /// Get the default exposure time of device (in us).
    /// </summary>
	virtual bool GetDefaultExposureTime(unsigned short* ExposureTime) = 0;
	/// <summary>
    /// 设置设备默认曝光时长(us)。\n
    /// Set the default exposure time of device (in us).
    /// </summary>
	virtual bool SetDefaultExposureTime(unsigned short ExposureTime) = 0;
	/// <summary>
    /// 获取设备默认触发方式。 \n
    /// Get he default trigger mode of device.
    /// </summary>
	virtual bool GetDefaultTriggerSource(TriggerSourceType* TriggerSource) = 0;
	/// <summary>
    /// 设置设备默认触发方式。 \n
    /// Set he default trigger mode of device.
    /// </summary>
	virtual bool SetDefaultTriggerSource(TriggerSourceType TriggerSource) = 0;
	/// <summary>
    /// 获取输出管脚特殊功能使能默认值。\n
    /// Get the default value of the special function activated states of output pins.
    /// </summary>
	virtual bool GetDefaultOutputSpecialFunction(unsigned char* SpecialFunctionActivated) = 0;
	/// <summary>
    /// 设置输出管脚特殊功能使能默认值。\n
    /// Set the default value of the special function activated states of output pins.
    /// </summary>
	virtual bool SetDefaultOutputSpecialFunction(unsigned char SpecialFunctionActivated) = 0;
	/// <summary>
    ///  获取输出管脚的默认输出状态。设置为特殊功能的管脚不受该属性控制。\n
    ///  Get the default states of output pins. Pins configured to be special function is not controlled by this property.
    /// </summary>
	virtual bool GetDefaultOutput(unsigned char* State) = 0;
	/// <summary>
    ///  设置输出管脚的默认输出状态。设置为特殊功能的管脚不受该属性控制。\n
    ///  Set the default states of output pins. Pins configured to be special function is not controlled by this property.
    /// </summary>
	virtual bool SetDefaultOutput(unsigned char State) = 0;

	//virtual ~IMPSizector() //虚析构函数在.NET调用时报异常
	void Free()
	{
		if (m_CallBack3DBase) delete m_CallBack3DBase;
		if (m_CallBack2DBase) delete m_CallBack2DBase;
	}

protected:
	virtual MP2DFrame* Set2DDataCallBack(CallBack2DBase* CallBack, unsigned char BufferSize) = 0;
	virtual MP3DFrame* Set3DDataCallBack(CallBack3DBase* CallBack, unsigned char BufferSize) = 0;

private:
	CallBack3DBase* m_CallBack3DBase;
	CallBack2DBase* m_CallBack2DBase;
};

//Factory for new MPSizector
/// <summary>
/// MPSizector的工厂类, 用于产生实例。\n
/// Factory class of MPSizector, used to generate instance.
/// </summary>
class DLL_EXPORT MPSizectorFactory
{
public:
	/// 获取一个新的MPSizector实例。\n
	/// Get a new instance of MPSizector.
	static IMPSizector* GetInstance();

	/// 析构一个MPSizector实例 \n
	/// Destruct MPSizector instance.
	static void DestructInstance(IMPSizector* Instance)
	{
		//因为回调对象在动态库外创建，因此释放也要在动态库外，而不应该在_DestructInstance（会调用动态库）中释放。
		Instance->Free();
		_DestructInstance(Instance);
	}

private:
	static void _DestructInstance(IMPSizector* Instance);
};

//Utils
/// <summary>
/// Utils类, 包含后处理及数据存取。\n
/// Utils class, including some post process method and data reading and writing .
/// </summary>
class DLL_EXPORT Utils
{
public:
	/// <summary>
    /// 保存此帧数据到本地文件，数据格式为二进制。\n
    /// Save the instance as binary in local file.
    /// </summary>
	/// <param name="Frame">3D帧数据。 \n 3D Frame.</param>
    /// <param name="FileName">文件名称。 \n File name string.</param>
    /// <returns>是否成功。\n If success.</returns>
    static bool Save(MP3DFrame* Frame, char* FileName);

	/// <summary>
    /// 从本地文件读取一帧数据。\n
    /// Load 3D frame data from local file.
    /// </summary>
	/// <param name="Frame">3D帧数据。 \n 3D Frame.</param>
    /// <param name="FileName">文件名称 \n File name</param>
    /// <returns>是否成功。\n If success.</returns>
    static bool Load(MP3DFrame* Frame, char* FileName);

	/// <summary>
    /// 保存此帧数据到本地文件，数据格式为三维点云。\n
    /// Save the instance as point cloud in local file. 
    /// </summary>
	/// <param name="Frame">3D帧数据。 \n 3D Frame.</param>
    /// <param name="FileName">文件名称 \n File name</param>
    /// <returns>是否成功。\n If success.</returns>
    static bool Save3DPointCloud(MP3DFrame* Frame, char* FileName);

	/// <summary>
	/// 后处理，降噪。\n
    /// Post process, filtrate the 3D data to reduce noise.
	/// </summary>
	static void PostProcessFiltrate(MP3DFrame* Frame);

	/// <summary>
	/// 后处理，祛除飞点。\n
    /// Post process, remove burrs from the 3D data.
	/// </summary>
	static void PostProcessRemoveBurrs(MP3DFrame* Frame);

	/// <summary>
    /// 后处理，修补缺损点。\n
    /// Post process, mend holes.
    /// </summary>
    static void PostProcessMend(MP3DFrame* Frame);

	//convert 3D gray to bitmap
    /// <summary>
    /// 将3D拍摄数据的灰度(Gray)信息转换成灰度Bitmap。 \n Convert MP3DFrameManaged to grayscale bitmap according to its points' Gray value.
    /// </summary>
	/// <param name="Frame">3D帧数据。 \n 3D Frame.</param>
    /// <param name="BitmapBuffer">Bitmap缓存，缓存大小应等于3D帧数据尺寸 \n Bitmap buffer, its size should equal to 3D frame size</param>
    static void GetGrayBitmap(MP3DFrame* Frame, unsigned char* BitmapBuffer);

	//convert 3D X,Y,Z to bitmap
    /// <summary>
    /// 将3D拍摄数据按其坐标信息转换成彩色Bitmap。 \n Convert MP3DFrameManaged to RGB bitmap according to its points' dimension value.
    /// </summary>
	/// <param name="Frame">3D帧数据。 \n 3D Frame.</param>
    /// <param name="RedBuffer">Red缓存，缓存大小应等于3D帧数据尺寸 \n Red buffer, its size should equal to 3D frame size</param>
	/// <param name="GreenBuffer">Red缓存，缓存大小应等于3D帧数据尺寸 \n Green buffer, its size should equal to 3D frame size</param>
	/// <param name="BlueBuffer">Red缓存，缓存大小应等于3D帧数据尺寸 \n Blue buffer, its size should equal to 3D frame size</param>
    /// <param name="Target">\[输入\]:转换的坐标类型(X,Y,Z)。例如，按高度信息转换，则输入Convert3DTargetType.Z。 \n [INPUT]:Converting type with respect to the dimension. For example, if you want to show the height information in 2D depth image, choose Convert3DTargetType.Z.</param>
    /// <param name="Max">\[输入\]:最大值。当坐标超过最大值，将显示过饱和颜色(TooHighColor)。 \n [INPUT]:Max value of the source. If the source value is larger than the max value, the point will display as over-saturated (TooHighColor).</param>
    /// <param name="Min">\[输入\]:最小值。当坐标不足最小值，将显示欠饱和颜色(TooLowColor)。 \n [INPUT]:Min value of the source. If the source value is less than the max value, the point will display as under-saturated (TooLowColor).</param>
    static void GetColorDepthBitmap(MP3DFrame* Frame, unsigned char* RedBuffer, unsigned char* GreenBuffer, unsigned char* BlueBuffer,
		Convert3DTargetType Target, float Max, float Min);

	/// <summary>
    /// 平面去畸变工具。拍摄样本之前，需要首先拍摄标准平面，获得平面畸变相关信息。将3D数据与已知标准平面拍摄数据对比，纠正拍摄畸变。 \n
    /// Planar Distortion rectifying. Before scanning the sample, scanning a standard plane is neccessary to acquire the data of distortion 
    /// caused by camera. Then by comparing the scanned data with the distorted plane data, the distortion can be thus rectified.
    /// </summary>
    /// <param name="Frame3D">\[输入\]:拍摄样本数据。 \n [INPUT]: Scanned data of the sample</param>
    /// <param name="RefPlan">\[输入\]:拍摄标准平面数据。\n [INPUT]: Scanned data of the standard plane.</param>
    static void RefPlanRectify(MP3DFrame* Frame3D, MP3DFrame* RefPlan);

	//calculate one 3D from multiple 3D
    /// <summary>
    /// 多帧拍摄的3D数据融合成一帧。遍历多张图像的点，根据选择把位置相同的点取多帧数据平均值，或者按灰度图中对应位置的最亮的那一帧的值。此工具常用于不同亮度的多次曝光方法。融合的新数据保存在Input数组的第一帧3D中\n
    /// Fuse mutltiple frames into one frame. Traverse the positions of all frames, assign the average value, or the value of in the frame that has the brightest value in the grayscale domain, according to the user. 
    /// This function is designed for multiple exposure method with various LED currents. The fused data is saved into the first frame of Input array.
    /// </summary>
    /// <param name="FrameNum">\[输入\]:需要融合的3D数据帧数。\n [INPUT]:Number of the frame of the data.</param>
    /// <param name="Input">\[输入\]:需要融合的3D数据数组。\n [INPUT]: Array of frames of the data.</param>
    /// <param name="Image2DSourceIndex">\[输入\]:新数据的灰度值需要采用的帧的下标。\n [INPUT]: The index that used to assign the 2D grayscale value to the new frame.</param>
    /// <param name="AverageAllValidPoints">\[输入\]:是否将所有帧取平均。 \n [INPUT]: If assgin average value to the new frame.</param>
    static void MultiExposureFusion(int FrameNum, MP3DFrame** Input,int Image2DSourceIndex, bool AverageAllValidPoints);

	//hight measurement, drop ratio is the ratio dropeed, for bot highest and lowest. return 0 on wrong parameter
    /// <summary>
    /// 测量高度。选取一片方形区域，获取区域内点云相对于零平面的绝对高度。选取区域内的有效点后，会按比例分别去除最高点和最低点，位于中间的有效点进行平均并返回。 \n
    /// Height Measurement. Selecting a rectangular area, measure the absolute height due to the zero plane. When selecting the valid points in the area, the highest and lowest points will be excluded by some ratio, and calculate the average height.
    /// </summary>
    /// <param name="Frame3D">\[输入\]:3D拍摄数据。\n [INPUT]:The scanned 3D data.</param>
    /// <param name="TopLeftX">\[输入\]:区域左上角的X坐标(像素)。\n [INPUT]:The X position in pixels of the top-left corner.</param>
    /// <param name="TopLeftY">\[输入\]:区域左上角的Y坐标(像素)。\n [INPUT]:The Y position in pixels of the top-left corner.</param>
    /// <param name="BottomRightX">\[输入\]:区域右下角的X坐标(像素)。\n [INPUT]:The X position in pixels of the bottom-right corner.</param>
    /// <param name="BottomRightY">\[输入\]:区域右下角的Y坐标(像素)。\n [INPUT]:The Y position in pixels of the bottom-right corner.</param>
    /// <param name="DropRatio">\[输入\]:需要去除的点的比例。去除的最高部分和最低部分相同。也就是说剩下的点数量为"总数*(1 - DropRatio*2)"。\n 
    /// [INPUT]:The ratio of the points that are about to exclude. The top part and lower part are equally ignored by this ratio, i.e., the number of remaining points is TotalNum*(1 - DropRatio*2).</param>
    /// <returns>区域的平均高度。 \n Average height of the area.</returns>
    static float MeasureZ(MP3DFrame* Frame3D, unsigned int TopLeftX, unsigned int TopLeftY, unsigned int BottomRightX, unsigned int BottomRightY, float DropRatio);

	//Convert 3D frame to range image
    /// <summary>
    /// 将MP3DFrame数据转换为RangeImage图像数据。 \n Convert MP3DFrame data to RangeImage image data.
    /// </summary>
	/// <param name="src_frame">3D帧数据。 \n 3D Frame.</param>
    /// <param name="dst_image">RangeImage指针，需先初始化\n RangeImage pointer, should already be initialized</param>
	static void ConvertToRangeImage(MP3DFrame* src_frame, RangeImage* dst_image);
};

extern "C"
{
	/// 获取API版本号
	/// Get API Version
	DLL_EXPORT float GetSizectorAPIVersion();

	//Create and Delete

	/// C语言风格调用函数，获取一个新的MPSizector实例。\n
	/// C-style function. Get a new instance of MPSizector. \n
	DLL_EXPORT IMPSizector* MPSizector_Create();
	/// C语言风格调用函数，删除一个MPSizector实例。\n
	/// C-style function. Delete a MPSizector instance. \n
	DLL_EXPORT void MPSizector_Delete(IMPSizector* pIMPSizector);

	//Device Managing

	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_UpdateDeviceList(IMPSizector* pIMPSizector);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT int MPSizector_GetDeviceCount(IMPSizector* pIMPSizector);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT void MPSizector_GetDeviceInfo(IMPSizector* pIMPSizector,MPDeviceInfo* pMPDeviceInfo, int Index);

	//Device connection

	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_Open(IMPSizector* pIMPSizector, MPDeviceInfo DeviceInfo);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_Close(IMPSizector* pIMPSizector);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_Reconnect(IMPSizector* pIMPSizector);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_IsConnected(IMPSizector* pIMPSizector);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT void MPSizector_GetCurrentDeviceInfo(IMPSizector* pIMPSizector,MPDeviceInfo* pMPDeviceInfo);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT void MPSizector_SetAutoReconnect(IMPSizector* pIMPSizector, bool AutoReconnect);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_IsUSB3(IMPSizector* pIMPSizector);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_SetAdvancedProtocalEnable(IMPSizector* pIMPSizector, bool AdvancedProtocalEnable);

	//Device Change Callback
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_SetDeviceConnectionChangeCallBack(IMPSizector* pIMPSizector, MPSizectorConnectionEventCallBackType CallBackFn);

	//Diagno
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_SetDiagnosticsCallBack(IMPSizector* pIMPSizector, MPSizectorDiagnosticsCallBackType CallBackFn);

	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_RequestForDebugRegisters(IMPSizector* pIMPSizector);

	//Capture Control
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_FireSoftwareTrigger(IMPSizector* pIMPSizector);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_Snap3D(IMPSizector* pIMPSizector, bool SoftwareTrigger, MP3DFrame* Buffer, unsigned int TimeOutMS);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_Snap2D(IMPSizector* pIMPSizector, bool SoftwareTrigger, MP2DFrame* Buffer, unsigned int TimeOutMS);

	//Callback Setting
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT MP3DFrame* MPSizector_Set3DDataCallBack(IMPSizector* pIMPSizector, MPSizector3DDataCallBackType CallBackFn, unsigned char BufferSize);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_Enable3DDataCallBack(IMPSizector* pIMPSizector);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_Disable3DDataCallBack(IMPSizector* pIMPSizector);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT MP2DFrame* MPSizector_Set2DDataCallBack(IMPSizector* pIMPSizector, MPSizector2DDataCallBackType CallBackFn, unsigned char BufferSize);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_Enable2DDataCallBack(IMPSizector* pIMPSizector);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_Disable2DDataCallBack(IMPSizector* pIMPSizector);


	//Camera Setting
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_GetHoldState(IMPSizector* pIMPSizector, bool* HoldState);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_SetHoldState(IMPSizector* pIMPSizector, bool HoldState);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_GetWorkingMode(IMPSizector* pIMPSizector, WorkingModeType* WorkingMode);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_SetWorkingMode(IMPSizector* pIMPSizector, WorkingModeType WorkingMode);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_GetLEDCurrent(IMPSizector* pIMPSizector, unsigned char* LEDCurrent);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_SetLEDCurrent(IMPSizector* pIMPSizector, unsigned char LEDCurrent);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_GetExposureTime(IMPSizector* pIMPSizector, unsigned short* ExposureTime);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_SetExposureTime(IMPSizector* pIMPSizector, unsigned short ExposureTime);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_GetTriggerSource(IMPSizector* pIMPSizector, TriggerSourceType* TriggerSource);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_SetTriggerSource(IMPSizector* pIMPSizector, TriggerSourceType TriggerSource);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_GetOutputSpecialFunction(IMPSizector* pIMPSizector, unsigned char* SpecialFunctionActivated);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_SetOutputSpecialFunction(IMPSizector* pIMPSizector, unsigned char SpecialFunctionActivated);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_GetOutput(IMPSizector* pIMPSizector, unsigned char* State);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_SetOutput(IMPSizector* pIMPSizector, unsigned char State);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_GetInput(IMPSizector* pIMPSizector, unsigned char* State);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_SoftwareReset(IMPSizector* pIMPSizector);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_GetInitState(IMPSizector* pIMPSizector, bool* IsInitDone);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_GetInitStateCode(IMPSizector* pIMPSizector, unsigned int* StateCode);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_GetDeviceState(IMPSizector* pIMPSizector, unsigned int* State);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_GetValidPointThreshold(IMPSizector* pIMPSizector, unsigned char* ValidPointThreshold);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_SetValidPointThreshold(IMPSizector* pIMPSizector, unsigned char ValidPointThreshold);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n	
	DLL_EXPORT bool MPSizector_GetOverExposureFilterThreshold(IMPSizector* pIMPSizector, unsigned char* OverExposureFilterThreshold);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_SetOverExposureFilterThreshold(IMPSizector* pIMPSizector, unsigned char OverExposureFilterThreshold);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_GetPreProcessLoopNum(IMPSizector* pIMPSizector, unsigned char* PreProcessLoopNum);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_SetPreProcessLoopNum(IMPSizector* pIMPSizector, unsigned char PreProcessLoopNum);

	//Default Settings, must call SaveDefaultSettings after changed. The saving process takes about 2s. 
	//Note that the valid point threshold default value is the same regester used in camera setting. 
	//So, if you want to change the valid point threshold default value, just call SetValidPointThreshold, then call SaveDefaultSettings
	
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_LoadDefaultSettings(IMPSizector* pIMPSizector);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_SaveDefaultSettings(IMPSizector* pIMPSizector);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_GetDefaultHoldState(IMPSizector* pIMPSizector, bool* HoldState);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_SetDefaultHoldState(IMPSizector* pIMPSizector, bool HoldState);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_GetDefaultWorkingMode(IMPSizector* pIMPSizector, WorkingModeType* WorkingMode);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_SetDefaultWorkingMode(IMPSizector* pIMPSizector, WorkingModeType WorkingMode);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_GetDefaultLEDCurrent(IMPSizector* pIMPSizector, unsigned char* LEDCurrent);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_SetDefaultLEDCurrent(IMPSizector* pIMPSizector, unsigned char LEDCurrent);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_GetDefaultExposureTime(IMPSizector* pIMPSizector, unsigned short* ExposureTime);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_SetDefaultExposureTime(IMPSizector* pIMPSizector, unsigned short ExposureTime);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_GetDefaultTriggerSource(IMPSizector* pIMPSizector, TriggerSourceType* TriggerSource);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_SetDefaultTriggerSource(IMPSizector* pIMPSizector, TriggerSourceType TriggerSource);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_GetDefaultOutputSpecialFunction(IMPSizector* pIMPSizector, unsigned char* SpecialFunctionActivated);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_SetDefaultOutputSpecialFunction(IMPSizector* pIMPSizector, unsigned char SpecialFunctionActivated);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_GetDefaultOutput(IMPSizector* pIMPSizector, unsigned char* State);
	/// C语言风格调用函数，参考IMPSizector类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of IMPSizector. \n
	DLL_EXPORT bool MPSizector_SetDefaultOutput(IMPSizector* pIMPSizector, unsigned char State);


	/// C语言风格调用函数，参考Utils类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of Utils. \n
	DLL_EXPORT bool Utils_Save(MP3DFrame* Frame, char* FileName);

	/// C语言风格调用函数，参考Utils类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of Utils. \n
    DLL_EXPORT bool Utils_Load(MP3DFrame* Frame, char* FileName);

	/// C语言风格调用函数，参考Utils类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of Utils. \n
    DLL_EXPORT bool Utils_Save3DPointCloud(MP3DFrame* Frame, char* FileName);

	/// C语言风格调用函数，参考Utils类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of Utils. \n
	DLL_EXPORT void Utils_PostProcessFiltrate(MP3DFrame* Frame);

	/// C语言风格调用函数，参考Utils类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of Utils. \n
	DLL_EXPORT void Utils_PostProcessRemoveBurrs(MP3DFrame* Frame);

	/// C语言风格调用函数，参考Utils类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of Utils. \n
    DLL_EXPORT void Utils_PostProcessMend(MP3DFrame* Frame);

	/// C语言风格调用函数，参考Utils类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of Utils. \n
    DLL_EXPORT void Utils_GetGrayBitmap(MP3DFrame* Frame, unsigned char* BitmapBuffer);

	/// C语言风格调用函数，参考Utils类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of Utils. \n
	DLL_EXPORT void Utils_GetColorDepthBitmap(MP3DFrame* Frame, unsigned char* RedBuffer, unsigned char* GreenBuffer, unsigned char* BlueBuffer,
		Convert3DTargetType Target, float Max, float Min);

	/// C语言风格调用函数，参考Utils类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of Utils. \n
	DLL_EXPORT void Utils_RefPlanRectify(MP3DFrame* Frame3D, MP3DFrame* RefPlan);

	/// C语言风格调用函数，参考Utils类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of Utils. \n
	DLL_EXPORT void Utils_MultiExposureFusion(int FrameNum, MP3DFrame** Input,int Image2DSourceIndex, bool AverageAllValidPoints);

	/// C语言风格调用函数，参考Utils类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of Utils. \n
	DLL_EXPORT float Utils_MeasureZ(MP3DFrame* Frame3D, unsigned int TopLeftX, unsigned int TopLeftY, unsigned int BottomRightX, unsigned int BottomRightY, float DropRatio);

	/// C语言风格调用函数，参考Utils类中对应函数的说明。\n
	/// C-style function. See the illustration of corresponding member of Utils. \n
	DLL_EXPORT void Utils_ConvertToRangeImage(MP3DFrame* src_frame, RangeImage* dst_image);
}



#endif