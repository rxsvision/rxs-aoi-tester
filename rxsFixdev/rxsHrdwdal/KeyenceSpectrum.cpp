#pragma once
#include "rxsHrdwdal.h"
#include "proteced.h"
#include <thread>
#include <locale.h>


#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>

using namespace std;

#define SDKDLL	1

vector<rxsPointCouldp> msdArray;


#if SDKDLL

#include "../KeyenceLib/include/CL3_IF.h"

#define MaxRequestDataLength		512000
// Maximum number of sequential acquired data - 10 seconds at sampling cycle 100us(fastest)
#define MaxSequenceMeasurementData		100000
// Maximum number of acquired data per time
#define MaxMeasurementDataCountPerTime	 8000

struct MeasurementData
{
	CL3IF_ADD_INFO addInfo;
	std::vector<CL3IF_OUTMEASUREMENT_DATA> outMeasurementData;
};

#ifdef _MYX86
#pragma comment(lib,"..\\KeyenceLib\\x86\\CL3_IF.lib")
#else
#pragma comment(lib,"..\\KeyenceLib\\x64\\CL3_IF.lib")
#endif


#else

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define RECVLEN			10000*12 + 128
#define ACPOINTNUMS		10000

WSADATA wd;
SOCKET c_socket;
unsigned char recvbuf[RECVLEN] = { 0 };
#endif // SDKDLL






KeyenceSpectrum::KeyenceSpectrum() {
	result = 0;
	memset(Sensorinfo.DevConnMethod.earth.Ip, 0, 128);
	Sensorinfo.DevConnMethod.earth.Port = 0;
	Sensorinfo.devType = DevType::Confocal;
	CtrInfo = new ControlInfo[1];
	if (CtrInfo != 0)
		memset(CtrInfo, 0, sizeof(ControlInfo));
	Sx = 0.0f;
	Sy = 0.0f;
}

KeyenceSpectrum::~KeyenceSpectrum() {
	Sensorinfo.ptrGrabResult = 0;
	if (CtrInfo != 0) {
		delete[] CtrInfo;
		CtrInfo = 0;
	}
}

short KeyenceSpectrum::open(const char* tsPath) {
	if (tsPath == 0) {
		return -1;
	}
	std::string fp = tsPath;
	int x = fp.find(',');
	if (x < 0) { return 110; }
	string fp1 = fp.substr(0, x);
	string fp2 = fp.substr((x + 1), fp.length() - x - 1);
	Sensorinfo.DevConnMethod.earth.Port = stoi(fp2.c_str());
	memcpy(Sensorinfo.DevConnMethod.earth.Ip, fp1.c_str(), fp1.length());//"192.168.1.10"
	if (Sensorinfo.DevConnMethod.earth.Port == 0 || Sensorinfo.DevConnMethod.earth.Ip[0] == '0') { return -3; }

#ifdef SDKDLL

	CL3IF_ETHERNET_SETTING ethernetSetting = {};
	vector<string> ipad = stringSplit(fp1, '.');

	unsigned int ip1 = atoi(ipad[0].c_str());
	unsigned int ip2 = atoi(ipad[1].c_str());
	unsigned int ip3 = atoi(ipad[2].c_str());
	unsigned int ip4 = atoi(ipad[3].c_str());

	ethernetSetting.portNo = Sensorinfo.DevConnMethod.earth.Port;
	ethernetSetting.ipAddress[0] = ip1;
	ethernetSetting.ipAddress[1] = ip2;
	ethernetSetting.ipAddress[2] = ip3;
	ethernetSetting.ipAddress[3] = ip4;

	ethernetSetting.reserved[0] = 0x00;
	ethernetSetting.reserved[1] = 0x00;
	LONG returnCode = CL3IF_OpenEthernetCommunication(CtrInfo[0].devID, &ethernetSetting, 10000);
	if (returnCode == CL3IF_RC_OK) {
		returnCode = 0;
	}
	result = returnCode;
	return result;

#else

	result = WSAStartup(MAKEWORD(2, 2), &wd);
	if (tsPath == 0) {
		return -2;
	}
	//创建套接字
//SOCKET c_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	c_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (c_socket == INVALID_SOCKET) {
		//cout << "socket error" << endl;
		return -3;
	}

	sockaddr_in c_addr;

	inet_pton(AF_INET, ip, &c_addr.sin_addr.S_un.S_addr);

	c_addr.sin_family = AF_INET;
	c_addr.sin_port = htons(Sensorinfo.DevConnMethod.earth.Port);
	int len = sizeof(c_addr);
	//非阻塞
	unsigned long ul = 1;
	if (ioctlsocket(c_socket, FIONBIO, &ul) != 0) {
		return SOCKET_ERROR;
	}
	result = connect(c_socket, (sockaddr*)&c_addr, len);

	if (result == INVALID_SOCKET) {
		//cout << "connect error" << endl;
		return -4;
	}
	return 0;
