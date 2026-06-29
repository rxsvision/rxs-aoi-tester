#pragma once
#include "public.h"

#define RPP_AX_ORG(Axng,v) ((std::vector<std::string>*) Axng)->push_back(v)



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

	UserManger();
	~UserManger();
	UserInfo	CurUser;
	bool Login(const char* Uname, const  char* Pwd);
	bool ResetUser(const char* Uname,const char* Pwd,const char *NewPwd, USER_AUTH newAuth);
	bool AddUser(const char* Uname,const char* Pwd, USER_AUTH auth);
	bool DelUser(const char* Uname,const char* Pwd);
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

typedef struct _kvpdChr {
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
	char	Flower[20];
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
	WorkParamManger();
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
	void* v_MPathp;
	void* v_SPointp;
	u8	tsWich;
	void ClrSelectedContent();
	void* extAppened;
};

class CLASSEXPORT ImageableGather :public FixServerBaser {
public:
	ImageableGather();
	~ImageableGather();
	short errCode();
	bool Prepare();
	ImageableSensorUnit* Selected(const char* sName);
	ImageableSensorUnit* CurSelectedGet();	
	short GatherReady();
	short GatherStop();
	virtual void* GatherResultGet(unsigned long& count);
	bool GatherAsynchronous(ImageableSensorUnit*);

	virtual void ReadyParamSet(void* param);
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
protected:
	unsigned runCircle;
	UserManger* userManger;
	FactoryConfigManger* FacParam;
	WorkParamManger* WrkParam;
	void* mainFrameptr;
	u8 baseFlag;
	SystemInfo	sysStatus;

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


	void IgnoreFlagSet(unsigned int);
	void IgnoreFlagClr(unsigned int);
	bool HasIgnoreStatus(unsigned int);
public:
	WorkProcess();
	~WorkProcess();
	void Ready(void**);
	//s==0  全灭，==1  亮绿灯， ==2 亮黄灯， == 3 红灯
	void AlamLight(unsigned char s = 0, bool on = true);
	bool reLoadMotionParamSet();

	ImageableGather* iabGather;
	ImageableSensorUnit* ImageableSensorGet(const char* name);
	const char* ImageableSensorForeach(bool& isStart, ImageableSensorUnit::DevType& isuType);
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
	SystemInfo	*sysStatus;
	UserManger* userManger;
	FactoryConfigManger* FacParam;
	WorkParamManger* WrkProparam;
	WorkProcess* Wrkpd;
	/// <summary>
	/// 前端消息回调处理设置
	/// </summary>
	/// <param name=""></param>
	/// <param name="font_obj"></param>
	void FontMessageProcessCB(uiProcessCallBack, void* font_obj);
	/// <summary>
	/// 发送消息到前端处理
	/// </summary>
	virtual void FrontProcessMsgSender(int msgNo, void* param, bool Ansy = false);


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

};

