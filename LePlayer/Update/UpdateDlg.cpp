#include "../stdafx.h"
#include "UpdateDlg.h"
#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>
#include <sstream>
#include "../MainDlg.h"
#include "CUpdateHelper.h"
#include "../include/LeReport.h"

#define TIMERID_DOWNLOAD_BEGIN				1001
#define TIMERID_DOWNLOAD_PROGRESS			1002

SUpdateDlg::SUpdateDlg(string appLoadUrl, string versionName) : SHostWnd(_T("LAYOUT:XML_ONLINEUPDATEWND"))
, m_NetErrorCount(0)
, m_appLoadUrl(appLoadUrl)
, m_versionName(versionName)
{
    m_pProgress = nullptr;
    m_pProNum = nullptr;
    m_nPage = 0;
    m_bNoUpdate = FALSE;
	m_hWndParent = NULL;
	m_bSuccessed = false;
}

SUpdateDlg::~SUpdateDlg()
{
}

int SUpdateDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    SetMsgHandled(FALSE);
    return 0;
}

BOOL SUpdateDlg::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    m_bMsgTransparent = TRUE;

	SWindow* pWnd = FindChildByName(L"windowprogress");
	if (pWnd)
		pWnd->SetAttribute(L"show", L"1");
	pWnd = FindChildByName(L"btn_close");
	if (pWnd)
		pWnd->SetAttribute(L"show", L"0");
	pWnd = FindChildByName(L"error");
	if (pWnd)
		pWnd->SetAttribute(L"show", L"0");

	//进度progress
	m_stTipText = FindChildByName2<SStatic>(L"progress_tip_text");
	m_pProNum = FindChildByName2<SStatic>(L"progress_pre_text");
	m_pProgress = FindChildByName2<SProgress>(L"progress_progress");
	m_iImage = FindChildByName2<SImageWnd>(L"image");

	m_vecTipText.push_back(L"正在更新，宝宝要耐心哦");
	m_vecTipText.push_back(L"海量片源，热门影视剧，看看新闻，4K花园......你想看的都在这里");

	// 这里要设置定时器，先让对话框显示出来，再开始下载
	SetTimer(TIMERID_DOWNLOAD_BEGIN, 100);
    return 0;
}

void SUpdateDlg::OnClose()
{
	m_bSuccessed = false;
	if ("2" == CUpdatehelper::GetInstance()->GetUpdateType())
	{
		// 如果是强制升级，直接退进程
		::PostMessage(CMainDlg::g_hMainWnd, UM_CLOSE, 0, 0);
	}
	else
		PostMessage(WM_QUIT);

	return;
}

void SUpdateDlg::OnRetry()
{
	SWindow* pWnd = FindChildByName(L"windowprogress");
	if (pWnd)
		pWnd->SetAttribute(L"show", L"1");
	pWnd = FindChildByName(L"btn_close");
	if (pWnd)
		pWnd->SetAttribute(L"show", L"0");
	pWnd = FindChildByName(L"error");
	if (pWnd)
		pWnd->SetAttribute(L"show", L"0");

	m_cUpdate.DownloadAndUnzip(m_appLoadUrl.c_str(), m_versionName.c_str(), true, m_hWnd);	
	return;	
}

bool SUpdateDlg::IsSuccessed()
{
	return m_bSuccessed;
}

