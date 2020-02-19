#include "stdafx.h"
#include "NetworkStateTool.h"
#include <Iphlpapi.h>

#pragma comment(lib,"Iphlpapi.lib")

HANDLE g_hEventNetworkState = NULL;
HANDLE g_hEventWirelessSignal = NULL;

bool IsValidArgsPtr(PARGS p)
{
	Args args;
	if(!ReadProcessMemory(GetCurrentProcess(), p, &args, sizeof(Args), 0))
		return false;
	return true;
}

unsigned _stdcall WirelessSignalMonitorFunc(void* Param)
{
	PARGS pArgs = (PARGS)Param;
	if (pArgs)
	{
		if (g_hEventWirelessSignal == NULL)
		{
			::CoUninitialize();
			_endthreadex(0);
			return 0;
		}
		while (WaitForSingleObject(g_hEventWirelessSignal, pArgs->uElapse) == WAIT_TIMEOUT)
		{
			
			if(IsValidArgsPtr(pArgs))
				pArgs->pTool->CheckWirelessSignalQuality(pArgs->pTool->GetCurrentAdapterId());
			else
				break;
		}
		CloseHandle(g_hEventWirelessSignal);
		g_hEventWirelessSignal = NULL;
	}
	_endthreadex(0);
	return 0;
}

unsigned _stdcall NetworkStateMonitorFunc(void* Param)
{
	::CoInitialize(NULL);
	PARGS pArgs = (PARGS)Param;
	if (pArgs)
	{
		if (g_hEventNetworkState == NULL)
		{
			::CoUninitialize();
			_endthreadex(0);
			return 0;
		}
		while (WaitForSingleObject(g_hEventNetworkState, pArgs->uElapse) == WAIT_TIMEOUT)
		{
			if (IsValidArgsPtr(pArgs))
			{
				VecNetworkConnectedInfo vecInfo;
				pArgs->pTool->GetNetworkConnectedInfo(vecInfo);
			}
			else
			{
				break;
			}
		}
		CloseHandle(g_hEventNetworkState);
		g_hEventNetworkState = NULL;
	}
	
	::CoUninitialize();
	_endthreadex(0);
	return 0;
}

CNetworkStateTool::CNetworkStateTool():
	m_pCallback(NULL),
m_connectedType (CT_Disconnected),
m_eSignalQuality ( WSQ_Signal0),
m_nSignal (100),
m_pNetworkStateArgs (NULL),
m_pWirelessSignalArgs (NULL),
m_bStartWirelessSignalMonitor (false),
m_bStartNetworkStateMonitor (false),

m_uElapse(DEFAULT_WIRELESS_MONITOR_ELAPSE),

m_pFNWlanOpenHandle (NULL),
m_pFNWlanCloseHandle (NULL),
m_pFNWlanQueryInterface (NULL)

{
	HMODULE hDll = LoadLibrary(L"Wlanapi.dll");
	if (hDll)
	{
		m_wlandll.Attach(hDll);
		m_pFNWlanOpenHandle = m_wlandll.GetFunc<PFNWlanOpenHandle>("WlanOpenHandle");
		m_pFNWlanCloseHandle = m_wlandll.GetFunc<PFNWlanCloseHandle>("WlanCloseHandle");
		m_pFNWlanQueryInterface = m_wlandll.GetFunc<PFNWlanQueryInterface>("WlanQueryInterface");
		m_pFNWlanFreeMemory = m_wlandll.GetFunc<PFNWlanFreeMemory>("WlanFreeMemory");
		m_pFNWlanEnumInterfaces = m_wlandll.GetFunc<PFNWlanEnumInterfaces>("WlanEnumInterfaces");
	}
}


CNetworkStateTool::~CNetworkStateTool()
{
	if (g_hEventNetworkState)
	{
		SetEvent(g_hEventNetworkState);
	}
	if (g_hEventWirelessSignal)
	{
		SetEvent(g_hEventWirelessSignal);
	}
	if (m_pNetworkStateArgs)
	{
		delete m_pNetworkStateArgs;
		m_pNetworkStateArgs = NULL;
	}
	if (m_pWirelessSignalArgs)
	{
		delete m_pWirelessSignalArgs;
		m_pWirelessSignalArgs = NULL;
	}
	m_bStartWirelessSignalMonitor = false;
	m_bStartNetworkStateMonitor = false;
}

