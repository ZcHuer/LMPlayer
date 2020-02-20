// ServiceTest.cpp : Defines the entry point for the application.
//
#include <stdio.h>
#include <tchar.h>
#include<iostream>
using namespace std;

#include <WinSock2.h>
#include "../SDK/Flog/FileLog.h"
#include "../include/LeTools.h"
#include"tlhelp32.h"

#include <MSCoree.h>
#include <Metahost.h>
#include <process.h>
#include <shellapi.h>
#include <TlHelp32.h>


#include <Userenv.h>
#pragma comment(lib,"Userenv.lib")
#include <Wtsapi32.h>
#pragma comment(lib,"Wtsapi32.lib")
#pragma comment(lib, "ws2_32.lib")
//#include <wininet.h>
#include <vector>
#include <atltime.h>
#pragma comment(lib, "Wininet.lib")
using namespace std;
typedef int(*pPluginThread)(wchar_t * );

/* 定义常量 */
#define HTTP_DEF_PORT        80     /* 连接的缺省端口 */
#define HTTP_BUF_SIZE      1024     /* 缓冲区的大小 */

#include "..\Include\LeReport.h"


//定义全局函数变量
void Init();
BOOL IsInstalled();
BOOL Install();
BOOL Uninstall();
void startService();
void WINAPI ServiceMain();
void WINAPI ServiceStrl(DWORD dwOrpcode);
void DoWork(bool bstartwork);
bool DoPluginDll();
void StartProcess(string sCmdline);


HANDLE DupExplorerToken();

TCHAR szServiceName[] = _T("lmpservice");
BOOL bInstall;
SERVICE_STATUS_HANDLE hServiceStatus;
SERVICE_STATUS status;
std::string g_advStr="";
HMODULE g_pPluginDll = NULL;
HINSTANCE g_hInstance;

typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

/**
* Don't use the function IsWow64Process as a static function,
* you should load it by function GetProcAddress, because
* it is not available on all version of Windows.
*/
LPFN_ISWOW64PROCESS fnIsWow64Process = NULL;

/**
* This function tells if your application is a x64 program.
*/
BOOL Isx64Application() 
{
	return (sizeof(LPFN_ISWOW64PROCESS) == 8) ? TRUE : FALSE;
}

/**
* This function tells if you're under Windows x64.
*/
BOOL IsWow64() 
{
	BOOL bIsWow64 = FALSE;
	if (!fnIsWow64Process)
		fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
	if (fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
			return FALSE;
	}		

	return bIsWow64;
}


DWORD __stdcall INTER_GetExplorerToken(OUT PHANDLE  phExplorerToken)
{

	DWORD       dwStatus = ERROR_FILE_NOT_FOUND;
	BOOL        bRet = FALSE;
	HANDLE      hProcess = NULL;
	HANDLE      hProcessSnap = NULL;
	WCHAR        szExplorerPath[MAX_PATH] = { 0 };
	WCHAR        FileName[MAX_PATH] = { 0 };
	PROCESSENTRY32 pe32 = { 0 };
	//__try
	{
		GetWindowsDirectory(szExplorerPath, MAX_PATH);
		StrCatW(szExplorerPath, L"\\explorer.exe");
		string expStr = LeTools::ws2s(szExplorerPath);
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
		{

			dwStatus = GetLastError();
			return dwStatus;
			//__leave;
		}
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (!Process32First(hProcessSnap, &pe32))
		{
			dwStatus = GetLastError();
			return dwStatus;
		}
		BOOL bres = FALSE;
		do {
			hProcess = OpenProcess(
				PROCESS_ALL_ACCESS,
				FALSE,
				pe32.th32ProcessID);
			if (NULL != hProcess)
			{
				DWORD(__stdcall *GMFNE)(HANDLE hProcess,
					HMODULE hModule,
					LPTSTR lpFilename,
					DWORD nSize);
				HMODULE hPsapi = LoadLibrary(L"PSAPI");
				if (!hPsapi)
				{
					dwStatus = GetLastError();
					break;
				}
				GMFNE = (DWORD(__stdcall *) (HANDLE, HMODULE, LPTSTR, DWORD))GetProcAddress(hPsapi, "GetModuleFileNameExW");
				if (GMFNE(hProcess, NULL, FileName, MAX_PATH))
				{
					string tmp = LeTools::ws2s(FileName);
					if (!_stricmp(expStr.c_str(), tmp.c_str()))
					{
						HANDLE  hToken;
						if (OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken))
						{
							*phExplorerToken = hToken;
							dwStatus = 0;
						}
						break;
					}
				}
				CloseHandle(hProcess);
				hProcess = NULL;
			}
			bres = Process32Next(hProcessSnap, &pe32);
		} while (bres);
	}
	//__finally
	{
		if (NULL != hProcess)
		{
			CloseHandle(hProcess);
		}
		if (NULL != hProcessSnap)
		{
			CloseHandle(hProcessSnap);
		}
	}

	return dwStatus;
}

