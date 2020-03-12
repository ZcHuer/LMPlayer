// LeDaemonWork.cpp : 定义 DLL 应用程序的导出函数。

#include "stdafx.h"
//#include <WinInet.h>
//wininet.h  winhttp.h

#include "LeDaemonWork.h"
#include "../SDK/http/Http.h"
#include "../Include/json/json.h"
#include "../SDK/Flog/FileLog.h"
#include "../Include/LeTools.h"
#include "../Include/LeReport.h"
#include <MSCoree.h>
#include <Metahost.h>
#include <process.h>
#include <shellapi.h>
#include <TlHelp32.h>
#include <iostream>
#include <shellapi.h>
#include <atltime.h>
#include <mutex>
#include "UpdateRepire.h"

#include <strsafe.h>
#include "RemoteCtrl.h"
#include "PopAdvCtrl.h"

#pragma comment(lib,"urlmon.lib")
#pragma comment(lib,"WinInet.lib")
using namespace std;
using namespace LeConnect;
std::string g_advStr = "";

wchar_t g_currentPath[MAX_PATH] = { 0 };
CTime g_timeService;
mutex g_AdvInformutex;
int g_index;
LONGLONG g_lTimeTemp = 0;
LONGLONG g_lTime = 0;
bool isNeedRepired();

bool CurrentIsLanchPath(string &currentPath)
{
	bool bres = FALSE;
	WCHAR wcPath[MAX_PATH] = { 0 };
	::GetModuleFileNameW(NULL, wcPath, MAX_PATH);
	PathRemoveFileSpecW(wcPath);
	currentPath = LeTools::ws2s(wcPath);
	PathAppendW(wcPath, L"\\LMPlayer.exe");
	if (PathFileExists(wcPath)) bres = TRUE;
	return bres;
}

