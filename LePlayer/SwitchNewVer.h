#pragma once
#include "core\SHostWnd.h"

class SwitchNewVer : public SHostWnd
{
public:
	SwitchNewVer();
	~SwitchNewVer();
public:
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	BOOL DoModel();
	void OnOk();

	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_ok", OnOk)
	EVENT_MAP_END()
		//HostWnd真实窗口消息处理
	BEGIN_MSG_MAP_EX(CFeedbackDlg)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnOk)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()

private:
	HWND m_hWndParent;
};

