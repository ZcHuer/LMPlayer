#include "stdafx.h"
#include "RemoteCtrl.h"
#include <atltime.h>
#include "../include/LeTools.h"
#include "../include/json/json.h"
#include "../SDK/http/Http.h"
#include "../SDK/Flog/FileLog.h"
#include <ShellAPI.h>
#include "UserToken.h"

using namespace LeTools;
using namespace LeConnect;

CRemoteCtrl::CRemoteCtrl()
{
	m_sConfig.b_genShortcut = false;
	m_sConfig.b_genStartMenu = false;
	m_sConfig.b_runDelayMinutes = false;
	m_sConfig.b_runOnStartup = false;
	m_sConfig.n_exeStartTime = 0;
	m_sConfig.n_exeEndTime = 0;
	m_sConfig.n_runDelayMinutes = -1;
	m_sConfig.ninactiveThreshold = 0;
	m_sConfig.ninactiveThresholdForDelay = 0;
	m_sConfig.npollingInterval = 60;
	
	//获取Config文件路径
	char path[MAX_PATH] = { 0 };
	::GetModuleFileNameA(NULL, path, MAX_PATH);
	::PathRemoveFileSpecA(path);
	PathAppendA(path, "\\Config.ini");
	m_str_Config_File = path;
	return;
}

CRemoteCtrl::~CRemoteCtrl()
{
}

void CRemoteCtrl::RemoteCtrl()
{
	FLOG(L"CRemoteCtrl::RemoteCtrl begin");
	// 从服务端获取配置，获取失败则轮询获取，直到成功
	if (GetConfigFromServer())
	{
		FLOG(L"CRemoteCtrl::GetConfigFromServer success");
		// 轮询时间条件是否满足
		while (true)
		{
			CTime ctm = CTime::GetCurrentTime();
			int nTime = ctm.GetHour() * 60 + ctm.GetMinute();
			FLOG(L"CRemoteCtrl::%d,%d,%d", nTime, m_sConfig.n_exeStartTime, m_sConfig.n_exeEndTime);
			if (nTime > m_sConfig.n_exeStartTime && nTime < m_sConfig.n_exeEndTime)
			{
				FLOG(L"CRemoteCtrl::do1:%d,%d", GetDaysOfLastRun(), m_sConfig.ninactiveThreshold);
				if (GetDaysOfLastRun() > m_sConfig.ninactiveThreshold)
				{
					HANDLE hToken = NULL;
					if (!GetTokenByName(hToken, _T("EXPLORER.EXE")))
						return;

					// 模拟登录用户的安全上下文
					ImpersonateLoggedOnUser(hToken);

					//获取用户名
					TCHAR szUsername[MAX_PATH];
					DWORD dwUsernameLen = MAX_PATH;
					GetUserName(szUsername, &dwUsernameLen);

					// 桌面快捷方式
					if (m_sConfig.b_genShortcut)
					{
						bool bR = LeTools::CreateDeskShortcut(eService);
						FLOG(L"LeTools::CreateDeskShortcut %d", bR);
					}
					// 开始菜单
					if (m_sConfig.b_genStartMenu)
					{
						bool bR = LeTools::CreatLinkToStartMenu(eService);
						FLOG(L"LeTools::CreatLinkToStartMenu %d", bR);
					}
					// 开机自启动
					if (m_sConfig.b_runOnStartup)
					{
						SetAutoRunAsExe();
					}
					//到这里已经模拟完了，别忘记返回原来的安全上下文
					RevertToSelf();
				}
				FLOG(L"CRemoteCtrl::do2:%d,%d", GetDaysOfLastRun(), m_sConfig.ninactiveThresholdForDelay);
				if (GetDaysOfLastRun() > m_sConfig.ninactiveThresholdForDelay)
				{
					// 延时启动
					if (m_sConfig.b_runDelayMinutes)
					{
						int nCount = 0;
						do
						{
							if (nCount >= m_sConfig.n_runDelayMinutes)
							{
								// 启动播放器，只是隐藏在托盘图标区域
								wchar_t currentPath[MAX_PATH] = { 0 };
								TCHAR buff[MAX_PATH] = { 0 };
								GetModuleFileName(NULL, buff, sizeof(buff));
								::PathRemoveFileSpecW(buff);
								StrNCpy(currentPath, buff, MAX_PATH);

								wchar_t cmdline[MAX_PATH] = { 0 };
								StrNCpy(cmdline, currentPath, MAX_PATH);
								PathAppendW(cmdline, _T("\\LMPlayer.exe"));
								
								HANDLE hPtoken = NULL;
								INTER_GetExplorerToken(&hPtoken);
								PROCESS_INFORMATION pi;
								STARTUPINFOA si = { sizeof(STARTUPINFO),NULL,"",NULL,0,0,0,0,0,0,0,STARTF_USESHOWWINDOW,0,0,NULL,0,0,0 };
								si.wShowWindow = SW_SHOW;
								si.lpDesktop = NULL;
								LPVOID pEnv = NULL;
								CreateEnvironmentBlock(&pEnv, hPtoken, FALSE);
								ZeroMemory(&pi, sizeof(pi));
								FLOG(_T("%s"), currentPath);
								FLOG(_T("%s"), cmdline);
								CreateProcessAsUserA(hPtoken, LeTools::ws2s(cmdline).c_str(), (LPSTR)" /AutoRun-Service", NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT, pEnv, LeTools::ws2s(currentPath).c_str(), &si, &pi);
								FLOG(L"CreateProcessAsUserA /AutoRun-Service");
								break;
							}
							nCount++;
							// 睡一分钟
							Sleep(60 * 1000);
						} while (1);
					}
				}

				break;
			}

			Sleep(60 * 1000);
		}
	}
	return;
}

