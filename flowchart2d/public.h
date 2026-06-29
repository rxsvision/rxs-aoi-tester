#pragma once
#include <vector>
#include <map>
#include <string>

#define RXSAPIS		__declspec(dllexport)
#define CLASSEXPORT		__declspec(dllexport)
#define INTMAXVAL		0xFFFFFFFF


#define	CBC_UPDWRKD				0	//回调消息代码 加工参数
#define CBC_WRKSTOP				1	//回调消息代码 加工停止
#define CBC_WRKSTART			2	//回调消息代码 加工开始
#define CBC_WRKPAUSE			3	//回调消息代码 加工暂停
#define CBC_RELOADPAGE			9	//回调消息代码 重新刷新页面

#define CBC_PCUSHOW				10	//回调消息代码 显示一副点云图
#define CBC_IMGSHOW				14	//回调消息代码 显示一张2D图片
#define CBC_STR_N_MSG				20	//回调消息代码 字符串一般消息
#define CBC_STR_W_MSG				21	//回调消息代码 字符串警告消息
#define CBC_STR_E_MSG				22	//回调消息代码 字符串错误消息

#define CBC_VISUAL_UPD				100//可视化报表结果更新

typedef unsigned char u8;
typedef unsigned char byte;
typedef int (*SoftTiggerEvent) (const char*, unsigned);
typedef int (*uiProcessCallBack)(void*, int CBC_Code, void* self);


typedef struct _point {
	float x;
	float y;
}rxsPoint;

typedef struct _pointi {
	int x;
	int y;
}rxsPointI;

typedef struct _recti {
	int x;
	int y;
	int w;
	int h;
	float Angle;
	int Cx;
	int Cy;
}rxsRectI;

typedef struct _pointCouldp {
	double x;
	double y;
	double z;
}rxsPointCouldp;


typedef struct _sysparam {
	float	Vel1, Vel2;//速度大小
	u8	UnAttended;//
	unsigned MotionInpCheckTime;//到位检测时间 ms
	u8 runScanms, checkScanms;//扫描时间 ms
	u8	PhyMacCoorLp;//机床坐标向
}SysParam;

/// <summary>
/// //GROUP 群组模式 包括PT, GEAR, FOLLOW, RUNIN, PVT等多轴组合运动方式
/// </summary>
enum MotionModel {
	PP,
	JOG,
	GROUPSALVER,
	GROUP_PT_MASTER,
	GROUP_GEAR_MASTER,
	GROUP_FOLLOW_MASTER,
	GROUP_PVT_MASTER,
	GROUP_LINE_INTERPOLATION_MASTER,//直线插补
	GROUP_ARC_INTERPOLATION_MASTER//圆弧插补
};

enum EncOptType { GETPOS, GETVEL, SETPOS, };


typedef struct _axissts_yh {
	//轴的规划位置
	double prfPos;
	//轴的规划速度
	double prfVel;
	//轴的规划加速度
	double prfAcc;

	double EncPos;
	double EncVel;
	double EncAcc;
	bool OnServ;
	bool IsInp;
	bool IsMotionFinsh;
	bool OnOrg;
	bool OnLimtP;
	bool OnLimtN;
	bool IsEmg;
	bool IsAlam;
}AxisStsInfo;

typedef struct _trapparam
{
	double acc;
	double dec;
	double velStart;
	short  smoothTime;
} TrapPrm;

typedef struct _limitpos
{
	long Positive;
	long Negative;
	long Homeoffset;
	//最大运动位置
	long Maxsportpos;
}LIMITPOS, * PLIMITPOS;

typedef struct _rethome
{
	//是否朝正方向回原点，否为负方向
	bool		   Ispostive;
	/// <summary>
	/// 模式回零，的模式设置
	/// </summary>
	u8 Mode;
	/// <summary>
	/// 0 为非回原点中， 1 为 第一段回原点中， 2 为第二段回原点中,
	/// </summary>
	u8 Flag;
	//unsigned short Clearpos;
	//回原点方向路径: 1 原点->原点， 2 从正方向-〉 原点，3 从负方向到-〉原点
	//u8 Vector;
	float Offset;
	//回原点速度
	double			Vel;
	//教零回原点速度
	double			ZeroCorrectvel;
	//教零距离
	long			ZeroCorrectpos;
}RethomeStsparam;


