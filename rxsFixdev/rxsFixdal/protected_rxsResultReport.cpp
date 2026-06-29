#pragma once

#include "rxsFixdal.h"
#include "protected.h"

using namespace std;

rxsResultReport::rxsResultReport(unsigned ID) {
	id = ID;
}

rxsResultReport::rxsResultReport(unsigned ID, rxsResultReportGrpsManger* rrgm, const char* tempName, const char* rrName) {
	id = ID;
	forName = tempName;
	unsigned nums = 0;
	string* fileds = rrgm->ResultReportTempFiledGet(tempName, nums);
	if (nums > 0 && fileds != 0) {
		SetFiled(fileds, nums, rrName);
	}
	else {
		name = rrName;
	}
}

rxsResultReport::~rxsResultReport() {
	facFiledGrps.clear();
	id = -1;
}

int rxsResultReport::ID() { return id; }
const char* rxsResultReport::TempName() { return forName.c_str(); }
const char* rxsResultReport::Name() { return name.c_str(); }

short rxsResultReport::SetFiled(std::string* filedName, unsigned len, std::string rpName) {
	name = rpName;
	for (unsigned x = 0; x < len; x++) {
		facFiledGrps.push_back(filedName[x]);
	}
	return 0;
}

int rxsResultReport::AddRow() {
	std::map<std::string, std::string> fmapVal;
	fmapValues.push_back(fmapVal);
	return fmapValues.size();
}

short rxsResultReport::SetValue(std::string filedName, std::string value, unsigned rowIndex) {
	int len = facFiledGrps.size();
	if (len > 0) {
		vector<string>::iterator has = std::find(facFiledGrps.begin(), facFiledGrps.end(), filedName);
		if (has == facFiledGrps.end()) {
			return -2;
		}
		int x = fmapValues.size();
		std::map<std::string, std::string>* fmapVal = 0;
		if (x > 0 && rowIndex < x) {
			fmapVal = &fmapValues[rowIndex];
			(*fmapVal)[filedName] = value;
			return 0;
		}
	}
	return -1;
}


const char* rxsResultReport::GetFiled(unsigned index) {
	int len = facFiledGrps.size();
	if (len > 0) {
		if (index < len) {
			return facFiledGrps[index].c_str();
		}
	}
	return 0;
}

unsigned rxsResultReport::RowCount() {
	return fmapValues.size();
}

void rxsResultReport::RowsClr() {
	if (fmapValues.size() > 0) {
		fmapValues.clear();
	}
}

const char* rxsResultReport::GetValue(unsigned rowIndex, unsigned index) {
	int mvsLen = fmapValues.size();
	int vLen = facFiledGrps.size();
	if (rowIndex < mvsLen) {
		std::map<std::string, std::string>* fmapVal = &fmapValues[rowIndex];
		int mvLen = (*fmapVal).size();
		if (mvLen > 0 && index < vLen) {
			string A = facFiledGrps[index];
			std::map<std::string, std::string>::iterator itr = (*fmapVal).find(A);
			if (itr == (*fmapVal).end()) {
				return 0;
			}
			return itr->second.c_str();
		}
	}
	return 0;
}

const char* rxsResultReport::GetValue(unsigned rowIndex, const char* filedName) {
	int mvsLen = fmapValues.size();
	if (rowIndex < mvsLen) {
		std::map<std::string, std::string>* fmapVal = &fmapValues[rowIndex];
		std::map<std::string, std::string>::iterator itr = (*fmapVal).find(filedName);
		if (itr == (*fmapVal).end()) {
			return 0;
		}
		return itr->second.c_str();
	}
	return 0;
}

std::string rxsResultReport::SaveReportString() {
	int vLen = facFiledGrps.size();
	int mvsLen = fmapValues.size();
	string fV;
	int mvLen = 0;
	if (vLen < 1 || mvsLen < 1) {
		return fV;
	}
	std::stringstream ss;
	ss << std::fixed << std::setprecision(5);
	ss << "\r\n" << forName << "->" << name << ":\n";
	for (unsigned x = 0; x < mvsLen; x++) {
		std::map<std::string, std::string>* fmapVal = &fmapValues[x];
		mvLen = (*fmapVal).size();
		if (mvLen == vLen) {
			for (unsigned x = 0; x < vLen; x++) {
				string V = facFiledGrps[x];
				ss << V << ":" << (*fmapVal)[V] << ",";
			}
		}
		else {
			for (unsigned x = 0; x < vLen; x++) {
				string V = facFiledGrps[x];
				if ((*fmapVal).find(V) != (*fmapVal).end()) {
					ss << V << ":" << (*fmapVal)[V] << ",";
				}
				else {
					ss << V << ":" << "VAL(NULL), ";
				}
			}
		}
		ss << "\r\n";
	}
	fV = ss.str();
	if (fV.length() > 1)
	{
		fV[fV.length() - 1] = '\n';
		return fV;
	}
	return fV;
}

rxsResultReportGrpsManger::rxsResultReportGrpsManger() {
	allField.push_back("ID");
	allField.push_back("编号");
	allField.push_back("序号");
	allField.push_back("颜色");
	allField.push_back("名称");
	allField.push_back("轮廓度");
	allField.push_back("面轮廓度");
	allField.push_back("条码内容");
	allField.push_back("变形度");
	allField.push_back("弧长(mm)");
	allField.push_back("投影长(mm)");
	allField.push_back("高度(mm)");
	allField.push_back("深度(mm)");
	allField.push_back("X位置(mm)");
	allField.push_back("Y位置(mm)");
	allField.push_back("PASS");
}


