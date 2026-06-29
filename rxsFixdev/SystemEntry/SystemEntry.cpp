
#include "rxsMainFrame.h"
#include "private.h"

#include <io.h>
#include <stdio.h>
#include <iostream>
#include <sstream>


using namespace inkType;
using namespace std;

extern swGtxparam* swGtxper;


RXSAPIS std::vector<std::string> stringSplit(const std::string& str, char delim) {
	std::stringstream ss(str);
	std::string item;
	std::vector<std::string> elems;
	while (std::getline(ss, item, delim)) {
		if (!item.empty()) {
			elems.push_back(item);
		}
	}
	return elems;
}



RXSAPIS std::string stringSubreplace(std::string resource_str, std::string sub_str, std::string new_str)
{
	std::string dst_str = resource_str;
	std::string::size_type pos = 0;
	while ((pos = dst_str.find(sub_str)) != std::string::npos)   //替换所有指定子串
	{
		dst_str.replace(pos, sub_str.length(), new_str);
	}
	return dst_str;
}


SystemInfo::SystemInfo() {
	alamType = 0;
	hadRest = false;
	fixStatus = FixStaus::INIT;
}

SystemInfo::~SystemInfo() {

}

bool SystemInfo::isAlam(ALAMTYPE& AlamType) {
	AlamType = (ALAMTYPE)alamType;
	return (alamType > 0);
}


void SystemInfo::AlamSet(ALAMTYPE type) {

	alamType = type;
}

SystemInfo::FixStaus	SystemInfo::CurFixStatus() {
	return (FixStaus)fixStatus;
}

void SystemInfo::FixStatusSet(FixStaus fs) {
	fixStatus = fs;
	if (fs == FixStaus::RDY) {
		alamType = 0;
		return;
	}
	if (fs == FixStaus::RESETING) {
		hadRest = false;
		return;
	}
}

bool SystemInfo::HadRestOk() {
	return hadRest;
}

void SystemInfo::HadRestFinsh() {
	hadRest = true;
}





UserManger::UserManger() {
	itr = new map<string, string>::iterator;
	memset(&CurUser, 0, sizeof(UserInfo));
	memset(&usr, 0, sizeof(UserInfo));
	v = new string;
}

UserManger::~UserManger() {
	delete (string*)v;
	delete (map<string, string>::iterator*) itr;
}


RXSAPIS bool UserManger::Login(const char* Uname, const char* Pwd) {
	UserInfo u = { (char*)Uname,(char*)Pwd,USER_AUTH::NOLOGIN };
	if (isdefadm(u)) {
		CurUser = u;
		return true;
	}
	if (matchuser(u)) {
		CurUser = u;
		return true;
	}
	return false;
}


void UserManger::hunx(void* s, char auth) {
	char r = (*(string*)s)[10];
	(*(string*)s)[10] = auth;
	(*(string*)s) += r;
}


bool UserManger::resetuVal(char* k, char* Pwd, char Auth) {
	Coded c;
	c.SetEncodeMode(2);
	string npwd = c.EnCode(Pwd, strlen(Pwd));
	hunx(&npwd, Auth);
	((swGtxparam*)swGtxper)->Users[k] = npwd;

	return true;
}

RXSAPIS bool UserManger::ResetUser(const char* Uname,const char* Pwd, const char* NewPwd, USER_AUTH newAuth) {
	if (CurUser.Auth == USER_AUTH::ADMIN) {//当前admin 可重置权限及密码
		return resetuVal((char*)Uname, (char*)NewPwd, newAuth);
	}
	if (memcmp(CurUser.Uname, Uname, strlen(CurUser.Uname)) == 0) {//当前用户只能重置自己的密码
		UserInfo su = { (char*)Uname,(char*)Pwd,USER_AUTH::NOLOGIN };
		if (matchuser(su)) {
			return resetuVal((char*)Uname, (char*)NewPwd, su.Auth);
		}
	}
	return false;
}

