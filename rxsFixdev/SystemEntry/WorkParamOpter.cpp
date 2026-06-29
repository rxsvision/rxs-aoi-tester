
#include "rxsMainFrame.h"
#include "private.h"

#include <map>
#include <vector>
#include <io.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <iomanip>


using namespace inkType;
using namespace std;


extern swGtxparam* swGtxper;

class ExtAppedSeg {
public:
	string SegName;
	string Content;

	bool operator==(ExtAppedSeg eas) {
		return (SegName == eas.SegName && Content == eas.Content);
	}

	bool operator==(string sname) {
		return (SegName == sname);
	}
};

typedef struct _thremap{
	std::string rmpName;
	bool enable = false;
}THRemap;

std::map<std::string, MeasurementJudgment> mjCfg;
std::map<std::string, THRemap> rrpRemap;

#define ExtAppended		(*(vector<ExtAppedSeg>*)extAppened)



WorkParamManger::WorkParamManger() {
	file = NULL;
	tsWich = 0;
	fn = new inkType::inkString;
	v_MPathp = new vector<Movepath>;
	v_SPointp = new vector<Scanpoint>;
	extAppened = new vector<ExtAppedSeg>;
}

WorkParamManger::~WorkParamManger() {
	if (file != NULL) {
		fclose((FILE*)file);
		file = NULL;
	}
	delete (inkType::inkString*)fn;
	fn = 0;
	delete (vector<Movepath>*)v_MPathp;
	v_MPathp = 0;
	delete (vector<Scanpoint>*)v_SPointp;
	v_SPointp = 0;
	delete (vector<ExtAppedSeg> *)extAppened;
	extAppened = 0;
}

RXSAPIS bool WorkParamManger::Load(const char* path) {
	if (path != NULL) {
		(*(inkString*)fn) = (char*)path;
	}
	else {
		path = (*(inkString*)fn).chrString();
	}
	file = fopen(path, "rt");
	if (file != NULL) {
		fseek((FILE*)file, 0, SEEK_SET);
		analysis();
		fclose((FILE*)file);
		file = 0;
		return  true;
	}
	return false;
}

RXSAPIS char* WorkParamManger::CurFilePath() {
	if (fn == 0) {
		return 0;
	}
	return  (*(inkString*)fn).chrString();
}

void WorkParamManger::Appended(const char* segName, const char* content) {
	vector<ExtAppedSeg>::iterator itr = std::find(ExtAppended.begin(), ExtAppended.end(), segName);
	if (itr != ExtAppended.end()) {
		itr->Content = content;
	}
	else {
		ExtAppedSeg eas;
		eas.SegName = segName;
		eas.Content = content;
		ExtAppended.push_back(eas);
	}
}


RXSAPIS void WorkParamManger::Save(const char* path) {
	if (path != NULL) {
		(*(inkString*)fn) = (char*)path;
	}
	else {
		path = (*(inkString*)fn).chrString();
	}
	file = fopen(path, "wt");//新建一个文本文件，已存在的文件将内容清空，只允许写
	if (file != NULL) {
		string wc;
		sConvert(&wc);
		fwrite(wc.c_str(), 1, wc.size(), (FILE*)file);
		fclose((FILE*)file);
		file = 0;
	}
}

