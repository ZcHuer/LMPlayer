#include "MyHttp.h"
#include "stdafx.h"
#include "MainDlg.h"
#include <WinUser.h>
#include <io.h>
#include "AboutDlg.h"
#include "MainSetDlg.h"
#include "Aes_encryptor.h"
#include "base64.h"
#include <Winnetwk.h>
#include "FeedbackDlg.h"
#pragma comment(lib,"Mpr.lib")

const int RESIZE = 1440;

#ifdef DWMBLUR	//win7毛玻璃开关
#include <dwmapi.h>
#pragma comment(lib,"dwmapi.lib")
#endif
#include "base641.h"
#include "ctrl/ImageHeader.h"
#include "ShortcutHint.h"
#include "SwitchNewVer.h"
#include "Update/CUpdateHelper.h"
#include "../include/LeReport.h"
#include "CMDParser.h"

wstring m_strCachePath = L"";//缓存目录
wstring m_strDefaultPath = L"";
HWND CMainDlg::g_hMainWnd = NULL;
mutex mutex_tj;
map<int, string> mapKey;
extern SStringT g_strCmdLine;
_DoMain g_doMain;
SStringT g_strToken = L"";
SListDlg* m_dlgList;
extern wstring g_strRootPath;

//extern HWND g_hPlayerWnd;
CAes_encryptor CMainDlg::m_aesEncry;

//#define DEBUG_TEST_URL	1

CMainDlg::CMainDlg() : SHostWnd(_T("LAYOUT:XML_MAINWND"))
{
	m_bLayoutInited = FALSE;
	m_dlgPlayer = nullptr;	
	m_ptScreen.x = 0;
	m_ptScreen.y = 0;
	m_bListShow = FALSE;
	m_nOldWidth = 1920;	
	m_dlgList = nullptr;
	m_bMiniShow = FALSE;
	m_eSrcType = eSourceType_Unknown;
	m_bRefreshSucess = FALSE;
	m_bWebPlay = FALSE;
	CMainDlg::m_aesEncry.InitKey("2323456765456545", "2323456765456545");
}

CMainDlg::~CMainDlg()
{
	if (m_dlgPlayer)
	{
		m_dlgPlayer->DestroyWindow();
		delete m_dlgPlayer;
	}	
	if (m_dlgList)
	{
		m_dlgList->DestroyWindow();
		delete m_dlgList;
	}		
}

int CMainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
#ifdef DWMBLUR	//win7毛玻璃开关
	MARGINS mar = { 5,5,30,5 };
	DwmExtendFrameIntoClientArea(m_hWnd, &mar);
#endif

	SetMsgHandled(FALSE);
	return 0;
}

BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	//MessageBox(NULL,L"", L"", MB_OK);
	m_bLayoutInited = TRUE;

	mapKey[HK_OPENFILE] = "hk_openfile";
	mapKey[HK_OPENFLODER] = "hk_openfolder";
	mapKey[HK_PAUSE] = "hk_pause";
	mapKey[HK_STOP] = "hk_stop";
	mapKey[HK_QUIT] = "hk_quit";
	mapKey[HK_FORWARD] = "hk_forward";
	mapKey[HK_REWIND] = "hk_rewind";
	mapKey[HK_FORWARD30] = "hk_forward30";
	mapKey[HK_REWIND30] = "hk_rewind30";
	mapKey[HK_UPVOICE] = "hk_upvolume";
	mapKey[HK_DOWNVOICE] = "hk_dowmvolume";
	mapKey[HK_MUTE] = "hk_mute";
	mapKey[HK_FULLSCREEN] = "hk_full";

	//g_strCmdLine = L"zhLenovo://cid=637689679;token=null;";
	//g_strCmdLine = L"zhLenovo://cid=634433948;token=null;";

	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	dwStyle &= ~WS_CAPTION;
	::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);//设置成新的样式

	g_hMainWnd = m_hWnd;

	//初始化导航栏
	//InitTag();
	//消息隔离处理
	ChangeWindowMessageFilterOk();
	//初始化窗口创建
	InitCreate();
	//初始化窗口适配
	InitWndSize();	

	m_shellNotifyIcon.Create(m_hWnd, GETRESPROVIDER->LoadIcon(_T("ICON_LOGO2"), 16), L"LMPlayer");
	m_shellNotifyIcon.Show();

	CImageHeader* pimg = FindChildByName2<CImageHeader>(L"img_login");
	if (pimg)
	{
		pimg->SetAttribute(L"skin", L"default1");
	}	

	DragAcceptFiles(m_hWnd, TRUE);	

	//初始化列表窗口播放顺序
	::PostMessage(m_dlgList->m_hWnd, UM_UPDATE_SKIN, 0, 0);	
	//异步初始化处理
	thread thInit = thread(&CMainDlg::DoInit, this);
	thInit.detach();
	::PostMessage(m_dlgPlayer->m_hWnd, UM_RECHECK_TOP, 0, 0);
			
	FLOG(L"CMainDlg::OnInitDialog end");
	return 0;
}

