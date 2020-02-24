#pragma once
class CAboutDlg : public SHostWnd
{
public:
    CAboutDlg();
    ~CAboutDlg();

    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);

    BOOL DoModel();
protected:
    void OnClose();
	void OnLnkYinsi();//隐私协议
	void OnLnkKaiyuan();//开源组件许可

    //soui消息
    EVENT_MAP_BEGIN()
        EVENT_NAME_COMMAND(L"btn_close", OnClose)
		EVENT_NAME_COMMAND(L"lnk_yinsi", OnLnkYinsi)
		EVENT_NAME_COMMAND(L"lnk_kaiyuan", OnLnkKaiyuan)
        EVENT_MAP_END()

    //HostWnd真实窗口消息处理
    BEGIN_MSG_MAP_EX(CTipDlg)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
        CHAIN_MSG_MAP(SHostWnd)
        REFLECT_NOTIFICATIONS_EX()
        END_MSG_MAP()
public:
	static bool m_bShow;

private:
    BOOL					m_bLayoutInited;
    HWND					m_hWndParent;
    BOOL					m_bSure;
    long m_lOldTime;	
};

