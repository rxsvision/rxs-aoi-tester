
#include "rxsMainFrame.h"
#include "private.h"

#include <io.h>
#include <stdio.h>
#include <string>


using namespace inkType;
using namespace std;


extern swGtxparam* swGtxper;

FactoryConfigManger::FactoryConfigManger() {
	file = 0;
	selectedRow = 0;
	path = new inkString;
	zzStoreKvp = new map<string, Vpd>;
	zzStoreMp = new vector<vector<string>>;
	itr = new map<string, Vpd>::iterator;
	retVal = { 0,0,0 };
}

FactoryConfigManger::~FactoryConfigManger() {
	delete (inkString*)path;
	delete (map<string, Vpd> *)zzStoreKvp;
	delete (vector<vector<string>>*)zzStoreMp;
	delete (map<string, Vpd>::iterator*)itr;
	path = 0;
	zzStoreKvp = 0;
	zzStoreMp = 0;
	itr = 0;
	if (file != 0) {
		fclose((FILE*)file);
		file = 0;
	}
}

void FactoryConfigManger::analysis() {
	FILE* afile = (FILE*)file;
	char buff[256] = { 0 };
	unsigned slen = 0;
	string sk;
	int i = 0;
	//while (fscanf(afile, "%s\n", buff) != EOF) {//内容中带 空格 的不好处理
	while (fgets(buff, 256, afile)) {
		slen = strlen(buff);
		if (slen > 1) {
			buff[slen - 1] = 0;//将\n 置零结束
			if (buff[slen - 2] == '\x03') {
				buff[slen - 2] = 0;
				sk = buff;
				i = 0;
				continue;
			}
			if (sk == "MPPS") {
				vector<string> rms = stringSplit(buff, '\x1F');
				if (rms.size() > 0) {
					((vector<vector<string>>*)zzStoreMp)->push_back(rms);
				}
			}
			if (sk == "KVPS") {
				vector<string> rks = stringSplit(buff, '\x1F');
				if (rks.size() > 2) {
					Vpd vd = { rks[1],rks[2] };
					(*(map<string, Vpd> *)zzStoreKvp)[rks[0]] = vd;
				}
			}
			if (sk == "USMGS") {
				vector<string> rks = stringSplit(buff, '\x1F');
				if (rks.size() > 1) {
					((swGtxparam*)swGtxper)->Users[rks[0]] = rks[1];
				}
			}
		}
	}
}

bool FactoryConfigManger::Open() {
	file = fopen(((inkString*)path)->chrString(), "r");
	if (file != 0) {
		fseek((FILE*)file, 0, SEEK_SET);
		analysis();
		fclose((FILE*)file);
		file = 0;
		return  true;
	}
	return false;
}

RXSAPIS bool FactoryConfigManger::Ready(char* fpath) {
	if (fpath != NULL) {
		(*(inkString*)path) = fpath;
		return Open();
	}
	return false;
}

RXSAPIS bool FactoryConfigManger::KvpUpdater(const char* key, const char* Val, const char* Desc) {
	if ((*(map<string, Vpd> *)zzStoreKvp).find(key) == (*(map<string, Vpd> *)zzStoreKvp).end()) {
		Vpd v = { Val,(Desc == 0 ? "保留" : Desc) };
		(*(map<string, Vpd> *)zzStoreKvp)[key] = v;
		return true;
	}
	if (Val != 0)
		(*(map<string, Vpd> *)zzStoreKvp)[key].val = Val;
	if (Desc != 0)
		(*(map<string, Vpd> *)zzStoreKvp)[key].des = Desc;
	return true;
}

RXSAPIS bool FactoryConfigManger::MotionParamUpdater(int rowIndex, int colIndex, const char* val) {
	vector<vector<string>>* ZZsm = (vector<vector<string>>*)zzStoreMp;
	if ((ZZsm->size() - rowIndex) < 1) {
		vector<string> ni;
		ZZsm->push_back(ni);
	}
	vector<string>* cgrps = &(*ZZsm)[rowIndex];
	if (colIndex < (*cgrps).size()) {
		(*cgrps)[colIndex] = val;
	}
	else {
		(*cgrps).push_back(val);
	}
	return true;
}


