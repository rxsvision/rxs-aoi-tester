#include "public.h"

#include <io.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;
#pragma warning(disable : 4996)

rxsResultReport::rxsResultReport() {
	rof = 0;
}

rxsResultReport::rxsResultReport(rxsResultReportGrpsManger* rrgm, const char* tempName, const char* rrName) {

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
}

const char* rxsResultReport::TempName() { return forName.c_str(); }
const char* rxsResultReport::Name() { return name.c_str(); }

void rxsResultReport::outFilter(int v) {
	if (v < 0) {
		filters.clear();
		return;
	}

	filters.push_back(v);
}

void rxsResultReport::outFormatSet(bool rno, bool rhead, bool isRows) {
	if (rno) {
		rof |= 1;
	}
	else {
		rof &= (~1);
	}

	if (rhead) {
		rof |= 2;
	}
	else {
		rof &= (~2);
	}

	if (isRows) {
		rof |= 4;
	}
	else {
		rof &= (~4);
	}
}


void rxsResultReport::FieldRemove(std::string fv) {
	auto iter = std::find(facFiledGrps.begin(), facFiledGrps.end(), fv);
	if (iter != facFiledGrps.end()) {
		facFiledGrps.erase(iter);
	}
}

short rxsResultReport::SetFiled(std::string* filedName, unsigned len, std::string rpName) {
	facFiledGrps.clear();
	name = rpName;
	return SetFiledAppend(filedName, len);
}

short rxsResultReport::SetFiledAppend(std::string* filedName, unsigned len) {
	for (unsigned x = 0; x < len; x++) {
		auto iter = std::find(facFiledGrps.begin(), facFiledGrps.end(), filedName[x]);
		if (iter != facFiledGrps.end()) {
			continue;
		}
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
		int x = fmapValues.size();
		std::map<std::string, std::string>* fmapVal = 0;
		if (x > 0 && rowIndex < x) {

			vector<string>::iterator has = std::find(facFiledGrps.begin(), facFiledGrps.end(), filedName);
			if (has == facFiledGrps.end()) {
				return -2;
			}

			fmapVal = &fmapValues[rowIndex];
			(*fmapVal)[filedName] = value;
			return 0;
		}
	}
	return -1;
}

bool rxsResultReport::hasFiled(const char* cn) {
	auto fi = std::find(facFiledGrps.begin(), facFiledGrps.end(), cn);
	if (fi != facFiledGrps.end()) {
		return fi->c_str();
	}
	return 0;
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

rxsResultReport rxsResultReport::operator ()() {
	rxsResultReport rrr;
	rrr.facFiledGrps = facFiledGrps;
	rrr.fmapValues = fmapValues;
	rrr.name = name;
	rrr.forName = forName;
	return rrr;
}

bool rxsResultReport::operator ==(std::string t) {
	return (name == t);
}

bool rxsResultReport::operator ==(rxsResultReport& t) {
	bool ret = (t.name == name);
	if (!ret) { return false; }
	ret = (t.facFiledGrps == facFiledGrps);
	return ret;
}

void rxsResultReport::Merge(rxsResultReport& from) {
	if (from.name == name) {
		if (from.facFiledGrps.size() > facFiledGrps.size()) {
			return;
		}
		else {
			int addCount = from.fmapValues.size();
			for (size_t i = 0; i < addCount; i++) {
				fmapValues.push_back(from.fmapValues[i]);
			}
		}
	}
}

void rxsResultReport::clear() {
	facFiledGrps.clear();
	fmapValues.clear();
	name.clear();
	forName.clear();
}

std::string rxsResultReport::SaveReportString(bool& isOk_ng) {
	size_t vLen = facFiledGrps.size();
	int mvsLen = fmapValues.size();
	string fV;
	int mvLen = 0;
	isOk_ng = true;
	if (vLen < 1 || mvsLen < 1) {
		return fV;
	}
	std::stringstream ss;
	ss << std::fixed << std::setprecision(5);
	if ((rof & 1) == 1) {
		ss << "\r\n" << forName << "->" << name << ":\n";
	}
	if ((rof & 2) == 2) {
		//×Ö¶ÎÁÐÍ·
		for (size_t i = 0; i < vLen; i++) {
			ss << facFiledGrps[i] << ",";
		}
		ss << "\r\n";
	}


	for (unsigned x = 0; x < mvsLen; x++) {
		std::map<std::string, std::string>* fmapVal = &fmapValues[x];
		mvLen = (*fmapVal).size();
		if (mvLen == vLen) {
			for (unsigned x = 0; x < vLen; x++) {
				if (std::find(filters.begin(), filters.end(), x) == filters.end()) {
					string V = facFiledGrps[x];
					ss << (*fmapVal)[V] << ",";
					if (isOk_ng) {
						if (V[0] == 'W' || V[0] == 'K' || V[0] == 'A') {
							continue;
						}
						else {
							if ((*fmapVal)[V] == "-1" || (*fmapVal)[V] == "-1.0" || (*fmapVal)[V] == "0") {
								isOk_ng = false;
							}
						}
					}
				}
			}
		}
		else {
			for (unsigned x = 0; x < vLen; x++) {
				if (std::find(filters.begin(), filters.end(), x) == filters.end()) {
					string V = facFiledGrps[x];
					if ((*fmapVal).find(V) != (*fmapVal).end()) {
						ss << (*fmapVal)[V] << ",";
						if (isOk_ng) {
							if (V[0] == 'W' || V[0] == 'K' || V[0] == 'A') {
								continue;
							}
							else {
								if ((*fmapVal)[V] == "-1" || (*fmapVal)[V] == "-1.0" || (*fmapVal)[V] == "0") {
									isOk_ng = false;
								}
							}
						}
					}
					else {
						ss << "/null,";
					}
				}
			}
		}
		if ((rof & 4) == 4) {
			ss << "\r\n";
		}
	}

	fV = ss.str();
	return fV;
}