BOOL SUpdateDlg::DoModel()
{
	BOOL bEnableParent = FALSE;	
	m_hWndParent = CMainDlg::g_hMainWnd;
	if (m_hWndParent && m_hWndParent != ::GetDesktopWindow() && ::IsWindowEnabled(m_hWndParent))
	{
		::EnableWindow(m_hWndParent, FALSE);
		bEnableParent = TRUE;
	}

	if (!Create(m_hWndParent, WS_POPUP | WS_CLIPCHILDREN, 0, 0, 0, 0, 0))
		return 0;
	CSimpleWnd::SendMessage(WM_INITDIALOG, (WPARAM)m_hWnd);

	HWND hWndLastActive = ::SetActiveWindow(m_hWnd);

	if (GetExStyle()&WS_EX_TOOLWINDOW)
		::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
	else
		::ShowWindow(m_hWnd, SW_SHOWNORMAL);

	SetWindowPos(HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (IsWindow())
	{
		CSimpleWnd::SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	}

	if (bEnableParent)
		::EnableWindow(m_hWndParent, TRUE);

	::SetActiveWindow(m_hWndParent);

	if (IsWindow())
		CSimpleWnd::DestroyWindow();

	return 0;
}

void SUpdateDlg::ShowError()
{
	SWindow* pWnd = FindChildByName(L"windowprogress");
	if (pWnd)
		pWnd->SetAttribute(L"show", L"0");
	pWnd = FindChildByName(L"btn_close");
	if (pWnd)
		pWnd->SetAttribute(L"show", L"1");
	pWnd = FindChildByName(L"error");
	if (pWnd)
		pWnd->SetAttribute(L"show", L"1");

	return;
}

LRESULT SUpdateDlg::WndPro(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	eUpdateMsg ePM = (eUpdateMsg)uMsg;
	switch (ePM)
	{
	case eUPDATE_Progress:
	{
		// 每次进度更新时把计时器清零，计时器够60秒，则认为网络异常
		KillTimer(TIMERID_DOWNLOAD_PROGRESS);
		SetTimer(TIMERID_DOWNLOAD_PROGRESS, 1000);

		static ULONG llMaxSize = 0;
		if (m_pProgress)
		{
			if (llMaxSize != lParam)
			{
				llMaxSize = lParam;
				WCHAR wBuff[64] = { 0 };
				wsprintf(wBuff, L"%ld", llMaxSize);
				m_pProgress->SetAttribute(L"max", wBuff);
			}
			m_pProgress->SetValue(wParam);
		}
		if (m_pProNum&&lParam)
		{
			WCHAR wBuff[128] = { 0 };
			int n = (__int64)wParam * 100 / lParam;
			wsprintf(wBuff, L"%d%%", n);
			m_pProNum->SetWindowTextW(wBuff);
		}
	}
	break;	
	case eUPDATE_Download_Success:
	{	
		CLeReport::GetInstance()->SendRTD_Eevent(RTD_UPDATE, "1", "2download");
	}
	break;
	case eUPDATE_Unzip_Success:
	{
		CLeReport::GetInstance()->SendRTD_Eevent(RTD_UPDATE, "1", "3unpack");		
		m_bSuccessed = true;
		PostMessage(WM_QUIT);
	}
	break;
	case eUPDATE_Download_Error:
		CLeReport::GetInstance()->SendRTD_Eevent(RTD_UPDATE, "1", "2downloadfail");		
		break;
	case eUPDATE_Unzip_Error:
		CLeReport::GetInstance()->SendRTD_Eevent(RTD_UPDATE, "1", "3unpackfail");
		break;
	break;
	case eUPDATE_Error_File:
	{
		CLeReport::GetInstance()->SendRTD_Eevent(RTD_UPDATE, "1", "2fileerror");
		ShowError();
	}
	break;	
	default:
		break;
	}
	
	return 0;
}

void SUpdateDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMERID_DOWNLOAD_BEGIN == nIDEvent)
	{
		KillTimer(TIMERID_DOWNLOAD_BEGIN);
		// 这里要判断是否正在升级，如果正在升级，则要挂上回调即可
		if (m_cUpdate.IsDownLoading())
		{
			m_cUpdate.SetCallBackHwnd(m_hWnd);
		}
		else
		{
			// 如果没在升级，则直接开始重新下载
			m_cUpdate.DownloadAndUnzip(m_appLoadUrl.c_str(), m_versionName.c_str(), true, m_hWnd);
		}
	}
	else if (TIMERID_DOWNLOAD_PROGRESS == nIDEvent)
	{
		m_NetErrorCount++;
		if (m_NetErrorCount >= 60)
		{
 			m_NetErrorCount = 0;
 			KillTimer(TIMERID_DOWNLOAD_PROGRESS);
			CUpdatehelper::GetInstance()->CancelDownload();
 			ShowError();
		}
	}
	return;
}