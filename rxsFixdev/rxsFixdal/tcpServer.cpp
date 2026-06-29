#include "tcpServer.h"




tcpServer::tcpServer() {

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		std::cout << "WinSock不能被初始化" << std::endl;
		WSACleanup();
		return;
	}
	m_accpect_thread = 0;
	m_recv_thread = 0;
	isStart = false;
}


tcpServer::~tcpServer() {
	isStart = false;
	if (m_accpect_thread != 0) {
		m_accpect_thread->detach();
		delete m_accpect_thread;
		m_accpect_thread = 0;
	}


	if (m_recv_thread != 0) {
		m_recv_thread->detach();
		delete m_recv_thread;
		m_recv_thread = 0;
	}

	closesocket(server);
	for (auto mitr = csgrp.begin(); mitr != csgrp.end(); mitr++) {
		closesocket(mitr->second.client);
		mitr->second.message.clear();
	}
	csgrp.clear();
	WSACleanup();
}


void tcpServer::clientAccpectThread() {
	int len = sizeof(SOCKADDR);
	SOCKET client;
	while (isStart) {
		client = accept(server, (SOCKADDR*)&cur_client_addr, &len);
		if (client == SOCKET_ERROR || client == INVALID_SOCKET) {
			
		}
		else {
			std::string c = inet_ntoa(cur_client_addr.sin_addr);
			std::cout << "连接成功 => from:" << c << std::endl;
			std::string key = c.append(":").append(std::to_string(cur_client_addr.sin_port));

			clientSession cs;
			cs.client = client;
			csgrp[key] = cs;
		}
		_sleep(10);
	}
}

void tcpServer::recvThread() {
	while (isStart) {
		for (auto miter = csgrp.begin(); miter != csgrp.end(); miter++)
		{
			char recvBuf[100];
			SOCKET client = miter->second.client;
			memset(recvBuf, 0, sizeof(recvBuf));
			if (recv(client, recvBuf, sizeof(recvBuf), 0) > 0) {
				std::string v = recvBuf;
				std::cout << inet_ntoa(cur_client_addr.sin_addr) << " =>has recv:" << v << std::endl;
				miter->second.message.push_back(v);
			}
		}
		_sleep(10);
	}
}

void tcpServer::Close() {
	isStart = false;
	if (m_accpect_thread != 0) {
		m_accpect_thread->detach();
		delete m_accpect_thread;
		m_accpect_thread = 0;
	}

	if (m_recv_thread != 0) {
		m_recv_thread->detach();
		delete m_recv_thread;
		m_recv_thread = 0;
	}
	closesocket(server);
	for (auto mitr = csgrp.begin(); mitr != csgrp.end(); mitr++) {
		closesocket(mitr->second.client);
		mitr->second.message.clear();
	}
	csgrp.clear();
}


bool tcpServer::Creater(const char* locip, unsigned short locPort) {
	SOCKADDR_IN server_addr;//使用结构体 SOCKADDR_IN 存储配置
	//memset(&server_addr, 0, sizeof(server_addr)); //清零
	server_addr.sin_family = AF_INET;

	if (locip == 0) {
		server_addr.sin_addr.s_addr = INADDR_ANY;
	}
	else {
		server_addr.sin_addr.s_addr = inet_addr(locip); //server端ip地址
	}
	server_addr.sin_port = htons(locPort); //监听端口

	ULONG noblock = 1;            //非阻塞参数
	server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	bind(server, (SOCKADDR*)&server_addr, sizeof(SOCKADDR));
	listen(server, 10);
	std::cout << "等待连接......" << std::endl;

	isStart = true;

	if (m_accpect_thread == 0) {
		m_accpect_thread = new std::thread(&tcpServer::clientAccpectThread, this);
	}

	if (m_recv_thread == 0) {
		m_recv_thread = new std::thread(&tcpServer::recvThread, this);
	}
	return true;
}



int tcpServer::Send(const char* tage, const char* content, int len) {
	//发送数据
	auto miter = csgrp.find(tage);
	if (miter == csgrp.end()) {
		return 0;
	}
	int iSend = send(miter->second.client, content, len, 0);
	if (iSend == SOCKET_ERROR) {
		std::cout << "send failed" << std::endl;
	}
	return iSend;
}


bool tcpServer::hasRecv(const char* tage, const char** ret) {
	auto miter = csgrp.find(tage);
	if (miter == csgrp.end()) {
		return false;
	}
	content = "";
	std::vector<std::string> msg = miter->second.message;
	int l = msg.size();
	if (l > 0) {
		for (int x = 0; x < l; x++) {
			content += msg[x] + "\r\n";
		}
		*ret = content.c_str();
		return true;
	}
	*ret = 0;
	return false;
}

void tcpServer::RecvClr(const char* tage) {
	auto miter = csgrp.find(tage);
	if (miter == csgrp.end()) {
		return;
	}
	miter->second.message.clear();
}

const char* tcpServer::clientForeach(int& index) {
	if (csgrp.size() == 0) { return 0; }
	if (index < csgrp.size()) {
		static std::map<std::string, clientSession>::iterator miter;
		if (index < 1) {
			miter = csgrp.begin();
			index += 1;
			return miter->first.c_str();
		}
		else {
			miter++;
			if (miter == csgrp.end()) {
				index = -1;
				return 0;
			}
			index += 1;
			return  miter->first.c_str();
		}
	}
	index = -1;
	return 0;
}
