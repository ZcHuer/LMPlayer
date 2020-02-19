// dui-demo.cpp : main source file
//

#include "stdafx.h"
#include <windows.h>
#include "MiniDump.h"
#include "MainDlg.h"
#include "Util\util.h"
#include "CommandParse.h"
#include "dllmodule.h"
#include "ctrl\ImageHeader.h"
#include <TlHelp32.h>
#include "Update\CUpdateHelper.h"
#include "InstallingGif.h"
#include "../include/LeReport.h"
#include "CMDParser.h"

//从PE文件加载，注意从文件加载路径位置
#define RES_TYPE 1
//#define RES_TYPE 0   //从文件中加载资源
// #define RES_TYPE 1  //从PE资源中加载UI资源

//定义唯一的一个R,UIRES对象,ROBJ_IN_CPP是resource.h中定义的宏。
ROBJ_IN_CPP

SStringT g_strCmdLine = L""; //命令行参数
wstring g_strRootPath = L"";
string g_strLonginString = "";
CComPtr<ICefManager> g_spCEFMgr;


void DoInit()
{
    //记录当前版本到本地
    //WriteVersion();
    //创建桌面图标信息
    //CreateDeskIcon();
	//初始化统计信息
	wstring wstr;
	LeTools::GetVersion(eLmplayer, wstr);
	CLeReport::GetInstance()->SetVersion(LeTools::ws2s(wstr).c_str());
	
	wstring wChannelID;
	LeTools::GetChannelID(eLmplayer, wChannelID);
	CLeReport::GetInstance()->SetchannelID(LeTools::ws2s(wChannelID).c_str());
}

eProcessType GetProcessType()
{
	eProcessType eType = eBrowserProcess;
	CCommandParse cmd;
	cmd.Parse(GetCommandLine());
	CCommand* pCmd = cmd.FindCommand(L"-type");
	if (NULL == pCmd)
	{
		eType = eBrowserProcess;
	}
	else
	{
		LPCWSTR pwsType = pCmd->GetArg(0);
		if (StrCmpI(pwsType, L"renderer") == 0)
		{
			eType = eRendererProcess;
		}
		// 		else if(StrCmpI(pwsType, L"gpu-process") == 0)
		// 		{
		// 			eType = eOtherProcess;
		// 		}
		else
		{
			eType = eOtherProcess;
		}
	}
	return eType;
}

DWORD GetProcessidFromName(LPCTSTR name)
{
	PROCESSENTRY32 pe;
	DWORD id = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapshot, &pe))
		return 0;
	while (1)
	{
		pe.dwSize = sizeof(PROCESSENTRY32);
		if (Process32Next(hSnapshot, &pe) == FALSE)
			break;
		if (wcscmp(pe.szExeFile, name) == 0)
		{
			id = pe.th32ProcessID;
			break;
		}
	}
	CloseHandle(hSnapshot);
	return id;
}

LPCWSTR CrashFileName(CString& strName)
{
	LPCWSTR pwsPrefix = L"";	
	strName.Format(L"LEPLAYER%s_%d_%s", pwsPrefix, GetCurrentProcessId(), CURVER);
	return strName;
}

HRESULT InitCrashHandler(LPCWSTR lpwsPrefix)
{
	if (NULL == lpwsPrefix)
		return E_INVALIDARG;

	UINT new_flags =
		SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX;
	UINT existing_flags = SetErrorMode(new_flags);
	SetErrorMode(existing_flags | new_flags);

	CMiniDump::SetDumpFileName(lpwsPrefix, g_strRootPath.c_str(), L"dump");
	//CMiniDump::SetCrashNotify(OnCrashNotify, 0);
	bool bRtn = CMiniDump::EnableTrackException(true);
	return bRtn ? S_OK : S_FALSE;
}

