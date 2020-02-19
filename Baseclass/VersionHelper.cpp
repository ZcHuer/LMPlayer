#include "stdafx.h"
#include "VersionHelper.h"

CVersion::CVersion()
{
	m_dwMajorVersion = 0;
	m_dwSubVersion = 0;
	m_unionMajorBuildVersion.dwMajorBuild = 0;
	m_unionSubBuildVersion.dwSubBuild = 0;
}

CVersion::CVersion(unsigned __int64 nVer)
{
	m_dwMajorVersion =	(DWORD)((WORD)((0xFFFF000000000000 & nVer) >> 48));
	m_dwSubVersion =	(DWORD)((WORD)((0x0000FFFF00000000 & nVer) >> 32));
	m_unionMajorBuildVersion.dwMajorBuild = (DWORD)((WORD)((0x00000000FFFF0000 & nVer) >> 16));
	m_unionSubBuildVersion.dwSubBuild =		(DWORD)((WORD)(0x000000000000FFFF & nVer));
}

CVersion::CVersion(LPCWSTR pwsVer, bool& bReturn)
{
	CVersion verTemp;
	bReturn = CVersion::StringToVersion(pwsVer, verTemp);

	*this = verTemp;
}

CVersion::CVersion(DWORD dwMajor, DWORD dwSub, DWORD dwMajorBuild, DWORD dwSubBuild)
{
	m_dwMajorVersion = dwMajor;
	m_dwSubVersion = dwSub;

	m_unionMajorBuildVersion.dwMajorBuild = dwMajorBuild;
	m_unionSubBuildVersion.dwSubBuild = dwSubBuild;
}

CVersion::CVersion(DWORD dwMajor, DWORD dwSub, WORD wMajorBuild, WORD wApartment, WORD wDate, WORD wType)
{
	m_dwMajorVersion = dwMajor;
	m_dwSubVersion = dwSub;

	m_unionMajorBuildVersion.stMajorBuild.wMajorBuild = wMajorBuild;
	m_unionMajorBuildVersion.stMajorBuild.wApartment = wApartment;

	m_unionSubBuildVersion.stMainSubBuild.wDate = wDate;
	m_unionSubBuildVersion.stMainSubBuild.wType = wType;
}

CVersion::CVersion(DWORD dwMajor, DWORD dwSub, DWORD dwYear, WORD wMonth, WORD wDay)
{
	m_dwMajorVersion = dwMajor;
	m_dwSubVersion = dwSub;

	m_unionMajorBuildVersion.dwMajorBuild = dwYear;

	m_unionSubBuildVersion.stMEESubBuild.wMonth = wMonth;
	m_unionSubBuildVersion.stMEESubBuild.wDay = wDay;
}

CVersion::CVersion(const CVersion& verCopy)
{
	m_dwMajorVersion = verCopy.m_dwMajorVersion;
	m_dwSubVersion = verCopy.m_dwSubVersion;
	m_unionMajorBuildVersion.dwMajorBuild = verCopy.m_unionMajorBuildVersion.dwMajorBuild;
	m_unionSubBuildVersion.dwSubBuild = verCopy.m_unionSubBuildVersion.dwSubBuild;
}

CVersion::~CVersion()
{

}

CVersion& CVersion::operator =(const CVersion& verRef)
{
	m_dwMajorVersion = verRef.m_dwMajorVersion;
	m_dwSubVersion = verRef.m_dwSubVersion;
	m_unionMajorBuildVersion.dwMajorBuild = verRef.m_unionMajorBuildVersion.dwMajorBuild;
	m_unionSubBuildVersion.dwSubBuild = verRef.m_unionSubBuildVersion.dwSubBuild;

	return *this;	
}

