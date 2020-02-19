#pragma once

//#define _SRWLOCK_SURPPORT_XP_

#if _MSC_VER <= 1900 //VS2015
#ifndef final
#define final
#endif //#ifndef final
#endif //#if _MSC_VER <= 1900

#if _MSC_VER >= 1700 //VS2012
#define GetFuncFromDll(pfn, fn, dll) decltype(::fn)* pfn = AtlGetProcAddressFn(GetModuleHandleA(dll), fn)
#else //#if _MSC_VER >= 1700
#define GetFuncFromDll(pfn, fn, dll) T##fn pfn = (T##fn)GetProcAddress(GetModuleHandleA(dll), #fn);
#endif //#if _MSC_VER >= 1700

typedef VOID (WINAPI *TInitializeSRWLock)(PSRWLOCK SRWLock);
typedef VOID (WINAPI *TReleaseSRWLockExclusive)(PSRWLOCK SRWLock);
typedef VOID (WINAPI *TReleaseSRWLockShared)(PSRWLOCK SRWLock);
typedef VOID (WINAPI *TAcquireSRWLockExclusive)(PSRWLOCK SRWLock);
typedef VOID (WINAPI *TAcquireSRWLockShared)(PSRWLOCK SRWLock);
typedef BOOLEAN (WINAPI *TTryAcquireSRWLockShared)(PSRWLOCK SRWLock);
typedef BOOLEAN (WINAPI *TTryAcquireSRWLockExclusive)(PSRWLOCK SRWLock);

class CSRWLockXp
{
public:
	CSRWLockXp()
		: m_hEventRead(NULL)
		, m_hSemWrite(NULL)
		, m_hMutex(NULL)
		, m_dwReaderCount(0)
		, m_bTryAcquire(false)
	{

	}
	
	CSRWLockXp(const CSRWLockXp& ref) : m_bTryAcquire(false)
	{
		ATLASSERT(FALSE);
	}

	CSRWLockXp& operator=(const CSRWLockXp& ref)
	{
		ATLASSERT(FALSE);
		return *this;
	}
#if _MSC_VER >= 1900

	CSRWLockXp(CSRWLockXp&& ref) = delete;
	CSRWLockXp& operator=(CSRWLockXp&& ref) = delete;

#endif //#if _MSC_VER >= 1900

	~CSRWLockXp()
	{
		if(m_hMutex)
			CloseHandle(m_hMutex);

		if(m_hEventRead)
			CloseHandle(m_hEventRead);

		if(m_hSemWrite)
			CloseHandle(m_hSemWrite);
	}

public:
	bool InitializeSRWLock()
	{
		m_dwReaderCount = 0;
		m_hMutex = CreateMutex(NULL, FALSE, NULL);
		m_hSemWrite = CreateSemaphore(NULL, 1, 1, NULL);
		m_hEventRead = CreateEvent(NULL, TRUE, TRUE, NULL);

		return m_hMutex && m_hSemWrite && m_hEventRead;
	}

	bool AcquireSRWLockExclusive()
	{
		DWORD dwLock = WAIT_FAILED;
		do 
		{
			HANDLE hz[] = {m_hMutex, m_hSemWrite};
			dwLock = WaitForMultipleObjects(sizeof(hz)/sizeof(hz[0]), hz, TRUE, m_bTryAcquire ? 1000 : INFINITE);
			if(WAIT_FAILED == dwLock)
				break;

#ifdef _DEBUG_SRWLOCK
			if(dwLock == WAIT_TIMEOUT)
			{
				DWORD dwWait = WaitForSingleObject(m_hMutex, 0);
				if(dwWait == WAIT_OBJECT_0) ReleaseMutex(m_hMutex);
				dwWait = WaitForSingleObject(m_hEventRead, 0);
				dwWait = WaitForSingleObject(m_hSemWrite, 0);
				if(dwWait == WAIT_OBJECT_0)
					ReleaseSemaphore(m_hSemWrite, 1, NULL);
				dwWait = 0;
			}
#endif //#ifdef _DEBUG_SRWLOCK

		} while (m_bTryAcquire && WAIT_OBJECT_0 != dwLock);

		if(dwLock == WAIT_OBJECT_0)
		{
			ResetEvent(m_hEventRead);
			//Writing, DisableRead

			ATLVERIFY(ReleaseMutex(m_hMutex));
			return true;
		}
		else
		{
			ATLASSERT(FALSE);
		}
		return false;
	}

