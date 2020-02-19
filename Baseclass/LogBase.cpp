#include "StdAfx.h"
#include "LogBase.h"
//#include <imagehlp.h>
//#include <shlobj.h>

CLockCS CLogBase::m_lock;
CLogBase::CLogBase(void)
	: m_bInited(FALSE)
#ifdef ENABLE_LOG_PROXY
#else //#ifdef ENABLE_LOG_PROXY
	, m_hLogFile(INVALID_HANDLE_VALUE), m_dwMaxLogFileSize(MAX_LOGFILE_SIZE),  m_timeridFlushFile(0), m_timeridHeartbeat(0), m_dwLastLogStamp(0)
#endif //#ifdef ENABLE_LOG_PROXY	
{
	WCHAR szPath[MAX_PATH] = { 0 };
	SHGetSpecialFolderPath(NULL, szPath, CSIDL_APPDATA, FALSE);
	PathAppend(szPath, L"Lenovo\\lebo\\config.ini");
	m_nLogLevel=GetPrivateProfileInt(L"Default", L"LogLevel",LOG_LEVEL_MEDIUM,szPath);
}

CLogBase::~CLogBase(void)
{
	UninitLog();
}

void CLogBase::_WriteLogW(LPCWSTR lpwsFormat, ...)
{
	if(NULL == lpwsFormat)
	{
		ATLASSERT(FALSE);
		return ;
	}

	WCHAR* pwsBuffer = new WCHAR[DEF_LOGBUFFER_SIZE];
	memset(pwsBuffer,0, sizeof(WCHAR)*DEF_LOGBUFFER_SIZE);

	va_list argptr;
	va_start( argptr, lpwsFormat);
	_vsnwprintf_s(pwsBuffer, DEF_LOGBUFFER_SIZE, _TRUNCATE, lpwsFormat, argptr);
	va_end(argptr);

	CLogBase* pLog = CLogBase::GetDefLogInstance();
	if(pLog->IsInited())
		pLog->WriteLogW(pwsBuffer);

	delete [] pwsBuffer;
}

void CLogBase::_WriteLogA(LPCSTR lpsFormat, ...)
{
	if(NULL == lpsFormat)
	{
		ATLASSERT(FALSE);
		return ;
	}

	CHAR* psBuffer = new CHAR[DEF_LOGBUFFER_SIZE];
	memset(psBuffer,0, sizeof(CHAR)*DEF_LOGBUFFER_SIZE);

	va_list argptr;
	va_start( argptr, lpsFormat);
	_vsnprintf_s(psBuffer, DEF_LOGBUFFER_SIZE, _TRUNCATE, lpsFormat, argptr);
	va_end(argptr);

	CLogBase* pLog = CLogBase::GetDefLogInstance();
	if(pLog->IsInited())
		pLog->WriteLogA(psBuffer);

	delete [] psBuffer;
}

void CLogBase::_WriteLogWithTimeStampW(int level,LPCWSTR lpwsFormat, ...)
{
	if(NULL == lpwsFormat)
	{
		ATLASSERT(FALSE);
		return ;
	}

	CLogBase* pLog = CLogBase::GetDefLogInstance();
	if(pLog->IsInited() && level>=pLog->GetLogLevel())
	{
		SYSTEMTIME st = {0};
		GetLocalTime(&st);

		WCHAR* psBuffer = new WCHAR[DEF_LOGBUFFER_SIZE];
		memset(psBuffer,0, sizeof(WCHAR)*DEF_LOGBUFFER_SIZE);

		va_list argptr;
		va_start( argptr, lpwsFormat);
		_vsnwprintf_s(psBuffer, DEF_LOGBUFFER_SIZE, _TRUNCATE, lpwsFormat, argptr);
		va_end(argptr);
		
		pLog->WriteLogExW(L"[%02d-%02d-%02d %02d:%02d:%02d.%03d]%s\r\n", 
			st.wYear, st.wMonth, st.wDay, 
			st.wHour, st.wMinute, st.wSecond, 
			st.wMilliseconds, (LPCWSTR)psBuffer);

		delete [] psBuffer;
	}
}

