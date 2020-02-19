#include<TlHelp32.h>
#if _MSC_VER <= 1700
#include <..\crt\src\mtdll.h>
#endif // _MSC_VER <= 1700
#include "..\3rdparty\detours\detours.h"
#pragma comment(lib, "..\\3rdparty\\detours\\lib\\detours.lib")// 相对工程目录

//#define __DEBUG_WITH_HOOK_	//detouse hook 开关
//#define _HOOK_LDRLOADDLL_		//DLL加载/卸载 监控开关
//#define _HOOK_CREATETHREAD_	//线程启动模块 监控开关
//#define _INCLUDE_				//.h文件引入声明用， 用后#undef _INCLUDE_

#ifndef _INCLUDE_
#pragma once

#ifdef __DEBUG_WITH_HOOK_

void PrintModuleList()
{
	HANDLE hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, GetCurrentProcessId() ); 
	if( hModuleSnap == INVALID_HANDLE_VALUE ) 
	{ 
		WTLTRACE(L"@@@===>>> PrintModuleList() Failed CreateToolhelp32Snapshot() return NULL");
		return ; 
	} 

	MODULEENTRY32 me32 = {0}; 
	me32.dwSize = sizeof( MODULEENTRY32 ); 
	if( !Module32First( hModuleSnap, &me32 ) ) 
	{ 
		WTLTRACE(L"@@@===>>> PrintModuleList() Failed Module32First() return FALSE");
		CloseHandle( hModuleSnap );
		return ; 
	} 

	DWORD dwIndex = 0;
	do 
	{ 
		++dwIndex;
		WTLTRACE(L"@@@===>>>PrintModuleList() Module_%d [%s][H:%#X,GC:%d,PC:%d,Path:%s]\r\n", dwIndex, me32.szModule, (DWORD)me32.hModule, me32.GlblcntUsage, me32.ProccntUsage, me32.szExePath);
	} while( Module32Next( hModuleSnap, &me32 ) ); 
	CloseHandle( hModuleSnap ); 
}

void PrintThreadList()
{
	HANDLE hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 ); 
	if( hThreadSnap == INVALID_HANDLE_VALUE ) 
	{
		WTLTRACE(L"@@@===>>> PrintThreadList() Failed CreateToolhelp32Snapshot() return NULL");
		return ;
	}

	THREADENTRY32 te32 = {0}; 
	te32.dwSize = sizeof(THREADENTRY32 ); 
	if( !Thread32First( hThreadSnap, &te32 ) ) 
	{
		WTLTRACE(L"@@@===>>> PrintThreadList() Failed Thread32First() return FALSE");
		CloseHandle( hThreadSnap );
		return ;
	}

	DWORD dwOwnerPID = GetCurrentProcessId();
	DWORD dwIndex = 0;
	do 
	{ 		
		if( te32.th32OwnerProcessID == dwOwnerPID )
		{
			++dwIndex;
			WTLTRACE(L"@@@===>>> PrintThreadList() Thread_%d : [%d/%#X]\r\n", dwIndex, te32.th32ThreadID, te32.th32ThreadID);
		}
	} while( Thread32Next(hThreadSnap, &te32 ) );

	CloseHandle( hThreadSnap );
}

//=================================================================================================
typedef HMODULE (__stdcall * PFNLoadLibraryExW)(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlag);
PFNLoadLibraryExW g_pfnLoadLibExW = (PFNLoadLibraryExW)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryExW");

HMODULE __stdcall _HookLoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlag)
{
	if(lpLibFileName)
		WTLTRACE(L"###===>>>LoadLibraryExW(%s,%s)\r\n", PathFindFileName(lpLibFileName), lpLibFileName);
	else
		WTLTRACE(L"###===>>>LoadLibraryExW(NULL, NULL)\r\n");

	return g_pfnLoadLibExW(lpLibFileName,  hFile, dwFlag);
}
typedef BOOL (WINAPI* PFNFreeLibrary)(__in HMODULE hLibModule);
PFNFreeLibrary g_pfnFreeLib = (PFNFreeLibrary)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "FreeLibrary");