typedef struct _salveparam {
	unsigned AxNo;
	void* ExtrenData;//额外参数比如直线插补等等需要分段的FIFO 结构之类
}SalveParam;


typedef struct _groupinfo
{
	//主轴编号
	unsigned MasterNo;
	unsigned SalveCount;//从轴数量
	SalveParam* SalveTable;//从轴号表及参数,
}GroupInfo;


typedef struct _axisinfo
{
	char* AxName;
	TrapPrm			MotionParam;
	LIMITPOS		LimitPos;
	AxisStsInfo		Status;
	RethomeStsparam Rethome;
	//最大加速度限制
	double			MaxAcclimit;
	float			TagePos;
	double			TageVel;
	double			RunVel;
	float			RunAcc;
	float			RunDec;
	float			Alpha;
	MotionModel		Mode;
	GroupInfo		GrpData;
}AxisInfo;

typedef struct _dioPortInfo {
	const char* ioName;
	unsigned mID;
	unsigned index;
	u8 isLogicOp;
	bool curStatus;
}dioPort;

typedef struct _poscmpsp {
	bool isHightSpeed;
	u8		Cmpdir;//-1,1
	u8		DoutNo;
	u8		DoutStatus;//0-关闭，1-打开
}PoscmpSetparam;

typedef struct _poscmpPointlist {
	unsigned ID;
	unsigned PointNums;
	float* PointArray;
}PoscmpPilist;


typedef struct _axmoveparam {
	const char* AxName;
	u8 method;
	float TageVel;
	float TagePos;
	float acc = 0.0f;
	float dec = 0.0f;
	//==0 不用等待，>0 具体等待时间ms,<0 到位状态等待
	int waitInp;
}AxMoveparam;


/// <summary>
/// 内部信号统一: 1-有信号-true, 0-无信号-false;
/// </summary>
class  CLASSEXPORT MotionUnit
{
protected:
	unsigned Serno;
	int errCode;
	void* EsData;
	unsigned AxNum;
	unsigned DiNum;
	unsigned DoNum;

public:
	typedef enum { DIO = 1, AIO = 2, MOTION, MOTIONCRD, UNKNOW } muType;
	muType	UnitType;

	MotionUnit(unsigned Serno = 0);
	~MotionUnit();
	virtual short Open(char* CfgPath);
	virtual void Close();

	virtual void ClearPos(unsigned AxisNo = 0);
	//清除 状态 及错误，SOE 1为清除 状态，2 为清除错误，3 为两者都清除
	virtual bool ClearStatusError(u8 SOE);
	virtual void JogMove(unsigned AxisNo, byte dir, AxisInfo& Axi);
	virtual void PpMoveAbs(unsigned AxisNo, AxisInfo& Axi);
	virtual void PpMoveRel(unsigned AxisNo, AxisInfo& Axi);
	virtual void GroupMove(unsigned AxisNo, AxisInfo& Axi);

	virtual bool Servon(unsigned AxisNo, bool OnOff);
	virtual void AxStop(unsigned AxisNo, bool isSmooth);

	virtual bool SetOutput(unsigned oSernum, bool Value);
	virtual	int GetInput(unsigned index);
	virtual int GetOutput(unsigned index);

	virtual void GroupParamSet(unsigned mAxisNo, AxisInfo& Axi);
	virtual void GroupMoveClose(unsigned mAxisNo, AxisInfo& Axi);
	virtual void AxHome(unsigned AxisNo, AxisInfo& Axi);
	virtual void SetSoftLimit(unsigned AxisNo, long Ppos, long Npos);
	//ioSignal 及  状态，所有的状态标志量
	virtual void GetAxStatus(unsigned AxisNo, AxisInfo& Axi);
	//轴的位置速度获取
	virtual void GetAxVelPos(unsigned AxisNo, AxisInfo& Axi);
	virtual void MoveOnHome(unsigned AxisNo);
	virtual void PosCompareSet(unsigned AxisNo, PoscmpSetparam&, PoscmpPilist& PointList);
};