#endif // SDKDLL
}


void KeyenceSpectrum::close() {
	msdArray.clear();
	Sensorinfo.ptrGrabResult = 0;
#ifdef SDKDLL
	LONG returnCode = CL3IF_CloseCommunication(CtrInfo[0].devID);
	result = returnCode;

#else
	closesocket(c_socket);
	WSACleanup();

#endif
	if (CtrInfo != 0) {
		delete[] CtrInfo;
		CtrInfo = 0;
	}
}

bool KeyenceSpectrum::ResetConnect() {
	close();
	string c = Sensorinfo.DevConnMethod.earth.Ip;
	c += ",";
	char iport[6];
	sprintf_s(iport, "%d", Sensorinfo.DevConnMethod.earth.Port);
	c += iport;
	return (Open((char*)c.c_str()) == 0);
}

//受到 Mode 影响 = 0内触发; = 1 外触发模式; = 2其他的一些触发采集模式
bool KeyenceSpectrum::GrabTigger(unsigned int gCount) {
#ifdef SDKDLL
	CL3IF_MEASUREMENT_DATA measurementData = {};
	LONG returnCode = CL3IF_GetMeasurementData(CtrInfo[0].devID, &measurementData);
	if (returnCode == CL3IF_RC_OK) {
		measurementData.addInfo.pulseCount;
		measurementData.addInfo.triggerCount;
		for (int i = 0; i < CL3IF_MAX_OUT_COUNT; i++)
		{
			measurementData.outMeasurementData[i].measurementValue;
			static_cast<CL3IF_VALUE_INFO>(measurementData.outMeasurementData[i].valueInfo);
			measurementData.outMeasurementData[i].judgeResult;
		}
		return true;
	}
	return false;
#else

	//先切换到测量模式
	RequestRsp("R0\x0D", 3);
	msdArray.clear();
	//在进行读值
	RequestRsp("MS,0,1\x0D", 7);
	return true;
#endif
}

//受到 Mode 影响 = 0内触发; = 1 内触发模式-高度数据; = 2内触发模式-厚度数据 ， 其他外触发！gCount -期望采集数据量。
bool KeyenceSpectrum::GrabTiggerContinuity(unsigned int gCount) {
	msdArray.clear();
	//sts &= (~2);
#ifdef SDKDLL
	//采集之前先清空一下
	LONG returnCode = CL3IF_ClearStorageData(CtrInfo[0].devID);
	if (returnCode != CL3IF_RC_OK) {

	}
	returnCode = CL3IF_StartStorage(CtrInfo[0].devID);
	return (returnCode == CL3IF_RC_OK);
#else

	//先切换到测量模式
	RequestRsp("R0\x0D", 3);
	//开始存储前 先初始化清空一下
	RequestRsp("DC\x0D", 3);
	//开始数据数据存储
	RequestRsp("DS\x0D", 3);

#endif
	return true;
}

void KeyenceSpectrum::GetSensorparam() {

}

void KeyenceSpectrum::SetSensorparam() {
	//设定模式
	RequestRsp("Q0\x0D", 3);
}


int KeyenceSpectrum::RequestRsp(const char* msg, unsigned msgLen) {
#ifdef SDKDLL




#else
	int ret = recv(c_socket, (char*)recvbuf, RECVLEN, 0);
	memset(recvbuf, 0, RECVLEN);
	ret = send(c_socket, msg, msgLen, 0);
	Sleep(60);
	result = recv(c_socket, (char*)recvbuf, RECVLEN, 0);
#endif

	return result;
}

bool KeyenceSpectrum::inReqrsp() {
	//return ((sts & 4) == 4);
	return true;
}

std::vector<int> ConvertOutTargetList(CL3IF_OUTNO outTarget)
{
	BYTE mask = 1;
	std::vector<int> outList;
	for (int i = 0; i < CL3IF_MAX_OUT_COUNT; i++)
	{
		if ((outTarget & mask) != 0)
		{
			outList.push_back(i + 1);
		}
		mask = static_cast<BYTE>(mask << 1);
	}
	return outList;
}


