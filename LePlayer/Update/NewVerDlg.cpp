#include "../stdafx.h"
#include "NewVerDlg.h"
#include "UpdateDlg.h"
#include "../MainDlg.h"
	
CNewVerDlg::CNewVerDlg() : SHostWnd(_T("LAYOUT:XML_NEWVER"))
{
	m_hWndParent = NULL;
}

CNewVerDlg::~CNewVerDlg()
{
}

int CNewVerDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetMsgHandled(FALSE);
	return 0;
}

BOOL CNewVerDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{    
    ShowWindow(SW_SHOWNORMAL);    
	return 0;
}

BOOL CNewVerDlg::DoModel()
{
	BOOL bEnableParent = FALSE;	
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

void CNewVerDlg::OnOK()
{
	PostMessage(WM_QUIT);
}

void CNewVerDlg::OnClose()
{
    PostMessage(WM_QUIT);
}