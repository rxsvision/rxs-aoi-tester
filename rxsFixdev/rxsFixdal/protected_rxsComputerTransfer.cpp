#pragma once

#include "rxsFixdal.h"
#include "protected_rxsComputerTransferManger.h"

using namespace std;




#if _NO_PRODUCT_USED_


void rxsRoiAreaManger::removenames(const char* name) {
	if (name == 0) { return; }
	namesGrps.erase(name);
}

bool rxsRoiAreaManger::Remove(roiArea rv) {
	vector<roiArea>::iterator itr = std::find(roiGrps.begin(), roiGrps.end(), rv);
	removenames(itr->forName);
	roiGrps.erase(itr);
	updateNames();
	return true;
}

bool rxsRoiAreaManger::Remove(const char* name) {
	map<string, int>::iterator nMiter = namesGrps.find(name);
	if (nMiter != namesGrps.end()) {
		int index = nMiter->second;
		Remove(roiGrps[index]);
	}
	return true;
}

bool rxsRoiAreaManger::Add(const char* Name, rxsPointCouldp coordp[3], rxsPointCouldp* Entiy, unsigned long pNums, u8 type) {
	map<string, int>::iterator nMiter = namesGrps.find(Name);
	if (nMiter != namesGrps.end()) {
		return false;
	}
	roiArea ra;
	memcpy(&ra, coordp, sizeof(rxsPointCouldp) * 3);
	ra.ce.Entiy = Entiy;
	ra.ce.pNums = pNums;
	ra.id = roiGrps.size();
	namesGrps[Name] = ra.id;
	ra.Type = type;
	roiGrps.push_back(ra);
	nMiter = namesGrps.find(Name);
	roiGrps[roiGrps.size() - 1].forName = nMiter->first.c_str();
	return true;
}

void rxsRoiAreaManger::Clear() {
	roiGrps.clear();
	namesGrps.clear();
}

roiArea* rxsRoiAreaManger::Get(unsigned index) {
	if (index < roiGrps.size()) {
		return &roiGrps[index];
	}
	return 0;
}

roiArea* rxsRoiAreaManger::Get(int ID) {
	for (unsigned x = 0; x < roiGrps.size(); x++) {
		if (roiGrps[x].id == ID) {
			return &roiGrps[x];
		}
	}
	return 0;
}

roiArea* rxsRoiAreaManger::Get(std::string forName, int& index) {
	for (unsigned x = 0; x < roiGrps.size(); x++) {
		if (memcmp(roiGrps[x].forName, forName.c_str(), forName.length()) == 0) {
			index = x;
			return &roiGrps[x];
		}
	}
	return 0;
}

int rxsRoiAreaManger::Get(roiArea*) {

	return 0;
}

unsigned rxsRoiAreaManger::Count() {
	return roiGrps.size();
}

std::string rxsRoiAreaManger::SaveStringGet() {
	if (roiGrps.size() > 0) {
		std::stringstream ss;
		ss << std::fixed << std::setprecision(5);
		for (unsigned x = 0; x < roiGrps.size(); x++) {
			ss << roiGrps[x].id << "," << roiGrps[x].forName << "," << (roiGrps[x].Type + 1) << ",";
			ss << roiGrps[x].P[0].x << "," << roiGrps[x].P[0].y << "," << roiGrps[x].P[0].z << ",";
			ss << roiGrps[x].P[1].x << "," << roiGrps[x].P[1].y << "," << roiGrps[x].P[1].z << ",";
			ss << roiGrps[x].P[2].x << "," << roiGrps[x].P[2].y << "," << roiGrps[x].P[2].z << "\n";
		}
		return ss.str();
	}
	return "";
}

short rxsRoiAreaManger::fill(std::string str) {
	vector<string> val = stringSplit(str, '\n');
	vector<string> cval;
	for (int x = 0; x < val.size(); x++) {
		cval = stringSplit(val[x], ',');
		int clen = cval.size();
		roiArea ra;
		ra.id = atoi(cval[0].c_str());
		namesGrps[cval[1]] = ra.id;
		if (clen > 2)
			ra.Type = (atoi(cval[2].c_str()) - 1);
		if (clen > 3)
			ra.P[0].x = atoi(cval[3].c_str());
		if (clen > 4)
			ra.P[0].y = atoi(cval[4].c_str());
		if (clen > 5)
			ra.P[0].z = atoi(cval[5].c_str());
		if (clen > 6)
			ra.P[1].x = atoi(cval[6].c_str());
		if (clen > 7)
			ra.P[1].y = atoi(cval[7].c_str());
		if (clen > 8)
			ra.P[1].z = atoi(cval[8].c_str());
		if (clen > 9)
			ra.P[2].x = atoi(cval[9].c_str());
		if (clen > 10)
			ra.P[2].y = atoi(cval[10].c_str());
		if (clen > 11)
			ra.P[2].z = atoi(cval[11].c_str());

		ra.ce.Entiy = 0;
		ra.ce.pNums = 0;
		roiGrps.push_back(ra);
		map<string, int>::iterator nMiter = namesGrps.find(cval[1]);
		roiGrps[roiGrps.size() - 1].forName = nMiter->first.c_str();
	}
	return 0;
}