bool CVersion::StringToVersion(LPCWSTR lpwsVersion, CVersion& verVersion)
{
	DWORD dwMajor = 0;
	DWORD dwSub = 0;
	DWORD dwMajorBuild = 0;
	DWORD dwSubBuild = 0;

	int nRtn = swscanf_s(lpwsVersion, L"%d.%d.%d.%d", &dwMajor, &dwSub, &dwMajorBuild, &dwSubBuild);
	if(nRtn != 4)
		return false;

	CVersion ver(dwMajor, dwSub, dwMajorBuild, dwSubBuild);
	verVersion = ver;

	return true;
}

long CVersion::Compare(CVersion& verVersion)
{
	if(m_dwMajorVersion > verVersion.m_dwMajorVersion)
		return 1;

	if(m_dwMajorVersion < verVersion.m_dwMajorVersion)
		return -1;

	if(m_dwSubVersion > verVersion.m_dwSubVersion)
		return 1;

	if(m_dwSubVersion < verVersion.m_dwSubVersion)
		return -1;

	if(m_unionMajorBuildVersion.dwMajorBuild > verVersion.m_unionMajorBuildVersion.dwMajorBuild)
		return 1;

	if(m_unionMajorBuildVersion.dwMajorBuild < verVersion.m_unionMajorBuildVersion.dwMajorBuild)
		return -1;

	if(m_unionSubBuildVersion.dwSubBuild > verVersion.m_unionSubBuildVersion.dwSubBuild)
		return 1;
	if(m_unionSubBuildVersion.dwSubBuild > verVersion.m_unionSubBuildVersion.dwSubBuild)
		return -1;
	
	return 0;
}

bool CheckFormat(LPCWSTR lpwsFormat)
{
	if(NULL == lpwsFormat)
		return false;

	long lLength = lstrlenW(lpwsFormat);
	if(lLength <= 0)
		return false;

	long lCount = 0;
	BOOL bFlag = false;
	const WCHAR* pWork = lpwsFormat;
	do 
	{
		if(pWork[0] == L'\0')//½áÎ²
			break;

		if(pWork[0] == L'%')
		{
			bFlag = true;
		}
		else if(pWork[0] == L'd')
		{
			if(bFlag)
			{
				++lCount;
				bFlag = false;
			}
		}
		++pWork;
	} while (TRUE);

	if(lCount == 4)
		return true;

	return false;
}

bool CVersion::GetString(WCHAR* lpwszVersionString, DWORD dwSize, LPCWSTR lpwsFormat/* = NULL*/)
{
	if(NULL == lpwszVersionString || dwSize <= 0)
		return false;

	if(lpwsFormat)
	{
		if(!CheckFormat(lpwsFormat))
			return false;

		_snwprintf_s(lpwszVersionString, dwSize, dwSize, lpwsFormat, 
			m_dwMajorVersion, m_dwSubVersion, m_unionMajorBuildVersion.dwMajorBuild, m_unionSubBuildVersion.dwSubBuild);

		return true;
	}

	_snwprintf_s(lpwszVersionString, dwSize, dwSize, L"%d.%d.%d.%d", 
				 m_dwMajorVersion, m_dwSubVersion, m_unionMajorBuildVersion.dwMajorBuild, m_unionSubBuildVersion.dwSubBuild);

	return true;
}

bool CVersion::GetVersion(unsigned __int64& refI64Vesion)
{
	union _TMP
	{
		unsigned __int64 i64Version;
		WORD wzVersion[4];
	};
	_TMP tmp = {0};

	tmp.wzVersion[3] = (WORD)(m_dwMajorVersion&0x0000FFFF);
	tmp.wzVersion[2] = (WORD)(m_dwSubVersion&0x0000FFFF);
	tmp.wzVersion[1] = (WORD)(m_unionMajorBuildVersion.dwMajorBuild&0x0000FFFF);
	tmp.wzVersion[0] = (WORD)(m_unionSubBuildVersion.dwSubBuild&0x0000FFFF);

	refI64Vesion = tmp.i64Version;
	return true;
}
