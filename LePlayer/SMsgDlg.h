#pragma once
class SMsgDlg : public SHostWnd
{
public:
    SMsgDlg();
    ~SMsgDlg();

    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);

    BOOL DoModel(int nCode=0);
protected:
    void OnClose();
    //soui消息
    EVENT_MAP_BEGIN()
        EVENT_NAME_COMMAND(L"btn_close", OnClose)
        EVENT_NAME_COMMAND(L"btn_sure", OnClose)
    EVENT_MAP_END()

    //HostWnd真实窗口消息处理
    BEGIN_MSG_MAP_EX(SMsgDlg)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_INITDIALOG(OnInitDialog)
        CHAIN_MSG_MAP(SHostWnd)
        REFLECT_NOTIFICATIONS_EX()
    END_MSG_MAP()

private:
    BOOL        m_bLayoutInited;
    HWND        m_hWndParent;
    int         m_nCode;
};

