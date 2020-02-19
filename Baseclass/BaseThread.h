#pragma once
#include "set_thread_name.h"
#include <typeinfo>
#include <memory>

#ifndef PRINTDBG
#define PRINTDBG
#else
extern void PRINTDBG(LPCWSTR, ...);
#endif //#ifndef PRINTDBG

#ifndef __ASSERT
#define __ASSERT	
#endif //#ifndef __ASSERT

#define THREAD_EXITCODE_UNKNOWN				-1
#define THREAD_EXITCODE_EXCEPTION			-2
#define THREAD_EXITCODE_INVALIDARG			-3

typedef struct ThreadParam
{
	LPVOID lpvThis;
	LPVOID lpvParam;
	HANDLE hReadyEvent;
}* LPThreadParam;

class CBaseThread
{
public:
	CBaseThread(void) : m_hThread(NULL), m_hAbortEvent(NULL), m_uThreadID(0),m_dwStopTimeout(0)
	{

	}

	virtual ~CBaseThread(void)
	{
		if(m_hThread)
		{//CloseThread
			if (m_hAbortEvent)
				SetEvent(m_hAbortEvent);

			DWORD dwTimeout = m_dwStopTimeout >= 500 ? m_dwStopTimeout : INFINITE;
			DWORD dwRet = WaitForSingleObject(m_hThread, dwTimeout);
			if (WAIT_TIMEOUT == dwRet)
			{
				ATLASSERT(FALSE);
				::TerminateThread(m_hThread, WAIT_TIMEOUT);
			}
			else
			{
				CloseHandle(m_hThread);
			}
			m_hThread = NULL;
		}
		
		//not running
		if(m_hAbortEvent)
			CloseHandle(m_hAbortEvent);
		m_hAbortEvent = NULL;
	}

public:
	virtual HRESULT StartThread(LPCSTR lpsThreadName, LPVOID lpvParam)
	{
		if(IsRunning())
			return S_FALSE;

		if(m_hAbortEvent)
		{
			ResetEvent(m_hAbortEvent);
		}
		else
		{
			m_hAbortEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			__ASSERT(m_hAbortEvent);
		}

		HANDLE hReadyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		__ASSERT(hReadyEvent);

		HRESULT hr = E_FAIL;

		ThreadParam* ptp = new ThreadParam;
		ptp->lpvThis = this;
		ptp->lpvParam = lpvParam;
		ptp->hReadyEvent = hReadyEvent;
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, _Thread, ptp,  CREATE_SUSPENDED, &m_uThreadID);
		if(NULL != m_hThread)
		{			
			SetThreadName(GenerateThreadName(lpsThreadName), m_uThreadID);//Ignore MemLeak

			ResumeThread(m_hThread);

			if(WaitForSingleObject(hReadyEvent, INFINITE) == WAIT_OBJECT_0)
			{
				hr = S_OK;
			}
			else
			{
				CloseHandle(m_hThread);
				m_hThread = NULL;
			}
		}
		else
		{
			__ASSERT(FALSE);
			delete ptp;
		}

		CloseHandle(hReadyEvent);

