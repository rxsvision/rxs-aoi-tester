#pragma once

// 包含必要的头文件
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
// 在Windows平台下，包含WinSock2头文件并禁用特定警告
#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma warning(disable:4996) 
#pragma comment(lib,"ws2_32.lib")
// 定义write和read为send和recv，以保持跨平台兼容性
#define write(fd,buf,len) send((fd),(buf),(len),0)
#define read(fd,buf,len) recv((fd),(buf),(len),0)
#else
// 这里做跨平台，TODO
#endif 

// 定义缓冲区大小
#define BUFSIZE (4*1024)

void LOG(const std::string& msg);

struct TCPDATA {
	std::string data;
};

class WSAResource {
public:
	WSAResource();
	~WSAResource();
};

/**
 * @class TcpClient
 * 提供一个简单的TCP客户端实现。
 */
class __declspec(dllexport) TCPClient
{
private:
	WSAResource wsaResource; // RAII类实例，自动管理Winsock资源
public:
	// 构造函数
	TCPClient();
	// 析构函数
	~TCPClient();
	/**
	 * 连接到指定IP和端口。
	 * @param ip 目标IP地址。
	 * @param port 目标端口号。
	 * @return 连接成功返回true，否则返回false。
	 */
	bool Connect(const char* ip, const unsigned short port);
	/**
	 * 关闭客户端连接。
	 * 调不调用都可，在析构里会close
	 */
	void Close();
	/**
	 * 启动客户端的收发线程。
	 */
	void start();
	/**
	 * 从服务器接收数据。
	 * @param buf 用于接收数据的缓冲区。
	 * @param bufsize 缓冲区的大小。
	 * @return 接收到的数据量，或错误时的负值。
	 */
	int Recv(char* buf, int bufsize);
	/**
	 * 向服务器发送数据。
	 * @param buf 要发送的数据缓冲区。
	 * @param bufsize 要发送的数据量。
	 * @return 发送的数据量，或错误时的负值。
	 */
	int Send(const char* buf, int bufsize);
private:
	/**
	 * 接收线程函数，用于不断从服务器接收数据。
	 */
	void tcp_thread_recv_fun();
	/**
	 * 发送线程函数，用于不断向服务器发送数据。
	 */
	void tcp_thread_send_fun();
	/**
	 * 用于关闭发送线程的内部函数。
	 */
	void close_send_thread();
	/**
	 * 用于关闭接收线程的内部函数。
	 */
	void close_recv_thread();
	/**
	 * 用于解析TCP收到的消息，发送数据处理线程
	 */
	void parse_recv_data(const char* data, const int size);
private:
	// 客户端的Socket句柄
	SOCKET m_socket;
	// 接收线程和发送线程的指针
	std::thread* m_recv_thread = nullptr;
	std::thread* m_send_thread = nullptr;
	// 发送线程退出标志
	bool m_send_quit = false;
	// 线程同步所需的互斥锁和条件变量
	std::mutex m_mutex;
	std::condition_variable m_cond;
	//接收数据
	TCPDATA m_recv_data;
};                                                                                                                  