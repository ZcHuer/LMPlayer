#include "stdafx.h"
#include "SPopupDlg.h"
//#include "SDownloadDlg.h"

SPopupDlg::SPopupDlg() : SHostWnd(_T("LAYOUT:XML_POPUP"))
{
    m_bLayoutInited = FALSE;
}

SPopupDlg::~SPopupDlg()
{
}

int SPopupDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    SetMsgHandled(FALSE);
    return 0;
}

BOOL SPopupDlg::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    m_bLayoutInited = TRUE;
    
    return 0;
}

void SPopupDlg::Show()
{
    int cx = GetSystemMetrics(SM_CXSCREEN);
    int cy = GetSystemMetrics(SM_CYSCREEN);
    SetWindowPos(NULL, cx - 320, cy - 220, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
}

void SPopupDlg::OnClose()
{
    ShowWindow(SW_HIDE);
}

void SPopupDlg::OnOpenMagr()
{
    ShowWindow(SW_HIDE);
    UINT wParam = (0 << 16) | 1;
    //::PostMessage(SDownloadDlg::m_pThis->m_hWnd, WM_COMMAND, wParam, 0);
}
