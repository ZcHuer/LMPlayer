#pragma once
#include <vector>
#include <strsafe.h>

#ifndef __ASSERT
#define __ASSERT	
#endif //#ifndef __ASSERT

//#if _MSC_VER <= 1900
//#define final 
//#endif//#if _MSC_VER <= 1900
//=================================================================================================
//lock

class ILock
{
public:
	ILock(){};
	virtual ~ILock(){};

public:
	virtual bool Lock() = 0;
	virtual bool Unlock() = 0;
};

class CLockNo : public ILock
{
public:
	CLockNo(){}
	virtual ~CLockNo(){}

public://ILock
	virtual bool Lock()
	{
		return true;
	}
	virtual bool Unlock()
	{
		return true;
	}
};

class CLockCS : public ILock
{
public:
	CLockCS(){InitializeCriticalSection(&m_cs);}
	virtual ~CLockCS(){DeleteCriticalSection(&m_cs);}

public://ILock
	virtual bool Lock()
	{
		EnterCriticalSection(&m_cs);
		return true;
	}
	virtual bool Unlock()
	{
		LeaveCriticalSection(&m_cs);
		return true;
	}
	
private:
	CRITICAL_SECTION m_cs;
};

class CAutoLock
{
public:
	CAutoLock(ILock* lpLock) : m_pLock(lpLock)
	{
		if(m_pLock)
			m_pLock->Lock();
	}

	CAutoLock(ILock& refLock) : m_pLock(&refLock)
	{
		if(m_pLock)
			m_pLock->Lock();
	}

	~CAutoLock()
	{
		if(m_pLock)
			m_pLock->Unlock();
	}

private:
	ILock* m_pLock;
};

#define AUTOLOCK(l)	CAutoLock _lock(&l) 

//=================================================================================================
//string

template<class T>
T* StringCopy(const T* lpsSrc, T* lpsDest = NULL, int nDestSize = 0)
{
	if(NULL == lpsSrc || (NULL != lpsDest && nDestSize <= 0))
	{
		__ASSERT(FALSE);
		return NULL;
	}

	int nSrcLength = 0;
	if(sizeof(T) == sizeof(char))
		nSrcLength = lstrlenA((LPCSTR)lpsSrc);
	else if(sizeof(T) == sizeof(wchar_t))
		nSrcLength = lstrlenW((LPCWSTR)lpsSrc);
	else
	{
		__ASSERT(FALSE);
		return NULL;
	}

	T* psDestInner = lpsDest;
	if(NULL == psDestInner)
	{
		psDestInner = new T[nSrcLength+1];
		memcpy(psDestInner, lpsSrc, (nSrcLength+1)*sizeof(T));//带结尾符号
	}
	else
	{	
		memcpy(psDestInner, lpsSrc, nDestSize*sizeof(T));
		psDestInner[max(0, nDestSize-1)] = (T)0;
	}

	return psDestInner;
}

//=================================================================================================

template<class T>
struct EventHandleInfo
{
	T* handle;
	bool bAuto;
	CString sName;

	EventHandleInfo()
	{
		handle = NULL;
		bAuto = true;
	}

	EventHandleInfo(T* h, bool b, LPCWSTR s)
	{
		handle = h;
		bAuto = b;
		sName = s;
	}
};

//=================================================================================================
#ifndef _RESERVE_EVENTHANDLER_COUNT_
#define _RESERVE_EVENTHANDLER_COUNT_			16
#endif //#ifndef _RESERVE_EVENTHANDLER_COUNT_