void WorkParamManger::sConvert(void* iwc) {
	string* C = (string*)iwc;
	string zjv;
	(*C) = "USEPARAM:\n";
	int len = (((swGtxparam*)swGtxper)->UserparamsDesc).size();
	for (unsigned x = 0; x < len; x++) {
		zjv = (((swGtxparam*)swGtxper)->UserparamsDesc)[x].val;
		if (zjv.length() > 0)
			(*C) += zjv;
		else
			(*C) += "\x02\x03";
		(*C) += "\n";
	}

	(*C) += (char*)"WRKPARAM:\n";
	len = (((swGtxparam*)swGtxper)->WrkparamsDesc).size();
	for (unsigned x = 0; x < len; x++) {
		zjv = (((swGtxparam*)swGtxper)->WrkparamsDesc)[x].val;
		if (zjv.length() > 0)
			(*C) += zjv;
		else
			(*C) += "\x02\x03";
		(*C) += "\n";
	}

	(*C) += "MOVPATH:\n";
	vector<Movepath>* Mp = ((vector<Movepath>*)v_MPathp);
	char buff[5];
	for (auto itr = (*Mp).begin(); itr < (*Mp).end(); itr++) {
		stringstream ss;
		ss << fixed << setprecision(5);
		sprintf(buff, "%d", itr->Type);
		ss << itr->id << ',' << itr->Sx << ',' << itr->Sy << ',' << itr->Sz << ',' << itr->Ex << ',' << itr->Ey << ',' << itr->Ez << ',' << itr->speed << ',' << buff;
		ss << ',' << itr->acc << ',' << itr->dec << ',' << itr->Flower << "\n";
		(*C) += ss.str();
	}
	(*C) += "SCANP:\n";
	vector<Scanpoint>* Sp = ((vector<Scanpoint>*)v_SPointp);
	for (auto itr = (*Sp).begin(); itr < (*Sp).end(); itr++) {
		stringstream ss;
		ss << fixed << setprecision(5);
		ss << itr->x << ',' << itr->y << ',' << itr->z << ',' << itr->len << ',' << itr->speed << ',' << itr->Type << ',' << itr->Pid << '\n';
		(*C) += ss.str();
	}
	(*C) += "MJTG:\n";
	for (auto mjcfgIter = mjCfg.begin(); mjcfgIter != mjCfg.end(); mjcfgIter++)
	{
		stringstream ss;
		ss << mjcfgIter->first << ',' << mjcfgIter->second.stdVal << ',' << mjcfgIter->second.pTolerance << ',' << mjcfgIter->second.nTolerance;
		ss << ',' << (mjcfgIter->second.enable ? "1" : "0") << '\n';
		(*C) += ss.str();
	}
	(*C) += "THRMP:\n";
	for (auto mjcfgIter = rrpRemap.begin(); mjcfgIter != rrpRemap.end(); mjcfgIter++)
	{
		stringstream ss;
		ss << mjcfgIter->first << ',' << mjcfgIter->second.rmpName << ',' << (mjcfgIter->second.enable ? "1" : "0") << '\n';
		(*C) += ss.str();
	}
	int l = ExtAppended.size();
	if (l > 0) {
		for (unsigned x = 0; x < l; x++)
			(*C) += ExtAppended[x].SegName + ":\n" + ExtAppended[x].Content + '\n';
	}
}