void CLogBase::_WriteLogWithTimeStampA(int level,LPCSTR lpsFormat, ...)
{
	if(NULL == lpsFormat)
	{
		ATLASSERT(FALSE);
		return ;
	}

	CLogBase* pLog = CLogBase::GetDefLogInstance();
	if(pLog->IsInited())
	{
		SYSTEMTIME st = {0};
		GetLocalTime(&st);

		CHAR* psBuffer = new CHAR[DEF_LOGBUFFER_SIZE];
		memset(psBuffer,0, sizeof(CHAR)*DEF_LOGBUFFER_SIZE);

		va_list argptr;
		va_start( argptr, lpsFormat);
		_vsnprintf_s(psBuffer, DEF_LOGBUFFER_SIZE, _TRUNCATE, lpsFormat, argptr);
		va_end(argptr);

		pLog->WriteLogExA("[%02d-%02d-%02d %02d:%02d:%02d.%03d]%s\r\n", 
			st.wYear, st.wMonth, st.wDay, 
			st.wHour, st.wMinute, st.wSecond, 
			st.wMilliseconds, psBuffer);

		delete [] psBuffer;
	}
}


void CLogBase::_WriteTraceWithTimeStampW(LPCWSTR lpwsFormat, ...)
{
	if (!CLogBase::IsEnableTrace())
		return;

	if (NULL == lpwsFormat)
	{
		ATLASSERT(FALSE);
		return;
	}

	CLogBase* pLog = CLogBase::GetDefLogInstance();
	if (pLog->IsInited())
	{
		SYSTEMTIME st = { 0 };
		GetLocalTime(&st);

		WCHAR* psBuffer = new WCHAR[DEF_LOGBUFFER_SIZE];
		memset(psBuffer, 0, sizeof(WCHAR)*DEF_LOGBUFFER_SIZE);

		va_list argptr;
		va_start(argptr, lpwsFormat);
		_vsnwprintf_s(psBuffer, DEF_LOGBUFFER_SIZE, _TRUNCATE, lpwsFormat, argptr);
		va_end(argptr);

		pLog->WriteLogExW(L"[%02d-%02d-%02d %02d:%02d:%02d.%03d][TRACE][TID:%#X]%s\r\n",
			st.wYear, st.wMonth, st.wDay,
			st.wHour, st.wMinute, st.wSecond,
			st.wMilliseconds, GetCurrentThreadId(), (LPCWSTR)psBuffer);

		delete[] psBuffer;
	}
}

void CLogBase::_WriteTraceWithTimeStampA(LPCSTR lpsFormat, ...)
{
	if (!CLogBase::IsEnableTrace())
		return;

	if (NULL == lpsFormat)
	{
		ATLASSERT(FALSE);
		return;
	}

	CLogBase* pLog = CLogBase::GetDefLogInstance();
	if (pLog->IsInited())
	{
		SYSTEMTIME st = { 0 };
		GetLocalTime(&st);

		CHAR* psBuffer = new CHAR[DEF_LOGBUFFER_SIZE];
		memset(psBuffer, 0, sizeof(CHAR)*DEF_LOGBUFFER_SIZE);

		va_list argptr;
		va_start(argptr, lpsFormat);
		_vsnprintf_s(psBuffer, DEF_LOGBUFFER_SIZE, _TRUNCATE, lpsFormat, argptr);
		va_end(argptr);

		pLog->WriteLogExA("[%02d-%02d-%02d %02d:%02d:%02d.%03d][TRACE][TID:%#X]%s\r\n",
			st.wYear, st.wMonth, st.wDay,
			st.wHour, st.wMinute, st.wSecond,
			st.wMilliseconds, GetCurrentThreadId(), psBuffer);

		delete[] psBuffer;
	}
}