void CMainDlg::InitWndSize()
{
	DEVMODE DevMode;
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &DevMode);
	m_ptScreen.x = DevMode.dmPelsWidth;
	m_ptScreen.y = DevMode.dmPelsHeight;

	int nX = GetSystemMetrics(SM_CXFULLSCREEN);
	int nY = GetSystemMetrics(SM_CYFULLSCREEN);
	int nWidth = 810;
	int nHeight = 560;
		
	SetWindowPos(NULL, (nX - nWidth)/2, (nY - nHeight)/2, nWidth, nHeight, 0);
	ReSetWnd();

	if (DevMode.dmPelsWidth <= 1366)
	{
		SWindow* pWnd = FindChildByName(L"btn_logo");
		//if (pWnd)pWnd->SetAttribute(L"skin", L"ng_logo");
		pWnd = FindChildByName(L"btn_login");
		if (pWnd)pWnd->SetAttribute(L"font", L"size:13");
		pWnd = FindChildByID(11);
		if (pWnd)pWnd->SetAttribute(L"font", L"size:15");
		pWnd = FindChildByID(12);
		if (pWnd)pWnd->SetAttribute(L"font", L"size:15");
	}
	else
	{
		SWindow* pWnd = FindChildByName(L"btn_logo");
		//if (pWnd)pWnd->SetAttribute(L"skin", L"ng_logo");
		pWnd = FindChildByName(L"btn_login");
		if (pWnd)pWnd->SetAttribute(L"font", L"size:16");
		pWnd = FindChildByID(11);
		if (pWnd)pWnd->SetAttribute(L"font", L"size:18");
		pWnd = FindChildByID(12);
		if (pWnd)pWnd->SetAttribute(L"font", L"size:18");
	}

	m_lStartTime =LeTools::GetTimeStamp();

	return;
}

void CMainDlg::DoInit()
{
	FLOG(L"CMainDlg::DoInit Begin");
	//初始化列表窗口播放顺序
	::PostMessage(m_dlgList->m_hWnd, UM_UPDATE_SKIN, 0, 0);
	//设置本地记录音量
	if (m_dlgPlayer) 
	{
		string sValue = "30";
		char* pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("volume");
		if (NULL != pchValue)
		{
			sValue = pchValue;
			CLeDB::GetInstance()->ReleaseStr(pchValue);
		}
		m_dlgPlayer->SetVolume(atoi(sValue.c_str()));
	}
	
	//如果不是记住密码，则程序再次启动，清空用户名记录
	string sKey = "";
	char* pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("passWord");
	if (NULL != pchValue)
	{
		sKey = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}
	if (/*stricmp(skey.c_str(), "null") == 0 || skey.length()<8*/sKey.length()==0) 
	{
		CLeDB::GetInstance()->Set()->UpdateSingleSet("userName", "");
	}

#ifndef DEBUG_TEST_URL
	//获取在线域名
	GetMainUrl();
#endif
	//默认文件关联
	LeTools::FA();
	
	//命令行处理
	DoCmdLine(g_strCmdLine.GetBuffer(0));

	// 设置报数器
	CLeReport::GetInstance()->SetDataServer(g_doMain.bigDataDomain.c_str(), "log/off_line_event");
	string strVer;
	LeTools::GetVersion(eLmplayer, strVer);	
	CLeReport::GetInstance()->SetVersion(strVer.c_str());
	wstring wstrChannel;
	LeTools::GetChannelID(eLmplayer, wstrChannel);
	CLeReport::GetInstance()->SetchannelID(LeTools::ws2s(wstrChannel).c_str());

	// 发送离线数据
	CLeReport::GetInstance()->SendCollectData();

	CUpdatehelper::GetInstance()->CheckUpdate();

	FLOG(L"CMainDlg::DoInit End");
}

void CMainDlg::HandleCmd_Active(wstring wstr_Active, eCmdSource eSource/* = eeCmdSource_Run*/)
{
	FLOG(L"CMainDlg::HandleCmd_Active begin");	
	FLOG(L"CMainDlg::HandleCmd_Active end");
}
//启动数据上报
void CMainDlg::HandleCmd_Run(wstring wstr_Run, eCmdSource eSource/* = eeCmdSource_Run*/)
{
	FLOG(L"CMainDlg::HandleCmd_Run begin");
	CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_RUN_TOTAL);
	
	// 悬浮球启动
	if (wstr_Run.find(CMD_RUN_PCM) != -1)
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_RUN_PCM);
	// 桌面快捷方式启动
	if (wstr_Run.find(CMD_RUN_DESKTOP) != -1)
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_RUN_DESKTOPLINK);
	// 开始菜单启动
	if (wstr_Run.find(CMD_RUN_STARTMENU) != -1)
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_RUN_STARTMENU);
	//开机启动
	if (wstr_Run.find(CMD_RUN_AUTORUN) != -1)
		ShowWindow(SW_HIDE);

	// 运行传入的命令行才上报，copydata传的不上报启动指令	
	if (eeCmdSource_Run == eSource)
	{
		CLeReport::GetInstance()->SetLaunchFrom(LeTools::ws2s(wstr_Run));
		CLeReport::GetInstance()->SendRTD_Eevent(RTD_RUNAPP, "1", "");
	}

	FLOG(L"CMainDlg::HandleCmd_Run end");
	return;
}

void CMainDlg::HandleCmd_Play(wstring wstr_Play, eCmdSource eSource/* = eeCmdSource_Run*/)
{
	FLOG(L"CMainDlg::HandleCmd_Play begin");
	bool bLocalFile = false;
	if (PathFileExists(wstr_Play.c_str()))
		bLocalFile = true;

	if (bLocalFile)
	{
		// 播放消息统一发给maindlg处理
		sPlayData* pData = new sPlayData;
		pData->eSignal = eSignalType_CmdLine;
		pData->eSrcType = eSourceType_Unknown;
		pData->strSource = wstr_Play.c_str();
		pData->bNoHistory = false;
		pData->strName = wstr_Play.c_str();
		pData->nNum = 0;
		pData->nTotal = 0;
		pData->bOnlyAddList = FALSE;
		pData->ifConsume = _T("2");
		if (bLocalFile)
		{
			wstring wstrName = wstr_Play;
			wstrName = wstrName.substr(wstrName.rfind(L"\\") + 1);
			pData->strName = wstrName;
			pData->sRD.videoId = LeTools::ws2s(wstrName);
			CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_RUN_FILEASS);
		}

		::PostMessage(CMainDlg::g_hMainWnd, UM_PLAY, (WPARAM)pData, 0);
	}	

	FLOG(L"CMainDlg::HandleCmd_Play end");
	return;
}

