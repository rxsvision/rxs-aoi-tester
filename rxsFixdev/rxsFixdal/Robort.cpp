#include "protected.h"
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
	int erc = ((tcpServer*)myServer)->Send("start", 5);
	return erc;
}


int myRobort::ImgProcessFinsh() {
	int erc = ((tcpServer*)myServer)->Send("ok", 2);
	return erc;
}


short myRobort::Org() {
	char content[512] = { 0 };
	int erc = ((tcpServer*)myServer)->Send(content, 512);
	return erc;
}

bool myRobort::isWorkEnd(int& wich) {
	std::string rv;
	if (((tcpServer*)myServer)->hasRecv(&rv)) {
		if (rv == "end") {
			((tcpServer*)myServer)->RecvClr();
			return true;
		}
		else if (rv == "") {
			;
		}
		wich = std::atoi(rv.c_str());
	}
	return false;
}



