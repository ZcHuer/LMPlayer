#include "stdafx.h"
#include "PlayerDlg.h"
#include "FilesFormat.h"
#include "SetDlg.h"
#include "AboutDlg.h"
#include "MainSetDlg.h"
#include "MainDlg.h"
#include "event\EventSubscriber.h"
#include "SysAlive.h"
#include "Playfailed.h"
#include "base641.h"
#include "../include/LeReport.h"

long g_lDefaultTime = 6 * 60;
extern mutex mutex_tj;
extern map<int, string> mapKey;
extern SStringT g_strToken;
extern _DoMain g_doMain;
extern SListDlg* m_dlgList;
extern wstring g_strRootPath;
HWND g_hPlayerWnd = NULL;
CPlayerDlg* CPlayerDlg::m_pThis = nullptr;
__int64 g_llSetTime = -1;
BOOL g_bMpc = FALSE;


ePlayStatus CPlayerDlg::m_ePlayerStatus;

CPlayerDlg::CPlayerDlg() : SHostWnd(_T("LAYOUT:XML_PLAYERWND"))
, m_bPlayingTopflag(false)
{
    m_bLayoutInited = FALSE;
    m_nVolume = 0;
    m_bMute = FALSE;
    m_pThis = this;
    m_bTigShow = FALSE;
    m_bClickTimer = FALSE;
	m_bMoveFlag = FALSE;
    m_pFlash = nullptr;
    m_bGifLoading = FALSE;
    m_bShowWeb = FALSE;
    m_bSetChannel = FALSE;
    m_bSetOpen = FALSE;
    m_bShowLink = FALSE;
    m_llResolutionTime = 0;
    m_bResolution = FALSE;
    m_nResolu = 2;
    m_nTotalTeleplay = 0;
    m_nMiguPlayID = 0;
    m_strIsVip = "2";
	m_bMiniShow = FALSE;
    m_sMd5 = "";
    m_dlgCtrl = NULL;
    m_bNoMark = FALSE;
	m_ePlayerStatus = ePS_Init;	
	m_nIncrement = 0;
	m_eShowMode = ePlayerWndMode_Init;
	m_eShowMode_Old = ePlayerWndMode_Init;
	m_eFileType = eLocalFile_Default;
	m_hThread_GetFunTVAdress = INVALID_HANDLE_VALUE;
	m_hMod_FunTV = NULL;
	pFunTVAdress = NULL;
	pFunTV_Stop = NULL;
	m_bFunTvHaveGet = false;
	m_pFirstData = NULL;
	m_pSecondData = NULL;
	m_bShowRight = NULL;
	m_sPlayData.eSignal = eSignalType_UnKnown;
	m_sPlayData.eSrcType = eSourceType_Unknown;
	m_sPlayData.strName = L"";
	m_sPlayData.strSource = L"";
	m_sPlayData.nNum = 0;
	m_sPlayData.nTotal = 0;
	m_sPlayData.currPlayTime = 0;
	m_sPlayData.lTotalTime = 0;
	m_sMiniRect.bInited = false;
	m_sMiniRect.cRect_Mini.left = 0;
	m_sMiniRect.cRect_Mini.top = 0;
	m_sMiniRect.cRect_Mini.right = 0;
	m_sMiniRect.cRect_Mini.bottom = 0;
	m_sMiniRect.bSetModeOver = false;
	m_bPlayerCreated = false;
}

CPlayerDlg::~CPlayerDlg()
{
	// 释放播放器线程
	ReleasePlayer();

	if (m_hMod_FunTV)
	{
		FreeModule(m_hMod_FunTV);
		m_hMod_FunTV = NULL;
	}
	//::DestroyWindow(m_CPlayer.GetPlayerWnd());
}

void CPlayerDlg::OnPlayStart(int nDuration)
{
	// 启动防休眠线程	
	SysAlive::Alive();
	ShowLoading(FALSE);
	if (m_dlgList->m_hWnd && !m_bNoMark)
		::PostMessage(m_dlgList->m_hWnd, UM_REDRAW_LISTWND_NOSORT, (WPARAM)m_sMd5.c_str(), 0);

	SetPlayStatus(ePS_Playing);
	//m_sPlayData.sRD.ifSuccess = "1";
	//隐藏进度条以及播放时间归零
	SSliderBar* pSliderBar = FindChildByName2<SSliderBar>(L"slider_player");
	if (pSliderBar)
		pSliderBar->SetAttribute(L"show", L"1");

	string sValue = "0";
	char* pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("noMark");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}
	
	m_bNoMark = atoi(sValue.c_str());
	m_sPlayData.lTotalTime = nDuration;

	//初始播放需要判断是否跳到播放记录
	WhenStartPlay();
	long lPlayed = 0;	
	string sPath =LeTools::ws2s(m_sPlayData.strSource);
	// 本地文件播放进度从本地数据库读取/网络视频从网络视频数据库表中查询
	if (m_sPlayData.eSrcType == eSourceType_Local)
		lPlayed = CLeDB::GetInstance()->Local()->GetPlayProcess_ByMd5_Local(LeTools::ws2s(m_sPlayData.sMd5).c_str());

	// 离总时长不超过10秒了，则认为结束了，需要重新播放，增加边界处理强壮性
	if (lPlayed + 10 >= m_sPlayData.lTotalTime)
	{
		lPlayed = 0;
	}

	if (lPlayed > 0)
		m_CPlayer.Seek(lPlayed);
	
	if (!m_bResolution && !m_llResolutionTime)
	{
		//设置进度条最大值
		SetSliderMax();
		//显示播放时间
		SetShowTime(0, m_sPlayData.lTotalTime);
	}

	//修正屏幕比例	
	pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("screenRatio");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}
	
	SetScreenRatio(atoi(sValue.c_str()) + SCREEN_NORMAL, FALSE);

	if (m_sPlayData.eSrcType == eSourceType_Local)
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_PLAY_FILESUCCESS);
	else
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_PLAY_NETSUCCESS);

	return;
}

void CPlayerDlg::OnPlaying(int nPlayPos, int nDuration)
{
	int nPos = nPlayPos;
	if (m_sPlayData.currPlayTime != nPos)
	{
		if (0 != m_nIncrement)
		{
			if (abs(nPos - m_sPlayData.currPlayTime) > 3)
				m_nIncrement = 0;
		}
		m_sPlayData.currPlayTime = nPos;
		SetSliderPos();
	}

	if (m_sPlayData.lTotalTime != nDuration)
	{
		m_sPlayData.lTotalTime = nDuration;
		// 设置进度条总长
		SetSliderMax();
	}
	SetShowTime(m_sPlayData.currPlayTime, m_sPlayData.lTotalTime);

	static int nOldTime = -1;
	if (nOldTime != m_sPlayData.currPlayTime)
	{
		if (!m_bNoMark && 0 != m_sPlayData.currPlayTime)
		{
			if (eSourceType_Local == m_sPlayData.eSrcType)
				CLeDB::GetInstance()->Local()->UpdateLocalFileList(LeTools::ws2s(m_sPlayData.sMd5).c_str(), m_sPlayData.currPlayTime, m_sPlayData.lTotalTime, 0);			
		}

		nOldTime = m_sPlayData.currPlayTime;
	}

	if (m_bShowLink && m_sPlayData.currPlayTime > g_lDefaultTime)
	{
		DoPause();
		return;
	}

	//在线播放切换分辨率时，时间
	if (m_llResolutionTime && m_bResolution)
	{
		m_CPlayer.Seek(m_llResolutionTime);
		m_bResolution = FALSE;
		return;
	}

	if (m_llResolutionTime&&m_llResolutionTime <= m_sPlayData.currPlayTime)
		m_llResolutionTime = 0;

	if (m_dlgCtrl)
		::PostMessage(m_dlgCtrl->m_hWnd, UM_PLAY_POS, (WPARAM)nPlayPos, (LPARAM)nDuration);

	return;
}

void CPlayerDlg::OnPlayEnd()
{
	SysAlive::Exit();
	DoStop(TRUE);
	OnBtnNext();
	return;
}

void CPlayerDlg::OnFileInvalid()
{
	//播放失败
	m_sPlayData.sRD.ifSuccess = "2";
	m_sPlayData.sRD.ifConsume = "2";
	CLeReport::GetInstance()->SendRTD_Play("1", "1", "播放", m_sPlayData.eSrcType, m_sPlayData.sRD);
	if (eSourceType_Local == m_sPlayData.eSrcType)
	{
		string strName =LeTools::ConvertGBKToUtf8((TCHAR*)(m_sPlayData.strName.c_str()));
		CLeDB::GetInstance()->Local()->UpdateLocalFileList(LeTools::ws2s(m_sPlayData.sMd5).c_str(), 0, 0, 1);

		if (m_dlgList->m_hWnd)
			::PostMessage(m_dlgList->m_hWnd, UM_REDRAW_LISTWND, GetPlayStatus(), 0);
	}
	else
	{
		// 其他视频，暂时不处理
	}
	
	DoStop();

	if (eSourceType_Local == m_sPlayData.eSrcType)
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_PLAY_FILEFAILED);
	else
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_PLAY_NETFAILED);

	return;
}

void CPlayerDlg::SeekPlay(int nPos)
{
	if (eLocalFile_swf == GetLocalFileType())
	{
		BOOL bPlay = m_pFlash->IsPlaying();
		m_pFlash->GotoFrame(nPos);
		if (bPlay) 
			m_pFlash->Play();
	}
	else
		m_CPlayer.Seek(nPos);
    SSliderBar* pSlider = FindChildByName2<SSliderBar>(L"slider_player");
    if (pSlider) 
	{
        pSlider->SetValue(nPos);
    }
}

void CPlayerDlg::SetVolume(int nVolume)
{
    //设置播放声音
	m_CPlayer.SetVolume(nVolume);
    //控件音量显示
    int nStatus = 0;
    static int nLastVolume = -1;
    m_nVolume = nVolume;

    SWindow* pWnd = FindChildByName(L"btn_volume");

    //如果已是静音，修改声音需要修改音量按钮状态
    if (m_bMute)
    {
        m_bMute = !m_bMute;
		m_CPlayer.SetMute(m_bMute);
        if (pWnd) {
            pWnd->SetAttribute(L"skin", L"ctrl_volume1");
            pWnd->SetAttribute(L"focusSkin", L"ctrl_volume2");
        } 
        nStatus = 2;
    }

    //音量变为0，修改音量按钮为静音状态
    if (nVolume == 0 && nLastVolume != 0 && pWnd) {
        pWnd->SetAttribute(L"skin", L"ctrl_jy1");
        pWnd->SetAttribute(L"focusSkin", L"ctrl_jy2");
        nStatus = 1;
    }
    if (nVolume != 0 && nLastVolume == 0 && pWnd) {
        pWnd->SetAttribute(L"skin", L"ctrl_volume1");
        pWnd->SetAttribute(L"focusSkin", L"ctrl_volume2");
        nStatus = 2;
    }
    nLastVolume = nVolume;

    if (m_dlgCtrl)
		m_dlgCtrl->ShowVolume(m_bMute, m_nVolume);
	SSliderBar* pSlider = FindChildByName2<SSliderBar>(L"slider_volume");
	if (pSlider)
		pSlider->SetValue(m_nVolume);
}

void CPlayerDlg::SetMute(BOOL bMute)
{
    m_bMute = bMute;
    if (m_bMute)
    {
		m_CPlayer.SetMute(m_bMute);
        SWindow* pWnd = FindChildByName(L"btn_volume");
        pWnd->SetAttribute(L"skin", L"ctrl_jy1");
        pWnd->SetAttribute(L"focusSkin", L"ctrl_jy2");
    }
}

void CPlayerDlg::EnableBtn(BOOL bLast, BOOL bNext)
{
    bLast = bNext = TRUE;
    SWindow* pWnd = FindChildByName(L"btn_last");
    if (pWnd) 
		pWnd->EnableWindow(bLast);
    pWnd = FindChildByName(L"btn_next");
    if (pWnd) 
		pWnd->EnableWindow(bNext);
    SWindow::Invalidate();
}

void CPlayerDlg::OnBtnStop()
{
    DoStop();
}

void CPlayerDlg::OnBtnPlay()
{
	if (ePS_Stoped == GetPlayStatus())
	{
		if (m_sPlayData.strSource.length() > 0)
		{
			sPlayData* pData = new sPlayData;
			pData->Copy(m_sPlayData);
			::PostMessage(CMainDlg::g_hMainWnd, UM_PLAY, (WPARAM)pData, 0);
		}
	}
	else
	{
		DoPause();
	}	
}

void CPlayerDlg::OnBtnSet()
{
	CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_CLICK_CTRL_SET);
	if (false == CSetDlg::m_bShow)
	{
		CSetDlg dlgSet;
		dlgSet.SetParent(m_hWnd);
		dlgSet.DoModel(FALSE);
	}    
}

void CPlayerDlg::OnBtnOpen()
{
	CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_CLICK_CTRL_OPENFILE);
	OnOpenfile(0);
}

void CPlayerDlg::OnFullScreen()
{
	CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_CLICK_FULL_KONGZHILAN);
    DoFullScreen();
}

void CPlayerDlg::OnMiniMode()
{
	CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_CLICK_MINI_CTRLBAR);
	if (GetPlayStatus() == ePS_Init || (GetPlayStatus() == ePS_Stoped && GetPlayerShowMode() ==ePlayerWndMode_Nomal))
		return;

	if (ePlayerWndMode_Mini == m_eShowMode)
		SetPlayerShowMode(ePlayerWndMode_Nomal);
	else if (ePlayerWndMode_Nomal == m_eShowMode)
	{		
		if (ePS_Stoped != GetPlayStatus())
		{			
			SetPlayerShowMode(ePlayerWndMode_Mini);			
		}
		else
		{
			m_eShowMode = ePlayerWndMode_Init;
		}
	}
		
	return;
}

