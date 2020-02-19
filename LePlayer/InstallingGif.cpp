#include "stdafx.h"
#include "InstallingGif.h"
#include "MainDlg.h"

bool CInstallingGif::m_bShow = false;
CInstallingGif::CInstallingGif() :SHostWnd(_T("LAYOUT:XML_INTALLING"))
{
	m_bLayoutInited = FALSE;
	m_hWndParent = NULL;
	m_bSure = FALSE;
	m_lOldTime = 0;
}

CInstallingGif::~CInstallingGif()
{
}

int CInstallingGif::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetMsgHandled(FALSE);
	return 0;
}

BOOL CInstallingGif::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
	
	return 0;
}

BOOL CInstallingGif::DoModel()
{
	m_bShow = true;
	if (!m_hWndParent) {
		m_hWndParent = ::GetActiveWindow();
	}
	m_hWndParent = CMainDlg::g_hMainWnd;
	BOOL bEnableParent = FALSE;
	if (m_hWndParent && m_hWndParent != ::GetDesktopWindow() && ::IsWindowEnabled(m_hWndParent)) {
		::EnableWindow(m_hWndParent, FALSE);
		bEnableParent = TRUE;
	}

	if (!Create(m_hWndParent, WS_POPUP | WS_CLIPCHILDREN, 0, 0, 0, 0, 0)) {
		return m_bSure;
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

