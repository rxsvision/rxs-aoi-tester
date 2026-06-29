#include "rxsHrdwdal.h"

#include <iostream>


#include "ThreadSafeBuffer.h"

#ifdef _MYX86
#pragma comment(lib,"..\\KeyenceLib\\ljx8000A\\x86\\LJX8_IF.lib")
#else
#pragma comment(lib,"..\\KeyenceLib\\ljx8000A\\x64\\LJX8_IF.lib")
#endif




#define ISHEIGHTSpeed_			1

#define ZRATE			0.00001
#define	XRATE_8080		0.0125
#define	XRATE_8060		0.005
#define	YRATE			0.010


#define	PROGMA_NO		0x02


/*
 Constructor for PROFILE_DATA
*/
PROFILE_DATA::PROFILE_DATA()
	:m_pnProfileData(NULL)
{
	m_pnProfileDataLen = 0;
	nMultipleValue = 1;
}

PROFILE_DATA::PROFILE_DATA(const LJX8IF_PROFILE_INFO& profileInfo, const LJX8IF_PROFILE_HEADER* header, const int* data, const LJX8IF_PROFILE_FOOTER* footer)
{
	m_profileInfo = profileInfo;
	m_profileHeader = *header;
	nMultipleValue = GetIsLuminanceOutput(profileInfo) ? MULTIPLE_VALUE_FOR_LUMINANCE_OUTPUT : 1;
	m_pnProfileDataLen = profileInfo.wProfileDataCount * nMultipleValue * profileInfo.byProfileCount;
	m_pnProfileData = new int[m_pnProfileDataLen];
	memcpy_s(m_pnProfileData, sizeof(int) * m_pnProfileDataLen, data, sizeof(int) * m_pnProfileDataLen);
	m_profileFooter = *footer;
}


PROFILE_DATA::PROFILE_DATA(const PROFILE_DATA& obj)
{
	m_profileInfo = obj.m_profileInfo;
	m_profileHeader = obj.m_profileHeader;
	m_profileFooter = obj.m_profileFooter;

	nMultipleValue = GetIsLuminanceOutput(obj.m_profileInfo) ? MULTIPLE_VALUE_FOR_LUMINANCE_OUTPUT : 1;
	m_pnProfileDataLen = obj.m_profileInfo.wProfileDataCount * obj.m_profileInfo.byProfileCount * nMultipleValue;
	m_pnProfileData = new int[m_pnProfileDataLen];
	for (int i = 0; i < m_pnProfileDataLen; i++)
	{
		m_pnProfileData[i] = obj.m_pnProfileData[i];
	}
}

bool PROFILE_DATA::GetIsLuminanceOutput(LJX8IF_PROFILE_INFO profileInfo)
{
	return profileInfo.byLuminanceOutput == LUMINANCE_OUTPUT_ON_VALUE;
}


PROFILE_DATA::~PROFILE_DATA()
{
	delete[] m_pnProfileData;
}




typedef struct _devceData {
	int ProfReceiveCount = 0;
	bool m_bIsBufferFull = false;
	float xRate;
	LJX8IF_PROFILE_INFO m_aProfileInfo;
	LJX8IF_ETHERNET_CONFIG m_ethernetConfig;
	std::vector<PROFILE_DATA> m_vecProfileData;					// Profile Data
	std::vector<PROFILE_DATA> m_vecProfileDataHighSpeed;			// Profile Data for high speed communication
	std::vector<rxsPointCouldp> pc;
	rxsPointCouldp StartPoint;
}DevData;




DevData	m_aDeviceData[LJX8IF_GUI_DEVICE_COUNT];




KeyenceLJX8000A::KeyenceLJX8000A() {
	Sensorinfo.devType = DevType::LineLsr;
	LONG lRc = LJX8IF_Initialize();
	LJX8IF_VERSION_INFO versionInfo = LJX8IF_GetVersion();
	CtrInfo = new ControlInfo[LJX8IF_GUI_DEVICE_COUNT];
	for (int x = 0; x < LJX8IF_GUI_DEVICE_COUNT; x++) {
		CtrInfo[x].devID = x;
	}
	m_aDeviceData[0].xRate = XRATE_8060;
	m_aDeviceData[1].xRate = XRATE_8080;
	m_aDeviceData[2].xRate = XRATE_8060;
	m_aDeviceData[3].xRate = XRATE_8080;
}