void WorkParamManger::analysis() {
	FILE* afile = (FILE*)file;
	char buff[1024] = { 0 };
	unsigned slen = 0;
	string sk;
	int i = 0;
	mjCfg.clear();
	rrpRemap.clear();
	ExtAppended.clear();
	((vector<Movepath>*)v_MPathp)->clear();
	((vector<Scanpoint>*)v_SPointp)->clear();
	while (fscanf(afile, "%s\n", buff) != EOF) {
		slen = strlen(buff);
		if (slen > 0) {
			if (buff[slen - 1] == ':') {
				buff[slen - 1] = 0;
				sk = buff;
				i = 0;
				continue;
			}

			if (sk == "USEPARAM") {
				if (buff[0] != 0x02 && buff[1] != 0x03) {
					if (i < (((swGtxparam*)swGtxper)->UserparamsDesc).size())
						(((swGtxparam*)swGtxper)->UserparamsDesc)[i].val = buff;
				}
				i += 1;
			}
			else if (sk == "WRKPARAM") {
				if (buff[0] != 0x02 && buff[1] != 0x03) {
					if (i < (((swGtxparam*)swGtxper)->WrkparamsDesc).size())
						(((swGtxparam*)swGtxper)->WrkparamsDesc)[i].val = buff;
				}
				i += 1;
			}
			else if (sk == "MOVPATH") {
				string lineContent = buff;
				vector<std::string> A = stringSplit(lineContent, ',');
				int al = A.size();
				Movepath mp;
				memset(&mp, 0, sizeof(Movepath));
				if (al > 0)
					mp.id = atoi(A[0].c_str());
				if (al > 1)
					mp.Sx = atof(A[1].c_str());
				if (al > 2)
					mp.Sy = atof(A[2].c_str());
				if (al > 3)
					mp.Sz = atof(A[3].c_str());
				if (al > 4)
					mp.Ex = atof(A[4].c_str());
				if (al > 5)
					mp.Ey = atof(A[5].c_str());
				if (al > 6)
					mp.Ez = atof(A[6].c_str());
				if (al > 7)
					mp.speed = atof(A[7].c_str());
				if (al > 8)
					mp.Type = atoi(A[8].c_str()); //A[8].c_str()[0];//atoi(A[8].c_str());
				if (al > 9)
					mp.acc = atof(A[9].c_str());
				if (al > 10)
					mp.dec = atof(A[10].c_str());
				if (al > 11) {
					std::string v = A[11];
					memcpy(mp.Flower, v.c_str(), v.length());
				}
				((vector<Movepath>*)v_MPathp)->push_back(mp);
			}
			else if (sk == "SCANP") {
				string lineContent = buff;
				vector<std::string> A = stringSplit(lineContent, ',');
				int al = A.size();
				Scanpoint sp;
				if (al > 0)
					sp.x = atof(A[0].c_str());
				if (al > 1)
					sp.y = atof(A[1].c_str());
				if (al > 2)
					sp.z = atof(A[2].c_str());
				if (al > 3)
					sp.len = atof(A[3].c_str());
				if (al > 4)
					sp.speed = atof(A[4].c_str());
				if (al > 5)
					sp.Type = atoi(A[5].c_str());
				if (al > 6)
					sp.Pid = atoi(A[6].c_str());
				sp.r = 0;
				((vector<Scanpoint>*)v_SPointp)->push_back(sp);
			}
			else if (sk == "MJTG") {
				string lineContent = buff;
				vector<std::string> A = stringSplit(lineContent, ',');
				int al = A.size();
				MeasurementJudgment mj = { 0,0,0,false };
				if (al > 0)
					mj.stdVal = atof(A[1].c_str());
				if (al > 1)
					mj.pTolerance = atof(A[2].c_str());
				if (al > 2)
					mj.nTolerance = atof(A[3].c_str());
				if (al > 3)
					mj.enable = (A[4] == "1");
				mjCfg[A[0]] = mj;
			}
			else if (sk == "THRMP") {
				string lineContent = buff;
				vector<std::string> A = stringSplit(lineContent, ',');
				int al = A.size();
				THRemap rpj = { "",false };
				if (al > 0)
					rpj.rmpName = atof(A[1].c_str());

				if (al > 1)
					rpj.enable = (A[2] == "1" || A[2] == "false");

				rrpRemap[A[0]] = rpj;
			}
			else {
				int l = ExtAppended.size();
				if (l > 0) {
					if (ExtAppended[l - 1].SegName == sk) {
						ExtAppended[l - 1].Content += "\n";
						ExtAppended[l - 1].Content += buff;
					}
					else {
						ExtAppedSeg eas = { sk,buff };
						ExtAppended.push_back(eas);
					}
				}
				else {
					ExtAppedSeg eas = { sk,buff };
					ExtAppended.push_back(eas);
				}
			}
			MeasurementJudgment mj;
			MeasurementStandForeach(mj, true);
		}
	}
}

void WorkParamManger::NewCreater() {
	if (swGtxper == 0) {
		return;
	}
	(*(vector<Movepath>*)v_MPathp).clear();
	(*(vector<Scanpoint>*)v_SPointp).clear();
	for (auto it = (((swGtxparam*)swGtxper)->UserparamsDesc).begin(); it != (((swGtxparam*)swGtxper)->UserparamsDesc).end(); it++) {
		it->val = "0";
	}
	for (auto it = (((swGtxparam*)swGtxper)->WrkparamsDesc).begin(); it != (((swGtxparam*)swGtxper)->WrkparamsDesc).end(); it++) {
		it->val = "0";
	}
}