void rxsRoiAreaManger::updateNames() {
	for (map<string, int>::iterator m = namesGrps.begin(); m != namesGrps.end(); m++) {
		roiGrps[m->second].forName = m->first.c_str();
	}
}

short rxsRoiAreaManger::PushCfgString(std::string s) {
	if (s.length() > 0) {
		return fill(s);
	}
	return -1;
}





#else

rxsRoiAreaManger::~rxsRoiAreaManger() {
	for (size_t n = 0; n < roiGrps.size(); n++) {
		if (roiGrps[n].CE.Entiy != 0) {
			free(roiGrps[n].CE.Entiy);
			roiGrps[n].CE.pNums = 0;
			roiGrps[n].CE.Entiy = 0;
		}
	}
	roiGrps.clear();
}


bool rxsRoiAreaManger::Remove(RoiArea rv) {
	vector<RoiArea>::iterator itr = std::find(roiGrps.begin(), roiGrps.end(), rv);
	if (itr != roiGrps.end()) {
		roiGrps.erase(itr);
	}
	return true;
}

bool rxsRoiAreaManger::Remove(std::string name) {
	auto fitr = std::find(roiGrps.begin(), roiGrps.end(), name);
	if (fitr != roiGrps.end()) {
		roiGrps.erase(fitr);
	}
	return true;
}

bool rxsRoiAreaManger::Add(std::string Name, rxsPointCouldp coordp[3], rxsPointCouldp* Entiy, unsigned long pNums, u8 flag) {
	RoiArea ra;
	ra.CE = { 0,0 };
	ra.Area.rName = Name;
	ra.Area.SORC = coordp[0];
	ra.Area.End = coordp[1];
	ra.Area.LWH = coordp[2];
	if (Entiy != 0) {
		ra.CE.Entiy = (rxsPointCouldp*)malloc(sizeof(rxsPointCouldp) * pNums);
		if (ra.CE.Entiy != 0) {
			ra.CE.pNums = pNums;
			memcpy(ra.CE.Entiy, Entiy, sizeof(rxsPointCouldp) * pNums);
		}
	}
	ra.id = roiGrps.size();
	ra.Area.Flag = flag;
	auto fitr = std::find(roiGrps.begin(), roiGrps.end(), Name);
	if (fitr != roiGrps.end()) {
		*fitr = ra;
	}
	else {
		roiGrps.push_back(ra);
	}
	return true;
}

void rxsRoiAreaManger::Clear() {
	for (size_t n = 0; n < roiGrps.size(); n++) {
		if (roiGrps[n].CE.Entiy != 0) {
			free(roiGrps[n].CE.Entiy);
			roiGrps[n].CE.pNums = 0;
			roiGrps[n].CE.Entiy = 0;
		}
	}
	roiGrps.clear();
}

RoiArea* rxsRoiAreaManger::Get(unsigned index) {
	if (index < roiGrps.size()) {
		return &roiGrps[index];
	}
	return 0;
}

RoiArea* rxsRoiAreaManger::Get(int ID) {
	for (unsigned x = 0; x < roiGrps.size(); x++) {
		if (roiGrps[x].id == ID) {
			return &roiGrps[x];
		}
	}
	return 0;
}

RoiArea* rxsRoiAreaManger::Get(std::string roiName, int& index) {
	for (unsigned x = 0; x < roiGrps.size(); x++) {
		if (roiGrps[x].Area.rName == roiName) {
			index = x;
			return &roiGrps[x];
		}
	}
	return 0;
}

int rxsRoiAreaManger::Get(RoiArea*) {

	return 0;
}

unsigned rxsRoiAreaManger::Count() {
	return roiGrps.size();
}