void CMainDlg::HandleCmd_Func(wstring wstr_Func, eCmdSource eSource/* = eeCmdSource_Run*/)
{
	FLOG(L"CMainDlg::HandleCmd_Func begin");
	// 支持联想ai项目的调用
	if (wstr_Func.find(L"lenovo-ai:play") != -1)
		m_dlgPlayer->PostMessageW(UM_AI_PLAY, 0, 0);

	if (wstr_Func.find(L"lenovo-ai:pause") != -1)
		m_dlgPlayer->PostMessageW(UM_AI_PAUSE, 0, 0);

	FLOG(L"CMainDlg::HandleCmd_Func end");
	return;
}

void CMainDlg::Native_Play(sPlayData* pPlayData)
{
	if (NULL == pPlayData)
		return;	

	if (m_dlgPlayer)
	{		
		pPlayData->sRD.ifSuccess= "";
		pPlayData->sRD.ifConsume = "2";
		//CLeReport::GetInstance()->SendRTD_Play("19", "1", "启动播放器", pPlayData->eSrcType, pPlayData->sRD);
		m_dlgPlayer->Play(pPlayData);
		pPlayData->sRD.ifSuccess = "1";
		pPlayData->sRD.ifConsume = "2";
		CLeReport::GetInstance()->SendRTD_Play("1", "1", "原生播放", m_dlgPlayer->m_sPlayData.eSrcType, pPlayData->sRD);
		pPlayData->sRD.ifSuccess = "";
	}
	m_eSrcType = pPlayData->eSrcType;

	// 标准模式下，才需要重置窗口，全屏和迷你都不动。
	if (m_dlgPlayer->GetPlayerShowMode() == ePlayerWndMode_Nomal)
	{
		//选择播放界面
	}
	else if (m_dlgPlayer->GetPlayerShowMode() == ePlayerWndMode_Mini)
	{
		if (m_dlgPlayer->IsMiniShow())
			m_dlgPlayer->SendMessage(WM_SYSCOMMAND, SC_RESTORE);
	}

	return;
}

void CMainDlg::Native_Stop()
{
	if (NULL == m_dlgPlayer)
		return;

	if (m_dlgPlayer->GetPlayStatus() == ePS_Playing)
		m_dlgPlayer->DoStop();

	// 处理播放记录
	HandleHistroy(&m_dlgPlayer->m_sPlayData);

	return;
}

void CMainDlg::DoCmdLine(wstring wstrCmdLine, eCmdSource eSource/* = eeCmdSource_Run*/)
{
	FLOG(L"CMainDlg::DoCmdLine begin");
	CCMDParser ccp;
	ccp.Parse(wstrCmdLine);
	if (ccp.IsRun())
		HandleCmd_Run(ccp.GetRunCmd(), eSource);
	if (ccp.IsPlay())
		HandleCmd_Play(ccp.GetPlayCmd(), eSource);
	if (ccp.IsFunc())
		HandleCmd_Func(ccp.GetFunCCmd(), eSource);	

	if (g_strCmdLine.Find(L"lenovo-ai:") != -1)
		PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE);

	FLOG(L"CMainDlg::DoCmdLine End");
	return;
}

// 从输入参数解析片源类型，但并不在成员中保存
eSourceType CMainDlg::ParseSourceType(wstring wstr)
{
	eSourceType eT = eSourceType_Local;
	wstring wsCmdLine = wstr;
	if (wsCmdLine.length() == 0)
		return eT;

	LPWSTR lpExtenson = PathFindExtension(wstr.c_str());

	return eT;
}

void CMainDlg::Play(sPlayData* pPlayData)
{
	FLOG(L"CMainDlg::Play begin");
	if (NULL == pPlayData)
		return;
	if (NULL == m_dlgPlayer)
		return;
	if (NULL == m_dlgList)
		return;	

	Native_Stop();
		
	// 如果传进来的数据并没分析源类型，则再次分析
	if (eSourceType_Unknown == pPlayData->eSrcType)
		pPlayData->eSrcType = ParseSourceType(pPlayData->strSource);	

	// 处理播放记录,开始播放时先处理一次，等结束时再处理一次，如果返回为false，则不再继续播放
	if (false == HandleHistroy(pPlayData))
		return;

	// 切换播放
	if (eSourceType_Local == pPlayData->eSrcType)
		Native_Play(pPlayData);	
	
	CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_PLAY_TOTAL);
		
	FLOG(L"CMainDlg::Play End");
	return;
}

bool CMainDlg::HandleHistroy(sPlayData* pPlayData)
{
	if (NULL == pPlayData)
		return false;
	if (pPlayData->eSrcType == eSourceType_Unknown)
		return false;

	// 本地播放添加播放列表，非本地播放要增加播放记录
	string sValue = "0";
	char* pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("noMark");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}
	int nNoMark = atoi(sValue.c_str());
	if (0 == nNoMark && false == pPlayData->bNoHistory)
	{
		if (eSourceType_Local == pPlayData->eSrcType)
		{
			if (eSignalType_PlayList != pPlayData->eSignal)
			{
				if (m_dlgList)
					m_dlgList->InsertList(pPlayData, !(pPlayData->bOnlyAddList));
				// 如果只需要添加到播放列表，则不再播放
				if (pPlayData->bOnlyAddList)
					return false;
			}
		}
	}
	return true;
}

void CMainDlg::GetDefalutKey()
{
	FLOG(L"GetDefalutKey begin");
	wstring sUrl =LeTools::s2ws(g_doMain.epgDomain);
	//https://epg.vgs.lenovo.com.cn/searchTop?lSrc=pcClient&relativeId=62
	sUrl.append(L"searchTop?lSrc=pcClient&relativeId=");
	sUrl.append(LeTools::s2ws(g_doMain.para2["relativeId"]));
	HttpRequest req(sUrl.c_str(), NULL, NULL);
	BOOL bSucc = req.doGet();
	if (bSucc)
	{
		string sResult = req.getStream().str();
		sResult =LeTools::ConvertUtf8ToGBK(sResult);
		Json::Reader jr;
		Json::Value jvData;
		if (jr.parse(sResult, jvData)) {
			if (jvData["code"].asInt() == 0)
			{
				string defaultSearch = jvData["data"]["defaultSearch"].asString();
				char* buff = new char[defaultSearch.length() + 1];
				memset(buff, 0, defaultSearch.length() + 1);
				memcpy(buff, defaultSearch.c_str(), defaultSearch.length());
				//::PostMessage(CMainDlg::g_hMainWnd, UM_DEFAULT_KEY, 0, (LPARAM)buff);
			}
		}
	}
	FLOG(L"GetDefalutKey end");
}

