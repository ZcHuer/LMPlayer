#pragma once
#include "BaseThread.h"

#ifdef _ENABLE_LOG_PROXY_
#define ENABLE_LOG_PROXY
#endif //

#define KB							1024
#define MB							1024*KB
#define GB							1024*MB

#define MAX_LOGFILE_SIZE			10	//MB
#define DEF_LOGBUFFER_SIZE			5000

#define MAX_SYSLOG_SIZE				5


#define TMSG_WRITELOG				(WM_USER + 100)

#define TIMERID_FLUSHFILEBUFFER		0x1234
#define TIMERELAPSE_FLUSHFILEBUFFER 2000

#define TIMERID_HEARTBEAT			0x1235
#define TIMERELAPSE_HEARTBEAT		90*1000

#define _ENCRYPT_LOG_
#ifdef _ENCRYPT_LOG_

#define HEADERFLAG_LOG				0xFFFE7700

//日志分级等级，数值越大表示越重要
//eg.  QG如果设计为3，大于等于3等级的日志都要记入文件
#define  LOG_LEVEL_IGNORE	       1
#define	 LOG_LEVEL_LOW		       2
#define  LOG_LEVEL_MEDIUM          3
#define  LOG_LEVEL_SERIOUS         4
#define  LOG_LEVEL_URGENT		   5
enum EEncryptType
{
	EET_XOR,
	EET_AES,
	EET_BASE64,

	EET_Reset,
};

struct EncryptLogItem
{
	DWORD dwHeaderFlag;				//HEADERFLAG_LOG
	DWORD dwVersion;				//Must 0
	DWORD dwEncryptType;			//EEncryptType
	DWORD dwEncryptKey;
	DWORD dwReserved1;
	DWORD dwReserved2;
	DWORD dwReserved3;
	DWORD dwLength;					//EncryptDataLength
};

#endif //#ifdef _ENCRYPT_LOG_

class CLogBase : public CMessageThread
{
public:
	CLogBase(void);
	virtual ~CLogBase(void);

	static CLogBase* GetDefLogInstance()
	{
		static CLogBase s_log;
		return &s_log;
	}

	static BOOL IsEnableTrace()
	{
		static DWORD s_dwEnableTrace = 0xFFFFFFFF;
		if (s_dwEnableTrace == 0xFFFFFFFF)
		{
			WCHAR wszTraceConfig[MAX_PATH] = {0};
			SHGetSpecialFolderPath(NULL, wszTraceConfig, CSIDL_APPDATA, FALSE);
			PathAppend(wszTraceConfig, L"Lenovo\\lebo\\TestMode.ini");
			s_dwEnableTrace = ::GetPrivateProfileInt(L"TestMode", L"EnableTrace", 0, wszTraceConfig);
		}
		return 1 == s_dwEnableTrace;
	}

	static void _WriteLogW(LPCWSTR lpwsFormat, ...);
	static void _WriteLogA(LPCSTR lpsFormat, ...);
	static void _WriteLogWithTimeStampW(int level,LPCWSTR lpwsFormat, ...);
	static void _WriteLogWithTimeStampA(int level,LPCSTR lpsFormat, ...);
	static void _WriteTraceWithTimeStampW(LPCWSTR lpwsFormat, ...);
	static void _WriteTraceWithTimeStampA(LPCSTR lpsFormat, ...);

	void WriteSysLog(LPCWSTR lpwFormat, ...);
	
public:
	BOOL IsInited(){return m_bInited;}

	int  GetLogLevel(){return m_nLogLevel;}
#ifdef ENABLE_LOG_PROXY
	HRESULT InitLog(IServiceProvider* lpSP);
#else //#ifdef ENABLE_LOG_PROXY
	HRESULT InitLog(LPCWSTR lpwsLogFile, DWORD dwMaxLogFileSize = MAX_LOGFILE_SIZE);
#endif //#ifdef ENABLE_LOG_PROXY
	HRESULT UninitLog();

