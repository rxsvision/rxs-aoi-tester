#include "TCPClient.h"

/**
 * TcpClient 类的构造函数
 * 该构造函数用于创建 TcpClient 类的实例。
 *
 * 参数：
 * 无
 *
 * 返回值：
 * 无
 *
 * 注：该构造函数不接受任何参数，也不返回任何值。
 * 在构造函数内部，针对不同的操作系统进行套接字初始化操作。
 * #ifdef _WIN32 宏用于判断是否在 Windows 系统下编译，
 * 如果是，则将成员变量 m_socket 初始化为 INVALID_SOCKET，
 * 即无效的套接字状态，这是在 Windows 系统下进行套接字操作的常规做法。
 */
TCPClient::TCPClient()
{
#ifdef _WIN32
	m_socket = INVALID_SOCKET; // 初始化套接字为无效状态，仅在 Windows 平台下有效
#endif
}

/**
 * TcpClient 类的析构函数
 *
 * 本函数在对象生命周期结束时自动调用，主要用于关闭 TCP 连接。
 * 无需参数。
 * 无返回值。
 */
TCPClient::~TCPClient()
{
	// 关闭 TCP 连接
	Close();
}

/**
 * @brief 连接到指定IP和端口的服务器
 *
 * @param ip 服务器的IP地址，格式为字符串
 * @param port 服务器的端口号，无符号短整型
 * @return true 连接成功
 * @return false 连接失败
 */
bool TCPClient::Connect(const char* ip, const unsigned short port)
{
	// 初始化发送线程退出标志
	m_send_quit = false;

	// 创建TCP套接字
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket < 0) {
		// 输出套接字创建失败错误信息
		std::string _errMsg = "connect : socker init error, error code: " + std::to_string(m_socket);
		LOG(_errMsg);
		return false;
	}

	// 设置服务器地址信息
	sockaddr_in _server;
	_server.sin_family = AF_INET;
	_server.sin_addr.S_un.S_addr = inet_addr(ip); // 将IP字符串转换为二进制地址
	_server.sin_port = htons(port); // 端口号网络字节序转换

	// 尝试连接服务器
	if (connect(m_socket, (sockaddr*)&_server, sizeof(_server)) == SOCKET_ERROR) {
		return false; // 连接失败
	}
	else {
		return true; // 连接成功
	}
}

/**
 * @brief 关闭TCP连接及清理资源
 */
void TCPClient::Close()
{
	if (m_socket <= 0)
		return;
	closesocket(m_socket);

	// 关闭接收和发送线程
	close_recv_thread();
	close_send_thread();
}

/**
 * @brief 接收数据
 *
 * @param buf 用于存储接收数据的缓冲区
 * @param bufsize 缓冲区的大小
 * @return int 实际接收到的数据量
 */
int TCPClient::Recv(char* buf, int bufsize)
{
	return read(m_socket, buf, bufsize, 0);
}

/**
 * @brief 发送数据
 *
 * 该函数用于通过TCP客户端发送指定缓冲区中的数据。
 *
 * @param buf 要发送的数据缓冲区的起始指针。
 * @param bufsize 要发送的数据量，以字节为单位。
 * @return int 实际发送的数据量，以字节为单位。如果发送过程中发生错误，返回值可能小于传入的bufsize。
 */
int TCPClient::Send(const char* buf, int bufsize)
{
	int _send_len = 0; // 已发送的数据量初始化为0
	while (_send_len != bufsize) // 循环，直到发送完所有数据
	{
		int _len = write(m_socket, buf + _send_len, bufsize - _send_len, 0); // 调用系统函数write发送数据
		if (_len <= 0) { // 发送失败或发送0字节，退出循环
			break;
		}
		_send_len += _len; // 更新已发送的数据量
	}
	return _send_len; // 返回实际发送的数据量
}

/**
 * @brief 启动接收和发送线程
 *
 * 该函数检查接收线程和发送线程是否已经启动，如果未启动，则分别启动这两个线程。
 * 启动线程使用了成员函数指针和绑定技术，将TcpClient对象的引用传递给线程函数。
 */
void TCPClient::start()
{
	// 检查并启动接收线程
	if (m_recv_thread == nullptr)
		m_recv_thread = new std::thread(&TCPClient::tcp_thread_recv_fun, this);
	// 检查并启动发送线程
	if (m_send_thread == nullptr)
		m_send_thread = new std::thread(&TCPClient::tcp_thread_send_fun, this);
}


/**
 * @brief 线程函数，用于在TCP客户端中接收数据
 *
 * 此函数在一个独立的线程中运行，不断尝试从连接的socket接收数据。
 * 当接收到数据时，它将数据打印到标准输出，并通知其他等待此信号的线程。
 * 如果在接收过程中发生错误，它将关闭发送线程并退出。
 */
