#pragma once
#if RXSFIXDAL_EXPORTS ||RXSHRDWDAL_EXPORTS|| _CONSOLE
#include "../SystemEntry/rxsMainFrame.h"
#else
#include "rxsMainFrame.h"
#endif // 0


typedef struct _CloudEntity {
	rxsPointCouldp* Entiy;
	unsigned long pNums;
}CloudEntity;

typedef struct _mycpcolor {
	unsigned char G;
	unsigned char B;
	unsigned char R;
	unsigned char A;
}cpColor;

typedef struct _ColorCloudEntity {
	CloudEntity	PC;
	cpColor		Color;
}ColorCloudEntity;



typedef struct _imgdata {
	u8 Channels;
	void* Data;
	unsigned Len;
	unsigned rows;
	unsigned cols;
}rxsImg;


class  CLASSEXPORT r232Connection {
public:
	r232Connection();
	~r232Connection();
protected:
	bool openSyn(const char* serial_name);
	short Comopen(const char* serial_name, int baud_rate, unsigned char parity, unsigned char byte_size, unsigned char stop_bits);
	void setSerialLastError(const char* error_msg);
	void* GetRsp(unsigned& len);
	int SendReq(void*, unsigned len);

	void* hCom = 0;
	void* content = 0;
};

class  CLASSEXPORT CST_TZls :public FHWObject, r232Connection {
public:
	CST_TZls();

	short Cnum = 4;
	//channel  'A'--'H'
	short brightnessDGet(char channel);

	//channel  'A'--'H',  val 0-255
	bool brightnessDSet(char channel, short val);
	//常亮 常灭模式;
	bool ConstantbrightModeSet(bool on_off);
	//脉冲频闪模式，channel  'A'--'H', pw 脉冲宽度 ms
	bool StrobeModeSet(char channel, int pw);

	bool ModeGet();

protected:
	short Link(const char* cmdline) override;
	short unLink() override;
private:
	void* Reader(unsigned& len) override;
	int Writer(void*, unsigned len) override;
	void* result = 0;
};

class  CLASSEXPORT CST_LightSource :public FHWObject, r232Connection {
public:
	CST_LightSource();

	short Cnum = 4;
	short ColorNum = 8;
	typedef enum { None = 0, White = 1, Blue = 2, Green = 3, Yellow = 4, RED_730 = 5, RED_850 = 6, UV = 7, RED = 8 } ColorIndex;
	typedef struct _partitioninfo {
		bool isOn;
		ColorIndex curCi;
		u8 curVal;
	}Partition;
	void Reset();
	unsigned char brightnessGet(short fq);
	bool brightnessSet(short BlockIndex, ColorIndex ci, short val);
	bool brightnessONOFF(ColorIndex* ofIndexVal, int len = 4);
	Partition* PartitionStatus();
protected:
	short Link(const char *cmdline) override;
	short unLink() override;

	Partition pblk[4] = { {0,None,0},{0,None,0},{0,None,0},{0,None,0} };
private:
	void* Reader(unsigned& len) override;
	int Writer(void*, unsigned len) override;
	void* result = 0;
};


class  CLASSEXPORT AdjustFocal :public FHWObject, r232Connection {
public:
	AdjustFocal();

	bool Range(int& min, int& max);
	bool Set(short v);
	short CurVal();
protected:
	short Link(const char* cmdline) override;
private:
	void* Reader(unsigned& len) override;
	int Writer(void*, unsigned len) override;
	short  val;
	int MaxV;
	int MinV;
};

class  CLASSEXPORT ZMotionDal :public MotionUnit {
private:
	bool AutoSearchIP();
	short baseCfgSet(const char*);
	bool BASfileUploader(const char* path);
	void resourceCfg();
public:

	typedef union _ExtrenDataGrp {
		struct arctd {
			rxsPoint center;
			rxsPoint ends;
		}ArcParam;
		float TgaePos;
	}GrpExtrenData;

