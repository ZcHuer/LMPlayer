// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "PlayerDlg.h"
#include "trayicon\SShellNotifyIcon.h"
#include "SListDlg.h"
#include "CommonDefine.h"

class CMainDlg : public SHostWnd
{
public:
    CMainDlg();
    ~CMainDlg();

	// 命令行处理相关
	void DoCmdLine(wstring wstrCmdLine, eCmdSource eSource = eeCmdSource_Run);
	void HandleCmd_Active(wstring wstr_Active, eCmdSource eSource = eeCmdSource_Run);
	void HandleCmd_Run(wstring wstr_Run, eCmdSource eSource = eeCmdSource_Run);
	void HandleCmd_Play(wstring wstr_Play, eCmdSource eSource = eeCmdSource_Run);
	void HandleCmd_Func(wstring wstr_Func, eCmdSource eSource = eeCmdSource_Run);
	void HandleCmd_OpenUrl(wstring wstr_Url, eCmdSource eSource = eeCmdSource_Run);
	/************************************************************************/
	// 切换播放
	void Play(sPlayData* pPlayData);
	bool HandleHistroy(sPlayData* pPlayData);
	void Native_Play(sPlayData* pPlayData);
	void Native_Stop();	
	sPlayData m_sPlayData;
	/************************************************************************/

    void OnClose();
    void OnMaximize();
    void OnRestore();
    void OnMinimize();
	void Onzhiding();
	void Onbuzhiding();
	void Onminimode();
    void OnSize(UINT nType, CSize size);
    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
    void InitWndSize(); //初始化窗口大小
    void DoInit();//可以异步初始化的相关处理	
	eSourceType ParseSourceType(wstring wstr);
	static void HttpSendPlayInfo(string sData);
	static void HttpDelPlayInfo(string sData);
protected:
    void GetDefalutKey();//获取默认搜索关键字
    void DoUpdate();//升级线程
    void GetMainUrl();
    void InitCreate();//初始化创建
    void ChangeWindowMessageFilterOk();  //处理win7系统消息隔离机制
	void ReSetWnd();	//重置窗口位置和大小
    void TrueClose();
    bool EditPro(EventArgs* pEvt);
    /*自定义控件消息*/
	void OnMenuBtn();
    void OnTagIDCommand(int nID);
    void OnTimer(UINT_PTR nIDEvent);
	void OnLastOne();//播放上一个
	void OnNextOne();//播放下一个

