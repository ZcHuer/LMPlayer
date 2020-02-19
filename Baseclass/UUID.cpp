#include "stdafx.h"
#include "UUID.h"
#include "CPU.h"
#include "MD5.h"
#include <Setupapi.h>
#pragma comment(lib, "Setupapi.lib")

//////////////////////////////////////////////////////////////////////////
//导出的API
//////////////////////////////////////////////////////////////////////////
void CGetDeviceInfo::CrtReadCPUInfomation( WCHAR* pName, int cchName, WCHAR* pVendor, int cchVerdor, long* pdwVender, long* pCoreCount, DWORD* pdwCPUSet )
{
	CCPU cpu_z;

	if( pName )
		cpu_z.GetName( pName, cchName );

	if( pVendor )
		cpu_z.GetVendorName( pVendor, cchVerdor );

	if( pdwVender )
		*pdwVender = cpu_z.GetVendor();

	if( pCoreCount )
		*pCoreCount = cpu_z.GetCoreCount();

	if( pdwCPUSet )
		*pdwCPUSet = cpu_z.GetCPUSet();
}

CGetDeviceInfo::CGetDeviceInfo(void)
{
	m_hDll = LoadLibrary(L"IPHLPAPI.dll");
}


CGetDeviceInfo::~CGetDeviceInfo(void)
{
	if (m_hDll)
	{
		FreeLibrary(m_hDll);
		m_hDll = NULL;
	}
}

typedef ULONG(WINAPI *PFNGetAdaptersInfo)(PIP_ADAPTER_INFO AdapterInfo, PULONG SizePointer);
BOOL CGetDeviceInfo::GetMAC(MIDINFO& mi)
{
	if (m_hDll == NULL)
		return FALSE;

	PFNGetAdaptersInfo pGetAdaptersInfo = (PFNGetAdaptersInfo)GetProcAddress(m_hDll, "GetAdaptersInfo");
	if (pGetAdaptersInfo == NULL)
		return FALSE;

	PIP_ADAPTER_INFO pAdapterInfo = NULL;
	DWORD dwRetVal = 0;
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO)*MAX_MAC_COUNT;
	pAdapterInfo = (PIP_ADAPTER_INFO)malloc(ulOutBufLen);

	dwRetVal = pGetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
	if (dwRetVal == ERROR_BUFFER_OVERFLOW)
	{
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
		dwRetVal = pGetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
	}

	bool bRet = FALSE;
	if (dwRetVal == NO_ERROR)
	{
		int iCnt = 0;
		int iNet = 0, iWireless = 3;
		PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
		while (pAdapter)
		{
			if (iCnt >= MAX_MAC_COUNT)
				break;

			if (pAdapter->Type == MIB_IF_TYPE_ETHERNET)
			{
				memcpy(mi.mac[iNet].Address, pAdapter->Address, sizeof(mi.mac[iNet].Address));
				iNet++;
				iCnt++;
			}
			else if (pAdapter->Type == IF_TYPE_IEEE80211)
			{
				memcpy(mi.mac[iWireless].Address, pAdapter->Address, sizeof(mi.mac[iWireless].Address));
				iWireless++;
				iCnt++;
			}

			pAdapter = pAdapter->Next;
		}

		bRet = TRUE;
	}

	free(pAdapterInfo);
	return bRet;
}

BOOL CGetDeviceInfo::GetCPU(MIDINFO& mi)
{
	//取得CPU信息
	CrtReadCPUInfomation(mi.szCPU, _countof(mi.szCPU), mi.szCPUVendor, _countof(mi.szCPUVendor), NULL, NULL, NULL);
	return TRUE;
}

void CGetDeviceInfo::GetMemorySize(u_int64& nSize)
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	nSize = statex.ullTotalPhys;
}