	ZMotionDal();
	void Close();
	short Open(char* CfgPath);

	void ClearPos(unsigned AxisNo = 0);
	//清除 状态 及错误，SOE 1为清除 状态，2 为清除错误，3 为两者都清除
	bool ClearStatusError(u8 SOE);
	void JogMove(unsigned AxisNo, byte dir, AxisInfo& Axi);
	void PpMoveAbs(unsigned AxisNo, AxisInfo& Axi);
	void PpMoveRel(unsigned AxisNo, AxisInfo& Axi);
	void GroupMove(unsigned AxisNo, AxisInfo& Axi);

	bool Servon(unsigned AxisNo, bool OnOff);
	void AxStop(unsigned AxisNo, bool isSmooth);

	bool SetOutput(unsigned oSernum, bool Value);
	int GetInput(unsigned index);
	int GetOutput(unsigned index);

	void GroupParamSet(unsigned mAxisNo, AxisInfo& Axi);
	void GroupMoveClose(unsigned mAxisNo, AxisInfo& Axi);
	void AxHome(unsigned AxisNo, AxisInfo& Axi);
	void SetSoftLimit(unsigned AxisNo, long Ppos, long Npos);
	//ioSignal 及  状态，所有的状态标志量
	void GetAxStatus(unsigned AxisNo, AxisInfo& Axi);
	//轴的位置速度获取
	void GetAxVelPos(unsigned AxisNo, AxisInfo& Axi);
	void PosCompareSet(unsigned AxisNo, PoscmpSetparam&, PoscmpPilist& PointList);
};


class  CLASSEXPORT robotMotion_Dal :public MotionUnit {
private:

	void resourceCfg();
public:

	typedef union _ExtrenDataGrp {
		struct arctd {
			rxsPoint center;
			rxsPoint ends;
		}ArcParam;
		float TgaePos;
	}GrpExtrenData;


	void Close();
	short Open(char* CfgPath);

	void ClearPos(unsigned AxisNo = 0);
	//清除 状态 及错误，SOE 1为清除 状态，2 为清除错误，3 为两者都清除
	bool ClearStatusError(u8 SOE);
	void JogMove(unsigned AxisNo, byte dir, AxisInfo& Axi);
	void PpMoveAbs(unsigned AxisNo, AxisInfo& Axi);
	void PpMoveRel(unsigned AxisNo, AxisInfo& Axi);
	void GroupMove(unsigned AxisNo, AxisInfo& Axi);

	bool Servon(unsigned AxisNo, bool OnOff);
	void AxStop(unsigned AxisNo, bool isSmooth);

	bool SetOutput(unsigned oSernum, bool Value);
	int GetInput(unsigned index);
	int GetOutput(unsigned index);

	void GroupParamSet(unsigned mAxisNo, AxisInfo& Axi);
	void GroupMoveClose(unsigned mAxisNo, AxisInfo& Axi);
	void AxHome(unsigned AxisNo, AxisInfo& Axi);
	void SetSoftLimit(unsigned AxisNo, long Ppos, long Npos);
	//ioSignal 及  状态，所有的状态标志量
	void GetAxStatus(unsigned AxisNo, AxisInfo& Axi);
	//轴的位置速度获取
	void GetAxVelPos(unsigned AxisNo, AxisInfo& Axi);
	void PosCompareSet(unsigned AxisNo, PoscmpSetparam&, PoscmpPilist& PointList);
};

class  CLASSEXPORT AuboRobotDal :public MotionUnit {
public:
	~AuboRobotDal();
	typedef union _ExtrenDataGrp {
		struct arctd {
			rxsPoint center;
			rxsPoint ends;
		}ArcParam;
		float TgaePos;
	}GrpExtrenData;


	void Close();
	short Open(char* CfgPath);