KeyenceLJX8000A::~KeyenceLJX8000A() {
	close();
	if (CtrInfo != 0) {
		delete[] CtrInfo;
		CtrInfo = 0;
	}
}

void KeyenceLJX8000A::CoordinateConvert() {


}

bool KeyenceLJX8000A::WorldCoordinateSet(u8 wich, float x, float y, float z) {
	for (int x = 0; x < LJX8IF_GUI_DEVICE_COUNT; x++) {
		m_aDeviceData[x].StartPoint.x = x;
		m_aDeviceData[x].StartPoint.y = y;
		m_aDeviceData[x].StartPoint.z = z;
	}
	return true;
}

////SimpleArray---另一种将轮廓数据中包含的高度数据和亮度数据以容易转换为Bitmap等格式存储在缓存中（Simple Array）
void ReceiveHighSpeedSimpleArray(LJX8IF_PROFILE_HEADER* p, unsigned short* pHeightProfileArray, unsigned short* pLuminanceProfileArray,
	unsigned long dwLuminanceEnable, unsigned long dwProfileDataCount, unsigned long dwCount, unsigned long dwNotify, unsigned long dwUser)
{
	LJX8IF_PROFILE_HEADER* pProfileHeaderArray = (LJX8IF_PROFILE_HEADER*)p;

	//m_bIsBufferFull[dwUser] = 
	//m_aDeviceData[dwUser].m_simpleArrayStoreHighSpeed.AddReceivedData(pHeightProfileArray, pLuminanceProfileArray, dwCount);
	//m_aDeviceData[dwUser].m_simpleArrayStoreHighSpeed.AddNotify(dwNotify);
}


void ReceiveHighSpeedData(BYTE* pBuffer, DWORD dwSize, DWORD dwCount, DWORD dwNotify, DWORD dwUser)
{
	std::vector<PROFILE_DATA> vecProfileData;

	int nProfileDataCount = (dwSize - sizeof(LJX8IF_PROFILE_HEADER) - sizeof(LJX8IF_PROFILE_FOOTER)) / sizeof(DWORD);

	for (DWORD i = 0; i < dwCount; i++)
	{
		BYTE* pbyBlock = pBuffer + dwSize * i;
		LJX8IF_PROFILE_HEADER* pHeader = (LJX8IF_PROFILE_HEADER*)pbyBlock;
		int* pnProfileData = (int*)(pbyBlock + sizeof(LJX8IF_PROFILE_HEADER));
		LJX8IF_PROFILE_FOOTER* pFooter = (LJX8IF_PROFILE_FOOTER*)(pbyBlock + dwSize - sizeof(LJX8IF_PROFILE_FOOTER));
		vecProfileData.push_back(PROFILE_DATA(m_aDeviceData[dwUser].m_aProfileInfo, pHeader, pnProfileData, pFooter));
	}
	CThreadSafeBuffer* threadSafeBuf = CThreadSafeBuffer::getInstance();
	if (threadSafeBuf->m_vecProfileData[dwUser].size() + vecProfileData.size() < BUFFER_FULL_COUNT)
	{
		threadSafeBuf->Add(dwUser, vecProfileData, dwNotify);
	}
	else
	{
		m_aDeviceData[dwUser].m_bIsBufferFull = true;
	}
}

