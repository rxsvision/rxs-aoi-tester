#include "private.h"
#include <mutex>


extern HrdWrkSyetemEnv hrdSyetemEnver;

typedef struct
{
	std::string ip;
	int port;
	std::string sdtxt;
}inParams;


TcpServerRecvStep::TcpServerRecvStep(int id) :StepBlockBase(id) {
	_type = StepBlockBase::TCP_SERVERRECV;
	outResultType = ( BLKOUTRESULTYPE_DATASET);
	p = new inParams;
}



TcpServerRecvStep::TcpServerRecvStep(int id, const char* name) : StepBlockBase(id, name) {
	_type = StepBlockBase::TCP_SERVERRECV;
	outResultType = ( BLKOUTRESULTYPE_DATASET);
	p = new inParams;
}


TcpServerRecvStep::~TcpServerRecvStep() {
	if (p != 0) {
		delete (inParams*)p;
		p = 0;
	}
}


short TcpServerRecvStep::Run() {
	std::string scon = ((inParams*)p)->sdtxt;
	size_t i = 0;
	while ((i = scon.find('\x1f', i)) != std::string::npos) {
		scon = scon.replace(i, 1, ",");
	}

	hrdSyetemEnver.pSystemer->Wrkpd->LocServerSend(
		((inParams*)p)->ip.c_str(), ((inParams*)p)->port,
		scon.c_str(), scon.length());
	return 0;
}

bool TcpServerRecvStep::InputParamStringCfgSet(std::string* ps, int l) {
	int index = 0;
	if (l > 2) {
		std::string val = ps[index];
		((inParams*)p)->ip = val;
		val = ps[index += 1];
		((inParams*)p)->port = atoi(val.c_str());
		val = ps[index += 1];
		((inParams*)p)->sdtxt = stringSubreplace(val, "\x1F", ",");
		return true;
	}
	return false;
}

std::string TcpServerRecvStep::InputParamSaveStringGet() {
	stringstream ss;
	((inParams*)p)->sdtxt = stringSubreplace(((inParams*)p)->sdtxt, ",", "\x1F");
	ss << ((inParams*)p)->ip << "," << std::to_string(((inParams*)p)->port) << "," << ((inParams*)p)->sdtxt;
	return ss.str();
}


void TcpServerRecvStep::sendTxt(const char* txt) {
	((inParams*)p)->sdtxt = txt;
}

const char* TcpServerRecvStep::sendTxt() {
	return ((inParams*)p)->sdtxt.c_str();
}

void TcpServerRecvStep::ipprConfig(const char* ip, int port) {
	((inParams*)p)->ip = ip;
	((inParams*)p)->port = port;
}

const char* TcpServerRecvStep::ipprConfig(int& port) {
	port = ((inParams*)p)->port;
	return ((inParams*)p)->ip.c_str();
}