class CLASSEXPORT MotionGpioSerialize {
public:
	MotionGpioSerialize();
	~MotionGpioSerialize();

	unsigned AxNum;
	unsigned DiNum;
	unsigned DoNum;

	AxisInfo* operator[](unsigned index);
	AxisInfo* operator[](const char* AxName);
	AxisInfo* AxGrpsGet();
	dioPort* DoGrpsGet();
	dioPort* DiGrpsGet();

	short Open(MotionUnit* mcu, unsigned  mNum = 0);
	void Close();

	//AxName 不能为空，轴移动，method = '+','-' jog移动±方向 'A','R' 点位 绝对 相对移动，其他群组运动。
	bool AxMove(const char* AxName, u8 method, float TageVel, float TagePos, float acc = 0.0f, float dec = 0.0f);
	bool AxMove(AxMoveparam mvp);
	//针对可标准接口化的机械手的方法
	bool MoveForRobort(AxMoveparam* grp, int nums);
	//AxName =0则全部停止，否则停止指定轴名称
	bool AxStop(const char* AxName = 0);
	//AxName =0则全部停止，否则停止指定轴名称
	bool AxHome(const char* AxName = 0);
	bool AxServon(bool on_off, const char* AxName = 0);

	bool IsMotionFinsh(const char* AxName);
	bool IsMotionInp(const char* AxName);
	bool IsOrgFinshed(const char* AxName = 0);

	/// <summary>
	/// 内部信号统一: 1-有信号-true, 0-无信号-false;
	/// </summary>
	bool SetDoutput(unsigned serIndex, bool on_or_off);
	bool SetDoutput(const char* key, bool on_or_off);
	u8 GetDinput(unsigned serIndex);
	u8 GetDinput(const char* key);
	bool SetAoutput(unsigned serIndex, bool on_or_off);
	bool SetAoutput(char* key, bool on_or_off);
	float GetAinput(unsigned serIndex);
	float GetAinput(char* key);
	bool PosCmpSet(const char* AxName, PoscmpSetparam psp, PoscmpPilist pclist);
	bool GroupMoveOpen(const char* masterAxName, MotionModel masterMM, unsigned SalveNum);
	bool GroupMoveSalveCfgPush(const char* masterAxName, const char* salveAxName, void* suData, int suSize);
	void GroupMoveCancel(const char* masterAxName);
private:
	void ready();
	bool isMotionPrevOk();
	bool AxNamePrevOk(const char* AxName);
	AxisInfo* AxNameToTageInfo(const char* AxName, unsigned& AxNo);
	u8 HomeModeAdj();
	void AxOrgHome(unsigned AxisNo);
	void AxSignalStsdeal(unsigned aid);
	void GetioSignal();
	void GetConverval(unsigned short Axisindex);
	void ConvertIOBuff(int iv, int ov);
	void ClearAxExtraData(unsigned AxisNo);
	AxisInfo* AxGrps;
	dioPort* DoGrps;
	dioPort* DiGrps;
	MotionUnit* MU;
	char inSideMode;
	void* dpThreader;
	short RunSignalProcess();
	int GrpMovePushIndex;
};



class  CLASSEXPORT FHWObject {

public:
	FHWObject();
	~FHWObject();
	short Open(const char* cmdline);
	int Request(void* d, unsigned len);
	void* Response(unsigned& len);
	bool Close();

protected:
	virtual void Init();
	virtual void* Reader(unsigned& len);
	virtual int Writer(void*, unsigned len);
	virtual short Link(const char* cmdline);
	virtual short unLink();
private:
	int flag;
};