	bool ReleaseSRWLockExclusive()
	{
		DWORD dwLock = WaitForSingleObject(m_hMutex, INFINITE);
		if(dwLock == WAIT_OBJECT_0)
		{
			//Writed, EnableRead
			SetEvent(m_hEventRead);
			LONG lPre = 0;
			ATLVERIFY(ReleaseSemaphore(m_hSemWrite, 1, &lPre));

			ATLVERIFY(ReleaseMutex(m_hMutex));
			return true;
		}
		else
		{
			ATLASSERT(FALSE);
		}
		return false;
	}

	bool AcquireSRWLockShared()
	{
		DWORD dwLock = WAIT_FAILED;
		do 
		{
			HANDLE hz[] = {m_hMutex, m_hEventRead};
			dwLock = WaitForMultipleObjects(sizeof(hz)/sizeof(hz[0]), hz, TRUE, m_bTryAcquire ? 1000 : INFINITE);
			if(WAIT_FAILED == dwLock)
				break;
#ifdef _DEBUG_SRWLOCK
			if(dwLock == WAIT_TIMEOUT)
			{
				DWORD dwWait = WaitForSingleObject(m_hMutex, 0);
				if(dwWait == WAIT_OBJECT_0) ReleaseMutex(m_hMutex);
				dwWait = WaitForSingleObject(m_hEventRead, 0);
				dwWait = WaitForSingleObject(m_hSemWrite, 0);
				if(dwWait == WAIT_OBJECT_0)
					ReleaseSemaphore(m_hSemWrite, 1, NULL);
				dwWait = 0;
			}
#endif //#ifdef _DEBUG_SRWLOCK
		} while (m_bTryAcquire && WAIT_OBJECT_0 != dwLock);

		if(dwLock == WAIT_OBJECT_0)
		{
			if(m_dwReaderCount == 0)
			{
				HANDLE hz[] = {m_hEventRead, m_hSemWrite};
				DWORD dwWait = WaitForMultipleObjects(sizeof(hz)/sizeof(hz[0]), hz, TRUE, INFINITE);
				if(WAIT_OBJECT_0 != dwWait)
				{
					ATLASSERT(FALSE);
					ATLVERIFY(ReleaseMutex(m_hMutex));
					return false;
				}
			}
			++m_dwReaderCount;			
			//Reading, DisableWrite

			ATLVERIFY(ReleaseMutex(m_hMutex));
			return true;
		}
		else
		{
			ATLASSERT(FALSE);
		}
		return false;
	}

	bool ReleaseSRWLockShared()
	{
		DWORD dwLock = WaitForSingleObject(m_hMutex, INFINITE);
		if(dwLock == WAIT_OBJECT_0)
		{
			//Readed, EnableWrite()
			--m_dwReaderCount;
			if(m_dwReaderCount == 0)
			{
				LONG lPre = 0;
				ATLVERIFY(ReleaseSemaphore(m_hSemWrite, 1, &lPre));
			}

			ATLVERIFY(ReleaseMutex(m_hMutex));
			return true;
		}
		else
		{
			ATLASSERT(FALSE);
		}

		return false;
	}

	bool TryAcquireSRWLockExclusive()
	{
		DWORD dwLock = WAIT_FAILED;
		do 
		{
			HANDLE hz[] = {m_hMutex, m_hSemWrite};
			dwLock = WaitForMultipleObjects(sizeof(hz)/sizeof(hz[0]), hz, TRUE, 10);
			if(WAIT_FAILED == dwLock)
				break;

		} while (false);

		if(dwLock == WAIT_OBJECT_0)
		{
			ResetEvent(m_hEventRead);
			//Writing, DisableRead

			ATLVERIFY(ReleaseMutex(m_hMutex));
			return true;
		}
		return false;
	}

	bool TryAcquireSRWLockShared() 
	{
		return false;DWORD dwLock = WAIT_FAILED;
		do 
		{
			HANDLE hz[] = {m_hMutex, m_hEventRead};
			dwLock = WaitForMultipleObjects(sizeof(hz)/sizeof(hz[0]), hz, TRUE, 10);
			if(WAIT_FAILED == dwLock)
				break;
		} while (false);

		if(dwLock == WAIT_OBJECT_0)
		{
			if(m_dwReaderCount == 0)
			{
				HANDLE hz[] = {m_hEventRead, m_hSemWrite};
				DWORD dwWait = WaitForMultipleObjects(sizeof(hz)/sizeof(hz[0]), hz, TRUE, 10);
				if(WAIT_OBJECT_0 != dwWait)
				{
					ATLVERIFY(ReleaseMutex(m_hMutex));
					return false;
				}
			}
			++m_dwReaderCount;			
			//Reading, DisableWrite

			ATLVERIFY(ReleaseMutex(m_hMutex));
			return true;
		}
		return false;
	}

private:
	HANDLE m_hMutex;			//Atom Lock
	HANDLE m_hEventRead;
	HANDLE m_hSemWrite;
	DWORD m_dwReaderCount;
	const bool m_bTryAcquire;
};