bool CNetworkStateTool::_WlanEnum(VecWirelessAdapter& adapters)
{
	DWORD dwMaxClientVersion = 2;
	DWORD dwCurClientVersion = 0;
	HANDLE hClientHandle;
	bool bRtn = false;
	if (m_pFNWlanOpenHandle && m_pFNWlanCloseHandle && m_pFNWlanQueryInterface
		&& m_pFNWlanEnumInterfaces && m_pFNWlanFreeMemory)
	{
		if (m_pFNWlanOpenHandle)
		{
			DWORD dwResult = m_pFNWlanOpenHandle(dwMaxClientVersion, NULL, &dwCurClientVersion, &hClientHandle);
			if (ERROR_SUCCESS != dwResult)
			{
				if (m_pFNWlanCloseHandle)
					m_pFNWlanCloseHandle(hClientHandle, nullptr);
				return false;
			}

			WLAN_INTERFACE_INFO_LIST* pIfList = NULL;
			PWLAN_INTERFACE_INFO pInfo = NULL;
			if (m_pFNWlanEnumInterfaces)
			{
				dwResult = m_pFNWlanEnumInterfaces(hClientHandle, NULL, &pIfList);
				if (ERROR_SUCCESS != dwResult)
				{
					if (m_pFNWlanCloseHandle)
						m_pFNWlanCloseHandle(hClientHandle, nullptr);
					return false;
				}

				DWORD dwCurIndex = pIfList->dwIndex;
				for (DWORD i = 0; i < pIfList->dwNumberOfItems; ++i)
				{
					pInfo = &pIfList->InterfaceInfo[i];
					adapters.push_back(pInfo->InterfaceGuid);
				}
			}
			if (m_pFNWlanFreeMemory)
				m_pFNWlanFreeMemory(pIfList);
			pIfList = NULL;
			if (m_pFNWlanCloseHandle)
				m_pFNWlanCloseHandle(hClientHandle, nullptr);
		}
	}
	return true;
}

bool CNetworkStateTool::_IsWirelessAdapter(GUID& adpter, VecWirelessAdapter& vecAdapters)
{
	VecWirelessAdapter::iterator it = std::find(vecAdapters.begin(), vecAdapters.end(), adpter);
	return it != vecAdapters.end();
}

bool CNetworkStateTool::_GetNetworkConnectedInfo(VecNetworkConnectedInfo& vecInfo)
{
	VecWirelessAdapter vecAdapter;
	_WlanEnum(vecAdapter);

	CComPtr<INetworkListManager> spNLM;
	HRESULT hr = spNLM.CoCreateInstance(__uuidof(NetworkListManager));
	if (FAILED(hr))
		return false;

	CComPtr<IEnumNetworkConnections> spENC;
	hr = spNLM->GetNetworkConnections(&spENC);
	if (FAILED(hr))
		return false;

	{
		ULONG uFetched = 0;
		CComPtr<INetworkConnection> spConn;
		while (S_OK == spENC->Next(1, &spConn, &uFetched))
		{
			NetworkConnectedInfo info;
			info.type = CT_Disconnected;
			CComPtr<INetwork> spNetwork;
			hr = spConn->GetNetwork(&spNetwork);
			if(spNetwork)
			{
				CComBSTR bstrName;
				hr = spNetwork->GetName(&bstrName);
				if(SUCCEEDED(hr)) 
					info.name = (LPCWSTR)bstrName;

				CComBSTR bstrDescription;
				hr = spNetwork->GetDescription(&bstrDescription);
				if(SUCCEEDED(hr)) 
					info.description = (LPCWSTR)bstrDescription;
			}
			
			GUID adptrid;
			hr = spConn->GetAdapterId(&adptrid);
			info.adapterId = adptrid;
			VARIANT_BOOL vbConnected = VARIANT_FALSE;
			hr = spConn->get_IsConnected(&vbConnected);
			if (SUCCEEDED(hr) && VARIANT_TRUE == vbConnected)
			{
				info.type = CT_LocalConnected;
			}

			VARIANT_BOOL vbInternetConnected = VARIANT_FALSE;
			hr = spConn->get_IsConnectedToInternet(&vbInternetConnected);
			if (SUCCEEDED(hr) && VARIANT_TRUE == vbInternetConnected)
			{
				info.type = CT_InternetConnected;
			}

			if (SUCCEEDED(hr) && (_IsWirelessAdapter(adptrid, vecAdapter)))
			{
				if (info.type == CT_LocalConnected)
					info.type = CT_WirelessLocalConnected;
				else if(info.type == CT_InternetConnected)
					info.type = CT_WirelessConnected;
			}
			vecInfo.push_back(info);
			spConn.Release();
		}
	}
	return true;
}

