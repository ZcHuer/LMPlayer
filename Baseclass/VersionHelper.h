#pragma once

class CVersion
{
private:
	struct stMainVersionMajorBuild
	{
		WORD wApartment;
		WORD wMajorBuild;		
	};

	struct stMainVersionSubBuild
	{
		WORD wType;
		WORD wDate;
	};

	struct stMEEVersionSubBuild
	{
		WORD wDay;
		WORD wMonth;	
	};

	union unionMajorBuild
	{
		DWORD dwMajorBuild;
		stMainVersionMajorBuild stMajorBuild;		
	};

	union unionSubBuild
	{
		DWORD dwSubBuild;
		stMainVersionSubBuild stMainSubBuild;
		stMEEVersionSubBuild stMEESubBuild;
	};

public:
	CVersion();
	CVersion(unsigned __int64 nVer);
	CVersion(LPCWSTR pwsVer, bool& bReturn);
	CVersion(DWORD dwMajor, DWORD dwSub, DWORD dwMajorBuild, DWORD dwSubBuild);
	CVersion(DWORD dwMajor, DWORD dwSub, WORD wMajorBuild, WORD wApartment, WORD wDate, WORD wType);
	CVersion(DWORD dwMajor, DWORD dwSub, DWORD dwYear, WORD wMonth, WORD wDay);
	CVersion(const CVersion& verCopy);
	virtual ~CVersion();

public:
	CVersion& operator =(const CVersion& verRef);

public:
	static bool StringToVersion(LPCWSTR lpwsVersion, CVersion& verVersion);

public:
	long Compare(CVersion& verVersion);
	bool IsOldVersion(CVersion& verVersion);
	bool GetString(WCHAR* lpwszVersionString, DWORD dwSize, LPCWSTR lpwsFormat = NULL);//±ÿ–Î”√%d
	bool GetVersion(unsigned __int64& refI64Vesion);
	
private:
	DWORD m_dwMajorVersion;
	DWORD m_dwSubVersion;
	unionMajorBuild m_unionMajorBuildVersion;
	unionSubBuild m_unionSubBuildVersion;
};