void FactoryConfigManger::ssConvert(void* sd) {
	string* A = (string*)sd;
	(*A) = "MPPS\x03\n";
	vector<vector<string>>* ZZsm = (vector<vector<string>>*)zzStoreMp;
	unsigned rl = ZZsm->size();
	for (unsigned ri = 0; ri < rl; ri++) {
		vector<string>* cols = &(*ZZsm)[ri];
		unsigned cl = cols->size();
		for (unsigned ci = 0; ci < cl; ci++) {
			(*A) += ((*cols)[ci] + '\x1F');
		}
		(*A)[(*A).length() - 1] = '\n';
	}
	(*A) += "KVPS\x03\n";
	map<string, Vpd>* mapKvp = (map<string, Vpd> *)zzStoreKvp;
	for (auto iter = (*mapKvp).begin(); iter != (*mapKvp).end(); iter++) {
		(*A) += (iter->first + '\x1F' + iter->second.val + '\x1F' + iter->second.des + '\n');
	}
	(*A) += "USMGS\x03\n";
	map<string, string>* mapUsers = &((swGtxparam*)swGtxper)->Users;
	for (auto iter = (*mapUsers).begin(); iter != (*mapUsers).end(); iter++)
	{
		(*A) += (iter->first + '\x1F' + iter->second + '\n');
	}
}


RXSAPIS bool FactoryConfigManger::Save(char* fpath) {
	if (fpath == 0) {
		fpath = ((inkString*)path)->chrString();
	}
	file = fopen(fpath, "wt");
	if (file != NULL) {
		string wc;
		ssConvert(&wc);
		fwrite(wc.c_str(), 1, wc.size(), (FILE*)file);
		fclose((FILE*)file);
		file = 0;
		return true;
	}
	return false;
}


RXSAPIS kvpdChr* FactoryConfigManger::KvpReadForech(bool reStart) {
	map<string, Vpd>* mapKvp = (map<string, Vpd> *)zzStoreKvp;
	if (mapKvp->size() > 0) {
		if (reStart) {
			*(map<string, Vpd>::iterator*) itr = (*mapKvp).begin();
			return 0;
		}
		map<string, Vpd>::iterator* Aitr = (map<string, Vpd>::iterator*)itr;
		if ((*Aitr) == (*mapKvp).end()) {
			return 0;
		}
		retVal = { (*Aitr)->first.c_str(),(*Aitr)->second.val.c_str(),(*Aitr)->second.des.c_str() };
		(*Aitr)++;
		return &retVal;
	}
	return 0;
}

kvpdChr* FactoryConfigManger::KvpRead(const char* key) {
	map<string, Vpd>* mapKvp = (map<string, Vpd> *)zzStoreKvp;
	if (mapKvp->size() < 1) { return 0; }
	if ((*mapKvp).find(key) == (*mapKvp).end()) { return 0; }
	Vpd* v = &(*mapKvp)[key];
	retVal = { key,v->val.c_str() ,v->des.c_str() };
	return &retVal;
}


RXSAPIS const char* FactoryConfigManger::operator[](const char* key) {
	map<string, Vpd>* mapKvp = (map<string, Vpd> *)zzStoreKvp;
	return (*mapKvp)[key].val.c_str();
}


RXSAPIS unsigned FactoryConfigManger::SelectedRow(unsigned rowIndex) {
	vector<vector<string>>* ZZsm = (vector<vector<string>>*)zzStoreMp;
	if (ZZsm->size() > rowIndex) {
		selectedRow = &(*ZZsm)[rowIndex];
		return ((vector<string> *)selectedRow)->size();
	}
	return 0;
}

RXSAPIS const char* FactoryConfigManger::operator[](unsigned colIndex) {
	if (selectedRow != 0) {
		if (((vector<string> *)selectedRow)->size() > colIndex) {
			return (*(vector<string> *)selectedRow)[colIndex].c_str();
		}
	}
	return 0;
}