BOOL WINAPI _HookFreeLibrary(HMODULE hLib)
{
	WCHAR wszDll[MAX_PATH] = {0};
	GetModuleFileName(hLib, wszDll, MAX_PATH);

	BOOL bRtn = g_pfnFreeLib(hLib);

	WTLTRACE(L"@@@===>>>FreeLibrary(%s,%#X,%s)\r\n", PathFindFileName(wszDll), (DWORD_PTR)GetModuleHandle(PathFindFileName(wszDll)), wszDll);
	return bRtn;
}

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING;

typedef UNICODE_STRING *PUNICODE_STRING;  

typedef LONG (__stdcall *PFNLdrLoadDll)(IN PWCHAR PathToFile OPTIONAL, IN ULONG Flags OPTIONAL, IN PUNICODE_STRING ModuleFileName, OUT PHANDLE ModuleHandle);

PFNLdrLoadDll g_pfnLdrLoadDll = (PFNLdrLoadDll)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "LdrLoadDll");

LONG __stdcall _HookLdrLoadDll(IN PWCHAR PathToFile OPTIONAL,  
							   IN ULONG Flags OPTIONAL,  
							   IN PUNICODE_STRING ModuleFileName,  
							   OUT PHANDLE ModuleHandle)
{
	if(ModuleFileName && ModuleFileName->Length > 0 && ModuleFileName->Buffer)
		WTLTRACE(L"###===>>>LdrLoadDll(%s,%s)\r\n", PathFindFileName(ModuleFileName->Buffer), ModuleFileName->Buffer);
	else
		WTLTRACE(L"###===>>>LdrLoadDll(NULL, NULL)\r\n");
	return g_pfnLdrLoadDll(PathToFile, Flags, ModuleFileName, ModuleHandle);
}

bool ModuleNameFromPtr(LPVOID lpvAddr, LPWSTR lpwsName, DWORD dwSize, LPVOID* lppvBaseAddress = NULL)
{
	if(NULL == lpvAddr
		|| NULL == lpwsName
		|| dwSize < MAX_PATH)
		return false;

	DWORD dwRtn = 0;
	HMODULE hModule = NULL;
	if(GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)lpvAddr, &hModule))
	{
		dwRtn = GetModuleFileName(hModule, lpwsName, dwSize);
		if(lppvBaseAddress)
			*lppvBaseAddress = (LPVOID)hModule;
	}
	return dwRtn > 0;
}

struct _ThreadParam
{
	bool bMSVCR;
	LPVOID pvStartAddress;
	LPVOID pvArg;
};

DWORD __stdcall _TreadProc(LPVOID lpvParam)
{
	UINT uRtn = 0;
	_ThreadParam* pParam = (_ThreadParam*)lpvParam;
	if(pParam)
	{
		LPTHREAD_START_ROUTINE pfnThread = (LPTHREAD_START_ROUTINE)pParam->pvStartAddress;
		uRtn = pfnThread(pParam->pvArg);
		WTLTRACE(L"@@@===>>>Thread Exit [%d/%#X] ========\r\n", GetCurrentThreadId(), GetCurrentThreadId());
		delete pParam;
	}
	return uRtn;
}

typedef HANDLE (WINAPI *PFNCreateThread)(
	__in_opt  LPSECURITY_ATTRIBUTES lpThreadAttributes,
	__in      SIZE_T dwStackSize,
	__in      LPTHREAD_START_ROUTINE lpStartAddress,
	__in_opt  LPVOID lpParameter,
	__in      DWORD dwCreationFlags,
	__out_opt LPDWORD lpThreadId
	);
PFNCreateThread g_pfnCreateThread = (PFNCreateThread)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "CreateThread");