RXSAPIS bool UserManger::AddUser(const char* Uname, const char* Pwd, USER_AUTH auth) {
	if (CurUser.Auth == USER_AUTH::ADMIN) {
		Coded c;
		c.SetEncodeMode(2);
		string npwd = c.EnCode((char*)Pwd, strlen(Pwd));
		hunx(&npwd, auth);
		((swGtxparam*)swGtxper)->Users[Uname] = npwd;
		return true;
	}
	return false;
}

RXSAPIS bool UserManger::DelUser(const char* Uname, const char* Pwd) {
	Coded c;
	c.SetEncodeMode(2);
	string jmhs = c.EnCode((char*)Pwd, strlen(Pwd));
	string aps = ((swGtxparam*)swGtxper)->Users[Uname];
	Anasyer(&aps);
	if (memcmp(aps.c_str(), jmhs.c_str(), jmhs.length()) == 0)
	{
		((swGtxparam*)swGtxper)->Users.erase(Uname);
		return true;
	}
	return false;
}

void UserManger::Anasyer(void* rp) {
	string* A = (string*)rp;
	int i = A->length();
	if (i > 0) {
		i -= 1;
		char s = (*A)[i];
		char j = (*A)[10];
		(*A)[10] = s;
		(*A)[i] = j;
	}
}

bool UserManger::isdefadm(UserInfo& uif) {
	const char* u = "SYSTEM";
	const char* pwd = "SYSTEM_QWE";
	unsigned x = 0;
	for (; uif.Uname[x] != 0; x++) {
		if (x > 7) {
			return false;
		}
		if (uif.Uname[x] != u[x]) {
			return false;
		}
	}
	if (x != 6) { return false; }
	if (x == 0) { ((swGtxparam*)swGtxper)->SytemLoger.Write((char*)"用户名长度为0!"); return false; }
	for (x = 0; uif.Pwd[x] != 0; x++) {
		if (x > 11) {
			return false;
		}
		if (uif.Pwd[x] != pwd[x]) {
			return false;
		}
	}
	if (x != 10) { return false; }
	if (x == 0) { ((swGtxparam*)swGtxper)->SytemLoger.Write((char*)"密码长度为0!"); return false; }
	uif.Uname = 0;
	uif.Auth = USER_AUTH::ADMIN;
	return true;
}

bool UserManger::matchuser(UserInfo& uif) {
	Coded c;
	c.SetEncodeMode(2);
	string jmhs = c.EnCode(uif.Pwd, strlen(uif.Pwd));
	string aps = ((swGtxparam*)swGtxper)->Users[uif.Uname];
	Anasyer(&aps);
	if (memcmp(aps.c_str(), jmhs.c_str(), jmhs.length()) == 0) {
		uif.Auth = (USER_AUTH)aps[aps.length()];
		return true;
	}
	return false;
}


UserManger::UserInfo* UserManger::UserForech(bool reStart) {
	if (((swGtxparam*)swGtxper)->Users.size() > 0) {
		if (reStart) {
			*(map<string, string>::iterator*) itr = ((swGtxparam*)swGtxper)->Users.begin();
			return 0;
		}
		map<string, string>::iterator* B = (map<string, string>::iterator*)itr;
		if ((*B) == ((swGtxparam*)swGtxper)->Users.end()) {
			return 0;
		}
		usr.Uname = (char*)(*B)->first.c_str();
		(*(string*)v) = (*B)->second;
		Anasyer(v);
		int vl = (*(string*)v).length();
		if (vl > 0) {
			vl -= 1;
			usr.Auth = (USER_AUTH)(*(string*)v)[vl];
			(*(string*)v)[vl] = 0;
			usr.Pwd = (char*)(*(string*)v).c_str();
			(*(map<string, string>::iterator*)itr)++;
			return &usr;
		}
	}
	return 0;
}