short KeyenceLJX8000A::open(const char* tsPath) {
	std::string fp;
	if (tsPath == 0) {
		fp = Sensorinfo.DevConnMethod.earth.Ip;
		fp += "," + std::to_string(Sensorinfo.DevConnMethod.earth.Port);
	}
	else {
		fp = tsPath;
	}
	std::vector<std::string> params = stringSplit(fp, ',');

	Sensorinfo.DevConnMethod.earth.Port = std::stoi(params[1].c_str());
	memcpy(Sensorinfo.DevConnMethod.earth.Ip, params[0].c_str(), params[0].length());
	if (Sensorinfo.DevConnMethod.earth.Port == 0 || Sensorinfo.DevConnMethod.earth.Ip[0] == '0') { return -3; }

	std::vector<std::string> ipad = stringSplit(params[0], '.');

	unsigned int ip1 = atoi(ipad[0].c_str());
	unsigned int ip2 = atoi(ipad[1].c_str());
	unsigned int ip3 = atoi(ipad[2].c_str());
	unsigned int ip4 = atoi(ipad[3].c_str());

	LJX8IF_ETHERNET_CONFIG ethernetConfig;

	ethernetConfig.abyIpAddress[0] = ip1;
	ethernetConfig.abyIpAddress[1] = ip2;
	ethernetConfig.abyIpAddress[2] = ip3;
	ethernetConfig.abyIpAddress[3] = ip4;
	
	ethernetConfig.wPortNo = Sensorinfo.DevConnMethod.earth.Port;
	ethernetConfig.reserve[0] = (BYTE)0;
	ethernetConfig.reserve[1] = (BYTE)0;

	for (int x = 0; x < LJX8IF_GUI_DEVICE_COUNT; x++) {

		m_aDeviceData[x].m_ethernetConfig = ethernetConfig;
		m_aDeviceData[x].m_ethernetConfig.abyIpAddress[3] = m_aDeviceData[x].m_ethernetConfig.abyIpAddress[3] + x;

		LONG lRc = LJX8IF_EthernetOpen(CtrInfo[x].devID, &m_aDeviceData[x].m_ethernetConfig);
		if (LJX8IF_RC_OK != lRc) { return -1; }

		lRc = LJX8IF_ChangeActiveProgram((LONG)CtrInfo[x].devID, PROGMA_NO);

#ifdef ISHEIGHTSpeed_

		DWORD m_dwProfileCount = 10;
		//SimpleArray---另一种将轮廓数据中包含的高度数据和亮度数据以容易转换为Bitmap等格式存储在缓存中（Simple Array）
		//lRc = LJX8IF_InitializeHighSpeedDataCommunicationSimpleArray(
		//	(LONG)CtrInfo[0].devID, &ethernetConfig,
		//	(WORD)ethernetConfig.wPortNo+1,
		//	ReceiveHighSpeedSimpleArray,
		//	m_dwProfileCount, CtrInfo[0].devID);
		//if (LJX8IF_RC_OK != lRc) { return -2; }

		lRc = LJX8IF_InitializeHighSpeedDataCommunication(
			(LONG)CtrInfo[x].devID, &m_aDeviceData[x].m_ethernetConfig,
			(WORD)m_aDeviceData[x].m_ethernetConfig.wPortNo + 1,
			//(m_bIsCountOnly ? CountProfileReceive : ReceiveHighSpeedData),
			ReceiveHighSpeedData,
			m_dwProfileCount, CtrInfo[x].devID);

		if (LJX8IF_RC_OK != lRc) { return -2; }


#endif // ISHEIGHTSpeed_
	}

	return 0;
}







void KeyenceLJX8000A::close() {
	LONG lRc = LJX8IF_RC_OK;
	for (int x = 0; x < LJX8IF_GUI_DEVICE_COUNT; x++) {
		lRc = LJX8IF_StopHighSpeedDataCommunication((LONG)CtrInfo[x].devID);
		lRc = LJX8IF_StopMeasure(CtrInfo[x].devID);

#ifdef ISHEIGHTSpeed_
		lRc = LJX8IF_FinalizeHighSpeedDataCommunication((LONG)CtrInfo[x].devID);

#endif // ISHEIGHTSpeed_

		lRc = LJX8IF_CommunicationClose(CtrInfo[x].devID);
	}
	lRc = LJX8IF_Finalize();
}


void KeyenceLJX8000A::stop() {
	LONG lRc = LJX8IF_RC_OK;
	for (int x = 0; x < LJX8IF_GUI_DEVICE_COUNT; x++) {
		lRc = LJX8IF_StopHighSpeedDataCommunication((LONG)CtrInfo[x].devID);
		lRc = LJX8IF_StopMeasure(CtrInfo[x].devID);
	}
}