void CMainDlg::DoUpdate()
{
	CUpdatehelper::GetInstance()->ManualCheck();
	return;
}

void CMainDlg::HttpSendPlayInfo(string sData)
{
	wstring sUrl =LeTools::s2ws(g_doMain.bssDomain);
	string sResult =LeTools::doPost(sUrl, L"/playRecod", sData);
	sResult =LeTools::ConvertUtf8ToGBK(sResult);
	// 暂时没处理返回结果
	return;
}

void CMainDlg::HttpDelPlayInfo(string sData)
{
	wstring sUrl =LeTools::s2ws(g_doMain.bssDomain);
	string sResult =LeTools::doPost(sUrl, L"/deletePlayRecord", sData);
	sResult =LeTools::ConvertUtf8ToGBK(sResult);
	
	return;
}

void CMainDlg::GetMainUrl()
{
	wstring wsUrl;
	LeTools::GetDefaultMainUrl(eLmplayer, wsUrl);
	wsUrl.append(L"back/urls/leplayer/1");
	wsUrl.append(L"?mac=");
	wsUrl.append(LeTools::s2ws(LeTools::GetMacString()));
	HttpRequest req(wsUrl.c_str(), NULL, NULL);
	BOOL bSucc = req.doGet();
	if (bSucc)
	{
		string sResult = req.getStream().str();
		FLOG(L"获取域名：%s",LeTools::s2ws(sResult).c_str());
		sResult =LeTools::ConvertUtf8ToGBK(sResult);		
		Json::Reader jr;
		Json::Value jvData;
		if (jr.parse(sResult, jvData)) 
		{
			if (jvData["code"].asInt() == 0)
			{
				Json::Value jv = jvData["data"];
				g_doMain.epgDomain = jv["epgDomain"].asString();
				g_doMain.bssDomain = jv["bssDomain"].asString();
				g_doMain.searchDomain = jv["searchDomain"].asString();
				g_doMain.bigDataDomain = jv["bigDataDomain"].asString();
				g_doMain.foundPageURL = jv["foundPageURL"].asString();
				g_doMain.isForceUpdate = jv["isForceUpdate"].asBool();
				Json::Value Jv_para2 = jv["para2"];
				Json::Value::Members members = Jv_para2.getMemberNames();
				for (Json::Value::Members::iterator iterMember = members.begin(); iterMember != members.end(); iterMember++)   // 遍历每个key
				{
					string strFirst = *iterMember;
					g_doMain.para2[strFirst] = Jv_para2[strFirst].asString();
				}
				g_doMain.para = jv["para"].asString();
				//获取默认关键字
				GetDefalutKey();
				//FLOG(L"获取域名成功1：%s",LeTools::s2ws(g_doMain.epgDomain).c_str());
				//FLOG(L"获取域名成功2：%s",LeTools::s2ws(g_doMain.bssDomain).c_str());
				//FLOG(L"获取域名成功3：%s",LeTools::s2ws(g_doMain.searchDomain).c_str());
				//FLOG(L"获取域名成功4：%s",LeTools::s2ws(g_doMain.bigDataDomain).c_str());

			}
		}
	}
}

void CMainDlg::InitCreate()
{
	FLOG(L"InitCreate begin ")
	//创建播放窗口
	if (NULL == m_dlgPlayer) 
	{
		m_dlgPlayer = new CPlayerDlg;
		m_dlgPlayer->Create(m_hWnd);
		m_dlgPlayer->SendMessageW(WM_INITDIALOG);
		SetParent(m_dlgPlayer->m_hWnd, m_hWnd);
	}

	if (NULL == m_dlgList) 
	{
		m_dlgList = new SListDlg;
		m_dlgList->Create(m_hWnd);
		m_dlgList->SendMessage(WM_INITDIALOG);
		m_dlgList->ShowWindow(SW_HIDE);
		::SetParent(m_dlgList->m_hWnd, m_hWnd);
	}

	ReSetWnd();
	FLOG(L"InitCreate end");
	return;
}

/*取消消息隔离*/
void CMainDlg::ChangeWindowMessageFilterOk()
{
	typedef BOOL(WINAPI*pfnChangeWindowMessageFilter)(UINT message, DWORD dwFlag);

	typedef BOOL(WINAPI* pfnAddClipboardFormatListener)(HWND);
	HMODULE hLib = LoadLibrary(L"User32.dll");//动态加载User32.dll
	if (hLib) {
		pfnChangeWindowMessageFilter pfn = (pfnChangeWindowMessageFilter)::GetProcAddress(hLib, "ChangeWindowMessageFilter");
		if (NULL != pfn) {
			pfn(WM_DROPFILES, 1);
			pfn(0x0049, 1);//WM_COPYGLOBALDATA
			pfn(WM_KEYDOWN, 1);
			pfn(WM_KEYUP, 1);
			pfn(WM_SYSKEYDOWN, 1);
			pfn(WM_SYSKEYUP, 1);
			pfn(WM_SHOWWINDOW, 1);
			pfn(WM_COPYDATA, 1);
		}
		//添加粘贴板监控消息
		pfnAddClipboardFormatListener pfnAdd = (pfnAddClipboardFormatListener)::GetProcAddress(hLib, "AddClipboardFormatListener");
		if (pfnAdd)
		{
			pfnAdd(m_hWnd);
		}
	}
}