//对应要操作的参数选组 1-用户参数，2-加工参数,3 移动路径，4 扫描点表 
void WorkParamManger::Selected(u8 tWich, bool AndClrContent) {
	if (tWich > 0 && tWich < 5) {
		tsWich = tWich;
		if (AndClrContent) {
			ClrSelectedContent();
		}
		return;
	}
	(((swGtxparam*)swGtxper)->SytemLoger.Write((char*)"参数错误-超参:Selected参数只能为1-4"));
}

void WorkParamManger::ClrSelectedContent() {
	if (tsWich == 1)
	{
		(((swGtxparam*)swGtxper)->UserparamsDesc).clear();
		return;
	}

	if (tsWich == 2)
	{
		(((swGtxparam*)swGtxper)->WrkparamsDesc).clear();
		return;
	}

	if (tsWich == 3)
	{
		(*(vector<Movepath>*)v_MPathp).clear();
		return;
	}

	if (tsWich == 4)
	{
		(*(vector<Scanpoint>*)v_SPointp).clear();
		return;
	}
}


bool WorkParamManger::DataEditer(unsigned rowIndex, void* data) {
	if (tsWich == 1)
	{
		if (rowIndex < (((swGtxparam*)swGtxper)->UserparamsDesc).size()) {
			Kvpdchr* kcv = (Kvpdchr*)data;
			//Kvpd val = { kcv->key,kcv->val,kcv->des };
			Kvpd* srcVal = &(((swGtxparam*)swGtxper)->UserparamsDesc)[rowIndex];
			if (kcv->val)
				srcVal->val = kcv->val;
			if (kcv->des)
				srcVal->des = kcv->des;
			return true;
		}
	}
	if (tsWich == 2)
	{
		if (rowIndex < (((swGtxparam*)swGtxper)->WrkparamsDesc).size()) {
			Kvpdchr* kcv = (Kvpdchr*)data;
			//Kvpd val = { kcv->key,kcv->val,kcv->des };
			Kvpd* srcVal = &(((swGtxparam*)swGtxper)->WrkparamsDesc)[rowIndex];// = val;
			if (kcv->val)
				srcVal->val = kcv->val;
			if (kcv->des)
				srcVal->des = kcv->des;
			return true;
		}
	}
	if (tsWich == 3)
	{
		Movepath* p = (Movepath*)data;
		if (rowIndex < (*(vector<Movepath>*)v_MPathp).size())
			(*(vector<Movepath>*)v_MPathp)[rowIndex] = *p;
		else
			(*(vector<Movepath>*)v_MPathp).push_back(*p);
		return true;
	}
	if (tsWich == 4)
	{
		Scanpoint* p = (Scanpoint*)data;
		if (rowIndex < (*(vector<Scanpoint>*)v_SPointp).size())
			(*(vector<Scanpoint>*)v_SPointp)[rowIndex] = *p;
		else
			(*(vector<Scanpoint>*)v_SPointp).push_back(*p);
		return true;
	}
	return false;
}

bool WorkParamManger::DataReaderKvpd(unsigned rowIndex, Kvpdchr& kcv) {
	if (tsWich == 1)
	{
		if (rowIndex < (((swGtxparam*)swGtxper)->UserparamsDesc).size()) {
			Kvpd* val = &(((swGtxparam*)swGtxper)->UserparamsDesc)[rowIndex];
			kcv = { (char*)val->key.c_str(),(char*)val->val.c_str(),(char*)val->des.c_str() };
			return true;
		}
	}
	if (tsWich == 2)
	{
		if (rowIndex < (((swGtxparam*)swGtxper)->WrkparamsDesc).size()) {
			Kvpd* val = &(((swGtxparam*)swGtxper)->WrkparamsDesc)[rowIndex];
			kcv = { (char*)val->key.c_str(),(char*)val->val.c_str(),(char*)val->des.c_str() };
			return true;
		}
	}
	return false;
}

bool WorkParamManger::DataReaderKvpd(const char* key, Kvpdchr& kcv) {
	memset(&kcv, 0, sizeof(Kvpdchr));


	return false;
}