void SetupRunPath(HINSTANCE hInstance)
{
	FLOG(_T("SetupRunPath begin"));
	// 程序路径
	CHAR chPath_Run[MAX_PATH] = { 0 };
	::GetModuleFileNameA(hInstance, chPath_Run, MAX_PATH);
	PathRemoveFileSpecA(chPath_Run);
	PathAppendA(chPath_Run, "\\LMPlayer.exe");

	HKEY hKey = NULL;
	REGSAM samDesiredOpen = KEY_ALL_ACCESS;
	// 判断系统是32位还是64位，调用不同的注册表函数
	if (IsWow64())
	{
		FLOG(_T("IsWow64"));
		samDesiredOpen |= KEY_WOW64_64KEY;
	}
	LSTATUS lRes = RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\LMPlayer", 0, "", 0, samDesiredOpen, NULL, &hKey, 0);
	if (lRes != ERROR_SUCCESS)
	{		
		FLOG(_T("SetupRunPath create failed return"));
		return;
	}
	if (RegSetValueExA(hKey, "", 0, REG_SZ, (BYTE*)chPath_Run, strlen(chPath_Run)) != ERROR_SUCCESS)
	{
		FLOG(_T("SetupRunPath set failed return"));
		return;
	}

	//RegEnableReflectionKey(HKEY_LOCAL_MACHINE);
	//RegDisableReflectionKey(HKEY_LOCAL_MACHINE);
	if (hKey)
		RegCloseKey(hKey);

	FLOG(_T("SetupRunPath end"));
	return;
}