void CMainDlg::ReSetWnd()
{
	if (NULL == m_dlgPlayer)
		return;
	if (NULL == m_dlgList)
		return;

	CRect rcClient = GetClientRect();
	int nWidth = rcClient.Width();	
		
	if (ePlayerWndMode_Nomal == m_dlgPlayer->GetPlayerShowMode())
	{
		//正常模式
		if (m_bListShow)
		{
			m_dlgList->SetWindowPos(NULL, rcClient.Width() - WIDTH_PLAYLIST - WIDTH_LEFTFRAME - WIDTH_SPLITER, HEIGHT_TITLE + HEIGHT_TOPFRAME, WIDTH_PLAYLIST, rcClient.Height() - HEIGHT_TITLE - HEIGHT_TOPFRAME - HEIGHT_BOTTOMFRAME, SWP_SHOWWINDOW);
			nWidth = nWidth - WIDTH_PLAYLIST - WIDTH_RIGHTFRAME - WIDTH_SPLITER - WIDTH_LEFTFRAME;
		}
		else
		{
			nWidth = nWidth - WIDTH_RIGHTFRAME - WIDTH_LEFTFRAME;
		}
		m_dlgPlayer->SetPlayerShowMode(ePlayerWndMode_Nomal);
		FLOG(L"resetWnd show:%d :%d ", nWidth, rcClient.Height() - HEIGHT_TITLE - HEIGHT_TOPFRAME - HEIGHT_BOTTOMFRAME);
		m_dlgPlayer->SetWindowPos(NULL, WIDTH_LEFTFRAME, HEIGHT_TITLE + HEIGHT_SPLITER, nWidth, rcClient.Height() - HEIGHT_TITLE - HEIGHT_TOPFRAME - HEIGHT_BOTTOMFRAME, SWP_SHOWWINDOW);
	}

	bool bTop = m_dlgPlayer->GetTopMostSet();
	SWindow *pBtnZhiding = FindChildByName(L"btn_zhiding");
	SWindow *btn_buzhiding = FindChildByName(L"btn_buzhiding");	
	pBtnZhiding->SetVisible(!bTop);
	btn_buzhiding->SetVisible(bTop);

	return;
}

void CMainDlg::TrueClose()
{	
	m_dlgPlayer->ShowCtrlWnd(FALSE);
	m_dlgPlayer->DoStop();
	ShowWindow(SW_HIDE);
	//m_shellNotifyIcon.Hide();
	//退出时清空播放记录
	char chTmp[MAX_PATH] = { 0 };
	string sValue = "0";
	char* pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("quitClear");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}	
	int nValue = atoi(sValue.c_str());
	if (nValue)
		CLeDB::GetInstance()->Net()->DelPlayRecodEx(FALSE);

	CLeDB::GetInstance()->Set()->UpdateSingleSet("volume",LeTools::Num2Str(m_dlgPlayer->m_nVolume).c_str());

	//如果不是记住密码，则程序再次启动，清空用户名记录
	memset(chTmp, 0, MAX_PATH);
	string skey = "";
	pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("passWord");
	if (NULL != pchValue)
	{
		skey = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}	
	if (stricmp(skey.c_str(), "null") == 0) 
	{
		CLeDB::GetInstance()->Set()->UpdateSingleSet("userName", "");
	}
	m_shellNotifyIcon.Hide();

	CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_QUIT);
	LeTools::KillProcess(L"lmp.exe");

	return;
}

void CMainDlg::OnMenuBtn()
{
	FLOG(L"点击LOGO，打开设置菜单");
	CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_CLICK_MENU);
	CRect rcWnd = GetWindowRect();
	CPoint pt;
	pt.x = rcWnd.left;
	pt.y = rcWnd.top;

	SWindow* pWnd = FindChildByName(L"btn_set");
	if (pWnd)
	{
		rcWnd = pWnd->GetWindowRect();
		pt.x += rcWnd.left;
		pt.y += rcWnd.bottom;
		pt.y += 10;
	}

	SMenuEx menu;
	// 判断升级进程是否存在,如果后台静默升级中，则弹出的菜单，没有手动升级这一菜单
	if (CUpdatehelper::GetInstance()->IsUpdateing())
		menu.LoadRes(_T("smenuex:logo_menuex2"));
	else
		menu.LoadRes(_T("smenuex:logo_menuex"));
	menu.LoadMenu();
	menu.TrackPopupMenu(0, pt.x, pt.y, m_hWnd);	
}

void CMainDlg::OnTimer(UINT_PTR nIDEvent)
{
	SetMsgHandled(FALSE);
	if (121 == nIDEvent)
	{
		//HANDLE hProcess = GetCurrentProcess();
		//SetProcessWorkingSetSize(hProcess, -1, -1);
	}		
}

void CMainDlg::OnMouseMove(UINT nFlags, CPoint point)
{	
	return;
}


void CMainDlg::OnKillFocus(HWND wndFocus)
{
	SetMsgHandled(FALSE);
	POINT pNow;
	GetCursorPos(&pNow);  //获取鼠标当前位置
	HWND hwndPointNow = WindowFromPoint(pNow);  //获取鼠标所在窗口的句柄
}

void CMainDlg::OnDropFiles(HDROP hDropInfo)
{
	if (m_dlgPlayer)
	{
		::PostMessage(m_dlgPlayer->m_hWnd, WM_DROPFILES, (WPARAM)hDropInfo, 0);
	}
}

