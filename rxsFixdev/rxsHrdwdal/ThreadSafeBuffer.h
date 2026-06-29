//Copyright (c) 2019 KEYENCE CORPORATION. All rights reserved.
#pragma once

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>

#include "../KeyenceLib/ljx8000A/include/LJX8_IF.h"
#include "../KeyenceLib/ljx8000A/include/LJX8_ErrorCode.h"

#include <vector>
#include <mutex>

#define LJX8IF_GUI_DEVICE_COUNT			4
#define LUMINANCE_OUTPUT_ON_VALUE		1

// Maximum amount of data for 1 profile
#define MAX_PROFILE_COUNT				3200

// Multiple value for luminance output on
#define MULTIPLE_VALUE_FOR_LUMINANCE_OUTPUT	2


#define BUFFER_FULL_COUNT				120000



struct PROFILE_DATA
{
	LJX8IF_PROFILE_INFO m_profileInfo;

	LJX8IF_PROFILE_HEADER m_profileHeader;
	int nMultipleValue;
	int m_pnProfileDataLen;
	int* m_pnProfileData;
	LJX8IF_PROFILE_FOOTER m_profileFooter;

	PROFILE_DATA();
	PROFILE_DATA(const LJX8IF_PROFILE_INFO& profileInfo, const LJX8IF_PROFILE_HEADER* header, const int* data, const LJX8IF_PROFILE_FOOTER* footer);
	PROFILE_DATA(const PROFILE_DATA& obj);

	bool GetIsLuminanceOutput(LJX8IF_PROFILE_INFO profileInfo);

	~PROFILE_DATA();
};

class CThreadSafeBuffer
{
//Singleton Pattern
private:
	CThreadSafeBuffer(void);
	static CThreadSafeBuffer* m_threadSafeBuffer;
	static const int BATCH_FINALIZE_FLAG_BIT_COUNT = 16;

public:
	static CThreadSafeBuffer* getInstance(void);
	~CThreadSafeBuffer(void);

	std::mutex m_csDataAccess;
	int m_anBatchNo[LJX8IF_GUI_DEVICE_COUNT];							// Batch number
	unsigned long m_adwCount[LJX8IF_GUI_DEVICE_COUNT];							// Profile data count
	unsigned long m_adwNotify[LJX8IF_GUI_DEVICE_COUNT];							// Callback function notification parameter
	std::vector<PROFILE_DATA> m_vecProfileData[LJX8IF_GUI_DEVICE_COUNT];		// Profile data
	
	void Add(unsigned long dwIndex, std::vector<PROFILE_DATA> &vecProfileData, unsigned long dwNotify);
	void AddCount(unsigned long deIndex, unsigned long dwCount, unsigned long dwNotify);
	void Get(unsigned long dwIndex, unsigned long* pdwNotify, int* pnBatchNo, std::vector<PROFILE_DATA> &vecProfileData);
	void ClearBuffer(int nCurrentDeviceID);
	unsigned long GetCount(unsigned long dwIndex, unsigned long* pdwNotify, int* pnBatchNo);
};
