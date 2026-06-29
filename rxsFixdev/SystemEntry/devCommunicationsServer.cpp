#include "private.h"
#include "tcpServer.h"



myRobort::myRobort()
{
	myServer = new tcpServer();
}




myRobort::~myRobort()
{
	((tcpServer*)myServer)->Close();
	delete ((tcpServer*)myServer);
	myServer = 0;
}



short myRobort::Open(std::string connStr) {
	std::vector<std::string> rrs = stringSplit(connStr, ',');
	const char* ip = rrs[0].c_str();
	unsigned short port = std::atoi(rrs[1].c_str());
	return ((tcpServer*)myServer)->Creater(ip, port) ? 0 : -1;
}


short myRobort::workStart() {
	std::string tage = "";
	int erc = ((tcpServer*)myServer)->Send(tage.c_str(), "start", 5);
	return erc;
}


int myRobort::ImgProcessFinsh() {
	std::string tage = "";
	int erc = ((tcpServer*)myServer)->Send(tage.c_str(), "ok", 2);
	return erc;
}


short myRobort::Org() {
	char content[512] = { 0 };
	std::string tage = "";
	int erc = ((tcpServer*)myServer)->Send(tage.c_str(), content, 512);
	return erc;
}

bool myRobort::isWorkEnd(int& wich) {
	const char* vvv = 0;
	std::string tage = "";
	if (((tcpServer*)myServer)->hasRecv(tage.c_str(), &vvv)) {
		std::string rv = vvv;
		if (rv == "end") {
			((tcpServer*)myServer)->RecvClr(tage.c_str());
			return true;
		}
		else if (rv == "") {
			;
		}
		wich = std::atoi(rv.c_str());
	}
	return false;
}





















devCommunicationsServer::devCommunicationsServer()
{
	myServer = new tcpServer();
}




devCommunicationsServer::~devCommunicationsServer()
{
	((tcpServer*)myServer)->Close();
	delete ((tcpServer*)myServer);
	myServer = 0;
}


void devCommunicationsServer::Close() {
	((tcpServer*)myServer)->Close();
}


bool devCommunicationsServer::hasOpen() {
	return ((tcpServer*)myServer)->hasOpen();
}


short devCommunicationsServer::Open(std::string connStr) {
	std::vector<std::string> rrs = stringSplit(connStr, ',');
	const char* ip = rrs[0].c_str();
	unsigned short port = std::atoi(rrs[1].c_str());
	return ((tcpServer*)myServer)->Creater(ip, port) ? 0 : -1;
}



int devCommunicationsServer::ThreeDimensionalProcess(std::string content) {
	const char* ip = 0;
	int port = 0;
	return SendContent(ip, port, content);
}

int devCommunicationsServer::TwoDimensionalProcess(std::string content) {
	const char* ip = 0;
	int port = 0;
	return SendContent(ip, port, content);
}


const char* devCommunicationsServer::ClientForeach(int& index) {
	return ((tcpServer*)myServer)->clientForeach(index);
}


const char* devCommunicationsServer::RecviceContent(const char* ip, int port) {
	const char* retv = 0;
	((tcpServer*)myServer)->hasRecv(ip, port, &retv);
	return retv;
}

const char* devCommunicationsServer::RecviceContent(const char* tage) {
	const char* retv = 0;
	if (((tcpServer*)myServer)->hasRecv(tage, &retv)) {
		return retv;
	}
	return 0;
}

void devCommunicationsServer::RecviceContentClr(const char* tage) {
	if (tage == 0) {
		int index = 0;
		while ((tage = ((tcpServer*)myServer)->clientForeach(index)) != 0) {
			((tcpServer*)myServer)->RecvClr(tage);
		}
	}
	else {
		((tcpServer*)myServer)->RecvClr(tage);
	}
}


int devCommunicationsServer::SendContent(const char* ip, int port, std::string content) {
	int erc = 0;
	std::string sip = ip;
	if (port == -1) {
		int index = 0;
		const char* cls;
		std::vector<std::string> tages;
		while ((cls = ((tcpServer*)myServer)->clientForeach(index)) != 0) {
			std::vector<std::string> vr = stringSplit(cls, ':');
			if (vr[0] == ip) {
				tages.push_back(cls);
			}
		}
		for (int x = 0; x < tages.size(); x++)
		{
			erc = ((tcpServer*)myServer)->Send(tages[x].c_str(), content.c_str(), content.length());
		}
	}
	else {
		std::string tage = sip.append(":").append(std::to_string(port));
		erc = ((tcpServer*)myServer)->Send(tage.c_str(), content.c_str(), content.length());
	}
	return erc;
}

int devCommunicationsServer::SendContent(const char* tege, std::string content) {
	if (tege == 0) {
		return ((tcpServer*)myServer)->Send(content.c_str(), content.length());
	}
	else {
		int erc = ((tcpServer*)myServer)->Send(tege, content.c_str(), content.length());
		return erc;
	}
}