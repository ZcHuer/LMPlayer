#pragma once

#include<Iphlpapi.h>
#include <xstring>

#define MID_MAX_MAC			6
typedef struct _tagMACAddress
{
	BYTE Address[MAX_ADAPTER_ADDRESS_LENGTH];
}MACADDR,*PMACADDR;

#define MAX_MAC_COUNT			6
typedef struct _tagMidInfo
{
	MACADDR mac[MAX_MAC_COUNT];
	WCHAR szCPU[50];								//CPU名称
	WCHAR szCPUVendor[16];					//CPU厂商
	DWORD dwMajorVersion;						//OS-主版本
	DWORD dwMinorVersion;						//OS-次版本
}MIDINFO,*PMIDINFO;

class CGetDeviceInfo
{
public:
	CGetDeviceInfo();
	virtual ~CGetDeviceInfo();

	BOOL GetMAC( MIDINFO& mi );
	BOOL GetCPU(MIDINFO& mi);
	void GetMemorySize(u_int64& nSize);
	void GetGraphics(std::wstring& device);
	void GetOSVersion(CString& strVer);
private:
	void WINAPI CrtReadCPUInfomation(WCHAR* pName, int cchName, WCHAR* pVendor, int cchVerdor, long* pdwVender, long* pCoreCount, DWORD* pdwCPUSet);
	BOOL IsWow64();
private:
	HMODULE m_hDll;
};

class CUUID
{
public:
	CUUID(void);
	~CUUID(void);

public:
	BOOL MakeUUID(std::string& struuid , int cchMax);	//UUID格式：{8BAD2B08-47F1-8E9E-79FED-87B11944732}

private:
	CGetDeviceInfo m_devInfo;
};

