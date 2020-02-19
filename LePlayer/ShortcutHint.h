#pragma once
#include "core\SHostWnd.h"

class CShortcutHintDlg : public SHostWnd
{
public:
	CShortcutHintDlg();
	~CShortcutHintDlg();
public:
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	BOOL DoModel();
	void OnCreateShortcut();
	void OnCancel();

	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_exit", OnCreateShortcut)
		EVENT_NAME_COMMAND(L"btn_Cancel", OnCancel)
	EVENT_MAP_END()
		//HostWnd真实窗口消息处理
	BEGIN_MSG_MAP_EX(CFeedbackDlg)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_CLOSE(OnCancel)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()

	static bool m_bNoHint;
	static bool m_bCreateShortcut;

private:
	HWND m_hWndParent;
};

