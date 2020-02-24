#include "stdafx.h"
#include "AboutDlg.h"
#include "MainDlg.h"


bool CAboutDlg::m_bShow = false;
CAboutDlg::CAboutDlg():SHostWnd(_T("LAYOUT:XML_ABOUTWND"))
{
    m_bLayoutInited = FALSE;
    m_hWndParent = NULL;
    m_bSure = FALSE;
    m_lOldTime = 0;
}


CAboutDlg::~CAboutDlg()
{
}

int CAboutDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    SetMsgHandled(FALSE);
    return 0;
}

BOOL CAboutDlg::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    m_bLayoutInited = TRUE;
    SWindow* pWnd = FindChildByName(L"version");
    if (pWnd)
    {
		wstring wstr;
		LeTools::GetVersion(eLmplayer, wstr);
        SStringT strVersion = L"°æ±¾ºÅ£º";
        strVersion.Append(wstr.c_str());
        pWnd->SetWindowTextW(strVersion);
    }
    m_lOldTime = time(0);
    return 0;
}	

BOOL CAboutDlg::DoModel()
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

void CAboutDlg::OnClose()
{
    PostMessage(WM_QUIT);
}

void CAboutDlg::OnLnkYinsi()
{
	ShellExecute(NULL, L"open", L"iexplore.exe", L"http://sta.vgs.lenovo.com.cn/privacy2.html", 0, SW_SHOW);
}

void CAboutDlg::OnLnkKaiyuan()
{
	ShellExecute(NULL, L"open", L"iexplore.exe", L"http://sta.vgs.lenovo.com.cn/notice.html", 0, SW_SHOW);
}

