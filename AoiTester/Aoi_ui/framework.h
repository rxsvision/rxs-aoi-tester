#pragma once
//#define	RXSAPI		extern "C" __declspec(dllexport)

#define RXSAPIS		__declspec(dllexport)
#define CLASSEXPORT		__declspec(dllexport)
#define INTMAXVAL		0xFFFFFFFF


#define RPP_AX_ORG(Axng,v) ((std::vector<std::string>*) Axng)->push_back(v)


typedef unsigned char u8;
typedef unsigned char byte;
typedef int (*SoftTiggerEvent) (const char*, unsigned);
typedef int (*uiProcessCallBack)(void*, int);


typedef struct _sysparam {
	float			Vel1, Vel2;//速度大小
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


class CLASSEXPORT SystemInfo
{
private:
	unsigned alamType;
	unsigned fixStatus;
	bool hadRest;

public:
	typedef enum { INIT = 0, RDY = 1, PAUSEING, RUNING, RESETING, ALAMING } FixStaus;
	typedef enum { EMG = 0, SERV, DRIVER, WORKING, SECURITY }ALAMTYPE;


	SystemInfo();
	~SystemInfo();

	bool isAlam(ALAMTYPE& AlamType);
	void AlamSet(ALAMTYPE);

	FixStaus	CurFixStatus();
	void FixStatusSet(FixStaus);
	bool HadRestOk();
	void HadRestFinsh();
};


class CLASSEXPORT UserManger {
public:

	typedef enum { NOLOGIN = 0, OPTER, TECH, ADMIN = 3 } USER_AUTH;

	typedef struct userInfo {
		char* Uname;
		char* Pwd;
		USER_AUTH Auth;
	}UserInfo;

	UserManger(void *Gtxper);
	~UserManger();
	UserInfo	CurUser;
	bool Login(const char* Uname, const  char* Pwd);
	bool ResetUser(const char* Uname,const char* Pwd,const char *NewPwd, USER_AUTH newAuth);
	bool AddUser(const char* Uname,const char* Pwd, USER_AUTH auth);
	bool DelUser(const char* Uname,const char* Pwd);
	UserInfo* UserForech(bool reStart);
private:
	void* swGtxper;
	void hunx(void* s,char a);
	void Anasyer(void* rp);
	bool matchuser(UserInfo& uif);
	bool resetuVal(char* k, char* v,char a);
	bool isdefadm(UserInfo& uif);
	UserInfo usr;
	void* itr;
	void* v;
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

	typedef enum GatherMethod {
		Signal,//单帧采集
		Continuous,//连续
	};
	typedef enum TiggerMethod {
		Encoder,//编码器
		External,//外部输入
		Software//软触发
	};

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
		float vfRate;//物理虚拟比列 = px/mm;
		int Width;
		int Height;
		u8 Mode;//抓取 / 触发  模式
		int BuffSize;
		void* ptrGrabResult;
		void* ExtendData;
	}ImgableSensorInfo, * PImgableSensorInfo;

	ImgableSensorInfo Sensorinfo;

	typedef struct _CtrInfo {
		int iDCHandle;
		unsigned ChannelNum;
		int devID;
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
	bool Grab(bool isAnsy,bool isOneOrContinuity);
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
	/// <summary>
	/// 触发模式切换
	/// </summary>
	/// <param name="wich">选择触发的方式</param>
	/// <param name="isClose">开启或关闭</param>
	/// <returns></returns>
	virtual bool TiggerTaber(short wich, bool isClose = false);

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

typedef struct _kvpdChr {
	const char* key;
	const char* val;
	const char* des;
}kvpdChr;



class CLASSEXPORT FactoryConfigManger
{
public:
	FactoryConfigManger(void* Gtxper);
	~FactoryConfigManger();
	bool Ready(char* path);
	bool Save(char* path = 0);
	//带新增插入功能，不存在key则执行新增插入。
	bool KvpUpdater(const char* key, const char* Val, const char* Desc);
	//带新增插入功能，超出行数则执行新增插入。
	bool MotionParamUpdater(int rowINdex, int colIndex, const char* val);
	//键值对参数获取
	const char* operator[](const char* key);
	kvpdChr* KvpReadForech(bool reStart);
	kvpdChr* KvpRead(const char* key);

	//运动参数获取---行选中，返回对应行中的列数
	unsigned SelectedRow(unsigned rowIndex);
	//运动参数获取
	const char* operator[](unsigned colIndex);
private:
	void* file;
	void* path;
	void analysis();
	bool Open();
	void* zzStoreKvp;
	void* zzStoreMp;
	void* swGtxper;
	void ssConvert(void* sd);
	void* selectedRow;
	void* itr;
	kvpdChr retVal;
};

typedef struct _Movepath {
	unsigned short id;
	float Sx;
	float Sy;
	float Sz;
	float Ex;
	float Ey;
	float Ez;
	float speed;
	/// <summary>
	/// 运动及动作方式说明
	/// bit 7，仅运动不进行扫描
	/// bit 4,5,6 - 0点位/ 1直线差补/ 2圆弧差补 / 7分段高速模式
	/// bit 0  X轴方向
	/// bit 1  Y轴方向
	/// bit 2  Z轴方向
	/// </summary>
	u8	Type;
}Movepath;

typedef struct _Scanpoint {
	float x;
	float y;
	float z;
	float r;
	float len;
	float speed;
	u8 Type;
	unsigned short Pid;
}Scanpoint;

typedef struct _kvpdchr {
	char* key;
	char* val;
	char* des;
}Kvpdchr;


class CLASSEXPORT WorkParamManger {
public:
	WorkParamManger(void* Gtxper);
	~WorkParamManger();
	bool Load(const char* path = 0);
	void NewCreater();
	void Save(const char* path);
	void Appended(const char* segName, const char* content);
	char* CurFilePath();
	//对应要操作的参数选组 1-用户参数，2-加工参数,3 移动路径，4 扫描点表 
	void Selected(u8 tWich, bool AndClrContent = false);
	bool DataEditer(unsigned rowIndex, void* data);
	/// <summary>
	/// 读取键值对参数内容，需要前置 Selected 选中，1- 用户/工艺参数，2-加工参数
	/// </summary>
	/// <param name="rowIndex">对应-行索引</param>
	/// <param name="kcv">读取到的返回值</param>
	/// <returns>成功返回 true ,否则 false</returns>
	bool DataReaderKvpd(unsigned rowIndex, Kvpdchr& kcv);
	/// <summary>
	/// (当前暂未实现) 通过键的方式读取键值对参数内容，需要前置 Selected 选中，1- 用户/工艺参数，2-加工参数
	/// </summary>
	/// <param name="key">对应-键值</param>
	/// <param name="kcv">读取到的返回值</param>
	/// <returns>成功返回 true ,否则 false</returns>
	bool DataReaderKvpd(const char* key, Kvpdchr& kcv);
	//运动移动路径和扫描点位径获取，执行之前需要 Selected(3 或 4)选中
	void* DataReaderMsp(unsigned rowIndex);

	const char* AppendedContentGet(const char* segName);
private:
	void* file;
	void* fn;
	void analysis();
	void sConvert(void *);
	void* swGtxper;
	void* v_MPathp;
	void* v_SPointp;
	u8	tsWich;
	void ClrSelectedContent();
	void* extAppened;
};

class CLASSEXPORT ImageableGather {
public:
	ImageableGather();
	~ImageableGather();
	bool Prepare(void*);
	ImageableSensorUnit* Selected(const char* sName);
	ImageableSensorUnit* CurSelectedGet();
	short GatherReady();
	short GatherStop();
	void* DataGet(unsigned& count);
	/// <param name="Mode">获取的时间格式 模式 0 -年月日，1 -年月日时分秒，2-年月日时分秒毫秒 其他  time</param>
	void DateTimeGet(char* sysspan, unsigned short Mode);


	virtual void Close();
protected:
	WorkParamManger* WrkParam;
	FactoryConfigManger* FacParam;
	void SysLogRecord(const char* str);
	void DebugInfo(const char* str);
	void PushImageableGrp(const char* name, ImageableSensorUnit*);
	void* mGpio;
	virtual bool GatherPrevProcess(ImageableSensorUnit*, short& errCode);
	virtual bool Gather(ImageableSensorUnit*, short& errCode);
	virtual bool GatherEndProcess(ImageableSensorUnit*, short& errCode);
	virtual bool SensorCreater();
private:
	u8 Method;
	void* Gxter;
	ImageableSensorUnit* curOpter;
};

class __declspec(dllexport) WorkProcess {
private:
	int step;
	void* mCombiner;
	void* cGtxer;
	void* trd_wp;
	void* trd_rp;
	void WorkRuning();
	void RuningProcess();
	void inSignalScanOpen();
	void inSignalScanClose();
	unsigned int IgnoreFlag;
protected:
	unsigned runCircle;
	UserManger* userManger;
	FactoryConfigManger* FacParam;
	WorkParamManger* WrkParam;
	ImageableGather* iabGather;
	void* mGpio;
	void* mainFrameptr;
	u8 baseFlag;
	SystemInfo	sysStatus;
	void SysLogRecord(const char* str);
	void DebugInfo(const char* str);
	bool SoftTiggerSet(const char*, u8 source, SoftTiggerEvent);
	bool MotionCombinerOpen(const char* cfgstr = 0);
	bool ImageableGatherOpen(ImageableGather* ig = 0);
	
	virtual short FunctionNumCaller(int funcNo, const char* paramStr) = 0;
	virtual short InitLoader();
	virtual void InSignlProcess();
	virtual void EmgStopdeal();
	virtual bool AutoClearAlam();
	virtual int WorkProcesser(int wrkStep);
	virtual	bool StartReady(const char* cmdLine) = 0;
	virtual bool StopBatchCompleted();
	virtual bool StopJob();
	virtual bool WorkProParamConditionCheck(bool isLoad_or_Save,bool be_or_after);

	

	/// <summary>
	/// 发送消息到前端处理
	/// </summary>
	virtual void FrontProcessMsgSender(int msgNo, void* param = 0);

	/// <param name="Mode">获取的时间格式 模式 0 -年月日，1 -年月日时分秒，2-年月日时分秒毫秒 其他  time</param>
	void DateTimeGet(char* sysspan, unsigned short Mode);

	void IgnoreFlagSet(unsigned int);
	void IgnoreFlagClr(unsigned int);
	bool HasIgnoreStatus(unsigned int);
public:
	WorkProcess();
	~WorkProcess();
	void Ready(void* Gtxer, void**);
	//s==0  全灭，==1  亮绿灯， ==2 亮黄灯， == 3 红灯
	void AlamLight(unsigned char s = 0, bool on = true);
	bool reLoadMotionParamSet();
	ImageableSensorUnit* ImageableSensorGet(const char* name);
	bool ImageableHasOpen(int& noWich);
	short Start(const char* cmdLine);
	bool Stop(bool needBatchCompleted);
	bool SoftTigger(const char* key);
	const char* motionCombinerForeach(bool &isStart);
	const char* motionCombinerSalveForeach(const char* Name,unsigned index);
	void motionCombinerSave();
	void motionCombinerAdd(const char* name, MotionModel, const char* masterName, unsigned SalveNum);
	void motionCombinerAppened(const char* name, const char* SalveName);
	bool motionCombinerCfgSet(const char* name, const char* content);
	bool WorkProReloader(const char* path);


	virtual void AlamRed(bool on = true) = 0;
	virtual void AlamGreen(bool on = true) = 0;
	virtual void AlamYellow(bool on = true) = 0;

	virtual bool ResetPrevProcess(void* AxNameGrps, unsigned& Axnum) = 0;
	virtual bool ResetFinsh(bool isResting, unsigned& cnt) = 0;

	virtual	bool Pause();
	virtual	bool Goon(char* cmdLine) = 0;
	virtual bool FacParamLoadSet(AxisInfo* axiGrps, unsigned axiNum) = 0;
	virtual void Close();
};

class  __declspec(dllexport) rxsMainFrame
{
private:
	void* swGtxper;
	void* gSct;
	void* gRct;
	SysParam		sysParm;
	int errCode;
	bool SysInit();
	bool AppConfigGet(char* path);
	void appSegdataAnalysisAlc(unsigned SegNum, void* Reader);
	void appSegdataAnalysisSid(u8 isD_i_o, unsigned SegNum, void* Reader);
	void appSegdataAnalysisVec(u8 isW_T, unsigned SegNum, void* Reader);
	void defaultSysParamInit();
	void* GetLocaltime(char* sysspan, unsigned short Mode = 0, char* systime = 0);
	void ScanCore();
	void RunCore();
	unsigned srCircle;

public:
	rxsMainFrame();
	~rxsMainFrame();
	/// <summary>
	/// 配合后端 WorkProcess::uiProcessDealer 使用处理
	/// </summary>
	uiProcessCallBack	uiProcesser;
	int ErrCode();
	char* MsgText();
	SystemInfo	*sysStatus;
	UserManger *userManger;
	FactoryConfigManger *FacParam;
	WorkParamManger *WrkProparam;

	bool Ready(MotionUnit*, WorkProcess*);
	void Close();

	short WorkStart(const char *cmdLine);
	short WorkStop(bool isBatchEnd);
	short WorkPause();
	short WorkGoon();
	short SysReset();

	AxisInfo* AxInfo(unsigned& GrpNum);
	dioPort* DinGrp(unsigned& GrpNum);
	dioPort* DoutGrp(unsigned& GrpsNum);

	bool AxServon(const char* AxName, bool OnOff);
	short AxHome(const char* AxName, bool ignoreAlam = false);
	short AxMove(const char* AxName, u8 method, bool ignoreAlam = false);
	short AxStop(const char* AxName, bool ignoreAlam = false);

	bool InSignelGet(const char* key);
	bool OutSignelSet(const char* key, bool on_off);
	void OutSignelTiggle(const char* key);
	void SysLogRecord(const char* msg);
};