bool CNetworkStateTool::GetNetworkConnectedInfo(VecNetworkConnectedInfo& vecInfo)
{
	if (!IsSupportedByWindowsVer())
		return false;

	_GetNetworkConnectedInfo(vecInfo);
	
	VecNetworkConnectedInfo::iterator it = vecInfo.begin();
	NetworkConnectedInfo currentInfo;
	currentInfo.type = CT_Disconnected;
	//优先记录有线连接状态，其次记录无线链接状态， 然后记录其他状态， 为连接状态变更通知使用
	for (; it != vecInfo.end(); it++)
	{
		if (currentInfo.type != it->type)
		{
			if (it->type == CT_InternetConnected)
			{
				currentInfo.type = it->type;
				currentInfo.adapterId = it->adapterId;
			}
			else if (it->type == CT_WirelessConnected)
			{
				if (currentInfo.type != CT_InternetConnected)
				{
					currentInfo.type = it->type;
					currentInfo.adapterId = it->adapterId;
				}
			}
			else
			{
				if(currentInfo.type != CT_InternetConnected && currentInfo.type != CT_WirelessConnected)
				{
					currentInfo.type = it->type;
					currentInfo.adapterId = it->adapterId;
				}
			}
		}
	}
	if (currentInfo.type != CT_Disconnected)
	{
		m_currentAdapterId = currentInfo.adapterId;
	}
	
	SetConnectedType(currentInfo.type);
	return true;
}

bool CNetworkStateTool::IsSupportedByWindowsVer()
{
	OSVERSIONINFO osvi;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&osvi);

	if (osvi.dwMajorVersion >= 6)
		return true;

	return false;
}

bool CNetworkStateTool::WireLessSignalChanged(WirelessSignalQuality eSignal, int iSignal)
{
	if (m_pCallback)
		m_pCallback->SignalQualityChanged(eSignal, iSignal);
	return true;
}

bool CNetworkStateTool::NetworkStateChanged(NetworkStateChangedType type)
{
	if (m_pCallback)
	{
		m_pCallback->NetworkStateChanged(type);
	}
	return true;
}

bool CNetworkStateTool::StartNetworkStateMonitor(UINT uElapse)
{
	if (g_hEventNetworkState == NULL)
	{
		g_hEventNetworkState = CreateEvent(NULL, TRUE, FALSE, L"NetworkStateEvent");
		if (g_hEventNetworkState)
		{
			m_pNetworkStateArgs = new Args;
			m_pNetworkStateArgs->pTool = this;
			m_pNetworkStateArgs->uElapse = uElapse;
			HANDLE hThreadNetworkState = (HANDLE)_beginthreadex(NULL, 0, &NetworkStateMonitorFunc, (void*)m_pNetworkStateArgs, 0, NULL);
			if (hThreadNetworkState)
			{
				m_bStartNetworkStateMonitor = true;
				CloseHandle(hThreadNetworkState);
			}
		}
	}	
	return true;
}

bool CNetworkStateTool::StopNetworkStateMonitor()
{
	if (g_hEventNetworkState)
	{
		SetEvent(g_hEventNetworkState);
	}
	if (m_pNetworkStateArgs)
	{
		delete m_pNetworkStateArgs;
		m_pNetworkStateArgs = NULL;
	}
	m_bStartNetworkStateMonitor = false;
	return true;
}