int KeyenceSpectrum::IsCollectionCompleted(unsigned int gCount) {
#ifdef SDKDLL
	DWORD index = 0;
	LONG m_sequenceStorageReceivedDataCount = 0;
	LONG returnCodeStorageIndex = CL3IF_GetStorageIndex(CtrInfo[0].devID, CL3IF_SELECTED_INDEX_NEWEST, &index);
	if (returnCodeStorageIndex != CL3IF_RC_OK)
	{
		return -1;
	}
	BYTE buffer[MaxRequestDataLength] = {};
	DWORD indexGet = index;
	DWORD m_sequenceStorageIndex = indexGet;
	//std::vector<MeasurementData> storageDataList;
	while (isGrabing())
	{
		Sleep(5);
		DWORD nextIndex = 0;
		DWORD obtainedDataCount = 0;
		CL3IF_OUTNO outTarget = {};
		const LONG returnCodeStorageData = CL3IF_GetStorageData(CtrInfo[0].devID, indexGet,
			MaxMeasurementDataCountPerTime, &nextIndex, &obtainedDataCount, &outTarget, reinterpret_cast<DWORD*>(buffer));
		//if (nextIndex == 0 || returnCodeStorageData != CL3IF_RC_OK)
		//{
		//	result = -2;
		//	break;
		//}
		if (nextIndex == 0) {
			continue;
		}

		indexGet = nextIndex;
		std::vector<int> outTargetList = ConvertOutTargetList(outTarget);
		const BYTE* pReadPosition = buffer;
		int storageDataCount = 0;
		for (DWORD i = 0; i < obtainedDataCount; i++)
		{
			if (MaxSequenceMeasurementData <= i + m_sequenceStorageReceivedDataCount)
			{
				Stop();
				break;
			}
			MeasurementData measurementData = {};
			measurementData.outMeasurementData.resize(outTargetList.size());

			measurementData.addInfo = *reinterpret_cast<const CL3IF_ADD_INFO*>(pReadPosition);
			pReadPosition += sizeof(CL3IF_ADD_INFO);

			for (unsigned int j = 0; j < outTargetList.size(); j++)
			{
				measurementData.outMeasurementData[j] = *reinterpret_cast<const CL3IF_OUTMEASUREMENT_DATA*>(pReadPosition);
				pReadPosition += sizeof(CL3IF_OUTMEASUREMENT_DATA);
			}
			//storageDataList.push_back(measurementData);
			rxsPointCouldp pCp;
			pCp.z = measurementData.outMeasurementData[0].measurementValue / 10000.0f;//转换成mm单位
			pCp.x = Sx;
			pCp.y = Sy;
			msdArray.push_back(pCp);
			storageDataCount++;
		}
		m_sequenceStorageReceivedDataCount += storageDataCount;
		if (m_sequenceStorageReceivedDataCount > gCount) {
			Stop();
			break;
		}
	}
	return m_sequenceStorageReceivedDataCount;
#else

	int pnums = 0;
	while (true) {
		Sleep(50);
		if ((sts & 2) == 2)
		{
			//停止数据存储
			RequestRsp("DT\x0D", 3);
			sts &= (~2);
		}
		RequestRsp("DA\x0D", 3);
		if (recvbuf[0] == 'D' && recvbuf[1] == 'A') {
			string rv = (char*)(&recvbuf[3]);
			rv = rv.erase(rv.length() - 1);
			vector<string> vvv = stringSplit(rv, ',');
			if (vvv.size() > 1) {
				// 0 --数据累积器停止,1 积累中，2 满数据 停止
				int dsSts = stoi(vvv[0]);
				pnums = stoi(vvv[1]);
				if (dsSts == 0) {
					return pnums;
				}

				if (dsSts == 2) {
					return pnums;
				}

				if (gCount < pnums) {//点数 超时
					//停止数据存储
					RequestRsp("DT\x0D", 3);
					sts &= (~2);
					break;
				}
			}

		}
	}
	return pnums;
#endif
}

int KeyenceSpectrum::MakeCmder(char cmd, char* param, unsigned ParamLen, char* CmdBuff) {
	short l = 4 + ParamLen;
	//char cl[4] = { (l >> 24) & 0xFF,(l >> 16) & 0xFF,(l >> 8) & 0xFF,l & 0xFF };
	char cl[4] = { l & 0xFF,(l >> 8) & 0xFF, (l >> 16) & 0xFF,(l >> 24) & 0xFF };

	char headFrame[12] = { 0x0F,0xE0,0x00,0x00,cl[0],cl[1],cl[2],cl[3], cmd ,0,0,0 };

	memcpy(CmdBuff, headFrame, 12);
	memcpy(&CmdBuff[12], param, ParamLen);

	return (12 + ParamLen);
}