	void ClearPos(unsigned AxisNo = 0);
	//清除 状态 及错误，SOE 1为清除 状态，2 为清除错误，3 为两者都清除
	bool ClearStatusError(u8 SOE);
	void JogMove(unsigned AxisNo, byte dir, AxisInfo& Axi);
	void PpMoveAbs(unsigned AxisNo, AxisInfo& Axi);
	void PpMoveRel(unsigned AxisNo, AxisInfo& Axi);
	void GroupMove(unsigned AxisNo, AxisInfo& Axi);

	bool Servon(unsigned AxisNo, bool OnOff);
	void AxStop(unsigned AxisNo, bool isSmooth);

	bool SetOutput(unsigned oSernum, bool Value);
	int GetInput(unsigned index);
	int GetOutput(unsigned index);

	void GroupParamSet(unsigned mAxisNo, AxisInfo& Axi);
	void GroupMoveClose(unsigned mAxisNo, AxisInfo& Axi);
	void AxHome(unsigned AxisNo, AxisInfo& Axi);
	void SetSoftLimit(unsigned AxisNo, long Ppos, long Npos);
	//ioSignal 及  状态，所有的状态标志量
	void GetAxStatus(unsigned AxisNo, AxisInfo& Axi);
	//轴的位置速度获取
	void GetAxVelPos(unsigned AxisNo, AxisInfo& Axi);
	void PosCompareSet(unsigned AxisNo, PoscmpSetparam&, PoscmpPilist& PointList);
};

/// <summary>
/// 立仪点光谱
/// </summary>
class CLASSEXPORT LightE_AOCC : public ImageableSensorUnit {
private:
	///控制器数量
	int ctrCount;
	void CtrInit();
	void ThinknessGet(unsigned dataSize);
	void HeightDataGet(unsigned dataSize);
	int sctr;
	//受到 Mode 影响 = 0内触发; = 1 外触发模式; = 2其他的一些触发采集模式
	bool GrabTigger(unsigned int gCount = 1000);
	//受到 Mode 影响 = 0内触发; = 1 内触发模式-高度数据; = 2内触发模式-厚度数据 ， 其他外触发！gCount -期望采集数据量。
	bool GrabTiggerContinuity(unsigned int gCount = 1000);
	void stop() override;
protected:
	short open(const char* tsPath = 0) override;
	void close() override;
public:
	LightE_AOCC();
	bool ResetConnect() override;

	bool grabContinuity(bool isAnsy, int gCount) override;
	void GetSensorparam() override;
	void SetSensorparam() override;

	//返回采集到的点数，< 0 异常错误,= 0 采集停止
	int IsCollectionCompleted(unsigned int gCount) override;
	void* DataGet(unsigned& outcount) override;
	bool TiggerTaber(short wich, bool isClose = false) override;
	bool expActive(short fno, void* param) override;

	void CoordinateConvert();
	bool WorldCoordinateSet(u8 wich, float x, float y, float z);

protected:
	bool Dark();
	//量程零点 及 上下边界设置
	bool OrgAndRange(float zVal, float upVal = 0.0f, float downVal = 0.0f);
};

/// <summary>
/// 基恩士点光谱
/// </summary>
class CLASSEXPORT KeyenceSpectrum : public ImageableSensorUnit {

private:
	float Sx;
	float Sy;
	int result;
	int RequestRsp(const char* msg, unsigned msgLen);
	int DataRequestRsp(const char* msg, unsigned msgLen);
	bool resultAnasyer(unsigned& errCode, unsigned& rspLen);

	bool inReqrsp();

	int MakeCmder(char cmd, char* param, unsigned ParamLen, char* CmdBuff);

	bool GetDataStoreObj(char objVal[4], bool isNew = true);
	int GetDataStoreVal(char objVal[4], unsigned getNum);

	bool GrabTigger(unsigned int gCount = 1000);
	bool GrabTiggerContinuity(unsigned int gCount = 1000);
protected:
	short open(const char* tsPath = 0) override;
	void close() override;
	void stop() override;
public:
	KeyenceSpectrum();
	~KeyenceSpectrum();

