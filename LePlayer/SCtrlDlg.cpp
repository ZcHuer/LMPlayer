#include "stdafx.h"
#include "SCtrlDlg.h"
#include "SListDlg.h"
#include "PlayerDlg.h"
#include "MainDlg.h"
//#include "LenovoData.h"
extern SListDlg* m_dlgList;
extern HWND g_hPlayerWnd;

extern SStringT g_strToken;

SCtrlDlg::SCtrlDlg() : SHostWnd(_T("LAYOUT:XML_CTRLWND"))
{
    m_bLayoutInited = FALSE;
    m_bShowList = FALSE;
    m_nResolu = 2;
	m_bShowFly = false;
	m_nDuration = 0;
	m_nCurPos = 0;
	m_eShowMode = ePlayerWndMode_Init;
	m_bCaptureFly = FALSE;
	m_bLBtnDown = false;
}

SCtrlDlg::~SCtrlDlg()
{
}

int SCtrlDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    SetMsgHandled(FALSE);
    return 0;
}

BOOL SCtrlDlg::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    m_bLayoutInited = TRUE;
    DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
    dwStyle &= ~(WS_CAPTION | WS_POPUPWINDOW | WS_THICKFRAME);
    dwStyle |= WS_CHILDWINDOW;
    ::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);//设置成新的样式
    //绑定进度条事件
    SSliderBar* pSlider = FindChildByName2<SSliderBar>(L"slider_player");
    if (pSlider)
        pSlider->GetEventSet()->subscribeEvent(EVT_SLIDER_POS, Subscriber(&SCtrlDlg::SliderEvent, this));
    pSlider = FindChildByName2<SSliderBar>(L"slider_volume");
    if (pSlider)
        pSlider->GetEventSet()->subscribeEvent(EVT_SLIDER_POS, Subscriber(&SCtrlDlg::SliderEvent, this));
	return 0;
}

void SCtrlDlg::ShowCtrl(BOOL bShow)
{
    SFlyWnd* pFly = FindChildByName2<SFlyWnd>(L"ctrl_fly");	
	if (pFly)
	{
		pFly->SetAttribute(L"show", bShow ? L"1" : L"0");
		pFly->SwitchState(!bShow);
	}
	
    if (!bShow)
	{
        SWindow* pWnd = FindChildByName(L"wnd_definition");
        if (pWnd)
			pWnd->SetAttribute(L"show", L"0");
        //ShowVolume(FALSE, 0);
    }
}

void SCtrlDlg::ShowHead(BOOL bShow)
{
	if (m_bShowFly == bShow)
		return;

	m_bShowFly = bShow;

    SFlyWnd* pFly = FindChildByName2<SFlyWnd>(L"head_fly");
	if (pFly)
	{
		pFly->SetAttribute(L"show", bShow ? L"1" : L"0");
		pFly->SwitchState(!bShow);		
	}	
}

void SCtrlDlg::SetHead(SStringT strTitle)
{
    SWindow* pWnd = FindChildByName(L"head_path");
    if (pWnd) 
		pWnd->SetWindowTextW(strTitle);
}

// void SCtrlDlg::ShowFavorites(BOOL bShow)
// {	
//     SStringT strShow = bShow ? L"1" : L"0";
//     SWindow* pWnd = FindChildByName(L"btn_favorites");
//     if (pWnd)
// 		pWnd->SetAttribute(L"show", strShow);
// }

void SCtrlDlg::ShowResol(BOOL bShow)
{
    //非咪咕来源不显示清晰度
    SStringT strShow = bShow ? L"1" : L"0";
    SWindow* pWnd = FindChildByName(L"btn_resolution");
    if (pWnd) {
        pWnd->SetAttribute(L"show", strShow);
    }
}