void CMainDlg::OnCommand(UINT uNotifyCode, int nID, HWND wndCtl)
{
	if (uNotifyCode == WM_SYSCOMMAND)
	{
		int a = 0;
	}
	if (999 == nID)
	{
		LONGLONG tmp =LeTools::GetTimeStamp();
		__int64 userTime = tmp - m_lStartTime;
		CLeReport::GetInstance()->SendRTD_Eevent(RTD_CLOSEAPP, "1", "托盘退出", "","", 0, userTime);
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_QUIT_TROY);		
		TrueClose();
		return;
	}
	switch (nID)
	{
	case OPEN_FILE:
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_CLICK_MENU_FILE);
		m_dlgPlayer->OnOpenfile(0);		
		break;
	case CMD_About: //关于LMPlayer
	{
		if (false == CAboutDlg::m_bShow)
		{
			CAboutDlg dlg;
			dlg.DoModel();
		}
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_CLICK_MENU_ABOUT);
		break;
	}
	case CMD_Update: //在线升级
	{
		thread td = thread(&CMainDlg::DoUpdate, this);
		td.detach();
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_CLICK_MENU_UPDATE);
		break;
	}
	case CMD_Feedback: //意见反馈
	{
		//CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_CLICK_MENU_FEEDBACK);
		//CFeedbackDlg dlg;
		//dlg.DoModel();
		ShellExecute(NULL, L"open", L"iexplore.exe", L"http://sta.vgs.lenovo.com.cn/lmplayer.html", 0, SW_SHOW);
		break;
	}
	case CMD_Setting: //设置
	{
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_CLICK_MENU_SET);
		if (false == CMainSetDlg::m_bShow)
		{
			
			CMainSetDlg dlg;
			dlg.DoModel();
		}		
		break;
	}	
	case CMD_QUIT: //退出
	{
		LONGLONG tmp =LeTools::GetTimeStamp();
		__int64 userTime = tmp - m_lStartTime;
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_CLICK_MENU_QUIT);
		CLeReport::GetInstance()->SendRTD_Eevent(RTD_CLOSEAPP, "1", "关闭应用", "", "", 0, userTime);
		TrueClose();
		break;
	}
	default:
		break;
	}

	//::PostMessage(m_dlgPlayer->m_hWnd, WM_COMMAND, MAKEWPARAM(nID, uNotifyCode), 1);
	return;
}

void CMainDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	SetMsgHandled(FALSE);
	bool bCtrl = FALSE;
	if (GetAsyncKeyState(VK_CONTROL) >> ((sizeof(SHORT) * 8) - 1))
		bCtrl = TRUE;
	
	if (m_dlgPlayer)
	{
		UINT lParam = (nFlags << 16) | nRepCnt;
		::PostMessage(m_dlgPlayer->m_hWnd, WM_KEYDOWN, nChar, lParam);
	}
}

void CMainDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
}

void CMainDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetMsgHandled(FALSE);
	
}

void CMainDlg::ShowShortcutHint()
{
	if (IsDesktopHaveShortcut())
		return;
	if (false == LeTools::GetUserShortcutSelect())
		return;
	CShortcutHintDlg dlg;
	dlg.DoModel();
	
	if (true == CShortcutHintDlg::m_bCreateShortcut)
		LeTools::CreateDeskShortcut(eLmplayer);
	else
	{
		if (CShortcutHintDlg::m_bNoHint)
			LeTools::SetUserShortctuSelect(false);
	}		

	return;
}

bool CMainDlg::IsDesktopHaveShortcut()
{
	BOOL bRet = FALSE;
	LPITEMIDLIST pidl;
	LPMALLOC pShellMalloc;
	WCHAR szDir[MAX_PATH] = { 0 };
	if (SUCCEEDED(SHGetMalloc(&pShellMalloc)))
	{
		if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl))) {
			// 如果成功返回true
			bRet = SHGetPathFromIDListW(pidl, szDir);
			pShellMalloc->Free(pidl);
		}
		pShellMalloc->Release();
	}
	//未获取到桌面路径，因为再尝试也是失败,只能返回true，等同于已经有了，后续就不再生成了。
	if (!bRet)
		return true;
	
	WCHAR wPath[MAX_PATH] = { 0 };
	wsprintf(wPath, L"%s\\%s.lnk", szDir, L"LMPlayer");

	// 判断快捷方式是否存在
	if (_waccess(wPath, 0) == -1)
		return false;

	return true;
}

LRESULT CMainDlg::OnCopyData(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	bool bActivation = false;

	COPYDATASTRUCT* cpd = (COPYDATASTRUCT*)lParam;
	if (/*因为直接点击exe调起时命令行长度为0，所以这里不能判断这个 cpd->cbData != 0 && */cpd->dwData == UM_COPYDATA_CMDLING)
	{
		FLOG(L"OnCopydata UM_COPYDATA_CMDLING");
		char* lpBuff = new char[cpd->cbData + 1];
		memset(lpBuff, 0, cpd->cbData + 1);
		memcpy(lpBuff, cpd->lpData, cpd->cbData);
		DoCmdLine(LeTools::s2ws(lpBuff).c_str());
		FLOG(_T("OnCopyData cmdline：%s"),LeTools::s2ws(lpBuff).c_str());
		delete[] lpBuff;
		lpBuff = nullptr;

		bActivation = true;
	}	
	
	if (bActivation)
	{
		// 激活lmplayer
		if (m_bMiniShow)
		{
			m_dlgPlayer->SendMessageW(WM_SYSCOMMAND, SC_RESTORE);
			SendMessageW(WM_SYSCOMMAND, SC_RESTORE);
		}

		ReSetWnd();
		SetForegroundWindow(m_hWnd);

		SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		SetWindowPos(HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		if (m_dlgPlayer)
			::PostMessage(m_dlgPlayer->m_hWnd, UM_RECHECK_TOP, 0, 0);

		// 如果是命令行形式启动，就是激活了客户端，发送了copydata，需要检查一下是否开机启动，继续升级检查
		CUpdatehelper::GetInstance()->WhenLeplayerActivation();
	}	

	return 0;
}
void CMainDlg::OnMove(CPoint ptPos)
{
	SetMsgHandled(FALSE);
	if (m_dlgPlayer)
		m_dlgPlayer->PostMessageW(WM_MOVE);	
	return;
}

LRESULT CMainDlg::OnIconNotify(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SMenu menu;
	switch (lParam)
	{
	case WM_RBUTTONDOWN:
	{
		if (CUpdatehelper::GetInstance()->IsUpdateing())
			menu.LoadMenu(_T("smenu:tray_menu2"));
		else
			menu.LoadMenu(_T("smenu:tray_menu"));
		
		POINT pt;
		GetCursorPos(&pt);
		menu.TrackPopupMenu(0, pt.x, pt.y, m_hWnd);
		break;
	}
	case WM_LBUTTONDOWN:
	{
		if (m_dlgPlayer->GetPlayerShowMode() == ePlayerWndMode_Nomal)
		{
			SendMessageW(WM_SYSCOMMAND, SC_RESTORE);
			m_dlgPlayer->SendMessageW(WM_SYSCOMMAND, SC_RESTORE);
			CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_CLICK_TUOPAN);
			CLeReport::GetInstance()->SendRTD_Eevent(RTD_RUNAPP, "1", "tray");
			CUpdatehelper::GetInstance()->WhenLeplayerActivation();
		}
		else if (m_dlgPlayer->GetPlayerShowMode() == ePlayerWndMode_Mini)
		{
			m_dlgPlayer->SendMessageW(WM_SYSCOMMAND, SC_RESTORE);
		}		
	}
	break;
	default:
		break;
	}
	return S_OK;
}