bool CNetworkStateTool::StartWirelessSignalMonitor(UINT uElapse)
{
	m_uElapse = uElapse;
	if (g_hEventWirelessSignal == NULL)
	{
		g_hEventWirelessSignal = CreateEvent(NULL, TRUE, FALSE, L"WirelessSignalEvent");
		if (g_hEventWirelessSignal)
		{
			m_pWirelessSignalArgs = new Args;
			m_pWirelessSignalArgs->pTool = this;
			m_pWirelessSignalArgs->uElapse = uElapse;
			HANDLE hThreadWirelessSignal = (HANDLE)_beginthreadex(NULL, 0, &WirelessSignalMonitorFunc, (void*)m_pWirelessSignalArgs, 0, NULL);
			if (hThreadWirelessSignal)
			{
				m_bStartWirelessSignalMonitor = true;
				CloseHandle(hThreadWirelessSignal);
			}
				
		}
	}
	return true;
}

bool CNetworkStateTool::StopWirelessSignalMonitor()
{
	if (g_hEventWirelessSignal)
	{
		SetEvent(g_hEventWirelessSignal);
	}
	if (m_pWirelessSignalArgs)
	{
		delete m_pWirelessSignalArgs;
		m_pWirelessSignalArgs = NULL;
	}
	m_bStartWirelessSignalMonitor = false;
	return true;
}

void CNetworkStateTool::SetWirelessSignalQuality(int nSignal)
{
	WirelessSignalQuality eSignalQuality;
	if (nSignal >= 0 && nSignal < 65)
		eSignalQuality = WSQ_Signal1;
	else if (nSignal >= 65 && nSignal < 75)
		eSignalQuality = WSQ_Signal2;
	else if (nSignal >= 75 && nSignal < 85)
		eSignalQuality = WSQ_Signal3;
	else if (nSignal >= 85 && nSignal < 95)
		eSignalQuality = WSQ_Signal4;
	else if (nSignal >= 95 && nSignal <= 100)
		eSignalQuality = WSQ_Signal5;
	else eSignalQuality = WSQ_Signal0;

	if (eSignalQuality != m_eSignalQuality)
	{
		m_eSignalQuality = eSignalQuality;
		m_nSignal = nSignal;
		if(m_bStartWirelessSignalMonitor)
			WireLessSignalChanged(m_eSignalQuality, nSignal);
	}
}

void CNetworkStateTool::SetConnectedType(ConnectedType type)
{
	if (m_connectedType != type)
	{
		m_connectedType = type;
		if(m_bStartNetworkStateMonitor)
			NetworkStateChanged((NetworkStateChangedType)m_connectedType);
		if (m_connectedType == CT_WirelessConnected)
		{
			if (m_uElapse != 0)
				StartWirelessSignalMonitor(m_uElapse);
		}
		else
			StopWirelessSignalMonitor();
	}
}

void CNetworkStateTool::CheckWirelessSignalQuality(GUID adapterId)
{
	DWORD dwMaxClientVersion = 2;
	DWORD dwCurClientVersion = 0;
	HANDLE hClientHandle;
	bool bRtn = false;
	if (m_pFNWlanOpenHandle && m_pFNWlanCloseHandle && m_pFNWlanQueryInterface)
	{
		DWORD dwResult = m_pFNWlanOpenHandle(dwMaxClientVersion, NULL, &dwCurClientVersion, &hClientHandle);
		if (ERROR_SUCCESS != dwResult)
		{
			m_pFNWlanCloseHandle(hClientHandle, nullptr);
			return ;
		}
		PWLAN_CONNECTION_ATTRIBUTES pConnectInfo = NULL;
		DWORD connectInfoSize = sizeof(WLAN_CONNECTION_ATTRIBUTES);
		WLAN_OPCODE_VALUE_TYPE opCode = wlan_opcode_value_type_invalid;

		dwResult = m_pFNWlanQueryInterface(hClientHandle, &adapterId, wlan_intf_opcode_current_connection, NULL, &connectInfoSize, (LPVOID*)&pConnectInfo, &opCode);
		if (ERROR_SUCCESS == dwResult)
		{
			if (pConnectInfo->wlanAssociationAttributes.wlanSignalQuality >= 0 && pConnectInfo->wlanAssociationAttributes.wlanSignalQuality <= 100)
			{
				SetWirelessSignalQuality((int)pConnectInfo->wlanAssociationAttributes.wlanSignalQuality);
			}
		}

		m_pFNWlanCloseHandle(hClientHandle, nullptr);
	}
}