template<class T, class L = CLockCS>
class CEventHandleProxyT : public T
{
public:
	CEventHandleProxyT()
	{
		m_vecEventHandles.reserve(_RESERVE_EVENTHANDLER_COUNT_);
	}
	virtual ~CEventHandleProxyT()
	{
		ATLASSERT(m_vecEventHandles.size() == 0);
	}

protected:
	template<class T, class L>
	class CAutoLock4Proxy
	{
	public:
		CAutoLock4Proxy(CEventHandleProxyT<T,L>* lpThis) : m_pThis(lpThis)
		{
			if(m_pThis)
				m_pThis->LockEventHandler();
		}
		~CAutoLock4Proxy()
		{
			if(m_pThis)
				m_pThis->UnlockEventHandler();
		}
	private:
		CEventHandleProxyT<T,L>* m_pThis;
	};
	typedef CAutoLock4Proxy<T, L> _AutoLock;

#define EVENTHANDLE_FORLOOP_BEG(p)	for(_EventHandleList::iterator it = m_vecEventHandles.begin(); it != m_vecEventHandles.end(); ++it) { _Ptr (p) = it->handle; if(p){
#define EVENTHANDLE_FORLOOP_END()	/*if(p){*/} /*for(;;){*/}
#define EVENTHANDLE_AUTOLOCK() 	_AutoLock _lock(this)

public:
	void AddEventHandle(const EventHandleInfo<T> &info)
	{
		if(NULL == info.handle)
			return ;

		EVENTHANDLE_AUTOLOCK();

		for(size_t i = 0; i < m_vecEventHandles.size(); ++i)
		{
			if(m_vecEventHandles[i].handle == info.handle)
			{
				return ;
			}
		}

		OnAddEventHandler(info);
		m_vecEventHandles.push_back(info);
	}

	void DelEventHandle(T* lpHandle)
	{
		if(NULL == lpHandle)
			return ;

		EVENTHANDLE_AUTOLOCK();
		for(_EventHandleList::iterator it = m_vecEventHandles.begin(); it != m_vecEventHandles.end(); ++it)
		{
			if(IsEqualHandler(it->handle, lpHandle))
			{
				OnDelEventHandler(*it);
				m_vecEventHandles.erase(it);
				return ;
			}
		}
	}

	void DeleteAllEventHandler()
	{
		EVENTHANDLE_AUTOLOCK();

		for(size_t i = 0; i < m_vecEventHandles.size(); ++i)
		{
			OnDelEventHandler(m_vecEventHandles[i]);
		}
		m_vecEventHandles.clear();
	}

protected:
	virtual bool IsEqualHandler(T* handler1, T* handler2){return handler1 == handler2;}
	virtual void OnAddEventHandler(const EventHandleInfo<T> &info){}
	virtual void OnDelEventHandler(const EventHandleInfo<T> &info){}

	void LockEventHandler(){m_lock.Lock();}
	void UnlockEventHandler(){m_lock.Unlock();}

protected:
	typedef T* _Ptr;
	typedef std::vector<EventHandleInfo<T>> _EventHandleList;
	_EventHandleList m_vecEventHandles;
	L m_lock;
};

#ifndef _O_TEXT
#define _O_TEXT         0x4000  /* file mode is text (translated) */
#endif //#ifndef _O_TEXT

class CStdoutRedirectHelper
{
public:
	CStdoutRedirectHelper(LPCWSTR lpwsTitle) : m_bAllocCtrl(FALSE)
	{
		m_bAllocCtrl = AllocConsole();
		if(m_bAllocCtrl)
		{
			#pragma warning(push)
			#pragma warning(disable:4996)
			freopen("CONOUT$","w",stdout);
			#pragma warning(pop)

			if(lpwsTitle)
				SetConsoleTitle(lpwsTitle);

#define _CONSOLE_WNDSIZE_WIDTH		880
#define _CONSOLE_WNDSIZE_HEIGHT		660
/*
			HANDLE hStdIn  = GetStdHandle(STD_INPUT_HANDLE);
			HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
			HANDLE hStdErr = GetStdHandle(STD_ERROR_HANDLE);

			COORD cd = {120,1024};
			SetConsoleScreenBufferSize(hStdIn, cd);
			SetConsoleScreenBufferSize(hStdOut, cd);
			SetConsoleScreenBufferSize(hStdErr, cd);
			SMALL_RECT rc = {0};
			RECT rcDesk = {0};
			::GetWindowRect(GetDesktopWindow(), &rcDesk);
			rc.Left = rcDesk.left + (rcDesk.right - rcDesk.left -_CONSOLE_WNDSIZE_WIDTH)/2;
			rc.Top = rcDesk.top + (rcDesk.bottom - rcDesk.top -_CONSOLE_WNDSIZE_HEIGHT)/2;
			rc.Bottom = rc.Top + _CONSOLE_WNDSIZE_HEIGHT;
			rc.Right = rc.Left + _CONSOLE_WNDSIZE_WIDTH;
			SetConsoleWindowInfo(hStdIn, FALSE, &rc);
			SetConsoleWindowInfo(hStdOut, FALSE, &rc);
			SetConsoleWindowInfo(hStdErr, FALSE, &rc);
*/		}
	}

