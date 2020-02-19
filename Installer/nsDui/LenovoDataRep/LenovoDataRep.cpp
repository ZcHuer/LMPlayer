// LenovoDataRep.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "LenovoDataRep.h"
#include "plugin-common.h"
#include "ReportUtil.h"

HINSTANCE g_hInstance;
HWND g_hwndParent;
extra_parameters *g_pluginParms = NULL;

CAes_encryptor g_aesEncry;

CString g_updateServiec = _T("https://gamecent-dev.vgs.lenovo.com.cn");
CString g_updatePath = _T("/back/gamelog/event");
CString g_updatePathPage = _T("/back/gamelog/page");

//CString g_InstallLePlayerserver = _T("https://collect.vgs.lenovo.com.cn/");
//CString g_InstallLePlayerPath = _T("log/event");

//https://leplayer-dev.vgs.lenovo.com.cn
//https://collect.vgs.lenovo.com.cn/
//log/event

CString g_InstallLePlayerserver = _T("https://collect.vgs.lenovo.com.cn/");
CString g_InstallLePlayerPath = _T("log/event");

//CString g_InstallLePlayerserver = _T("https://collect-dev.vgs.lenovo.com.cn/");
//CString g_InstallLePlayerPath = _T("log/event");

CString g_version =_T("");
CString g_channelID = _T("");

#define NSMETHOD_INIT(parent) {\
        g_pluginParms = extra; \
        g_hwndParent = parent; \
        EXDLL_INIT(); }


// 这是导出变量的一个示例
LENOVODATAREP_API int nLenovoDataRep=0;

// 这是导出函数的一个示例。
LENOVODATAREP_API int fnLenovoDataRep(void)
{
    return 42;
}

// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 LenovoDataRep.h
CLenovoDataRep::CLenovoDataRep()
{
    return;
}

extern "C" __declspec(dllexport) void __cdecl
ReportLenovoData(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
	NSMETHOD_INIT(hwndParent);
	{
		
		wchar_t service[1024] = { 0 };
		popstring(service);
		wstring wservice = service;
		if (wservice.length()>0)
		{
			g_updateServiec = service;
		}
		// eventid and detail
		//int idEvent = popint();
		wchar_t idEvent[16] = { 0 };
		popstring(idEvent);
		CString strEvent = idEvent;
		wchar_t buf[1024] = { 0 };
		popstring(buf);
		wchar_t version[64] = { 0 };
		popstring(version);
		g_version = version;
		
		g_aesEncry.InitKey("1102130405061708", "1102130405061708");
		SendRePort(strEvent, buf);
		// ==
	}
}


extern "C" __declspec(dllexport) void __cdecl
ReportLePlayerData(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
	NSMETHOD_INIT(hwndParent);
	{

		wchar_t service[1024] = { 0 };
		popstring(service);
		wstring wservice = service;
		if (wservice.length() > 0)
		{
			g_updateServiec = service;
		}
		// eventid and detail
		//int idEvent = popint();
		wchar_t idEvent[16] = { 0 };
		popstring(idEvent);
		CString strEvent = idEvent;
		wchar_t buf[1024] = { 0 };
		popstring(buf);
		wchar_t version[64] = { 0 };
		popstring(version);
		g_version = version;
		wchar_t channelID[64] = { 0 };
		popstring(channelID);
		g_channelID = channelID;
		g_aesEncry.InitKey("1102130405061708", "1102130405061708");
		SendRePortLePlayer(strEvent, buf);
		// ==
	}
}

extern "C" __declspec(dllexport) void __cdecl
ReportPageEvent(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
	NSMETHOD_INIT(hwndParent);
	{
		//MessageBox(NULL, L"1", L"2", 0);
		wchar_t service[1024] = { 0 };
		popstring(service);
		wstring wservice = service;
		if (wservice.length() > 0)
		{
			g_updateServiec = service;
		}
		// eventid and detail
		wchar_t pageID[20] = {0};
		popstring(pageID);

		wchar_t version[64] = { 0 };
		popstring(version);
		g_version = version;

		g_aesEncry.InitKey("1102130405061708", "1102130405061708");
		SendRePortPageEvent(pageID);
		// == 
	}
}

extern "C" __declspec(dllexport) void __cdecl
IsBigger(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
	NSMETHOD_INIT(hwndParent);
	{
		//MessageBox(NULL, L"1", L"2", 0);
		wchar_t f1[1024] = { 0 };
		wchar_t f2[1024] = { 0 };
		popstring(f1);
		popstring(f2);
		float fd1 = atof(LeTools::ws2s(f1).c_str());
		float fd2 = atof(LeTools::ws2s(f2).c_str());
		int nRes = 0;
		if (fd1 > fd2) nRes = 1;
		pushint(nRes);
	}
}

extern "C" __declspec(dllexport) void __cdecl
GetSystemVer(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
	NSMETHOD_INIT(hwndParent);
	{
		//MessageBox(NULL, L"1", L"2", 0);
		int nRes = LeTools::GetSystemVer();
		pushint(nRes);
	}
}

extern "C" __declspec(dllexport) void __cdecl
TaskbarPinWin10(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
	NSMETHOD_INIT(hwndParent);
	{
		//MessageBox(NULL, L"1", L"2", 0);
		//TaskbarPinWin10(TRUE, path, L"LePlayer.exe");
		wchar_t bPin[24] = { 0 };
		wchar_t path[1024] = { 0 };
		wchar_t file [1024] = { 0 };
		popstring(bPin);
		popstring(path);
		popstring(file);
		if (0 == _tcscmp(bPin, _T("true")))
		{
			LeTools::TaskbarPinWin10(true,path, file);
		}
		if (0 == _tcscmp(bPin, _T("false")))
		{
			LeTools::TaskbarPinWin10(false, path, file);
		}
		

	}
}