#ifdef ENABLE_LOG_PROXY

HRESULT CLogBase::InitLog(IServiceProvider* lpSP)
{
	AUTOLOCK(m_lock);
	if(NULL == lpSP)
		return E_INVALIDARG;

	if(m_bInited)
		return S_FALSE;

	BOOL bOk = FALSE;
	do 
	{
		lpSP->QueryService(SID_ServiceLog, __uuidof(IServiceLog), (LPVOID*)&m_spLog);
		if(!m_spLog)
			break;

		bOk = TRUE;
	} while (FALSE);

	if(!bOk)
	{
		m_spLog.Release();
		return E_FAIL;
	}

	if(CMessageThread::IsRunning())
		CMessageThread::DestroyThread();

	HRESULT hr = CMessageThread::StartThread("LogBaseProxy", NULL);

	if((S_OK == hr && m_bInited))
		return S_OK;

	m_spLog.Release();
	
	return E_FAIL;
}

#else //#ifdef ENABLE_LOG_PROXY

HRESULT CLogBase::InitLog(LPCWSTR lpwsLogFile, DWORD dwMaxLogFileSize/* = MAX_LOGFILE_SIZE*/)
{
	AUTOLOCK(m_lock);
	if(NULL == lpwsLogFile)
		return E_INVALIDARG;

	if(m_bInited)
		return S_FALSE;

	if(CMessageThread::IsRunning())
		CMessageThread::DestroyThread();

	m_strLogFile = lpwsLogFile;
	m_dwMaxLogFileSize = dwMaxLogFileSize;
	HRESULT hr = CMessageThread::StartThread(NULL, NULL);

	return (S_OK == hr && m_bInited) ? S_OK : E_FAIL;
}

#endif //#ifdef ENABLE_LOG_PROXY

HRESULT CLogBase::UninitLog()
{
	AUTOLOCK(m_lock);
	CMessageThread::DestroyThread();
	m_bInited = FALSE;
	return S_OK;
}

void CLogBase::WriteLogW(LPCWSTR lpwsLog)
{
	if(NULL == lpwsLog)
		return ;

	LPCWSTR psLog = HELPER::StringClone(lpwsLog);
	if(!CMessageThread::PostThreadMsg(TMSG_WRITELOG, 1, (LPARAM)psLog))
		delete [] psLog;
}

void CLogBase::WriteLogA(LPCSTR lpsLog)
{
	if(NULL == lpsLog)
		return ;

	LPCSTR psLog = HELPER::StringClone(lpsLog);
	if(!CMessageThread::PostThreadMsg(TMSG_WRITELOG, 0, (LPARAM)psLog))
		delete [] psLog;
}

void CLogBase::WriteLogExW(LPCWSTR lpwsFormat, ...)
{
	if(NULL == lpwsFormat)
	{
		ATLASSERT(FALSE);
		return ;
	}

	WCHAR* pwsBuffer = new WCHAR[DEF_LOGBUFFER_SIZE];
	memset(pwsBuffer,0, sizeof(WCHAR)*DEF_LOGBUFFER_SIZE);

	va_list argptr;
	va_start( argptr, lpwsFormat);
	_vsnwprintf_s(pwsBuffer, DEF_LOGBUFFER_SIZE, _TRUNCATE, lpwsFormat, argptr);
	va_end(argptr);

	if(!CMessageThread::PostThreadMsg(TMSG_WRITELOG, 1, (LPARAM)pwsBuffer))
		delete [] pwsBuffer;
}

