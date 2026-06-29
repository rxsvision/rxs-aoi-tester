#pragma once

#include "public.h"


using namespace std;
#pragma warning(disable : 4996)


rxsResultReportGrpsManger::rxsResultReportGrpsManger() {
	hasClr = false;
	//---后期3D流程部分优化掉后，此处这些列名也可以全部优化掉---
	allField.push_back("ID");
	allField.push_back("编号");
	allField.push_back("序号");
	allField.push_back("颜色");
	allField.push_back("名称");
	allField.push_back("轮廓度");
	allField.push_back("面轮廓度");
	allField.push_back("平面度");
	allField.push_back("条码内容");
	allField.push_back("变形度");
	allField.push_back("弧长(mm)");
	allField.push_back("投影长(mm)");
	allField.push_back("高度(mm)");
	allField.push_back("断差");
	allField.push_back("深度(mm)");
	allField.push_back("X位置(mm)");
	allField.push_back("Y位置(mm)");
	allField.push_back("上最大偏差");
	allField.push_back("下最大偏差");
	allField.push_back("总最大偏差");
	allField.push_back("变形度");
	allField.push_back("变形度V1");
	allField.push_back("变形度V2");
	allField.push_back("上落差");
	allField.push_back("下落差");
	allField.push_back("锋锐度");
	allField.push_back("变形类型");
	allField.push_back("预测长");
	allField.push_back("内P长宽");
	allField.push_back("GAP1");
	allField.push_back("GAP2");
	allField.push_back("PASS");
	mergeMode = true;
	isRowFormat = false;
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
	rxsResultReport rp(this, TempName.c_str(), rpName.c_str());
	//rp.SetFiled(&tempFiled[0], tempFiled.size(), rpName);
	nameMapID[rp.Name()] = Rpps.size();
	Rpps.push_back(rp);
	return 0;
}

short rxsResultReportGrpsManger::ResultReportPush(rxsResultReport rpp) {
	if (mergeMode) {
		bool hasFind = false;
		for (size_t i = 0; i < Rpps.size(); i++) {
			if (Rpps[i] == rpp.Name()) {//名称等即合并
				Rpps[i].Merge(rpp);
				hasFind = true;
				break;
			}
		}
		if (!hasFind) {
			nameMapID[rpp.Name()] = Rpps.size();
			Rpps.push_back(rpp);
		}
	}
	else {
		map<string, int>::iterator mitr = nameMapID.find(rpp.Name());
		if (mitr != nameMapID.end() && !hasClr) {
			int i = mitr->second;
			if (i < Rpps.size()) {
				Rpps[i] = rpp;
			}
		}
		else {
			nameMapID[rpp.Name()] = Rpps.size();
			Rpps.push_back(rpp);
		}
	}
	return 0;
}

int rxsResultReportGrpsManger::Count() {
	return Rpps.size();
}


void rxsResultReportGrpsManger::MergeMode(bool on) {
	mergeMode = on;
}

bool rxsResultReportGrpsManger::MergeMode() {
	return mergeMode;
}

rxsResultReport* rxsResultReportGrpsManger::SingleReportSpec(int tageIndex, int* fliterColsIndex, int num) {
	if (tageIndex < Rpps.size()) {
		rxsResultReport* rpp = &Rpps[tageIndex];
		if (rpp == 0) { return 0; }
		const char* colName = 0;
		int ci = 0;
		std::vector<std::string> cols;
		bool noFilter = true;
		do {
			noFilter = true;
			for (int fci = 0; fci < num; fci++) {
				if (fliterColsIndex[fci] == ci) {
					noFilter = false;
					break;
				}
			}
			if (noFilter) {
				if ((colName = rpp->GetFiled(ci)) == 0) {
					break;
				}
				cols.push_back(colName);
			}
			ci += 1;
		} while (true);


		for (int x = 0; x < Rpps.size(); x++) {
			if (x == tageIndex) { continue; }
			int r = Rpps[x].RowCount();
			for (int y = 0; y < cols.size(); y++) {
				if (Rpps[x].hasFiled(cols[y].c_str())) {
					for (int m = 0; m < r; m++) {
						const char* v = Rpps[x].GetValue(m, cols[y].c_str());
						if (v == 0) { continue; }
						rpp->SetValue(cols[y], v, m);
					}
				}
			}
		}
		return rpp;
	}
	return 0;
}

bool rxsResultReportGrpsManger::SingleReportMerge(rxsResultReport& out) {
	std::vector<std::string> allCols;
	std::vector<std::vector<std::string>> allVals;
	for (int x = 0; x < Rpps.size(); x++) {
		const char* cv = 0;
		int ci = 0;
		while ((cv = Rpps[x].GetFiled(ci)) != 0) {
			allCols.push_back(cv);
			int rc = Rpps[x].RowCount();
			std::vector<std::string> cmVal;
			for (int n = 0; n < rc; n++) {
				const char* vv = Rpps[x].GetValue(n, cv);
				if (vv == 0) {
					cmVal.push_back("null");
				}
				else {
					cmVal.push_back(vv);
				}
			}
			allVals.push_back(cmVal);
			ci += 1;
		}
	}
	if (allCols.size() > 0) {
		out.clear();
		out.SetFiled(&allCols[0], allCols.size(), "SRM_New");
		for (int y = 0; y < allVals.size(); y++) {
			for (int g = 0; g < allVals[y].size(); g++) {
				if (out.RowCount() > g) {
					out.SetValue(allCols[y], allVals[y][g], g);
				}
				else {
					out.AddRow();
					out.SetValue(allCols[y], allVals[y][g], g);
				}
			}
		}
		return true;
	}
	return false;
}


void rxsResultReportGrpsManger::ResultReportClr() {
	Rpps.clear();
	hasClr = true;
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
	bool isOk_ng = true;
	bool isOk = true;
	if (l > 0) {
		for (unsigned x = 0; x < l; x++) {
			if (isOk) {
				content += Rpps[x].SaveReportString(isOk);
			}
			else {
				content += Rpps[x].SaveReportString(isOk_ng);
			}
			if (isRowFormat) {
				content += "\n";
			}
			else {
				content += ",";
			}
		}
	}
	isOk_ng = isOk;
	content += "\n";
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
		int index = nameMapID[rrpName];
		//return ResultReportGet(id)
		if (index < Rpps.size()) {
			return &Rpps[index];
		}
	}
	return  0;
}

bool rxsResultReportGrpsManger::TempExist(std::string tmpName) {
	return (TempRpps.find(tmpName) != TempRpps.end());
}

void rxsResultReportGrpsManger::RemapColumnHeadSet(std::string src, std::string now) {
	chremap[src] = now;
}

std::string rxsResultReportGrpsManger::RemapColumnHeadGet(std::string src) {
	if (chremap.size() > 0) {
		return src;
	}
	auto iter = chremap.find(src);
	if (iter == chremap.end()) {
		return src;
	}
	return iter->second;
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
		rs += (grps[m]->ResultReportSaveStringGet() + '\n');
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
	rxsResultReportGrpsManger* v = 0;
	*v = *p;
	grps.push_back(v);
}

rxsResultReport* ResultReportGrpAssemble::operator[](const char* rrName) {
	rxsResultReport* r = 0;
	for (int i = 0; i < grps.size(); i++) {
		if ((r = grps[i]->ResultReportGet(rrName)) != 0) {
			return r;
		}
	}
	return r;
}