ePlayerWndMode CPlayerDlg::GetPlayerShowMode()
{
	return m_eShowMode;
}

void CPlayerDlg::SetPlayerShowMode(ePlayerWndMode eMode)
{
	if (m_eShowMode == eMode)
		return;	

	m_sMiniRect.bSetModeOver = false;

	if (m_dlgCtrl)
		m_dlgCtrl->SetPlayerShowMode(eMode);
	
	//改变全屏按钮图标
	//SWindow* pWnd_full = FindChildByName(L" btn_fullscreen");
	SWindow* pWnd_NomalCtrl = FindChildByName(L"wnd_ctrl");
	SWindow * pCaption = FindChildByName(L"playCaption");
	if (ePlayerWndMode_Nomal == eMode)
	{
		::PostMessage(CMainDlg::g_hMainWnd, UM_SHOWMAIN, 0, 0);
		if (pCaption)
			pCaption->SetAttribute(L"pos", L"0,0,0,0");
		FLOG(L"CPlayerDlg::SetPlayerShowMode ePlayerWndMode_Nomal");
		if (m_bMiniShow)
		{
			SendMessage(WM_SYSCOMMAND, SC_RESTORE);
			m_bMiniShow = FALSE;
		}
		
		DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
		dwStyle |= WS_THICKFRAME;
		dwStyle &= ~WS_CAPTION;
		dwStyle |= WS_CHILDWINDOW;
		::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
		SetParent(m_hWnd, CMainDlg::g_hMainWnd);
		// 因为涉及到播放列表/详情页的窗口，所以要发给主窗口重置窗口
		::PostMessage(CMainDlg::g_hMainWnd, UM_RESETWND, 0, 0);
		SetMargin(CRect(0, 0, 0, 0));
		HideAloneCtrlBar();

		//正常播放的控制栏
		if (pWnd_NomalCtrl)
			pWnd_NomalCtrl->SetAttribute(L"show", L"1");
		// 设置全屏按钮
	}
	else if (ePlayerWndMode_Mini == eMode)
	{
		::PostMessage(CMainDlg::g_hMainWnd, UM_SHOWMAIN, 0, 1);
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_CLICK_MINI);
		FLOG(L"CPlayerDlg::SetPlayerShowMode ePlayerWndMode_Mini");
		//小窗口播放
		if (pCaption)
			pCaption->SetAttribute(L"pos", L"0,0,-0,-0");
		SetParent(m_hWnd, NULL);

		if (!m_sMiniRect.bInited)
		{// 如果迷你窗口的尺寸未初始化，则给初始化尺寸
			RECT rcWnd_Main;
			::GetWindowRect(CMainDlg::g_hMainWnd, &rcWnd_Main);
			m_sMiniRect.cRect_Mini.left = rcWnd_Main.right - WIDTH_PLAYER_MINI - 50;
			m_sMiniRect.cRect_Mini.top = rcWnd_Main.bottom - HEIGHT_PLAYER_MINI - 50;
			m_sMiniRect.cRect_Mini.right = rcWnd_Main.right - 50;
			m_sMiniRect.cRect_Mini.bottom = rcWnd_Main.bottom - 50;
			m_sMiniRect.bInited = true;
		}

		SetWindowPos(HWND_TOPMOST, m_sMiniRect.cRect_Mini.left, m_sMiniRect.cRect_Mini.top, m_sMiniRect.cRect_Mini.Width(), m_sMiniRect.cRect_Mini.Height(), SWP_SHOWWINDOW);
		
		DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
		dwStyle |= WS_CAPTION;
		dwStyle &= ~WS_CHILDWINDOW;
		//dwStyle &= ~WS_MAXIMIZEBOX;
		//dwStyle &= ~WS_THICKFRAME;
		::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);//设置成新的样式	
		//::PostMessage(CMainDlg::g_hMainWnd, UM_RESETWND, eTab_HomePage, 0);
		ShowRightTig(FALSE);		
		SetMargin(CRect(5, 5, 5, 5));
		if (pWnd_NomalCtrl)
			pWnd_NomalCtrl->SetAttribute(L"show", L"0");		
		CLeReport::GetInstance()->SendRTD_Play("30", "1", "model", m_sPlayData.eSrcType, m_sPlayData.sRD);
	}
	else if (ePlayerWndMode_Full == eMode)
	{
		if (pWnd_NomalCtrl)
			pWnd_NomalCtrl->SetAttribute(L"show", L"0");
		if (pCaption)
			pCaption->SetAttribute(L"pos", L"0,0,0,0");
		FLOG(L"CPlayerDlg::SetPlayerShowMode ePlayerWndMode_Full");
		if (m_bMiniShow)
		{
			SendMessage(WM_SYSCOMMAND, SC_RESTORE);
			m_bMiniShow = FALSE;
		}
		GetWindowPlacement(CMainDlg::g_hMainWnd, &m_OldWndPlacement);
		SetParent(m_hWnd, NULL);
		//计算窗口全屏位置
		RECT rcFull;
		rcFull.left = 0;
		rcFull.right = ::GetSystemMetrics(SM_CXSCREEN);
		rcFull.top = 0;
		rcFull.bottom = ::GetSystemMetrics(SM_CYSCREEN);

	
		//设置窗口状态位置结构
		WINDOWPLACEMENT wndpl;
		wndpl.length = sizeof(WINDOWPLACEMENT);
		wndpl.flags = 0;
		wndpl.showCmd = SW_SHOWNA| SW_SHOWNORMAL;
		wndpl.rcNormalPosition = rcFull;
				
		//设置窗口位置			
		SetMargin(CRect(1, 0, 0, 0));		
		SetWindowPos(HWND_TOPMOST, 0,0, rcFull.right, rcFull.bottom, SWP_SHOWWINDOW);		
		CLeReport::GetInstance()->SendRTD_Play("30", "1", "全屏", m_sPlayData.eSrcType, m_sPlayData.sRD);
	}
	
	// 调整播放器的窗口大小
	m_CPlayer.ReWndSize(0, 0, 0, eMode == ePlayerWndMode_Nomal ? HEIGHT_CTRBAR : 0);

	SWindow::Invalidate();
	PostMessage(UM_RECHECK_TOP);

	// 状态最后改变，改变过程中引发的错乱另行处理
	m_eShowMode_Old = m_eShowMode;
	m_eShowMode = eMode;
	m_sMiniRect.bSetModeOver = true;

	
	return;
}

void CPlayerDlg::ShowAsNoTop()
{
	char chTmp[MAX_PATH] = { 0 };
	string sValue = "0";
	char* pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("topmost");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}
	    
    int nValue = atoi(sValue.c_str());
    if (nValue != 0) 
	{
        if (m_eShowMode != ePlayerWndMode_Mini)
            ::SetWindowPos(CMainDlg::g_hMainWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE);
        else
            SetWindowPos(HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE);
    }
}

// void CPlayerDlg::ShowAudio(BOOL bShow)
// {
//     SWindow* pWnd = FindChildByName(L"audio");
//     WCHAR wBuff[64] = { 0 };
//     _itow(bShow, wBuff, 10);
//     if (pWnd) pWnd->SetAttribute(L"show", wBuff);
// }

