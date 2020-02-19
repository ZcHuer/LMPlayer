#pragma once
#include "Bind.h"
#include "BaseThread.h"

//=================================================================================================
namespace FBind
{
	class IFCallWorker
	{
	public:
		IFCallWorker(){}
		virtual ~IFCallWorker(){}

	public:
		virtual DWORD GetWorkerThreadID() = 0;
		virtual BOOL SyncFCall(FBind::FCall* lpCall, DWORD_PTR dwpResultPtr) = 0;
		virtual BOOL AsyncFCall(FBind::FCall* lpCall, DWORD_PTR dwpResultPtr) = 0;
		virtual BOOL AsyncFCall(FBind::FCall* lpCall, DWORD_PTR dwpResultPtr, HMODULE hModule) = 0;
		virtual BOOL AsyncFCall(FBind::FCall* lpCall, DWORD_PTR dwpResultPtr, HMODULE hModule, LPCSTR szFunc)
		{
			return AsyncFCall(lpCall, dwpResultPtr, hModule);
		}
		virtual DWORD_PTR GetCurrentResultPtr() = 0;

	protected:
		struct _FCallInfo
		{
			FBind::FCall* _pCall;
			DWORD_PTR _dwResultPtr;
			BOOL _bPost;
			HMODULE _hModule;
			DWORD _dwPostThread;
			LPSTR _psName;
			CStringA _psFuncName;
			BOOL _canceled;

			_FCallInfo(FBind::FCall* lpCall, DWORD_PTR dwResultPtr, BOOL bPost)
				: _pCall(lpCall), _dwResultPtr(dwResultPtr), _bPost(bPost), _hModule(NULL)
				, _psName(NULL), _canceled(FALSE)
			{

			}

			_FCallInfo(FBind::FCall* lpCall, DWORD_PTR dwResultPtr, BOOL bPost, HMODULE hModule, LPCSTR szFunc=NULL) 
				: _pCall(lpCall), _dwResultPtr(dwResultPtr), _bPost(bPost), _hModule(hModule), _canceled(FALSE)
				, _psName(NULL)
			{
				if (bPost)
				{
					_dwPostThread = GetCurrentThreadId();
					
					LPCSTR psName = typeid(*lpCall).name();
					int nLen = lstrlenA(psName) + 1;
					_psName = new char[nLen];
					lstrcpynA(_psName, psName, nLen);
					
					if (szFunc)
						_psFuncName = szFunc;
				}
			}


			~_FCallInfo()
			{
				if (_psName)
					delete[] _psName;
			}

			void SetCanceled() { _canceled = TRUE;  };
		};
	};

//=================================================================================================

	template<class L = CLockNo>
	class IFCallWorkerProxy : public IFCallWorker
	{
	public:
		IFCallWorkerProxy():m_pWorker(NULL){}
		virtual ~IFCallWorkerProxy(){}

	public:
		void SetFCallWorker(IFCallWorker* lpWorker)
		{
			AUTOLOCK(m_lck);
			m_pWorker = lpWorker;
		}
		virtual IFCallWorker* GetFCallWorker()
		{
			return m_pWorker;
		}
	public://IFCallWorker
		virtual DWORD GetWorkerThreadID()
		{
			AUTOLOCK(m_lck);
			IFCallWorker* pWorker = GetFCallWorker();
			if(pWorker)
				return pWorker->GetWorkerThreadID();
			return 0;
		}

		virtual BOOL SyncFCall(FBind::FCall* lpCall, DWORD_PTR dwpResultPtr)
		{
			AUTOLOCK(m_lck);
			IFCallWorker* pWorker = GetFCallWorker();
			if(pWorker)
				return pWorker->SyncFCall(lpCall, dwpResultPtr);
			return FALSE;
		}

		virtual BOOL AsyncFCall(FBind::FCall* lpCall, DWORD_PTR dwpResultPtr)
		{
			AUTOLOCK(m_lck);
			IFCallWorker* pWorker = GetFCallWorker();
			if (pWorker)
				return pWorker->AsyncFCall(lpCall, dwpResultPtr);
			return FALSE;
		}

		virtual BOOL AsyncFCall(FBind::FCall* lpCall, DWORD_PTR dwpResultPtr, HMODULE hModule)
		{
			AUTOLOCK(m_lck);
			IFCallWorker* pWorker = GetFCallWorker();
			if(pWorker)
				return pWorker->AsyncFCall(lpCall, dwpResultPtr, hModule);
			return FALSE;
		}

		virtual DWORD_PTR GetCurrentResultPtr()
		{
			AUTOLOCK(m_lck);
			IFCallWorker* pWorker = GetFCallWorker();
			if(pWorker)
				return pWorker->GetCurrentResultPtr();
			return 0;
		}

	protected:
		IFCallWorker* m_pWorker;
		L m_lck;
	};

//=================================================================================================

	template<class T>
	class CFCallWorkerWnd : public IFCallWorker
	{
	public:
		CFCallWorkerWnd() : m_uRegMsg(0), m_uSubclsID(0), m_dwResultPtr(0), m_dwCurThreadID(0), m_uRegMsgException(0)
		{
			m_uRegMsgException = ::RegisterWindowMessage(L"FCallWorkerWnd_RegisterMessageString_ExceptionNotify");
		}