//
class CSRWLock final
{
public:
	CSRWLock() 
	{
		m_SRWLock.Ptr = NULL;
#ifdef _SRWLOCK_SURPPORT_XP_
		GetFuncFromDll(pfnInitSRWLock, TryAcquireSRWLockShared, "Kernel32.dll");
		m_bSupportSRWLock = NULL != pfnInitSRWLock;
#endif //#ifdef _SRWLOCK_SURPPORT_XP_
	}

	CSRWLock(const CSRWLock& ref)
	{
		ATLASSERT(FALSE);
	}

	CSRWLock& operator=(const CSRWLock& ref)
	{
		ATLASSERT(FALSE);
		return *this;
	}
#if _MSC_VER >= 1900

	CSRWLock(CSRWLock&& ref) = delete;
	CSRWLock& operator=(CSRWLock&& ref) = delete;

#endif //#if _MSC_VER >= 1900

	~CSRWLock()
	{
#ifdef _SRWLOCK_SURPPORT_XP_

		if(!IsSupportSRWLock())
		{
			CSRWLockXp* pSRWLockXp = reinterpret_cast<CSRWLockXp*>(m_SRWLock.Ptr);
			if(pSRWLockXp)
			{
				delete pSRWLockXp;
			}
		}
#endif //#ifdef _SRWLOCK_SURPPORT_XP_
	}

public:
	bool InitializeSRWLock()			//Supported : WinVista
	{
#ifdef _SRWLOCK_SURPPORT_XP_
		if(!IsSupportSRWLock())
		{
			CSRWLockXp* pSRWLockXp = new CSRWLockXp();
			m_SRWLock.Ptr = (LPVOID)pSRWLockXp;

			return pSRWLockXp->InitializeSRWLock();
		}

#endif //#ifdef _SRWLOCK_SURPPORT_XP_

		GetFuncFromDll(pfnInitSRWLock, InitializeSRWLock, "Kernel32.dll");

		if(pfnInitSRWLock)
		{
			pfnInitSRWLock(&m_SRWLock);
			return true;
		}
		ATLVERIFY(FALSE);
		return false;
	}

	bool AcquireSRWLockExclusive()
	{
#ifdef _SRWLOCK_SURPPORT_XP_

		if(!IsSupportSRWLock())
		{
			CSRWLockXp* pSRWLockXp = reinterpret_cast<CSRWLockXp*>(m_SRWLock.Ptr);
			if(NULL == pSRWLockXp)
				return false;

			return pSRWLockXp->AcquireSRWLockExclusive();
		}

#endif //#ifdef _SRWLOCK_SURPPORT_XP_

		GetFuncFromDll(pfnInitSRWLock, AcquireSRWLockExclusive, "Kernel32.dll");
		if(pfnInitSRWLock)
		{
			pfnInitSRWLock(&m_SRWLock);
			return true;
		}
		ATLVERIFY(FALSE);
		return false;
	}

	bool ReleaseSRWLockExclusive()
	{
#ifdef _SRWLOCK_SURPPORT_XP_

		if(!IsSupportSRWLock())
		{
			CSRWLockXp* pSRWLockXp = reinterpret_cast<CSRWLockXp*>(m_SRWLock.Ptr);
			if(NULL == pSRWLockXp)
				return false;

			return pSRWLockXp->ReleaseSRWLockExclusive();
		}

#endif //#ifdef _SRWLOCK_SURPPORT_XP_

		GetFuncFromDll(pfnInitSRWLock, ReleaseSRWLockExclusive, "Kernel32.dll");
		if(pfnInitSRWLock)
		{
			pfnInitSRWLock(&m_SRWLock);
			return true;
		}
		ATLVERIFY(FALSE);
		return false;
	}