HANDLE
WINAPI
_HookCreateThread(
				  __in_opt  LPSECURITY_ATTRIBUTES lpThreadAttributes,
				  __in      SIZE_T dwStackSize,
				  __in      LPTHREAD_START_ROUTINE lpStartAddress,
				  __in_opt  LPVOID lpParameter,
				  __in      DWORD dwCreationFlags,
				  __out_opt LPDWORD lpThreadId
				  )
{
	DWORD dwFlag = 0;
	WCHAR wszDll[MAX_PATH] = {0};
	LPVOID pvAddress = lpStartAddress;
	LPVOID pvBaseAddress = NULL;
	if(ModuleNameFromPtr(lpStartAddress, wszDll, MAX_PATH,&pvBaseAddress))
	{	
		DWORD dwVCRVer = 0;
		CharLowerW(wszDll);
		int nRtn = swscanf_s(PathFindFileName(wszDll), L"msvcr%d.dll", &dwVCRVer);
		if (nRtn != 1)
		{
			WCHAR wszBuf[64] = {0};
			nRtn = swscanf_s(PathFindFileName(wszDll), L"ucrtbase%[^.].dll", wszBuf, 60);
		}
		if (nRtn == 1 || StrCmpIW(PathFindFileName(wszDll), L"msvcrt.dll") == 0)
		{
#if _MSC_VER >= 1700
			if(lpParameter)
			{
				pvAddress = (LPVOID)(*static_cast<DWORD_PTR*>(lpParameter));
				bool bRtn = ModuleNameFromPtr(pvAddress, wszDll, MAX_PATH, &pvBaseAddress);
				if (bRtn)
				{

					dwFlag = 1;
				}
				else
				{
					dwFlag = 2;
				}
			}
			else
			{
				dwFlag = 3;
			}
#else //#if _MSC_VER >= 1700
			_ptiddata ptd = (_ptiddata)lpParameter;
			if (ptd)
			{
				bool bRtn = ModuleNameFromPtr(ptd->_initaddr, wszDll, MAX_PATH, &pvBaseAddress);
				if (bRtn)
				{
					pvAddress = ptd->_initaddr;
					dwFlag = 1;
				}
				else
				{
					dwFlag = 2;
				}
			}
			else
			{
				dwFlag = 3;
			}
#endif //#if _MSC_VER >= 1700

		}
		else
		{
			dwFlag = 4;
		}
	}
	else
	{
		dwFlag = 5;
	}
	DWORD dwTID1 = 0;
	if(NULL == lpThreadId)
		lpThreadId = &dwTID1;

	_ThreadParam* pThreadParam = NULL;
	{
		if(dwFlag == 1)//msvcr*.dll
		{
			pThreadParam = new _ThreadParam;
			pThreadParam->bMSVCR = true;
			pThreadParam->pvStartAddress = lpStartAddress;
			pThreadParam->pvArg = lpParameter;
		}
		else if(dwFlag == 4)//not msvcr*.dll
		{
			pThreadParam = new _ThreadParam;
			pThreadParam->bMSVCR = false;
			pThreadParam->pvStartAddress = lpStartAddress;
			pThreadParam->pvArg = lpParameter;
		}
	}

	HANDLE hThread = g_pfnCreateThread(lpThreadAttributes, dwStackSize, 
		pThreadParam ? _TreadProc : lpStartAddress, 
		pThreadParam ? (LPVOID)pThreadParam :lpParameter, 
		dwCreationFlags, lpThreadId);

	if(hThread)
	{	
		DWORD dwTID = lpThreadId ? *lpThreadId : 0;
		switch(dwFlag)
		{
		case 1:
			WTLTRACE(L"@@@===>>>[%s] _beginthreadex() [%d/%#X][%#X/%#X][%s]\r\n", PathFindFileName(wszDll), dwTID, dwTID, pvAddress, pvBaseAddress, wszDll);
			break;
		case 2:
			WTLTRACE(L"@@@===>>>[%s]N _beginthreadex(GetModuleNameFail) [%d/%#X][%#X/%#X][%s]\r\n", PathFindFileName(wszDll), dwTID, dwTID, pvAddress, pvBaseAddress, wszDll);
			break;
		case 3:
			WTLTRACE(L"@@@===>>>[%s]N _beginthreadex(ptd == NULL) [%d/%#X][%#X/%#X][%s]\r\n", PathFindFileName(wszDll), dwTID, dwTID, pvAddress, pvBaseAddress, wszDll);
			break;
		case 4:
			WTLTRACE(L"@@@===>>>[%s] CreateThread() [%d/%#X][%#X/%#X][%s]\r\n", PathFindFileName(wszDll), dwTID, dwTID, pvAddress, pvBaseAddress, wszDll);
			break;
		case 5:
			WTLTRACE(L"@@@===>>>Get Call CreateThread() Module Failed [%d/%#X]\r\n", dwTID, dwTID, pvAddress);
			break;
		}
	}
	else
	{
		if(pThreadParam)
			delete pThreadParam;
	}
	return hThread;
}

