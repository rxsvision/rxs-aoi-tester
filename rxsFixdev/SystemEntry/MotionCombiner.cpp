
#include "rxsMainFrame.h"
#include "private.h"


using namespace std;


#define motionGpioServPtr	((MotionGpioSerialize*)mgpio)


MotionCombiner::MotionCombiner(void* mgp) {
	mgpio = mgp;
	forIter = GrpManger.begin();
}

MotionCombiner::~MotionCombiner() {
	for (std::map<std::string, MotionGrpCombiner>::iterator A = GrpManger.begin(); A != GrpManger.end(); A++) {
		if (A->second.SalveAxList != 0) {
			delete[] A->second.SalveAxList;
			A->second.SalveAxList = 0;
		}
	}
	GrpManger.clear();
	mgpio = 0;
}

bool MotionCombiner::Creater(std::string name, MotionModel Mode, std::string masterAxName, std::string* salveNames, unsigned salveNums) {
	if (salveNums > 0) {
		std::map<std::string, MotionGrpCombiner>::iterator mgcIter = GrpManger.find(name);
		if (mgcIter == GrpManger.end()) {
			unsigned hasfill = 0;
			string* slist = new string[salveNums];
			if (salveNames != 0) {
				for (; hasfill < salveNums; hasfill++) {
					slist[hasfill] = salveNames[hasfill];
				}
			}
			MotionGrpCombiner mgc = { Mode,masterAxName,slist,salveNums,hasfill };
			GrpManger[name] = mgc;
			return true;
		}
	}
	return false;
}

bool MotionCombiner::Appened(std::string name, std::string salveNames) {
	if (GrpManger.size() < 1) { return 0; }
	std::map<std::string, MotionGrpCombiner>::iterator mgcIter = GrpManger.find(name);
	if (mgcIter == GrpManger.end()) { return false; }

	if (mgcIter->second.hasFillSalve < mgcIter->second.SlaveNum) {
		mgcIter->second.SalveAxList[mgcIter->second.hasFillSalve] = salveNames;
		mgcIter->second.hasFillSalve += 1;
	}
	else {
		string* src = mgcIter->second.SalveAxList;
		int ns = mgcIter->second.SlaveNum * 2;
		mgcIter->second.SalveAxList = new string[ns];
		memcpy(mgcIter->second.SalveAxList, src, sizeof(std::string) * mgcIter->second.SlaveNum);
		mgcIter->second.SlaveNum = ns;
		delete[] src;
		src = 0;
		mgcIter->second.SalveAxList[mgcIter->second.hasFillSalve] = salveNames;
		mgcIter->second.hasFillSalve += 1;
	}
	return true;
}

void MotionCombiner::Remove(std::string name) {
	std::map<std::string, MotionGrpCombiner>::iterator mgcIter = GrpManger.find(name);
	if (mgcIter != GrpManger.end()) {
		delete[] mgcIter->second.SalveAxList;
		mgcIter->second.SalveAxList = 0;
		GrpManger.erase(name);
	}
}

const char* MotionCombiner::Foreach(bool isStart) {
	if (GrpManger.size() < 1) { return 0; }
	if(isStart){ forIter = GrpManger.begin(); }
	if (forIter != GrpManger.end()) {
		const char* v = forIter->first.c_str();
		forIter++;
		return v;
	}
	else {
		forIter = GrpManger.begin();
	}
	return 0;
}

MotionGrpCombiner* MotionCombiner::Get(std::string name) {
	if (GrpManger.size() < 1) { return 0; }
	std::map<std::string, MotionGrpCombiner>::iterator mgcIter = GrpManger.find(name);
	if (mgcIter != GrpManger.end()) {
		return &mgcIter->second;
	}
	return 0;
}

std::string MotionCombiner::GetSaveCfgString(std::string name) {
	if (GrpManger.size() < 1) { return ""; }
	std::map<std::string, MotionGrpCombiner>::iterator A = GrpManger.find(name);
	if (A == GrpManger.end()) { return ""; }
	std::stringstream ss;
	ss << std::fixed << std::setprecision(5);
	MotionGrpCombiner mgc = A->second;
	ss << mgc.GrpMode << ',' << mgc.MatserAx << ',' << mgc.SlaveNum;
	for (size_t i = 0; i < mgc.SlaveNum; i++)
	{
		ss << ',' << mgc.SalveAxList[i];
	}
	return ss.str();
}

bool MotionCombiner::ImportCfgString(const char* str) {

	return true;
}

bool MotionCombiner::ImportCfgString(std::string name, std::string content) {
	std::map<std::string, MotionGrpCombiner>::iterator A = GrpManger.find(name);
	if (A == GrpManger.end()) {
		vector<string> sv = stringSplit(content, ',');
		int SlaveNum = atoi(sv[2].c_str());
		string* sAlist = new string[SlaveNum];
		int x = 0;
		for (; x < SlaveNum; x++) {
			sAlist[x] = sv[3 + x];
		}
		MotionGrpCombiner mgc = { (MotionModel)atoi(sv[0].c_str()) ,sv[1],sAlist,SlaveNum ,x };
		GrpManger[name] = mgc;
	}
	return false;
}