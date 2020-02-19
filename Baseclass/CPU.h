#pragma once
class CCPU
{
public:
	CCPU(void);
	virtual ~CCPU(void);

public:
	void GetName(WCHAR* pCPUName, int cchMax);
	void GetVendorName(WCHAR* pName,  int cchMax);
	long GetVendor();
	long GetCoreCount();
	DWORD GetCPUSet();


private:
	void  Executecpuid(DWORD veax); //用以执行cpuid指令

private:
	bool m_bIA64;
	DWORD m_dwCPUSet;
	DWORD m_dwCPUCount;

	DWORD  m_eax;   // 存储返回的eax
	DWORD  m_ebx;   // 存储返回的ebx
	DWORD  m_ecx;   // 存储返回的ecx
	DWORD  m_edx;   // 存储返回的edx
};

