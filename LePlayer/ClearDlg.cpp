#include "stdafx.h"
#include "ClearDlg.h"

CClearDlg::CClearDlg():SHostWnd(_T("LAYOUT:XML_CLEARWND"))
{
    m_bLayoutInited = FALSE;
    m_hWndParent = NULL;
    m_bSure = FALSE;
    m_bNoprompt = FALSE;
    //m_bClearBtn = TRUE;
    m_nPage = 0;
}


CClearDlg::~CClearDlg()
{
}

int CClearDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    SetMsgHandled(FALSE);
    return 0;
}

BOOL CClearDlg::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    m_bLayoutInited = TRUE;
	SWindow* pWnd = FindChildByName(L"txt_msg");
	if (pWnd == nullptr) return 0;
    if (m_nPage==0)
        pWnd->SetWindowTextW(L"播放记录删除后将不能恢复，确定要删除当前记录吗？");
    if (m_nPage == 1)
        pWnd->SetWindowTextW(L"清空播放记录后将不能恢复，请确定执行清空操作？");
    if (m_nPage == 2)
        pWnd->SetWindowTextW(L"删除任务后将不再继续下载，确定要执行当前操作吗？");
    if (m_nPage == 3)
        pWnd->SetWindowTextW(L"是否删除已下载的本地文件？");
	//LeTools::SetWindowRgn(m_hWnd);
    return 0;
}

BOOL CClearDlg::DoModel(int nPage)
{
    if (!m_hWndParent) {
        m_hWndParent = ::GetActiveWindow();
    }
    m_nPage = nPage;
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

    return m_bSure;
}

BOOL CClearDlg::IsNoprompt()
{
    return m_bNoprompt; 
}

void CClearDlg::OnClose()
{
    PostMessage(WM_QUIT);
}

void CClearDlg::OnSure()
{
    m_bSure = TRUE;
    OnClose();
}

void CClearDlg::OnCancel()
{
    m_bSure = FALSE;
    OnClose();
}


