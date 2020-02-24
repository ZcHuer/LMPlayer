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
	void OnLnkYinsi();//��˽Э��
	void OnLnkKaiyuan();//��Դ������

    //soui��Ϣ
    EVENT_MAP_BEGIN()
        EVENT_NAME_COMMAND(L"btn_close", OnClose)
		EVENT_NAME_COMMAND(L"lnk_yinsi", OnLnkYinsi)
		EVENT_NAME_COMMAND(L"lnk_kaiyuan", OnLnkKaiyuan)
        EVENT_MAP_END()

    //HostWnd��ʵ������Ϣ����
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