	bool ResetConnect() override;
	bool grabContinuity(bool isAnsy, int gCount) override;
	void GetSensorparam() override;
	void SetSensorparam() override;

	//返回采集到的点数，< 0 异常错误,= 0 采集停止
	int IsCollectionCompleted(unsigned int gCount) override;
	void* DataGet(unsigned& outcount) override;
	bool TiggerTaber(short wich, bool isClose = false) override;
	bool expActive(short fno, void* param) override;
	void CoordinateConvert();
	bool WorldCoordinateSet(u8 wich, float x, float y, float z);
protected:
	bool Dark();
	//量程零点 及 上下边界设置
	bool OrgAndRange(float zVal, float upVal = 0.0f, float downVal = 0.0f);
};

/// <summary>
/// 基恩士线激光
/// </summary>
class CLASSEXPORT KeyenceLJX8000A : public ImageableSensorUnit {
protected:
	short open(const char* tsPath = 0) override;
	void close() override;
	void stop() override;
	bool grabContinuity(bool isAnsy, int gCount) override;
public:
	KeyenceLJX8000A();
	~KeyenceLJX8000A();

	bool ResetConnect() override;
	void GetSensorparam() override;
	void SetSensorparam() override;
	int IsCollectionCompleted(unsigned int gCount) override;
	void* DataGet(unsigned& outcount) override;
	rxsPointCouldp* DataGetPointCould(unsigned& num) override;
	void CoordinateConvert() override;
	bool WorldCoordinateSet(u8 wich, float x, float y, float z) override;
private:
};

/// <summary>
/// 翌视线激光
/// </summary>
class CLASSEXPORT NVTLineLsr : public ImageableSensorUnit {
private:
	int inBindBuff(int);
	void baseCfgSet(int sw, const char* path = 0);
	bool GrabTigger(unsigned int gCount = 1000);
protected:
	short open(const char* tsPath = 0) override;
	void close() override;
	void stop() override;
public:
	NVTLineLsr();
	~NVTLineLsr();

	bool ResetConnect() override;
	bool grabContinuity(bool isAnsy, int gCount) override;
	void GetSensorparam() override;
	void SetSensorparam() override;
	int IsCollectionCompleted(unsigned int gCount) override;
	void* DataGet(unsigned& outcount) override;

	bool TiggerTaber(short wich, bool isClose) override;
	bool expActive(short fno, void* param) override;
	void CoordinateConvert() override;
	bool WorldCoordinateSet(u8 wich, float x, float y, float z) override;
protected:
	/// <summary>
	/// 打开扫描激光线
	/// </summary>
	/// <returns></returns>
	bool OpenScanerLine();
	//量程零点 及 上下边界设置
	bool OrgAndRange(float zVal, float upVal = 0.0f, float downVal = 0.0f);
};

/// <summary>
/// 普雷斯特
/// </summary>
class CLASSEXPORT Precitec : public ImageableSensorUnit {
protected:
	short open(const char* tsPath = 0) override;
	void close() override;
	void stop() override;
	bool grabContinuity(bool isAnsy, int gCount) override;
public:
	Precitec();
	~Precitec();
	bool ResetConnect() override;
	void GetSensorparam() override;
	void SetSensorparam() override;
	int IsCollectionCompleted(unsigned int gCount) override;
	void* DataGet(unsigned& outcount) override;

	bool TiggerTaber(short wich, bool isClose) override;
	bool expActive(short fno, void* param) override;
	void CoordinateConvert() override;
	/// <summary>
	/// wich 最高位 bit7 指出 是否终点,其他位指示是哪一个轴
	/// </summary>
	bool WorldCoordinateSet(u8 wich, float x, float y, float z) override;

private:
	rxsPoint sp;
	rxsPoint ep;
	u8 dir;
};

