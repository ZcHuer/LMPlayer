#include "../stdafx.h"
#include "UserChoiceDlg.h"
#include "UpdateDlg.h"
#include "../MainDlg.h"
	
CUserChoiceDlg::CUserChoiceDlg(string strTitle, string strTips, string strBtnTips, bool bCanClose /*= false*/) : SHostWnd(_T("LAYOUT:XML_USERCHOISE"))
{
	m_hWndParent = NULL;
	m_bIsBtnOk = false;
	m_strTitle = strTitle;
	m_strTips = strTips;
	m_strBtnTips = strBtnTips;
	m_bCanClose = bCanClose;
}

CUserChoiceDlg::~CUserChoiceDlg()
{
}

int CUserChoiceDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetMsgHandled(FALSE);
	return 0;
}

BOOL CUserChoiceDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	SWindow* pWnd = FindChildByName(L"btn_close");
	if (pWnd)
		pWnd->SetAttribute(L"show", m_bCanClose ? L"1" : L"0");

	pWnd = FindChildByName(L"text_title");
	if (pWnd)
		pWnd->SetWindowTextW(LeTools::s2ws(m_strTitle).c_str());

	pWnd = FindChildByName(L"text_tip");
	if (pWnd)
		pWnd->SetWindowTextW(LeTools::s2ws(m_strTips).c_str());
	
	pWnd = FindChildByName(L"btn_ok");
	if (pWnd)
		pWnd->SetWindowTextW(LeTools::s2ws(m_strBtnTips).c_str());

    ShowWindow(SW_SHOWNORMAL);
	return TRUE;
}

BOOL CUserChoiceDlg::DoModel()
{
	BOOL bEnableParent = FALSE;
	if (!m_hWndParent)
	{
		m_hWndParent = ::GetActiveWindow();
	}
	m_hWndParent = CMainDlg::g_hMainWnd;
	if (m_hWndParent && m_hWndParent != ::GetDesktopWindow() && ::IsWindowEnabled(m_hWndParent))
	{
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

void CUserChoiceDlg::OnOK()
{
	m_bIsBtnOk = true;
	PostMessage(WM_QUIT);
}

void CUserChoiceDlg::OnClose()
{
	m_bIsBtnOk = false;
    PostMessage(WM_QUIT);
}

bool CUserChoiceDlg::IsBtnOk()
{
	return m_bIsBtnOk;
}
