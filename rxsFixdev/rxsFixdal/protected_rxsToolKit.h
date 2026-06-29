#pragma once
#include "protected.h"


//#define GLOG_NO_ABBREVIATED_SEVERITIES
#define NOMINMAX
#define BOOST_USE_WINDOWS_H
#define _CRT_SECURE_NO_WARNINGS



class rxsToolKit {
public:
	bool isDebug;
	bool notCalculating;

	rxsToolKit();
	~rxsToolKit();

	bool Ready();
	void SavePathSet(std::string path);
	short ToolKitCaller(std::string toolName, u8 dir, CloudEntity, rxsResultReport*, roiArea*, unsigned, void* extParam);
	void ToolKitAdd(std::string toolName, int mid);
	std::string* ToolsNameGet(unsigned& len);
private:
	
	std::string inCouldSavePath;
	std::map<std::string, int> tookMap;
	void ForeachMethods();
	struct _apiInfo {
		std::vector<std::string> Names;
		std::vector<std::string> Addrs;
		std::vector<std::string> Nos;
	}ToolsInfos;
	bool isValidCould(rxsPointCouldp* cp, unsigned cpNums);
	void ToolsInfosClr();

protected:
	//以下计算算子来自于算法dll
	bool fitBSpline(rxsPointCouldp* cp, unsigned cpNums, rxsResultReport* Result, u8 dir);
	bool semiconductor(rxsPointCouldp* cp, unsigned cpNums, BaseFace bf, rxsResultReport*, u8 dir);
	bool PIN(rxsPointCouldp* cp, unsigned cpNums, BaseFace bf, rxsResultReport*, u8 dir);
	/// <summary>
	/// 配准,当outResult 为空时候为不满足两幅图效果不需要进行计算，不为空时候则说明有两幅需要进行计算
	/// </summary>
	bool registration(rxsPointCouldp* cp, unsigned& cpNums, std::string& rtTx, rxsResultReport* outResult = 0);
	bool rtConvert(rxsPointCouldp* cp, unsigned& cpNums, std::string rtTx, rxsResultReport* outResult = 0);
	bool keyboard(rxsPointCouldp* cp, unsigned cpNums, rxsResultReport*, u8 dir);
	bool surfaceProfile(rxsPointCouldp* cp, unsigned cpNums, rxsResultReport*);
	bool computeConvexHull(rxsPointCouldp* cp, unsigned cpNums, rxsResultReport*);
	bool volume(rxsPointCouldp* cp, unsigned cpNums, rxsResultReport*);
};