std::string rxsRoiAreaManger::SaveStringGet() {
	if (roiGrps.size() > 0) {
		std::stringstream ss;
		ss << std::fixed << std::setprecision(5);
		int flag = 0;
		for (unsigned x = 0; x < roiGrps.size(); x++) {
			flag = roiGrps[x].Area.Flag;
			ss << roiGrps[x].id << "," << roiGrps[x].Area.rName << "," << std::to_string(flag) << ",";
			ss << roiGrps[x].Area.SORC.x << "," << roiGrps[x].Area.SORC.y << "," << roiGrps[x].Area.SORC.z << ",";
			ss << roiGrps[x].Area.End.x << "," << roiGrps[x].Area.End.y << "," << roiGrps[x].Area.End.z << ",";
			ss << roiGrps[x].Area.LWH.x << "," << roiGrps[x].Area.LWH.y << "," << roiGrps[x].Area.LWH.z << "\n";
		}
		return ss.str();
	}
	return "";
}

short rxsRoiAreaManger::fill(std::string str) {
	//vector<string> val = stringSplit(str, '\n');
	//vector<string> cval;
	//for (int x = 0; x < val.size(); x++) {
	//	cval = stringSplit(val[x], ',');
	//	int clen = cval.size();
	//	RoiArea ra;
	//	ra.id = atoi(cval[0].c_str());
	//	namesGrps[cval[1]] = ra.id;
	//	if (clen > 2)
	//		ra.Type = (atoi(cval[2].c_str()) - 1);
	//	if (clen > 3)
	//		ra.P[0].x = atoi(cval[3].c_str());
	//	if (clen > 4)
	//		ra.P[0].y = atoi(cval[4].c_str());
	//	if (clen > 5)
	//		ra.P[0].z = atoi(cval[5].c_str());
	//	if (clen > 6)
	//		ra.P[1].x = atoi(cval[6].c_str());
	//	if (clen > 7)
	//		ra.P[1].y = atoi(cval[7].c_str());
	//	if (clen > 8)
	//		ra.P[1].z = atoi(cval[8].c_str());
	//	if (clen > 9)
	//		ra.P[2].x = atoi(cval[9].c_str());
	//	if (clen > 10)
	//		ra.P[2].y = atoi(cval[10].c_str());
	//	if (clen > 11)
	//		ra.P[2].z = atoi(cval[11].c_str());

	//	ra.CE.Entiy = 0;
	//	ra.CE.pNums = 0;
	//	roiGrps.push_back(ra);
	//	map<string, int>::iterator nMiter = namesGrps.find(cval[1]);
	//	roiGrps[roiGrps.size() - 1].forName = nMiter->first.c_str();
	//}
	return 0;
}


short rxsRoiAreaManger::PushCfgString(std::string s) {
	if (s.length() > 0) {
		return fill(s);
	}
	return -1;
}


#endif // _NO_PRODUCT_USED_
















rxsComputerTransfer::rxsComputerTransfer(const char* name, rxsRoiAreaManger* ram, rxsResultReportGrpsManger* rrgm, rxsToolKit* tk) {
	rtk = tk;
	roiGrpMgr = ram;
	resultReportGrpMgr = rrgm;
	CompleteCloud.Entiy = 0;
	CompleteCloud.pNums = 0;
	cName = name;
	callCount = 0;
	callFinshedCount = 0;
}

rxsComputerTransfer::rxsComputerTransfer(std::string str, rxsRoiAreaManger* ram, rxsResultReportGrpsManger* rrgm, rxsToolKit* tk) {
	CompleteCloud.Entiy = 0;
	CompleteCloud.pNums = 0;
	rtk = tk;
	roiGrpMgr = ram;
	resultReportGrpMgr = rrgm;
	stringFill(str);
}

rxsComputerTransfer::~rxsComputerTransfer() {
	roidGrp.clear();
}

void rxsComputerTransfer::stringFill(std::string str) {
	vector<string> sV = stringSplit(str, ',');
	cName = sV[0];
	toolKitName = sV[1];

}

unsigned rxsComputerTransfer::roiCount() {
	return roidGrp.size();
}

const char* rxsComputerTransfer::Name() {
	return cName.c_str();
}

void rxsComputerTransfer::roiClr() {
	roidGrp.clear();
}

void rxsComputerTransfer::roiPush(int id) {
	vector<int>::iterator viiter = std::find(roidGrp.begin(), roidGrp.end(), id);
	if (viiter == roidGrp.end()) {
		roidGrp.push_back(id);
	}
}

int* rxsComputerTransfer::roiGrps(unsigned& Nums) {
	Nums = roidGrp.size();
	if (Nums < 1) { return 0; }
	return &roidGrp[0];
}