class  CLASSEXPORT ImageableSensorUnit
{
public:
	typedef enum { Cream = 0, PointLsr = 1, LineLsr = 2, Confocal = 3, LineConfocal = 4 }DevType;
	typedef enum { INIT = 1, ON = 2, GRABING = 4 }DevStatus;
	typedef struct earthConn {
		char Ip[128];
		unsigned short Port;
	}EarthConn;

	typedef struct commConn {
		char comName[10];
		unsigned Baud;
		u8	stopbit;
	}Rs232Conn;

	typedef struct usbConn {
		/// <summary>
		/// 第几代usb
		/// </summary>
		u8 speed;
	}UsbConn;

	typedef union connMeathod {
		EarthConn	earth;
		UsbConn		usb;
		Rs232Conn	rs232;
	}ConnMeathod;

	typedef enum {
		Signal,//单帧采集
		Continuous,//连续
	}GatherMethod;

	typedef enum {
		Encoder,//编码器
		External,//外部输入
		Software//软触发
	}TiggerMethod;

private:
	u8 aloct;
	void* ParamGprs;
	int sts;
protected:
	int errCode;
	//type = 0 char; = 1 int; = 2 float ; = 3 double;
	void allocBuff(unsigned Size, u8 type = 0);
	void BuffClr();
	char* CofigPath;
	void SetCfgpath(char* cfgfile);
	void SetExtendData(int size);
	int slectedChannelno;
	void InsertGetParams(const char* name, const char* val);
	bool UpdateGetParams(const char* name, const char* val);
	void StatusFlagSet(DevStatus);
	void StatusFlagClr(DevStatus);
	bool isGrabing();
	virtual short open(const char* tsPath = 0);
	virtual void close();
	virtual void stop();
	virtual bool hasConnectioned();
	/// <summary>
	/// 连续触发采集操作,ccNums =1 时非连续采集，  结果内容-存储在 ImgableSensorInfo.ptrGrabResult
	/// </summary>
	/// <param name="gCount"></param>
	/// <returns></returns>
	virtual bool grabContinuity(bool isAnsy, int ccNums = 1);
public:
	typedef struct _imgablesensorinfo
	{
		unsigned ID;
		char ModelName[50];
		DevType			devType;
		ConnMeathod	DevConnMethod;
		bool	ReverseX;//水平翻转
		bool	ReverseY;//垂直翻转

		u8 Mode;//抓取 / 触发  模式
		int BuffSize;
		void* ptrGrabResult;
		void* ExtendData;
	}ImgableSensorInfo, * PImgableSensorInfo;
	ImgableSensorInfo Sensorinfo;
	typedef struct _CtrInfo {
		int devID;
		void* DevHandle;
		unsigned ChannelNum;
		int Width;
		int Height;
		float vfRate;//物理虚拟比列 = px/mm;
		int MeasureType;
		int TiggerMode;
		int CaptureMode;
		int sampleFreq;
		float expTime;
		float gain;
		int offsetLine;
		int lines;
	}ControlInfo;

	ControlInfo* CtrInfo;

	ImageableSensorUnit(unsigned index = 0);
	~ImageableSensorUnit();

	short Open(const char* tsPath = 0);
	bool Grab(bool isAnsy, unsigned OneOrContinuityNums);
	void Stop();
	void Close();

	virtual bool ResetConnect();
	virtual void GetSensorparam();
	virtual void SetSensorparam();

	virtual void CoordinateConvert();
	virtual bool WorldCoordinateSet(u8 wich, float x, float y, float z);
	/// <summary>
	/// 采集是否完成判断，在异步采集等一些情况下使用
	/// </summary>
	/// <param name="gCount">根据特定的情况含义不同</param>
	virtual int IsCollectionCompleted(unsigned int gCount);
	virtual void* DataGet(unsigned& outcount);
	virtual rxsPointCouldp* DataGetPointCould(unsigned& outcount);
	/// <summary>
	/// 触发模式切换
	/// </summary>
	/// <param name="wich">选择触发的方式</param>
	/// <param name="isClose">开启或关闭</param>
	/// <returns></returns>
	virtual bool TiggerTaber(short wich, bool isClose = false);
	virtual void FrontendDisplayWndPush(void* sd);
	/// <summary>
	/// 扩展操作
	/// </summary>
	virtual bool expActive(short fno, void* param);
	virtual bool DinGet(unsigned diIndex);
	virtual void DoutSet(unsigned doindex);

