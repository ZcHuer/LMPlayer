#pragma once
#include <wlanapi.h>
#include <Netlistmgr.h>
#include "dllmodule.h"

class CNetworkStateTool;

#define DEFAULT_WIRELESS_MONITOR_ELAPSE              10000

typedef struct Args
{
	CNetworkStateTool* pTool;
	UINT uElapse;
} *PARGS;

typedef std::vector<GUID> VecWirelessAdapter;
enum WirelessSignalQuality
{
	WSQ_Signal0 = 0,
	WSQ_Signal1,
	WSQ_Signal2,
	WSQ_Signal3,
	WSQ_Signal4,
	WSQ_Signal5,
};

enum ConnectedType
{
	CT_Disconnected = 0,
	CT_LocalConnected,//本地连接
	CT_InternetConnected,//有线
	CT_WirelessConnected,//无线
	CT_WirelessLocalConnected,//无线本线
};

enum NetworkStateChangedType
{
	NSCT_toDisconnected = 0,
	NSCT_toLocalConnected,
	NSCT_toInternetConnected,
	NSCT_toWirelessConnected,
	NSCT_toWirelessLocalConnected,
};

struct NetworkConnectedInfo
{
	ConnectedType type;
	GUID adapterId;
	CString name;
	CString description;
	CString adapterType;
	CString mac;
	vector<CString> ipAddress;
};

typedef vector<NetworkConnectedInfo> VecNetworkConnectedInfo;

//=========dll 中的函数定义================
typedef DWORD(WINAPI *PFNWlanOpenHandle)(DWORD   dwClientVersion, PVOID   pReserved, PDWORD  pdwNegotiatedVersion, PHANDLE phClientHandle);
typedef DWORD (WINAPI *PFNWlanCloseHandle)(HANDLE hClientHandle, PVOID  pReserved);
typedef DWORD (WINAPI *PFNWlanEnumInterfaces)(HANDLE hClientHandle, PVOID   pReserved, PWLAN_INTERFACE_INFO_LIST *ppInterfaceList);
typedef VOID (WINAPI *PFNWlanFreeMemory)(PVOID pMemory);
typedef DWORD (WINAPI *PFNWlanQueryInterface)(HANDLE  hClientHandle, const GUID *pInterfaceGuid, WLAN_INTF_OPCODE  OpCode, PVOID  pReserved, PDWORD  pdwDataSize, PVOID  *ppData, PWLAN_OPCODE_VALUE_TYPE pWlanOpcodeValueType);
//=======================================================

class NetworkStateCallback
{
public:
	virtual void NetworkStateChanged(NetworkStateChangedType type) = 0;
	virtual void SignalQualityChanged(WirelessSignalQuality eSignal, int iSignal) = 0;
};

class CNetworkStateTool
{
public:
	CNetworkStateTool();
	virtual ~CNetworkStateTool();

	bool SetCallback(NetworkStateCallback* pCallback) { m_pCallback = pCallback; return true; }
	bool GetNetworkConnectedInfo(VecNetworkConnectedInfo& vecInfo);
	bool GetProxyHttpInfo(bool& bProxyEnable, bool& bProxyHttp, CString& cstrProxyServer, CString& cstrProxyOveride);
	bool GetNetWorkInfo(VecNetworkConnectedInfo& vecInfo);

	bool StartNetworkStateMonitor(UINT uElapse);
	bool StopNetworkStateMonitor();
	bool StartWirelessSignalMonitor(UINT uElapse);
	bool StopWirelessSignalMonitor();

	ConnectedType GetConnectedType() { return m_connectedType; }
	void SetConnectedType(ConnectedType type);
	WirelessSignalQuality GetWirelessSignalQuality() { return m_eSignalQuality; }
	void SetWirelessSignalQuality(int nSignal);
	int GetWirelessSignal() { return m_nSignal; }
	void CheckWirelessSignalQuality(GUID adapterId);
	void SetWirelessSignalMonitorElapse(UINT uElapse) { m_uElapse = uElapse; }
	GUID GetCurrentAdapterId() { return m_currentAdapterId; }

private:
	bool IsSupportedByWindowsVer();
	bool WireLessSignalChanged(WirelessSignalQuality eSignal, int iSignal);
	bool NetworkStateChanged(NetworkStateChangedType type);

public:
	bool _GetNetworkConnectedInfo(VecNetworkConnectedInfo& info);
	bool _WlanEnum(VecWirelessAdapter& adapters);
	bool _IsWirelessAdapter(GUID& adpter, VecWirelessAdapter& vecAdapters);

private:
	NetworkStateCallback* m_pCallback;
	GUID m_currentAdapterId;
	ConnectedType m_connectedType;
	HANDLE m_hWirelessSignal;
	HANDLE m_hNetworkState;
	PARGS m_pWirelessSignalArgs;
	PARGS m_pNetworkStateArgs;
	WirelessSignalQuality m_eSignalQuality;
	int m_nSignal;
	UINT m_uElapse;
	bool m_bStartNetworkStateMonitor;
	bool m_bStartWirelessSignalMonitor;
	CDllModule m_wlandll;
	PFNWlanOpenHandle m_pFNWlanOpenHandle;
	PFNWlanCloseHandle m_pFNWlanCloseHandle;
	PFNWlanQueryInterface m_pFNWlanQueryInterface;
	PFNWlanFreeMemory m_pFNWlanFreeMemory;
	PFNWlanEnumInterfaces m_pFNWlanEnumInterfaces;
};