		if(FAILED(hr))
		{
			CloseHandle(m_hAbortEvent);
			m_hAbortEvent = NULL;
		}	
		return hr;
	}

	virtual HRESULT DestroyThread()
	{
		if(!IsRunning())
			return S_FALSE;

		StopThread();

		if(GetCurrentThreadId() == m_uThreadID)
		{
			//Abort Self Call 
			return E_ABORT;
		}

		DWORD dwTimeout = m_dwStopTimeout >= 500 ? m_dwStopTimeout : INFINITE;

		DWORD dwRet = WaitForSingleObject(m_hThread, dwTimeout);
		if( WAIT_TIMEOUT == dwRet ) {
			ATLASSERT(FALSE);
			::TerminateThread(m_hThread, WAIT_TIMEOUT);
		}
		else {
			dwRet = S_OK;
			CloseHandle(m_hThread);
		}
		m_hThread = NULL;

		if(m_hAbortEvent)
			CloseHandle(m_hAbortEvent);
		m_hAbortEvent = NULL;

		return dwRet;
	}

	virtual HRESULT StopThread()
	{
		if(!IsRunning())
			return S_FALSE;

		SetThreadPriority(m_hThread, THREAD_PRIORITY_TIME_CRITICAL);

		__ASSERT(m_hAbortEvent);
		SetEvent(m_hAbortEvent);

		return S_OK;
	}

	BOOL IsRunning()
	{
		if(NULL == m_hThread)
			return FALSE;

		DWORD dwExitCode = 0;
		BOOL bRtn = ::GetExitCodeThread(m_hThread, &dwExitCode);
		if(!bRtn)
		{
			m_hThread = NULL;
			m_uThreadID = 0;
			return FALSE;
		}

		if(STILL_ACTIVE == dwExitCode)
			return TRUE;

		CloseHandle(m_hThread);
		m_hThread = NULL;
		m_uThreadID = 0;
		return FALSE;
	}

	DWORD Wait(HANDLE* lphzEvent, DWORD dwCount, DWORD dwTimeout = 50, bool bComplete = false)
	{
		DWORD dwNewCount = bComplete ? 2 : 1;
		if(NULL == lphzEvent)
			dwCount = 0;

		dwNewCount += dwCount;
		HANDLE* pWaitEventArray = new HANDLE[dwNewCount];

		pWaitEventArray[0] = m_hAbortEvent;
		for(DWORD i = 0; i < dwCount; ++i)
			pWaitEventArray[i+1] = lphzEvent[i];
		if(bComplete)
			pWaitEventArray[dwNewCount-1] = m_hThread;

		DWORD dwWaitObj = WaitForMultipleObjects(dwNewCount, pWaitEventArray, FALSE, dwTimeout);

		delete pWaitEventArray;
		return dwWaitObj;
	}

	UINT GetThreadID()
	{
		if(IsRunning())
			return m_uThreadID;
		return 0;
	}

	void SetStopTimeout( DWORD dwTimeout ) { m_dwStopTimeout = dwTimeout; }

protected:
	virtual LPCWSTR GetThreadName()
	{
		return L"BaseThread";
	}

	virtual BOOL IsAbort()
	{
		__ASSERT(m_hAbortEvent);
		DWORD dwWaitObj = WaitForSingleObject(m_hAbortEvent, 0);
		return (WAIT_OBJECT_0 == dwWaitObj);
	}

	virtual void BeforeRun(LPVOID lpvParam)
	{
#if _MSC_VER >= 1900 //VS2015
		PRINTDBG(L"BaseThread[%d, %s] [%s] BeforRun....", m_uThreadID, GetThreadName(), m_strThreadNameW.get());
#endif //#if _MSC_VER >= 1900 //VS2015
		::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	}

	virtual void AfterRun(LPVOID lpvParam)
	{
		CoUninitialize();
#if _MSC_VER >= 1900 //VS2015
		PRINTDBG(L"BaseThread[%d, %s] [%s] AfterRun....", m_uThreadID, GetThreadName(), m_strThreadNameW.get());
#endif //#if _MSC_VER >= 1900 //VS2015
	}

	virtual UINT Run(LPVOID lpvParam)
	{
		return 0;
	}
	