LRESULT CMainDlg::WndPro(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{	
	case WM_POWERBROADCAST:
	{
		FLOG(L"电源接通");
		if (LeTools::IsNetworkAlive())
		{
			FLOG(L"网络连接");
		}
		else
		{
			FLOG(L"网络不通");
		}
		break;
	}
	case UM_PLAY:
	{
		sPlayData* p = (sPlayData*)wParam;		
		if (p)
		{
			Play(p);
			delete p;			
		}
	}
	break;
	case UM_STOP:
	{		
		ReSetWnd();
	}
	break;
	case UM_PLAY_LAST:
	{
		OnLastOne();
	}
	break;
	case UM_PLAY_NEXT:
	{
		OnNextOne();
	}
	break;	
	case UM_SHOW_LISTWND:
	{
		m_bListShow = lParam;
		ReSetWnd();
		CLeReport::GetInstance()->SaveData_Count(m_bListShow ? ERD_COUNT_CLICK_SHOWRIGHT : ERD_COUNT_CLICK_HIDERIGHT);
		break;
	}
	case UM_SETMIMI_WND:
	{
		if (wParam == 1) 
			ShowWindow(SW_SHOWNORMAL);		

		ReSetWnd();
	}
	break;
	case UM_MIGUI_HIDE:
	{
		ReSetWnd();		
	}
	break;	
	case UM_CLEAR_LIST:
	{
		if (m_dlgList) 
		{
			m_dlgList->DoClearList();
		}
	}break;
	case UM_CLOSE:
	{
		TrueClose();
	}break;
	case UM_RESETWND:
	{
		//SetTitleCulSel((eTab_CurSel)wParam);
		ReSetWnd();
	}
	break;	
	case UM_SHOWMAIN:
	{
		if ((int)lParam == 0)
			ShowWindow(SW_SHOWNORMAL);
		else if ((int)lParam == 1)
			ShowWindow(SW_HIDE);
		
		break;
	}
	case UM_MINI_CLOSE:
	{
		OnClose();
		break;
	}	
	default:
		break;
	}
	return 0;
}

void CMainDlg::OnLastOne()
{
	if (NULL == m_dlgPlayer)
		return;	

	if (eSourceType_Local == m_dlgPlayer->m_sPlayData.eSrcType)
		m_dlgList->PlayLast();
}

void CMainDlg::OnNextOne()
{
	if (NULL == m_dlgPlayer)
		return;
	sReportData sRd;
	CLeReport::GetInstance()->SendRTD_Play("7", "1", "点击下一集", m_dlgPlayer->m_sPlayData.eSrcType, m_dlgPlayer->m_sPlayData.sRD);
	
	if (eSourceType_Local == m_dlgPlayer->m_sPlayData.eSrcType)
	{
		BOOL bOK = m_dlgList->PlayNext();
		if (!bOK)
			m_dlgPlayer->DoStop();
	}
}

//
void CMainDlg::OnClose()
{
	FLOG(L"点击关闭按钮");
	CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_CLICK_CLOSE);
	SetMsgHandled(TRUE);

	//if (m_dlgPlayer->GetPlayerShowMode() == ePlayerWndMode_Mini)
	//::PostMessage(m_dlgPlayer->m_hWnd, UM_MINI_CLOSE, 0, 0);

	char chTmp[MAX_PATH] = { 0 };
	string sValue = "0";
	char* pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("quit");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}	
	if (!atoi(sValue.c_str()))
	{
		string sValue = "1";
		char* pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("minPause");
		if (NULL != pchValue)
		{
			sValue = pchValue;
			CLeDB::GetInstance()->ReleaseStr(pchValue);
		}
		
		if (atoi(sValue.c_str()))
		{
			if (ePS_Playing == m_dlgPlayer->GetPlayStatus() || ePS_Opening == m_dlgPlayer->GetPlayStatus())
				m_dlgPlayer->OnBtnPlay();
		}
		m_dlgPlayer->ShowCtrlWnd(FALSE);
		m_dlgPlayer->ShowWindow(SW_HIDE);
		ShowWindow(SW_HIDE);		
		CLeReport::GetInstance()->SendRTD_Play("30", "1", "退出到托盘", m_dlgPlayer->m_sPlayData.eSrcType,m_dlgPlayer->m_sPlayData.sRD);
		return;
	}
	else
	{
		//计算时长
		LONGLONG tmp =LeTools::GetTimeStamp();
		__int64 userTime = tmp - m_lStartTime;
		CLeReport::GetInstance()->SendRTD_Eevent(RTD_CLOSEAPP, "1", "关闭应用", "", "", 0, userTime);
		TrueClose();
	}
}