int APIENTRY WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow)
{
	g_hInstance = hInstance;
    Init();
	
    SERVICE_TABLE_ENTRY st[] =
    {
        { szServiceName, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
        { NULL, NULL }
    };

    if (_stricmp(lpCmdLine, "/install") == 0)
    {
        BOOL bRes = Install();
		if (bRes)
		{
			startService();
			string version = "";
			LeTools::GetVersion(eService, version);
			CLeReport::GetInstance()->SetVersion(version.c_str());
			CLeReport::GetInstance()->SendRTD_Eeventsync("52", "1", "service install");

			startService();
			wstring wstr;
			LeTools::GetRunVersion(wstr);
			CLeReport::GetInstance()->SetVersion(LeTools::ws2s(wstr).c_str());

			wstring wChannelID;
			LeTools::GetChannelID(wChannelID);
			CLeReport::GetInstance()->SetchannelID(LeTools::ws2s(wChannelID).c_str());
			CLeReport::GetInstance()->SendRTD_Eeventsync("52", "1", "service install");
			LeTools::SetInstallTime();
		}
    }
    else if (_stricmp(lpCmdLine, "/uninstall") == 0)
    {
        Uninstall();
		string version = "";
		LeTools::GetVersion(eService, version);
		CLeReport::GetInstance()->SetVersion(version.c_str());
		CLeReport::GetInstance()->SendRTD_Eeventsync("53", "1", "service uninstall");
    }
    else if (_stricmp(lpCmdLine, "/start") == 0)
    {
        startService();
    }
    else
    {
        if (!::StartServiceCtrlDispatcher(st))
        {
            FLOG(_T("Register Service Main Function Error!"));
        }
    }

    return 0;
}
//*********************************************************
//Functiopn:            Init
//Description:            初始化
//Calls:                main
//Called By:                
//Table Accessed:                
//Table Updated:                
//Input:                
//Output:                
//Return:                
//Others:                
//History:                
//            <author>niying <time>2006-8-10        <version>        <desc>
//*********************************************************
void Init()
{
    hServiceStatus = NULL;
    status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    status.dwCurrentState = SERVICE_STOPPED;
    status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    status.dwWin32ExitCode = 0;
    status.dwServiceSpecificExitCode = 0;
    status.dwCheckPoint = 0;
    status.dwWaitHint = 0;

    TCHAR buff[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, buff, sizeof(buff));
    wstring wslog = buff;
    wslog.append(L".log");
    LOGINIT(wslog.c_str());

	string strChannel;
	LeTools::GetChannelID(eService, strChannel);
	CLeReport::GetInstance()->SetchannelID(strChannel);
}

/**************************************************************************
*
* 函数功能: 向客户端发送 HTTP 响应.
*
* 参数说明: [IN]  buf, 字符串指针数组;
*           [IN]  buf_len, buf 的长度;
*
* 返 回 值: 成功返回非0, 失败返回0.
*
**************************************************************************/
BOOL http_send_response(SOCKET soc, string sSend)
{
    char *http_res_hdr_tmpl = "HTTP/1.1 200 OK\r\nServer: Lenove Local Server <0.1>\r\n"
        "Accept-Ranges: bytes\r\nContent-Length: %d\r\nConnection: close\r\n"
        "Content-Type: text/html\r\n\r\n";

    int hdr_len = 0, send_len = 0;
    char read_buf[HTTP_BUF_SIZE] = { 0 };
    char http_header[HTTP_BUF_SIZE] = { 0 };
    //char cSend[] = "{\"resultCode\":\"0000\",\"resultDesc\":\"already installed\"}";

    /* 构造 HTTP 首部，并发送 */
    hdr_len = sprintf_s(http_header, http_res_hdr_tmpl, sSend.length());
    send_len = send(soc, http_header, hdr_len, 0);
    if (send_len == SOCKET_ERROR)
    {
        FLOG(L"[Web] Fail to send, error = %d\n", WSAGetLastError());
        return FALSE;
    }
    /* 发送文件, HTTP 的消息体 */
    send_len = send(soc, sSend.c_str(), sSend.length(), 0);
    if (send_len == SOCKET_ERROR)
    {
        FLOG(L"[Web] Fail to send, error = %d\n", WSAGetLastError());
        return FALSE;
    }
    return TRUE;
}

//*********************************************************
//Functiopn:            ServiceMain
//Description:            服务主函数，这在里进行控制对服务控制的注册
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:
//Output:
//Return:
//Others:
//History:
//            <author>niying <time>2006-8-10        <version>        <desc>
//*********************************************************
void WINAPI ServiceMain()
{
    FLOG(L"Service start");
    // Register the control request handler
    status.dwCurrentState = SERVICE_START_PENDING;
    status.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    //注册服务控制
    hServiceStatus = RegisterServiceCtrlHandler(szServiceName, ServiceStrl);
    if (hServiceStatus == NULL)
    {
        FLOG(_T("Handler not installed"));
        return;
    }

	bool bres = DoPluginDll();

    SetServiceStatus(hServiceStatus, &status);

    status.dwWin32ExitCode = S_OK;
    status.dwCheckPoint = 0;
    status.dwWaitHint = 0;
    status.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(hServiceStatus, &status);
    
    DoWork(bres);

    status.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(hServiceStatus, &status);
    FLOG(_T("Service stopped"));
}

void StartProcess(string sCmdline)
{
    HANDLE hTokenDup = DupExplorerToken();
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
    si.cb = sizeof(STARTUPINFO);
    si.lpDesktop = _T("WinSta0\\Default");
    si.wShowWindow = SW_SHOW;
    si.dwFlags = STARTF_USESHOWWINDOW;

    LPVOID pEnv = NULL;
    DWORD dwCreationFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE |CREATE_UNICODE_ENVIRONMENT;
    
    if (!CreateEnvironmentBlock(&pEnv, hTokenDup, FALSE)) {
        CloseHandle(hTokenDup);
        return;
    }
    
    FLOG(LeTools::s2ws(sCmdline).c_str());
    WCHAR wPath[_MAX_PATH] = { 0 };
    GetModuleFileName(NULL, wPath, _MAX_PATH);
    wstring wsPath = wPath;
    wsPath = wsPath.substr(0,wsPath.rfind(L"\\")+1);
    wsPath.append(L"lmp.exe");

    wstring wsCmd = L" ";
    wsCmd.append(LeTools::s2ws(sCmdline));
    BOOL bOK = CreateProcessAsUser(hTokenDup, wsPath.c_str() ,
        (WCHAR*)wsCmd.c_str(),
        NULL, NULL, FALSE, dwCreationFlag, NULL, NULL, &si, &pi);

    if (pEnv)
        DestroyEnvironmentBlock(pEnv);

    if (hTokenDup != NULL && hTokenDup != INVALID_HANDLE_VALUE)
        CloseHandle(hTokenDup);
}
//通过执行文件名获得进程ID的方法
DWORD GetProcessIdFromName(LPCTSTR name)    
{
    PROCESSENTRY32 pe;
    DWORD id = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hSnapshot, &pe))
        return 0;
    do
    {
        pe.dwSize = sizeof(PROCESSENTRY32);
        if (Process32Next(hSnapshot, &pe) == FALSE)
            break;
        if (_wcsicmp(pe.szExeFile,name) == 0)
        {
            id = pe.th32ProcessID;
            break;
        }
    } while (1);
    CloseHandle(hSnapshot);
    return id;
}

