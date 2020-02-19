#pragma once
#include <string>
#include <windows.h>
#include <strsafe.h>
#pragma warning(push)
#pragma warning(disable : 4091)
#include <DbgHelp.h>
#include <shlobj.h>
#pragma warning(pop)

#pragma comment(lib, "dbghelp.lib")

//#define _USE_EHa_

#ifndef MKDIREXIST
#define MKDIREXIST(x)
#pragma message(__FILE__ " warnning: MKDIREXIST(x) Not Defined, Used by SetDumpFileName(...)\r\n")
#endif //#ifndef MKDIREXIST

#ifdef _USE_EHa_
void cdecl _CB_MapSEIntoCPPExceptions(UINT uCode, _EXCEPTION_POINTERS* pExcPointers);
#endif //#ifdef _USE_EHa_

LONG WINAPI _CB_UnhandledExceptionFilter(_EXCEPTION_POINTERS* lpExceptionInfo);

typedef DWORD (WINAPI *PFNCrashNotify)(LPCWSTR lpwsDumpFile, DWORD_PTR dwUserData);

class CMiniDump
{
public:
	CMiniDump(LPCWSTR lpwsSubpath, LPCWSTR lpwsPrefix = NULL)
	{
		CMiniDump::SetDumpFileName(lpwsPrefix, NULL, lpwsSubpath);
		CMiniDump::EnableTrackException(true);
	}
	~CMiniDump()
	{
		//CMiniDump::EnableTrackException(false);
	}
private:
#ifdef _USE_EHa_
	static _se_translator_function s_pSysSEHTrans;
#endif//#ifdef _USE_EHa_
	static LPTOP_LEVEL_EXCEPTION_FILTER s_pSysSEHFilter;
	static _EXCEPTION_POINTERS*	 s_pExcPoints;
	static std::wstring s_strDumpPath;
	static std::wstring s_strDumpPrefix;
	static PFNCrashNotify s_pfnNotify;
	static DWORD_PTR s_dwUserData;

	friend void cdecl _CB_MapSEIntoCPPExceptions(UINT uCode, _EXCEPTION_POINTERS* pExcPointers);
	friend LONG WINAPI _CB_UnhandledExceptionFilter(_EXCEPTION_POINTERS* lpExceptionInfo);

public:
	static BOOL SetDumpFileName(LPCWSTR lpwsPrefixName, LPCWSTR lpwsPath, LPCWSTR lpwsSubPath)
	{
		WCHAR wszBuf[MAX_PATH] = {0};
		if (NULL == lpwsPath)
			return FALSE;
		//	SHGetSpecialFolderPath(NULL, wszBuf, CSIDL_APPDATA, TRUE);
		//else
			StringCchCopy(wszBuf, MAX_PATH, lpwsPath);
		if(lpwsSubPath)
			PathAppend(wszBuf, lpwsSubPath);
		
		PathRemoveBackslash(wszBuf);
	
		MKDIREXIST(wszBuf);
		
		s_strDumpPath = wszBuf;
		if(NULL == lpwsPrefixName)
		{
			GetModuleFileName(NULL, wszBuf, MAX_PATH);
			PathRemoveExtension(wszBuf);
			lpwsPrefixName = PathFindFileName(wszBuf);
		}
		ATLASSERT(lpwsPrefixName);
		s_strDumpPrefix = lpwsPrefixName;
		return PathFileExists(s_strDumpPath.c_str());
	}

	static BOOL SetCrashNotify(PFNCrashNotify pfnNotify, DWORD_PTR dwUserData)
	{
		s_pfnNotify = pfnNotify;
		s_dwUserData = dwUserData;
		return TRUE;
	}