void rxsComputerTransfer::ToolKitNameSet(std::string Name) {
	toolKitName = Name;
}

const char* rxsComputerTransfer::ToolKitName() {
	return toolKitName.c_str();
}

void rxsComputerTransfer::ResultReportSet(std::string rrName) {
	rpName = rrName;
}

const char* rxsComputerTransfer::ResultReportName(/*int& id*/) {
	if (rpName.length() == 0) {
		/*id = -1;*/
		return 0;
	}
	rxsResultReport* result = resultReportGrpMgr->ResultReportGet(rpName);
	if (result == 0) { return 0; }
	return result->Name();
}

void rxsComputerTransfer::ScanCloudSet(rxsPointCouldp* pointCloud, unsigned long pNums) {
	CompleteCloud.Entiy = pointCloud;
	CompleteCloud.pNums = pNums;
}

void rxsComputerTransfer::InputRoiSelected() {

}

bool rxsComputerTransfer::operator== (const rxsComputerTransfer& p1) {
	return((p1.cName == cName) && (p1.toolKitName == toolKitName));
}

bool rxsComputerTransfer::operator== (std::string name) {
	return(name == cName);
}

short rxsComputerTransfer::Run(u8 dir, int curid) {
	if (rtk == 0) { return -11; }
	rxsResultReport* result = resultReportGrpMgr->ResultReportGet(rpName);
	if (result == 0) { return -12; }
	if (CompleteCloud.Entiy == 0 || CompleteCloud.pNums < 1) { return -13; }
	int roinum = roidGrp.size();
	callCount += 1;
	if (roinum > 0) {
		//roiArea* ROIs = (roiArea*)malloc(sizeof(roiArea) * roinum);
		//for (int m = 0; m < roinum; m++) {
		//	roiArea* A = roiGrpMgr->Get(roidGrp[m]);
		//	if (A != 0 && ROIs != 0) {
		//		memcpy(&ROIs[m], A, sizeof(roiArea));
		//	}
		//}

		RoiArea* ROIs = (RoiArea*)malloc(sizeof(RoiArea) * roinum);
		for (int m = 0; m < roinum; m++) {
			RoiArea* A = roiGrpMgr->Get(roidGrp[m]);
			if (A != 0 && ROIs != 0) {
				memcpy(&ROIs[m], A, sizeof(RoiArea));
			}
		}
		short ret = rtk->ToolKitCaller(toolKitName, dir, CompleteCloud, result, ROIs, roinum, curid, 0);
		free(ROIs);
		ROIs = 0;
		callFinshedCount += 1;
		return ret;
	}
	else {
		short rr = rtk->ToolKitCaller(toolKitName, dir, CompleteCloud, result, 0, 0, curid, 0);
		callFinshedCount += 1;
		return rr;
	}
}

bool rxsComputerTransfer::AllRunOk() {
	//rxsResultReport* rpp = resultReportGrpMgr->ResultReportGet(rpName);
	//int rc = rpp->RowCount();
	if (callFinshedCount == callCount) {
		callCount = 0;
		callFinshedCount = 0;
		return true;
	}
	return false;
}

std::string rxsComputerTransfer::SaveStringGet() {
	int roin = roidGrp.size();
	rxsResultReport* result = resultReportGrpMgr->ResultReportGet(rpName);
	std::stringstream ss;
	ss << std::fixed << std::setprecision(5);
	ss << cName << "," << toolKitName << "," << result->TempName() << "," << result->Name() << "," << roin;
	if (roin > 0) {
		ss << "," << roidGrp[0];
		for (unsigned x = 1; x < roin; x++) {
			ss << "," << roidGrp[x];
		}
	}
	ss << "\n";
	return ss.str();
}







rxsComputerTransferManger::rxsComputerTransferManger(rxsRoiAreaManger* ram, rxsResultReportGrpsManger* rrrgm, rxsToolKit* rtk) {
	roiGrp = ram;
	rrGrp = rrrgm;
	rtks = rtk;
}

rxsComputerTransferManger::~rxsComputerTransferManger() {
	ctNameMap.clear();
	ctGrps.clear();
	roiGrp = 0;
	rrGrp = 0;
	rtks = 0;
}

bool rxsComputerTransferManger::exist(std::string comName) {
	return (ctNameMap.find(comName) != ctNameMap.end());
}