	public:
		template<class T>
		static LRESULT CALLBACK _SUBCLASSPROC(
			HWND hWnd,
			UINT uMsg,
			WPARAM wParam,
			LPARAM lParam,
			UINT_PTR uIdSubclass,
			DWORD_PTR dwRefData
			)
		{
			CFCallWorkerWnd<T>* pThis = (CFCallWorkerWnd<T>*)(dwRefData);
			if(pThis)
			{
				return pThis->_OnSubclassWndProc(hWnd, uMsg, wParam, lParam, uIdSubclass, dwRefData);
			}
			return DefSubclassProc(hWnd, uMsg,wParam,lParam);
		}

		LRESULT _OnSubclassWndProc(HWND hWnd,
			UINT uMsg,
			WPARAM wParam,
			LPARAM lParam,
			UINT_PTR uIdSubclass,
			DWORD_PTR dwRefData)
		{
			if (uIdSubclass != m_uSubclsID)
			{
				ATLASSERT(FALSE);
				return DefSubclassProc(hWnd, uMsg, wParam, lParam);
			}

			if(uMsg == m_uRegMsg)
			{
				return OnRegMsgSafe(wParam, lParam);
			}
			LRESULT lr = DefSubclassProc(hWnd, uMsg, wParam, lParam);
			if (uMsg == WM_NCDESTROY)
			{
				BOOL bRtn = RemoveWindowSubclass(hWnd, CFCallWorkerWnd<T>::_SUBCLASSPROC<T>, m_uSubclsID);
				ATLASSERT(bRtn);
				m_uSubclsID = 0;
				m_dwCurThreadID = 0;
				m_uRegMsg = 0;
			}
			return lr;
		}

		BOOL EnableFCallWorkerWnd()
		{
			T* pThis = dynamic_cast<T*>(this);
			if(NULL == pThis)
				return FALSE;

			DWORD dwTID = GetWindowThreadProcessId(pThis->m_hWnd,NULL);
			if(dwTID != GetCurrentThreadId())
				return FALSE;

			if(0 != m_uSubclsID)
				return FALSE;

			m_uRegMsg = RegisterWindowMessage(L"FCallWorkerWnd_RegisterMessageString");

			m_uSubclsID = (UINT_PTR)this;
			BOOL bRtn = SetWindowSubclass(pThis->m_hWnd, CFCallWorkerWnd<T>::_SUBCLASSPROC<T>, m_uSubclsID, (DWORD_PTR)this);
			if(!bRtn)
				m_uSubclsID = 0;
			else
				m_dwCurThreadID = dwTID;

			return bRtn;
		}	

	public://IFCallWorker
		virtual DWORD GetWorkerThreadID()
		{
			if(m_dwCurThreadID)
				return m_dwCurThreadID;

			T* pThis = dynamic_cast<T*>(this);
			if(NULL == pThis || NULL == pThis->m_hWnd)
				return 0;

			DWORD dwPID = 0;
			DWORD dwTID = GetWindowThreadProcessId(pThis->m_hWnd, &dwPID);

			return dwTID;
		}

		virtual BOOL SyncFCall(FBind::FCall* lpCall, DWORD_PTR dwpResultPtr)
		{
			T* pThis = dynamic_cast<T*>(this);
			if (NULL == pThis)
				return FALSE;
			if (m_uRegMsg == 0)
				return FALSE;

			_FCallInfo* _pParam = new _FCallInfo(lpCall, dwpResultPtr, FALSE);
			if (_pParam)
			{
				DWORD_PTR dwpResult = 0;
				BOOL bSend = ::SendMessageTimeout(pThis->m_hWnd, m_uRegMsg, (WPARAM)_pParam, 0, SMTO_NORMAL, 1000, &dwpResult);
				if (!bSend)
					delete _pParam;
				return bSend;
			}
			return FALSE;
		}

		BOOL SyncFCall(FBind::FCall* lpCall, DWORD_PTR dwpResultPtr, BOOL &bTimeOut)
		{
			T* pThis = dynamic_cast<T*>(this);
			if (NULL == pThis)
				return FALSE;
			if (m_uRegMsg == 0)
				return FALSE;

			bTimeOut = FALSE;
			_FCallInfo* _pParam = new _FCallInfo(lpCall, dwpResultPtr, FALSE);
			if (_pParam)
			{
				DWORD_PTR dwpResult = 0;
				BOOL bSend = ::SendMessageTimeout(pThis->m_hWnd, m_uRegMsg, (WPARAM)_pParam, 0, SMTO_NORMAL, 1000, &dwpResult);
				if (!bSend)
				{
					DWORD dwErr = GetLastError();
					if (ERROR_TIMEOUT == dwErr)
					{
						_pParam->SetCanceled();
						bTimeOut = TRUE;
					}
					else
					{
						delete _pParam;
					}
				}
				return bSend;
			}
			return FALSE;
		}

