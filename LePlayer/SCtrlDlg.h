#pragma once
#include "../Player/Player.h"
class SCtrlDlg : public SHostWnd
{
public:
    SCtrlDlg();
    ~SCtrlDlg();

    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
    void ShowCtrl(BOOL bShow);
    void ShowHead(BOOL bShow);
    void SetHead(SStringT strTitle);
    //void ShowFavorites(BOOL bShow);
    void ShowResol(BOOL bShow);
    void ShowClose(BOOL bShow);
	void SetPlayerShowMode(ePlayerWndMode eMode);
    void ShowTig(BOOL bShow);
    void ShowVolume(BOOL bSlient,int nVolume);
    void ReBtnRect(SStringT strName,SStringT strPos);
    void ShowXuanji(BOOL bShow);
    void SetResolu(int nResolu);
    void ShowVip(BOOL bShow);
    void OnBtnResolution();//清晰度
	void SetLocalFileType(eLocalFileType eT);
	void ReSetBtnStatus(ePlayStatus eStatus, sPlayData sData, int nN, int nT);

protected:
    void OnBtnVolume();//声音按钮
    void OnBtnLast();//上一集
    void OnBtnNext();//下一集
    void OnBtnPause();//暂停/继续
    void OnBtnFullScreen();//全屏/还原
    void OnBtnMiniMode();//窗口迷你模式
    void OnBtnSet();//小设置窗口
    void OnLnkBtn();
    void OnClose();    
    void OnMinimize();    
    void OnBtnTig();//收起按钮
    void SetShowTime(int nPos, int nDuration);//设置显示时间
    bool SliderEvent(EventArgs* pEvt);//进度条事件
	bool FlyState(EventArgs* pEvt);//进度条事件
    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);//键盘消息
    LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void OnMouseLeave();
    void OnMove(CPoint ptPos);
	void OnBtnStop();
    //soui消息
    EVENT_MAP_BEGIN()
        EVENT_NAME_COMMAND(L"btn_volume", OnBtnVolume)
        EVENT_NAME_COMMAND(L"btn_last", OnBtnLast)
		EVENT_NAME_COMMAND(L"btn_stop", OnBtnStop)
        EVENT_NAME_COMMAND(L"btn_next", OnBtnNext)
        EVENT_NAME_COMMAND(L"btn_play", OnBtnPause)
        EVENT_NAME_COMMAND(L"btn_fullscreen", OnBtnFullScreen)
        EVENT_NAME_COMMAND(L"btn_nomal", OnBtnMiniMode)
        EVENT_NAME_COMMAND(L"btn_set", OnBtnSet)
        EVENT_NAME_COMMAND(L"btn_resolution", OnBtnResolution)
        EVENT_NAME_COMMAND(L"btn_tig", OnBtnTig)
        EVENT_NAME_COMMAND(L"lnk_huiyuan", OnLnkBtn)
        EVENT_NAME_COMMAND(L"btn_close", OnClose)
        EVENT_NAME_COMMAND(L"btn_min", OnMinimize)
    EVENT_MAP_END()

    //HostWnd真实窗口消息处理
    BEGIN_MSG_MAP_EX(SCtrlDlg)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_KEYDOWN(OnKeyDown)
        MSG_WM_SYSKEYDOWN(OnKeyDown)
		MSG_WM_MOUSEWHEEL(OnMouseWheel)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
        MSG_WM_MOVE(OnMove)
        MESSAGE_HANDLER_EX(UM_PLAY_POS,WindowProc)
        MESSAGE_HANDLER_EX(UM_SKIN_FAVORITE,WindowProc)
        MESSAGE_HANDLER_EX(UM_HIDE_RESOLU,WindowProc)
        CHAIN_MSG_MAP(SHostWnd)
        REFLECT_NOTIFICATIONS_EX()
    END_MSG_MAP()
private:
    BOOL m_bLayoutInited;
    __int64 m_llTotalTime;
    __int64 m_llNowTime;
    BOOL m_bShowList;
    int m_nResolu;

	BOOL m_bShowFly;
	// 单位：秒
	int m_nDuration;
	int m_nCurPos;
	ePlayerWndMode m_eShowMode;
	eLocalFileType m_eFileType;
	BOOL m_bCaptureFly;
	bool m_bLBtnDown;
};