bool KeyenceLJX8000A::grabContinuity(bool isAnsy, int gCount) {
	LONG lRc = LJX8IF_RC_OK;

	//根据具体的主机设定来
	lRc = LJX8IF_StopMeasure(CtrInfo[0].devID);
	lRc = LJX8IF_ClearMemory(CtrInfo[0].devID);
	//根据具体的主机设定来
	lRc = LJX8IF_StopMeasure(CtrInfo[2].devID);
	lRc = LJX8IF_ClearMemory(CtrInfo[2].devID);


	for (int x = 0; x < LJX8IF_GUI_DEVICE_COUNT; x++) {
		m_aDeviceData[x].ProfReceiveCount = 0;
		m_aDeviceData[x].m_bIsBufferFull = false;
		m_aDeviceData[x].m_vecProfileDataHighSpeed.clear();
		m_aDeviceData[x].pc.clear();

#ifdef ISHEIGHTSpeed_

		LJX8IF_HIGH_SPEED_PRE_START_REQ request;
		request.bySendPosition = 2;
		LJX8IF_PROFILE_INFO profileInfo;

		lRc = LJX8IF_PreStartHighSpeedDataCommunication((LONG)CtrInfo[x].devID, &request, &profileInfo);
		if (lRc != LJX8IF_RC_OK) return false;
		m_aDeviceData[x].m_aProfileInfo = profileInfo;


		lRc = LJX8IF_StartHighSpeedDataCommunication((LONG)CtrInfo[x].devID);
		if (lRc != LJX8IF_RC_OK) return false;
#endif // ISHEIGHTSpeed_

	}

	//根据具体的主机设定来
	lRc = LJX8IF_StartMeasure(CtrInfo[0].devID);
	lRc = LJX8IF_StartMeasure(CtrInfo[2].devID);

	return true;
}


bool KeyenceLJX8000A::ResetConnect() {
	close();
	Sleep(100);
	LONG lRc = LJX8IF_Initialize();
	LJX8IF_VERSION_INFO versionInfo = LJX8IF_GetVersion();
	return (open() == 0);
}


void KeyenceLJX8000A::GetSensorparam() {

}

void KeyenceLJX8000A::SetSensorparam() {

}

int KeyenceLJX8000A::IsCollectionCompleted(unsigned int gCount) {
	int nums = 0;
	while (isGrabing()) {
		Sleep(10);
		int nBatchNo = 0;
		DWORD dwNotify = 0;
		for (int i = 0; i < LJX8IF_GUI_DEVICE_COUNT; i++)
		{
			CThreadSafeBuffer* threadSafeBuf = CThreadSafeBuffer::getInstance();
			std::vector<PROFILE_DATA> vecProfileData;
			threadSafeBuf->Get(i, &dwNotify, &nBatchNo, vecProfileData);

			if (vecProfileData.size() == 0 && dwNotify == 0) continue;

			for (unsigned int j = 0; j < vecProfileData.size(); j++)
			{
				if (m_aDeviceData[i].m_vecProfileDataHighSpeed.size() < BUFFER_FULL_COUNT)
				{
					int index = m_aDeviceData[i].m_vecProfileDataHighSpeed.size();
					m_aDeviceData[i].m_vecProfileDataHighSpeed.push_back(vecProfileData.at(j));
					PROFILE_DATA* pd = &(m_aDeviceData[i].m_vecProfileDataHighSpeed[index]);
					int hdl = pd->m_pnProfileDataLen / pd->nMultipleValue;
					for (int xj = 0; xj < hdl; xj++) {
						if (pd->m_pnProfileData[xj] < -2147483644) {
							continue;
						}
						double x = m_aDeviceData[i].xRate * xj + m_aDeviceData[0].StartPoint.x;
						double y = -YRATE * index + m_aDeviceData[0].StartPoint.y;
						//(pd->m_pnProfileData[xj] < -2147483644) ? -99.9f : 
						rxsPointCouldp p = { x,y,pd->m_pnProfileData[xj] * ZRATE };
						m_aDeviceData[i].pc.push_back(p);
					}
				}
				m_aDeviceData[i].ProfReceiveCount++;
			}
			if (dwNotify == 0) continue;
		}
	}
	return m_aDeviceData[1].pc.size();
}


void* KeyenceLJX8000A::DataGet(unsigned& outcount) {

#ifdef ISHEIGHTSpeed_
	if (slectedChannelno < LJX8IF_GUI_DEVICE_COUNT) {
		outcount = m_aDeviceData[slectedChannelno].pc.size();
		Sensorinfo.ptrGrabResult = &m_aDeviceData[slectedChannelno].pc[0];
		return Sensorinfo.ptrGrabResult;
	}
#else


#endif // ISHEIGHTSpeed_
	outcount = 0;
	return 0;
}


rxsPointCouldp* KeyenceLJX8000A::DataGetPointCould(unsigned& num) {


	return (rxsPointCouldp*)DataGet(num);

}