		virtual BOOL AsyncFCall(FBind::FCall* lpCall, DWORD_PTR dwpResultPtr)
		{
			T* pThis = dynamic_cast<T*>(this);
			if(NULL == pThis)
				return FALSE;
			if (m_uRegMsg == 0)
				return FALSE;

			_FCallInfo* _pParam = new _FCallInfo(lpCall, dwpResultPtr, TRUE);
			BOOL bPost = ::PostMessage(pThis->m_hWnd, m_uRegMsg, (WPARAM)_pParam, 0);
			if(!bPost && _pParam)
				delete _pParam;
			return bPost;
		}

		virtual BOOL AsyncFCall(FBind::FCall* lpCall, DWORD_PTR dwpResultPtr, HMODULE hModule)
		{
			T* pThis = dynamic_cast<T*>(this);
			if (NULL == pThis)
				return FALSE;
			if (m_uRegMsg == 0)
				return FALSE;

			_FCallInfo* _pParam = new _FCallInfo(lpCall, dwpResultPtr, TRUE, hModule);
			BOOL bPost = ::PostMessage(pThis->m_hWnd, m_uRegMsg, (WPARAM)_pParam, 0);
			if (!bPost && _pParam)
				delete _pParam;
			return bPost;
		}

		virtual DWORD_PTR GetCurrentResultPtr()
		{
			return m_dwResultPtr;
		}

		LRESULT OnRegMsgSafe(WPARAM wp, LPARAM lp)
		{
			LRESULT lr = 0;
			try
			{
				lr = OnRegMsg(wp, lp);
			}
			catch (...)
			{
				T* pThis = dynamic_cast<T*>(this);
				if (m_uRegMsg && pThis && pThis->m_hWnd)
				{
					DWORD_PTR dwResult = 0;
					BOOL bRtn = ::SendMessageTimeout(pThis->m_hWnd, MSG_ExceptonNotify(), wp, lp, SMTO_NORMAL, 200, &dwResult);
					ATLASSERT(bRtn);
				}
				lr = 0;
			}
			return lr;
		}

		LRESULT OnRegMsg(WPARAM wp, LPARAM lp)
		{
			_FCallInfo* param = reinterpret_cast<_FCallInfo*>(wp);
			if(param)
			{
				if (!param->_canceled)
				{
					FBind::FCall* pCall = (FBind::FCall*)param->_pCall;
					if (pCall)
					{
						m_dwResultPtr = (DWORD_PTR)param->_dwResultPtr;

						TCHAR szFilePath[MAX_PATH + 1] = { 0 };
						if (0 != GetModuleFileName(param->_hModule, szFilePath, MAX_PATH))
						{
							pCall->Call();
						}
						else
						{
							ATLASSERT(FALSE);
						}

						m_dwResultPtr = 0;
						if (param->_bPost)
							FBind::DeleteT(&pCall);
					}
				}
				else 
				{
					// 同步调用，已经取消任务，需要在这里删除FCall
					FBind::FCall* pCall = (FBind::FCall*)param->_pCall;
					if (!param->_bPost && pCall){
						FBind::DeleteT(&pCall);
					}
				}
				delete param;
			}			
			return 1;
		}

	private:
		UINT m_uRegMsg;
		UINT_PTR m_uSubclsID;
		DWORD_PTR m_dwResultPtr;
		DWORD m_dwCurThreadID;
		UINT m_uRegMsgException;

	public:
		UINT MSG_ExceptonNotify() { return m_uRegMsgException; }
	};

//=================================================================================================

	class CFCallWorker : public FBind::IFCallWorker
						, public CMessageThread
	{
	public:
		CFCallWorker(void);
		virtual ~CFCallWorker(void);

	public://FBind::IFCallWorker
		virtual DWORD GetWorkerThreadID();
		virtual BOOL SyncFCall(FBind::FCall* lpCall, DWORD_PTR dwpResultPtr);
		virtual BOOL AsyncFCall(FBind::FCall* lpCall, DWORD_PTR dwpResultPtr);
		virtual BOOL AsyncFCall(FBind::FCall* lpCall, DWORD_PTR dwpResultPtr, HMODULE hModule);
		virtual BOOL AsyncFCall(FBind::FCall* lpCall, DWORD_PTR dwpResultPtr, HMODULE hModule, LPCSTR szFunc);
		virtual DWORD_PTR GetCurrentResultPtr();

	protected://CMessageThread
		virtual void BeforeRun(LPVOID lpvParam);
		virtual void AfterRun(LPVOID lpvParam);
		virtual LRESULT OnMessage(UINT uMsg, WPARAM wp, LPARAM lp, BOOL& bHandled);

	private:
		class _TLSIndexInitializer
		{
		public:
			_TLSIndexInitializer();
			~_TLSIndexInitializer();
		public:
			DWORD GetTLSIndex();
		private:
			static DWORD s_dwTLSIndex;
		};
		static _TLSIndexInitializer s_TLSIndex;
		static BOOL SetCurrentThreadWorker(CFCallWorker* lpWorker);

	public:
		static CFCallWorker* GetCurrentThreadWorker();

	protected://
		static UINT s_uFCallRegMsg;	
		DWORD_PTR m_dwResultPtr;
	};
}//namespace