bool HaveVersionLocal()
{
	bool bres = false;
	wstring wstrCfg = L"";
	// 获取当前配置文件路径
	WCHAR wcPath_Cfg[MAX_PATH] = { 0 };
	WCHAR wcPathInstall[MAX_PATH] = {0};
	::GetModuleFileName(NULL, wcPath_Cfg, MAX_PATH);
	PathRemoveFileSpecW(wcPath_Cfg);
	PathRemoveFileSpecW(wcPath_Cfg);

	PathAppendW(wcPathInstall, wcPath_Cfg);
	PathAppendW(wcPath_Cfg, L"\\Config.ini");
	wstrCfg = wcPath_Cfg;

	wstring wstrCurVer = L"";
	// 获取当前版本号
	WCHAR wcCurVer[MAX_PATH] = { 0 };
	::GetPrivateProfileStringW(L"Update", L"CurVer", L"", wcCurVer, MAX_PATH, wcPath_Cfg);
	wstrCurVer = wcCurVer;

	// 获取新版本号
	wstring wstrNewVer = L"";
	WCHAR wcNewVer[MAX_PATH] = { 0 };
	::GetPrivateProfileStringW(L"Update", L"NewVer", L"", wcNewVer, MAX_PATH, wcPath_Cfg);
	wstrNewVer = wcNewVer;
	wstring lanchPath = L"";
	wstring wstrVersionExe = L"";
	wstring wstrVer = L"";
	wstrVer = wcPathInstall;
	wstrVer += L"\\";
	lanchPath = wstrVer;
	wstrVer += wcNewVer;

	wstrVersionExe = wstrVer;
	wstrVersionExe += L"\\lmp.exe";
	lanchPath += L"\\LMPlayer.exe";

	if (0 != StrCmpW(wstrCurVer.c_str(), wstrNewVer.c_str()) && wstrNewVer.length() != 0 && PathFileExistsW(wstrVersionExe.c_str()))
	{
		//有新版本
		bres = true;
		ShellExecute(NULL, L"open", lanchPath.c_str(),NULL, NULL, SW_SHOW);
	}

	return bres;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int /*nCmdShow*/)
{
	//初始化日志路径
	TCHAR buff_log[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buff_log, sizeof(buff_log));
	wstring wslog = buff_log;
	wstring wslog_Data = buff_log;
	wslog.append(L".log");
	wslog_Data.append(L".data");
	LOGINIT(wslog.c_str());
	LOGINIT_Data(wslog_Data.c_str());	
	FLOG(L">>>启动,命令行：%s", lpstrCmdLine);
	//MessageBox(NULL,lpstrCmdLine, L"main", 0);
	g_strCmdLine = lpstrCmdLine;
	// 为了应对windows直接打开文件路径带的引号，先去掉。
	if (g_strCmdLine.Find(L"\"")!=-1 && g_strCmdLine.Find(L"\"") == 0)
	{
		g_strCmdLine = g_strCmdLine.Mid(g_strCmdLine.Find(L"\"") + 1, g_strCmdLine.GetLength());
		if (g_strCmdLine.Find(L"\"") != -1)
			g_strCmdLine = g_strCmdLine.Mid(0, g_strCmdLine.Find(L"\""));
	}
		
	SApplication *theApp = NULL;
	SComMgr *pComMgr = NULL;
    HWND hMainWnd = FindWindowA("LMPlayer", "lmplayer");
	CHandle hLePlayerMutex;
	TCHAR buff[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buff, sizeof(buff));
	g_strRootPath = buff;
	g_strRootPath = g_strRootPath.substr(0, g_strRootPath.rfind(L"\\") + 1);
	
	CString strName;
	HRESULT hr = InitCrashHandler(CrashFileName(strName));
	
	wstring wstrVer;
	LeTools::GetVersion(eLmplayer, wstrVer);
	FLOG(L"版本号：%s", wstrVer.c_str());
	SYSTEMTIME tm;
	::GetSystemTime(&tm);
	int nDate_Now = tm.wYear * 360 + tm.wMonth * 30 + tm.wDay;
	string strTime = LeTools::Num2Str(nDate_Now);
	string strCfg;
	LeTools::GetConfigFilePath(eLmplayer, strCfg);
	WritePrivateProfileStringA("service", "runtime", strTime.c_str(), strCfg.c_str());

	//单例判断，如果已经启动，则直接转发命令行后退出
	hLePlayerMutex.Attach(::CreateMutexW(NULL, TRUE, LMPLAYER_MUTEX_STRING));
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		FLOG(L"already start cmdline: %s", g_strCmdLine);
		int npos = g_strCmdLine.Find(CMD_RUN_AUTORUN);
		if (hMainWnd != NULL && npos < 0)
		{
			string strPath = "";
			strPath.append(LeTools::ws2s(g_strCmdLine.GetBuffer(0)));
			COPYDATASTRUCT cpd;
			cpd.dwData = UM_COPYDATA_CMDLING;
			cpd.cbData = strPath.length();
			cpd.lpData = (void*)strPath.c_str();
			::SendMessage(hMainWnd, WM_COPYDATA, 0, (LPARAM)&cpd);
		}
		return 0;
	}	
	
	//初始化本地数据库		
	CLeDB::GetInstance()->InitSql(/*strdb*/);
	CLeDB::GetInstance()->CreateTable();
	CLeDB::GetInstance()->CreateIndex();

	//一些可以异步的初始化工作，放到线程中提高启动速度
	thread th = thread(DoInit);
	th.detach();

	hr = OleInitialize(NULL);
	pComMgr = new SComMgr;

	//将程序的运行路径修改到项目所在目录所在的目录
	TCHAR szCurrentDir[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szCurrentDir, sizeof(szCurrentDir));
	LPTSTR lpInsertPos = _tcsrchr(szCurrentDir, _T('\\'));
	_tcscpy(lpInsertPos + 1, _T(".."));
	SetCurrentDirectory(szCurrentDir);

	BOOL bLoaded = FALSE;
	CAutoRefPtr<SOUI::IImgDecoderFactory> pImgDecoderFactory;
	CAutoRefPtr<SOUI::IRenderFactory> pRenderFactory;
	bLoaded = pComMgr->CreateRender_GDI((IObjRef**)&pRenderFactory);
	SASSERT_FMT(bLoaded, _T("load interface [render] failed!"));
	bLoaded = pComMgr->CreateImgDecoder((IObjRef**)&pImgDecoderFactory);
	SASSERT_FMT(bLoaded, _T("load interface [%s] failed!"), _T("imgdecoder"));

	pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);
	theApp = new SApplication(pRenderFactory, hInstance, L"LMPlayer");
	CAutoRefPtr<IResProvider>   pResProvider;