void CMainDlg::OnMaximize()
{
	SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
	m_bMiniShow = FALSE;
	CLeReport::GetInstance()->SendRTD_Play("30", "1", "最大化", m_dlgPlayer->m_sPlayData.eSrcType, m_dlgPlayer->m_sPlayData.sRD);
	CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_CLICK_MAX);
}

void CMainDlg::OnRestore()
{
	SendMessage(WM_SYSCOMMAND, SC_RESTORE);
	m_bMiniShow = FALSE;
	CLeReport::GetInstance()->SendRTD_Eevent("35", "1", "最大化还原");
}

void CMainDlg::OnMinimize()
{
	if (NULL == m_dlgPlayer)
		return;

	m_bMiniShow = TRUE;
	CLeReport::GetInstance()->SendRTD_Play("30", "1", "最小化", m_dlgPlayer->m_sPlayData.eSrcType, m_dlgPlayer->m_sPlayData.sRD);
	FLOG(L"点击最小化按钮");
	
	if (m_dlgPlayer->GetPlayerShowMode() == ePlayerWndMode_Nomal)
	{
		char chTmp[MAX_PATH] = { 0 };
		string sValue = "1";
		char* pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("minPause");
		if (NULL != pchValue)
		{
			sValue = pchValue;
			CLeDB::GetInstance()->ReleaseStr(pchValue);
		}		
		if (atoi(sValue.c_str()))
			m_dlgPlayer->Min2Pause();
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
	}
	else if (m_dlgPlayer->GetPlayerShowMode() == ePlayerWndMode_Mini)
	{	
		//ShowWindow(SW_HIDE);
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
		if (false == m_dlgPlayer->IsMiniShow())
		{
 			m_dlgPlayer->SendMessage(WM_SYSCOMMAND, SC_RESTORE);
			::SetForegroundWindow(m_dlgPlayer->m_hWnd);			
		}
	}
	CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_CLICK_MIN);
	return;
}

void CMainDlg::Onzhiding()
{
 	SWindow *pBtnZhiding = FindChildByName(L"btn_zhiding");
 	pBtnZhiding->SetVisible(FALSE);
 	SWindow *btn_buzhiding = FindChildByName(L"btn_buzhiding");
 	btn_buzhiding->SetVisible(TRUE);
	m_dlgPlayer->SetTopMost(true);
}

void CMainDlg::Onbuzhiding()
{
 	SWindow *pBtnZhiding = FindChildByName(L"btn_zhiding");
 	pBtnZhiding->SetVisible(TRUE);
 	SWindow *btn_buzhiding = FindChildByName(L"btn_buzhiding");
 	btn_buzhiding->SetVisible(FALSE);
	m_dlgPlayer->SetTopMost(false);
}

void CMainDlg::Onminimode()
{
	m_dlgPlayer->OnMiniMode();	
}

void CMainDlg::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);
	if (!m_bLayoutInited) 
		return;

	SWindow *pBtnMax = FindChildByName(L"btn_max");
	SWindow *pBtnRestore = FindChildByName(L"btn_restore");
	if (!pBtnMax || !pBtnRestore) 
		return;

	if (nType == SIZE_MAXIMIZED)
	{
		pBtnRestore->SetVisible(TRUE);
		pBtnMax->SetVisible(FALSE);

		RECT rect;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
		int nX = rect.right - rect.left;
		int nY = rect.bottom - rect.top;

		SetWindowPos(NULL, 0, 0, nX, nY, SWP_NOMOVE | SWP_NOZORDER);
		CenterWindow(m_hWnd);
	}
	else if (nType == SIZE_RESTORED)
	{
		//OnRestore();
		m_bMiniShow = FALSE;
		pBtnRestore->SetVisible(FALSE);
		pBtnMax->SetVisible(TRUE);
	}
	else if (SIZE_MINIMIZED == nType)
	{
		//最小化
		OnMinimize();
	}
	ReSetWnd();	
}

bool CMainDlg::IsNewVerReady()
{
	// 获取当前配置文件路径
	WCHAR wcPath_Cfg[MAX_PATH] = { 0 };
	::GetModuleFileName(NULL, wcPath_Cfg, MAX_PATH);
	PathRemoveFileSpecW(wcPath_Cfg);
	PathAppendW(wcPath_Cfg, L"\\..\\Config.ini");

	// 获取当前版本号
	WCHAR wcCurVer[MAX_PATH] = { 0 };
	::GetPrivateProfileStringW(L"Update", L"CurVer", L"", wcCurVer, MAX_PATH, wcPath_Cfg);

	// 获取新版本号
	WCHAR wcNewVer[MAX_PATH] = { 0 };
	::GetPrivateProfileStringW(L"Update", L"NewVer", L"", wcNewVer, MAX_PATH, wcPath_Cfg);

	if (0 != StrCmpW(wcCurVer, wcNewVer) && lstrlen(wcNewVer) != 0)
		return true;

	return false;
}

void CMainDlg::RunNewVer()
{
	// 提示切换
	SwitchNewVer dlg;
	dlg.DoModel();

	// 切换
	// 启动launch程序，由launch程序来选择新版本
	WCHAR wcMoudlePath[MAX_PATH] = { 0 };
	::GetModuleFileNameW(NULL, wcMoudlePath, MAX_PATH);
	PathRemoveFileSpecW(wcMoudlePath);
	PathRemoveFileSpecW(wcMoudlePath);
	wstring wWorkPath = wcMoudlePath;
	PathAppendW(wcMoudlePath, L"\\LMPlayer.exe");

	if (PathFileExistsW(wcMoudlePath))
		::ShellExecuteW(NULL, L"open", wcMoudlePath, L"runnew", wWorkPath.c_str(), SW_SHOW);
	return;
}