rxsResultReportGrpsManger::~rxsResultReportGrpsManger() {
	TempRpps.clear();
	Rpps.clear();
	allField.clear();
	nameMapID.clear();
	itr = TempRpps.begin();
}


string* rxsResultReportGrpsManger::AllFieldGet(unsigned& Len) {
	Len = allField.size();
	return &allField[0];
}

unsigned rxsResultReportGrpsManger::ResultReportTempSet(std::string tempName, std::string* filedName, unsigned len) {
	std::vector<std::string> tempFiled;
	for (unsigned x = 0; x < len; x++) {
		tempFiled.push_back(filedName[x]);
	}
	TempRpps[tempName] = tempFiled;
	return TempRpps.size();
}

unsigned rxsResultReportGrpsManger::ResultReportTempAppends(std::string tempName, std::string filedName) {
	TempRpps[tempName].push_back(filedName);
	return TempRpps.size();
}

const char* rxsResultReportGrpsManger::ResultReportTempNameForech(bool isStart) {
	if (isStart) {
		itr = TempRpps.begin();
	}
	else {
		itr++;
	}
	if (itr != TempRpps.end()) {
		const char* r = (itr->first.c_str());
		return r;
	}
	return 0;
}

std::string* rxsResultReportGrpsManger::ResultReportTempFiledGet(const char* TempName, unsigned& FiledNums) {
	FiledNums = 0;
	if (TempRpps.size() > 0) {
		std::vector<std::string>* V = &TempRpps[TempName];
		FiledNums = V->size();
		return &(*V)[0];
	}
	return 0;
}


short rxsResultReportGrpsManger::ResultReportAdd(std::string TempName, std::string rpName) {
	if (!TempExist(TempName)) {
		return -1;
	}
	//std::vector<std::string> tempFiled = TempRpps[TempName];
	rxsResultReport rp(Rpps.size(), this, TempName.c_str(), rpName.c_str());
	//rp.SetFiled(&tempFiled[0], tempFiled.size(), rpName);
	Rpps.push_back(rp);
	nameMapID[rp.Name()] = rp.ID();
	return 0;
}

short rxsResultReportGrpsManger::ResultReportPush(rxsResultReport rpp) {
	map<string, int>::iterator mitr = nameMapID.find(rpp.Name());
	if (mitr != nameMapID.end()) {
		Rpps[mitr->second] = rpp;
	}
	else {
		nameMapID[rpp.Name()] = Rpps.size();
		Rpps.push_back(rpp);
	}
	return 0;
}


void rxsResultReportGrpsManger::ResultReportClr() {
	Rpps.clear();
}

void rxsResultReportGrpsManger::ResultReportContentClr(rxsResultReport* spec) {
	if (spec == 0) {
		int len = Rpps.size();
		if (len > 0) {
			for (unsigned x = 0; x < len; x++) {
				Rpps[x].RowsClr();
			}
		}
	}
	else {
		spec->RowsClr();
	}
}

std::string rxsResultReportGrpsManger::ResultReportSaveStringGet() {
	std::string content = "";
	int l = Rpps.size();
	if (l > 0) {
		for (unsigned x = 0; x < l; x++) {
			content += Rpps[x].SaveReportString();
		}
	}
	return content;
}

rxsResultReport* rxsResultReportGrpsManger::ResultReportGet(int index) {
	if (index < Rpps.size()) {
		return &Rpps[index];
	}
	return 0;
}

rxsResultReport* rxsResultReportGrpsManger::ResultReportGet(std::string rrpName) {
	if (Rpps.size() > 0 && nameMapID.size() > 0) {
		map<string, int>::iterator itr = nameMapID.find(rrpName);
		if (itr == nameMapID.end()) { return 0; }
		int id = nameMapID[rrpName];
		return ResultReportGet(id);
	}
	return  0;
}

bool rxsResultReportGrpsManger::TempExist(std::string tmpName) {
	return (TempRpps.find(tmpName) != TempRpps.end());
}






ResultReportGrpAssemble::ResultReportGrpAssemble()
{

}

ResultReportGrpAssemble::~ResultReportGrpAssemble()
{
	grps.clear();
}

std::string ResultReportGrpAssemble::GetString() {
	int l = grps.size();
	string rs = "";
	for (unsigned m = 0; m < l; m++) {
		rs += (grps[m].ResultReportSaveStringGet() + '\n');
	}
	return rs;
}

bool ResultReportGrpAssemble::Print() {
	string str = GetString();
	//调用打印机
	if (str.length() > 0) {


		return true;
	}
	return false;
}

void ResultReportGrpAssemble::flushResult(const char* path, const char* contData, unsigned cdLen) {
	FILE* file = fopen(path, "wt");
	if (file != NULL) {
		fwrite(contData, 1, cdLen, file);
		fclose(file);
	}
}


bool ResultReportGrpAssemble::Save(const char* path) {
	string str = GetString();
	if (str.length() > 0 && path != 0)
	{
		flushResult(path, str.c_str(), str.length());
		return true;
	}
	return false;
}

void ResultReportGrpAssemble::Clr() {
	grps.clear();
}

unsigned ResultReportGrpAssemble::Count() {
	return grps.size();
}

void ResultReportGrpAssemble::Push(rxsResultReportGrpsManger* p) {
	rxsResultReportGrpsManger v = *p;
	grps.push_back(v);
}