bool KeyenceSpectrum::resultAnasyer(unsigned& errCode, unsigned& rspLen) {
#ifdef SDKDLL


#else

	if ((recvbuf[0] == 0x0F) && (recvbuf[1] == 0xE0) && (recvbuf[2] == 0x00) && (recvbuf[3] == 0x00)) {

		unsigned char errcode[4];
		memcpy(errcode, &recvbuf[12], 4);
		errCode = (errcode[3] << 24) | (errcode[2] << 16) | (errcode[1] << 8) | errcode[0];
		if (errCode != 0) {
			return false;
		}

		unsigned char dataLen[4];
		memcpy(dataLen, &recvbuf[4], 4);
		rspLen = (dataLen[3] << 24) | (dataLen[2] << 16) | (dataLen[1] << 8) | dataLen[0];
		return true;
	}
	errCode = -1;
#endif
	return false;
}

bool KeyenceSpectrum::GetDataStoreObj(char objVal[4], bool isNew) {
#ifdef SDKDLL









#else
	char cmd = 0x02;
	char cmdbuff[12 + 4];
	char param[4] = { (isNew ? 1 : 0),0,0,0 };// 1最新的数据对象 ,0 最旧的数据对象
	int cmdLen = MakeCmder(cmd, param, 4, cmdbuff);
	RequestRsp(cmdbuff, cmdLen);
	unsigned errcode = 0;
	unsigned datalen = 0;
	if (resultAnasyer(errcode, datalen)) {
		if (recvbuf[8] != cmd) {
		}
		memcpy(objVal, &recvbuf[16], 4);
		return true;
	}
#endif

	return false;
}


int KeyenceSpectrum::DataRequestRsp(const char* msg, unsigned msgLen) {
#ifdef SDKDLL

	return 0;
#else
	sts |= 4;
	memset(recvbuf, 0, RECVLEN);
	int ret = send(c_socket, msg, msgLen, 0);
	if (ret == SOCKET_ERROR) {
		return -1;
	}
	Sleep(50);
	thread recThread([this]() {
		int totalNum = 0;
		int rdLen = 1000;
		int i = 0, index = 0;
		int maxCnt = ACPOINTNUMS / rdLen;
		while (true)
		{
			index = 0 + rdLen * i;
			result = recv(c_socket, (char*)&recvbuf[index], rdLen, 0);
			if (result > 0)
			{
				totalNum = totalNum + result;
			}
			else
			{
				result = totalNum;
				break;
			}
			i += 1;
			if (i == maxCnt) {
				result = totalNum;
				break;
			}
			Sleep(8);
		}
		sts &= (~4);
		}
	);
	recThread.join();
	return result;
#endif
}

int KeyenceSpectrum::GetDataStoreVal(char nextObjVal[4], unsigned outcount) {
#ifdef SDKDLL

	BYTE buffer[MaxRequestDataLength] = {};
	DWORD index = 0;
	DWORD requestDataCount = outcount;

	if (requestDataCount > MaxMeasurementDataCountPerTime)
	{
		//("requestDataCount is Invalid Value");
		return -1;
	}

	DWORD nextIndex = 0;
	DWORD obtainedDataCount = 0;
	CL3IF_OUTNO outTarget = {};

	LONG returnCode = CL3IF_GetStorageData(CtrInfo[0].devID, index, requestDataCount,
		&nextIndex, &obtainedDataCount, &outTarget, reinterpret_cast<DWORD*>(buffer));

	if (returnCode == CL3IF_RC_OK) {

		return obtainedDataCount;
	}
	result = returnCode;
	return -result;

#else

	char cmd = 0x03;
	char cmdbuff[12 + 8];
	char GLN[4] = { ACPOINTNUMS & 0xFF,(ACPOINTNUMS >> 8) & 0xFF, (ACPOINTNUMS >> 16) & 0xFF,(ACPOINTNUMS >> 24) & 0xFF };

	char param[8] = { nextObjVal[0],nextObjVal[1],nextObjVal[2],nextObjVal[3],GLN[0],GLN[1],0,0 };
	int cmdLen = MakeCmder(cmd, param, 8, cmdbuff);
	DataRequestRsp(cmdbuff, cmdLen);
	unsigned errcode = 0;
	unsigned datalen = 0;
	int isize = outcount / ACPOINTNUMS;
	int mod = outcount % ACPOINTNUMS;
	if (resultAnasyer(errcode, datalen)) {
		for (unsigned jx = 0; jx < isize; jx++) {



		}

		if (recvbuf[8] != cmd) {

		}

		memcpy(nextObjVal, &recvbuf[16], 4);//下个要获取的对象
		int rvDataLen = 0;
		char dbval[4];
		char rval[8];
		memcpy(dbval, &recvbuf[20], 4);
		rvDataLen = ((dbval[1] << 8) | dbval[0]);//dbval[4],dbval[3] 为保留数据
		double fval = 0;
		int index = 0;
		for (unsigned i = 0; i < rvDataLen; i++) {
			memset(dbval, 0, 4);
			memset(rval, 0, 8);
			index = 28 + i * 12;
			memcpy(dbval, &recvbuf[index], 4);//计数值
			index = 32 + i * 12;
			memcpy(rval, &recvbuf[index], 8);//前4个 byte 测量数据，byte 5 测量结果是否异常标记， byte 6 测量判断结果， 后 2 byte保留

			fval = ((rval[3] << 24) | (rval[2] << 16) | (rval[1] << 8) | rval[0]);
			msdArray.push_back(fval);
		}
		return 0;
	}
	return errcode > 90 ? -1 : errCode;
#endif
}