	~CStdoutRedirectHelper()
	{
		if(m_bAllocCtrl)
			FreeConsole();
	}

private:
	BOOL m_bAllocCtrl;
};

#define CONSOLE_DEBUG		wprintf

#define DEF_WAITTIMEOUT		2000

template<class T>
class CReturnEvent
{
public:
	CReturnEvent(LPCWSTR lpwsEventNamePrefix) : m_hEvent(NULL), m_pValue(NULL)
	{
		if(lpwsEventNamePrefix)
			m_strEventName.Format(L"%s_ReturnEvent_%d", lpwsEventNamePrefix, GetTickCount());
		else
			m_strEventName.Format(L"DefaultReturnEvent_%d", GetTickCount());

		m_hEvent = CreateEvent(NULL, TRUE, FALSE, m_strEventName);
	}

	CReturnEvent(T* lpValue, LPCWSTR lpwsEventName) : m_hEvent(NULL), m_strEventName(lpwsEventName), m_pValue(lpValue)
	{

	}

	virtual ~CReturnEvent()
	{
		if(m_hEvent)
			CloseHandle(m_hEvent);
	}

public:
	LPCWSTR GetEventName()
	{
		return m_strEventName;
	}

	bool Wait(DWORD dwTimeout = DEF_WAITTIMEOUT)
	{
		if(NULL == m_hEvent)
			return false;

		HANDLE hEventSetResult = CreateEvent(NULL, TRUE, FALSE, m_strEventName + L"_SetResult");
		if(NULL == hEventSetResult)
			return false;

		bool bSuccess = false;
		if(WAIT_OBJECT_0 == WaitForSingleObject(m_hEvent, dwTimeout))
		{
			Sleep(2);//保证事件可用
			if(WAIT_OBJECT_0 == WaitForSingleObject(hEventSetResult, 200))
			{
				bSuccess = true;
			}
			else
			{
				bSuccess = false;
			}
		}

		CloseHandle(hEventSetResult);
		return bSuccess;
	}

	bool BeforeSetReturnValue()
	{
		bool bRtn = false;
		m_hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, m_strEventName);
		if(m_hEvent)
		{
			SetEvent(m_hEvent);
			CloseHandle(m_hEvent);
			Sleep(2);//保证事件可用
			m_hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, m_strEventName + L"_SetResult");
			if(m_hEvent)
				bRtn = true;
		}
		return bRtn;
	}

	bool AfterSetReturnValue()
	{
		if(NULL == m_hEvent)
		{
			return false;
		}

		SetEvent(m_hEvent);
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
		return true;
	}

	bool SetReturnValue(T t)
	{
		bool bRtn = false;
		if(BeforeSetReturnValue())
		{
			bRtn = SetReturnValueDirect(t);

			AfterSetReturnValue();
		}
		return bRtn;
	}

	bool SetReturnValueDirect(T t)
	{
		if(m_pValue)
		{
			*m_pValue = t;
			return true;
		}
		return false;
	}

private:
	CString m_strEventName;
	HANDLE m_hEvent;
	T* m_pValue;
};

