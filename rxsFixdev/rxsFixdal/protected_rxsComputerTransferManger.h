#pragma once
//#include "protected_rxsToolKit.h"
#include "../rxsImgProcess/protected_rxsToolKit.h"





class rxsComputerTransfer {
public:
	rxsComputerTransfer(const char* Name, rxsRoiAreaManger*, rxsResultReportGrpsManger*, rxsToolKit*);
	rxsComputerTransfer(std::string, rxsRoiAreaManger*, rxsResultReportGrpsManger*, rxsToolKit*);
	~rxsComputerTransfer();
	bool operator== (const rxsComputerTransfer& p1);
	bool operator== (std::string name);
	const char* Name();
	unsigned roiCount();
	void roiClr();
	void roiPush(int id);
	int* roiGrps(unsigned& Nums);
	void ToolKitNameSet(std::string Name);
	const char* ToolKitName();
	void ResultReportSet(std::string rpName);
	const char* ResultReportName(/*int& id*/);
	void ScanCloudSet(rxsPointCouldp* pointCloud, unsigned long pNums);
	short Run(u8 dir, int curid);
	std::string SaveStringGet();
	bool AllRunOk();
private:
	void InputRoiSelected();
	rxsRoiAreaManger* roiGrpMgr;
	rxsResultReportGrpsManger* resultReportGrpMgr;
	rxsToolKit* rtk;
	CloudEntity	CompleteCloud;
	std::vector<int> roidGrp;
	std::string toolKitName;
	std::string rpName;
	std::string cName;
	unsigned callCount;
	unsigned callFinshedCount;
	void stringFill(std::string);
};

class rxsComputerTransferManger {
private:
	std::vector<rxsComputerTransfer>	ctGrps;
	std::map<std::string, rxsComputerTransfer*> ctNameMap;
	rxsRoiAreaManger* roiGrp;
	rxsResultReportGrpsManger* rrGrp;
	rxsToolKit* rtks;
	short remove(rxsComputerTransfer* ctr);
	void namsmapUpdate();
public:
	rxsComputerTransferManger(rxsRoiAreaManger*, rxsResultReportGrpsManger*, rxsToolKit* rtk);
	~rxsComputerTransferManger();
	bool exist(std::string comName);
	short Add(const char* comName, const char* roiName, const char* ToolKitName, const char* rrName);
	short Update(const char* comName, const char* roiName, const char* ToolKitName, const char* rrName);
	short Remove(std::string Name);
	rxsComputerTransfer* Get(std::string comName);
	rxsComputerTransfer* Get(unsigned index);
	int Count();
	void Clear();
	std::string CfgSaveString();
	void CfgStringSet(const char* string);
	bool isCompleted();
};
