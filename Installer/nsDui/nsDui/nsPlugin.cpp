#include <windows.h>
#include <commctrl.h>
#include <Shlobj.h>
#include "plugin-common.h"
#include "DlgMain.h"
#include "LeTools.h"
#include "../SDK/http/Http.h"
#include "../include/json/json.h"


#pragma comment(lib,"Shell32.lib")
#pragma comment(lib,"LeTools.lib")
#pragma comment(lib,"Urlmon.lib")

HINSTANCE g_hInstance;
HWND g_hwndParent;
extra_parameters *g_pluginParms = NULL;
CDlgMain *g_pMainDlg = NULL;
std::map<HWND, WNDPROC> g_windowInfoMap;
CDuiString g_progressCtrlName = _T("");
CDuiString g_tabLayoutCtrlName = _T("");
CDuiString g_tabShowLabelName = _T("");
CDuiString g_wstrFrom = _T("install");
bool g_bMSGLoopFlag = true;

using namespace LeConnect;

typedef struct AdvInfo
{
	string mediaType;				//跳转类型
	string elementUrl;				//图片资源地址
	string jumpUrl;					//跳转地址					playUrl
	string cpid;					//CPID
	string fsp2;					//优酷播放地址
}AdvInfo,*pAdvinfo;

AdvInfo g_advInfo[3];

#define NSMETHOD_INIT(parent) {\
        g_pluginParms = extra; \
        g_hwndParent = parent; \
        EXDLL_INIT(); }

BOOL WINAPI DllMain(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
    g_hInstance = (HINSTANCE)hInst;

    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        //do what you want at init time.
    }

    if (ul_reason_for_call == DLL_THREAD_DETACH || ul_reason_for_call == DLL_PROCESS_DETACH) {
        //clean up code.
    }

    return TRUE;
}

// NSIS插件导出函数,NSIS规定函数声明格式如下：
extern "C" __declspec(dllexport) void __cdecl
add ( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
    NSMETHOD_INIT(hwndParent);
    {
        // == 添加自己代码
        int i = popint();
        int j = popint();
        int k = i + j;
        pushint(k);
        // ==
    }
}

extern "C" __declspec(dllexport) void __cdecl
GetDialogSize ( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
    NSMETHOD_INIT(hwndParent);
    {
        HWND hwnd = (HWND)popint();
        RECT rect;
        ::GetWindowRect(hwnd, &rect);

        pushint(rect.bottom - rect.top);
        pushint(rect.right - rect.left);
    }
}

extern "C" __declspec(dllexport) void __cdecl
GetDialogStyle ( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
    NSMETHOD_INIT(hwndParent);
    {
        HWND hwnd = (HWND)popint();
        int style = (int)::GetWindowLongA(hwnd, GWL_STYLE);
        pushint(style);
    }
}

extern "C" __declspec(dllexport) void __cdecl
GetSetupPath ( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
    NSMETHOD_INIT(hwndParent);
    {
        wchar_t buf[512] = {0};
        ::GetModuleFileName(NULL, buf, 512);
        pushstring(buf);
    }
}

extern "C" __declspec(dllexport) void __cdecl
Trace ( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
    NSMETHOD_INIT(hwndParent);
    {
        wchar_t buf[1024] = {0};
        popstring(buf);
        DUI__Trace(_T("NSISHelper Trace:%ws"), buf);
    }
}

extern "C" __declspec(dllexport) void __cdecl
GetCtrlPos ( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
    NSMETHOD_INIT(hwndParent);
    {
        HWND hwnd = (HWND)popint();
        RECT rect;
        GetClientRect(hwnd, &rect);
        DUI__Trace(_T("%d %d %d %d %d"), hwnd, rect.left, rect.top, rect.right, rect.bottom);

        POINT lt = {rect.left, rect.top};
        POINT rb = {rect.right, rect.bottom};

        ::ClientToScreen(hwndParent, &lt);
        ::ClientToScreen(hwndParent, &rb);

        pushint(rb.y);
        pushint(rb.x);
        pushint(lt.y);
        pushint(lt.x);
    }
}


