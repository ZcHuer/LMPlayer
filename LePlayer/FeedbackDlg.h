#pragma once
#include "core\SHostWnd.h"

struct sFeedBackInfo
{
	string m_QQCode;
	string m_QQUrl;
	string m_QQName;
};
class CFeedbackDlg :
	public SHostWnd
{
public:
	CFeedbackDlg();
	~CFeedbackDlg();
public:
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	BOOL DoModel();
	void OnClose();
	void OnFeedConcel();

	void OnFeed();
	void OnLnkCustomer();
	void OnLnkQQ();
	void OnQQLink(int nID);
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_close", OnClose)
		EVENT_NAME_COMMAND(L"btn_FeedConcel", OnFeedConcel)
		EVENT_NAME_COMMAND(L"btn_Feed", OnFeed)
		EVENT_NAME_COMMAND(L"lnk_Customer", OnLnkCustomer)
		EVENT_ID_COMMAND_RANGE(2000, 2010, OnQQLink)
		EVENT_NAME_COMMAND(L"lnk_QQ", OnLnkQQ)
	EVENT_MAP_END()
		//HostWnd真实窗口消息处理
	BEGIN_MSG_MAP_EX(CFeedbackDlg)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_TIMER(OnTimer)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()



private:
	HWND					m_hWndParent;
	BOOL					m_bSure;

public:
	int ReportLog(SStringT ReportData);
	SStringT GetCheckProblem();
	void OnTimer(UINT_PTR nIDEvent);
	void ReLoadInfor();
	vector<sFeedBackInfo> m_qqVector;
	/*string m_QQCode;
	string m_QQUrl;
	string m_QQName;*/
	CAes_encryptor m_aesEncry;
	string m_CustomPhone;
	
};