class HELPER
{
public:
	static BOOL MakeDirExist(LPCWSTR lpwsDir, bool bRemoveFile = false)
	{
		if(NULL == lpwsDir)
			return FALSE;
		
		int nLen = lstrlen(lpwsDir);
		if(nLen <= 0)
			return FALSE;

		int nLen2 = (((nLen >> 2) + 1) << 2) + 4;

		LPWSTR pwsBuf = new WCHAR[nLen2];
		StringCchCopy(pwsBuf, nLen2, lpwsDir);
		memset(&pwsBuf[nLen], 0, (nLen2-nLen)*sizeof(WCHAR));

		if(bRemoveFile)
		{
			PathRemoveFileSpec(pwsBuf);
			BOOL bRtn = MakeDirExist(pwsBuf);
			delete [] pwsBuf;
			return bRtn;
		}

		BOOL bRtn = PathFileExists(pwsBuf);
		if(!bRtn)
		{
			PathRemoveFileSpec(pwsBuf);
			if(HELPER::MakeDirExist(pwsBuf))
			{
				bRtn = CreateDirectory(lpwsDir, NULL);
			}
		}
		delete [] pwsBuf;
		return bRtn;
	}
	static BOOL MakeDirExist(LPCSTR lpwsDir, bool bRemoveFile = false)
	{
		if(NULL == lpwsDir)
			return FALSE;

		int nLen = lstrlenA(lpwsDir);
		if(nLen <= 0)
			return FALSE;

		int nLen2 = (((nLen >> 2) + 1) << 2) + 4;

		LPSTR pwsBuf = new CHAR[nLen2];
		StringCchCopyA(pwsBuf, nLen2, lpwsDir);
		memset(&pwsBuf[nLen], 0, (nLen2-nLen)*sizeof(CHAR));

		if(bRemoveFile)
		{
			PathRemoveFileSpecA(pwsBuf);
			BOOL bRtn = MakeDirExist(pwsBuf);
			delete [] pwsBuf;
			return bRtn;
		}

		BOOL bRtn = PathFileExistsA(pwsBuf);
		if(!bRtn)
		{
			PathRemoveFileSpecA(pwsBuf);
			if(HELPER::MakeDirExist(pwsBuf))
			{
				bRtn = CreateDirectoryA(lpwsDir, NULL);
			}
		}
		delete [] pwsBuf;
		return bRtn;
	}

	static LPCWSTR StringClone(LPCWSTR lpstr)
	{
		if(NULL == lpstr)
			return NULL;

		int nLen = lstrlenW((LPCWSTR)lpstr);
		LPWSTR pstrNew = new WCHAR[nLen + 16];
		lstrcpynW((LPWSTR)pstrNew, (LPCWSTR)lpstr, nLen+8);//此函数会自动补结尾0

		return pstrNew;	
	}

	static LPCSTR StringClone(LPCSTR lpstr)
	{
		if(NULL == lpstr)
			return NULL;

		int nLen = lstrlenA((LPCSTR)lpstr);
		LPSTR pstrNew = new char[nLen + 16];
		lstrcpynA((LPSTR)pstrNew, (LPCSTR)lpstr, nLen+8);//此函数会自动补结尾0

		return pstrNew;	
	}
};

template <class T, class I>
BOOL CreateObject(I** lppI, T** lppT = NULL)
{
	CComObject<T>* pObj = NULL;
	HRESULT hr = CComObject<T>::CreateInstance(&pObj);
	if(FAILED(hr))
		return FALSE;

	pObj->AddRef();

	hr = pObj->QueryInterface(lppI);

	pObj->Release();

	if(FAILED(hr))
		return FALSE;

	if(lppT)
		*lppT = dynamic_cast<T*>(pObj);

	return TRUE;
}

