#pragma once

class CNewVerDlg : public SHostWnd
{
public:
	CNewVerDlg();
	~CNewVerDlg();
	
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	BOOL DoModel();
	void OnOK();
	void OnClose();

protected:
   
	LRESULT OnWndPro(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL bHandled);
	//soui消息
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_close", OnClose)
        EVENT_NAME_COMMAND(L"btn_cancel", OnOK)
    EVENT_MAP_END()

	//HostWnd真实窗口消息处理
	BEGIN_MSG_MAP_EX(CNewVerDlg)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()

private:
	HWND m_hWndParent;
};