void* KeyenceSpectrum::DataGet(unsigned& outcount) {
#ifdef SDKDLL


#else
	msdArray.clear();
	char fd[4] = { 0,0,0,0 };
	if (!GetDataStoreObj(fd)) {
		outcount = 0;
		//初始化 清空一下 数据存储
		RequestRsp("DC\x0D", 3);
		return 0;
	}
	memset(fd, 0, 4);
	int rcode = GetDataStoreVal(fd, outcount);
	if (rcode < 0) {
	};

	//初始化 清空一下 数据存储
	RequestRsp("DC\x0D", 3);
#endif
	outcount = msdArray.size();
	if (outcount > 0) {

		Sensorinfo.ptrGrabResult = &msdArray[0];
		return (rxsPointCouldp*)Sensorinfo.ptrGrabResult;
	}
	return 0;
}

void KeyenceSpectrum::stop() {
#ifdef SDKDLL
	LONG returnCode = CL3IF_StopStorage(CtrInfo[0].devID);
#endif
}

bool KeyenceSpectrum::TiggerTaber(short wich, bool isClose) {
#ifdef SDKDLL

	isClose = (wich > 2) ? false : true;

	CL3IF_ENCODER_SETTING encoderSetting;
	memset(&encoderSetting, 0, sizeof(encoderSetting));

	encoderSetting.encoderOnOff = isClose ? FALSE : TRUE;
	encoderSetting.operatingMode = 2;//0  关闭，1 时序 TIMING，2 触发 TRIGGER
	//0- "1-phase 1 multiplier"; 1-"2-phase 1 multiplier"; 2-"2-phase 2 multiplier"; 3-"2-phase 4 multiplier";
	encoderSetting.enterMode = CtrInfo[0].offsetLine;
	encoderSetting.decimationPoint = CtrInfo[0].lines;
	encoderSetting.detectionEdge = 2;// 0- Rising;  1-Falling; 2-Both Edge;
	/*
	0("100ns"));
	1("200ns"));
	2("500ns"));
	3("1us"));
	4("2us"));
	5("5us"));
	6("10us"));
	7("20us"));
	*/
	encoderSetting.minInputTime = 2;
	/*
	0("OFF"));
	1("Positive"));
	2("Negative"));
	*/
	encoderSetting.pulseCountOffsetDetectionLogic = 0;
	encoderSetting.presetValue = 0;
	LONG returnCode = CL3IF_SetEncoder(CtrInfo[0].devID, &encoderSetting);
	return (returnCode == CL3IF_RC_OK);

#else

	//先切换成 设定模式
	RequestRsp("Q0\x0D", 3);
	//编码器 或者连续触发模式设置


	return true;
#endif
}

bool KeyenceSpectrum::expActive(short fno, void* param) {
	switch (fno)
	{
	case 1:
		return Dark();
	case 2:
		//OrgAndRange()
		break;
	default:
		break;
	}
	return false;
}


bool KeyenceSpectrum::Dark() {

	return true;
}


//量程零点 及 上下边界设置
bool KeyenceSpectrum::OrgAndRange(float zVal, float upVal, float downVal) {

	return true;
}

void KeyenceSpectrum::CoordinateConvert() {}

bool KeyenceSpectrum::WorldCoordinateSet(u8 wich, float x, float y, float z) {
	bool isEnd = ((wich & 128) == 128);
	if (isEnd) { return true; }
	Sx = x;
	Sx = y;
	return true;
}

bool KeyenceSpectrum::grabContinuity(bool isAnsy, int gCount) {
	return GrabTiggerContinuity(10000);
}