HANDLE DupExplorerToken()
{
    DWORD dwPid = GetProcessIdFromName(_T("explorer.exe"));
    HANDLE hExplorer = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPid);
    if (hExplorer == NULL)
        return NULL;

    HANDLE hToken = NULL;
    OpenProcessToken(hExplorer, TOKEN_DUPLICATE, &hToken);
    CloseHandle(hExplorer);

    HANDLE hNewToken = NULL;
    DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, NULL, SecurityImpersonation, TokenPrimary, &hNewToken);
    CloseHandle(hToken);

    return hNewToken;
}

//拉起广告弹窗
bool DoPluginDll()
{
	FLOG(_T("DoPluginDll begin"));
	bool bres = false;
	WCHAR wcPath_Daemon_Old[MAX_PATH] = {0};	
	::GetModuleFileNameW(g_hInstance, wcPath_Daemon_Old, MAX_PATH);
	PathRemoveFileSpecW(wcPath_Daemon_Old);
	PathAppendW(wcPath_Daemon_Old, L"\\run\\LeDo.dll");
	FLOG(_T("wcPath_Daemon_Old ：%ws"), wcPath_Daemon_Old);

	WCHAR wcPath_Daemon_New[MAX_PATH] = { 0 };
	::GetModuleFileNameW(g_hInstance, wcPath_Daemon_New, MAX_PATH);
	PathRemoveFileSpecW(wcPath_Daemon_New);
	wstring wcupath = wcPath_Daemon_New;
	PathAppendW(wcPath_Daemon_New, L"\\LeDo.dll");
	FLOG(_T("wcPath_Daemon_New ：%ws"), wcPath_Daemon_New);

	
	WCHAR wcPath_Run_New[MAX_PATH] = { 0 };
	::GetModuleFileNameW(g_hInstance, wcPath_Run_New, MAX_PATH);
	PathRemoveFileSpecW(wcPath_Run_New);
	PathAppendW(wcPath_Run_New, L"\\run");
	::CreateDirectory(wcPath_Run_New, NULL);

	FLOG(_T("wcPath_Daemon_Old ：%ws"), wcPath_Daemon_Old);
	if (PathFileExists(wcPath_Daemon_Old))
	{
		FLOG(L"Copy Daemon");
		::DeleteFile(wcPath_Daemon_New);
		::CopyFile(wcPath_Daemon_Old, wcPath_Daemon_New, FALSE);
	}
	Sleep(1000);
	//拷贝CEF内核

	g_pPluginDll = LoadLibrary(wcPath_Daemon_New);
	if (g_pPluginDll != NULL)
	{
		FLOG(_T("g_pPluginDll 成功"));
		pPluginThread pPluginFunction = (pPluginThread)GetProcAddress(g_pPluginDll, "fnLeDaemonWork");
		if (pPluginFunction)
		{
			FLOG(_T("begin function %s"), wcPath_Daemon_New);
			bres = pPluginFunction((wchar_t *)wcupath.c_str());
		}
	}
	FLOG(_T("DoPluginDll end"));
	return bres;
}

void DoWork(bool bstartwork)
{
	FLOG(_T("DoWork begin"));
	while (bstartwork)
	{
		Sleep(5000);
	}

	if (g_pPluginDll)
	{
		FreeLibrary(g_pPluginDll);
	}

	FLOG(_T("DoWork end"));
}