void SCtrlDlg::ShowClose(BOOL bShow)
{
    //显示顶部控制栏
    SStringT strShow = bShow ? L"1" : L"0";
    SWindow* pWnd = FindChildByName(L"minimode");
    if (pWnd) 
		pWnd->SetAttribute(L"show", strShow);
    SStringT sPos = bShow ? L"-170,|0,@60,@20" : L"-70,|0,@60,@20";
    
	pWnd = FindChildByName(L"btn_favorites");
    if (pWnd)
		pWnd->SetAttribute(L"pos", sPos);
}

void SCtrlDlg::SetPlayerShowMode(ePlayerWndMode eMode)
{	
	if (m_eShowMode == eMode)
		return;

	m_eShowMode = eMode;
	
	SWindow* pWnd_full = FindChildByName(L"btn_fullscreen");
	SWindow* pWnd_caption = FindChildByName(L"movectrl");
	SWindow* pWnd_nomal = FindChildByName(L"btn_nomal");
	if (ePlayerWndMode_Nomal == m_eShowMode)
	{		
		ShowClose(FALSE);
		if (pWnd_caption)
			pWnd_caption->SetAttribute(L"pos", L"0,0,0,0");
		if (pWnd_full)
		{
			pWnd_full->SetAttribute(L"skin", L"ctrl_full1");
			pWnd_full->SetAttribute(L"focusSkin", L"ctrl_full2");
			pWnd_full->SetAttribute(L"tip", L"全屏");
		}
	}
	else if (ePlayerWndMode_Mini == m_eShowMode)
	{
		ShowClose(TRUE);
		if (pWnd_caption)
			pWnd_caption->SetAttribute(L"pos", L"0,0,-0,-0");
		if (pWnd_full)
		{
			pWnd_full->SetAttribute(L"skin", L"ctrl_full1");
			pWnd_full->SetAttribute(L"focusSkin", L"ctrl_full2");
			pWnd_full->SetAttribute(L"tip", L"全屏");
		}
 		if (pWnd_nomal)
 			pWnd_nomal->SetAttribute(L"show", L"1");
	}
	else if (ePlayerWndMode_Full == m_eShowMode)
	{		
		ShowClose(FALSE);
		if (pWnd_caption)
			pWnd_caption->SetAttribute(L"pos", L"0,0,0,0");
 		if (pWnd_nomal)
 			pWnd_nomal->SetAttribute(L"show", L"0");
		if (pWnd_full)
		{
			pWnd_full->SetAttribute(L"skin", L"exitFull1");
			pWnd_full->SetAttribute(L"focusSkin", L"exitFull2");
			pWnd_full->SetAttribute(L"tip", L"退出全屏");
		}		
	}
	
	return;
}

void SCtrlDlg::ShowTig(BOOL bShow)
{
	SWindow* pWnd = FindChildByName(L"btn_tig");
    SStringT strShow = bShow ? L"1" : L"0";
    if (pWnd)
		pWnd->SetAttribute(L"show", strShow);
}

void SCtrlDlg::ShowVolume(BOOL bSlient, int nVolume)
{
	SWindow* pWnd = NULL;
	/*FindChildByName(L"wnd_voice");
    if (pWnd)pWnd->SetAttribute(L"show", strShow);
    if (nVolume >=0)
    {
        pWnd = FindChildByName(L"voice");
        WCHAR wBuff[64] = { 0 };
        _itow(nVolume,wBuff,10);
        if (pWnd)pWnd->SetWindowTextW(wBuff);
    }*/
	pWnd = FindChildByName(L"btn_volume");
	if (bSlient) 
	{
		pWnd->SetAttribute(L"skin", L"ctrl_jy1");
		pWnd->SetAttribute(L"focusSkin", L"ctrl_jy2");
	}
	else {
		pWnd->SetAttribute(L"skin", L"ctrl_volume1");
		pWnd->SetAttribute(L"focusSkin", L"ctrl_volume2");
	}

    SSliderBar* pSlider = FindChildByName2<SSliderBar>(L"slider_volume");
    if (pSlider) 
		pSlider->SetValue(nVolume);
}

void SCtrlDlg::ReBtnRect(SStringT strName, SStringT strPos)
{
    SWindow* pBtn = FindChildByName(strName);
    if (pBtn)pBtn->SetAttribute(L"pos", strPos);
}

