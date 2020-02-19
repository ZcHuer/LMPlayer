#pragma once
class CefMsgDlg : public SHostWnd
{
public:
	CefMsgDlg();
	~CefMsgDlg();
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	void SetParent(HWND parent);
	BOOL DoModel(BOOL b);
protected:
	void OnClose();
	void OnRadioClick(int id);
	//soui消息
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_close", OnClose)
		EVENT_ID_COMMAND_RANGE(SCREEN_NORMAL, 100, OnRadioClick)
		EVENT_MAP_END()

		//HostWnd真实窗口消息处理
		BEGIN_MSG_MAP_EX(CefMsgDlg)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_INITDIALOG(OnInitDialog)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
		END_MSG_MAP()

public:
	static bool m_bShow;
private:
	BOOL        m_bLayoutInited;
	HWND        m_hWndParent;
	BOOL        m_bSure;
	BOOL        m_bEnable;
};