bool CNetworkStateTool::GetProxyHttpInfo(bool& bProxyEnable, bool& bProxyHttp, CString& cstrProxyServer, CString& cstrProxyOveride)
{
	CString cstrSubKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings";

	DWORD dwProxyEnable = 0;
	DWORD dwProxyHttp = 0;
	TCHAR szData[MAX_PATH];

	DWORD dwType = REG_DWORD;
	DWORD cbData = sizeof(dwProxyEnable);
	if (ERROR_SUCCESS != SHGetValue(HKEY_CURRENT_USER, cstrSubKey, L"ProxyEnable", &dwType, &dwProxyEnable, &cbData))
		return false;
	bProxyEnable = (1 == dwProxyEnable);

	cbData = sizeof(szData);
	dwType = REG_SZ;
	if (ERROR_SUCCESS != SHGetValue(HKEY_CURRENT_USER, cstrSubKey, L"ProxyServer", &dwType, szData, &cbData))
		return false;
	cstrProxyServer = szData;

	if (ERROR_SUCCESS != SHGetValue(HKEY_CURRENT_USER, cstrSubKey, L"ProxyOverride", &dwType, szData, &cbData))
		return false;
	cstrProxyOveride = szData;

	cbData = sizeof(dwProxyHttp);
	if (ERROR_SUCCESS != SHGetValue(HKEY_CURRENT_USER, cstrSubKey, L"ProxyHttp", &dwType, &dwProxyHttp, &cbData))
		return false;
	bProxyHttp = (1 == dwProxyHttp);

	return true;
}

bool CNetworkStateTool::GetNetWorkInfo(VecNetworkConnectedInfo& vecInfo)
{
	if (!IsSupportedByWindowsVer())
		return false;

	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		//如果函数返回的是ERROR_BUFFER_OVERFLOW
		//则说明GetAdaptersInfo参数传递的内存空间不够,同时其传出stSize,表示需要的空间大小
		//这也是说明为什么stSize既是一个输入量也是一个输出量
		//释放原来的内存空间
		delete pIpAdapterInfo;
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	}
	if (ERROR_SUCCESS == nRel)
	{
		while (pIpAdapterInfo)
		{
			NetworkConnectedInfo info;
			info.name = pIpAdapterInfo->AdapterName;
			info.description = pIpAdapterInfo->Description;
			switch (pIpAdapterInfo->Type)
			{
			case MIB_IF_TYPE_OTHER:
				info.adapterType = L"OTHER";
				break;
			case MIB_IF_TYPE_ETHERNET:
				info.adapterType = L"ETHERNET";
				break;
			case MIB_IF_TYPE_TOKENRING:
				info.adapterType = L"TOKENRING";
				break;
			case MIB_IF_TYPE_FDDI:
				info.adapterType = L"FDDI";
				break;
			case MIB_IF_TYPE_PPP:
				info.adapterType = L"PPP";
				break;
			case MIB_IF_TYPE_LOOPBACK:
				info.adapterType = L"LOOPBACK";
				break;
			case MIB_IF_TYPE_SLIP:
				info.adapterType = L"SLIP";
				break;
			default:
				break;
			}
			for (DWORD i = 0; i < pIpAdapterInfo->AddressLength; i++)
			{
				if (i < pIpAdapterInfo->AddressLength - 1)
				{
					info.mac.AppendFormat(L"%02X-", pIpAdapterInfo->Address[i]);
				}
				else
				{
					info.mac.AppendFormat(L"%02X\n", pIpAdapterInfo->Address[i]);
				}
			}
			//可能网卡有多IP,因此通过循环去判断
			IP_ADDR_STRING *pIpAddrString = &(pIpAdapterInfo->IpAddressList);
			do
			{
				CStringA strIpAddress;
				strIpAddress.AppendFormat("IpAddress: %s\r\n", pIpAddrString->IpAddress.String);
				strIpAddress.AppendFormat("IpMask: %s\r\n", pIpAddrString->IpMask.String);
				strIpAddress.AppendFormat("GatewayList: %s", pIpAdapterInfo->GatewayList.IpAddress.String);
				CUTFString utf(strIpAddress);
				info.ipAddress.push_back(utf.UTF16());
				pIpAddrString = pIpAddrString->Next;
			} while (pIpAddrString);
			pIpAdapterInfo = pIpAdapterInfo->Next;
			vecInfo.push_back(info);
		}

	}
	if (pIpAdapterInfo)
	{
		delete pIpAdapterInfo;
	}

	return true;
}
