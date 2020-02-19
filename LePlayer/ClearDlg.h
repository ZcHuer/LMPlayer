#pragma once

class CClearDlg :
    public SHostWnd
{
public:
    CClearDlg();
    ~CClearDlg();
    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);

    BOOL DoModel(int nPage=1);
    BOOL IsNoprompt();
protected:
    void OnClose();
    void OnSure();
    void OnCancel();

    //soui��Ϣ
    EVENT_MAP_BEGIN()
        EVENT_NAME_COMMAND(L"btn_close", OnClose)
        EVENT_NAME_COMMAND(L"btn_cancel", OnCancel)
        EVENT_NAME_COMMAND(L"btn_sure", OnSure)
    EVENT_MAP_END()

    //HostWnd��ʵ������Ϣ����
    BEGIN_MSG_MAP_EX(CClearDlg)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_INITDIALOG(OnInitDialog)
        CHAIN_MSG_MAP(SHostWnd)
        REFLECT_NOTIFICATIONS_EX()
    END_MSG_MAP()

private:
    BOOL    m_bLayoutInited;
    HWND    m_hWndParent;
    BOOL    m_bSure;
    BOOL    m_bNoprompt;
   // BOOL    m_bClearBtn;
    int m_nPage;
};