void SCtrlDlg::ShowXuanji(BOOL bShow)
{
    SStringT strShow = bShow ? L"1" : L"0";
    SWindow* pWnd = FindChildByName(L"btn_resolution");
    if (pWnd) {
        pWnd->SetAttribute(L"show", strShow);
    }
}

void SCtrlDlg::SetResolu(int nResolu)
{
    m_nResolu = nResolu;
    SStringT str = L"标清";
    if (m_nResolu == 2)str = L"高清";
    if (m_nResolu == 3)str = L"超清";
    SWindow* pWnd = FindChildByName(L"btn_resolution");
    if (pWnd)pWnd->SetWindowTextW(str);
}

void SCtrlDlg::ShowVip(BOOL bShow)
{
    SStringT sStr = bShow ? L"1" : L"0";
    SWindow* pWnd = FindChildByName(L"wnd_lnk");
    if (pWnd)pWnd->SetAttribute(L"show", sStr);
}

void SCtrlDlg::OnBtnVolume()
{
    ::PostMessage(g_hPlayerWnd, UM_SHOW_VOLUME,0,0);
}

void SCtrlDlg::OnBtnLast()
{
    ::PostMessage(CMainDlg::g_hMainWnd,UM_PLAY_LAST,0,0);
}

void SCtrlDlg::OnBtnNext()
{
    ::PostMessage(CMainDlg::g_hMainWnd, UM_PLAY_NEXT,0,0);
}

void SCtrlDlg::OnBtnPause()
{
    ::PostMessage(g_hPlayerWnd, UM_PLAY_PAUSE, 0, 0);
}

void SCtrlDlg::OnBtnFullScreen()
{
    ::PostMessage(g_hPlayerWnd, UM_SET_FULLSCREEN, 0, 0);
}

void SCtrlDlg::OnBtnMiniMode()
{
    ::PostMessage(g_hPlayerWnd, UM_MINI_MODE, 0, 0);
}

void SCtrlDlg::OnBtnSet()
{
    ::PostMessage(g_hPlayerWnd, UM_BTN_SET,0,0);
}

void SCtrlDlg::OnLnkBtn()
{
//     wstring wsCmdLine = L"-id:咪咕";
//     wsCmdLine.append(L" -url:https://www.miguvideo.com/wap/resource/lenovo_pc/memberCenter/user_center.jsp?skin=dark&token=");
//     wsCmdLine.append(g_strToken);
//     ShellExecute(NULL, L"open", g_strSmartHome.c_str(), wsCmdLine.c_str(), NULL, SW_SHOW);
// 
//     if (CPlayerDlg::m_pThis->m_pPlayInfo) {
//         CLeReport::GetInstance()->SendRTD_Eevent("31", "1", "播放窗口底部购买VIP", CPlayerDlg::m_pThis->m_pPlayInfo->name, "2");
//     }
}

void SCtrlDlg::OnClose()
{
    ::PostMessage(CMainDlg::g_hMainWnd,UM_MINI_CLOSE,0,0);
}

void SCtrlDlg::OnMinimize()
{
    ::PostMessage(g_hPlayerWnd, UM_MINI_MIN, 0, 0);
}

void SCtrlDlg::OnBtnResolution()
{
    //CPlayerDlg::m_pThis->ShowAsNoTop();
    CRect rcWnd;
    ::GetWindowRect(g_hPlayerWnd, rcWnd);
    SetWindowPos(0, rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), SWP_NOACTIVATE | SWP_SHOWWINDOW);
    SWindow* pWnd = FindChildByName(L"btn_resolution");
    if (!pWnd)return;
    CRect rcBtn = pWnd->GetWindowRect();
    int nX = rcBtn.right - rcBtn.Width() / 2;

    pWnd = FindChildByName(L"wnd_definition");
    if (!pWnd)return;
    nX -= pWnd->GetWindowRect().Width() / 2;
    pWnd->SetAttribute(L"show", L"1");
    WCHAR cBuff[64] = { 0 };
    wsprintf(cBuff, L"%d,{16,@88,@122", nX);
    pWnd->SetAttribute(L"pos", cBuff);

    for (size_t i = 1; i < 4; i++)
    {
        SStringT sClass = i != m_nResolu ? L"cls_definition" : L"cls_definition1";
        pWnd = FindChildByID(880 + i);
        if (pWnd) pWnd->SetAttribute(L"class", sClass);
    }
}

