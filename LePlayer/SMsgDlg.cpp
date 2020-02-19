#include "stdafx.h"
#include "SMsgDlg.h"

SMsgDlg::SMsgDlg() : SHostWnd(_T("LAYOUT:XML_MSG"))
{
    m_bMsgTransparent = FALSE;
    m_hWndParent = NULL;
    m_nCode = 0;
}


SMsgDlg::~SMsgDlg()
{
}

int SMsgDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    SetMsgHandled(FALSE);
    return 0;
}

BOOL SMsgDlg::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    m_bMsgTransparent = TRUE;
    SWindow* pWnd;
    if (m_nCode==0)
    {
        pWnd = FindChildByName(L"torrent");
        if (pWnd)pWnd->SetAttribute(L"show",L"1");
    }
    if (m_nCode == 1)
    {
        pWnd = FindChildByName(L"torrent_error");
        if (pWnd)pWnd->SetAttribute(L"show", L"1");
    }
    if (m_nCode == 2)
    {
        pWnd = FindChildByName(L"task_error");
        if (pWnd)pWnd->SetAttribute(L"show", L"1");
    }
    QwBase::SetWindowRgn(m_hWnd);
    return 0;
}

BOOL SMsgDlg::DoModel(int nCode)
{
    m_nCode = nCode;
    if (!m_hWndParent) {
        m_hWndParent = ::GetActiveWindow();
    }

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

    return 0;
}

void SMsgDlg::OnClose()
{
    PostMessage(WM_QUIT);
}