	bool HasOpen();
	void ChannelSelected(int channelNo);
	void ChannelSelectedGet(int& ChannelNo);
};

class rxsResultReportGrpsManger;

class CLASSEXPORT rxsResultReport {
public:
	rxsResultReport();
	rxsResultReport(rxsResultReportGrpsManger*, const char* tempName, const char* rrName);
	~rxsResultReport();
	const char* Name();
	const char* TempName();
	unsigned RowCount();
	void RowsClr();
	short SetFiled(std::string* filedName, unsigned len, std::string rpName);
	short SetValue(std::string filedName, std::string value, unsigned rowIndex);
	int AddRow();
	const char* GetFiled(unsigned index);
	const char* GetValue(unsigned rowIndex, unsigned index);
	const char* GetValue(unsigned rowIndex, const char* filedName);
	std::string SaveReportString();
	rxsResultReport operator ()();
	void clear();
private:
	std::vector<std::string> facFiledGrps;
	std::vector<std::map<std::string, std::string>>fmapValues;
	std::string name;
	std::string forName;
};


class CLASSEXPORT rxsResultReportGrpsManger {
public:
	rxsResultReportGrpsManger();
	~rxsResultReportGrpsManger();
	std::string* AllFieldGet(unsigned& Len);
	unsigned ResultReportTempSet(std::string tempName, std::string* filedName, unsigned len);
	unsigned ResultReportTempAppends(std::string tempName, std::string filedName);
	const char* ResultReportTempNameForech(bool isStart = true);
	std::string* ResultReportTempFiledGet(const char* TempName, unsigned& FiledNums);

	std::string ResultReportSaveStringGet();
	short ResultReportAdd(std::string TempName, std::string rpName);
	rxsResultReport* ResultReportGet(int index);
	rxsResultReport* ResultReportGet(std::string rrpName);
	void ResultReportClr();
	void ResultReportContentClr(rxsResultReport* spec = 0);
	bool TempExist(std::string tmpName);
	short ResultReportPush(rxsResultReport rpp);
private:
	bool hasClr;
	/// <summary>
	/// 所有字段域
	/// </summary>
	std::vector<std::string> allField;
	/// <summary>
	/// 通过字段域构成的输出报表模板
	/// </summary>
	std::map<std::string, std::vector<std::string>> TempRpps;
	std::map<std::string, std::vector<std::string>>::iterator itr;
	/// <summary>
	/// 输出报表实列
	/// </summary>
	std::vector<rxsResultReport> Rpps;
	std::map<std::string, int> nameMapID;
};


class CLASSEXPORT ResultReportGrpAssemble
{
public:
	ResultReportGrpAssemble();
	~ResultReportGrpAssemble();
	bool Print();
	bool Save(const char* path);
	void Clr();
	unsigned Count();
	void Push(rxsResultReportGrpsManger*);

private:
	std::vector<rxsResultReportGrpsManger> grps;
	std::string GetString();
	void flushResult(const char* path, const char* contData, unsigned cdLen);
};


class CLASSEXPORT FixServerBaser
{
public:
	FixServerBaser();
	~FixServerBaser();

	void SysLogRecord(const char* msg);
	/// <param name="Mode">获取的时间格式 模式 0 -年月日，1 -年月日时分秒，2-年月日时分秒毫秒 其他  time</param>
	void DateTimeGet(char* sysspan, unsigned short Mode);

	MotionGpioSerialize* MotionGpio;
protected:
	void DebugInfo(const char* str);
	void* GetLocaltime(char* sysspan, unsigned short Mode = 0, char* systime = 0);

private:

};


RXSAPIS std::vector<std::string> stringSplit(const std::string& str, char delim);

                                                                                                                                                                                                                                                                                                                                                                                                      