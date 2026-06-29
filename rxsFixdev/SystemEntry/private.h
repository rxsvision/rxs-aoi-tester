#pragma once
#include "public.h"
#pragma warning(disable : 4996)

#include "../inkType/inkFile.h"
#include "../inkType/inkType.h"
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <mutex>




typedef struct  _kdvparam {
	std::string key;
	std::string val;
	std::string des;
}Kvpd;

typedef struct _valdesp {
	std::string val;
	std::string des;

	bool operator== (const char*name) {
		return (val == name);
	}
	bool operator== (std::string name) {
		return (val == name);
	}
	bool operator== (_valdesp obj) {
		return (val == obj.val && des == obj.des);
	}
}Vpd;

typedef struct _Axmapi {
	std::string name;
	int mIndex;
}AxmapIndex;


typedef int (*SoftTiggerEvent) (const char*, unsigned);

class __declspec(dllexport) SoftTiggerManger {
public:


	typedef struct _TiggerCondition {
		/// <summary>
		/// =0数字输入 DI; =1模拟输入 Ai; =2 TCP外部软信号
		/// </summary>
		u8 TiggerSource;
		union MyCondition
		{

			float Val;
		}Condition;

	}TiggerCondition;

	SoftTiggerManger();
	~SoftTiggerManger();

	unsigned PortNumGet();
	void TiggerEventSet(const char* key, int source, SoftTiggerEvent);
	void TiggerSourceSet();


private:
	typedef struct _SoftinSignal {
		const char* tiggerName;
		TiggerCondition	TiggerC;
		SoftTiggerEvent TiggerEvent;
	}SoftinSignal;
	//优化成 vector
	std::map<std::string, SoftinSignal> keySinGrps;
	void Tigger();
	bool TiggerCheck(SoftinSignal* s);

};


class ImageableGrpManger
{
public:
	ImageableGrpManger();
	~ImageableGrpManger();
	int Count();
	ImageableSensorUnit* Selected(const char* Name);
	const char* SelectedName();
	bool HasOpen(int &index);
	const char* GrpsNameForeach(bool& isStart, ImageableSensorUnit::DevType& isuType);
	bool Push(std::string name, ImageableSensorUnit* isu);
	void Remove(std::string name);
private:
	const char* slectedName;
	std::map<std::string, ImageableSensorUnit*> isuGrps;
	std::map<std::string, ImageableSensorUnit*>::iterator iter;
};


typedef struct FuncNoCallParam {
	int No;
	inkType::inkString Paramstr;
	bool ignoreAlam;
}FuncNoParam;



class __declspec(dllexport) SysConfigManger {
public:
	SysConfigManger();
	~SysConfigManger();
	int Open(char* path);
	void Close();
	int ReadSegSet(char* SegName);
	const char* operator[](unsigned index);
	const char* operator[](char* key);
	bool Editer(char* key, char* val);
	bool Save();
private:
	void* file;
	void* rsfPath;
	void analysis();
	void* siv;
	void* secedseg;
};


class myRobort
{
public:
	myRobort();
	~myRobort();

	short Open(std::string connStr);
	int ImgProcessFinsh();
	short Org();
	short workStart();
	bool isWorkEnd(int& wich);

private:
	void* myServer;
};




class devCommunicationsServer
{
public:
	devCommunicationsServer();
	~devCommunicationsServer();
	short Open(std::string connStr);
	void Close();
	int ThreeDimensionalProcess(std::string content);
	int TwoDimensionalProcess(std::string content);
	bool hasOpen();
	int SendContent(const char* ip, int port, std::string content);
	int SendContent(const char* tege, std::string content);
	const char* ClientForeach(int& index);

	const char* RecviceContent(const char* ip, int port);
	const char* RecviceContent(const char* tage);
	void RecviceContentClr(const char* tage);
private:
	void* myServer;
};









typedef struct _MotionGrpCombiner {
	MotionModel GrpMode;
	std::string MatserAx;
	std::string* SalveAxList;
	int SlaveNum;
	unsigned hasFillSalve;
}MotionGrpCombiner;

class MotionCombiner {
public:
	MotionCombiner(void* mgp);
	~MotionCombiner();

	bool Creater(std::string name, MotionModel Mode, std::string masterAxName, std::string* salveNames, unsigned salveNums);
	bool Appened(std::string name, std::string salveNames);
	void Remove(std::string name);
	const char* Foreach(bool isStart = false);
	MotionGrpCombiner* Get(std::string name);
	std::string GetSaveCfgString(std::string name);
	bool ImportCfgString(const char* str);
	bool ImportCfgString(std::string name, std::string content);
private:
	std::map<std::string, MotionGrpCombiner> GrpManger;
	std::map<std::string, MotionGrpCombiner>::iterator forIter;
	void* mgpio;
};



typedef struct _uiproParam {
	int msgNo;
	void* param;
	void* fntPtr;
	uiProcessCallBack	uiProcesser;
}uiProParam;


//class rxsMainFrame;
//class UserManger;

typedef struct _Gtxparam {
	char* CurModulePath;
	inkType::inkString	bpFile;
	inkType::inkString	keyCode;
	inkType::inkLog	SytemLoger;
	inkType::inkString MsgText;

	FuncNoParam	funcNo;
	SoftTiggerManger	SoftTigger;

	MotionGpioSerialize* MotionGpioSer;
	void* Wrkprocess;
	void* Systemptr;

	void* UserMange;
	void* FactoryParamer;
	void* WorkParamer;

	/// <summary>
	/// bit 0，bit 1 系统全局线程运行；
	/// bit 2 运动序列化操作类 线程运行标记；
	/// bit 3,功能号调用触发；
	/// bit 7- 系统复位；
	/// bit 6,系统复位预处理完成切换到下一状态--即当前状态
	/// bit 5,系统复位状态切换
	/// </summary>
	unsigned	sysFlag;
	/// <summary>
	/// 系统异常标志
	/// </summary>
	unsigned	char sysExpFlag;

	std::map<std::string, int> AxGrpsMapIndex;
	std::map<std::string, dioPort> DinGrps;
	std::map<std::string, dioPort> DoutGrps;
	std::vector<std::string> Dipsq;
	std::vector<std::string> Dopsq;
	std::vector<std::string> Axpsq;

	std::vector<Kvpd> WrkparamsDesc;
	std::vector<Kvpd> UserparamsDesc;
	std::map<std::string, std::string> Users;


	ImageableGrpManger* imgableGrpMgr;

	uiProParam uipp;
	std::mutex uipLock;
}swGtxparam;




#define pSystem			((rxsMainFrame*)mainFrameptr)
