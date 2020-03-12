// LMPlayer.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <ShlObj.h>
#include <shellapi.h>
#include <TlHelp32.h>
#include <ImageHlp.h>
#include <string>
#include "../SDK/FLog/FileLog.h"
#include "../Include/LeReport.h"
#include "../Include/Data_RealTime.h"

using namespace std;

#define LMPLAYER_MUTEX_STRING	  L"{A974FD90-34FE-4B69-A509-75E43255FE49}"
#define LEUPDATE_MUTEX_STRING	  L"{95209F0E-6D08-48BE-B660-5161524B371F}"

// ɱ������
BOOL KillProcess(LPCTSTR lpProcessName)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapShot, &pe))
	{
		return FALSE;
	}
	CString strProcessName = lpProcessName;
	strProcessName.MakeLower();
	while (Process32Next(hSnapShot, &pe))
	{
		CString scTmp = pe.szExeFile;
		scTmp.MakeLower();
		if (!scTmp.Compare(strProcessName))
		{
			DWORD dwProcessID = pe.th32ProcessID;
			HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessID);
			::TerminateProcess(hProcess, 0);
			CloseHandle(hProcess);
		}
		scTmp.ReleaseBuffer();
	}
	strProcessName.ReleaseBuffer();

	return TRUE;
}

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

static BOOL MakeDirExist(LPCWSTR lpwsDir)
{
	if (NULL == lpwsDir)
		return FALSE;

	int nLen = lstrlen(lpwsDir);
	if (nLen <= 0)
		return FALSE;

	LPWSTR pwsBuf = new WCHAR[MAX_PATH];;
	memset(pwsBuf, 0, MAX_PATH);
	memcpy_s(pwsBuf, MAX_PATH, lpwsDir, nLen*2);

	BOOL bRtn = PathFileExists(lpwsDir);
	if (!bRtn)
	{
		PathRemoveFileSpec(pwsBuf);
		if (MakeDirExist(pwsBuf))
		{
			bRtn = CreateDirectory(lpwsDir, NULL);
		}
	}
	delete[] pwsBuf;
	return bRtn;
}

#ifndef SAFE_DELETE
#define SAFE_DELETE(ptr) do{ if(ptr) {delete ptr; ptr=NULL;} }while(0)
#endif 
#ifndef SAFE_CREATELP
#define SAFE_CREATELP(a,b,c) a *b = new a[c + 1]; memset(b,0,sizeof(a) * (c + 1))
#endif
#ifndef DELETE_LP_NULL
#define DELETE_LP_NULL(x)  if(x){delete [] x;x = NULL;}
#endif
#ifndef SAFE_CREATEARRAY
#define SAFE_CREATEARRAY(a,b,c) a b[c]; memset(b,0,sizeof(a) * c);
#endif
#ifndef SAFE_INITLP
#define SAFE_INITLP(a,b,c)b = new a[c+1]; memset(b,0,sizeof(a) * (c+1));
#endif
#ifndef SAFE_lP
#define SAFE_lP(a,b) a * b = NULL;
#endif

string ws2s(wstring str)
{
	string s = "";
	if (!str.empty())
	{
		int nStrLen = str.length();
		SAFE_CREATELP(char, lpBuff, nStrLen * 2);
		WideCharToMultiByte(CP_ACP, 0, str.c_str(), nStrLen, lpBuff, nStrLen * 2, 0, 0);
		s = lpBuff;
		SAFE_DELETE(lpBuff);
	}
	return s;
}

wstring s2ws(string str)
{
	wstring ws = L"";
	if (!str.empty())
	{
		int nStrLen = str.length();
		SAFE_CREATELP(WCHAR, lpBuff, nStrLen);
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), nStrLen, lpBuff, nStrLen);
		ws = lpBuff;
		SAFE_DELETE(lpBuff);
	}
	return ws;
}

// ɱ������-lmplayer��,ɱn��
void Killlmplayer(int n)
{
	FLOG(_T("Killlmplayer begin"));
	int i = n;
	bool bKillLePlayer = false;
	while (i-- > 0)
	{
		CHandle hLePlayerMutex;
		hLePlayerMutex.Attach(::CreateMutexW(NULL, TRUE, LMPLAYER_MUTEX_STRING));
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			KillProcess(L"lmp.exe");
		}
		else
			bKillLePlayer = true;

		if (bKillLePlayer)
			break;

		Sleep(1000);
	}
	FLOG(_T("Killlmplayer end"));
	return;
}