void CPlayerDlg::PlayUrl(string sUrl, string strHead, int Resolu)
{    
    UpdateResolu(Resolu);
    m_bSetChannel = TRUE;//是否初始化声道

    RePlayerShow(1);

    m_sPlayData.lTotalTime = 0;
	m_CPlayer.OpenAndPlay(sUrl.c_str());
    ShowFlyHead(TRUE);

	if (m_bPlayingTopflag)
		::SetWindowPos(CMainDlg::g_hMainWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    //ShowAudio(0);
    if (m_dlgCtrl)
		m_dlgCtrl->SetHead(LeTools::s2ws(strHead).c_str());
    //PostMessage(UM_ENABLE_STOP, 0, 1);
    m_llResolutionTime = 0;
	ShowFlyHead(TRUE);
}

unsigned int WINAPI _StartFunTVThread(void *param)
{
	sFunTVThreadStruct * pData = (sFunTVThreadStruct *)param;
	if (NULL == pData)
		return 0;
	if (NULL == pData->pPlayer)
		return 0;
	
	pData->pPlayer->GetFunTVPlayAdress((char*)(pData->strUrl.c_str()), pData->hWnd);
	delete pData;

	return 0;
}

void CPlayerDlg::Play(sPlayData* pData)
{
	if (NULL == pData)
		return;

	// 播放时才创建播放器线程
	if (!m_bPlayerCreated)
		CreatePlayer();

	SetShowTime(0, 0);
	SetSliderPos(TRUE);
	SetPlayStatus(ePS_Opening);

	// 保存播放元素
	m_sPlayData.Copy(*pData);
	
	// 设置标题
	if (m_dlgCtrl)
		m_dlgCtrl->SetHead(pData->strName.c_str());

	MD5 md(LeTools::ws2s(m_sPlayData.strSource));
	m_sMd5 = md.toString();
	
	// 本地和在线，播放
	if (pData->eSrcType == eSourceType_Local)
	{
		// 如非停止，先停止
		if (GetPlayStatus() != ePS_Stoped)
		{
			if (m_pFlash && eLocalFile_swf == GetLocalFileType())
			{
				m_pFlash->Stop();
			}
			else
			{
				m_CPlayer.Stop();
			}
		}

		SetLocalFileType(eLocalFile_Default);		
		LPWSTR lpExtenson = PathFindExtension(pData->strName.c_str());
		if (wcsicmp(lpExtenson, L".swf") == 0)
			SetLocalFileType(eLocalFile_swf);

		m_bSetChannel = TRUE;		
		RePlayerShow(1);
		if (eLocalFile_swf == GetLocalFileType())
		{
			try
			{
				if (m_pFlash)
					m_pFlash->Play(m_sPlayData.strSource.c_str());
				SetTimer(ET_FLASH_PLAY, 100);
				SetSliderMax();
			}
			catch (...)
			{
				SetLocalFileType(eLocalFile_Default);
			}
		}
		else
		{
			wstring wfile = m_sPlayData.strSource;
			string filepath =LeTools::ConvertGBKToUtf8((TCHAR*)(m_sPlayData.strSource.c_str()));
			//m_CPlayer.OpenAndPlay(filepath);
			m_CPlayer.OpenAndPlay((char*)(filepath.c_str()));
			ShowFlyHead(TRUE);
		}

		if (m_bPlayingTopflag)
		{
			::SetWindowPos(CMainDlg::g_hMainWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		}
		
		//判断是否是MPC格式
		g_bMpc = FALSE;
		if (wcsicmp(lpExtenson, L".mpc") == 0)g_bMpc = TRUE;

		if (m_dlgCtrl)
			m_dlgCtrl->SetHead(pData->strName.c_str());
		
		//if (eSignalType_CmdLine == m_sPlayData.eSignal || eSignalType_DrogFile == m_sPlayData.eSignal)
		//{
			// 添加同文件夹下的其他文件
			//AddOtherFile(m_sPlayData.strSource);
		//}		
	}
}

void CPlayerDlg::ReResolution(string sUrl)
{
    if (sUrl.empty() || sUrl.size() == 0)
		return;
	m_llResolutionTime = m_sPlayData.currPlayTime;
	m_CPlayer.OpenAndPlay(sUrl.c_str());
    m_bResolution = TRUE;
}

void CPlayerDlg::RePlayerShow(BOOL b)
{
    WCHAR wBuff[64] = { 0 };
    int iShow = !b;
    _itow_s(iShow, wBuff,10);
    SWindow* pWnd = FindChildByName(L"player");
    if (pWnd)
		pWnd->SetAttribute(L"show", wBuff);

    if (eLocalFile_swf == GetLocalFileType())
    {
        pWnd = FindChildByName(L"flash_player");
        if (pWnd)
			pWnd->SetAttribute(L"show", L"1");
    }
    else
    {
        pWnd = FindChildByName(L"flash_player");
        if (pWnd)
			pWnd->SetAttribute(L"show", L"0");
    }
	
//     if (!b)
// 	{
//         ShowAudio(FALSE);
//     }
  
    SWindow::Invalidate();
}

int CPlayerDlg::IsExistFormat(SStringT strPath)
{
    LPWSTR lpExtenson = PathFindExtension(strPath);
    lpExtenson = _wcslwr(lpExtenson);
    if (_wcsicmp(lpExtenson, L".torrent") == 0)
    {
        return 4;
    }
    for each (auto var in szView)
    {
        if (var.compare(lpExtenson) == 0)
            return 1;
    }
    for each (auto var in szAudio)
    {
        if (var.compare(lpExtenson) == 0)
            return 2;
    }
    for each (auto var in szSubFormat)
    {
        if (var.compare(lpExtenson) == 0)
            return 3;
    }
    return 0;
}

void CPlayerDlg::DownloadAndPlay(list<string> lstFile, BOOL bPlay)
{
    if (lstFile.size() == 0) return;
    list<string>::iterator iter = lstFile.begin();
    BOOL bFirst = TRUE;
    for (; iter != lstFile.end(); iter++)
    {
        SStringT strPath =LeTools::s2ws(*iter).c_str();
        int nRet = IsExistFormat(strPath);
        if (0 == nRet|| 4== nRet||3== nRet)
            continue;
    }
    ShowWindow(SW_SHOW);
}

void CPlayerDlg::SetShowTime(int nPos, int nDuration)
{
	int nP = 0, nD = 0;
	if (nDuration < 0)
		nD = 0;
	else if (nDuration > 99 * 3600)
		nD = 99 * 3600;
	else
		nD = nDuration;

	if (nPos < 0)
		nP = 0;
	else if (nPos > nD)
		nP = nD;
	else
		nP = nPos;

    WCHAR wBuff[256] = { 0 };
	if (nD == 0)
	{
		wsprintf(wBuff, L"00:00:00/00:00:00");
	}
	else
	{
		if (eLocalFile_swf == GetLocalFileType())
		{
			wsprintf(wBuff, L"%ld/%d", (long)nP, (long)nD);
		}
		else
		{
			wsprintf(wBuff, L"%02d:%02d:%02d/%02d:%02d:%02d", nP / 3600, (nP % 3600) / 60, nP % 60, nD / 3600, (nD % 3600) / 60, nD % 60);
		}
	}
    
    SStatic* pText = FindChildByName2<SStatic>(L"showtime");
    if (pText) {
        pText->SetWindowTextW(wBuff);
    }
}

void CPlayerDlg::SetSliderPos(BOOL bReset/* = FALSE*/)
{
	if (NULL == m_dlgCtrl)
		return;

	if (bReset)
	{
		::PostMessage(m_dlgCtrl->m_hWnd, UM_PLAY_POS, 0, 0);
		SSliderBar* pSlider = FindChildByName2<SSliderBar>(L"slider_player");
		if (pSlider && !pSlider->isDrag())
		{
			pSlider->SetValue(0);
		}
		return;
	}

    __int64 nTime = 0;
	if (eLocalFile_swf == GetLocalFileType())
		nTime = m_pFlash->GetFrameNum();
	else
		nTime = m_sPlayData.currPlayTime;

    if (m_bShowLink&&nTime>g_lDefaultTime){
        nTime = g_lDefaultTime;
    }

    if (g_bMpc&&g_llSetTime == nTime) 
		return;

    SSliderBar* pSlider = FindChildByName2<SSliderBar>(L"slider_player");
    if (pSlider&&!pSlider->isDrag())
    {
        if (eLocalFile_swf == GetLocalFileType())
            pSlider->SetValue(nTime);
        else
            pSlider->SetValue(nTime);
        

        //判读AB点重复
        if (pSlider->IsHaveAB() && pSlider->IsBpos())
        {
            __int64 nNowTime = pSlider->GetApos();
			if (eLocalFile_swf == GetLocalFileType()) {
				m_pFlash->GotoFrame(nNowTime);
				m_pFlash->Play();
			}
			else
				m_CPlayer.Seek(nNowTime);

        }
        if (pSlider->IsHaveAB() && pSlider->IsOutAB()){
            pSlider->RemoveAB();
        }
    }

	::PostMessage(m_dlgCtrl->m_hWnd, UM_PLAY_POS, nTime, m_sPlayData.lTotalTime);
	return;
}

void CPlayerDlg::ShowVolume(BOOL bShow)
{
    if (bShow)
		ShowAsNoTop();
    else 
		PostMessage(UM_RECHECK_TOP);
    CRect rcWnd = GetWindowRect();
    m_dlgCtrl->SetWindowPos(HWND_TOP, rcWnd.left, rcWnd.top + 1, rcWnd.Width(), rcWnd.Height() - 2, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    if (m_dlgCtrl)
		m_dlgCtrl->ShowVolume(bShow,m_nVolume);
}

void CPlayerDlg::DoStop(BOOL bPlayEnd)
{
	SysAlive::Exit();
	ShowCtrlWnd(FALSE);
	ShowLoading(FALSE);

	if (ePS_Stoped == GetPlayStatus())
	{
		SetLocalFileType(eLocalFile_Default);
		SetShowTime(0, 0);
		m_dlgList->StopPlay(bPlayEnd);
		return;
	}

	SetPlayStatus(ePS_Stoped);	
    g_bMpc = FALSE;

    //播放停止
	if (pFunTV_Stop)
	{
		pFunTV_Stop();
	}
	m_CPlayer.Stop();
    //flash控件停止播放
    if (eLocalFile_swf == GetLocalFileType())
	{
        m_pFlash->Stop();
        KillTimer(ET_FLASH_PLAY);
    }
    //隐藏进度条以及播放时间归零
    SSliderBar* pSliderBar = FindChildByName2<SSliderBar>(L"slider_player");
    if (pSliderBar)
    {
        pSliderBar->RemoveAB();
        pSliderBar->SetAttribute(L"show", L"0");
        pSliderBar->SetValue(0);
    }
    
	if (m_bPlayingTopflag)
		::SetWindowPos(CMainDlg::g_hMainWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	ShowFlyHead(FALSE);
    RePlayerShow(0);
	SetLocalFileType(eLocalFile_Default);
    SetShowTime(0, 0);    
    m_dlgList->StopPlay(bPlayEnd);
    ShowVipTip(FALSE);
    if(m_dlgCtrl)
        ::PostMessage(m_dlgCtrl->m_hWnd, UM_PLAY_POS, 0, 0);
	//::PostMessage(CMainDlg::g_hMainWnd, UM_SHOWMAIN,0,0);
	::PostMessage(CMainDlg::g_hMainWnd, UM_STOP, 0, 0);
	
	// 上报播放时长
	m_sPlayData.sRD.playTime = LeTools::Num2Str(GetTickCount() - m_sPlayData.WatchDuration);
	CLeReport::GetInstance()->SendRTD_Play("61", "1", "停止", m_sPlayData.eSrcType, m_sPlayData.sRD);

	return;
}

void CPlayerDlg::Min2Pause()
{
	if (ePS_Playing == GetPlayStatus())
	{
		SetPlayStatus(ePS_Pause);
		ReSetBtnStatus();
		m_CPlayer.Pause();
	}	
}

/*静音切换*/
void CPlayerDlg::DoMute()
{
    int nStatus = 0;
	m_bMute = m_CPlayer.GetMute();
    SWindow* pWnd = FindChildByName(L"btn_volume");

    m_bMute = !m_bMute;
	m_CPlayer.SetMute(m_bMute);
    if (pWnd)
    {
        if (m_bMute)
        {
            pWnd->SetAttribute(L"skin", L"ctrl_jy1");
            pWnd->SetAttribute(L"focusSkin", L"ctrl_jy2");
            nStatus = 1;
        }
        else
        {
            pWnd->SetAttribute(L"skin", L"ctrl_volume1");
            pWnd->SetAttribute(L"focusSkin", L"ctrl_volume2");
            nStatus = 2;
        }
    }    
}

void CPlayerDlg::DoPause()
{
	if (ePS_Playing == GetPlayStatus())
	{
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_PLAY_PAUSE);
		SetPlayStatus(ePS_Pause);
	}
	else if (ePS_Pause == GetPlayStatus())
	{
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_PLAY_PLAY);
		SetPlayStatus(ePS_Playing);		
		m_sPlayData.sRD.ifSuccess = "1";
		m_sPlayData.sRD.ifConsume = "2";
		CLeReport::GetInstance()->SendRTD_Play("1", "1", "播放", m_sPlayData.eSrcType, m_sPlayData.sRD);
		m_sPlayData.sRD.ifSuccess = "";
	}
	
	if (m_bPlayingTopflag && GetPlayStatus() == ePS_Pause)
	{
		::SetWindowPos(CMainDlg::g_hMainWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	}
	BOOL bIsPlay = FALSE;
    if (eLocalFile_swf == m_eFileType)
    {		
        bIsPlay = m_pFlash->IsPlaying();
        if (bIsPlay)
            m_pFlash->Stop();
        else
            m_pFlash->Play();
        bIsPlay = !bIsPlay;
    }
    else
    {
		if (ePS_Playing == GetPlayStatus())
			m_CPlayer.Play();
		else if (ePS_Pause == GetPlayStatus())
			m_CPlayer.Pause();
    } 
	if (m_dlgList->m_hWnd)
		::PostMessage(m_dlgList->m_hWnd, UM_REDRAW_LISTWND, GetPlayStatus(), 0);

	return;
}

void CPlayerDlg::DoFullScreen()
{
	if (ePlayerWndMode_Full == m_eShowMode)
	{		
		// 还原原有模式
		SetPlayerShowMode(m_eShowMode_Old);
		CLeReport::GetInstance()->SendRTD_Eevent(RTD_CANCELFULL, "1", "全屏取消");
	}
	else
	{
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_CLICK_FULL);
		SetPlayerShowMode(ePlayerWndMode_Full);
		//CLeReport::GetInstance()->SendRTD_Eevent("23", "1", "全屏");
	}

	return;
}

/*快进*/
void CPlayerDlg::Forward(int nSpeed)
{
	if (GetPlayStatus() == ePS_Playing || GetPlayStatus() == ePS_Pause)
	{
		if (eLocalFile_swf == m_eFileType)
		{
			BOOL bPlay = m_pFlash->IsPlaying();
			long lFrame = m_pFlash->GetFrameNum();
			lFrame += nSpeed;
			if (lFrame > m_sPlayData.lTotalTime)
				lFrame = m_sPlayData.lTotalTime;
			m_pFlash->GotoFrame(lFrame);
			if (bPlay)
				m_pFlash->Play();
		}
		else
		{
			m_nIncrement += nSpeed;
			int nPos = min(m_sPlayData.currPlayTime + m_nIncrement, m_sPlayData.lTotalTime);
			nPos = max(m_sPlayData.currPlayTime + m_nIncrement, 0);
			SeekPlay(nPos);
		}
		SetSliderPos();
	}
	CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_PLAY_FORWORD);
}

/*快退*/
void CPlayerDlg::Rewind(int nSpeed)
{
	if (GetPlayStatus() == ePS_Playing || GetPlayStatus() == ePS_Pause)
    {
        if (eLocalFile_swf == m_eFileType)
        {
            BOOL bPlay = m_pFlash->IsPlaying();
            long lFrame = m_pFlash->GetFrameNum();
            lFrame -= nSpeed;
            if (lFrame < 0) lFrame = 0;
            m_pFlash->GotoFrame(lFrame);
            if (bPlay)
				m_pFlash->Play();
        }
		else
		{
			m_nIncrement -= nSpeed;
			int nPos = min(m_sPlayData.currPlayTime + m_nIncrement, m_sPlayData.lTotalTime);
			nPos = max(m_sPlayData.currPlayTime + m_nIncrement, 0);
			SeekPlay(nPos);
		}			

        SetSliderPos();
    }
	CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_PLAY_REWIND);
}

void CPlayerDlg::ReSetBtnStatus()
{
	// 播放暂停按钮
	SWindow* pWnd_Play = FindChildByName(L"btn_play");
	if (!pWnd_Play)
		return;

	if (ePS_Playing == GetPlayStatus() || ePS_Opening == GetPlayStatus())
	{
		pWnd_Play->SetAttribute(L"skin", L"ctrl_pause1");
		pWnd_Play->SetAttribute(L"focusSkin", L"ctrl_pause2");
		pWnd_Play->SetAttribute(L"tip", L"暂停");
	}
	else
	{
		pWnd_Play->SetAttribute(L"skin", L"ctrl_play1");
		pWnd_Play->SetAttribute(L"focusSkin", L"ctrl_play2");
		pWnd_Play->SetAttribute(L"tip", L"播放");
	}

	// 上一个下一个按钮
	SWindow* pWnd_last = FindChildByName(L"btn_last");
	SWindow* pWnd_next = FindChildByName(L"btn_next");
	if (!pWnd_last || !pWnd_next)
		return;
	int nNum = 0, nTotal = 0;
	bool bAutoNext = false;
	if (m_sPlayData.eSrcType == eSourceType_Local)
	{// 如果是本地文件，看播放列表里的个数		
		m_dlgList->GetNumAndTotal(nNum, nTotal);		
	}
	else
	{// 其他情况，一律禁用
		nNum = nTotal = 0;
	}

	if (bAutoNext)
	{
		pWnd_last->SetAttribute(L"enable", L"1");
		pWnd_next->SetAttribute(L"enable", L"1");
	}
	else
	{
		if (nTotal > 1)
		{
			pWnd_last->SetAttribute(L"enable", nNum > 1 ? L"1" : L"0");
			pWnd_next->SetAttribute(L"enable", nNum <= nTotal ? L"1" : L"0");
		}
		else
		{
			pWnd_last->SetAttribute(L"enable", L"0");
			pWnd_next->SetAttribute(L"enable", L"0");
		}
	}	

	// 停止按钮,暂时没有停止按钮
	m_CPlayer.ReWndSize(0, 0, 0, m_eShowMode == ePlayerWndMode_Nomal ? HEIGHT_CTRBAR : 0);

	// 同步独立控制栏
	if (m_dlgCtrl)
		m_dlgCtrl->ReSetBtnStatus(GetPlayStatus(), m_sPlayData, nNum, nTotal);
}

void CPlayerDlg::SetSliderMax()
{
    //获取总播放时长
	if (eLocalFile_swf == m_eFileType)
		m_sPlayData.lTotalTime = m_pFlash->GetTotalFrames();
 	    
    //设置进度条最大值
    SSliderBar* pSliderBar = FindChildByName2<SSliderBar>(L"slider_player");
    if (pSliderBar)
    {
        pSliderBar->RemoveAB();
        pSliderBar->SetAttribute(L"show", L"1");

        wchar_t wTotalTime[64] = { 0 };
        if (eLocalFile_swf == m_eFileType)
            _ltow(m_sPlayData.lTotalTime, wTotalTime, 10);
        else
            _ltow(m_sPlayData.lTotalTime, wTotalTime, 10);
        pSliderBar->SetAttribute(L"max", wTotalTime);
    }

    if(m_dlgCtrl)
        ::PostMessage(m_dlgCtrl->m_hWnd, UM_PLAY_POS, 0, m_sPlayData.lTotalTime);
}