void SCtrlDlg::SetLocalFileType(eLocalFileType eT)
{
	if (m_eFileType == eT)
		return;
	m_eFileType = eT;
}

void SCtrlDlg::ReSetBtnStatus(ePlayStatus eStatus, sPlayData sData, int nN, int nT)
{
	// 播放暂停按钮
	SWindow* pWnd_Play = FindChildByName(L"btn_play");
	if (!pWnd_Play)
		return;

	bool bPlay = eStatus == ePS_Playing || ePS_Opening == eStatus;
	if (bPlay)
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
	if (sData.eSrcType == eSourceType_Local)
	{// 如果是本地文件，看播放列表里的个数		
		nNum = nN;
		nTotal = nT;
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
			pWnd_next->SetAttribute(L"enable", nNum < nTotal ? L"1" : L"0");
		}
		else
		{
			pWnd_last->SetAttribute(L"enable", L"0");
			pWnd_next->SetAttribute(L"enable", L"0");
		}
	}	

	return;
}

void SCtrlDlg::OnBtnTig()
{
    m_bShowList = !m_bShowList;
    SWindow* pWnd = FindChildByName(L"btn_tig");
    if (pWnd)
    {
        SStringT sStr = m_bShowList ? L"skin_tag1" : L"skin_tag4";
        pWnd->SetAttribute(L"skin", sStr);
        sStr = m_bShowList ? L"skin_tag2" : L"skin_tag3";
        pWnd->SetAttribute(L"focusSkin", sStr);
    }
    ::PostMessage(CMainDlg::g_hMainWnd, UM_SHOW_LISTWND, 0, m_bShowList);
}

void SCtrlDlg::SetShowTime(int nPos, int nDuration)
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
		if (eLocalFile_swf == m_eFileType)
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

bool SCtrlDlg::FlyState(EventArgs* pEvt)
{
	BOOL  bRet = FALSE;
	if (EVT_FLYSTATE == pEvt->GetID())
	{
		if (pEvt->sender->GetName() == tr(_T("movectrl")))
		{
		}
	}

	return bRet;
}