//深视
class CLASSEXPORT SSZN_SR8060 : public ImageableSensorUnit {
protected:
	short open(const char* tsPath = 0) override;
	void close() override;
	void stop() override;
	bool grabContinuity(bool isAnsy, int gCount) override;
public:
	SSZN_SR8060();
	~SSZN_SR8060();
	bool ResetConnect() override;
	void GetSensorparam() override;
	void SetSensorparam() override;
	int IsCollectionCompleted(unsigned int gCount) override;
	void* DataGet(unsigned& outcount) override;
	rxsPointCouldp* DataGetPointCould(unsigned& num) override;

	bool TiggerTaber(short wich, bool isClose) override;
	bool expActive(short fno, void* param) override;
	void CoordinateConvert() override;
	/// <summary>
	/// wich 最高位 bit7 指出 是否终点,其他位指示是哪一个轴
	/// </summary>
	bool WorldCoordinateSet(u8 wich, float x, float y, float z) override;
private:
	int reT;
	void LoadCfg(const char *path);
	int mDispH;
	int* mProfileData;
	unsigned char* mIntensityData;
	unsigned int* mEncoderData;
	bool bCamBOnline;
	void resultConvert();
	void resultConvert(int* pd, unsigned lineNums);
	double H_Up;
	double H_Down;
	int HOffset;
};


/// <summary>
/// 盛相结构光
/// </summary>
class CLASSEXPORT MPSizectorDal : public ImageableSensorUnit {
protected:
	short open(const char* tsPath = 0) override;
	void close() override;
	void stop() override;
	bool grabContinuity(bool isAnsy, int gCount) override;
	int DeviceCount;
public:
	MPSizectorDal();
	~MPSizectorDal();

	bool ResetConnect() override;
	void GetSensorparam() override;
	void SetSensorparam() override;
	int IsCollectionCompleted(unsigned int gCount) override;
	void* DataGet(unsigned& outcount) override;
	rxsPointCouldp* DataGetPointCould(unsigned& num) override;
	void CoordinateConvert() override;
	bool WorldCoordinateSet(u8 wich, float x, float y, float z) override;
private:
};


class CLASSEXPORT HkCream_ch120 : public ImageableSensorUnit {
protected:
	short open(const char* tsPath = 0) override;
	void close() override;
	void stop() override;
	bool grabContinuity(bool isAnsy, int gCount) override;
	bool hasConnectioned() override;
public:
	HkCream_ch120();
	~HkCream_ch120();
	bool ResetConnect() override;

	void GetSensorparam() override;
	void SetSensorparam() override;
	int IsCollectionCompleted(unsigned int gCount) override;
	void* DataGet(unsigned& outcount) override;

	bool TiggerTaber(short wich, bool isClose) override;
	bool expActive(short fno, void* param) override;
	void CoordinateConvert() override;
	bool WorldCoordinateSet(u8 wich, float x, float y, float z) override;
	void FrontendDisplayWndPush(void*) override;

	//ExposureTime....等等float  参数设置
	void SpecParamSet(const char* pkey, float v) override;
	void SpecParamSet(const char* pkey, int v) override;
	
private:
	void* DevList;
	void* Stream;
	int nRet;
	bool statusIsNotOk();
};



/// <summary>
/// 数显千分表
/// </summary>
class CLASSEXPORT DialGauge : public ImageableSensorUnit {
protected:
	short open(const char* tsPath = 0) override;
	void close() override;
	void stop() override;
	bool grabContinuity(bool isAnsy, int gCount) override;
public:
	DialGauge();
	~DialGauge();
	bool ResetConnect() override;

	void GetSensorparam() override;
	void SetSensorparam() override;
	int IsCollectionCompleted(unsigned int gCount) override;
	void* DataGet(unsigned& outcount) override;

	bool TiggerTaber(short wich, bool isClose) override;
	bool expActive(short fno, void* param) override;
	void CoordinateConvert() override;
	bool WorldCoordinateSet(u8 wich, float x, float y, float z) override;

private:
	float val;
};