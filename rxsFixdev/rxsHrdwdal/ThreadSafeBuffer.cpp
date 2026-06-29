//Copyright (c) 2019 KEYENCE CORPORATION. All rights reserved.

#include "ThreadSafeBuffer.h"

// Singleton Pattern

CThreadSafeBuffer* CThreadSafeBuffer::m_threadSafeBuffer = 0;

CThreadSafeBuffer::CThreadSafeBuffer(void)
{
	for(int i = 0; i < LJX8IF_GUI_DEVICE_COUNT; i++){ 
		m_adwCount[i]  = 0; 
		m_adwNotify[i] = 0;
		m_anBatchNo[i] = 0;
	}
	//for(int i = 0; i < LJX8IF_GUI_DEVICE_COUNT; i++){ }
	//for(int i = 0; i < LJX8IF_GUI_DEVICE_COUNT; i++){ }
}

CThreadSafeBuffer::~CThreadSafeBuffer(void)
{
	if (m_threadSafeBuffer != NULL)
	{
		delete m_threadSafeBuffer;
	}
}

CThreadSafeBuffer* CThreadSafeBuffer::getInstance(void)
{ 
	if(m_threadSafeBuffer == 0)
	{
		m_threadSafeBuffer = new CThreadSafeBuffer();
	}
	return m_threadSafeBuffer;
}

/*
 @note Add the profile data
 @param Current device ID
 @param Profile data
 @param notify
*/
void CThreadSafeBuffer::Add(unsigned long dwIndex, std::vector<PROFILE_DATA> &vecProfileData, unsigned long dwNotify)
{

	m_csDataAccess.lock();

	for(unsigned int i = 0; i < vecProfileData.size(); i++)
	{
		m_vecProfileData[dwIndex].push_back(vecProfileData.at(i));
	}

	m_adwCount[dwIndex]  += (unsigned int)vecProfileData.size();
	m_adwNotify[dwIndex] |= dwNotify; 
	if ((dwNotify & (0x1 << BATCH_FINALIZE_FLAG_BIT_COUNT)) != 0) m_anBatchNo[dwIndex]++;
	
	m_csDataAccess.unlock();
}

/*
 @note Add the profile count
 @param Current device ID
 @param Profile count
 @param notify
*/
void CThreadSafeBuffer::AddCount(unsigned long dwIndex, unsigned long dwCount, unsigned long dwNotify)
{
	m_csDataAccess.lock();

	m_adwCount[dwIndex]  += dwCount;
	m_adwNotify[dwIndex] |= dwNotify; 
	if ((dwNotify & (0x1 << BATCH_FINALIZE_FLAG_BIT_COUNT)) != 0) m_anBatchNo[dwIndex]++;
	
	m_csDataAccess.unlock();
}

/*
 @note Get the profile
 @param Current device ID
 @param Notify
 @param Batch Number
 @param Profile data
*/
void CThreadSafeBuffer::Get(unsigned long dwIndex, unsigned long* pdwNotify, int* pnBatchNo, std::vector<PROFILE_DATA> &vecProfileData)
{
	m_csDataAccess.lock();

	vecProfileData = m_vecProfileData[dwIndex];		// Store the data
	m_vecProfileData[dwIndex].clear();				// Clear the data

	*pdwNotify = m_adwNotify[dwIndex];
	m_adwNotify[dwIndex] = 0;
	*pnBatchNo = m_anBatchNo[dwIndex];
	
	m_csDataAccess.unlock();
}

/*
 @note Get the profile count
 @param Current device ID
 @param Notify
 @param Batch Number
*/
unsigned long CThreadSafeBuffer::GetCount(unsigned long dwIndex, unsigned long* pdwNotify, int* pnBatchNo)
{
	m_csDataAccess.lock();

	*pdwNotify = m_adwNotify[dwIndex];
	m_adwNotify[dwIndex] = 0;
	*pnBatchNo = m_anBatchNo[dwIndex];
	m_csDataAccess.unlock();

	return m_adwCount[dwIndex];
}

/*
 @note Clear the buffer data
 @param Current Device ID
*/
void CThreadSafeBuffer::ClearBuffer(int nCurrentDeviceID)
{
	m_csDataAccess.lock();

	m_vecProfileData[nCurrentDeviceID].clear();
	m_vecProfileData[nCurrentDeviceID].shrink_to_fit();
	m_adwCount[nCurrentDeviceID]  = 0;
	m_anBatchNo[nCurrentDeviceID] = 0;
	m_adwNotify[nCurrentDeviceID] = 0;

	m_csDataAccess.unlock();
}