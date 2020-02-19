#pragma once
class SPopupDlg : public SHostWnd
{
public:
    SPopupDlg();
    ~SPopupDlg();

    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
    void Show();
protected:
    void OnClose();
    void OnOpenMagr();
    //soui��Ϣ
    EVENT_MAP_BEGIN()
        EVENT_NAME_COMMAND(L"btn_close", OnClose)
        EVENT_NAME_COMMAND(L"btn_magr", OnOpenMagr)
    EVENT_MAP_END()

    //HostWnd��ʵ������Ϣ����
    BEGIN_MSG_MAP_EX(SPopupDlg)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_INITDIALOG(OnInitDialog)
        CHAIN_MSG_MAP(SHostWnd)
        REFLECT_NOTIFICATIONS_EX()
    END_MSG_MAP()

private:
    BOOL    m_bLayoutInited;
};