private:
	static UINT __stdcall _Thread(LPVOID lpvParam)
	{
		ThreadParam* pTP = (ThreadParam*)lpvParam;
		if(NULL == pTP)
		{
			__ASSERT(FALSE);
			return THREAD_EXITCODE_INVALIDARG;
		}

		CBaseThread* pThis = (CBaseThread*)pTP->lpvThis;
		if(NULL == pThis)
		{
			__ASSERT(FALSE);
			return THREAD_EXITCODE_INVALIDARG;
		}

		UINT uRtn = THREAD_EXITCODE_UNKNOWN;

		if(pTP->hReadyEvent)
			SetEvent(pTP->hReadyEvent);

		uRtn = pThis->ThreadProc(pTP->lpvParam);

		if(pTP)
			delete pTP;

		return uRtn;
	}

	UINT ThreadProc(LPVOID lpvParam)
	{
		UINT uRtn = 0;	
		
#if _MSC_VER >= 1900 //VS2015
		PRINTDBG(L"BaseThread[%d, %s] [%s] Running....", m_uThreadID, GetThreadName(), m_strThreadNameW.get());
#endif //#if _MSC_VER >= 1900 //VS2015
		BeforeRun(lpvParam);

		uRtn = Run(lpvParam);

		AfterRun(lpvParam);

#if _MSC_VER >= 1900 //VS2015
		PRINTDBG(L"BaseThread[%d, %s] [%s] Finished....", m_uThreadID, GetThreadName(), m_strThreadNameW.get());
#endif//#if _MSC_VER >= 1900 //VS2015
		return uRtn;
	}

#if _MSC_VER >= 1900 //VS2015
	LPSTR GenerateThreadName(LPCSTR lpsThreadName)
	{
		LPCSTR psClassName = StrStrIA((LPCSTR)typeid(*this).name(), "class ");
		if (psClassName)
			psClassName += 6/*Length("class ")*/;

		std::unique_ptr<CHAR> psModuleFileName(new CHAR[128]{});
		LPSTR psModuleName = psModuleFileName.get();
		{//ModuleName
			HMODULE hModule = NULL;
			if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)(DWORD_PTR)CBaseThread::_Thread, &hModule))
			{
				GetModuleFileNameA(hModule, psModuleFileName.get(), 124);
				psModuleName = PathFindFileNameA(psModuleFileName.get());
				PathRemoveExtensionA(psModuleName);
			}
		}

		if (!m_strThreadNameA)
			m_strThreadNameA.reset(new CHAR[128]{});
		StringCchPrintfA(m_strThreadNameA.get(), 127, "%s_%s_%s", psModuleName, psClassName, lpsThreadName?lpsThreadName:"");

		//Unicode16
		{
			if(!m_strThreadNameW)
				m_strThreadNameW.reset(new WCHAR[MAX_PATH]{});
			int nLen = lstrlenA(m_strThreadNameA.get());
			if (nLen > 0)
			{
				MultiByteToWideChar(CP_ACP, 0, m_strThreadNameA.get(), nLen, m_strThreadNameW.get(), MAX_PATH);
			}
		}
		return m_strThreadNameA.get();
	}
#else //#if _MSC_VER >= 1900 //VS2015
	LPSTR GenerateThreadName(LPCSTR lpsThreadName)
	{
		return "BaseThread";
	}
#endif //#if _MSC_VER >= 1900 //VS2015

protected:
	HANDLE m_hThread;
	HANDLE m_hAbortEvent;
	UINT m_uThreadID;

	DWORD m_dwStopTimeout;
#if _MSC_VER >= 1900 //VS2015
	std::unique_ptr<CHAR> m_strThreadNameA;
	std::unique_ptr<WCHAR> m_strThreadNameW;
#endif //#if _MSC_VER >= 1900 //VS2015
};

class CMessageThread : public CBaseThread
{
public:
	CMessageThread()
	{

	}
	virtual ~CMessageThread()
	{
	
	}

public:
	virtual LRESULT OnMessage(UINT uMsg, WPARAM wp, LPARAM lp, BOOL& bHandled)
	{
		if(uMsg == m_uSyncMsgCall)
		{
			TMsgParam* pParam = (TMsgParam*)wp;
			if(pParam)
			{
				pParam->lr = OnMessage(pParam->uMsg, pParam->wp, pParam->lp, pParam->bHandled);
				SetEvent(pParam->hEventComplete);
			}
		}
		return 0;
	}
	
