#include "stdafx.h"
#include "ShortcutHint.h"
#include "MainDlg.h"
bool CShortcutHintDlg::m_bNoHint = false;
bool CShortcutHintDlg::m_bCreateShortcut = true;
CShortcutHintDlg::CShortcutHintDlg() : SHostWnd(_T("LAYOUT:XML_SHORTCUTHINT"))
{
	m_hWndParent = NULL;
}

CShortcutHintDlg::~CShortcutHintDlg()
{
}

BOOL CShortcutHintDlg::DoModel()
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

int CShortcutHintDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetMsgHandled(false);
	return 0;
}

BOOL CShortcutHintDlg::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{	
	return 0;
}

void CShortcutHintDlg::OnCreateShortcut()
{
	SWindow* pWnd = FindChildByID(2000);
	if (pWnd)
		m_bNoHint = pWnd->IsChecked();

	m_bCreateShortcut = true;
	PostMessage(WM_QUIT);
}

void CShortcutHintDlg::OnCancel()
{
	SWindow* pWnd = FindChildByID(2000);
	if (pWnd)
		m_bNoHint = pWnd->IsChecked();

	m_bCreateShortcut = false;
	PostMessage(WM_QUIT);
}