#if (RES_TYPE == 0)
	CreateResProvider(RES_FILE, (IObjRef**)&pResProvider);
	if (!pResProvider->Init((LPARAM)_T("uires"), 0))
	{
		SASSERT(0);
		return 1;
	}
#else 
	CreateResProvider(RES_PE, (IObjRef**)&pResProvider);
	pResProvider->Init((WPARAM)hInstance, 0);
#endif
	// 自定义接口
	theApp->RegisterWindowClass<SGifPlayer>();
	theApp->RegisterWndFactory(TplSWindowFactory<SImageButton1>());
	theApp->RegisterWndFactory(TplSWindowFactory<SCheckBoxEx>());
	theApp->RegisterWndFactory(TplSWindowFactory<SWndBtn>());
	theApp->RegisterWndFactory(TplSWindowFactory<SFlyWnd>());//注册飞行动画控件
	theApp->RegisterWndFactory(TplSWindowFactory<SListTitle>());
	theApp->RegisterWndFactory(TplSWindowFactory<SListWnd>());
	theApp->RegisterWndFactory(TplSWindowFactory<SAreaButton>());
	theApp->RegisterWndFactory(TplSWindowFactory<SLineCtrl>());
	theApp->RegisterWndFactory(TplSWindowFactory<SItemWnd>());
	theApp->RegisterWndFactory(TplSWindowFactory<CImageHeader>());
	theApp->RegisterWndFactory(TplSWindowFactory<SFadeFrame>());//注册渐显隐动画控件

	theApp->RegisterSkinFactory(TplSkinFactory<SSkinGif>());//注册SkinGif
	theApp->RegisterSkinFactory(TplSkinFactory<SSkinAPNG>());//注册SSkinAPNG
	SSkinGif::Gdiplus_Startup();

	theApp->InitXmlNamedID(namedXmlID, ARRAYSIZE(namedXmlID), TRUE);
	theApp->AddResProvider(pResProvider);
		
	CMainDlg dlgMain;
	dlgMain.Create(GetActiveWindow());
	dlgMain.SendMessage(WM_INITDIALOG);
	FLOG(L"g_strCmdLine:%s", g_strCmdLine);
	if (g_strCmdLine.Find(CMD_RUN_AUTORUN) != -1)
	{
		// 开机启动，只留托盘在右下角，不启动窗口，不在任务栏显示
		FLOG(L"开机启动，只留托盘");
		dlgMain.ShowWindow(SW_HIDE);
		CUpdatehelper::GetInstance()->SetLeplayerAutoRun(true);
	}
	else
	{
		FLOG(L"非开机启动");			
		dlgMain.CenterWindow(dlgMain.m_hWnd);
		dlgMain.ShowWindow(SW_SHOWNORMAL);
		CUpdatehelper::GetInstance()->SetLeplayerAutoRun(false);
	}
	hMainWnd = dlgMain.m_hWnd;
	theApp->Run(dlgMain.m_hWnd);	
	theApp->UnregisterWindowClass<SGifPlayer>();
	theApp->UnregisterSkinClass<SSkinGif>();//注册SkinGif
	theApp->UnregisterSkinClass<SSkinAPNG>();//注册SSkinAPNG
	SSkinGif::Gdiplus_Shutdown();
			
	OleUninitialize();	
	// 释放数据库相关
	CLeDB::GetInstance()->Release();

	if (theApp)
		delete theApp;
	if (pComMgr)
		delete pComMgr;

	CUpdatehelper::GetInstance()->Release();

	FLOG(L"<<<<<<退出")
	LeTools::KillProcess(L"lmp.exe");	
	
    return 0;
}

