#include "stdafx.h"
#include "SetDlg.h"
#include "../include/LeReport.h"

extern HWND g_hPlayerWnd;
bool CSetDlg::m_bShow = false;
CSetDlg::CSetDlg():SHostWnd(_T("LAYOUT:XML_SETWND"))
{
    m_bLayoutInited = FALSE;
    m_hWndParent = NULL;
    m_bSure = FALSE;
}

CSetDlg::~CSetDlg()
{
}

int CSetDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    SetMsgHandled(FALSE);
    return 0;
}

BOOL CSetDlg::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    m_bLayoutInited = TRUE;	
    //ÏÔÊ¾±ÈÀý
	char chTmp[MAX_PATH] = { 0 };
	string sValue = "1";
	char* pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("screenRatio");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}

    SRadioBox* pRadiobox = FindChildByID2<SRadioBox>(atoi(sValue.c_str())+ SCREEN_NORMAL);
    if (pRadiobox){
        pRadiobox->SetCheck(1);
    }
    //ÖÃ¶¥ÏÔÊ¾
	pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("topmost");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}
	
    pRadiobox = FindChildByID2<SRadioBox>(NOTOPMOST + atoi(sValue.c_str()));
    if (pRadiobox){
        pRadiobox->SetCheck(TRUE);
    }
    //Æ¬Í·Æ¬Î²
    pRadiobox = FindChildByID2<SRadioBox>(SKIP_HEAD_END);
    if (pRadiobox)
    {
        //pRadiobox->SetCheck(g_set.m_bSkipHeadend);
        pRadiobox->EnableWindow(m_bEnable);
    }
    pRadiobox = FindChildByID2<SRadioBox>(NOSKIP_HEAD_END);
    if (pRadiobox)
    {
        pRadiobox->EnableWindow(m_bEnable);
    }
    return 0;
}
void CSetDlg::SetParent(HWND parent)
{
	m_hWndParent = parent;
}
#include "MainDlg.h"
BOOL CSetDlg::DoModel(BOOL b)
{
	m_bShow = true;
    m_bEnable = b;
    if (!m_hWndParent) {
        m_hWndParent = ::GetActiveWindow();
    }

	BOOL bEnableParent = FALSE;
	if (!m_hWndParent) {
		m_hWndParent = ::GetActiveWindow();
	}
	m_hWndParent = CMainDlg::g_hMainWnd;
	if (m_hWndParent && m_hWndParent != ::GetDesktopWindow() && ::IsWindowEnabled(m_hWndParent)) {
		::EnableWindow(m_hWndParent, FALSE);
		bEnableParent = TRUE;
	}
	if (!Create(m_hWndParent, WS_POPUP | WS_CLIPCHILDREN, 0, 0, 0, 0, 0)) {
		return 0;
	}

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

	// From MFC
	// hide the window before enabling the parent, etc.


	if (IsWindow())
	{
		CSimpleWnd::SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	}

	if (bEnableParent)
		::EnableWindow(m_hWndParent, TRUE);

	::SetActiveWindow(m_hWndParent);

	if (IsWindow())
		CSimpleWnd::DestroyWindow();

	m_bShow = false;
    return m_bSure;
}

void CSetDlg::OnClose()
{
    PostMessage(WM_QUIT);
}

void CSetDlg::OnRadioClick(int id)
{
    ::PostMessage(g_hPlayerWnd,WM_COMMAND, MAKEWPARAM(id,1),(LPARAM)m_hWnd);
}
