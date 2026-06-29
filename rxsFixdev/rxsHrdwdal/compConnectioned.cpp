#include "rxsHrdwdal.h"
#include "proteced.h"
#include <Windows.h>
#include <string>
#include <iostream>

//using namespace std;

#define RC_BUFFZISE    1024

/*******************************************************************
* 名称： openSyn
* 功能： 同步方式打开串口,并配置默认信息
* 参数：
    serial_name：串口名称
    baud_rate  ：波特率，取值如下
        ......
        CBR_9600    9600bps
        CBR_14400   14400bps
        ......
    parity     ：校验方式
        EVENPARITY  偶校验
        MARKPARITY  标号校验
        NOPARITY    无校验
        ODDPARITY   奇校验
        SPACEPARITY 空格校验
    byte_size  ：数据位大小
        4，5，6，7，8
    stop_bits  ：停止位
        ONESTOPBIT      1个停止位
        ONE5STOPBITS    1.5个停止位
        TWOSTOPBITS     2个停止位
* 返回： 正确返回为ture，错误返回为false
*******************************************************************/

short r232Connection::Comopen(const char* serial_name, int baud_rate, unsigned char parity, unsigned char byte_size, unsigned char stop_bits) {
    if (!openSyn(serial_name))
        return false;
    DCB dcb;
    if (false == GetCommState(hCom, &dcb))//获得当前串口的配置信息
    {
        setSerialLastError("SerialInterface::open() : GetCommState Error");
        return false;
    }

    dcb.DCBlength = sizeof(DCB);
    dcb.BaudRate = baud_rate;
    dcb.Parity = parity;
    dcb.ByteSize = byte_size;
    dcb.StopBits = stop_bits;

    if (false == SetCommState(hCom, &dcb))//用DCB结构重新配置串行端口信息
    {
        setSerialLastError("SerialInterface::open() : SetCommState Error");
        return false;
    }

    //超时处理
    COMMTIMEOUTS timeouts;
    timeouts.ReadIntervalTimeout = MAXDWORD; //读间隔超时
    // 把间隔超时设为最大，把总超时设为0将导致ReadFile立即返回并完成操作 

    timeouts.ReadTotalTimeoutMultiplier = 0; //读时间系数
    timeouts.ReadTotalTimeoutConstant = 0; //读时间常量
    timeouts.WriteTotalTimeoutMultiplier = 50; // 写时间系数
    timeouts.WriteTotalTimeoutConstant = 2000; //写时间常量
    //总的读/写超时时间 = Read(Write)TotalTimeoutMultiplier x 要读/写的字节数 + Read(Write)TotalTimeoutConstant. 
    if (false == SetCommTimeouts(hCom, &timeouts))
    {
        setSerialLastError("SerialInterface::open() : SetCommTimeouts Error");
        return false;
    }

    //清空缓冲区，为读写串口做准备
    if (false == PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_RXABORT))
    {
        setSerialLastError("SerialInterface::open() : PurgeComm Error");
        return false;
    }
    return true;
}


bool r232Connection::openSyn(const char* serial_name)
{
    hCom = CreateFileA(
        serial_name,//普通文件名或者设备文件名，这里是串口名
        GENERIC_READ | GENERIC_WRITE,//访问模式,读和写
        0,//共享模式，独占模式
        NULL,//指向安全属性的指针,不使用，传NULL
        OPEN_EXISTING,//如何创建,在串口中必须设置为OPEN_EXISTING。表示不能创建新端口只能打开已有的端口。
        FILE_ATTRIBUTE_NORMAL,//文件属性,使用默认属性FILE_ATTRIBUTE_NORMAL。
        NULL//用于复制文件句柄,通常这个参数设置为NULL，为空表示不使用模板
    );

    if (INVALID_HANDLE_VALUE == hCom)//出错判断
    {
        hCom = NULL;
        setSerialLastError("open():CreateFileA Error!");
        return false;
    }
    return true;
}

void r232Connection::setSerialLastError(const char* error_msg)
{
    std::cout << error_msg << std::endl;
}

void* r232Connection::GetRsp(unsigned& len) {
    len = 0;
    if (false == PurgeComm(hCom, PURGE_TXCLEAR))
    {
        setSerialLastError("SerialInterface::purgeBuff() : PurgeComm Error");
        return 0;
    }
    if (content == 0) { return 0; }
    memset(content, 0, RC_BUFFZISE);
    DWORD writeSize = 0;
    bool ret = ReadFile(hCom, content, RC_BUFFZISE, &writeSize, NULL);
    len = writeSize;
    return (ret) ? content : 0;
}

int r232Connection::SendReq(void* data, unsigned len) {
    if (false == PurgeComm(hCom, PURGE_RXCLEAR))
    {
        setSerialLastError("SerialInterface::purgeBuff() : PurgeComm Error");
        return 0;
    }
    DWORD writeSize = 0;
    bool ret = WriteFile(hCom, data, len, &writeSize, NULL);
    if (0 == ret)
    {
        //*(string*)last_error = "Error By writeStr(string str)";
        return 0;
    }
    //*(string *)last_error = "";
    return writeSize;
}


r232Connection::r232Connection() {
    content = malloc(RC_BUFFZISE);
}

r232Connection::~r232Connection() {
    if (NULL != hCom)
    {
        CloseHandle(hCom);
        hCom = NULL;
    }

    if (content != 0) {
        free(content);
        content = 0;
    }
}