LONG WINAPI  MyExceptionFilter(EXCEPTION_POINTERS *pExptInfo)
{
	printf("Exception...");

	std::wstringstream ss;
	ss << CUtil::GetAppPathDir() << L"\\Dump";

	if (!PathFileExists(ss.str().c_str())) {
		bool result = CUtil::CreateFullDir(ss.str().c_str());
	}

    wstring wsPath = g_strRootPath;
    wsPath.append(L"Dump\\ExceptionDump.dmp");

	HANDLE hFile = ::CreateFile(wsPath.c_str(), GENERIC_WRITE,
		FILE_SHARE_WRITE, NULL, CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION exptInfo;
		exptInfo.ThreadId = ::GetCurrentThreadId();
		exptInfo.ExceptionPointers = pExptInfo;
		//将内存堆栈dump到文件中
		//MiniDumpWriteDump需引入dbghelp头文件
		BOOL bOK = ::MiniDumpWriteDump(::GetCurrentProcess(),
			::GetCurrentProcessId(), hFile, MiniDumpNormal,
			&exptInfo, NULL, NULL);
	}
	return EXCEPTION_EXECUTE_HANDLER;
}

//LPTOP_LEVEL_EXCEPTION_FILTER pPrevFilter = ::SetUnhandledExceptionFilter(MyExceptionFilter);
//string str1 = "1234567890~!@#$%^&*()_+<>?:abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
//string str2 =LeTools::EncryptStr(str1);
//string str3 =LeTools::DecryptStr(str2);