FixServerBaser::FixServerBaser()
{
	MotionGpio = 0;
	mainFrameptr = 0;
}

FixServerBaser::~FixServerBaser()
{

}

void FixServerBaser::VariableAssignment() {
	MotionGpio = swGtxper->MotionGpioSer;
	mainFrameptr = swGtxper->Systemptr;
}

void FixServerBaser::DateTimeGet(char* sysspan, unsigned short Mode) {
	time_t rawtime;
	time(&rawtime);
	struct tm* ti = localtime(&rawtime);
	if (sysspan == 0) { return; }
	if (Mode == 0)
	{
		sprintf(sysspan, "%.4d%.2d%.2d", (1900 + ti->tm_year), ti->tm_mon + 1, ti->tm_mday);
	}
	else if (Mode == 1) {
		sprintf(sysspan, "%.4d%.2d%.2d%.2d%.2d%.2d", (1900 + ti->tm_year), ti->tm_mon + 1, ti->tm_mday, ti->tm_hour, ti->tm_min, ti->tm_sec);
	}
	else if (Mode == 2)
	{
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
		sprintf(sysspan, "%.4d%.2d%.2d%.2d%.2d%.2d-%.3d", (1900 + ti->tm_year), ti->tm_mon + 1, ti->tm_mday, ti->tm_hour, ti->tm_min, ti->tm_sec, (int)ms.count());
	}
	else {
		sysspan = asctime(ti);
	}
}

void FixServerBaser::SysLogRecord(const char* msg, int errType) {
	if (msg != 0) {
		(*swGtxper).SytemLoger.Write((char*)msg);
		if (mainFrameptr && errType > -1) {
			pSystem->FrontProcessMsgSender(errType, (void*)msg);
		}
	}
}

void FixServerBaser::DebugInfo(const char* str) {
	swGtxper->SytemLoger.Write((char*)str);
}

void* FixServerBaser::GetLocaltime(char* sysspan, unsigned short Mode, char* systime)
{
	time_t rawtime;
	time(&rawtime);
	struct tm* ti = localtime(&rawtime);
	if (systime != NULL)
	{
		systime = asctime(ti);
	}
	if (Mode == 0)
	{
		sprintf(sysspan, "%.4d%.2d%.2d", (1900 + ti->tm_year), ti->tm_mon + 1, ti->tm_mday);
	}
	else {
		sprintf(sysspan, "%.4d%.2d%.2d%.2d%.2d%.2d", (1900 + ti->tm_year), ti->tm_mon + 1, ti->tm_mday, ti->tm_hour, ti->tm_min, ti->tm_sec);
	}
	return ti;
}




SoftTiggerManger::SoftTiggerManger() {
}

SoftTiggerManger::~SoftTiggerManger() {

}

unsigned SoftTiggerManger::PortNumGet() {
	return keySinGrps.size();
}

void SoftTiggerManger::TiggerSourceSet() {

}

void SoftTiggerManger::TiggerEventSet(const char* key, int source, SoftTiggerEvent func) {

	SoftinSignal s;
	s.TiggerEvent = func;
	s.TiggerC.TiggerSource = source;
	
	keySinGrps[key] = s;
}

bool SoftTiggerManger::TiggerCheck(SoftinSignal* s) {
	if (s->TiggerC.TiggerSource == 0) {

	}
	else if (s->TiggerC.TiggerSource == 1) {

	}
	return false;
}


void SoftTiggerManger::Tigger() {
	const char* kv = 0;
	SoftinSignal* s = 0;
	for (
		std::map<std::string, SoftinSignal>::iterator titer = keySinGrps.begin(); titer != keySinGrps.end(); titer++) {
		s = &titer->second;
		kv = titer->first.c_str();
		s->tiggerName = kv;

		if (TiggerCheck(s)) {
			//s->TiggerEvent(kv, s->ID);
		}
	}
}