bool SCtrlDlg::SliderEvent(EventArgs * pEvt)
{
    if (EVT_SLIDER_POS == pEvt->GetID())
    {
        if (pEvt->sender->GetName() == tr(_T("slider_player")))
        {
            SSliderBar* pSlider = (SSliderBar*)pEvt->sender;
            EventSliderPos* evtPos = (EventSliderPos*)pEvt;

            if (!pSlider->isDrag()) 
			{
                int nNewTime = 0;
                if (eLocalFile_swf == m_eFileType)
				{
//                     BOOL bPlay = m_pFlash->IsPlaying();
//                     nNewTime = m_pFlash->GetFrameNum();
//                     m_pFlash->GotoFrame(evtPos->nPos);
//                     if (bPlay) 
// 						m_pFlash->Play();
                }
                else 
				{
                    int nPos = evtPos->nPos;
                    //if (m_bShowLink&&nPos > g_lDefaultTime / 1000)nPos = g_lDefaultTime / 1000;
                    nNewTime = m_llNowTime;
                    ::PostMessage(g_hPlayerWnd, UM_SET_PLAYPOS, 0, evtPos->nPos);
                }                
            }
        }
        if (pEvt->sender->GetName()==tr(_T("slider_volume")))
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

void SCtrlDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    SetMsgHandled(FALSE);
    UINT lParam = (nFlags << 16) | nRepCnt;
    ::PostMessage(g_hPlayerWnd, WM_KEYDOWN, nChar, lParam);
}

LRESULT SCtrlDlg::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {	
    case UM_PLAY_POS:
	{
		if (lParam != m_nDuration && lParam != 0)
		{
			m_nDuration = lParam;
			//设置进度条最大值
			SSliderBar* pSliderBar = FindChildByName2<SSliderBar>(L"slider_player");
			if (pSliderBar)
			{
				pSliderBar->RemoveAB();
				pSliderBar->SetAttribute(L"show", L"1");

				wchar_t wTotalTime[64] = { 0 };
				if (eLocalFile_swf == m_eFileType)
					_ltow(m_nDuration, wTotalTime, 10);
				else
					_ltow(m_nDuration, wTotalTime, 10);
				pSliderBar->SetAttribute(L"max", wTotalTime);
			}
		}
		
		SetShowTime(wParam, lParam);
		SSliderBar* pSlider = FindChildByName2<SSliderBar>(L"slider_player");

		if (pSlider && !pSlider->isDrag())
		{
			if (eLocalFile_swf == m_eFileType)
				pSlider->SetValue(wParam);
			else
				pSlider->SetValue(wParam);

			//判读AB点重复
			if (pSlider->IsHaveAB() && pSlider->IsBpos())
			{
				__int64 nNowTime = pSlider->GetApos();
				if (eLocalFile_swf == m_eFileType) {

				}
				else {
					::PostMessage(g_hPlayerWnd, UM_SET_PLAYPOS, 0, nNowTime);
				}
			}
			if (pSlider->IsHaveAB() && pSlider->IsOutAB())
				pSlider->RemoveAB();
		}
		
	}
	break;
    case UM_SKIN_FAVORITE:
    {
        SWindow* pWnd = FindChildByName(L"btn_favorites");
        if (!pWnd)
			break;
        SStringT sColor = wParam == TRUE ? L"#f6c723" : L"#a7acbc";
        pWnd->SetAttribute(L"colorText", sColor);
        SStringT strSkin = lParam == 0 ? L"skin_teleplay" : L"skin_collect";
        if (wParam)
			strSkin += L"_2";
        pWnd->SetAttribute(L"skin", strSkin);
    }break;
    case UM_HIDE_RESOLU:
    {
        SWindow* pWnd = FindChildByName(L"wnd_definition");
        if (pWnd)pWnd->SetAttribute(L"show",L"0");
    }break;
    default:
        break;
    }
    return 0;
}

BOOL SCtrlDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	SetMsgHandled(FALSE);
	::PostMessage(g_hPlayerWnd, WM_MOUSEWHEEL, MAKEWPARAM(0,zDelta), 0);
	return FALSE;
}

void SCtrlDlg::OnMouseLeave()
{
	//SetMsgHandled(FALSE);
	//ShowHead(FALSE);
}

void SCtrlDlg::OnMove(CPoint ptPos)
{
	if (ePlayerWndMode_Mini == m_eShowMode)
	{
// 		SWindow* pWnd_caption = FindChildByName(L"movectrl");
// 		if (NULL == pWnd_caption)
// 			return;
// 		int cxScreen = GetSystemMetrics(SM_CXSCREEN);  // wide
// 		int cyScreen = GetSystemMetrics(SM_CYSCREEN);  // high
// 		if (ptPos.y < 0 || ptPos.y + 200 > cyScreen || ptPos.x < 0 || ptPos.x + 200 > cxScreen)
// 			pWnd_caption->SetAttribute(L"pos", L"0,0,0,0");
// 		else
// 			pWnd_caption->SetAttribute(L"pos", L"0,0,-0,-0");

		::SetWindowPos(g_hPlayerWnd, 0, ptPos.x, ptPos.y - 1, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
		return;
	}
	
	return;
}

void SCtrlDlg::OnBtnStop()
{
	::PostMessage(g_hPlayerWnd, UM_PLAY_STOP, 0, 0);
}
