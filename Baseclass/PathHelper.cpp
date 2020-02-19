#include "stdafx.h"
#include "PathHelper.h"
#include <assert.h>
#include <Shlwapi.h>
#include <strsafe.h>
#include <tchar.h>
#include <ShlObj.h>


bool PathHelper::GetRegString(HKEY hKey, LPCWSTR lpwsPath, LPCWSTR lpwsKey, CString& strValue)
{
	DWORD dwSize = 0;
	DWORD dwType = REG_NONE;
	LSTATUS ls = SHGetValue(hKey, lpwsPath, lpwsKey, &dwType, NULL, &dwSize);
	if (dwType != REG_SZ)
		return false;

	if (dwSize == 0)
		return true;

	dwSize += sizeof(WCHAR) * 2;
	ls = SHGetValue(hKey, lpwsPath, lpwsKey, &dwType, (LPVOID)strValue.GetBufferSetLength(dwSize), &dwSize);
	strValue.ReleaseBuffer();

	return ERROR_SUCCESS == ls;
}

bool PathHelper::GetInstallPathFile(CString& strFile, LPCWSTR lpwsFile)
{
	CString strInstallPath;
	CString strProductVer;
	if (GetRegString(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\51talkim", L"InstallPath", strInstallPath)
		&& GetRegString(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\51talkim", L"DisplayVersion", strProductVer))
	{
		PathRemoveBackslash(strInstallPath.GetBufferSetLength(MAX_PATH));
		strInstallPath.ReleaseBuffer();

		strFile.Format(L"%s\\%s", strInstallPath, strProductVer);
		if (PathFileExists(strFile))
		{
			if (NULL != lpwsFile)
			{
				strFile.Append(L"\\");
				strFile.Append(lpwsFile);
				if (PathFileExists(strFile))
					return true;
			}
			else
			{
				return true;
			}
		}
	}
	return false;
}

bool PathHelper::GetDisplayVersion(CString& strValue)
{
	if(GetRegString(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\51talkim", L"DisplayVersion", strValue))
	{
		return true;
	}

	return false;
}