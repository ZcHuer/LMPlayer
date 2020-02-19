#include "stdafx.h"
#include "ProcessHelper.h"

#include <strsafe.h>
#include <ShlObj.h>
#include <Shlwapi.h>
#include <shellapi.h>
#include <TlHelp32.h>
#include <tchar.h>

#pragma comment(lib, "shlwapi.lib")

namespace {
    DWORD GetProcessID(LPCTSTR lpAppPath)
    {
        DWORD dwProcessID = 0;
        // make snapshot of the system
        HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == NULL) {
            _ASSERT(0);
            return dwProcessID;
        }

        // get app name
        LPTSTR lpcsAppName = PathFindFileName(lpAppPath);

        // recursive to check process by app name
        PROCESSENTRY32 pe;
        ZeroMemory(&pe, sizeof(PROCESSENTRY32));
        pe.dwSize = sizeof(PROCESSENTRY32);
        ::Process32First(hSnapshot, &pe);
        do
        {
            if (_tcsicmp(pe.szExeFile, lpcsAppName) == 0)
            {
                dwProcessID = pe.th32ProcessID;
                break;
            }
        } while (::Process32Next(hSnapshot, &pe));
        ::CloseHandle(hSnapshot);
        return dwProcessID;
    }
}

namespace process {
    bool ExecuteExe(LPCTSTR lpAppPath, LPCTSTR lpcsCmd, LPCTSTR lpOperation /*= L"open"*/, bool bShow /*= true*/, bool bWaitTillComplete /*= false*/)
    {
        SHELLEXECUTEINFO info = { 0 };
        info.cbSize = sizeof(SHELLEXECUTEINFO);
        info.fMask = SEE_MASK_NOCLOSEPROCESS;
        info.lpVerb = lpOperation;
        info.lpFile = lpAppPath;
        info.lpParameters = lpcsCmd;
        TCHAR tcsDir[MAX_PATH] = { 0 };
        ::StringCbCopy(tcsDir, MAX_PATH * sizeof(TCHAR), lpAppPath);
        ::PathRemoveFileSpec(tcsDir);
        info.lpDirectory = tcsDir;
        info.nShow = bShow ? SW_SHOWNORMAL : SW_HIDE;
        BOOL bSuccess = ::ShellExecuteEx(&info);
        if (!bSuccess)
        {
            return false;
        }

        if (bWaitTillComplete) {
            ::WaitForSingleObject(info.hProcess, INFINITE);
        }
        return true;
    }

    bool IsRunning(LPCTSTR lpProcName)
    {
        HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if (hSnapshot == INVALID_HANDLE_VALUE) { return false; }

        PROCESSENTRY32 pe = { sizeof(pe) };
        BOOL fOk;

        for (fOk = ::Process32First(hSnapshot, &pe); fOk; fOk = ::Process32Next(hSnapshot, &pe))
        {
            if (!_tcscmp(pe.szExeFile, lpProcName))
            {
                ::CloseHandle(hSnapshot);
                return true;
            }
        }

        ::CloseHandle(hSnapshot);
        return false;
    }


    bool Terminate(LPCTSTR lpAppPath)
    {
        DWORD dwProcessID = GetProcessID(lpAppPath);
        for (int i = 0; i < 30 && dwProcessID != 0; i++) {
            HANDLE hProcessID = ::OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessID);
            if (hProcessID != NULL)
            {
                ::TerminateProcess(hProcessID, 0);
            }
            ::Sleep(100);
            dwProcessID = GetProcessID(lpAppPath);
        }
        return true;
    }

    // determine whether application is already running. based on mutex
    bool IsAppExist(LPCTSTR lpAppGUID)
    {
        HANDLE hMutex = ::CreateMutex(NULL, FALSE, lpAppGUID);
        int nLastErr = GetLastError();
        if (hMutex == NULL)	{
            _ASSERT(0);
            return false;
        }
        ::CloseHandle(hMutex);
        return (nLastErr == ERROR_ALREADY_EXISTS) ? true : false;
    }

    HANDLE CreateAppMutex(LPCTSTR lpAppGUID)
    {
        HANDLE hMutex;

        SECURITY_DESCRIPTOR sd;
        InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
        SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);

        SECURITY_ATTRIBUTES sa;
        ZeroMemory(&sa, sizeof sa);
        sa.bInheritHandle = FALSE;
        sa.nLength = sizeof(sa);
        sa.lpSecurityDescriptor = &sd;
        hMutex = CreateMutex(&sa, FALSE, lpAppGUID);
        if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            return NULL;
        }

        return hMutex;
    }
}

