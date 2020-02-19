#ifndef __PATH_HELPER_H__
#define __PATH_HELPER_H__

#include <string>
#include <windows.h>

#pragma once

class PathHelper {
public:
	PathHelper() {}
    ~PathHelper() {}

public:
	static bool GetRegString(HKEY hKey, LPCWSTR lpwsPath, LPCWSTR lpwsKey, CString& strValue);
	static bool GetInstallPathFile(CString& strFile, LPCWSTR lpwsFile = NULL);
	static bool GetDisplayVersion(CString& strValue);

};

#endif