//=================================================================================================
class CUTFString
{
public:
	CUTFString(LPCSTR lpsUTF8) : m_strUTF8(lpsUTF8?lpsUTF8:"")
	{
		int nLength = MultiByteToWideChar(CP_UTF8, 0, m_strUTF8.c_str(), m_strUTF8.length(), NULL, 0);
		if(nLength > 0)
		{
			nLength = (((nLength >> 4) + 1) << 4);
			WCHAR* pwsBuf = new WCHAR[nLength];
			memset(pwsBuf, 0, nLength*sizeof(WCHAR));
			nLength = MultiByteToWideChar(CP_UTF8, 0, m_strUTF8.c_str(), m_strUTF8.length(), pwsBuf, nLength);
			m_strUTF16 = pwsBuf;
			delete [] pwsBuf;
		}
	}
	CUTFString(LPCWSTR lpwsUTF16) : m_strUTF16(lpwsUTF16?lpwsUTF16:L"")
	{
		
	}
	CUTFString(const CUTFString& ref)
	{
		this->m_strUTF8 = ref.m_strUTF8;
		this->m_strUTF16 = ref.m_strUTF16;
		this->m_strANSI = ref.m_strANSI;
	}
	CUTFString& operator=(const CUTFString& ref)
	{
		if(this != &ref)
		{
			this->m_strUTF8 = ref.m_strUTF8;
			this->m_strUTF16 = ref.m_strUTF16;
			this->m_strANSI = ref.m_strANSI;
		}
		return *this;
	}
	CUTFString& operator=(LPCSTR lpwsUTF8)
	{
		Clear();

		m_strUTF8 = lpwsUTF8 ? lpwsUTF8 : "";

		int nLength = MultiByteToWideChar(CP_UTF8, 0, m_strUTF8.c_str(), m_strUTF8.length(), NULL, 0);
		if (nLength > 0)
		{
			nLength = (((nLength >> 4) + 1) << 4);
			WCHAR* pwsBuf = new WCHAR[nLength];
			memset(pwsBuf, 0, nLength * sizeof(WCHAR));
			nLength = MultiByteToWideChar(CP_UTF8, 0, m_strUTF8.c_str(), m_strUTF8.length(), pwsBuf, nLength);
			m_strUTF16 = pwsBuf;
			delete[] pwsBuf;
		}
		return *this;
	}
	CUTFString& operator=(LPCWSTR lpwsUTF16)
	{
		Clear();
		m_strUTF16 = lpwsUTF16 ? lpwsUTF16 : L"";
		return *this;
	}
	operator LPCWSTR()
	{
		return UTF16();
	}
	LPCSTR ANSI()
	{
		if (m_strANSI.empty())
		{
			int nLength = WideCharToMultiByte(CP_ACP, 0, m_strUTF16.c_str(), m_strUTF16.length(), NULL, 0, NULL, NULL);
			if (nLength > 0)
			{
				nLength = (((nLength >> 4) + 1) << 4);
				char* psBuf = new char[nLength];
				memset(psBuf, 0, nLength);
				nLength = WideCharToMultiByte(CP_ACP, 0, m_strUTF16.c_str(), m_strUTF16.length(), psBuf, nLength, NULL, NULL);
				m_strANSI = psBuf;
				delete[] psBuf;
			}
		}
		return m_strANSI.c_str();
	}
	LPCSTR UTF8()
	{
		if (m_strUTF8.empty())
		{
			int nLength = WideCharToMultiByte(CP_UTF8, 0, m_strUTF16.c_str(), m_strUTF16.length(), NULL, 0, NULL, NULL);
			if (nLength > 0)
			{
				nLength = (((nLength >> 4) + 1) << 4);
				char* psBuf = new char[nLength];
				memset(psBuf, 0, nLength);
				nLength = WideCharToMultiByte(CP_UTF8, 0, m_strUTF16.c_str(), m_strUTF16.length(), psBuf, nLength, NULL, NULL);
				m_strUTF8 = psBuf;
				delete[] psBuf;
			}
		}
		return m_strUTF8.c_str();
	}
	LPCWSTR UTF16()
	{
		return m_strUTF16.c_str();
	}
	LONG LengthUTF8()
	{
		if (m_strUTF8.empty())
			UTF8();

		return m_strUTF8.length();
	}

	LONG LengthUTF16()
	{
		return m_strUTF16.length();
	}

	LONG LengthANSI()
	{
		if (m_strANSI.empty())
			ANSI();

		return m_strANSI.length();
	}
private:
	void Clear()
	{
		m_strANSI.clear();
		m_strUTF16.clear();
		m_strUTF8.clear();
	}
private:
	std::wstring m_strUTF16;
	std::string  m_strUTF8;
	std::string  m_strANSI;
};

//=================================================================================================
template<class T, class I>
class IUnknownSimpleImpl : public T
{
public://IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject)
	{
		if(NULL == ppvObject)
			return E_INVALIDARG;

		if(riid == __uuidof(IUnknown))
		{
			*ppvObject = (void*)(dynamic_cast<IUnknown*>(dynamic_cast<I*>(this)));
			if(*ppvObject)
			{
				AddRef();
				return S_OK;
			}
		}
		else if(riid == __uuidof(I))
		{
			*ppvObject = (void*)dynamic_cast<I*>(this);
			if(*ppvObject)
			{
				AddRef();
				return S_OK;
			}
		}
		return E_NOINTERFACE;
	}
	virtual ULONG STDMETHODCALLTYPE AddRef( void)
	{
		return 1;
	}
	virtual ULONG STDMETHODCALLTYPE Release( void)
	{
		return 1;
	}
};