void CGetDeviceInfo::GetGraphics(std::wstring& device)
{
	DWORD dwIdx = 0;
	SP_DEVINFO_DATA adapterData;
	DWORD dwType = 0;
	DWORD dwBuflen = 0;
	wchar_t* pBuf = NULL;
	HDEVINFO hScsi = INVALID_HANDLE_VALUE;

	//显卡GUID  
	GUID guidScsiRaid = { 0x4d36e968,0xe325,0x11CE,{ 0xBF,   0xC1,   0x08,   0x00,   0x2B,   0xE1,   0x03,   0x18 } };

	BOOL   bret = FALSE;
	hScsi = SetupDiGetClassDevs(&guidScsiRaid, NULL, NULL, DIGCF_PRESENT);
	adapterData.cbSize = sizeof(SP_DEVINFO_DATA);

	
	for (dwIdx = 0; SetupDiEnumDeviceInfo(hScsi, dwIdx, &adapterData); dwIdx++)
	{
		SetupDiGetDeviceRegistryProperty(hScsi, &adapterData, SPDRP_DEVICEDESC, &dwType, NULL, 0, &dwBuflen);

		if (ERROR_INSUFFICIENT_BUFFER == GetLastError())
		{
			pBuf = (wchar_t*)malloc(dwBuflen * sizeof(wchar_t));
			if (SetupDiGetDeviceRegistryProperty(hScsi, &adapterData, SPDRP_DEVICEDESC, &dwType, (unsigned char *)pBuf, dwBuflen, NULL))
			{
				device = (wchar_t*)pBuf;
			}
			free(pBuf);
			pBuf = NULL;
		}
	}

	SetupDiDestroyDeviceInfoList(hScsi);
}

typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
LPFN_ISWOW64PROCESS fnIsWow64Process;

BOOL CGetDeviceInfo::IsWow64()
{
	BOOL bIsWow64 = FALSE;

	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
		GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

	if (NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
		{
			//handle error  
		}
	}

	return bIsWow64;
}

void CGetDeviceInfo::GetOSVersion(CString& strVer)
{
	BOOL bIs64 = IsWow64();

	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&osvi);
	if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
	{
		strVer = L"xp-32";
	}
	else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1)
	{
		if (bIs64)
		{
			strVer = L"win7-64";
		}
		else
		{
			strVer = L"win7-32";
		}
	}
	else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion >= 2)
	{
		if (bIs64)
		{
			strVer = L"win8-64";
		}
		else
		{
			strVer = L"win8-32";
		}
	}
	else if (osvi.dwMajorVersion == 10)
	{
		if (bIs64)
		{
			strVer = L"win10-64";
		}
		else
		{
			strVer = L"win10-32";
		}
	}
	else
	{
		strVer = L"other";
	}
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#define MAX_MD5_BUFFER			16
CUUID::CUUID(void)
{
}


CUUID::~CUUID(void)
{
}

BOOL CUUID::MakeUUID(std::string& struuid , int cchMax)
{
	MIDINFO mi = {0};

	BOOL bGetMac = m_devInfo.GetMAC(mi);
	if( !bGetMac )
		return FALSE;

	m_devInfo.GetCPU(mi);

	//保持操作系统版本号
	OSVERSIONINFO si={0};
	si.dwOSVersionInfoSize = sizeof(si);
	GetVersionEx(&si);
	mi.dwMajorVersion = si.dwMajorVersion;
	mi.dwMinorVersion = si.dwMinorVersion;

	//通过Buffer生成MD5

	BYTE byMD5[MAX_MD5_BUFFER] = {0};
	//CrtGetMD5((BYTE*)&mi, sizeof(mi), byMD5, sizeof(byMD5));

	 struuid = CMD5Checksum::GetMD5((BYTE*)&mi, sizeof(mi));
	
	//UUID格式：{8BAD2B08-47F1-8E9E-79FED-87B11944732}
// 	swprintf( pUUID ,  _T("{%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X}"), 
// 		byMD5[0], byMD5[1], byMD5[2], byMD5[3], 
// 		byMD5[4], byMD5[5], 
// 		byMD5[6], byMD5[7], 
// 		byMD5[8], byMD5[9], 
// 		byMD5[10], byMD5[11], byMD5[12], byMD5[13], byMD5[14], byMD5[15] );

	return TRUE;
}