bool CheckAdvPop(string advInfo)
{
	BOOL bRes = FALSE;
	Json::Reader jr;
	Json::Value jvData;
	if (jr.parse(advInfo, jvData)) {
		string url = jvData["datas"][0]["downAddress"].asString();
		bRes = TRUE;
	}
	else
	{
		bRes = FALSE;
	}
	return bRes;
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
		//StrCatW(szExplorerPath, L"\\explorer.exe");
		//StrNCatW(szExplorerPath, L"\\explorer.exe", MAX_PATH);
		StringCchCat(szExplorerPath, MAX_PATH, L"\\explorer.exe");
		string expStr =LeTools::ws2s(szExplorerPath);
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

BOOL CheckPophistory(string adid)
{
	BOOL bRes = FALSE;
	string path = "";
	LeTools::GetAdPopHistoryCfg(path);
	FILE *fp;
	fopen_s(&fp,path.c_str(), "a+");
	if (fp)
	{
		char buf[1024] = "";
		long pos;
		do
		{
			size_t rsize = fread(buf, 1, 1024, fp);
			string tmp = buf;
			if (tmp.find(adid))
			{
				bRes = FALSE;
				break;
			}
			else
			{
				bRes = TRUE;
			}
			int index = tmp.rfind(',');
			if (index != 1024)
			{
				fseek(fp, index - 1024, SEEK_CUR);
			}
			pos = ftell(fp);
		} while (pos != EOF);

		fwrite(adid.c_str(), 1, adid.length(), fp);
		fclose(fp);
	}
	return bRes;
}
void TagAdFlag(string adid)
{
	string path = "";
	LeTools::GetAdPopHistoryCfg(path);
	FILE *fp;
	fopen_s(&fp,path.c_str(), "a+");
	if (fp)
	{
		fwrite(adid.c_str(), 1, adid.length(), fp);
		fwrite(",", 1, 1, fp);
		fclose(fp);
	}

}
bool isNeedRepired()
{
	//检查播放器是否存在
	//1 卸载；0或者不传
	bool bres = TRUE;
	WCHAR wcMoudlePath[MAX_PATH] = { 0 };
	::GetModuleFileNameW(NULL, wcMoudlePath, MAX_PATH);
	//FLOG(wcMoudlePath);
	string path;
	bool isoldServer = CurrentIsLanchPath(path);
	if (isoldServer)
	{
		PathRemoveFileSpecW(wcMoudlePath);
		PathAppendW(wcMoudlePath, L"\\LMPlayer.exe");
	}
	else
	{
		PathRemoveFileSpecW(wcMoudlePath);
		PathRemoveFileSpecW(wcMoudlePath);
		PathAppendW(wcMoudlePath, L"\\LePlayer\\LMPlayer.exe");
	}
	
	FLOG(wcMoudlePath);
	if (PathFileExistsW(wcMoudlePath))
	{
		//MessageBox(lpwsContent, L"4",  0);
		bres = FALSE;
	}

	return bres;
	
}

bool IsDownloadOk(string sPath)
{
	FILE* file = NULL;
	errno_t e = fopen_s(&file, sPath.c_str(), "rb");
	if (file)
	{
		_fseeki64(file, 0, SEEK_END);
		__int64 llSize = _ftelli64(file);
		_fseeki64(file, 0, SEEK_SET);
		char* lpData = (char*)malloc(llSize + 1);
		memset(lpData, 0, llSize + 1);
		fread(lpData, llSize, 1, file);
		fclose(file);

		//MD5 md5(lpData, llSize);
		//string sMd5 = md5.toString();
		//free(lpData);
		//if (stricmp(sMd5.c_str(),m_strMd5Value.c_str())==0)
		//{
		return true;
		//}
	}
	return false;
}


void CheckRepire()
{
	//https://sus.lex.lenovo.com.cn/api/list?tvSign=lenovo_windows&sn=00FF0E87E604
	bool bSucc = false;
	wstring wsUrl = L"https://sus.lex.lenovo.com.cn/api/list?";
	wsUrl.append(L"tvSign=lenovo_windows");
	wsUrl.append(L"&sn=");
	string strMac = LeTools::GetMacString();
	wsUrl.append(LeTools::s2ws(strMac));
	HttpRequest req(wsUrl.c_str(), NULL, NULL);
	bSucc = req.doGet();

	if (bSucc)
	{
		string sData = req.getStream().str();
		sData = LeTools::ConvertUtf8ToGBK(sData);
		Json::Reader jr;
		Json::Value jv;
		if (jr.parse(sData, jv))
		{
			if (jv["code"].asInt() == 200)
			{
				FLOG(L"CheckRepire sucess");
				Json::Value jvData = jv["data"][0];
				int nflag = jvData["allowRepair"].asInt();
				if (0 == nflag)
					return;
				else
				{
					FLOG(L"sRepireUrl 允许修复");
					string repairPacUrl = jvData["repairPacUrl"].asString();
					CUpdateRepire update;
					update.DownloadAndRepire(repairPacUrl.c_str(), "");

				}
			}
		}
	}

}
unsigned int WINAPI OnRepired(void *param)
{
	//修复逻辑
	do
	{
		FLOG(L"CheckRepire begin");
		FLOG(L"CheckRepire:%d ", isNeedRepired());
		if(isNeedRepired())
			CheckRepire();
		Sleep(60 * 60 * 1000);
	} while (true);
	return S_OK;
}

LONGLONG GetTime(string time)
{
	LONGLONG ntime = 0;
	if (time.length() == 0)
	{
		CTime tm = GetCurrentTime();
		ntime = tm.GetTime();
	}
	else
	{
		//00:00
		CTime tm =GetCurrentTime();
		int iyear = tm.GetYear();
		int month = tm.GetMonth();
		int day = tm.GetDay();
		int hour =atoi(time.substr(0, 2).c_str());
		int minmont= atoi(time.substr(3).c_str());
		CTime tm1(iyear, month, day, hour, minmont, 0);
		ntime = tm1.GetTime();

	}
	return ntime;
}

unsigned int WINAPI PopAdvThread(void *param)
{
	//每天成功拉起一次，之后退出
	FLOG(L"PopAdvThread Begin ");
	CPopAdvCtrl crc;
	crc.PopAdvCtrl();
	return S_OK;
}

// 文件关联线程,启动时执行一次
unsigned int WINAPI FAThread(void *param)
{
	FLOG(_T("FAThread begin"));
	DWORD dwTick = ::GetTickCount();
	while (true)
	{
		LeTools::FA();

		if (::GetTickCount() - dwTick < 60 * 60 * 1000)
			Sleep(5 * 60 * 1000);
		else
			dwTick = ::GetTickCount();
	}
	FLOG(_T("FAThread end"));
	return 0;
}

// void CopyOurFiles()
// {
// 	string path;
// 	bool isoldServer = CurrentIsLanchPath(path);
// 
// 	WCHAR wcPath_Pop_Old[MAX_PATH] = { 0 };
// 	::GetModuleFileNameW(NULL, wcPath_Pop_Old, MAX_PATH);
// 	PathRemoveFileSpecW(wcPath_Pop_Old);
// 	PathAppendW(wcPath_Pop_Old, L"\\run\\LePopAdSe.exe");
// 	
// 	FLOG(_T("wcPath_Pop_Old ：%ws"), wcPath_Pop_Old);
// 
// 	WCHAR wcPath_Pop_New[MAX_PATH] = { 0 };
// 	::GetModuleFileNameW(NULL, wcPath_Pop_New, MAX_PATH);
// 	PathRemoveFileSpecW(wcPath_Pop_New);
// 	PathAppendW(wcPath_Pop_New, L"\\LePopAdSe.exe");
// 	FLOG(_T("wcPath_Pop_New ：%ws"), wcPath_Pop_New);
// 
// 	FLOG(_T("wcPath_Pop_Old ：%ws"), wcPath_Pop_Old);
// 	if (PathFileExists(wcPath_Pop_Old))
// 	{
// 		FLOG(L"Copy Pop");
// 		::DeleteFile(wcPath_Pop_New);
// 		::CopyFile(wcPath_Pop_Old, wcPath_Pop_New, FALSE);
// 	}
// }

unsigned int WINAPI ServiceReport(void *param)
{
	FLOG(L"ServiceReport begin");
	do
	{
		wstring wstrVersion;
		LeTools::GetVersion(eService, wstrVersion);
		FLOG(L"ServiceReport:%s", wstrVersion.c_str());
		string strVersion = LeTools::ws2s(wstrVersion);
		CLeReport::GetInstance()->SetVersion(strVersion.c_str());
		wstring wstrChannel;
		LeTools::GetChannelID(eService, wstrChannel);
		CLeReport::GetInstance()->SetchannelID(LeTools::ws2s(wstrChannel).c_str());
		bool bres = CLeReport::GetInstance()->SendRTD_Eeventsync("66", "1", "服务日活");
		if (bres)
		{
			FLOG(L"wstrChannel:%s", wstrChannel.c_str());
			FLOG(L"服务日活 上报成功")
			break;
		}
		Sleep(30 * 1000);
		
	} while (true);
	
	FLOG(L"ServiceReport end");
	return 1;
}

unsigned int WINAPI GetConfigThread(void *param)
{
	FLOG(L"GetConfigThread Begin ");
	CRemoteCtrl crc;
	crc.RemoteCtrl();
	FLOG(L"GetConfigThread end ");
	return 0;
}
bool CheckUninsFlagByName(wstring appPath)
{
	bool bres = false;
	int index =appPath.find_last_of(L"\\");
	wstring filename = appPath.substr(index);
	FLOG(L"filename:%s", filename.c_str());
	if (filename==L"LXB.exe")
	{
		bres = true;
	}
	return bres;
}

bool CheckStartWork()
{
	// 默认不启动
	bool bres = false;
	//当前主板时间
	SYSTEMTIME stime;
	::GetSystemTime(&stime);
	CTime tm1(stime.wYear, stime.wMonth, stime.wDay, stime.wHour, stime.wMinute, stime.wSecond);
	//获取服务注册时间：20191001
	LONGLONG strtime = LeTools::GetInstallTime(eService);
	CTime tm2(strtime);
	CTimeSpan spn = tm1 - tm2;
	if (spn.GetTotalHours() >= 24 * 30)
		bres = true;
	return bres;
}

LEDAEMONWORK_API int fnLeDaemonWork(wchar_t * cPath)
{
	TCHAR buff[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buff, sizeof(buff));
	wstring wslog = buff;
	wslog.append(L".log");
	LOGINIT(wslog.c_str());
	FLOG(L"fnLeDaemonWork begin cPath:%s", buff);
	//StrCpyW(g_currentPath, cPath);
	StrNCpy(g_currentPath, cPath, 260);

	bool bAbaleStartWork = CheckStartWork();
	if (bAbaleStartWork)
	{
		FLOG(L"fnLeDaemonWork CheckStartWork true");
		_beginthreadex(NULL, 0, FAThread, NULL, 0, NULL);
		//复制依赖文件
		//CopyOurFiles();
		//LeTools::WriteLePlayerProtocol();
		//服务报活
		_beginthreadex(NULL, 0, ServiceReport, NULL, 0, NULL);
		//修复检测线程
		//_beginthreadex(NULL, 0, OnRepired, NULL, 0, NULL);
		//五分钟检查一次
		//FLOG(L"PopAdvThread");
		//_beginthreadex(NULL, 0, PopAdvThread, NULL, 0, NULL);
		//后台请求，入口申请
		//_beginthreadex(NULL, 0, GetConfigThread, NULL, 0, NULL);
	}
	else
	{
		//检测30天内
		wstring wstr;
		LeTools::GetRunVersion(wstr);
		CLeReport::GetInstance()->SetVersion(LeTools::ws2s(wstr).c_str());
		wstring wChannelID;
		LeTools::GetChannelID(wChannelID);
		CLeReport::GetInstance()->SetchannelID(LeTools::ws2s(wChannelID).c_str());
		bool bres = CLeReport::GetInstance()->SendRTD_Eeventsync("74", "1", "Service Sleep");
		FLOG(L"fnLeDaemonWork CheckStartWork false");
	}
	return bAbaleStartWork;
}