template<class T, class I>
class IUnknownImpl : public T
{
public:
	IUnknownImpl() : m_lRefCount(0) {}
	virtual ~IUnknownImpl() { ATLASSERT(m_lRefCount == 0);}

public:
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject)
	{
		if(IsEqualIID(riid, __uuidof(IUnknown)))
		{
			AddRef();
			*ppvObject = static_cast<IUnknown*>(this);
			return S_OK;
		}
		else if(IsEqualIID(riid, __uuidof(I)))
		{
			AddRef();
			*ppvObject = static_cast<I*>(this);
			return S_OK;
		}
		return E_NOINTERFACE;
	}
	virtual ULONG STDMETHODCALLTYPE AddRef( void)
	{
		return InterlockedIncrement(&m_lRefCount);
	}
	virtual ULONG STDMETHODCALLTYPE Release( void)
	{
		LONG lRef = InterlockedDecrement(&m_lRefCount);
		if(lRef == 0)
		{
			delete this;
		}
		ATLASSERT(lRef >= 0);
		return lRef;
	}

private:
	LONG m_lRefCount;
};

template<class T, class I, class I2>
class IUnknownImpl2 : public IUnknownImpl<T, I>
{
public:
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject)
	{
		if(IsEqualIID(riid, __uuidof(I2)))
		{
			AddRef();
			*ppvObject = static_cast<I2*>(this);
			return S_OK;
		}
		return IUnknownImpl<T, I>::QueryInterface(riid, ppvObject);
	}
};


class CRefEvent
{
public:
	CRefEvent() : m_lRefCount(0), m_hEvent(NULL)
	{
		Create();
		ATLASSERT(m_hEvent);
	}

	~CRefEvent()
	{
		ATLASSERT(0 >= m_lRefCount);
		CloseHandle(m_hEvent);
	}

public:
	BOOL Create()
	{
		if(m_hEvent)
			return TRUE;
		ATLASSERT(0 == m_lRefCount);
		m_hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		return NULL != m_hEvent;
	}

	BOOL Destroy()
	{
		if(NULL == m_hEvent)
			return TRUE;
		ATLASSERT(0 == m_lRefCount);
		BOOL bRtn = ::CloseHandle(m_hEvent);
		m_hEvent = NULL;
		return bRtn;
	}

	BOOL SetEvent()
	{
		BOOL bRtn = FALSE;
		if(InterlockedIncrement(&m_lRefCount) > 0)
		{
			bRtn = ::SetEvent(m_hEvent);
		}
		return bRtn;
	}

	BOOL ResetEvent()
	{
		BOOL bRtn = FALSE;
		if(InterlockedDecrement(&m_lRefCount) <= 0)
		{
			bRtn = ::ResetEvent(m_hEvent);
		}
		return bRtn;
	}
	DWORD WaitEvent(DWORD dwTimeout)
	{
		return ::WaitForSingleObject(m_hEvent, dwTimeout);
	}
	HANDLE GetEventHandle()
	{
		return m_hEvent;
	}

protected:
	volatile LONG m_lRefCount;
	HANDLE m_hEvent;
};

#ifndef _wtoui64
#define _wtoui64(s) _wcstoui64((s), NULL, 10)
#endif //#ifndef _wtoui64

#if _MSC_VER <= 1900
#define final
#endif//#if _MSC_VER <= 1900

class CDbgMemLeak final
{
	_CrtMemState m_checkpoint;

public:
	explicit CDbgMemLeak()
	{
		_CrtMemCheckpoint(&m_checkpoint);
	}

	~CDbgMemLeak()
	{
		_CrtMemState checkpoint; (checkpoint);
		_CrtMemCheckpoint(&checkpoint);
		_CrtMemState diff; (diff);
		_CrtMemDifference(&diff, &m_checkpoint, &checkpoint);
		_CrtMemDumpStatistics(&diff);
		_CrtMemDumpAllObjectsSince(&diff);
	};
};