// =========================================== DUILIB =============================================
CProgressUI *g_pProcess_timer = 0;
CLabelUI *g_pLabel = 0;
int g_pProcess_rangeValue = 0;
int g_pProcess_endValue = 0;
double g_pProcess_curValue = 0;
int g_nAutorunCallBackid = 0;
#define PROCESS_SLIDER_TIMER_ID 1
#define AUTO_INSTALL_TIMER_ID 2
#define  SHOW_PIC 3
#define  GETADV_PIC 4

void SetAdvInfo(Json::Value jv)
{
	int nsize = jv.size();
	if (nsize < 3) return;
	for (int i=0;i<nsize;i++)
	{
		g_advInfo[i].cpid = jv[i]["cpId"].asString();
		if (g_advInfo[i].cpid == "118")
		{
			string urlPara = "";
			if (jv[i]["channelId"].asString().length()>0) {
				urlPara += "&channelId=" + jv[i]["channelId"].asString();
			}
		
			if (jv[i]["cpId"].asString().length()>0) {
				urlPara += "&cpId=" + jv[i]["cpId"].asString();
			}
			if (jv[i]["moduleId"].asString().length() > 0) {
				urlPara += "&moduleId=" + jv[i]["moduleId"].asString();
			}
			if (jv[i]["categoryId"].asString().length() > 0) {
				urlPara += "&category1=" + jv[i]["categoryId"].asString();
			}
			string baseUrl = "http://leplayer.vgs.lenovo.com.cn";
			baseUrl += "/#/detail?vid=";
			baseUrl += jv[i]["albumId"].asString();
			baseUrl += jv[i]["outTvIdBst"].asString();
			baseUrl += urlPara;
			g_advInfo[i].jumpUrl = baseUrl;

		}
		else if (g_advInfo[i].cpid == "119")
		{
			g_advInfo[i].fsp2 = "iku://|play|lenovo-cms|videoid|home|"+ jv[i]["album"]["outTvIdBst"].asString();
		}
		else
		{
			g_advInfo[i].jumpUrl = jv[i]["jumpUrl"].asString();;
			g_advInfo[i].fsp2 = "";
		}
		string sUrl = jv[i]["elementUrl"].asString();
		char cBuff[MAX_PATH] = { 0 };
		GetTempPathA(MAX_PATH, cBuff);
		string sPath = cBuff;
		//sPath.append(sUrl.substr(sUrl.rfind("/") + 1));
		sPath += "\\Leboskin\\ShowPic";
		sPath += LeTools::Num2Str(i);
		sPath += ".png";
		string picName = "ShowPic";
		picName += LeTools::Num2Str(i);
		picName += ".png";
		HRESULT hRes = URLDownloadToFileA(0, sUrl.c_str(), sPath.c_str(), 0, 0);
		if (hRes == S_OK) {

		}
		g_advInfo[i].elementUrl = picName;
		g_advInfo[i].mediaType = jv[i]["mediaType"].asString();
	}
}

