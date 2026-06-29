#pragma once
#include "public.h"

#define RPP_AX_ORG(Axng,v) ((std::vector<std::string>*) Axng)->push_back(v)


/// <summary>
/// 系统状态类
/// </summary>
class CLASSEXPORT SystemInfo
{
private:
	unsigned alamType;
	unsigned fixStatus;
	bool hadRest;

public:
	/// <summary>
	/// 设备状态枚举类型，初始化，准备完成，暂停中，运行中，复位中，报警中
	/// </summary>
	typedef enum { INIT = 0, RDY = 1, PAUSEING, RUNING, RESETING, ALAMING } FixStaus;
	/// <summary>
	/// 报警类型枚举：急停报警，驱动报警，加工报警，安全报警（如安全光栅、安全门等...）
	/// </summary>
	typedef enum { EMG = 0, SERV, DRIVER, WORKING, SECURITY }ALAMTYPE;


	SystemInfo();
	~SystemInfo();

	/// <summary>
	/// 是否报警指示
	/// </summary>
	/// <param name="AlamType">需要获取的报警类型</param>
	/// <returns>true 报警中，false 无报警</returns>
	bool isAlam(ALAMTYPE& AlamType);

	/// <summary>
	/// 报警类型设置
	/// </summary>
	/// <param name="">需要设置的报警类型</param>
	void AlamSet(ALAMTYPE);

	/// <summary>
	/// 当前设备状态获取
	/// </summary>
	/// <returns>枚举类型，对应的设备状态</returns>
	FixStaus	CurFixStatus();

	/// <summary>
	/// 设备状态设置
	/// </summary>
	/// <param name="">枚举中的设备类型</param>
	void FixStatusSet(FixStaus);
	/// <summary>
	/// 设备是否已经复位
	/// </summary>
	/// <returns>指示是否已经复位</returns>
	bool HadRestOk();
	/// <summary>
	/// 设备已经复位完成
	/// </summary>
	void HadRestFinsh();
};

/// <summary>
/// 用户管理类
/// </summary>
class CLASSEXPORT UserManger {
public:

	/// <summary>
	/// 用户全选枚举，未登入，操作员，示教技术员/工程师，管理员
	/// </summary>
	typedef enum { NOLOGIN = 0, OPTER, TECH, ADMIN = 3 } USER_AUTH;

	typedef struct userInfo {
		char* Uname;
		char* Pwd;
		USER_AUTH Auth;
	}UserInfo;

	UserManger();
	~UserManger();
	/// <summary>
	/// 当前用户
	/// </summary>
	UserInfo	CurUser;
	
	/// <summary>
	/// 用户登入
	/// </summary>
	/// <param name="Uname">用户名</param>
	/// <param name="Pwd">密码</param>
	/// <returns>成功返回 true </returns>
	bool Login(const char* Uname, const  char* Pwd);

	/// <summary>
	/// 重置用户
	/// </summary>
	/// <param name="Uname">用户名</param>
	/// <param name="Pwd">原密码</param>
	/// <param name="NewPwd">新密码</param>
	/// <param name="newAuth">权限</param>
	/// <returns>成功返回true</returns>
	bool ResetUser(const char* Uname,const char* Pwd,const char *NewPwd, USER_AUTH newAuth);
	bool AddUser(const char* Uname,const char* Pwd, USER_AUTH auth);
	bool DelUser(const char* Uname,const char* Pwd);

	/// <summary>
	/// 遍历用户
	/// </summary>
	/// <param name="reStart">true 指示重第一个开始遍历</param>
	/// <returns>存在返回，不存在 则返回NULL</returns>
	UserInfo* UserForech(bool reStart);
private:
	void hunx(void* s,char a);
	void Anasyer(void* rp);
	bool matchuser(UserInfo& uif);
	bool resetuVal(char* k, char* v,char a);
	bool isdefadm(UserInfo& uif);
	UserInfo usr;
	void* itr;
	void* v;
};