	HRESULT InitPriveteLog(LPCWSTR lpwsLogFile, DWORD dwMaxLogFileSize = MAX_SYSLOG_SIZE);
	HRESULT UninitPriveteLog();

	void WriteLogW(LPCWSTR lpwsLog);
	void WriteLogA(LPCSTR lpsLog);
	void WriteLogExW(LPCWSTR lpwsFormat, ...);
	void WriteLogExA(LPCSTR lpsFormat, ...);

public:
	virtual void BeforeRun(LPVOID lpvParam);
	virtual void AfterRun(LPVOID lpvParam);
	virtual LRESULT OnMessage(UINT uMsg, WPARAM wp, LPARAM lp, BOOL& bHandled);

private:
	void RecordBasicInfo();
	void SafeWriteLogW(LPCWSTR lpwsLog);
	void SafeWriteLogA(LPCSTR lpsLog);
	BOOL ResetLogSize(HANDLE hFile, LONG lSize = 2*MB);

private:
#ifdef _ENCRYPT_LOG_

	HRESULT _Encrypt(LPBYTE lpbyBuff, DWORD dwLength, LPBYTE* lppbyBuff, DWORD* lpdwSize)
	{
		if(NULL == lpbyBuff || dwLength == 0 || lppbyBuff == NULL || NULL == lpdwSize)
			return E_INVALIDARG;

		LPBYTE pbyNewBuf = new BYTE[sizeof(EncryptLogItem) + dwLength];
		EncryptLogItem* pInfo = (EncryptLogItem*)pbyNewBuf;
		pInfo->dwHeaderFlag = HEADERFLAG_LOG;
		pInfo->dwVersion = 0;
		pInfo->dwEncryptType = EET_XOR;
		pInfo->dwEncryptKey = GetTickCount();
		pInfo->dwLength = dwLength;

		LPBYTE pbyEncData = (LPBYTE)(pbyNewBuf + sizeof(EncryptLogItem));
		LPBYTE pbyDataSrc = lpbyBuff;
		LPBYTE pbyDataEnd = lpbyBuff + dwLength;
		do 
		{
			if(pbyDataSrc + sizeof(DWORD) < pbyDataEnd)
			{
				*(DWORD*)pbyEncData = *(DWORD*)pbyDataSrc ^ pInfo->dwEncryptKey;
				pbyEncData += sizeof(DWORD);
				pbyDataSrc += sizeof(DWORD);
			}
			else
			{
				for(; pbyDataSrc < pbyDataEnd; ++pbyDataSrc, ++pbyEncData)
				{
					*pbyEncData = *pbyDataSrc;
				}
				break;
			}
		} while (pbyDataSrc < pbyDataEnd);
		*lppbyBuff = pbyNewBuf;
		*lpdwSize = sizeof(EncryptLogItem) + dwLength;
		return S_OK;
	}

	HRESULT _WriteData(LPBYTE lpbyBuff, DWORD dwLength, HANDLE hFile)
	{
		if(INVALID_HANDLE_VALUE == hFile)
			return E_FAIL;

		LPBYTE pbyBuff = NULL;
		DWORD dwSize = 0;
		HRESULT hr = _Encrypt(lpbyBuff, dwLength, &pbyBuff, &dwSize);
		if(FAILED(hr))
			return hr;

		DWORD dwWrited = 0;
		BOOL bRtn = WriteFile(hFile, pbyBuff, dwSize, &dwWrited, NULL);
		delete [] pbyBuff;

		if(bRtn && dwWrited == dwSize)
			return S_OK;

		return E_FAIL;
	}

#else //#ifdef _ENCRYPT_LOG__LOG_

