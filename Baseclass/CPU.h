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
	void  Executecpuid(DWORD veax); //����ִ��cpuidָ��

private:
	bool m_bIA64;
	DWORD m_dwCPUSet;
	DWORD m_dwCPUCount;

	DWORD  m_eax;   // �洢���ص�eax
	DWORD  m_ebx;   // �洢���ص�ebx
	DWORD  m_ecx;   // �洢���ص�ecx
	DWORD  m_edx;   // �洢���ص�edx
};