typedef struct  {
	const char* key;
	const char* val;
	const char* des;
}kvpdChr;



class CLASSEXPORT FactoryConfigManger
{
public:
	FactoryConfigManger();
	~FactoryConfigManger();
	bool Ready(char* path);
	bool Save(char* path = 0);
	//带新增插入功能，不存在key则执行新增插入。
	bool KvpUpdater(const char* key, const char* Val, const char* Desc);
	//带新增插入功能，超出行数则执行新增插入。
	bool MotionParamUpdater(int rowINdex, int colIndex, const char* val);
	//键值对参数获取
	const char* operator[](const char* key);
	/// <summary>
	/// 键值对参数遍历
	/// </summary>
	/// <param name="reStart">指示迭代器是否重置为begin()</param>
	/// <returns>存在则返回，遍历结束则为NULL</returns>
	kvpdChr* KvpReadForech(bool reStart);
	
	/// <summary>
	/// 键值对参数读取
	/// </summary>
	/// <param name="key">指定需要读取的键</param>
	/// <returns>存在对应键的值则返回，不存在则为NULL</returns>
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
	void ssConvert(void* sd);
	void* selectedRow;
	void* itr;
	kvpdChr retVal;
};

#define				FLOWER_LEN			128

typedef struct _Movepath {
	unsigned short id;
	float Sx;
	float Sy;
	float Sz;
	float Ex;
	float Ey;
	float Ez;
	float speed;
	float acc;
	float dec;
	/// <summary>
	/// 运动及动作方式说明
	/// bit 7，仅运动不进行扫描
	/// bit 4,5,6 - 0点位/ 1直线差补/ 2圆弧差补 / 7分段高速模式
	/// bit 0  X轴方向
	/// bit 1  Y轴方向
	/// bit 2  Z轴方向
	/// </summary>
	u8	Type;
	char	Flower[FLOWER_LEN];
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

/// <summary>
/// 加工运行参数管理类
/// </summary>
class CLASSEXPORT WorkParamManger {
public:
	WorkParamManger();
	~WorkParamManger();
	bool Load(const char* path = 0);
	void NewCreater();
	void Save(const char* path);
	void Appended(const char* segName, const char* content);
	char* CurFilePath();
	//对应要操作的参数选组 1-用户参数，2-加工参数,3 移动路径，4 扫描点表 
	void Selected(u8 tWich, bool AndClrContent = false);
	/// <summary>
	/// 编辑加工参数中对应的内容
	/// </summary>
	/// <param name="rowIndex">对应的行索引</param>
	/// <param name="data">对应数据，Kvpdchr* 、Movepath* 、Scanpoint*</param>
	/// <returns>成功与否</returns>
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
	/// <summary>
	/// 对点位表进行全体偏移
	/// </summary>
	void PointOffsetSet(rxsPointCouldp p);
	/// <summary>
	/// 测量项标准值设定判断计算
	/// </summary>
	/// <param name="mj"></param>
	/// <param name="curVal"></param>
	/// <returns></returns>
	bool MeasurementResultJudgment(MeasurementJudgment mj, float curVal);
	//测量项标准值设定
	void MeasurementStandSet(const char *mName, MeasurementJudgment);
	void rrpThRemapSet(const char* mName, const char*, bool);
	//测量项标准值移除
	void MeasurementStandRemove(const char* mName);
	void rrpThRemove(const char* mName);
	//测量项标准值遍历---reset指示迭代器是否重置为begin
	const char* MeasurementStandForeach(MeasurementJudgment& out, bool reset = false);
	const char* rrpThForeach(char** outName, bool& outEnb, bool reset = false);
	//测量项标准值获取
	MeasurementJudgment* MeasurementStandGet(const char*);
	const char* rrpThGet(const char*, bool*);
	//所有测量项标准值清空
	void MeasurementStandAllClr();
	void rrpThAllClr();
private:
	void* file;
	void* fn;
	void analysis();
	void sConvert(void *);
	void* v_MPathp;
	void* v_SPointp;
	u8	tsWich;
	void ClrSelectedContent();
	void* extAppened;
};

/// <summary>
/// 采集器类--2D图像及3D点激光/光谱及线激光/光谱传感器采集类
/// </summary>
class CLASSEXPORT ImageableGather :public FixServerBaser {
public:
	ImageableGather();
	~ImageableGather();
	short errCode();
	/// <summary>
	/// 类准备--第一个被执行的方法
	/// </summary>
	/// <returns></returns>
	bool Prepare();
	/// <summary>
	/// 选中对应传感器单元
	/// </summary>
	/// <param name="sName">采集需要被选中的对应传感器单元的别名</param>
	/// <returns>成功返回对应指针，失败则为NULL</returns>
	ImageableSensorUnit* Selected(const char* sName);
	/// <summary>
	/// 获取当前选中传感器单元的指针
	/// </summary>
	/// <returns>不存在则为NULL</returns>
	ImageableSensorUnit* CurSelectedGet();
	/// <summary>
	/// 采集准备
	/// </summary>
	/// <returns></returns>
	short GatherReady();

	/// <summary>
	/// 采集停止
	/// </summary>
	/// <returns></returns>
	short GatherStop();
	/// <summary>
	/// 采集结果获取
	/// </summary>
	/// <param name="count"></param>
	/// <returns></returns>
	virtual void* GatherResultGet(unsigned long& count);
	bool GatherAsynchronous(ImageableSensorUnit*);

	virtual void ReadyParamSet(void* param);
	/// <summary>
	/// 采集执行
	/// </summary>
	/// <param name="">指定的使用的采集的单元，可以为NULL即默认采用当前选中单元</param>
	/// <returns></returns>
	virtual bool Gather(ImageableSensorUnit*);
	virtual void Close();
protected:
	WorkParamManger* WrkParam;
	FactoryConfigManger* FacParam;
	void PushImageableGrp(const char* name, ImageableSensorUnit*);
	void errCodeSet(short code);
	virtual void Init();
	virtual bool GatherPrevProcess(ImageableSensorUnit*);
	virtual bool GatherEndProcess(ImageableSensorUnit*);
	virtual bool SensorCreater();
	const char* curSelectedName();
	void AllocAttenData(int size);
	void* AttenDataGet();
private:
	short ercode;
	u8 Method;
	void* Gxter;
	void* attenData;
	ImageableSensorUnit* curOpter;
	ImageableSensorUnit* curTxopter;
};

/// <summary>
/// 设备的运行处理类
/// </summary>
class __declspec(dllexport) WorkProcess :public FixServerBaser {
private:
	int step;
	void* mCombiner;
	void* trd_wp;
	void* trd_rp;
	void WorkRuning();
	void RuningProcess();
	void inSignalScanOpen();
	void inSignalScanClose();
	unsigned int IgnoreFlag;
	void* devcommserver;
protected:
	unsigned runCircle;
	UserManger* userManger;
	FactoryConfigManger* FacParam;
	WorkParamManger* WrkParam;
	u8 baseFlag;
	SystemInfo	sysStatus;

	bool SoftTiggerSet(const char*, u8 source, SoftTiggerEvent);
	bool MotionCombinerOpen(const char* cfgstr = 0);
	bool ImageableGatherOpen(ImageableGather* ig = 0);
	
	virtual short FunctionNumCaller(int funcNo, const char* paramStr) = 0;
	/// <summary>
	/// 设备的初始化加载方法
	/// </summary>
	/// <returns>成功返回0，失败非 0</returns>
	virtual short InitLoader();
	virtual void InSignlProcess();
	virtual void EmgStopdeal();
	virtual bool AutoClearAlam();
	/// <summary>
	/// 加工运行开始处理
	/// </summary>
	/// <param name="wrkStep">无用，保留</param>
	/// <returns></returns>
	virtual int WorkProcesser(int wrkStep);
	/// <summary>
	/// 加工开始准备方法
	/// </summary>
	/// <param name="cmdLine">加工开始前处理的内容的传入</param>
	/// <returns>成功 true,否则false</returns>
	virtual	bool StartReady(const char* cmdLine) = 0;
	virtual bool StopBatchCompleted();
	virtual bool StopJob();

	virtual bool WorkProParamConditionCheck(bool isLoad_or_Save,bool be_or_after);
	/// <summary>
	/// 状态标志设置操作
	/// </summary>
	void IgnoreFlagSet(unsigned int);
	void IgnoreFlagClr(unsigned int);
	bool HasIgnoreStatus(unsigned int);
public:
	WorkProcess();
	~WorkProcess();
	void Ready(void**);
	virtual void* HrdWareObjectGet(int index) = 0;
	//s==0 三色报警灯 全灭，==1  亮绿灯， ==2 亮黄灯， == 3 红灯
	void AlamLight(unsigned char s = 0, bool on = true);
	//重新再运动参数并且进行设置
	bool reLoadMotionParamSet();
	/// <summary>
	/// 采集器
	/// </summary>
	ImageableGather* iabGather;
	/// <summary>
	/// 2、3D传感器单元获取
	/// </summary>
	/// <param name="name">对应传感器单元的别名</param>
	/// <returns>对应指针，失败为NULL</returns>
	ImageableSensorUnit* ImageableSensorGet(const char* name);
	/// <summary>
	/// 传感器单元遍历
	/// </summary>
	/// <param name="isStart">指示迭代器是否重置为begin</param>
	/// <param name="isuType">返回传感器类型</param>
	/// <returns>传感器单元的别名</returns>
	const char* ImageableSensorForeach(bool& isStart, ImageableSensorUnit::DevType& isuType);
	bool ImageableHasOpen(int& noWich);

	/// <summary>
	/// 设备加工开始
	/// </summary>
	short Start(const char* cmdLine);
	/// <summary>
	/// 设备加工停止
	/// </summary>
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


	const char* LocServerAccessForeach(int&);
	int LocServerSend(const char* tage, const char* content, int len);
	int LocServerSend(const char* tip, int tPort, const char* content, int len);
	const char* LocServerReceive(const char* tage);
	const char* LocServerReceive(const char* tip, int tPort);
	void LocServerMsgClr(const char* tage = 0);
	bool LocServerOn(bool);
};

/// <summary>
/// 系统管理类
/// </summary>
class  __declspec(dllexport) rxsMainFrame :public FixServerBaser
{
private:
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
	void ScanCore();
	void RunCore();
	unsigned srCircle;
public:
	rxsMainFrame();
	~rxsMainFrame();
	
	int ErrCode();
	char* MsgText();
	const char* AppPath();
	SystemInfo	*sysStatus;
	UserManger* userManger;
	FactoryConfigManger* FacParam;
	WorkParamManger* WrkProparam;
	WorkProcess* Wrkpd;
	/// <summary>
	/// 前端消息回调处理设置
	/// </summary>
	/// <param name=""> 回调函数 </param>
	/// <param name="font_obj">前端对象指针</param>
	void FontMessageProcessCB(uiProcessCallBack, void* font_obj);
	/// <summary>
	/// 发送消息到前端处理
	/// </summary>
	virtual void FrontProcessMsgSender(int msgNo, void* param, bool Ansy = false);


	bool Ready(MotionUnit*, WorkProcess*);
	void Close();
	//加工开始
	short WorkStart(const char *cmdLine);
	//加工停止
	short WorkStop(bool isBatchEnd);
	//加工暂停
	short WorkPause();
	//加工继续
	short WorkGoon();
	//系统复位
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
};