short rxsComputerTransferManger::Add(const char* comName, const char* roiName, const char* ToolKitName, const char* rrName) {
	if (ToolKitName == 0 || rrName == 0) {
		return -1;
	}
	rxsComputerTransfer ct(comName, roiGrp, rrGrp, rtks);
	if (roiName != 0) {
		int roiIndex = -1;
		//roiArea* ra = roiGrp->Get(roiName, roiIndex);
		RoiArea* ra = roiGrp->Get(roiName, roiIndex);
		if (ra == 0) { return -2; }
		ct.roiPush(ra->id);
	}
	ct.ToolKitNameSet(ToolKitName);
	ct.ResultReportSet(rrName);
	ctGrps.push_back(ct);
	ctNameMap[ct.Name()] = &ctGrps[ctGrps.size() - 1];
	return 0;
}

short rxsComputerTransferManger::Update(const char* comName, const char* roiName, const char* ToolKitName, const char* rrName) {
	rxsComputerTransfer* rct = ctNameMap[comName];
	if (rct == 0) {
		return -1;
	}
	if (roiName != 0) {
		int roiIndex = 0;
		//roiArea* ra = roiGrp->Get(roiName, roiIndex);
		RoiArea* ra = roiGrp->Get(roiName, roiIndex);
		if (ra == 0) { return -2; }
		rct->roiPush(ra->id);
	}
	if (ToolKitName != 0) {
		rct->ToolKitNameSet(ToolKitName);
	}
	if (rrName != 0) {
		rct->ResultReportSet(rrName);
	}
	return 0;
}


short rxsComputerTransferManger::remove(rxsComputerTransfer* ctr) {
	rxsComputerTransfer val = *ctr;
	std::vector<rxsComputerTransfer>::iterator itr = std::find(ctGrps.begin(), ctGrps.end(), val);
	ctGrps.erase(itr);
	return 0;
}

short rxsComputerTransferManger::Remove(std::string Name) {
	map<std::string, rxsComputerTransfer*>::iterator itr = ctNameMap.find(Name);
	if (itr != ctNameMap.begin()) {
		ctNameMap.erase(Name);
		remove(itr->second);
	}
	return 0;
}

rxsComputerTransfer* rxsComputerTransferManger::Get(std::string comName) {
	unsigned nums = ctGrps.size();
	if (nums > 0) {
		std::vector<rxsComputerTransfer>::iterator itr = std::find(ctGrps.begin(), ctGrps.end(), comName);
		if (itr != ctGrps.end()) {
			int i = itr - ctGrps.begin();
			return &ctGrps[i];
		}
	}
	return 0;
}

rxsComputerTransfer* rxsComputerTransferManger::Get(unsigned index) {
	unsigned nums = ctGrps.size();
	if (index < nums) {
		return &ctGrps[index];
	}
	return 0;
}

int rxsComputerTransferManger::Count() {
	return ctGrps.size();
}

void rxsComputerTransferManger::Clear() {
	ctGrps.clear();
	ctNameMap.clear();
}

std::string rxsComputerTransferManger::CfgSaveString() {
	string content = "";
	for (unsigned x = 0; x < ctGrps.size(); x++) {
		content += ctGrps[x].SaveStringGet();
	}
	return content;
}

void rxsComputerTransferManger::CfgStringSet(const char* str) {
	if (str == 0) { return; }
	string content = str;
	vector<string> sV = stringSplit(content, '\n');
	int l = sV.size();
	if (l > 0) {
		for (unsigned x = 0; x < l; x++) {
			vector<string> ssV = stringSplit(sV[x], ',');
			rxsComputerTransfer rct(ssV[0].c_str(), roiGrp, rrGrp, rtks);
			rct.ToolKitNameSet(ssV[1]);
			if (rrGrp->ResultReportAdd(ssV[2], ssV[3]) == 0) {
				rct.ResultReportSet(ssV[3]);
			}
			int num = atoi(ssV[4].c_str());
			if (num > 0) {
				for (int n = 0; n < num; n++) {
					rct.roiPush(atoi(ssV[5 + n].c_str()));
				}
			}
			ctGrps.push_back(rct);
		}
		namsmapUpdate();
	}
}

void rxsComputerTransferManger::namsmapUpdate() {
	ctNameMap.clear();
	for (int m = 0; m < ctGrps.size(); m++) {
		rxsComputerTransfer* rxctr = &ctGrps[ctGrps.size() - 1];
		ctNameMap[rxctr->Name()] = rxctr;
	}
}

bool rxsComputerTransferManger::isCompleted() {
	for (int m = 0; m < ctGrps.size(); m++) {
		if (!ctGrps[m].AllRunOk())
		{
			return false;
		}
	}
	return true;
}
