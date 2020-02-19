#ifndef NetWorkMonitor_h__
#define NetWorkMonitor_h__

#include <Netlistmgr.h>
#include <atlcomcli.h>

#include "OSVersion.h"

class NetworkChangeCallback
{
public:
	virtual void ConnectivityChanged(BOOL bOK) = 0;
};

class CNetworkChangeNotify : public INetworkListManagerEvents
{
public:
	CNetworkChangeNotify() : m_ref(1), m_NetworkChangeCallback(NULL), m_dwCookie(0)
	{
		
	}

	~CNetworkChangeNotify() {};

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject)
	{
		HRESULT Result = S_OK;
		if (IsEqualIID(riid, IID_IUnknown))
		{
			AddRef();
			*ppvObject = (IUnknown *)this;
		}
		else if (IsEqualIID(riid ,IID_INetworkListManagerEvents))
		{
			AddRef();
			*ppvObject = (INetworkListManagerEvents*)this;
		}
		else
		{
			Result = E_NOINTERFACE;
		}

		return Result;
	}

	ULONG STDMETHODCALLTYPE AddRef()
	{
		return (ULONG)InterlockedIncrement(&m_ref);
	}

	ULONG STDMETHODCALLTYPE Release()
	{
		LONG Result = InterlockedDecrement(&m_ref);
		if (Result == 0)
			delete this;
		return (ULONG)Result;
	}

	virtual HRESULT STDMETHODCALLTYPE ConnectivityChanged(
		/* [in] */ NLM_CONNECTIVITY newConnectivity)
	{
		if((newConnectivity & NLM_CONNECTIVITY_IPV4_INTERNET) || (newConnectivity & NLM_CONNECTIVITY_IPV6_INTERNET))
		{
			m_NetworkChangeCallback->ConnectivityChanged(TRUE);
		}
		else
		{
			m_NetworkChangeCallback->ConnectivityChanged(FALSE);
		}
		return S_OK;
	}

public:
	HRESULT Advise(NetworkChangeCallback* pCallback)
	{
		if(COSVersion::GetInstance()->IsWindowsXP())
			return E_FAIL;

		m_NetworkChangeCallback = pCallback;

		CComPtr<IUnknown> spUnknown;
		HRESULT Result = CoCreateInstance(CLSID_NetworkListManager, NULL, CLSCTX_ALL, IID_IUnknown, (void **)&m_spNetworkListManager);
		if (SUCCEEDED(Result))
		{
			CComPtr<IConnectionPointContainer> spCPContainer;
			Result = m_spNetworkListManager.QueryInterface(&spCPContainer);
			if (SUCCEEDED(Result))
			{
				CComPtr<IConnectionPoint> spConnectPoint;
				Result = spCPContainer->FindConnectionPoint(IID_INetworkListManagerEvents, &spConnectPoint);
				if(SUCCEEDED(Result))
				{
					return spConnectPoint->Advise((IUnknown*)this, &m_dwCookie);
				}
			}
		}
		return E_FAIL;
	}

	HRESULT UnAdvise()
	{
		m_NetworkChangeCallback = NULL;
		CComPtr<IConnectionPointContainer> spCPContainer;
		HRESULT Result = m_spNetworkListManager.QueryInterface(&spCPContainer);
		if (SUCCEEDED(Result))
		{
			CComPtr<IConnectionPoint> spConnectPoint;
			Result = spCPContainer->FindConnectionPoint(IID_INetworkListManagerEvents, &spConnectPoint);
			if(SUCCEEDED(Result))
			{
				return spConnectPoint->Unadvise(m_dwCookie);
			}
		}
		return E_FAIL;
	}

private:
	LONG m_ref;
	DWORD m_dwCookie;
	NetworkChangeCallback* m_NetworkChangeCallback;
	CComPtr<INetworkListManager> m_spNetworkListManager;
};

/* XP网络状态变更通知
BOOL InitWSA()
{
	WSAData data = {0};
	WSAStartup(MAKEWORD(2, 0), &data);
	return TRUE;
}

BOOL UnInitWSA()
{
	WSACleanup();
	return TRUE;
}

void RegisterNetWorkChangeEvent(HWND hWnd, UINT uMsg, WPARAM context)
{
	WSAData data = {0};
	WSAStartup(MAKEWORD(2, 0), &data);

	WSAQUERYSET querySet = {0};
	querySet.dwSize = sizeof(WSAQUERYSET);
	querySet.dwNameSpace = NS_NLA;

	HANDLE LookupHandle = NULL;
	WSALookupServiceBegin(&querySet, LUP_RETURN_ALL, &LookupHandle);

	DWORD BytesReturned = 0;
	WSACOMPLETION wsaComletion;
	wsaComletion.Type = NSP_NOTIFY_HWND;
	wsaComletion.Parameters.WindowMessage.hWnd = hWnd;
	wsaComletion.Parameters.WindowMessage.uMsg = uMsg;
	wsaComletion.Parameters.WindowMessage.context = context;
	WSANSPIoctl(LookupHandle, SIO_NSP_NOTIFY_CHANGE, NULL, 0, NULL, 0, &BytesReturned, &wsaComletion);
	WSALookupServiceEnd(LookupHandle);
}
*/

#endif // NetWorkMonitor_h__