// ��ȡһϵ�б���
void GetVerAndExePath(HINSTANCE hInstance, wstring& wstrCfg, wstring& wstrCurVer, wstring& wstrNewVer, 
					wstring& wstrRun, wstring& wstrVer, wstring& wstrRunExe, wstring& wstrVersionExe)
{
	// ��ȡ��ǰ�����ļ�·��
	WCHAR wcPath_Cfg[MAX_PATH] = { 0 };
	::GetModuleFileName(hInstance, wcPath_Cfg, MAX_PATH);
	PathRemoveFileSpecW(wcPath_Cfg);
	PathAppendW(wcPath_Cfg, L"\\Config.ini");
	wstrCfg = wcPath_Cfg;

	// ��ȡ��ǰ�汾��
	WCHAR wcCurVer[MAX_PATH] = { 0 };
	::GetPrivateProfileStringW(L"Update", L"CurVer", L"", wcCurVer, MAX_PATH, wcPath_Cfg);
	wstrCurVer = wcCurVer;

	// ��ȡ�°汾��
	WCHAR wcNewVer[MAX_PATH] = { 0 };
	::GetPrivateProfileStringW(L"Update", L"NewVer", L"", wcNewVer, MAX_PATH, wcPath_Cfg);
	wstrNewVer = wcNewVer;

	WCHAR wcPath_Run[MAX_PATH] = { 0 };
	::GetModuleFileNameW(hInstance, wcPath_Run, MAX_PATH);
	PathRemoveFileSpecW(wcPath_Run);

	wstrRun = wcPath_Run;
	wstrRun += L"\\run";
	wstrVer = wcPath_Run;
	wstrVer += L"\\";
	wstrVer += wcNewVer;

	wstrVersionExe = wstrVer;
	wstrVersionExe += L"\\lmp.exe";

	wstrRunExe = wcPath_Run;
	wstrRunExe += L"\\run\\lmp.exe";

	return;
}

// ɾ��run�ļ��У��������°汾�ļ���Ϊrun
BOOL RemoveRunAndRename(wstring wstrRun, wstring wstrVer)
{
	FLOG(_T("RemoveRunAndRename begin"));

	// ɾ��run�ļ���
	SHFILEOPSTRUCTW fop;
	ZeroMemory(&fop, sizeof(SHFILEOPSTRUCT));
	fop.wFunc = FO_DELETE;
	TCHAR tchTmp[MAX_PATH + 1];
	ZeroMemory(tchTmp, (MAX_PATH + 1) * sizeof(TCHAR));
	_tcscpy(tchTmp, wstrRun.c_str());
	tchTmp[wstrRun.length()] = _T('\0');
	tchTmp[wstrRun.length() + 1] = _T('\0');
	fop.pFrom = tchTmp;
	fop.pTo = NULL;
	fop.fFlags |= FOF_SILENT;        /*����ʾ����*/
	fop.fFlags |= FOF_NOERRORUI;    /*�����������Ϣ*/
	fop.fFlags |= FOF_NOCONFIRMATION;/*������ȷ��*/
	fop.fFlags &= ~FOF_ALLOWUNDO;
	fop.hNameMappings = NULL;
	fop.hwnd = NULL;
	fop.lpszProgressTitle = NULL;
	int n = SHFileOperationW(&fop);

	for (int i = 0; i < 5; i++)// ��ֹ�첽�����Ĳ�������
	{
		//�ļ��в�����
		if (!PathFileExistsW(wstrRun.c_str()))
		{
			// �Ѱ汾�Ÿ���Ϊrun		
			ZeroMemory(&fop, sizeof(fop));

			fop.wFunc = FO_RENAME;

			TCHAR tchTmp_From[MAX_PATH + 1];
			ZeroMemory(tchTmp_From, (MAX_PATH + 1) * sizeof(TCHAR));
			_tcscpy(tchTmp_From, wstrVer.c_str());
			tchTmp_From[wstrVer.length()] = _T('\0');
			tchTmp_From[wstrVer.length() + 1] = _T('\0');
			fop.pFrom = tchTmp_From;

			TCHAR tchTmp_To[MAX_PATH + 1];
			ZeroMemory(tchTmp_To, (MAX_PATH + 1) * sizeof(TCHAR));
			_tcscpy(tchTmp_To, wstrRun.c_str());
			tchTmp_To[wstrRun.length()] = _T('\0');
			tchTmp_To[wstrRun.length() + 1] = _T('\0');
			fop.pTo = tchTmp_To;

			fop.fFlags |= FOF_SILENT;        /*����ʾ����*/
			fop.fFlags |= FOF_NOERRORUI;    /*�����������Ϣ*/
			fop.fFlags |= FOF_NOCONFIRMATION;/*������ȷ��*/
			fop.fFlags &= ~FOF_ALLOWUNDO;
			fop.hNameMappings = NULL;
			fop.hwnd = NULL;
			fop.lpszProgressTitle = NULL;
			int n = SHFileOperationW(&fop);
			if (0 == n)
			{
				FLOG(_T("Rename true"));
				return TRUE;
			}				
			else
			{
				FLOG(_T("Rename false"));
				return FALSE;
			}				
		}
		Sleep(300);
		FLOG(_T("Sleep(300)"));
	}
	FLOG(_T("RemoveRunAndRename end"));
	return FALSE;
}