void InstallDetours()
{
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	LONG lr = NO_ERROR;
#ifdef _HOOK_LDRLOADDLL_
	{	
		OSVERSIONINFOEX ver = {0};
		ver.dwOSVersionInfoSize = sizeof(ver);
		if(GetVersionEx((LPOSVERSIONINFO)&ver) && ver.dwMajorVersion >= 6)
		{
			lr = DetourAttach(&(PVOID&)g_pfnLdrLoadDll, _HookLdrLoadDll);
			ATLASSERT(NO_ERROR == lr);
		}
		else
		{
			lr = DetourAttach(&(PVOID&)g_pfnLoadLibExW, _HookLoadLibraryExW);
			ATLASSERT(NO_ERROR == lr);
		}
		lr = DetourAttach(&(PVOID&)g_pfnFreeLib, _HookFreeLibrary);
		ATLASSERT(NO_ERROR == lr);
	}
#endif //#ifdef _HOOK_LDRLOADDLL_
	
#ifdef _HOOK_CREATETHREAD_
	lr = DetourAttach(&(PVOID&)g_pfnCreateThread, _HookCreateThread);
	ATLASSERT(NO_ERROR == lr);
#endif //#ifdef _HOOK_CREATETHREAD_

	DetourTransactionCommit();
}

void UninstallDetours()
{
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

#ifdef _HOOK_LDRLOADDLL_
	OSVERSIONINFOEX ver = {0};
	ver.dwOSVersionInfoSize = sizeof(ver);
	if(GetVersionEx((LPOSVERSIONINFO)&ver) && ver.dwMajorVersion >= 6)
	{
		DetourDetach(&(PVOID&)g_pfnLdrLoadDll, _HookLdrLoadDll);
	}
	else
	{
		DetourDetach(&(PVOID&)g_pfnLoadLibExW, _HookLoadLibraryExW);
	}
	DetourDetach(&(PVOID&)g_pfnFreeLib, _HookFreeLibrary);
#endif //#ifdef _HOOK_LDRLOADDLL_

#ifdef _HOOK_CREATETHREAD_
	DetourDetach(&(PVOID&)g_pfnCreateThread, _HookCreateThread);
#endif//#ifdef _HOOK_CREATETHREAD_

	DetourTransactionCommit();
}

#endif //#ifdef __DEBUG_WITH_HOOK_
#endif //#ifndef _INCLUDE_

#ifdef _INCLUDE_

extern void InstallDetours();
extern void UninstallDetours(); 
extern void PrintThreadList();
extern void PrintModuleList();

#endif //#ifdef _INCLUDE_

#ifdef __DEBUG_WITH_HOOK_
#define INITMONITOR()	InstallDetours()
#define UNINITMONITOR()	UninstallDetours()
#define PRINTTREADLIST() PrintThreadList()
#define PRINTMODULELIST() PrintModuleList()

#else //#ifdef __DEBUG_WITH_HOOK_

#define INITMONITOR()
#define UNINITMONITOR()
#define PRINTTREADLIST()
#define PRINTMODULELIST()

#endif //#ifdef __DEBUG_WITH_HOOK_