/*打开文件*/
void CPlayerDlg::OnOpenfile(int nWnd)
{
    SetTopMost(0);
    TCHAR szBuffer[MAX_PATH] = { 0 };
    OPENFILENAME ofn = { 0 };
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = m_hWnd;
    ofn.lpstrFilter = szFilters;
    ofn.lpstrInitialDir = _T("C:\\");//默认的文件路径 
    ofn.lpstrFile = szBuffer;//存放文件的缓冲区 
    ofn.nMaxFile = sizeof(szBuffer) / sizeof(*szBuffer);
    ofn.nFilterIndex = 0;
    ofn.Flags = OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_EXPLORER|
        OFN_ENABLEHOOK|OFN_ENABLESIZING|OFN_NOCHANGEDIR;//标志如果是多选要加上OFN_ALLOWMULTISELECT
    if (GetOpenFileName(&ofn))
    {
        SStringT strPath = ofn.lpstrFile;
		wstring wstrName = strPath;
		wstrName = wstrName.substr(wstrName.rfind(L"\\") + 1);

        string sName =LeTools::ws2s(strPath.GetBuffer(0));
        sName = sName.substr(sName.rfind("\\")+1);

        int nRet = IsExistFormat(strPath);
        if (0 == nRet)
        {
            goto END;
        }
        if (4 == nRet) //种子
        {
            goto END;
        }
        if (3 == nRet) //字幕
        {
			m_CPlayer.SetSubtitlefile(LeTools::ConvertGBKToUtf8(strPath.GetBuffer(strPath.GetLength())).c_str());
            goto END;
        }

		// 播放消息统一发给maindlg处理
		sPlayData* pData = new sPlayData;
		pData->eSignal = eSignalType_OpenFile;
		pData->eSrcType = eSourceType_Local;
		pData->bNoHistory = false;
		pData->strSource = strPath;
		pData->strName = wstrName.c_str();
		pData->nNum = 0;
		pData->nTotal = 0;
		pData->sRD.videoId =LeTools::ws2s(wstrName).c_str();
		pData->ifConsume = _T("2");
		if (nWnd == 3 && ePS_Stoped != m_ePlayerStatus)
		{
			pData->bOnlyAddList = true;
		}
		else
		{
			pData->bOnlyAddList = false;
		}		

		::PostMessage(CMainDlg::g_hMainWnd, UM_PLAY, (WPARAM)pData, 0);
    }
END:
    PostMessage(UM_RECHECK_TOP);
    SetForegroundWindow(m_hWnd);
}

void CPlayerDlg::FunTv_PreGet(sPlayData* pData)
{
	FLOG(L"CPlayerDlg::FunTv_PreGet");
	if (NULL == pData)
		return;
	if (NULL == m_pFirstData)// 如果是第一个数据，保存下来
	{		
		m_pFirstData = pData;
		m_bFunTvHaveGet = false;
	}
	else if (m_bFunTvHaveGet)// 如果已经获取了风行的地址，则先停止
	{
		m_pFirstData = pData;
		m_bFunTvHaveGet = false;

		// 如非停止，先停止
		if (GetPlayStatus() != ePS_Stoped)
		{
			if (NULL != pFunTV_Stop)
				pFunTV_Stop();			
		}
	}
	else // 如果当前有数据在获取风行地址，则保存为第二数据，返回
	{
		FLOG(L"替换second return");
		m_pSecondData = pData;
		return;
	}

	// 有了缓存机制后，暂时不销毁正在获取的线程，防止风行模块的崩溃
// 	if (m_hThread_GetFunTVAdress != INVALID_HANDLE_VALUE)
// 	{
// 		TerminateThread(m_hThread_GetFunTVAdress, 0);
// 		CloseHandle(m_hThread_GetFunTVAdress);
// 		m_hThread_GetFunTVAdress = INVALID_HANDLE_VALUE;
// 	}
	
	// 开启线程，获取真正的播放地址
	sFunTVThreadStruct* pFunTVData = new sFunTVThreadStruct;
	pFunTVData->pPlayer = this;
	pFunTVData->strUrl =LeTools::ws2s(pData->strSource);
	pFunTVData->hWnd = m_hWnd;
	FLOG(L"CPlayerDlg::FunTv_PreGet Thread-Begin");
	m_hThread_GetFunTVAdress = (HANDLE)_beginthreadex(NULL, 0, _StartFunTVThread, (void *)pFunTVData, 0, 0);
	return;
}

/*
通过播放数据获取到地址后
判断是否second是否为空
如果未空，则播放
如果不未空，则扔掉first数据和地址
将second的播放数据赋值给first
second赋值为空
*/
void CPlayerDlg::FunTv_AfterGet(char* pFsp, char* pAdress)
{
	FLOG(L"CPlayerDlg::FunTv_PreGet Thread-End");
	m_bFunTvHaveGet = true;
	if (NULL == m_pSecondData)
	{
		// 如果没有排队的，就直接开始播放
		PlayFucTV(pFsp, pAdress);
		return;
	}
	else
	{
		// 如果有排队的，就直接提上来，之前的扔掉
		FunTv_PreGet(m_pSecondData);
		m_pSecondData = NULL;
	}
	return;
}

void CPlayerDlg::CreatePlayer()
{
	// 创建播放器
	CRect rcWnd = GetClientRect();
	rcWnd.bottom -= 66;
	m_CPlayer.CreatePlayer(m_hWnd, &rcWnd);
	m_CPlayer.AttachCallBack(this);
	m_bPlayerCreated = true;
}

void CPlayerDlg::ReleasePlayer()
{
	if (!m_bPlayerCreated)
		return;

	m_CPlayer.Quit();

	m_bPlayerCreated = false;
}

int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg, LPARAM /*lParam*/, LPARAM lpData)
{
    if(uMsg== BFFM_INITIALIZED)
    {
        SendMessage(hwnd, BFFM_SETSELECTION, (WPARAM)TRUE,lpData);
    }
    return 0;
}

/*打开文件夹*/
void CPlayerDlg::OnOpenfloder(int nWnd)
{
    SetTopMost(false);

    wchar_t *szDefaultDir = L"C:\\";
    wchar_t szPath[MAX_PATH] = { 0 };
    BROWSEINFO brInfo;
    ITEMIDLIST *pItem;
    brInfo.hwndOwner = m_hWnd;// 窗口句柄  
    brInfo.pidlRoot = 0;
    brInfo.pszDisplayName = 0;
    brInfo.lpszTitle = L"打开文件夹";
    brInfo.lpfn = BrowseCallbackProc;
    brInfo.lParam = LPARAM(szDefaultDir);
    brInfo.ulFlags = 0;
    brInfo.iImage = 0;
    pItem = SHBrowseForFolder(&brInfo);
    PostMessage(UM_RECHECK_TOP);
    SetForegroundWindow(m_hWnd);

	wstring wstrName;
	SStringT wsPath;
    if (pItem != NULL)
    {
        if (SHGetPathFromIDList(pItem, szPath))
        {
			wsPath = szPath;
            wsPath += _T("\\*.*");
            HANDLE hSearch;
            WIN32_FIND_DATA data;
            hSearch = FindFirstFile(wsPath, &data);
			int i = 0;
			do 
			{
                if (data.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY&&wcsicmp(data.cFileName, L".") && wcsicmp(data.cFileName, L"..")) 
				{
                    wsPath = szPath;
                    wsPath += L"\\";
                    wsPath += data.cFileName;

                    int nRet = IsExistFormat(wsPath);
                    if (0 == nRet || 4 == nRet) 
						continue;
                    if (3 == nRet) //字幕
                    {
                        m_CPlayer.SetSubtitlefile(LeTools::ConvertGBKToUtf8(wsPath.GetBuffer(wsPath.GetLength())).c_str());
                        continue;
                    }

					// 播放消息统一发给maindlg处理
					wstrName = wsPath;
					wstrName = wstrName.substr(wstrName.rfind(L"\\") + 1);
					sPlayData* pData = new sPlayData;
					pData->eSignal = eSignalType_OpenFile;
					pData->eSrcType = eSourceType_Local;
					pData->bNoHistory = false;
					pData->strSource = wsPath;
					pData->strName = wstrName.c_str();
					pData->sRD.videoId = LeTools::ws2s(wstrName);
					pData->nNum = 0;
					pData->nTotal = 0;
					pData->ifConsume = _T("2");
					// 添加文件夹，就只是添加，不允许播放。等添加完毕之后，播放最后一个。
					pData->bOnlyAddList = true;
					::PostMessage(CMainDlg::g_hMainWnd, UM_PLAY, (WPARAM)pData, 0);

					i++;
                }				
            }
			while (FindNextFile(hSearch, &data));
            FindClose(hSearch);

            CLeReport::GetInstance()->SendRTD_Eevent(RTD_ADDFILE, "1", "本地视频添加文件夹");
        }
    }

	// 添加文件夹，就只是添加，不允许播放。等添加完毕之后，播放最后一个。
	sPlayData* pData = new sPlayData;
	pData->eSignal = eSignalType_OpenFile;
	pData->eSrcType = eSourceType_Local;
	pData->bNoHistory = false;
	pData->strSource = wsPath;
	pData->strName = wstrName.c_str();
	pData->sRD.videoId = LeTools::ws2s(wstrName);
	pData->nNum = 0;
	pData->nTotal = 0;
	pData->ifConsume = _T("2");	
	pData->bOnlyAddList = false;
	::PostMessage(CMainDlg::g_hMainWnd, UM_PLAY, (WPARAM)pData, 0);

	return;
}

void CPlayerDlg::AddOtherFile(wstring strSource)
{
	int nL = strSource.length();
	if (nL > MAX_PATH)
		return;

	wchar_t wpath[MAX_PATH] = {0};
	memcpy(wpath, strSource.c_str(), nL*2);
	::PathRemoveFileSpecW(wpath);

	SStringT wsPath = wpath;
	wsPath += _T("\\*.*");
	HANDLE hSearch;
	WIN32_FIND_DATA data;
	hSearch = FindFirstFile(wsPath, &data);
	int i = 0;
	do
	{
		if (data.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY&&wcsicmp(data.cFileName, L".") && wcsicmp(data.cFileName, L".."))
		{
			wsPath = wpath;
			wsPath += L"\\";
			wsPath += data.cFileName;

			int nRet = IsExistFormat(wsPath);
			if (0 == nRet || 4 == nRet)
				continue;
			if (3 == nRet) //字幕
			{
				m_CPlayer.SetSubtitlefile(LeTools::ConvertGBKToUtf8(wsPath.GetBuffer(wsPath.GetLength())).c_str());
				continue;
			}

			// 播放消息统一发给maindlg处理
			wstring wstrName = wsPath;
			wstrName = wstrName.substr(wstrName.rfind(L"\\") + 1);
			sPlayData* pData = new sPlayData;
			pData->eSignal = eSignalType_OpenFile;
			pData->eSrcType = eSourceType_Local;
			pData->bNoHistory = false;
			pData->strSource = wsPath;
			pData->strName = wstrName.c_str();
			pData->nNum = 0;
			pData->nTotal = 0;
			pData->ifConsume = _T("2");
			pData->bOnlyAddList = true;

			::PostMessage(CMainDlg::g_hMainWnd, UM_PLAY, (WPARAM)pData, 0);

			i++;
		}
	} while (FindNextFile(hSearch, &data));
	FindClose(hSearch);

	CLeReport::GetInstance()->SendRTD_Eevent(RTD_ADDFILE, "1", "本地视频添加文件夹");
}

void CPlayerDlg::SetScreenRatio(int nID, BOOL b)
{
    switch (nID)
    {
    case SCREEN_NORMAL:
        m_CPlayer.SetAspectRatio(SCALE_Original);
        break;
    case SCREEN_16_9:
        m_CPlayer.SetAspectRatio(SCALE_16X9);
        break;
    case SCREEN_4_3:
        m_CPlayer.SetAspectRatio(SCALE_4X3);
        break;
    case SCREEN_FULL:
        m_CPlayer.SetAspectRatio(SCALE_fill);
        break;
    default:
        return;
        break;
    }
	int nRatio = nID - SCREEN_NORMAL;
	m_sPlayData.sRD.sScreen =LeTools::Num2Str(nRatio);
	
    if (b) {
        CLeDB::GetInstance()->Set()->UpdateSingleSet("screenRatio",LeTools::Num2Str(nRatio).c_str());
    }

}

void CPlayerDlg::OnAddSubtitle(int nWnd)
{
    SetTopMost(false);
    TCHAR szBuffer[MAX_PATH] = { 0 };
    OPENFILENAME ofn = { 0 };
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = m_hWnd;
    ofn.lpstrFilter = L"字幕文件(*.sub;*.srt;*.ssa;*.ass)\0*.sub;*.srt;*.ssa;*.ass\0所有文件(*.*)\0 * .*\0";
    ofn.lpstrInitialDir = _T("D:\\Program Files");//默认的文件路径 
    ofn.lpstrFile = szBuffer;//存放文件的缓冲区 
    ofn.nMaxFile = sizeof(szBuffer) / sizeof(*szBuffer);
    ofn.nFilterIndex = 0;
    ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER |
        OFN_ENABLEHOOK | OFN_ENABLESIZING | OFN_NOCHANGEDIR;;//标志如果是多选要加上OFN_ALLOWMULTISELECT
    if (GetOpenFileName(&ofn))
    {
        SStringT strPath = ofn.lpstrFile;

        int nRet = IsExistFormat(strPath);
        if (3 == nRet) //字幕
        {
            m_CPlayer.SetSubtitlefile(LeTools::ConvertGBKToUtf8(strPath.GetBuffer(strPath.GetLength())).c_str());
        }
    }
    SetForegroundWindow(m_hWnd);
}