// ����lmplayer���Ƿ���Ҫ���°汾���Ƿ��л��ɹ��������ļ�·�����°汾·�����ɰ汾exe·�����°汾exe·���������У�
void Runlmplayer(BOOL bNeedUpdate, BOOL bSwitchOk, wstring wstrCfg, wstring wstrNewVer,
				wstring wstrRunExe, wstring wstrVersionExe, CString cstrCommand)
{
	FLOG(_T("RunLeplayer begin"));
	if (bNeedUpdate)
	{
		FLOG(_T("bNeedUpdate 1"));
		if (bSwitchOk)
		{			
			FLOG(_T("bSwitchOk 1"));
			WritePrivateProfileStringW(L"Update", L"CurVer", wstrNewVer.c_str(), wstrCfg.c_str());
			if (PathFileExistsW(wstrRunExe.c_str()))
				::ShellExecuteW(NULL, L"open", wstrRunExe.c_str(), cstrCommand.GetString(), NULL, SW_SHOW);
			else
				::MessageBox(NULL, L"�����ļ�����, ���������ذ�װ��https://sta.vgs.lenovo.com.cn/leplayer.html", L"��ʾ", MB_OK);
			// ���ﱨ�����滻�ɹ�
			CLeReport::GetInstance()->SendRTD_Eeventsync(RTD_UPDATE, "1", "4replace");
		}
		else
		{
			FLOG(_T("bSwitchOk 0"));
			if (PathFileExistsW(wstrVersionExe.c_str()))
				::ShellExecuteW(NULL, L"open", wstrVersionExe.c_str(), cstrCommand.GetString(), NULL, SW_SHOW);
			else
				::MessageBox(NULL, L"�����ļ�����, ���������ذ�װ��https://sta.vgs.lenovo.com.cn/leplayer.html", L"��ʾ", MB_OK);
			// ���ﱨ�����滻ʧ��
			CLeReport::GetInstance()->SendRTD_Eeventsync(RTD_UPDATE, "1", "4replacefail");
		}
	}
	else
	{
		FLOG(_T("bNeedUpdate 0"));
		if (PathFileExistsW(wstrRunExe.c_str()))
			::ShellExecuteW(NULL, L"open", wstrRunExe.c_str(), cstrCommand.GetString(), NULL, SW_SHOW);
		else
			::MessageBox(NULL, L"�����ļ�����, ���������ذ�װ��https://sta.vgs.lenovo.com.cn/leplayer.html", L"��ʾ", MB_OK);
	}
	FLOG(_T("Runlmplayer end"));
	return;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int nCmdShow)
{
	//��ʼ����־·��
	TCHAR buff_log[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buff_log, sizeof(buff_log));
	wstring wslog = buff_log;
	wslog.append(L".log");
	LOGINIT(wslog.c_str());
	FLOG(_T("wWinMain begin"));
	FLOG(L">>>����,�����У�%s", lpCmdLine);

    UNREFERENCED_PARAMETER(lpCmdLine);
	CString cstrCommand = lpCmdLine;
	// ������-�����°汾
	if (cstrCommand.Find(L"runnew") != -1)	
		Killlmplayer(3);

	wstring wstrCfg;		// �����ļ�·��
	wstring wstrCurVer;		// ��ǰ�汾��
	wstring wstrNewVer;		// �°汾��
	wstring wstrRun;		// run·��
	wstring wstrVer;		// �°汾·��
	wstring wstrRunExe;		// run��exe·��
	wstring wstrVersionExe;	// �°汾��exe·��
	GetVerAndExePath(hInstance, wstrCfg, wstrCurVer, wstrNewVer, wstrRun, wstrVer, wstrRunExe, wstrVersionExe);
	
	BOOL bNeedUpdate = FALSE;
	BOOL bSwitchOk = FALSE;
	// ��������汾��һ��
	if (0 != StrCmpW(wstrCurVer.c_str(), wstrNewVer.c_str()) && wstrNewVer.length() != 0 && PathFileExistsW(wstrVersionExe.c_str()))
	{
		bNeedUpdate = TRUE;
		Killlmplayer(1);
		bSwitchOk = RemoveRunAndRename(wstrRun, wstrVer);		
	}

	// ����lmplayer���Ƿ���Ҫ���°汾���Ƿ��л��ɹ��������ļ�·�����°汾·�����ɰ汾exe·�����°汾exe·���������У�
	Runlmplayer(bNeedUpdate, bSwitchOk, wstrCfg, wstrNewVer, wstrRunExe, wstrVersionExe, cstrCommand);
	FLOG(_T("wWinMain end"));
	return 0;
}