	BOOL PostThreadMsg(UINT uMsg, WPARAM wp, LPARAM lp)
	{
		if(CBaseThread::IsRunning())
		{
			return ::PostThreadMessage(CBaseThread::GetThreadID(), uMsg, wp, lp);
		}
		return FALSE;
	}
	struct TMsgParam
	{
		HANDLE hEventComplete;
		UINT uMsg;
		WPARAM wp;
		LPARAM lp;
		LRESULT lr;
		BOOL bHandled;
	};
	BOOL SendThreadMsg(UINT uMsg, WPARAM wp, LPARAM lp, LRESULT* lpLR)
	{
		if(0 == m_uSyncMsgCall)
		{
			ATLASSERT(FALSE);
			return FALSE;
		}

		TMsgParam* pParam = new TMsgParam;
		pParam->hEventComplete = CreateEvent(NULL, TRUE, FALSE, NULL);
		pParam->uMsg = uMsg;
		pParam->wp = wp;
		pParam->lp = lp;
		pParam->lr = 0;

		if(!PostThreadMsg(m_uSyncMsgCall, (WPARAM)pParam, 0))
		{
			CloseHandle(pParam->hEventComplete);
			delete pParam;
			return FALSE;
		}

		BOOL bRtn = FALSE;
		DWORD dwWait = WaitForSingleObject(pParam->hEventComplete, INFINITE);
		if(dwWait == WAIT_OBJECT_0)
			bRtn = pParam->bHandled;
		else
		{
			ATLASSERT(FALSE);
			WaitForSingleObject(m_hThread, INFINITE);
		}
		if(bRtn && lpLR)
			*lpLR = pParam->lr;

		CloseHandle(pParam->hEventComplete);
		delete pParam;
		return bRtn;
	}

public://CBaseThread
	virtual LPCWSTR GetThreadName()
	{
		return L"MessageThread";
	}

	virtual HRESULT StartThread(LPCSTR lpsThreadName, LPVOID lpvParam)
	{
		if(CBaseThread::IsRunning())
			return S_FALSE;

		HANDLE* phEventReady = new HANDLE;
		*phEventReady = CreateEvent(NULL, TRUE, FALSE, NULL);
		HRESULT hr = CBaseThread::StartThread(lpsThreadName, phEventReady);
		if(S_OK == hr)
		{
			DWORD dwWait = CBaseThread::Wait(phEventReady, 1, INFINITE);
			if(dwWait != WAIT_OBJECT_0+1)
			{
				hr = E_FAIL;
			}
		}
		CloseHandle(*phEventReady);
		delete phEventReady;
		return hr;
	}

	virtual HRESULT StopThread()
	{
		::PostThreadMessage(CBaseThread::GetThreadID(), WM_QUIT, 0, 0);
		return __super::StopThread();
	}

	virtual void BeforeRun(LPVOID lpvParam)
	{
		__super::BeforeRun(lpvParam);
		MSG msg;
		PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		
		HANDLE* phEventReady = (HANDLE*)lpvParam;
		if(phEventReady && *phEventReady)
		{
			SetEvent(*phEventReady);
		}

		m_uSyncMsgCall = ::RegisterWindowMessage(L"CMessageThread_SYNC_MESSAGE_CALL");
	}

	virtual void AfterRun(LPVOID lpvParam)
	{
		__super::AfterRun(lpvParam);
	}

	virtual UINT Run(LPVOID lpvParam)
	{
		MSG msg;
		BOOL bRet = FALSE;
		while((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
		{
			if (bRet == -1)
			{
				// handle the error and possibly exit
			}
			else
			{
				BOOL bHandled = FALSE;
				LRESULT lr = OnMessage(msg.message, msg.wParam, msg.lParam, bHandled);
				if (!bHandled)
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}
		return 0;
	}
private:
	UINT m_uSyncMsgCall;
};


void UpdateMainThreadId();
bool IsInMainThread();
#define MUST_IN_MAIN_THREAD ATLASSERT(IsInMainThread())