void CPlayerDlg::OnHotkey(int nID)
{
	LONGLONG tmp = 0;
    switch (nID)
    {
    //case HK_OPENFILE:
	//	OnOpenfile(0); 
	//	break;
    //case HK_OPENFLODER:
	//	OnOpenfloder(0); 
	//	break;
    case HK_PAUSE:
		DoPause(); 
		break;
    case HK_STOP:
		DoStop(); 
		break;
    case HK_QUIT: 
		::PostMessage(CMainDlg::g_hMainWnd, UM_CLOSE,0,0); 
		break;
    case HK_FORWARD: 
	{
		char chTmp[MAX_PATH] = { 0 };
		string sValue = "10";
		char* pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("forwardSpeed");
		if (NULL != pchValue)
		{
			sValue = pchValue;
			CLeDB::GetInstance()->ReleaseStr(pchValue);
		}		
        Forward(atoi(sValue.c_str()));		
		CLeReport::GetInstance()->SendRTD_Play("3", "1", "快进", m_sPlayData.eSrcType, m_sPlayData.sRD);
    }
	break;
    case HK_REWIND: 
	{
		char chTmp[MAX_PATH] = { 0 };
		string sValue = "10";
		char* pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("rewindSpeed");
		if (NULL != pchValue)
		{
			sValue = pchValue;
			CLeDB::GetInstance()->ReleaseStr(pchValue);
		}
		
        Rewind(atoi(sValue.c_str()));		
		CLeReport::GetInstance()->SendRTD_Play("4", "1", "快退", m_sPlayData.eSrcType, m_sPlayData.sRD);
    } 
	break;
    case HK_FORWARD30:		
		CLeReport::GetInstance()->SendRTD_Play("3", "1", "快进", m_sPlayData.eSrcType, m_sPlayData.sRD);
		Forward(30); 
		break;
    case HK_REWIND30:		
		CLeReport::GetInstance()->SendRTD_Play("4", "1", "快退", m_sPlayData.eSrcType, m_sPlayData.sRD);
		Rewind(30); 
		break;
    case HK_UPVOICE:
		UpVolume(); 
		break;
    case HK_DOWNVOICE:
		DownVolume(); 
		break;
    case HK_MUTE:
		DoMute(); 
		break;
    case HK_FULLSCREEN:
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_CLICK_FULL_HOTKEY);
		DoFullScreen(); 
		break;
    default:
        break;
    }
}

void CPlayerDlg::UpVolume(int nPos)
{
    int nStatus = 0;
    static int nOldVolume = m_nVolume;
    nOldVolume = m_nVolume;
    SWindow* pWnd = FindChildByName(L"btn_volume");
    if (m_bMute)
    {
        m_bMute = !m_bMute;
        m_CPlayer.SetMute(m_bMute);
        pWnd->SetAttribute(L"skin", L"ctrl_volume1");
        pWnd->SetAttribute(L"focusSkin", L"ctrl_volume2");
        nStatus = 2;
    }
	m_nVolume = m_CPlayer.GetVolume();
	m_nVolume += nPos;
	if (m_nVolume >= 100)
		m_nVolume = 100;
	m_CPlayer.SetVolume(m_nVolume);

    if (nOldVolume==0 && m_nVolume != 0)
    {
        if (pWnd) {
            pWnd->SetAttribute(L"skin", L"ctrl_volume1");
            pWnd->SetAttribute(L"focusSkin", L"ctrl_volume2");
        }
        nStatus = 2;
    }
    if (m_dlgCtrl)
		m_dlgCtrl->ShowVolume((m_nVolume == 0),m_nVolume);
	SSliderBar* pSlider = FindChildByName2<SSliderBar>(L"slider_volume");
	if (pSlider)
		pSlider->SetValue(m_nVolume);
}

void CPlayerDlg::DownVolume(int nPos)
{
    int nStatus = 0;
    SWindow* pWnd = FindChildByName(L"btn_volume");
    static int nOldVolume = m_nVolume;
    nOldVolume = m_nVolume;
    if (m_bMute)
    {
        m_bMute = !m_bMute;
        m_CPlayer.SetMute(m_bMute);
        if (pWnd) {
            pWnd->SetAttribute(L"skin", L"ctrl_volume1");
            pWnd->SetAttribute(L"focusSkin", L"ctrl_volume2");
        }
        nStatus = 2;
    }

	m_nVolume = m_CPlayer.GetVolume();
	m_nVolume -= nPos;
	if (m_nVolume<0)
		m_nVolume = 0;
	m_CPlayer.SetVolume(m_nVolume);

    if (nOldVolume != 0 && m_nVolume == 0)
    {
        pWnd->SetAttribute(L"skin", L"ctrl_jy1");
        pWnd->SetAttribute(L"focusSkin", L"ctrl_jy2");
        nStatus = 1;
    }
	if (m_dlgCtrl)
		m_dlgCtrl->ShowVolume((m_nVolume == 0), m_nVolume);
	SSliderBar* pSlider = FindChildByName2<SSliderBar>(L"slider_volume");
	if (pSlider)
		pSlider->SetValue(m_nVolume);
    
}

void CPlayerDlg::ShowFlyHead(BOOL bShow)
{
	if (NULL == m_dlgCtrl)
		return;

	if (ePlayerWndMode_Mini != m_eShowMode)
	{
		if (ePS_Stoped == GetPlayStatus() || ePS_Init == GetPlayStatus())
			return;
	}	

	if (bShow)
	{
		// 为了在迷你窗口的时候边框可以拖拽拉伸窗口大小，所以留1个像素在上边和下边
		int nTopJust = 0;
		int nBottomJust = 0;
		if (ePlayerWndMode_Mini == m_eShowMode)
		{
			nTopJust = 1;
			nBottomJust = 2;
		}

		CRect rcWnd = GetWindowRect();
		m_dlgCtrl->SetWindowPos(HWND_TOP, rcWnd.left, rcWnd.top + nTopJust, rcWnd.Width(), rcWnd.Height() - nBottomJust, SWP_NOACTIVATE | SWP_SHOWWINDOW);
		m_dlgCtrl->ShowHead(TRUE);
		if (ePlayerWndMode_Mini == m_eShowMode || ePlayerWndMode_Full == m_eShowMode)
		{
			m_dlgCtrl->ShowCtrl(TRUE);
		}
		SetTimer(ET_HIDEHEAD,2000);
	}
	else
	{
		m_dlgCtrl->ShowHead(FALSE);
		m_dlgCtrl->ShowCtrl(FALSE);
		//PostMessage(UM_RECHECK_TOP);
	}
    
	return;
}

void CPlayerDlg::HideAloneCtrlBar()
{
    if (NULL == m_dlgCtrl)
		return;

	m_dlgCtrl->ShowCtrl(FALSE);

	return;
}

void CPlayerDlg::SetPlayStatus(ePlayStatus eStatus)
{
	if (m_ePlayerStatus == eStatus)
		return;
	m_ePlayerStatus = eStatus;

	SWindow* pWnd_mini = FindChildByName(L"btn_mini_Nomal");
	if (pWnd_mini)
		pWnd_mini->SetAttribute(L"enable", ePS_Stoped == eStatus ? L"0" : L"1");
	SWindow* pWnd_full = FindChildByName(L"btn_fullscreen");
	if (pWnd_full)
		pWnd_full->SetAttribute(L"enable", ePS_Stoped == eStatus ? L"0" : L"1");

	// 根据状态设置按钮状态
	ReSetBtnStatus();

	if (eStatus == ePS_Pause)
	{
		m_sPlayData.sRD.ifSuccess = "";
		CLeReport::GetInstance()->SendRTD_Play("2", "1", "暂停", m_sPlayData.eSrcType, m_sPlayData.sRD);
	}		

	return;
}

ePlayStatus CPlayerDlg::GetPlayStatus()
{
	return m_ePlayerStatus;
}

bool CPlayerDlg::IsMiniShow()
{
	return m_bMiniShow;
}

// 0不置顶  1置顶
void CPlayerDlg::SetTopMost(bool bTop)
{
	m_bPlayingTopflag = FALSE;
	
	if (!bTop)
	{
		CLeDB::GetInstance()->Set()->UpdateSingleSet("topmost", "0");
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_CLICK_ZHIDING_CONGBU);
		::SetWindowPos(CMainDlg::g_hMainWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	}		
	else if (bTop)
	{
		CLeDB::GetInstance()->Set()->UpdateSingleSet("topmost", "1");
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_CLICK_ZHIDING_SHIZHONG);
		if (m_eShowMode ==ePlayerWndMode_Nomal)
		{
			::SetParent(m_hWnd,CMainDlg::g_hMainWnd);
			::SetWindowPos(CMainDlg::g_hMainWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE| SWP_SHOWWINDOW);
		}
		else
		{
			::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		}		
		CLeReport::GetInstance()->SendRTD_Play("30", "1", "始终置顶", m_sPlayData.eSrcType, m_sPlayData.sRD);
	}	
	return;
}

bool CPlayerDlg::GetTopMostSet()
{
	char chTmp[MAX_PATH] = { 0 };
	string sValue = "0";
	char* pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("topmost");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}
	if (1 == atoi(sValue.c_str()))
		return true;
	else
		return false;
}

void CPlayerDlg::ShowVipTip(BOOL b)
{
    m_bShowLink = b;
	if (m_bShowLink) 
	{
		//ShowAsNoTop();
	}		
    else
		PostMessage(UM_RECHECK_TOP);
    if (m_dlgCtrl)
		m_dlgCtrl->ShowVip(b);
}

void CPlayerDlg::PlayAndVip(BOOL isPlay, BOOL isVip)
{
    ShowVipTip(!isPlay);
    m_strIsVip = isVip == TRUE ? "1" : "2";
}

BOOL CPlayerDlg::AddMenuItem(pugi::xml_node xmlMenu, int nParentID, SStringT strValue, int nID)
{
    pugi::xml_node xmlItem = xmlMenu.first_child();
    while (xmlItem)
    {
        if (wcscmp(L"item", xmlItem.name()) == 0)
        {
            if (xmlItem.attribute(L"id").as_int(0) == nParentID)
            {
                pugi::xml_node xmlChild =
                    xmlItem.insert_copy_after(xmlItem.last_child(), xmlItem.child(L"item"));
                xmlChild.attribute(L"id").set_value(nID);
                int nRadio = 0;
                if (CPlayerDlg::m_pThis->/*m_avPlayer*/m_CPlayer.GetSubtitleSpu() + SUBTITLE_START == nID)
                    nRadio = 1;
                xmlChild.attribute(L"radio").set_value(nRadio);
                xmlChild.first_child().set_value(strValue);
                return TRUE;
            }

            if (AddMenuItem(xmlItem, nParentID, strValue, nID))
            {
                return TRUE;
            }
        }

        if (nID == xmlItem.attribute(L"id").as_int(0))
        {
            pugi::xml_node xmlChild =
                xmlItem.insert_copy_after(xmlItem.last_child(), xmlItem.child(L"menuItem"));
            xmlChild.attribute(L"id").set_value(nID);
            xmlChild.first_child().set_value(strValue);
            return TRUE;
        }
        xmlItem = xmlItem.next_sibling();
    }
    return FALSE;
}

void CPlayerDlg::ShowRightTig(BOOL bShow)
{
	if (bShow)
	{
		if (!m_bTigShow)
		{
			KillTimer(ET_HIDETIG);
			//ShowAsNoTop();
			CRect rcWnd = GetWindowRect();
			m_bTigShow = TRUE;
			if (m_dlgCtrl)
			{
				m_dlgCtrl->ShowHead(FALSE);
				m_dlgCtrl->ShowCtrl(FALSE);
				// 显示按钮时，整个独立控制栏窗口其实是显示状态，所以默认独立控制栏不显示
				m_dlgCtrl->SetWindowPos(HWND_TOP, rcWnd.left, rcWnd.top + 1, rcWnd.Width(), rcWnd.Height() - 2, SWP_NOACTIVATE | SWP_SHOWWINDOW);
				m_dlgCtrl->ShowTig(m_bTigShow);
			}
			SetTimer(ET_HIDETIG, 3000);
		}
	}
	else
	{
		if (m_dlgCtrl)
			m_dlgCtrl->ShowTig(FALSE);
		KillTimer(ET_HIDETIG);
		m_bTigShow = FALSE;
	}
	return;
}

void CPlayerDlg::WhenStartPlay()
{
    if (ePlayerWndMode_Full == m_eShowMode) 
		return;  //不是开始播放或者窗口已全屏不再处理
    if (IsExistFormat(m_sPlayData.strSource.c_str()) == 2) 
		return;

	char chTmp[MAX_PATH] = { 0 };
	string sValue = "1";
	char* pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("wndStyle");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}
	
    int nValue = atoi(sValue.c_str());
    if (nValue == 2) //自动全屏
    {
        DoFullScreen();
    }

    if (nValue == 1)
    {
        //视频适应窗口不处理
    }

    if (nValue == 0) //窗口适应视频
    {
        
    }
}

void CPlayerDlg::UpdateResolu(int nResolu)
{
    m_nResolu = nResolu;
    if (m_dlgCtrl)m_dlgCtrl->SetResolu(m_nResolu);
}

int CPlayerDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    SetMsgHandled(FALSE);
    return 0;
}

BOOL CPlayerDlg::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
	// 加载风行模块/查找模块内函数地址