void SetControlPic()
{
	for (int i=0;i<3;i++)
	{
		string strCtrlName = "ShowPic";
		strCtrlName += LeTools::Num2Str(i);
		wstring wPicName = LeTools::s2ws(strCtrlName).c_str();
		wPicName += L".png";
		CButtonUI *pBtn = static_cast<CButtonUI *>(g_pMainDlg->GetPaintManagerUI()->FindControl(LeTools::s2ws(strCtrlName).c_str()));
		if (pBtn)
		{
			pBtn->SetAttribute(L"bkimage", wPicName.c_str());
		}

	}
}
void CALLBACK TimerProc_SliderProcess(
    HWND hwnd,
    UINT uMsg,
    UINT idEvent,
    DWORD dwTime
    ){
    switch (idEvent){
    case PROCESS_SLIDER_TIMER_ID:
    {
        double iremain = (double)g_pProcess_endValue - g_pProcess_curValue;
        double dinc = iremain / (double)g_pProcess_rangeValue;
        g_pProcess_curValue += dinc;
        if (g_pProcess_timer)
            g_pProcess_timer->SetValue((int)g_pProcess_curValue);
		if (g_pLabel)
		{
			CDuiString tmp;
			tmp.Format(_T("%d%%"), (int)g_pProcess_curValue);
			g_pLabel->SetText(tmp);
		}
    }
    break;
    case AUTO_INSTALL_TIMER_ID:
    {
        KillTimer(g_pMainDlg->GetHWND(), PROCESS_SLIDER_TIMER_ID);
        if (g_nAutorunCallBackid > 0){
            int ntmp = g_nAutorunCallBackid;
            g_nAutorunCallBackid = 0;
             g_pluginParms->ExecuteCodeSegment(ntmp - 1, 0);
             //g_pluginParms->ExecuteCodeSegment(ntmp, 0);
        }
    }
    break;
	case SHOW_PIC:
	{
		CTabLayoutUI *pTab = static_cast<CTabLayoutUI *>(g_pMainDlg->GetPaintManagerUI()->FindControl(g_tabShowLabelName));
		if (pTab)
		{
			int nindex = pTab->GetCurSel();
			int ncount = pTab->GetCount();
			nindex++;
			if (ncount > 0 && nindex > 0 && nindex < ncount)
				pTab->SelectItem(nindex);
			else
				pTab->SelectItem(0);
		}
	}
	case GETADV_PIC:
	{
		KillTimer(g_pMainDlg->GetHWND(), GETADV_PIC);
		//启动安装获取一次
		
		//设置tab页显示图片
	}
	break;
    default:
        break;
    }
}

void GetAdv()
{
	string sUrl = "";
	sUrl = "http://epg.vgs.lenovo.com.cn/getAdvertisement/62?advType=MUP";
	HttpRequest req(LeTools::s2ws(sUrl).c_str(), NULL, NULL);
	BOOL bSucc = req.doGet();
	if (bSucc)
	{
		string sResult = req.getStream().str();
		sResult = LeTools::ConvertUtf8ToGBK(sResult);
		Json::Reader jr;
		Json::Value jvData;
		if (jr.parse(sResult, jvData)) {

			if (jvData["code"].asInt() == 0)
			{
				Json::Value jvAdData = jvData["datas"];
				int nSize = jvAdData.size();
				bool bInstall = false;
				bool bUnInstall = false;
				int nShowIndex = 0;
				if (g_wstrFrom == _T("install"))
					bInstall = true;
				else if (g_wstrFrom == _T("uninstall"))
					bUnInstall = true;
				for (int i = 0; i < nSize; i++)
				{
					if (jvAdData[i]["advStyle"].asString() == "PCclientD" &&bInstall == true)
					{
						nShowIndex = i;
						break;
					}
					if (jvAdData[i]["advStyle"].asString() == "PCclientC" &&bUnInstall == true)
					{
						nShowIndex = i;
						break;
					}
				}

				Json::Value jvElement = jvAdData[nShowIndex]["advElements"];
				SetAdvInfo(jvElement);
				SetControlPic();

			}
		}

	}
}

NSISAPI FindControl(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
	wchar_t controlName[MAX_PATH];
    ZeroMemory(controlName, MAX_PATH * sizeof(TCHAR));

    popstring( controlName );
    CControlUI *pControl = static_cast<CControlUI *>(g_pMainDlg->GetPaintManagerUI()->FindControl( controlName ));

    if( pControl == NULL )
        pushint( - 1 );

    pushint( 0 );
}

NSISAPI  OnControlBindNSISScript(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
	wchar_t controlName[MAX_PATH];
    ZeroMemory(controlName, MAX_PATH);
    popstring(controlName);
    int callbackID = popint();
    g_pMainDlg->SaveToControlCallbackMap( controlName, callbackID );
}

NSISAPI ExitDUISetup(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
    ExitProcess(0);
}