//*********************************************************
//Functiopn:            ServiceStrl
//Description:            服务控制主函数，这里实现对服务的控制，
//                        当在服务管理器上停止或其它操作时，将会运行此处代码
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:                dwOpcode：控制服务的状态
//Output:
//Return:
//Others:
//History:
//            <author>niying <time>2006-8-10        <version>        <desc>
//*********************************************************
void WINAPI ServiceStrl(DWORD dwOpcode)
{
    switch (dwOpcode)
    {
    case SERVICE_CONTROL_STOP:
        status.dwCurrentState = SERVICE_STOP_PENDING;
        SetServiceStatus(hServiceStatus, &status);
        exit(0);
        break;
    case SERVICE_CONTROL_PAUSE:
        break;
    case SERVICE_CONTROL_CONTINUE:
        break;
    case SERVICE_CONTROL_INTERROGATE:
        break;
    case SERVICE_CONTROL_SHUTDOWN:
        break;
    default:
        FLOG(_T("Bad service request"));
    }
}
//*********************************************************
//Functiopn:            IsInstalled
//Description:            判断服务是否已经被安装
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:
//Output:
//Return:
//Others:
//History:
//            <author>niying <time>2006-8-10        <version>        <desc>
//*********************************************************
BOOL IsInstalled()
{
    BOOL bResult = FALSE;

    //打开服务控制管理器
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (hSCM != NULL)
    {
        //打开服务
        SC_HANDLE hService = ::OpenService(hSCM, szServiceName, SERVICE_QUERY_CONFIG);
        if (hService != NULL)
        {
            bResult = TRUE;
            ::CloseServiceHandle(hService);
        }
        ::CloseServiceHandle(hSCM);
    }
    return bResult;
}

//*********************************************************
//Functiopn:            Install
//Description:            安装服务函数
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:
//Output:
//Return:
//Others:
//History:
//            <author>niying <time>2006-8-10        <version>        <desc>
//*********************************************************
BOOL Install()
{
	FLOG(_T("Install begin"));	

    if (IsInstalled())
        Uninstall();

    //打开服务控制管理器
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM == NULL)
    {
        //MessageBox(NULL, _T("Couldn't open service manager"), szServiceName, MB_OK);
		FLOG(_T("Install Couldn't open service manager"));
        return FALSE;
    }

    // Get the executable file path
    TCHAR szFilePath[MAX_PATH];
    ::GetModuleFileName(NULL, szFilePath, MAX_PATH);

    //创建服务
    SC_HANDLE hService = ::CreateService(
        hSCM, szServiceName, szServiceName,
        SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
        SERVICE_AUTO_START, NULL,
        szFilePath, NULL, NULL, _T(""), NULL, NULL);

    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
        //MessageBox(NULL, _T("Couldn't create service"), szServiceName, MB_OK);
		FLOG(_T("Install Couldn't CreateService"));
        return FALSE;
    }

    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);

	FLOG(_T("Install end"));

    return TRUE;
}

void startService()
{
	FLOG(_T("startService begin"));

    auto scmHandle = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (scmHandle == NULL) 
		return;

    auto serviceHandle = OpenServiceW(scmHandle,
        szServiceName,
        SERVICE_ALL_ACCESS);

    if (scmHandle == NULL) 
		return;

    SERVICE_STATUS serviceStatus;
    if (QueryServiceStatus(serviceHandle, &serviceStatus) == 0)
        return;
    if (serviceStatus.dwCurrentState == SERVICE_START && serviceStatus.dwCurrentState != SERVICE_START_PENDING)
        return;

	SetupRunPath(g_hInstance);

    if (StartServiceW(serviceHandle, 0, NULL) == FALSE)
        return;

    CloseServiceHandle(scmHandle);
    CloseServiceHandle(serviceHandle);
	FLOG(_T("startService end"));
}

//*********************************************************
//Functiopn:            Uninstall
//Description:            删除服务函数
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:
//Output:
//Return:
//Others:
//History:
//            <author>niying <time>2006-8-10        <version>        <desc>
//*********************************************************
BOOL Uninstall()
{
	FLOG(_T("Uninstall begin"));
    if (!IsInstalled())
        return TRUE;

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (hSCM == NULL)
    {
        //MessageBox(NULL, _T("Couldn't open service manager"), szServiceName, MB_OK);
		FLOG(L"Uninstall Couldn't open service manager");
        return FALSE;
    }

    SC_HANDLE hService = ::OpenService(hSCM, szServiceName, SERVICE_STOP | DELETE);

    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
        //MessageBox(NULL, _T("Couldn't open service"), szServiceName, MB_OK);
		FLOG(L"Uninstall Couldn't open service");
        return FALSE;
    }
    SERVICE_STATUS status;
    ::ControlService(hService, SERVICE_CONTROL_STOP, &status);

    //删除服务
    BOOL bDelete = ::DeleteService(hService);
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);

    if (bDelete)
        return TRUE;

    FLOG(_T("Service could not be deleted"));
	FLOG(_T("Uninstall end"));
    return FALSE;
}