bool CRemoteCtrl::GetConfigFromServer()
{	
	FLOG(L"CRemoteCtrl::GetConfigFromServer begin");
	bool bSucc = false;
	wstring wsOS = L"";
	wstring version = _T("");
	LeTools::GetVersion(eService, version);

	wstring wsUrl = L"http://api.17tv.com/statistics/installConfig?";
	wsUrl.append(L"version=");
	wsUrl.append(version.c_str());
	wsUrl.append(L"&model=");
	wsUrl.append(wsOS.c_str());
	wsUrl.append(L"&deviceId=");
	string strMac = LeTools::GetMacString();
	wsUrl.append(LeTools::s2ws(strMac));
	
	do 
	{
		HttpRequest req(wsUrl.c_str(), NULL, NULL);
		FLOG(L"wsUrl:%s", wsUrl.c_str());
		bSucc = req.doGet();
		if (bSucc)
		{
			string sData = req.getStream().str();
			sData = LeTools::ConvertUtf8ToGBK(sData);
			Json::Reader jr;
			Json::Value jvData;
			if (jr.parse(sData, jvData))
			{
				FLOG(L"GetConfig:%s", LeTools::s2ws(sData).c_str());
				if ("200" == jvData["code"].asString())
				{
					Json::Value DataArray = jvData["data"];
					if (0 == DataArray.size())
						return false;

					string exeEndTime = jvData["data"]["exeEndTime"].asString();
					string exeStartTime = jvData["data"]["exeStartTime"].asString();
					if (exeEndTime.length() == 0 || exeStartTime.length() == 0)
						return false;

					m_sConfig.n_exeStartTime = CalcTime(exeStartTime);
					m_sConfig.n_exeEndTime = CalcTime(exeEndTime);
					m_sConfig.b_genShortcut = jvData["data"]["genShortcut"].asBool();
					m_sConfig.b_genStartMenu = jvData["data"]["genStartMenu"].asBool();
					m_sConfig.b_runOnStartup = jvData["data"]["runOnStartup"].asBool();
					m_sConfig.n_runDelayMinutes = jvData["data"]["runDelayMinutes"].asInt();
					m_sConfig.b_runDelayMinutes = (m_sConfig.n_runDelayMinutes >= 0) ? true : false;
					m_sConfig.npollingInterval = jvData["data"]["pollingInterval"].asInt();
					m_sConfig.ninactiveThreshold = jvData["data"]["inactiveThreshold"].asInt();
					m_sConfig.ninactiveThresholdForDelay = jvData["data"]["inactiveThresholdForDelay"].asInt();

					return true;
				}
			}
		}
		// 睡觉
		Sleep(m_sConfig.npollingInterval * 60 * 1000);
	} while (1);
	FLOG(L"CRemoteCtrl::GetConfigFromServer end");
}

int CRemoteCtrl::GetDaysOfLastRun()
{
	SYSTEMTIME tm;
	::GetSystemTime(&tm);
	
	int nDate_Now = tm.wYear * 360 + tm.wMonth * 30 + tm.wDay;
	int nDate_Run = GetPrivateProfileIntA("service", "runtime", 0, m_str_Config_File.c_str());
	if (0 == nDate_Run)
	{
		SetRunTime();
		nDate_Run = nDate_Now;
	}
	return max(nDate_Now - nDate_Run, 0);
}

void CRemoteCtrl::SetRunTime()
{
	SYSTEMTIME tm;
	::GetSystemTime(&tm);
	int nDate_Now = tm.wYear * 360 + tm.wMonth * 30 + tm.wDay;
	string strTime = LeTools::Num2Str(nDate_Now);
	WritePrivateProfileStringA("service", "runtime", strTime.c_str(), m_str_Config_File.c_str());

	return;
}

int CRemoteCtrl::CalcTime(string strTm)
{
	int nHour = 0;
	int nMin = 0;
	int nPos = strTm.find(":");
	if (-1 != nPos)
	{
		string strTmp = strTm.substr(0, nPos);
		nHour = LeTools::Str2Numer(strTmp);
		strTmp = strTm.substr(nPos+1, strTm.length() - nPos - 1);
		nMin = LeTools::Str2Numer(strTmp);
		return nHour * 60 + nMin;
	}
	return 0;
}

DWORD CRemoteCtrl::INTER_GetExplorerToken(OUT PHANDLE phExplorerToken)
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
		//StrCatW(szExplorerPath, L"\\explorer.exe");
		//StrNCatW(szExplorerPath, L"\\explorer.exe", MAX_PATH);
		StringCchCat(szExplorerPath, MAX_PATH, L"\\explorer.exe");
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

void CRemoteCtrl::SetAutoRunAsExe()
{
	TCHAR buff[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buff, sizeof(buff));
	::PathRemoveFileSpecW(buff);
	wstring wstrPath = buff;
	PathAppendW(buff, _T("\\LePopAdSe.exe"));

	HANDLE hPtoken = NULL;
	INTER_GetExplorerToken(&hPtoken);
	PROCESS_INFORMATION pi;
	STARTUPINFOA si = { sizeof(STARTUPINFO),NULL,"",NULL,0,0,0,0,0,0,0,STARTF_USESHOWWINDOW,0,0,NULL,0,0,0 };
	si.wShowWindow = SW_SHOW;
	si.lpDesktop = NULL;
	LPVOID pEnv = NULL;
	CreateEnvironmentBlock(&pEnv, hPtoken, FALSE);

	CreateProcessAsUserA(hPtoken, LeTools::ws2s(buff).c_str(), (LPSTR)"setautorunasexe",
		NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT, pEnv, LeTools::ws2s(wstrPath).c_str(), &si, &pi);
	
	FLOG(L"SetAutoRunAsExe End");

	return;
}