void* WorkParamManger::DataReaderMsp(unsigned rowIndex) {
	if (tsWich == 3) {
		vector<Movepath>* A = (vector<Movepath>*)v_MPathp;
		if (rowIndex < A->size()) {
			return &((*A)[rowIndex]);
		}
	}
	if (tsWich == 4) {
		vector<Scanpoint>* B = (vector<Scanpoint>*)v_SPointp;
		if (rowIndex < B->size()) {
			return &((*B)[rowIndex]);
		}
	}
	return 0;
}

const char* WorkParamManger::AppendedContentGet(const char* segName) {
	int l = ExtAppended.size();
	string sn = segName;
	vector<ExtAppedSeg>::iterator itr = std::find(ExtAppended.begin(), ExtAppended.end(), sn);
	if (itr == ExtAppended.end()) {
		return 0;
	}
	return itr->Content.c_str();
}

void WorkParamManger::PointOffsetSet(rxsPointCouldp p) {

}

bool WorkParamManger::MeasurementResultJudgment(MeasurementJudgment mj, float curVal) {
	if (mj.enable) {
		float dif = mj.stdVal - curVal;
		return ((dif > mj.nTolerance) && (dif < mj.pTolerance));
	}
	return true;
}

void WorkParamManger::MeasurementStandSet(const char* mName, MeasurementJudgment mjv) {
	mjCfg[mName] = mjv;
	MeasurementJudgment out;
	MeasurementStandForeach(out, true);
}

void WorkParamManger::rrpThRemapSet(const char* mName, const char * rname,bool enb) {
	THRemap thmp = { rname ,enb };
	rrpRemap[mName] = thmp;
	bool n = false;
	rrpThForeach(0, n, true);
}


void WorkParamManger::MeasurementStandAllClr() {
	mjCfg.clear();
}

void WorkParamManger::rrpThAllClr() {
	rrpRemap.clear();
}

void WorkParamManger::MeasurementStandRemove(const char* mName) {
	auto mjIter = mjCfg.find(mName);
	if (mjIter == mjCfg.end()) {
		return;
	}
	mjCfg.erase(mjIter);
}

void WorkParamManger::rrpThRemove(const char* mName) {
	auto mjIter = rrpRemap.find(mName);
	if (mjIter == rrpRemap.end()) {
		return;
	}
	rrpRemap.erase(mjIter);
}

const char* WorkParamManger::MeasurementStandForeach(MeasurementJudgment& out, bool reset) {
	if (mjCfg.size() < 1) { return 0; }
	static auto mjiter = mjCfg.begin();
	if (reset) {
		mjiter = mjCfg.begin();
		out = { 0,0,0,false };
		return 0;
	}
	else {
		if (mjiter == mjCfg.end()) {
			mjiter = mjCfg.begin();
			out = { 0,0,0,false };
			return 0;
		}
		const char* v = mjiter->first.c_str();
		out = mjiter->second;
		mjiter++;
		return v;
	}
}

const char* WorkParamManger::rrpThForeach(char** outName, bool& outEnb, bool reset ) {
	if (rrpRemap.size() < 1) { return 0; }
	static auto mjiter = rrpRemap.begin();
	if (reset) {
		mjiter = rrpRemap.begin();
		(*outName) =(char *) mjiter->second.rmpName.c_str();
		outEnb = mjiter->second.enable;
		return 0;
	}
	else {
		if (mjiter == rrpRemap.end()) {
			mjiter = rrpRemap.begin();
			*outName = 0;
			outEnb = false;
			return 0;
		}
		const char* v = mjiter->first.c_str();
		(*outName) = (char*)mjiter->second.rmpName.c_str();
		outEnb = mjiter->second.enable;
		mjiter++;
		return v;
	}
}


MeasurementJudgment* WorkParamManger::MeasurementStandGet(const char* mName) {
	auto mjIter = mjCfg.find(mName);
	if (mjIter == mjCfg.end()) {
		return 0;
	}
	return &mjIter->second;
}

const char* WorkParamManger::rrpThGet(const char* key, bool* enb) {
	auto mjIter = rrpRemap.find(key);
	if (mjIter == rrpRemap.end()) {
		*enb = false;
		return key;
	}
	*enb = mjIter->second.enable;
	return mjIter->second.rmpName.c_str();
}