static UINT_PTR PluginCallback(enum NSPIM msg)
{
    return 0;
}

NSISAPI InitDUISetup( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
    NSMETHOD_INIT(hwndParent);
    extra->RegisterPluginCallback(g_hInstance, PluginCallback);
    {
        CPaintManagerUI::SetInstance(g_hInstance);
        
		wchar_t wszFrom[24] = { 0 };
		popstring(wszFrom);
		g_wstrFrom = wszFrom;

		wchar_t pszName[512] = { 0 };
        popstring(pszName); //skinfile path ex: c:/users/user/.../temp/nsxxx/
        CPaintManagerUI::SetResourcePath(pszName);
        g_pMainDlg = new CDlgMain();
        popstring(pszName); //only file name ex:install0804.xml
        g_pMainDlg->SetSkinFilePath(pszName);
	
        popstring(pszName);
       // g_pMainDlg->SetLicenseFilePath(pszName);
		g_pMainDlg->Create(NULL, _T("LMPlayer"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 600, 800);
		//g_pMainDlg->m_pBackWnd->CenterWindow();

		
		//g_pMainDlg->SetMyIcon();
        g_pMainDlg->CenterWindow();
        g_pMainDlg->ShowWindow(FALSE);
        {//一下代码支持初始化显示指定的tab页面，tab name 是xml中的页面容器名称 index是第几个页面，从0开始计数
            popstring(pszName); //tab name
            int nindex=popint(); //page index
           CTabLayoutUI *pTabLayout = static_cast<CTabLayoutUI *>(g_pMainDlg->GetPaintManagerUI()->FindControl(pszName));
            if (pTabLayout) {
                int ncount=pTabLayout->GetCount();
                if (ncount>0 && nindex>0 && nindex <ncount)
                    pTabLayout->SelectItem(nindex);
            }
			
        }
		popstring(pszName);
		g_tabShowLabelName = pszName;
		GetAdv();
		int ntime = popint();
		SetTimer(g_pMainDlg->GetHWND(), SHOW_PIC, ntime, &TimerProc_SliderProcess);
		
        pushint(int(g_pMainDlg->GetHWND()));
		

    }
}

NSISAPI ShowPage ( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
    NSMETHOD_INIT(hwndParent);
    {
        g_pMainDlg->ShowWindow(true);
        //CPaintManagerUI::MessageLoop();
        wchar_t szbuf[512] = { 0 };
        popstring(szbuf);
        if (0 == _tcscmp(szbuf, _T("autorun"))){
            g_nAutorunCallBackid = popint();
            int tValue = popint();
            SetTimer(g_pMainDlg->GetHWND(), AUTO_INSTALL_TIMER_ID, tValue, &TimerProc_SliderProcess);
        }
        MSG msg = { 0 };
        while( ::GetMessage(&msg, NULL, 0, 0) && g_bMSGLoopFlag ) 
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }
}

NSISAPI GetCheckboxStatus ( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
    NSMETHOD_INIT(hwndParent);
    {
		wchar_t pszName[512] = { 0 };

        popstring(pszName);
        CCheckBoxUI *pChbAgree = static_cast<CCheckBoxUI *>(g_pMainDlg->GetPaintManagerUI()->FindControl(pszName));
        if(!pChbAgree) {
            pushint(-1);
            return;
        }
        DUI__Trace(L"%ws status:%d",pszName,pChbAgree->GetCheck()?1:0);
        pushint(pChbAgree->GetCheck()?1:0);
    }
}

NSISAPI ItemEnabledByCheckbox(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
    NSMETHOD_INIT(hwndParent);
    {
        TCHAR itemName[MAX_PATH];
        ZeroMemory(itemName, sizeof(itemName));
        TCHAR checkName[MAX_PATH];
        ZeroMemory(checkName, sizeof(checkName));

        popstring(itemName);
        popstring(checkName);
        CCheckBoxUI *pChbAgree = static_cast<CCheckBoxUI *>(g_pMainDlg->GetPaintManagerUI()->FindControl(checkName));
        if (!pChbAgree)
            return;
        CButtonUI *pBtn = static_cast<CButtonUI *>(g_pMainDlg->GetPaintManagerUI()->FindControl(itemName));
        if (!pBtn)
            return;
        //得到的是以前的状态，点击后状态改变为反
		bool bcheck = pChbAgree->GetCheck();
        pBtn->SetEnabled(!bcheck);
    }
}

NSISAPI SetControlAttribute(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
    NSMETHOD_INIT(hwndParent);
    {
        TCHAR itemName[MAX_PATH];
        ZeroMemory(itemName, sizeof(itemName));
        TCHAR propert[MAX_PATH];
        ZeroMemory(propert, sizeof(propert));
        TCHAR bvalue[MAX_PATH];
        ZeroMemory(bvalue, sizeof(bvalue));

        popstring(itemName);
        popstring(propert);
        popstring(bvalue);
        CControlUI *pctrl = static_cast<CControlUI *>(g_pMainDlg->GetPaintManagerUI()->FindControl(itemName));
        if (!pctrl)
            return;
        CDuiString dsPropert;
        CDuiString dsvalue;
        dsPropert = propert;
        dsvalue = bvalue;
        if (dsPropert == L"visible")
            pctrl->SetVisible(dsvalue == L"true");
        if (dsPropert == L"enabled")
            pctrl->SetEnabled(dsvalue == L"true");
        if (dsPropert == L"pos"){
            RECT rc;
            int npos[8] = { 0 };
            int i = 0;
            const wchar_t *d = L",";
            wchar_t *p;
            p = _tcstok(bvalue, d);
            while (p)
            {
                npos[i++] = _tstoi(p);
                p = _tcstok(NULL, d);
            }
            rc.left = npos[0];
            rc.top = npos[1];
            rc.right = npos[2];
            rc.bottom = npos[3];
            pctrl->SetPos(rc);
        }
    }
}

NSISAPI SetWindowSize(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
    NSMETHOD_INIT(hwndParent);
    {
        HWND hwnd = (HWND)popint();
        int w = popint();
        int h = popint();
        RECT rect;
        GetWindowRect(hwnd, &rect);
        MoveWindow(hwnd, rect.left, rect.top, w, h, true);


        POINT lt = { rect.left, rect.top };
        POINT rb;
        rb.x = rect.left + w;
        rb.y = rect.top + h;
        ::ClientToScreen(hwnd, &lt);
        ::ClientToScreen(hwnd, &rb);
        rect.left = lt.x;
        rect.top = lt.y;
        rect.right = rb.x;
        rect.bottom = rb.y;
        g_pMainDlg->SendMessageW(WM_SIZING, WMSZ_TOPLEFT, (LPARAM)&rect);
        g_pMainDlg->SendMessageW(WM_SIZING, WMSZ_BOTTOMRIGHT, (LPARAM)&rect);
    }
}

NSISAPI IsControlVisible(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
    NSMETHOD_INIT(hwndParent);
    {
        wchar_t pszName[512] = { 0 };

        popstring(pszName);
        CControlUI *pctrl = static_cast<CControlUI *>(g_pMainDlg->GetPaintManagerUI()->FindControl(pszName));
        if (!pctrl)
            return;
        if (pctrl->IsVisible()){
            pushint(1);
        }
        else{
            pushint(0);
        }
    }
}

NSISAPI NextPage ( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
    NSMETHOD_INIT(hwndParent);
    {
        wchar_t pszTabName[512] = {0};

        popstring(pszTabName);

        CTabLayoutUI *pTabLayout = static_cast<CTabLayoutUI *>(g_pMainDlg->GetPaintManagerUI()->FindControl(pszTabName));
        if( pTabLayout) {
            int currentIndex = pTabLayout->GetCurSel();
            pTabLayout->SelectItem(currentIndex+1);
        }
    }
}

NSISAPI SelectPage(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
	NSMETHOD_INIT(hwndParent);
	{
		wchar_t pszTabName[512] = { 0 };
		int index= 0;
		popstring(pszTabName);
		index =popint();

		CTabLayoutUI *pTabLayout = static_cast<CTabLayoutUI *>(g_pMainDlg->GetPaintManagerUI()->FindControl(pszTabName));
		if (pTabLayout) {
			int ncount = pTabLayout->GetCount();
			if (ncount > 0 && index > 0 && index < ncount)
				pTabLayout->SelectItem(index);
		}
	}
}

NSISAPI SetBgImage(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
    NSMETHOD_INIT(hwndParent);
    {
		wchar_t pszTabName[512] = { 0 };
		wchar_t pszImageName[512] = { 0 };

        popstring(pszTabName);
        popstring(pszImageName);
        CContainerUI *pLayout = static_cast<CContainerUI *>(g_pMainDlg->GetPaintManagerUI()->FindControl(pszTabName));
        if (pLayout) {
            pLayout->SetBkImage(pszImageName);
        }
    }
}


NSISAPI PrePage ( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
    NSMETHOD_INIT(hwndParent);
    {
		wchar_t pszTabName[512] = { 0 };

        popstring(pszTabName);

        CTabLayoutUI *pTabLayout = static_cast<CTabLayoutUI *>(g_pMainDlg->GetPaintManagerUI()->FindControl(pszTabName));

        if( pTabLayout) {
            int currentIndex = pTabLayout->GetCurSel();
            pTabLayout->SelectItem(currentIndex-1);
        }
    }
}

NSISAPI SetSliderRange ( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
    NSMETHOD_INIT(hwndParent);
    {
		wchar_t buf[512] = { 0 };
        popstring( buf );
        int iMin = popint();
        int iMax = popint();

        CProgressUI *pProgress = static_cast<CProgressUI *>(g_pMainDlg->GetPaintManagerUI()->FindControl( buf ));

        if( pProgress == NULL )
            return;

        pProgress->SetMaxValue(iMax);
        pProgress->SetMinValue(iMin);
    }
}



NSISAPI SetSliderValue ( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
    NSMETHOD_INIT(hwndParent);
    {
		wchar_t buf[512] = { 0 };
        popstring( buf );
		wchar_t bufLabel[512] = {};
		popstring(bufLabel);
        int iValue = popint();

        //nsis调用这个函数表示7z已经开始解压，释放.7z文件肯定已经完成，防止脚本没有调用结束计时器
        KillTimer(g_pMainDlg->GetHWND(), PROCESS_SLIDER_TIMER_ID);

        CProgressUI *pProgress = static_cast<CProgressUI *>(g_pMainDlg->GetPaintManagerUI()->FindControl( buf ));

        if( pProgress)
            pProgress->SetValue(iValue);

		CLabelUI * plabel = static_cast<CLabelUI*>(g_pMainDlg->GetPaintManagerUI()->FindControl(bufLabel));
		
		if (plabel)
		{
			CDuiString tmp;
			int imax = pProgress->GetMaxValue();
			double scale = (double)((double)iValue / (double)(imax));
			tmp.Format(_T("%d%%"), iValue);
			plabel->SetText(tmp);
		}
    }
}

NSISAPI SetSliderProcessTimeValue(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
    NSMETHOD_INIT(hwndParent);
    {
		wchar_t buf[512] = { 0 };
		wchar_t buf2[512] = { 0 };
        popstring(buf);
		popstring(buf2);
        int start = popint();
        g_pProcess_endValue = popint();
        int tValue = popint();

        g_pProcess_rangeValue = g_pProcess_endValue - start;
        g_pProcess_curValue = start;

        g_pProcess_timer = static_cast<CProgressUI *>(g_pMainDlg->GetPaintManagerUI()->FindControl(buf));
		g_pLabel = static_cast<CLabelUI *>(g_pMainDlg->GetPaintManagerUI()->FindControl(buf2));
        SetTimer(g_pMainDlg->GetHWND(), PROCESS_SLIDER_TIMER_ID, tValue, &TimerProc_SliderProcess);
    }
}

NSISAPI KillSliderProcessTimeValue(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
    NSMETHOD_INIT(hwndParent);
    {
        KillTimer(g_pMainDlg->GetHWND(), PROCESS_SLIDER_TIMER_ID);
    }
}

NSISAPI SetDirValue ( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
    NSMETHOD_INIT(hwndParent);
    {
		wchar_t buf[512] = { 0 };
        popstring( buf );
        CEditUI *pEdit = static_cast<CEditUI *>(g_pMainDlg->GetPaintManagerUI()->FindControl(L"editDir"));
        if(pEdit)
            pEdit->SetText(buf);
    }
}

NSISAPI GetDirValue ( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
    NSMETHOD_INIT(hwndParent);
    {
        CDuiString strFolderPath;
        CEditUI *pEdit = static_cast<CEditUI *>(g_pMainDlg->GetPaintManagerUI()->FindControl(L"editDir"));
        if(pEdit)
            strFolderPath = pEdit->GetText();

		pushstring((wchar_t*)strFolderPath.GetData());
    }
}

NSISAPI SelectInstallDir ( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
    NSMETHOD_INIT(hwndParent);
    {
		wchar_t buf[512] = { 0 };
        BROWSEINFO bi;
        memset(&bi, 0, sizeof(BROWSEINFO));

        popstring(buf);

        bi.hwndOwner = g_pMainDlg->GetHWND();
        bi.lpszTitle = buf;
        bi.ulFlags = 0x0040 ; 

		wchar_t szFolderPath[MAX_PATH] = { 0 };
        LPITEMIDLIST idl = SHBrowseForFolder(&bi);
        if(idl == NULL)
        {
            pushstring(szFolderPath);
            return;
        }

        SHGetPathFromIDList(idl, szFolderPath);

        CEditUI *pEdit = static_cast<CEditUI *>(g_pMainDlg->GetPaintManagerUI()->FindControl(L"editDir"));
        if(pEdit)
            pEdit->SetText(szFolderPath);

        pushstring(szFolderPath);
    }
}

NSISAPI EnableButton(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
	wchar_t controlName[MAX_PATH];
	ZeroMemory(controlName, MAX_PATH);
	popstring(controlName);
	
	CControlUI *pControl = static_cast<CControlUI *>(g_pMainDlg->GetPaintManagerUI()->FindControl(controlName));

	wchar_t attu[MAX_PATH];
	ZeroMemory(attu, MAX_PATH);
	popstring(attu);

	if (pControl)
	{
		pControl->SetAttribute(_T("enabled"), attu);
	}
	//ExitProcess(0);
}




//NSISAPI StartInstall ( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
//{
//    NSMETHOD_INIT(hwndParent);
//    {
//        g_bMSGLoopFlag = false;
//    }
//}
//
//BOOL CALLBACK PluginNewWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    BOOL res = 0;
//    std::map<HWND, WNDPROC>::iterator iter = g_windowInfoMap.find( hwnd );
//
//    if( iter != g_windowInfoMap.end() ) {
//        if (message == WM_NCCREATE || message == WM_CREATE || message == WM_PAINT || message== WM_NCPAINT) {
//            ShowWindow( hwnd, SW_HIDE );
//        } else if( message == LVM_SETITEMTEXT ) {
//            ;
//        } else if( message == PBM_SETPOS ) {
//            CProgressUI *pProgress = static_cast<CProgressUI *>(g_pMainDlg->GetPaintManagerUI()->FindControl( g_progressCtrlName ));
//
//            if( pProgress == NULL )
//                return 0;
//
//            pProgress->SetMaxValue( 30000 );
//            pProgress->SetValue( (int)wParam);
//
//            if( pProgress->GetValue() == 30000 ) {
//                CTabLayoutUI *pTab = static_cast<CTabLayoutUI *>(g_pMainDlg->GetPaintManagerUI()->FindControl( g_tabLayoutCtrlName ));
//                if( pTab == NULL )
//                    return -1;
//
//                int currentIndex = pTab->GetCurSel();
//                DUI__Trace("tabName:%s index:%d",g_tabLayoutCtrlName,currentIndex);
//                pTab->SelectItem(2);
//                
//            }
//        } else {
//            res = CallWindowProc( iter->second, hwnd, message, wParam, lParam);
//        }
//    }
//
//    return res;
//}



//NSISAPI BindProgressControl ( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
//{
//    NSMETHOD_INIT(hwndParent);
//    {
//        char buf[512] = {0};
//        popstring( buf );
//        g_progressCtrlName = buf;
//
//        memset(buf, 0, 512);
//        popstring(buf);
//        g_tabLayoutCtrlName = buf;
//
//        // 接管page instfiles的消息.
//        g_windowInfoMap[hwndParent] = (WNDPROC) SetWindowLong(hwndParent, GWL_WNDPROC, (long) PluginNewWindowProc);
//        HWND hProgressHWND = FindWindowEx( FindWindowEx( hwndParent, NULL, _T("#32770"), NULL ), NULL, _T("msctls_progress32"), NULL );
//        g_windowInfoMap[hProgressHWND] = (WNDPROC) SetWindowLong(hProgressHWND, GWL_WNDPROC, (long) PluginNewWindowProc);
//        HWND hInstallDetailHWND = FindWindowEx( FindWindowEx( hwndParent, NULL, _T("#32770"), NULL ), NULL, _T("SysListView32"), NULL );
//        g_windowInfoMap[hInstallDetailHWND] = (WNDPROC) SetWindowLong(hInstallDetailHWND, GWL_WNDPROC, (long) PluginNewWindowProc);
//    }
//}

NSISAPI IsUSBDraver(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
	NSMETHOD_INIT(hwndParent);
	{
		wchar_t controlName[MAX_PATH];
		ZeroMemory(controlName, MAX_PATH);
		popstring(controlName);
		int nret = GetDriveType(controlName);
		if (nret == 2)
			pushint(0);
		else
			pushint(-1);
	}
	//ExitProcess(0);
}

NSISAPI LunchApp(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
	NSMETHOD_INIT(hwndParent);
	{
		int nindex =popint();
		wchar_t buf[1024] = { 0 };
		popstring(buf);

		if (nindex >= 0 && nindex < 3)
		{
			/*
			string mediaType;				//跳转类型
			string elementUrl;				//图片资源地址
			string jumpUrl;					//跳转地址					playUrl
			string cpid;					//CPID
			string fsp2;					//优酷播放地址
			*/
			Json::Value jParam;
			jParam["cpId"]=LeTools::Str2Numer(g_advInfo[nindex].cpid);
			jParam["mediaType"]= g_advInfo[nindex].mediaType;
			jParam["elementUrl"] =g_advInfo[nindex].elementUrl;
			jParam["url"] = g_advInfo[nindex].jumpUrl;
			jParam["fsp2"] = g_advInfo[nindex].fsp2;
			jParam["type"] = "inner";

			string seParame = jParam.toStyledString();
			wstring wsp = LeTools::s2ws(seParame.c_str());
			ShellExecute(NULL, L"open", buf, wsp.c_str(), NULL, SW_SHOW);
			pushint(0);
		}
		else
		{
			pushint(1);
		}
	}
	//ExitProcess(0);
}

NSISAPI UnFA(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
	NSMETHOD_INIT(hwndParent);
	{
		LeTools::UnFA();
	}
	//ExitProcess(0);
}

NSISAPI DuiMsb(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra)
{
	NSMETHOD_INIT(hwndParent);
	{
		wchar_t buf1[1024] = { 0 };
		wchar_t buf2[1024] = { 0 };
		popstring(buf1);
		popstring(buf2);

		::MessageBox(g_pMainDlg->GetHWND(), buf1, buf2, MB_OK);
	}
	//ExitProcess(0);
}
