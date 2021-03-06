#pragma once

class CUserChoiceDlg : public SHostWnd
{
public:
	CUserChoiceDlg(string strTitle, string strTips, string strBtnTips, bool bCanClose = false);
	~CUserChoiceDlg();
	
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	BOOL DoModel();
	void OnOK();
	void OnClose();
	bool IsBtnOk();

protected:   
	LRESULT OnWndPro(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL bHandled);
	//soui消息
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_close", OnClose)
        EVENT_NAME_COMMAND(L"btn_ok", OnOK)
    EVENT_MAP_END()

	//HostWnd真实窗口消息处理
	BEGIN_MSG_MAP_EX(CUserChoiceDlg)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_INITDIALOG(OnInitDialog)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()

private:
	HWND m_hWndParent;
	bool m_bIsBtnOk;
	string m_strTitle;
	string m_strTips;
	string m_strBtnTips;
	bool m_bCanClose;
};
