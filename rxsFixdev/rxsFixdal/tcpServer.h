#pragma once


#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <thread> //多线程
#include <winsock.h> //socket头文件
#pragma comment (lib,"ws2_32.lib")

#pragma warning(disable : 4996)


typedef struct {
	SOCKET client;
	std::vector<std::string> message;
}clientSession;







class tcpServer {
public:
	tcpServer();
	~tcpServer();


	bool Creater(const char* locip, unsigned short locPort);
	void Close();

	int Send(const char* tage, const char* content, int len);

	bool hasRecv(const char* tage, const char**);
	void RecvClr(const char* tage);

	const char* clientForeach(int &index);
private:
	std::thread* m_accpect_thread = nullptr;
	std::thread* m_recv_thread = nullptr;
	SOCKET server;
	bool isStart;
	SOCKADDR_IN cur_client_addr;
	void clientAccpectThread();
	void recvThread();
	std::map<std::string, clientSession> csgrp;
	std::string content;
};