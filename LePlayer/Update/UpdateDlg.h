#pragma once
#include <vector>
#include "..\Update\Update.h"

class SUpdateDlg : public SHostWnd
{
public:
	SUpdateDlg(string appLoadUrl, string versionName);
    ~SUpdateDlg();

    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);	
    void OnClose();
    void OnRetry();
    void OnCancel();
	bool IsSuccessed();
	BOOL DoModel();
	void ShowError();

	LRESULT WndPro(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    LRESULT OnWndPro(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL bHandled);
	void OnTimer(UINT_PTR nIDEvent);
    //soui消息
    EVENT_MAP_BEGIN()
        EVENT_NAME_COMMAND(L"btn_close", OnClose)
        EVENT_NAME_COMMAND(L"btn_sure", OnRetry)
    EVENT_MAP_END()

    //HostWnd真实窗口消息处理
    BEGIN_MSG_MAP_EX(SUpdateDlg)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_CLOSE(OnClose)
		MESSAGE_HANDLER_EX(eUPDATE_Progress, WndPro)		
		MESSAGE_HANDLER_EX(eUPDATE_Success, WndPro)
		MESSAGE_HANDLER_EX(eUPDATE_Error_Download, WndPro)
		MESSAGE_HANDLER_EX(eUPDATE_Error_Unzip, WndPro)
		MESSAGE_HANDLER_EX(eUPDATE_Error_File, WndPro)
        CHAIN_MSG_MAP(SHostWnd)
        REFLECT_NOTIFICATIONS_EX()
    END_MSG_MAP()
private:
    SProgress* m_pProgress;
    SStatic* m_pProNum;
	SStatic* m_stTipText;
	SImageWnd* m_iImage;
    int m_nPage;
    BOOL m_bNoUpdate;//取消升级
	std::vector<std::wstring> m_vecTipText;
	int m_nIndex;
	HWND m_hWndParent;
	int m_NetErrorCount;
	bool m_bSuccessed;
	CUpdate m_cUpdate;
	string m_appLoadUrl;
	string m_versionName;
};