    /*系统消息*/
    void OnMouseMove(UINT nFlags, CPoint point);
    void OnKillFocus(HWND wndFocus);
    void OnDropFiles(HDROP hDropInfo);// 文件拖拽
    void OnCommand(UINT uNotifyCode, int nID, HWND wndCtl);//菜单响应函数
    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);//键盘消息
    void OnRButtonDown(UINT nFlags, CPoint point);//右键消息
    void OnLButtonDown(UINT nFlags, CPoint point);
    LRESULT OnCopyData(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnMove(CPoint ptPos);
    /*自定义消息*/
    LRESULT OnIconNotify(UINT uMsg, WPARAM wParam, LPARAM lParam);//托盘通知消息处理函数
    LRESULT WndPro(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void ShowShortcutHint();
	bool IsDesktopHaveShortcut();
	
	bool IsNewVerReady();
	void RunNewVer();
    //soui消息
	
    EVENT_MAP_BEGIN()
        EVENT_NAME_COMMAND(L"btn_close", OnClose)
        EVENT_NAME_COMMAND(L"btn_min", OnMinimize)
        EVENT_NAME_COMMAND(L"btn_max", OnMaximize)
        EVENT_NAME_COMMAND(L"btn_restore", OnRestore)
		EVENT_NAME_COMMAND(L"btn_set", OnMenuBtn)
		EVENT_NAME_COMMAND(L"btn_zhiding", Onzhiding)
		EVENT_NAME_COMMAND(L"btn_buzhiding", Onbuzhiding)
		EVENT_NAME_COMMAND(L"btn_xiaochuang", Onminimode)
    EVENT_MAP_END()
    //HostWnd真实窗口消息处理
    BEGIN_MSG_MAP_EX(CMainDlg)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_DROPFILES(OnDropFiles)
        MSG_WM_CLOSE(OnClose)
        MSG_WM_SIZE(OnSize)
        MSG_WM_KILLFOCUS(OnKillFocus)
        MSG_WM_SETFOCUS(OnSetFocus)
        MSG_WM_COMMAND(OnCommand)
        MSG_WM_MOUSEMOVE(OnMouseMove)
        MSG_WM_KEYDOWN(OnKeyDown)
		//MSG_WM_SYSCOMMAND(OnSysCommand)
		//MSG_WM_ACTIVATE(OnActivate)
        //MSG_WM_SYSKEYDOWN(OnKeyDown)
        MSG_WM_TIMER(OnTimer)
        MSG_WM_RBUTTONDOWN(OnRButtonDown)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_MOVE(OnMove)
        MESSAGE_HANDLER_EX(WM_COPYDATA, OnCopyData)
        MESSAGE_HANDLER_EX(WM_ICONNOTIFY, OnIconNotify)
        MESSAGE_HANDLER_EX(UM_DEL_SEARCHLIST, WndPro)
        MESSAGE_HANDLER_EX(UM_SEARCHLIST, WndPro)       
        MESSAGE_HANDLER_EX(UM_WEB_MSG, WndPro)       
        MESSAGE_HANDLER_EX(UM_SHOW_LISTWND, WndPro)
        MESSAGE_HANDLER_EX(UM_SETMIMI_WND, WndPro)
        MESSAGE_HANDLER_EX(UM_MIGUI_HIDE, WndPro)     
        MESSAGE_HANDLER_EX(UM_EXT_SET, WndPro)
        MESSAGE_HANDLER_EX(UM_CLEAR_LIST, WndPro)
        MESSAGE_HANDLER_EX(UM_CLOSE, WndPro)
        MESSAGE_HANDLER_EX(UM_NO_EDITCHANGE, WndPro)
        MESSAGE_HANDLER_EX(UM_RESETWND, WndPro)
		MESSAGE_HANDLER_EX(UM_PLAY, WndPro)
		MESSAGE_HANDLER_EX(UM_DETAIL_FUNTV, WndPro)
		MESSAGE_HANDLER_EX(UM_DETAIL_KANKAN, WndPro)	
		MESSAGE_HANDLER_EX(UM_WEB_PLAY, WndPro)
		MESSAGE_HANDLER_EX(UM_PLAY_LAST, WndPro)
		MESSAGE_HANDLER_EX(UM_PLAY_NEXT, WndPro)
		MESSAGE_HANDLER_EX(UM_SHOWMAIN, WndPro)
		MESSAGE_HANDLER_EX(UM_STOP, WndPro)
		MESSAGE_HANDLER_EX(WM_POWERBROADCAST,WndPro)
		MESSAGE_HANDLER_EX(UM_MINI_CLOSE, WndPro)
        CHAIN_MSG_MAP(SHostWnd)
        REFLECT_NOTIFICATIONS_EX()
    END_MSG_MAP()

public:
	static bool ParseActiveParame(SStringT copydate);
	static HWND g_hMainWnd;
	static CAes_encryptor m_aesEncry;
private:
    BOOL m_bLayoutInited;
    CPlayerDlg* m_dlgPlayer;	
    SShellNotifyIcon m_shellNotifyIcon;// 托盘图标
    POINT m_ptScreen;
    BOOL m_bListShow;
    int m_nOldWidth;
    BOOL m_bMiniShow;
	eSourceType m_eSrcType;
	LONGLONG m_lStartTime;
	BOOL m_bRefreshSucess;
	BOOL m_bWebPlay;
};
