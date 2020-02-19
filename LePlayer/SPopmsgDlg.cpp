#include "stdafx.h"
#include "SPopmsgDlg.h"
extern HWND g_hWnd;

SPopmsgDlg::SPopmsgDlg() : SHostWnd(_T("LAYOUT:XML_POPMSG"))
{
    m_bLayoutInited = FALSE;
    m_hWndParent = NULL;
    m_nCode = 0;
    m_bNoprompt = FALSE;
}

SPopmsgDlg::~SPopmsgDlg()
{
}

int SPopmsgDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    SetMsgHandled(FALSE);
    return 0;
}

BOOL SPopmsgDlg::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    m_bLayoutInited = TRUE;
    QwBase::SetWindowRgn(m_hWnd);
    return 0;
}

int SPopmsgDlg::DoModel()
{
    if (!m_hWndParent) {
        m_hWndParent = ::GetActiveWindow();
    }
    m_hWndParent = g_hWnd;
    BOOL bEnableParent = FALSE;
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

    SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

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

    return m_nCode;
}

BOOL SPopmsgDlg::IsNoprompt()
{
    return m_bNoprompt;
}

void SPopmsgDlg::OnClose()
{
    PostMessage(WM_QUIT);
}

void SPopmsgDlg::OnOK()
{
    SCheckBox* pCheck = FindChildByName2<SCheckBox>(L"chk_rmb");
    if (pCheck)
    {
        m_bNoprompt = pCheck->IsChecked();
    }
    /*
    if (FindChildByID(111)->IsChecked())
        m_nCode = 1;
    else
        m_nCode = 2;
    */
    m_nCode = 2;
    OnClose();
}

void SPopmsgDlg::OnCannel()
{
    m_nCode = 1;
    OnClose();
}