void CLogBase::WriteLogExA(LPCSTR lpsFormat, ...)
{
	if(NULL == lpsFormat)
	{
		ATLASSERT(FALSE);
		return ;
	}

	CHAR* psBuffer = new CHAR[DEF_LOGBUFFER_SIZE];
	memset(psBuffer,0, sizeof(CHAR)*DEF_LOGBUFFER_SIZE);

	va_list argptr;
	va_start( argptr, lpsFormat);
	_vsnprintf_s(psBuffer, DEF_LOGBUFFER_SIZE, _TRUNCATE, lpsFormat, argptr);
	va_end(argptr);

	if(!CMessageThread::PostThreadMsg(TMSG_WRITELOG, 0, (LPARAM)psBuffer))
		delete [] psBuffer;
}

void CLogBase::BeforeRun(LPVOID lpvParam)
{
#ifdef ENABLE_LOG_PROXY
	m_bInited = TRUE;
#else //#ifdef ENABLE_LOG_PROXY
	if(m_hLogFile != INVALID_HANDLE_VALUE)
	{
		ATLASSERT(FALSE);
		CloseHandle(m_hLogFile);
	}
	HELPER::MakeDirExist(m_strLogFile, true);
	m_hLogFile = CreateFile(m_strLogFile, GENERIC_WRITE|GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(INVALID_HANDLE_VALUE != m_hLogFile)
	{
		DWORD dwSizeHigh = 0;
		DWORD dwSizeLow = GetFileSize(m_hLogFile, &dwSizeHigh);
		if(dwSizeLow == INVALID_FILE_SIZE || dwSizeHigh > 0 || dwSizeLow >= m_dwMaxLogFileSize*MB)
		{
			m_bInited = ResetLogSize(m_hLogFile);
			ATLASSERT(m_bInited);
		}
		else
		{
			m_bInited = TRUE;
		}
		SetFilePointer(m_hLogFile, 0, NULL, FILE_END);
	}
	else
	{
		m_bInited = FALSE;
		ATLASSERT(FALSE);
	}
#endif //#ifdef ENABLE_LOG_PROXY
	__super::BeforeRun(lpvParam);

#ifndef ENABLE_LOG_PROXY
	if(m_bInited)
	{
		m_timeridFlushFile = ::SetTimer(NULL, TIMERID_FLUSHFILEBUFFER, TIMERELAPSE_FLUSHFILEBUFFER, NULL);
		m_timeridHeartbeat = 0;// ::SetTimer(NULL, TIMERID_HEARTBEAT, TIMERELAPSE_HEARTBEAT, NULL);

		SafeWriteLogW(L"===========================LogPrintBegin============================\r\n");
		RecordBasicInfo();
	}
#endif//#ifndef ENABLE_LOG_PROXY
}

void CLogBase::AfterRun(LPVOID lpvParam)
{
#ifndef ENABLE_LOG_PROXY
	if(m_hLogFile != INVALID_HANDLE_VALUE)
	{
		SafeWriteLogW(L"===========================LogPrintEnd==============================\r\n");
		FlushFileBuffers(m_hLogFile);
		CloseHandle(m_hLogFile);
		m_hLogFile = INVALID_HANDLE_VALUE;
	}
#else
	m_spLog.Release();
#endif //#ifndef ENABLE_LOG_PROXY
	m_bInited = FALSE;
	__super::AfterRun(lpvParam);
#ifndef ENABLE_LOG_PROXY
	::KillTimer(NULL, m_timeridFlushFile);
	m_timeridFlushFile = 0;

	::KillTimer(NULL, m_timeridHeartbeat);
	m_timeridHeartbeat = 0;
#endif //#ifdef ENABLE_LOG_PROXY
}

LRESULT CLogBase::OnMessage(UINT uMsg, WPARAM wp, LPARAM lp, BOOL& bHandled)
{
	switch(uMsg)
	{
	case TMSG_WRITELOG:
		{
#ifndef ENABLE_LOG_PROXY
			m_dwLastLogStamp = GetTickCount();
#endif//#ifndef ENABLE_LOG_PROXY
			if(wp == 0)//Ansi
			{
				LPCSTR psBuffer = (LPCSTR)lp;
				if(psBuffer)
				{
					SafeWriteLogA(psBuffer);
					delete [] psBuffer;
				}
			}
			else //Unicode
			{
				LPCWSTR pwsBuffer = (LPCWSTR)lp;
				if(pwsBuffer)
				{
					SafeWriteLogW(pwsBuffer);
					delete [] pwsBuffer;
				}
			}
			break;
		}
#ifndef ENABLE_LOG_PROXY
	case WM_TIMER:
		{
			if(m_timeridFlushFile == wp)
			{
				if(m_hLogFile != INVALID_HANDLE_VALUE)
				{
					LONG lSizeHigh = 0;
					DWORD dwSizeLow = SetFilePointer(m_hLogFile, 0, &lSizeHigh, FILE_CURRENT);
					if(lSizeHigh > 0 || dwSizeLow >= m_dwMaxLogFileSize*MB)
					{
						BOOL bRtn = ResetLogSize(m_hLogFile);
						ATLASSERT(bRtn);
					}
					else
					{
						FlushFileBuffers(m_hLogFile);
						SetEndOfFile(m_hLogFile);
					}
				}
			}
			else if(m_timeridHeartbeat == wp)
			{
				DWORD dwTick = GetTickCount();
				if(dwTick > m_dwLastLogStamp && ((dwTick - m_dwLastLogStamp) >= TIMERELAPSE_HEARTBEAT))
				{
					LOGPRINTTIMESTAMP(L"LogBase::Heartbeat");
				}
			}
			break;
		}
#endif //#ifdef ENABLE_LOG_PROXY
	}
	return 0;
}

void CLogBase::RecordBasicInfo()
{
#ifndef ENABLE_LOG_PROXY
	this->WriteLogExW(L"PID:[%d] \r\nACVersion:[%s] \r\nCommandLine:[%s]\r\n", GetCurrentProcessId(), VERSION_WSTRING_BUILD, GetCommandLine());
#endif //#ifndef ENABLE_LOG_PROXY
}

void CLogBase::SafeWriteLogW(LPCWSTR lpwsLog)
{
	if(NULL == lpwsLog || 
#ifdef ENABLE_LOG_PROXY
		!m_spLog
#else //#ifdef ENABLE_LOG_PROXY
		INVALID_HANDLE_VALUE == m_hLogFile
#endif //#ifdef ENABLE_LOG_PROXY
		)
		return ;

#ifdef ENABLE_LOG_PROXY

	m_spLog->PrintLogW(lpwsLog, FALSE);

#else //#ifdef ENABLE_LOG_PROXY

	DWORD dwWrited = 0;
	BOOL bRtn = TRUE;
	DWORD dwCur = SetFilePointer(m_hLogFile, 0, NULL, FILE_END);

	DWORD dwLength = lstrlenW(lpwsLog);
	dwLength *= sizeof(WCHAR);

	HRESULT hr = _WriteData((LPBYTE)lpwsLog, dwLength, m_hLogFile);
	ATLASSERT(SUCCEEDED(hr));

#endif //#ifdef ENABLE_LOG_PROXY
}

void CLogBase::SafeWriteLogA(LPCSTR lpsLog)
{
	if(NULL == lpsLog || 
#ifdef ENABLE_LOG_PROXY
		!m_spLog
#else //#ifdef ENABLE_LOG_PROXY
		NULL == m_hLogFile
#endif //#ifdef ENABLE_LOG_PROXY
		)
		return ;

#ifdef ENABLE_LOG_PROXY

	m_spLog->PrintLogA(lpsLog, FALSE);

#else //#ifdef ENABLE_LOG_PROXY

	int nALen = lstrlenA(lpsLog);
	int nLength = MultiByteToWideChar(CP_ACP, 0, lpsLog, nALen, NULL, 0);
	if(nLength > 0)
	{
		nLength = (((nLength >> 4) + 1) << 4);
		WCHAR* pwsBuf = new WCHAR[nLength];
		memset(pwsBuf, 0, nLength*sizeof(WCHAR));
		nLength = MultiByteToWideChar(CP_ACP, 0, lpsLog, nALen, pwsBuf, nLength);
	
		SafeWriteLogW(pwsBuf);

		delete [] pwsBuf;
	}
	ATLASSERT(nLength > 0);
#endif //#ifdef ENABLE_LOG_PROXY
}

BOOL CLogBase::ResetLogSize(HANDLE hFile, LONG lSize/* = 2*MB*/)
{
	if(INVALID_HANDLE_VALUE == hFile || lSize < 0)
		return FALSE;

	//定位到文件头
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	DWORD dwResetCount = 0;
	BOOL bRtn = FALSE;
	//跳过前40条记录（保留登录信息）
	LONG lDistance = 0;
	for(DWORD i = 0; i < 40; ++i)
	{
		DWORD dwReaded = 0;
		EncryptLogItem item = {0};
		bRtn = ReadFile(hFile, &item, sizeof(EncryptLogItem), &dwReaded, NULL);
		if(!bRtn)
		{
			ATLASSERT(FALSE);
			break;
		}
		lDistance += sizeof(EncryptLogItem) + item.dwLength;
		SetFilePointer(hFile, lDistance, NULL, FILE_BEGIN);
	}
	if(bRtn)
	{
		for(DWORD i = 0; i < 100; ++i)
		{
			DWORD dwReaded = 0;
			EncryptLogItem item = {0};
			bRtn = ReadFile(hFile, &item, sizeof(EncryptLogItem), &dwReaded, NULL);
			if(!bRtn)
			{
				ATLASSERT(FALSE);
				break;
			}
			if(item.dwEncryptType != EET_Reset)
			{
				SetFilePointer(hFile, -1*(LONG)sizeof(EncryptLogItem), NULL, FILE_CURRENT);
				break;
			}
			++dwResetCount;
			lDistance += sizeof(EncryptLogItem) + item.dwLength;
			SetFilePointer(hFile, lDistance, NULL, FILE_BEGIN);
		}
	}
	
	//删除中间的xxx M左右数据
	if(bRtn)
	{
		DWORD dwSize = GetFileSize(hFile, NULL);
		if (dwSize != INVALID_FILE_SIZE)
		{
			DWORD dwSizeDel = 0;
			do
			{
				DWORD dwReaded = 0;
				EncryptLogItem item = { 0 };
				bRtn = ReadFile(hFile, &item, sizeof(EncryptLogItem), &dwReaded, NULL);
				if (!bRtn)
				{
					ATLASSERT(FALSE);
					break;
				}
				if (item.dwHeaderFlag != HEADERFLAG_LOG)
				{
					ATLASSERT(FALSE);
					break;
				}
				dwSizeDel += dwReaded + item.dwLength;
				SetFilePointer(hFile, item.dwLength, NULL, FILE_CURRENT);

				if (dwSizeDel > dwSize)
				{
					bRtn = FALSE;
					ATLASSERT(FALSE);
					break;
				}
			} while (((LONG)(dwSize - dwSizeDel) - lDistance) > lSize);
		}
	}
	DWORD dwCurPos = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
	
	//移动后面的数据覆盖之前的数据
	if(bRtn)
	{
		DWORD dwFileSize = GetFileSize(hFile, NULL);
		if(dwFileSize != INVALID_FILE_SIZE && dwFileSize > dwCurPos)
		{
			DWORD dwReaded = 0;
			DWORD dwBufLen = dwFileSize - dwCurPos;
			LPBYTE pbyBuf = new BYTE[dwBufLen];
			bRtn = ReadFile(hFile, pbyBuf, dwBufLen, &dwReaded, NULL);
			ATLASSERT(bRtn);
			if(bRtn)
			{
				//添加重置日志
				if(bRtn)
				{
					CString strResetLog;
					strResetLog.Format(L"~~~~~~~~~~~~[%d]ResetLogSize~~~~~~~~~~~~~~~\r\n", ++dwResetCount);

					DWORD dwWrited = 0;
					BOOL bRtn = TRUE;
					DWORD dwCur = SetFilePointer(hFile, lDistance, NULL, FILE_BEGIN);

					DWORD dwLength = strResetLog.GetLength();
					dwLength *= sizeof(WCHAR);

					//HRESULT hr = _WriteData((LPBYTE)(LPCWSTR)strResetLog, dwLength, hFile);
					HRESULT hr = E_FAIL;
					do 
					{
						LPBYTE pbyBuff = NULL;
						DWORD dwSize = 0;
						hr = _Encrypt((LPBYTE)(LPCWSTR)strResetLog, dwLength, &pbyBuff, &dwSize);
						if(FAILED(hr))
							break;

						EncryptLogItem* pItem = (EncryptLogItem*)pbyBuff;
						pItem->dwEncryptType = EET_Reset;

						DWORD dwWrited = 0;
						BOOL bRtn = WriteFile(hFile, pbyBuff, dwSize, &dwWrited, NULL);
						delete [] pbyBuff;

						if(bRtn && dwWrited == dwSize)
						{
							hr = S_OK;
							break;
						}
					} while (FALSE);
					ATLASSERT(SUCCEEDED(hr));

					lDistance = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
				}

				DWORD dwWrited = 0;
				SetFilePointer(hFile, lDistance, NULL, FILE_BEGIN);
				bRtn = WriteFile(hFile, pbyBuf, dwBufLen, &dwWrited, NULL);
				ATLASSERT(bRtn);
				if(bRtn)
				{
					FlushFileBuffers(hFile);
					SetFileValidData(hFile, lDistance + dwWrited);
					SetEndOfFile(hFile);
				}
			}
		}
		else
		{
			ATLASSERT(FALSE);
			bRtn = FALSE;
		}
	}
	
	if(!bRtn)
	{
		//出现异常，重置整个文件
		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	}

	return bRtn;
}

HRESULT CLogBase::InitPriveteLog( LPCWSTR lpwsLogFile, DWORD dwMaxLogFileSize /*= MAX_SYSLOG_SIZE*/ )
{
	AUTOLOCK(m_lock);
	if(NULL == lpwsLogFile)
		return E_INVALIDARG;

	if(CMessageThread::IsRunning())
		CMessageThread::DestroyThread();

	m_strLogFile = lpwsLogFile;
	m_dwMaxLogFileSize = dwMaxLogFileSize;
	HRESULT hr = CMessageThread::StartThread(NULL, NULL);

	return (S_OK == hr ) ? S_OK : E_FAIL;
}

HRESULT CLogBase::UninitPriveteLog()
{
	AUTOLOCK(m_lock);
	CMessageThread::DestroyThread();
	return S_OK;
}

void CLogBase::WriteSysLog( LPCWSTR lpwFormat, ... )
{
// 	if (!CLogBase::IsEnableTrace())
// 		return;

	if (NULL == lpwFormat)
	{
		ATLASSERT(FALSE);
		return;
	}

	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);

	WCHAR* psBuffer = new WCHAR[DEF_LOGBUFFER_SIZE];
	memset(psBuffer, 0, sizeof(WCHAR)*DEF_LOGBUFFER_SIZE);

	va_list argptr;
	va_start(argptr, lpwFormat);
	_vsnwprintf_s(psBuffer, DEF_LOGBUFFER_SIZE, _TRUNCATE, lpwFormat, argptr);
	va_end(argptr);

	WriteLogExW(L"[%02d-%02d-%02d %02d:%02d:%02d.%03d][TRACE][TID:%#X]%s\r\n",
		st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond,
		st.wMilliseconds, GetCurrentThreadId(), (LPCWSTR)psBuffer);

	delete[] psBuffer;
}