	HRESULT _WriteData(LPBYTE lpbyBuff, DWORD dwLength, HANDLE hFile)
	{
		if(INVALID_HANDLE_VALUE == hFile)
			return E_FAIL;

		DWORD dwWrited = 0;
		BOOL bRtn = WriteFile(hFile, pbyBuff, dwSize, &dwWrited, NULL);
		delete [] pbyBuff;

		if(bRtn && dwWrited == dwSize)
			return S_OK;

		return E_FAIL;
	}
#endif //#ifdef _ENCRYPT_LOG__LOG_

private:
	BOOL m_bInited;
	int  m_nLogLevel;
	static CLockCS m_lock;
	
	CString m_strLogFile;
	DWORD m_dwMaxLogFileSize;

#ifdef ENABLE_LOG_PROXY
	
	CComQIPtr<IServiceLog> m_spLog;

#else //#ifdef ENABLE_LOG_PROXY
	
	HANDLE m_hLogFile;
//	CString m_strLogFile;
//	DWORD m_dwMaxLogFileSize;
	DWORD_PTR m_timeridFlushFile;
	DWORD_PTR m_timeridHeartbeat;
	DWORD m_dwLastLogStamp;

#endif //#ifdef ENABLE_LOG_PROXY
};

#ifdef ENABLE_LOG_PROXY

#define LOGPRINT_INIT_PROXY(p)			CLogBase::GetDefLogInstance()->InitLog(p)
#define LOGPRINT_UNINIT_PROXY()			CLogBase::GetDefLogInstance()->UninitLog()

#else //#ifdef ENABLE_LOG_PROXY

#define LOGPRINT_INIT_FILE(f)			CLogBase::GetDefLogInstance()->InitLog(f)
#define LOGPRINT_INIT_FILE_WITHSIZE(f, s)		CLogBase::GetDefLogInstance()->InitLog(f, s)
#define LOGPRINT_UNINIT()				CLogBase::GetDefLogInstance()->UninitLog()

#endif //#ifdef ENABLE_LOG_PROXY

//#ifdef PRIVETE_LOG  //#ifdef PRIVETE_LOG
#define PRIVATE_LOG_INIT(f)		CLogBase::GetDefLogInstance()->InitPriveteLog(f)
#define PRIVATE_LOG_UNINIT()	CLogBase::GetDefLogInstance()->UninitPriveteLog()
//#endif	//#endif  def PRIVETE_LOG

#define LOGPRINTW			CLogBase::_WriteLogW
#define LOGPRINTA			CLogBase::_WriteLogA
#define LOGLEVELPRINTTIMESTAMPW(level,format,...) CLogBase::_WriteLogWithTimeStampW(level,format,__VA_ARGS__)
#define LOGLEVELPRINTTIMESTAMPA(level,format,...) CLogBase::_WriteLogWithTimeStampA(level,format,__VA_ARGS__)
#define LOGPRINTTIMESTAMPW(format,...)	CLogBase::_WriteLogWithTimeStampW(LOG_LEVEL_MEDIUM,format,__VA_ARGS__)
#define LOGPRINTTIMESTAMPA(format,...)	CLogBase::_WriteLogWithTimeStampA(LOG_LEVEL_MEDIUM,format,__VA_ARGS__)

#define LOGTRACETIMESTAMPW	CLogBase::_WriteTraceWithTimeStampW
#define LOGTRACETIMESTAMPA	CLogBase::_WriteTraceWithTimeStampA

#ifdef _UNICODE
#define LOGPRINT			LOGPRINTW
#define LOGLEVELPRINTTIMESTAMP	LOGLEVELPRINTTIMESTAMPW
#define LOGPRINTTIMESTAMP	LOGPRINTTIMESTAMPW
#define LOGTRACETIMESTAMP	LOGTRACETIMESTAMPW	
#else //#ifdef _UNICODE
#define LOGPRINT			LOGPRINTA
#define LOGLEVELPRINTTIMESTAMP	LOGLEVELPRINTTIMESTAMPA
#define LOGPRINTTIMESTAMP	LOGPRINTTIMESTAMPA
#define LOGTRACETIMESTAMP	LOGTRACETIMESTAMPA
#endif //#ifdef _UNICODE