// 	wchar_t path[MAX_PATH] = { 0 };
// 	::GetModuleFileNameW(NULL, path, MAX_PATH);
// 	::PathRemoveFileSpecW(path);
// 	wstring wstrPath = path;
// 	wstrPath += L"\\funotc.dll";
// 	m_hMod_FunTV = ::LoadLibraryW(wstrPath.c_str());
// 	if (NULL != m_hMod_FunTV)
// 	{
// 		pFunTVAdress = (FUNC_FunTV)::GetProcAddress(m_hMod_FunTV, "GetPlayUrl");
// 		pFunTV_Stop = (FUNC_FunTV_Stop)::GetProcAddress(m_hMod_FunTV, "StopPlay");
// 		if (pFunTV_Stop)
// 			pFunTV_Stop();
// 	}

	ShowLoading(FALSE);
    m_bLayoutInited = TRUE;
	m_bPlayingTopflag = GetTopMostSet();

	//允许拖拽文件到窗口
    DragAcceptFiles(m_hWnd, TRUE);

    g_hPlayerWnd = m_hWnd;
	
	// 创建独立控制栏
	if (NULL == m_dlgCtrl)
	{
		m_dlgCtrl = new SCtrlDlg;
		m_dlgCtrl->Create(m_hWnd);
		m_dlgCtrl->SendMessageW(WM_INITDIALOG);
		m_dlgCtrl->ShowWindow(SW_HIDE);
	}

	// 创建flash播放插件
    m_pFlash = FindChildByName2<SFlashCtrl>(L"flash_player");

    //绑定进度条事件
    SSliderBar* pSlider = FindChildByName2<SSliderBar>(L"slider_player");
    if (pSlider) {
        pSlider->GetEventSet()->subscribeEvent(EVT_SLIDER_POS, Subscriber(&CPlayerDlg::SliderEvent, this));
    }
	pSlider = FindChildByName2<SSliderBar>(L"slider_volume");
	if (pSlider)
		pSlider->GetEventSet()->subscribeEvent(EVT_SLIDER_POS, Subscriber(&CPlayerDlg::SliderEvent, this));
	
	SetPlayStatus(ePS_Stoped);
	SetPlayerShowMode(ePlayerWndMode_Nomal);

	//CreatePlayer();

	// 初始状态隐藏播放器窗口
	RePlayerShow(0);

    return 0;
}
/*
	因为老板要求当前播放的文件在播放列表的最上边
	所以，拖拽文件进来的时候，播放的文件，应该最后添加进播放列表。
	可以借鉴打开文件夹的做法，修改这个函数。
	首先遍历文件夹，然后无论是否需要播放，统一发送play消息给主窗口，由主窗口来添加播放列表并决定播放那一个文件。
	随后再做，最近时间先处理更重要的事项。
*/
void CPlayerDlg::OnDropFiles(HDROP hDropInfo)
{
    int nFiles = DragQueryFile(hDropInfo, -1, nullptr, 0); //获取文件数量
    for (size_t i = 0; i < nFiles; i++)
    {
        WCHAR* szPath = new WCHAR[MAX_PATH];
        memset(szPath, 0, MAX_PATH);
        DragQueryFileW(hDropInfo, i, szPath, MAX_PATH); // 获取文件路径

        int nRet = IsExistFormat(szPath);
        if (0 == nRet) 
			continue;
        if (3 == nRet) //字幕
        {
            m_CPlayer.SetSubtitlefile(LeTools::ConvertGBKToUtf8(szPath).c_str());
            continue;
        }

		// 播放消息统一发给maindlg处理
		wstring wstrName = szPath;
		wstrName = wstrName.substr(wstrName.rfind(L"\\") + 1);
		sPlayData* pData = new sPlayData;
		pData->eSignal = eSignalType_DrogFile;
		pData->eSrcType = eSourceType_Local;
		pData->bNoHistory = false;
		pData->strSource = szPath;
		pData->strName = wstrName.c_str();
		pData->nNum = 0;
		pData->nTotal = 0;
		pData->bOnlyAddList = (0 == i) ? false : true;
		pData->ifConsume = _T("2");
		::PostMessage(CMainDlg::g_hMainWnd, UM_PLAY, (WPARAM)pData, 0);

		delete szPath;
    }

    ::DragFinish(hDropInfo);
}

void CPlayerDlg::OnSize(UINT nType, CSize size)
{
    SetMsgHandled(FALSE);
    if (!m_bLayoutInited) 
		return;

	if (!m_bMiniShow && m_sMiniRect.bSetModeOver && m_sMiniRect.bInited && ePlayerWndMode_Mini == m_eShowMode)
	{// 记录迷你窗口的大小
		m_sMiniRect.cRect_Mini.right = m_sMiniRect.cRect_Mini.left + size.cx;
		m_sMiniRect.cRect_Mini.bottom = m_sMiniRect.cRect_Mini.top + size.cy;
	}

    if (m_dlgCtrl) 
	{
        CRect rcWnd = GetWindowRect();
        m_dlgCtrl->SetWindowPos(HWND_TOP, rcWnd.left, rcWnd.top + 1, rcWnd.Width(), rcWnd.Height() - 2, SWP_NOACTIVATE);
    }
}

void CPlayerDlg::OnTimer(UINT_PTR nIDEvent)
{
    SetMsgHandled(FALSE);
    if (ET_SHOW_VOLUME == nIDEvent)
    {
        SStatic* pVoice = FindChildByName2<SStatic>(L"voice");
        if (pVoice)pVoice->SetAttribute(L"show",L"0");
        KillTimer(nIDEvent);
    }   
    if (ET_HIDETIG == nIDEvent)
    {
		ShowRightTig(FALSE);        
       // PostMessage(UM_RECHECK_TOP);
    }
	if (ET_HIDEHEAD ==nIDEvent)
	{
		KillTimer(ET_HIDEHEAD);

		// 如果鼠标在浮动栏或者控制栏，就再等2秒
		CPoint pt;
		GetCursorPos(&pt);
		RECT rc = GetWindowRect();
		RECT rc_top = rc;
		rc_top.bottom = rc.top + HEIGHT_TITLE;
		RECT rc_ctrl = rc;
		rc_ctrl.top = rc.bottom - HEIGHT_CTRBAR;
		if (PtInRect(&rc_ctrl, pt) || PtInRect(&rc_top, pt))
			SetTimer(ET_HIDEHEAD, 2000);
		else
			ShowFlyHead(FALSE);
	}
    if (ET_CLICK_PAUSE == nIDEvent)
    {
        KillTimer(nIDEvent);
        //点击屏幕暂停
		char chTmp[MAX_PATH] = { 0 };
		string sValue = "1";
		char* pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("clickPause");
		if (NULL != pchValue)
		{
			sValue = pchValue;
			CLeDB::GetInstance()->ReleaseStr(pchValue);
		}
		
        if (atoi(sValue.c_str())&& !m_bMoveFlag)
		{
            DoPause();
        }
        m_bClickTimer = FALSE;
    }
    if (ET_FLASH_PLAY == nIDEvent)
    {
        //初始播放需要判断是否跳到播放记录
        SetSliderMax();
        m_bFirstStart = FALSE;
        long lPlayed = 0;
        //处理音频，滚动条不动
        if (lPlayed == 0) 
            lPlayed = 1;
        m_pFlash->GotoFrame(lPlayed);
        m_pFlash->Play();
		        
        long lFrame = m_pFlash->GetFrameNum();
		SetShowTime(lFrame, m_sPlayData.lTotalTime);
		SetSliderPos();
        if (lFrame >= m_sPlayData.lTotalTime - 3)
        {
			PostMessage(UM_FLASH_PLAY_END, 0, 0);
            KillTimer(nIDEvent);
        }
    }
}

void CPlayerDlg::OnCommand(UINT uNotifyCode, int nID, HWND wndCtl)
{
    SetScreenRatio(nID,TRUE);
	LONGLONG tmp = 0;
    switch (nID)
    {
        /*打开文件*/
    case OPEN_FILE: 
		OnOpenfile((int)wndCtl);
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_PLAY_OPENFILE);
		break;   
        /*打开文件夹*/
    case OPEN_FLODER: 
		OnOpenfloder((int)wndCtl); 
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_PLAY_OPENFILEFLODER);
		break;    
        //片头片尾设置
    case SKIP_HEAD_END:  break;
    case NOSKIP_HEAD_END: break;
        //置顶设置
    case ALWAYS_TOPMOST:
    {		
        SetTopMost(true);
		//::PostMessage(CMainDlg::g_hMainWnd, UM_RESETWND, 0, 0);
        break;
    }
    case NOTOPMOST:
    {		
        SetTopMost(false);
		//::PostMessage(CMainDlg::g_hMainWnd, UM_RESETWND, 0, 0);
        break;
    }    
    
    /*循环模式设置*/
    case Sequential_play://顺序播放
    {
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_FILELIST_SHUNXU);
		CLeDB::GetInstance()->Set()->UpdateSingleSet("playOrder", "0");
        ::PostMessage(m_dlgList->m_hWnd, UM_UPDATE_SKIN, 0, 0);
        break;
    }
    case Random_play://随机播放
    {
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_FILELIST_SUIJI);
		CLeDB::GetInstance()->Set()->UpdateSingleSet("playOrder", "1");
        ::PostMessage(m_dlgList->m_hWnd, UM_UPDATE_SKIN, 0, 0);
        //CLeReport::GetInstance()->SendRTD_Eevent("38", "1", "随机播放");
        break;
    }
    case List_loop://列表循环
    {
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_FILELIST_LISTXUNHUAN);
		CLeDB::GetInstance()->Set()->UpdateSingleSet("playOrder", "4");
        ::PostMessage(m_dlgList->m_hWnd, UM_UPDATE_SKIN, 0, 0);
        //CLeReport::GetInstance()->SendRTD_Eevent("38", "1", "列表循环");
        break;
    }
    case Single_play://单个播放
    {
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_FILELIST_SINGLE);
		CLeDB::GetInstance()->Set()->UpdateSingleSet("playOrder", "2");
        ::PostMessage(m_dlgList->m_hWnd, UM_UPDATE_SKIN, 0, 0);
        //CLeReport::GetInstance()->SendRTD_Eevent("38", "1", "单个播放");
        break;
    }
    case Single_cycle://单个循环
    {
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_FILELIST_SINGLEXUNHUAN);
		CLeDB::GetInstance()->Set()->UpdateSingleSet("playOrder", "3");
        ::PostMessage(m_dlgList->m_hWnd, UM_UPDATE_SKIN, 0, 0);
        break;
    }
        //暂停/继续
    case CMD_PAUSE:
		DoPause(); 
		break;
        //停止
    case CMD_STOP:
		DoStop(); 
		break;
        //快进
    case CMD_FORWARD: 
	{
		char chTmp[MAX_PATH] = { 0 };
		string sValue = "10";
		char* pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("forwardSpeed");
		if (NULL != pchValue)
		{
			sValue = pchValue;
			CLeDB::GetInstance()->ReleaseStr(pchValue);
		}
		
        Forward(atoi(sValue.c_str()));
    }
	break;
        //快退
    case CMD_REWIND: 
	{
		char chTmp[MAX_PATH] = { 0 };
		string sValue = "10";
		char* pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("rewindSpeed");
		if (NULL != pchValue)
		{
			sValue = pchValue;
			CLeDB::GetInstance()->ReleaseStr(pchValue);
		}		
        Rewind(atoi(sValue.c_str()));
    }
	break;
        //设置A点
    case CMD_SET_A:
    {
        SSliderBar* pSlider = FindChildByName2<SSliderBar>(L"slider_player");
        if (pSlider) 
		{
            pSlider->SetApos();
        }
        SWindow::Invalidate();
        break;
    }
        //设置B点
    case CMD_SET_B:
    {
        SSliderBar* pSlider = FindChildByName2<SSliderBar>(L"slider_player");
        if (pSlider) {
            pSlider->SetBpos();
        }
        SWindow::Invalidate();
        break;
    }
        //取消AB点
    case CMD_NO_AB:
    {
        SSliderBar* pSlider = FindChildByName2<SSliderBar>(L"slider_player");
        if (pSlider) {
            pSlider->RemoveAB();
        }
        SWindow::Invalidate();
        break;
    }
        //全屏
    case FULL_SCREEN:
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_CLICK_FULL_YOUJIAN);
		DoFullScreen(); 
		break;
        //静音
    case CMD_MUTE:DoMute(); break;
        //无恒模式
    case NO_PLAY_RECORD:
		CLeDB::GetInstance()->Set()->UpdateSingleSet("noMark","1"); 
		break;
        //取消无恒模式
    case REMOVE_NOPLAY_RECORD:
		CLeDB::GetInstance()->Set()->UpdateSingleSet("noMark", "0"); 
		break;
        //添加本地字幕
    case SUBTITLE_LOAD:OnAddSubtitle((int)wndCtl);
		break;
        //mini模式
    case MINI_SCREEN:
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_CLICK_MINI_YOUJIAN);
		SetPlayerShowMode(ePlayerWndMode_Mini);
		::PostMessage(CMainDlg::g_hMainWnd, UM_SETMIMI_WND, 1, 1);
		//SetPlayerShowMode(ePlayerWndMode_Mini); 
		break;
        //取消mini模式
    case NORMAL_SCREEN:
		//SetPlayerShowMode(ePlayerWndMode_Full);
		::PostMessage(CMainDlg::g_hMainWnd, UM_SETMIMI_WND, 1, 0);
		break;
        //隐藏字幕
    //case SUBTITLE_START:
    //{
    //   m_CPlayer.SetSubtitleSpu(-1);
    //   break;
    //}
    case AudioChannel_Stereo:
    {
		CLeDB::GetInstance()->Set()->UpdateSingleSet("channel", "0");
        m_CPlayer.SetAudioChannel(0);
        break;
    }    
    case AudioChannel_Left:
    {
		CLeDB::GetInstance()->Set()->UpdateSingleSet("channel", "1");
        m_CPlayer.SetAudioChannel(1);
        break;
    }
    case AudioChannel_Right:
    {
		CLeDB::GetInstance()->Set()->UpdateSingleSet("channel", "2");
        m_CPlayer.SetAudioChannel(2);
        break;
    }
	case AudioChannel_RStereo:
	{
		CLeDB::GetInstance()->Set()->UpdateSingleSet("channel", "3");
		m_CPlayer.SetAudioChannel(3);
		break;
	}
    case AudioChannel_Dolbys:
    {
		CLeDB::GetInstance()->Set()->UpdateSingleSet("channel", "4");
        m_CPlayer.SetAudioChannel(4);
        break;
    }
    default:
        break;
    }

    //设置字幕子轨迹
    for (size_t i = 0; i < m_vecSubCmdID.size(); i++)
    {
        if (nID == m_vecSubCmdID[i])
        {
            m_CPlayer.SetSubtitleSpu(m_vecSubCmdID[i] - SUBTITLE_START);
            break;
        }
    }
}

void CPlayerDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (GetPlayStatus() == ePS_Init || GetPlayStatus() == ePS_Stoped)
		return;

    SetMsgHandled(FALSE);
	if (!PtInPlayer(point))
	{
		return;
	}
    SWindow* pWnd = FindChildByName(L"wnd_voice");
    if (pWnd)pWnd->SetAttribute(L"show", L"0");

    //隐藏清晰度窗口
    pWnd = FindChildByName(L"wnd_definition");
    if (pWnd)pWnd->SetAttribute(L"show", L"0");

    CRect rcWnd = CRect(0, 0, 0, 0);
    if (ePlayerWndMode_Full != m_eShowMode)
    {
        ::GetWindowRect(m_hWnd, rcWnd);
    }
	/*
    SMenu menu;
    menu.LoadRes(_T("smenu:right_menu"));
	*/
	SMenuEx menu;
	menu.LoadRes(_T("smenuex:right_menuex"));

    _HotKey hk;
    //设置热键信息
    int nID = 10;
    for (size_t i = HK_OPENFILE; i < HK_OPENURL; i++) 
	{
		CLeDB::GetInstance()->HotKey()->GetHotkey(mapKey[i].c_str(), hk);
        menu.SetItemValue(nID++, L"hotkeyText", hk.GetWString().c_str());
    }
	CLeDB::GetInstance()->HotKey()->GetHotkey(mapKey[HK_PAUSE].c_str(), hk);
    menu.SetItemValue(21, L"hotkeyText", hk.GetWString().c_str());
	CLeDB::GetInstance()->HotKey()->GetHotkey(mapKey[HK_STOP].c_str(), hk);
    menu.SetItemValue(22, L"hotkeyText", hk.GetWString().c_str());
	CLeDB::GetInstance()->HotKey()->GetHotkey(mapKey[HK_FORWARD].c_str(), hk);
    menu.SetItemValue(23, L"hotkeyText", hk.GetWString().c_str());
	CLeDB::GetInstance()->HotKey()->GetHotkey(mapKey[HK_REWIND].c_str(), hk);
    menu.SetItemValue(24, L"hotkeyText", hk.GetWString().c_str());
	CLeDB::GetInstance()->HotKey()->GetHotkey(mapKey[HK_FULLSCREEN].c_str(), hk);
    menu.SetItemValue(40, L"hotkeyText", hk.GetWString().c_str());
	CLeDB::GetInstance()->HotKey()->GetHotkey(mapKey[HK_MUTE].c_str(), hk);
    menu.SetItemValue(54, L"hotkeyText", hk.GetWString().c_str());
    //设置屏幕比例信息
	char chTmp[MAX_PATH] = { 0 };
	string sValue = "0";
	char* pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("screenRatio");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}
	
    menu.SetItemValue(atoi(sValue.c_str()) + SCREEN_NORMAL, L"radio", L"1");
    //声道
	pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("channel");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}
	
    menu.SetItemValue(60 + atoi(sValue.c_str()), L"radio", L"1");
    
    /*
    //设置片头片尾信息
    if (g_set.m_bSkipHeadend)
        menu.SetItemValue(49, L"radio", L"1");
    else
        menu.SetItemValue(50, L"radio", L"1");
        */
    //设置置顶信息
	pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("topmost");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}
	
    menu.SetItemValue(NOTOPMOST + atoi(sValue.c_str()), L"radio", L"1");
	

    //如果全屏
	if (ePlayerWndMode_Full == m_eShowMode)
		menu.SetItemValue(40, L"enable", L"0");
        menu.SetItemValue(40, L"check", L"1");
    //如果是静音
    if (m_bMute)
        menu.SetItemValue(54, L"check", L"1");
    //如果mini模式
    if (ePlayerWndMode_Mini == m_eShowMode)
        menu.SetItemValue(41, L"check", L"1");
    else
        menu.SetItemValue(42, L"check", L"1");

    //全屏或mini模式下不可打开网页
    if (ePlayerWndMode_Mini == m_eShowMode || ePlayerWndMode_Full == m_eShowMode)
    {
        menu.SetItemValue(12, L"disable", L"1");
    }

    int nEnable = 0;
    if (ePlayerWndMode_Full == m_eShowMode ||m_bShowWeb) {
        nEnable = 1;
    }

    if (m_bShowWeb){//打开网页时全屏不可用
        menu.SetItemValue(40, L"disable", L"1");
    }

    WCHAR wBuff[64] = { 0 };
    _itow(nEnable, wBuff, 10);
    //mini模式item是否启用
    menu.SetItemValue(41, L"disable", wBuff);
    menu.SetItemValue(42, L"disable", wBuff);

    m_vecSubCmdID.clear();//清空原有的ID记录
                          //只有在播放时字幕可用
    if (GetPlayStatus() == ePS_Playing)
    {
        //播放 / 暂停，停止，快进，快退，AB点重复，仅在播放媒体文件时有效，
        menu.SetItemValue(200, L"disable", L"0");
        menu.SetItemValue(57, L"disable", L"0");
		//libvlc_track_description_t* pSpu = NULL;/*m_avPlayer*///m_CPlayer.GetSpuDescription();
//         if (pSpu)
//         {
//             menu.SetItemValue(SUBTITLE_CHILDS, L"disable", L"0");
//             while (pSpu)
//             {
//                 if (pSpu->i_id != -1)
//                 {
//                     string strSubname = pSpu->psz_name;
//                     if (LeTools::IsTextUTF8((char*)strSubname.c_str(), strSubname.length())) {
//                         strSubname =LeTools::ConvertUtf8ToGBK(pSpu->psz_name);
//                     }
// 
//                     menu.AddChildItem(SUBTITLE_CHILDS,LeTools::s2ws(strSubname).c_str(),
//                         SUBTITLE_START + pSpu->i_id, AddMenuItem);
//                     m_vecSubCmdID.push_back(SUBTITLE_START + pSpu->i_id);
//                 }
//                 pSpu = pSpu->p_next;
//             }
//         }
        //当前字幕不显示
        if (m_CPlayer.GetSubtitleSpu() == -1)
        {
            menu.SetItemValue(SUBTITLE_START, L"radio", L"1");
        }
    }
	menu.setShowMode(m_eShowMode);
    menu.LoadMenu();
	//::SetParent((HWND)menu.GetSwnd(), m_hWnd);
	
    menu.TrackPopupMenu(TPM_NONOTIFY|TPM_TOPALIGN, rcWnd.left + point.x, rcWnd.top + point.y, m_hWnd);
	
    //SetForegroundWindow(m_hWnd);//解决窗口无法收到消息问题
}

void CPlayerDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    SetMsgHandled(FALSE);    

    BOOL bAtl = FALSE, bShift = FALSE, bCtrl = FALSE;
    if (GetAsyncKeyState(VK_MENU) >> ((sizeof(SHORT) * 8) - 1))
        bAtl = TRUE;

    if (GetAsyncKeyState(VK_SHIFT) >> ((sizeof(SHORT) * 8) - 1))
        bShift = TRUE;

    if (GetAsyncKeyState(VK_CONTROL) >> ((sizeof(SHORT) * 8) - 1))
        bCtrl = TRUE;	

    for (size_t i = HK_OPENFILE; i <= HK_FULLSCREEN; i++)
    {
        _HotKey hk;
        CLeDB::GetInstance()->HotKey()->GetHotkey(mapKey[i].c_str(), hk);
        if (hk.IsHotKeyDown(bCtrl, bAtl, bShift, nChar))
        {
            OnHotkey(i);
            return;
        }
    }

    if (VK_F2 == nChar)
    {
        if (!m_bSetOpen)
        {
            m_bSetOpen = TRUE;
            CMainSetDlg dlg;
            dlg.DoModel();
            m_bSetOpen = FALSE;
        }
    }
    
    if (VK_ESCAPE == nChar && ePlayerWndMode_Full == m_eShowMode)
    {
        DoFullScreen();
    }
}

void CPlayerDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	SetMsgHandled(FALSE);
	// 显示和隐藏右侧开关
	static CPoint ptOld = CPoint(0, 0);
	if (ptOld.x != point.x || ptOld.y != point.y)
	{
		ptOld = CPoint(point.x, point.y);
		if (ePlayerWndMode_Nomal == m_eShowMode)
		{
			ShowFlyHead(TRUE);
			ShowRightTig(TRUE);
		}
		else
		{
			ShowFlyHead(TRUE);
		}
		
	}
	return;
}

void CPlayerDlg::OnMove(CPoint ptPos)
{
    SetMsgHandled(FALSE);
	// 非迷你模式，窗口控制比较复杂，独立控制栏要消失，迷你模式下，只能依赖浮动栏拖动，所以不要消失。
	
	if (m_dlgCtrl)
	{
		if (ePlayerWndMode_Nomal == m_eShowMode)
			m_dlgCtrl->ShowWindow(SW_HIDE);		
	}

	if (m_bMiniShow)
		return;

	if (ePlayerWndMode_Mini == m_eShowMode)
	{
		int cxScreen = GetSystemMetrics(SM_CXSCREEN);
		RECT RC = GetClientRect();
		if (cxScreen <= RC.right - RC.left)
		{
			SendMessage(WM_SYSCOMMAND, SC_RESTORE);
			return;
		}
	}
	
	if (ptPos.x != 0 || ptPos.y != 0)
	{
		if (m_sMiniRect.bSetModeOver && m_sMiniRect.bInited && ePlayerWndMode_Mini == m_eShowMode)
		{
			m_bMoveFlag = TRUE;
			int nW = m_sMiniRect.cRect_Mini.Width();
			int nH = m_sMiniRect.cRect_Mini.Height();
			CRect winRt = GetWindowRect();
			m_sMiniRect.cRect_Mini.left = ptPos.x;
			m_sMiniRect.cRect_Mini.top = ptPos.y;
			m_sMiniRect.cRect_Mini.right = m_sMiniRect.cRect_Mini.left + nW;
			m_sMiniRect.cRect_Mini.bottom = m_sMiniRect.cRect_Mini.top + nH;
			if (m_dlgCtrl)
			{
				m_dlgCtrl->ShowHead(FALSE);
				m_dlgCtrl->ShowCtrl(FALSE);
				//::PostMessage(m_dlgCtrl->m_hWnd,WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(winRt.left, winRt.top));
				//::PostMessage(m_dlgCtrl->m_hWnd, WM_MOVE, MAKEWPARAM(), MAKELPARAM());
				//m_dlgCtrl->SetWindowPos(NULL, winRt.left, winRt.top, nW, nH, SWP_SHOWWINDOW);
			}
		}	
	}
}

void CPlayerDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    SetMsgHandled(FALSE);
    if (m_dlgCtrl) 
	{
        PostMessage(UM_RECHECK_TOP);
        //m_dlgCtrl->ShowVolume(FALSE, -1);
        ::PostMessage(m_dlgCtrl->m_hWnd, UM_HIDE_RESOLU,0,0);
    }
    //点击在控制栏上
    SWindow* pWnd = FindChildByName(L"wnd_ctrl");
    if (pWnd)
    {
        CRect rcBtn = pWnd->GetWindowRect();
        if (PtInRect(rcBtn, point)) return;
    }
    
    //点击进度条
    pWnd = FindChildByName(L"slider_player");
    if (pWnd && !pWnd->OnNcHitTest(point))
        return;

    if (GetPlayStatus() == ePS_Stoped && !m_bShowWeb)
    {
        SWindow* pWnd = FindChildByName(L"btn_player_menu");
        if (!pWnd) 
			return;
        CRect rcBtn = pWnd->GetWindowRect();
        CRect rcLeft = rcBtn;
        rcLeft.right -= 40;
		if (GetPlayerShowMode() == ePlayerWndMode_Mini &&PtInRect(rcBtn, point))
		{
			OnOpenfile(0);
			CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_PLAY_OPENFILE_LOGO);
			return;
		}
		else
		{
			if (PtInRect(rcLeft, point))
			{
				OnOpenfile(0);
				CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_PLAY_OPENFILE_LOGO);
				return;
			}

		}
        
        CRect rcRight = rcBtn;
        rcRight.left = rcRight.right-40;
        if (PtInRect(rcRight, point))
        {
            CRect rcWnd = GetWindowRect();

            CPoint pt;
            pt.x = rcWnd.left + rcBtn.left;
            pt.y = rcWnd.top + rcBtn.bottom;
            SMenuEx menu;
            menu.LoadRes(_T("SMENU:player_menuex"));
            _HotKey hk;
            CLeDB::GetInstance()->HotKey()->GetHotkey(mapKey[HK_OPENFILE].c_str(), hk);
            menu.SetItemValue(10, L"hotkeyText", hk.GetWString().c_str());
            CLeDB::GetInstance()->HotKey()->GetHotkey(mapKey[HK_OPENFLODER].c_str(), hk);
            menu.SetItemValue(11, L"hotkeyText", hk.GetWString().c_str());

            menu.LoadMenu();
			
			//
            menu.TrackPopupMenu(0, pt.x, pt.y, m_hWnd);

			CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_PLAY_OPENFILE_LIST);
            return;
        }
    }

	if (ePS_Playing == GetPlayStatus() || ePS_Pause == GetPlayStatus())
	{
		if (!m_bClickTimer)
		{
			m_bClickTimer = TRUE;
			SetTimer(ET_CLICK_PAUSE, 250);
		}
	}    
}

void CPlayerDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    SetMsgHandled(FALSE);
	if (!PtInPlayer(point))
	{
		return;
	}
    SWindow* pWnd = FindChildByName(L"ctrl_fly");
    if (pWnd&&!pWnd->OnNcHitTest(point))
        return;
    pWnd = FindChildByName(L"head_fly");
    if (pWnd&&!pWnd->OnNcHitTest(point))
        return;
    pWnd = FindChildByName(L"wnd_voice");
    if (pWnd && !pWnd->OnNcHitTest(point))
        return;
    pWnd = FindChildByName(L"wnd_definition");
    if (pWnd && !pWnd->OnNcHitTest(point))
        return;
    pWnd = FindChildByName(L"slider_player");
    if (pWnd && !pWnd->OnNcHitTest(point))
        return;

	if (ePlayerWndMode_Full == m_eShowMode || ePS_Playing == GetPlayStatus() || ePS_Pause == GetPlayStatus())
	{
		if (m_bClickTimer) 
		{
			KillTimer(ET_CLICK_PAUSE);
			m_bClickTimer = FALSE;
		}
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_CLICK_FULL_DOUBLECLICK);
		DoFullScreen();
	}	
}

BOOL CPlayerDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    SetMsgHandled(FALSE);
    if (m_bShowWeb)
		return 0;
    if (zDelta>0)
        UpVolume(10);
    else
        DownVolume(10);
    return 0;
}

void CPlayerDlg::OnMouseLeave()
{
	return;
}

void CPlayerDlg::OnClose()
{
	// 只有迷你模式会触发这个消息
	m_bMiniShow = FALSE;
    DoStop();
	m_dlgCtrl->ShowHead(FALSE);
	SetPlayerShowMode(ePlayerWndMode_Nomal);
}

void CPlayerDlg::OnMaximize()
{
	m_bMiniShow = FALSE;
    CLeReport::GetInstance()->SendRTD_Eevent(RTD_MAXBTN, "1", "最大化");
    SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
}

void CPlayerDlg::OnRestore()
{
	m_bMiniShow = FALSE;
    SendMessage(WM_SYSCOMMAND, SC_RESTORE);
    CLeReport::GetInstance()->SendRTD_Eevent(RTD_MAXBTN, "1", "最大化还原");
}

void CPlayerDlg::OnMinimize()
{
	ShowFlyHead(FALSE);

	m_bMiniShow = TRUE;
	char chTmp[MAX_PATH] = { 0 };
	string sValue = "1";
	char* pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("minPause");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}	
    if (atoi(sValue.c_str()))
        Min2Pause();
    
    SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
	return;
}

void CPlayerDlg::ShowLoading(BOOL bShow)
{	
	if (bShow == m_bGifLoading)
		return;//已操作，不再处理

	m_bGifLoading = bShow;

	SStringT strShow = bShow == TRUE ? L"1" : L"0";
	SGifPlayer *pGifPlayer = FindChildByName2<SGifPlayer>(L"gif_load");
	if (pGifPlayer) 
	{
		pGifPlayer->SetAttribute(L"show", strShow);
	}
	SWindow* pWnd = FindChildByName(L"load_text");
	if (pWnd)
		pWnd->SetAttribute(L"show", strShow);
	return;
}

void CPlayerDlg::OnBtnResolution()
{
    if (m_dlgCtrl)m_dlgCtrl->OnBtnResolution();
}

void CPlayerDlg::OnBtnLast()
{
	::PostMessage(CMainDlg::g_hMainWnd, UM_PLAY_LAST, 0, 0);
}

void CPlayerDlg::OnBtnNext()
{
	::PostMessage(CMainDlg::g_hMainWnd, UM_PLAY_NEXT, 0, 0);
	CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_CLICK_NEXT);
}

bool CPlayerDlg::SliderEvent(EventArgs * pEvt)
{
    if (EVT_SLIDER_POS == pEvt->GetID())
    {
        if (pEvt->sender->GetName() == tr(_T("slider_player")))
        {
            SSliderBar* pSlider = (SSliderBar*)pEvt->sender;
            EventSliderPos* evtPos = (EventSliderPos*)pEvt;

            if (!pSlider->isDrag()) 
			{
                if (eLocalFile_swf == m_eFileType)
				{
                    BOOL bPlay = m_pFlash->IsPlaying();
                    int nNewTime = m_pFlash->GetFrameNum();
                    m_pFlash->GotoFrame(evtPos->nPos);
                    if (bPlay) 
						m_pFlash->Play();
                    SetShowTime(evtPos->nPos, m_sPlayData.lTotalTime);

                    //上报数据
                    string sName =LeTools::ws2s(m_sPlayData.strSource);
                    sName = sName.substr(sName.rfind("\\") + 1);
                }
                else {
					int smin, smax;
					pSlider->GetRange(&smin, &smax);
                    int nPos = evtPos->nPos;
                    if (m_bShowLink&&nPos > g_lDefaultTime)
						nPos = g_lDefaultTime;
					m_CPlayer.Seek(nPos);
					SetShowTime(nPos, m_sPlayData.lTotalTime);
                    
					//SetSliderPos();
                }
            }
        }
		if (pEvt->sender->GetName() == tr(_T("slider_volume")))
		{
			EventSliderPos* evtPos = (EventSliderPos*)pEvt;
			::PostMessage(g_hPlayerWnd, UM_VLUME_POS, 0, evtPos->nPos);
			SWindow* pWnd = FindChildByName(L"voice");
			WCHAR wBuff[64] = { 0 };
			_itow(evtPos->nPos, wBuff, 10);
			if (pWnd)
				pWnd->SetWindowTextW(wBuff);
		}
    }
    return false;
}

void CPlayerDlg::OnDefinitionID(int nID)
{
    SWindow* pWnd = FindChildByName(L"wnd_definition");
    if (pWnd)pWnd->SetAttribute(L"show",L"0");

    if (nID == 881)
    {
        UpdateResolu(1);        
        return;
    }
    if (nID == 882)
    {
        UpdateResolu(2);        
        return;
    }
    if (nID == 883)
    {
        UpdateResolu(3);        
        return;
    }
}

void CPlayerDlg::OnBtnVolume()
{
	int nStatus = 0;
	SWindow* pWnd = FindChildByName(L"btn_volume");
	static int nOldVolume = m_nVolume;
	nOldVolume = m_nVolume;
	if (pWnd)
	{
		m_bMute = !m_bMute;
		m_CPlayer.SetMute(m_bMute);
		if (m_bMute) 
		{
			pWnd->SetAttribute(L"skin", L"ctrl_jy1");
			pWnd->SetAttribute(L"focusSkin", L"ctrl_jy2");
		}
		else
		{
			pWnd->SetAttribute(L"skin", L"ctrl_volume1");
			pWnd->SetAttribute(L"focusSkin", L"ctrl_volume2");
		}
	}
	
	if (m_dlgCtrl)
		m_dlgCtrl->ShowVolume(m_bMute, m_nVolume);
	SSliderBar* pSlider = FindChildByName2<SSliderBar>(L"slider_volume");
	if (pSlider)
	{
		pSlider->SetValue(m_bMute?0:m_nVolume);
	}
	CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_CLICK_VOLUME);
	return;
}

LRESULT CPlayerDlg::WndPro(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DWORD dw = ::GetCurrentThreadId();
    switch (uMsg)
    {
	case UM_AI_PLAY:
	{
		if (ePS_Playing == GetPlayStatus())
			break;
		OnBtnPlay();
	}
	break;
	case UM_AI_PAUSE:
	{
		if (ePS_Pause == GetPlayStatus())
			break;
		OnBtnPlay();
	}
	break;
    case UM_PLAY_PAUSE:
    {
        OnBtnPlay();
    }
	break;
	case UM_PLAY_STOP:
	{
		OnBtnStop();
	}break;
    case UM_SET_PLAYPOS:
    {
        if (eLocalFile_swf == m_eFileType)
		{
        }
        else 
		{
			m_CPlayer.Seek(lParam);
        }
    }break;
    case UM_BTN_SET:
    {
        OnBtnSet();
    }break;
    case UM_SET_FULLSCREEN:
    {		
        OnFullScreen();		
    }
	break;
    case UM_MINI_MODE:
    {		
        OnMiniMode();		
    }
	break;
    case UM_SHOW_VOLUME:
    {
		OnBtnVolume();
    }break;
    case UM_VLUME_POS:
    {
        SetVolume(lParam);
    }
	break;    
    case UM_PLAY_ERROR:
    {
        DoStop();
    }break;
    case UM_MINI_MAX:
    {
        if (lParam == 1)
            OnMaximize();
        else
            OnRestore();
    }break;
    case UM_MINI_MIN:
    {
        OnMinimize();
    }break;	
	case UM_SHOW_PLAYHEAD_MSG:
		ShowFlyHead(FALSE);
		break;
	case UM_PLAY_FUNTV:
		FunTv_AfterGet((char*)wParam, (char*)lParam);
		break;
    default:
        break;
    }
    return 0;
}

LRESULT CPlayerDlg::PlayEndPro(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DoStop(TRUE);
	::PostMessage(m_dlgCtrl->m_hWnd, UM_PLAY_POS, 0, 0);
	//隐藏进度条以及播放时间归零
	SSliderBar* pSliderBar = FindChildByName2<SSliderBar>(L"slider_player");
	if (pSliderBar)
	{
		pSliderBar->RemoveAB();
		pSliderBar->SetAttribute(L"show", L"0");
		pSliderBar->SetValue(0);
	}
	SetShowTime(0, 0);
	OnBtnNext();

	return 0;
}

/*检查窗口置顶属性*/
LRESULT CPlayerDlg::ReCheckTop(UINT uMsg, WPARAM wParam, LPARAM lParam)
{	
    SetTopMost(GetTopMostSet());
    return 0;
}

bool CPlayerDlg::PtInPlayer(CPoint pt)
{
	SWindow * pPlay = FindChildByName("player");
	CRect rt;
	rt =pPlay->GetClientRect();
	return PtInRect(rt, pt);
}

eLocalFileType CPlayerDlg::GetLocalFileType()
{
	return m_eFileType;
}

void CPlayerDlg::SetLocalFileType(eLocalFileType eT)
{
	if (m_eFileType == eT)
		return;
	m_eFileType = eT;
	if (m_dlgCtrl)
		m_dlgCtrl->SetLocalFileType(eT);
	return;
}

void CPlayerDlg::GetFunTVPlayAdress(char* pUrl, HWND hWnd)
{
	char* ret = NULL;
	if (NULL != pFunTVAdress)
		ret = pFunTVAdress(pUrl);	
	
	int nSize_fsp = strnlen_s(pUrl, MAX_PATH);
	int nSize_Adress = strnlen_s(ret, MAX_PATH);
 	if (nSize_Adress > 0)
	{
		char* pfsp = new char[nSize_fsp + 1];
		memset(pfsp, 0, nSize_fsp + 1);
		memcpy(pfsp, pUrl, nSize_fsp);
		char* pA = new char[nSize_Adress + 1];
		memset(pA, 0, nSize_Adress + 1);
		memcpy(pA, ret, nSize_Adress);
		PostMessage(UM_PLAY_FUNTV, (WPARAM)pfsp, (LPARAM)pA);
	}
	else
	{
		char* pFsp = new char[MAX_PATH];
		memset(pFsp, 0, MAX_PATH);
		memcpy(pFsp, pUrl, strnlen_s(pUrl, MAX_PATH));
		PostMessage(UM_PLAY_FUNTV, (WPARAM)pFsp, 0);
	}
	return;
}

void CPlayerDlg::PlayFucTV(char* pFsp, char* pAdress)
{
	if (NULL == pAdress)
	{
		// 已经停止播放了，就不再提示下架了；
		if (ePS_Stoped == m_ePlayerStatus)
			return;
		// 如果已经切换视频了，就不再提示下架了
		string strFsp_Old =LeTools::ws2s(m_sPlayData.strSource);
		if (0 != strFsp_Old.compare(pFsp))
			return;

		// 停止播放
		DoStop();
		// 提示版权原因导致失败
		Playfailed dlg;
		dlg.DoModel();
	}
	else
	{
		// 比较fsp，如果不一样，就返回，不播放
		string strFsp = pFsp;
		wstring wstr_Source =LeTools::s2ws(strFsp).c_str();
		if (0 == m_sPlayData.strSource.compare(wstr_Source))
			m_CPlayer.OpenAndPlay(pAdress);
		delete pFsp;
		delete pAdress;
	}	
	return;
}

void CPlayerDlg::ShowCtrlWnd(BOOL bShow /*= TRUE*/)
{
	if (!m_dlgCtrl)
		return;

	m_dlgCtrl->ShowWindow(bShow ? SW_SHOWNORMAL : SW_HIDE);
}

void CPlayerDlg::OnNCXButtonDown(int fwButton, short nHittest, CPoint ptPos)
{
	SetMsgHandled(FALSE);
}
void CPlayerDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	SetMsgHandled(FALSE);
	m_bMoveFlag = FALSE;
}