void TCPClient::tcp_thread_recv_fun()
{
	char* _buf = new char[BUFSIZE]; // 分配一个用于接收数据的缓冲区
	memset(_buf, 0, BUFSIZE);

	while (1) {
		int _recv_len = Recv(_buf, BUFSIZE); // 尝试接收数据
		if (_recv_len == SOCKET_ERROR) { // 如果接收过程中发生错误
			std::string errMsg = "tcp_thread_recv_fun : recv error!";
			LOG(errMsg);
			close_send_thread();
			break;
		}
		else if (_recv_len == 0) {
			std::string msg = "tcp_thread_recv_fun : peer disconnected!";
			LOG(msg);
			close_send_thread();
			break;
		}
		else {
			_buf[_recv_len] = '\0'; // 在接收缓冲区的末尾添加字符串结束符，后续有完整的通信协议之后，可不用添加
			parse_recv_data(_buf, _recv_len);
		}
	}

	std::string msg = "tcp_thread_recv_fun quit";// 打印线程退出信息
	LOG(msg);
	delete[] _buf;
}

/**
 * @brief TCP客户端线程发送函数
 * 该函数为TCP客户端的发送线程入口函数，它不断发送预定义的数据直到接收到停止发送的指令。
 *
 * @note 该函数无参数且无返回值。
 */
void TCPClient::tcp_thread_send_fun()
{
	//char _buf[] = "recv data!"; // 定义发送的数据内容

	while (1) {
		std::unique_lock<std::mutex> _l(m_mutex); // 加锁，保证线程安全
		m_cond.wait(_l); // 等待条件变量的通知，以进行下一步操作
		if (m_send_quit == false) {
			//Send(_buf, sizeof(_buf)); // 如果未接收到停止发送的指令，发送数据
			Send(m_recv_data.data.c_str(), m_recv_data.data.size());
		}
		else {
			break; // 如果接收到停止发送的指令，退出循环
		}
	}

	std::string msg = "tcp_thread_send_fun quit";// 打印线程退出信息
	LOG(msg);
}

/**
 * @brief 关闭发送线程
 *
 * 该函数用于安全地关闭TcpClient的发送线程。它首先通过设置发送退出标志为true来通知发送线程应该停止运行，
 * 然后通过条件变量唤醒可能正在等待的发送线程，接着加入到发送线程中以确保其真正终止，
 * 最后清理线程资源并将发送线程指针设置为nullptr。
 */

void TCPClient::close_send_thread()
{
	if (m_send_thread != nullptr) { // 检查发送线程是否正在运行
		m_send_quit = true; // 设置发送退出标志，通知发送线程应当终止
		m_cond.notify_one(); // 唤醒可能正在等待的发送线程，以便它可以退出
		m_send_thread->join(); // 等待发送线程终止
		delete m_send_thread; // 清理发送线程的资源
		m_send_thread = nullptr; // 将发送线程指针设置为nullptr，表示没有正在运行的发送线程
	}
}


/**
 * @brief 关闭接收线程
 *
 * 该函数用于安全地关闭TcpClient的接收线程。它首先检查接收线程指针是否非空，然后
 * 调用线程的join方法等待线程结束，接着删除线程对象，并将线程指针设置为nullptr。
 * 这个函数确保了在删除线程前，线程已经完成执行，避免了资源泄露和潜在的并发问题。
 */

void TCPClient::close_recv_thread()
{
	if (m_recv_thread != nullptr) { // 检查接收线程指针是否非空
		m_recv_thread->join();     // 等待接收线程结束
		delete m_recv_thread;       // 删除接收线程对象
		m_recv_thread = nullptr;    // 将线程指针设置为nullptr
	}
}

void TCPClient::parse_recv_data(const char* data, const int size)
{
	std::unique_lock<std::mutex> _l(m_mutex); //用于线程同步

	std::cout << "Recv len: " << size << "\t data: " << data << std::endl; // 打印接收到的数据
	m_recv_data.data = std::string(data);

	m_cond.notify_one(); // 通知其他线程有数据接收
}

/**
 * WSAResource 类构造函数
 * 该构造函数用于初始化 Winsock。仅当编译目标为 _WIN32 平台时，才会执行 Winsock 的初始化过程。
 *
 * 参数:
 * 无
 *
 * 返回值:
 * 无
 *
 * 注意:
 * 如果 WSAStartup 失败，将记录错误信息并抛出 std::runtime_error 异常。
 */
WSAResource::WSAResource()
{
#ifdef _WIN32
	// 初始化 Winsock 版本 2.2
	WSADATA wsaData;
	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (err != 0)
	{
		std::string errMsg = "WSAStartup failed";
		LOG(errMsg);
		throw std::runtime_error(errMsg); // 如果 WSAStartup 失败，则抛出运行时错误
	}
#endif
}

/**
 * WSAResource 类析构函数
 * 该析构函数用于在对象销毁时清理 Winsock。仅当编译目标为 _WIN32 平台时执行。
 *
 * 参数:
 * 无
 *
 * 返回值:
 * 无
 */
WSAResource::~WSAResource()
{
#ifdef _WIN32
	// 执行 Winsock 清理
	WSACleanup();
#endif
}

void LOG(const std::string& msg)
{
	std::cout << msg << std::endl;
}

