#pragma once
class CInstallingGif : public SHostWnd
{
public:
	CInstallingGif();
	~CInstallingGif();

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);

	BOOL DoModel();
protected:

	//soui��Ϣ
	EVENT_MAP_BEGIN()
		EVENT_MAP_END()

		//HostWnd��ʵ������Ϣ����
		BEGIN_MSG_MAP_EX(CTipDlg)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_INITDIALOG(OnInitDialog)
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