	bool AcquireSRWLockShared()
	{
#ifdef _SRWLOCK_SURPPORT_XP_
		
		if(!IsSupportSRWLock())
		{
			CSRWLockXp* pSRWLockXp = reinterpret_cast<CSRWLockXp*>(m_SRWLock.Ptr);
			if(NULL == pSRWLockXp)
				return false;

			return pSRWLockXp->AcquireSRWLockShared();
		}

#endif //#ifdef _SRWLOCK_SURPPORT_XP_

		GetFuncFromDll(pfnInitSRWLock, AcquireSRWLockShared, "Kernel32.dll");
		if(pfnInitSRWLock)
		{
			pfnInitSRWLock(&m_SRWLock);
			return true;
		}
		ATLVERIFY(FALSE);
		return false;

	}

	bool ReleaseSRWLockShared()
	{
#ifdef _SRWLOCK_SURPPORT_XP_

		if(!IsSupportSRWLock())
		{
			CSRWLockXp* pSRWLockXp = reinterpret_cast<CSRWLockXp*>(m_SRWLock.Ptr);
			if(NULL == pSRWLockXp)
				return false;

			return pSRWLockXp->ReleaseSRWLockShared();
		}

#endif //#ifdef _SRWLOCK_SURPPORT_XP_

		GetFuncFromDll(pfnInitSRWLock, ReleaseSRWLockShared, "Kernel32.dll");
		if(pfnInitSRWLock)
		{
			pfnInitSRWLock(&m_SRWLock);
			return true;
		}
		ATLVERIFY(FALSE);
		return false;
	}

	bool TryAcquireSRWLockExclusive()	//Supported : Win7
	{
#ifdef _SRWLOCK_SURPPORT_XP_

		if(!IsSupportSRWLock())
		{
			CSRWLockXp* pSRWLockXp = reinterpret_cast<CSRWLockXp*>(m_SRWLock.Ptr);
			if(NULL == pSRWLockXp)
				return false;

			return pSRWLockXp->TryAcquireSRWLockExclusive();
		}

#endif //#ifdef _SRWLOCK_SURPPORT_XP_

		GetFuncFromDll(pfnInitSRWLock, TryAcquireSRWLockExclusive, "Kernel32.dll");
		if(pfnInitSRWLock)
		{
			pfnInitSRWLock(&m_SRWLock);
			return true;
		}
		ATLVERIFY(FALSE);
		return false;
	}

	bool TryAcquireSRWLockShared()		//Supported : Win7
	{
#ifdef _SRWLOCK_SURPPORT_XP_

		if(!IsSupportSRWLock())
		{
			CSRWLockXp* pSRWLockXp = reinterpret_cast<CSRWLockXp*>(m_SRWLock.Ptr);
			if(NULL == pSRWLockXp)
				return false;

			return pSRWLockXp->TryAcquireSRWLockShared();
		}
#endif //#ifdef _SRWLOCK_SURPPORT_XP_

		GetFuncFromDll(pfnInitSRWLock, TryAcquireSRWLockShared, "Kernel32.dll");
		if(pfnInitSRWLock)
		{
			pfnInitSRWLock(&m_SRWLock);
			return true;
		}

		ATLVERIFY(FALSE);
		return false;
	}

#ifdef _SRWLOCK_SURPPORT_XP_

private:
	bool IsSupportSRWLock()				//Check Win7
	{
		return m_bSupportSRWLock;
	}

private:
	bool m_bSupportSRWLock;

#endif //#ifdef _SRWLOCK_SURPPORT_XP_

private:
	SRWLOCK m_SRWLock;
};

class CAutoWriteLock final
{
public:
	CAutoWriteLock(CSRWLock& l) 
		: m_lock(l)
		, m_bLocked(false)
	{
		m_bLocked = m_lock.AcquireSRWLockExclusive();
	}

	~CAutoWriteLock()
	{
		if(m_bLocked)
			m_lock.ReleaseSRWLockExclusive();
	}

public:
	void ReleaseLock()
	{
		if(m_bLocked)
			m_bLocked = !m_lock.ReleaseSRWLockShared();
	}

private:
	CSRWLock& m_lock;
	volatile bool m_bLocked;
};

class CAutoReadLock final
{
public:
	CAutoReadLock(CSRWLock& l)
		: m_lock(l)
		, m_bLocked(false)
	{
		m_bLocked = m_lock.AcquireSRWLockShared();
	}

	~CAutoReadLock()
	{
		if(m_bLocked)
			m_lock.ReleaseSRWLockShared();
	}

public:
	void ReleaseLock()
	{
		if(m_bLocked)
			m_bLocked = !m_lock.ReleaseSRWLockShared();
	}

private:
	CSRWLock& m_lock;
	volatile bool m_bLocked;
};