	static BOOL DumpExceptionInfoFile(struct _EXCEPTION_POINTERS *lpExceptionInfo)
	{
		if (lpExceptionInfo == NULL)
		{
			return FALSE;
		}

		OSVERSIONINFO osvi;
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx (&osvi);

		if(osvi.dwMajorVersion < 5 || (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion < 1))
			return FALSE;

		SYSTEMTIME st = {0};
		GetLocalTime(&st);

		MKDIREXIST(CMiniDump::s_strDumpPath.c_str());
		WCHAR wszDump[MAX_PATH] = {0};
		StringCchPrintf(wszDump, MAX_PATH, L"%s\\%s_%04d%02d%02d_%02d%02d%02d.dmp", 
			s_strDumpPath.c_str(), s_strDumpPrefix.c_str(), 
			st.wYear, st.wMonth, st.wDay, 
			st.wHour, st.wMinute, st.wSecond);

		HANDLE hFile = CreateFile(wszDump, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			return FALSE;
		
		MINIDUMP_EXCEPTION_INFORMATION ExInfo;
		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = lpExceptionInfo;
		ExInfo.ClientPointers = false;

		BOOL bOK = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL);
		::CloseHandle(hFile);

		if(s_pfnNotify)
			s_pfnNotify(wszDump, s_dwUserData);
		return bOK;
	}
#ifdef _USE_EHa_
	static void cdecl __CB_MapSEIntoCPPExceptions(UINT uCode, _EXCEPTION_POINTERS* pExcPointers)
	{
		static bool bFirstSEH = true;
		if (bFirstSEH)
		{
			DumpExceptionInfoFile(pExcPointers);
			bFirstSEH = false;
		}
		s_pExcPoints = pExcPointers;
	}
#endif//#ifdef _USE_EHa_
	static LONG WINAPI __CB_UnhandledExceptionFilter(_EXCEPTION_POINTERS* lpExceptionInfo)
	{
		if (s_pExcPoints != lpExceptionInfo)
		{
			DumpExceptionInfoFile(lpExceptionInfo);
		}
		return EXCEPTION_EXECUTE_HANDLER;
	}

	static void DisableSetUnhandledExceptionFilter()
	{
		void* addr = (void*)GetProcAddress(LoadLibrary(L"kernel32.dll"),
			"SetUnhandledExceptionFilter");

		if (addr)
		{
			int size = 0;
			unsigned char code[] = { 0x33, 0xC0, 0xC2, 0x04, 0x00 };//xor eax,eax ; ret ; int 3 ; int 3

			DWORD dwOldFlag, dwTempFlag;
			VirtualProtect(addr, size, PAGE_READWRITE, &dwOldFlag);
			WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL);
			VirtualProtect(addr, size, dwOldFlag, &dwTempFlag);
		}
	}

	static bool EnableTrackException(bool bEnable)
	{
		if (bEnable == true)
		{
#ifdef _USE_EHa_
			if (s_pSysSEHTrans == NULL)
			{
				s_pSysSEHTrans = _set_se_translator(_CB_MapSEIntoCPPExceptions);
			}
			else
			{
				_set_se_translator(_CB_MapSEIntoCPPExceptions);
			}
#endif //#ifdef _USE_EHa_
			if (s_pSysSEHFilter == NULL)
			{
				s_pSysSEHFilter = ::SetUnhandledExceptionFilter(_CB_UnhandledExceptionFilter);
			}
			else
			{
				::SetUnhandledExceptionFilter(_CB_UnhandledExceptionFilter);
			}
			DisableSetUnhandledExceptionFilter();
			//_set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
		}
		else
		{
#ifdef _USE_EHa_
			_set_se_translator(s_pSysSEHTrans);
#endif//#ifdef _USE_EHa_
			::SetUnhandledExceptionFilter(s_pSysSEHFilter);
		}
		return true;
	}
};
#ifdef _USE_EHa_
_se_translator_function CMiniDump::s_pSysSEHTrans = NULL;
#endif //#ifdef _USE_EHa_

LPTOP_LEVEL_EXCEPTION_FILTER CMiniDump::s_pSysSEHFilter = NULL;
_EXCEPTION_POINTERS*	CMiniDump::s_pExcPoints = NULL;
std::wstring CMiniDump::s_strDumpPath;
std::wstring CMiniDump::s_strDumpPrefix;
PFNCrashNotify CMiniDump::s_pfnNotify = NULL;
DWORD_PTR CMiniDump::s_dwUserData = 0;

#ifdef _USE_EHa_
void cdecl _CB_MapSEIntoCPPExceptions(UINT uCode, _EXCEPTION_POINTERS* pExcPointers)
{
	CMiniDump::__CB_MapSEIntoCPPExceptions(uCode, pExcPointers);
}
#endif //#ifdef _USE_EHa_
LONG WINAPI _CB_UnhandledExceptionFilter(_EXCEPTION_POINTERS* lpExceptionInfo)
{
	return CMiniDump::__CB_UnhandledExceptionFilter(lpExceptionInfo);
}

#define MINIDUMP(p,f) CMiniDump __dump(p,f)