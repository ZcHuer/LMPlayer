#pragma once
#include "SCtrlDlg.h"
#include "../Player/Player.h"

typedef char* (*FUNC_FunTV)(char* mac);
typedef void(*FUNC_FunTV_Stop)();

class CPlayerDlg;
struct sFunTVThreadStruct
{
	CPlayerDlg* pPlayer;
	string strUrl;
	HWND hWnd;
};

struct SMiniRect
{
	bool bInited;
	CRect cRect_Mini;
	bool bSetModeOver;		// 判断是否设置模式完成，中间状态时，触发的onsize不处理
};

class CPlayerDlg : public SHostWnd, public IPlayerCallBack
{
public:
    CPlayerDlg();
    ~CPlayerDlg();

	// IPlayerCallBack
	void OnPlayStart(int nDuration);
	void OnPlaying(int nPlayPos, int nDuration);
	void OnPlayEnd();
	void OnFileInvalid();

    void PlayUrl(string sUrl,string strHead,int Resolu);
	void Play(sPlayData* pData);					// 调播放器播放（本地/风行/其他）（风行web版/优酷，都不调用。）
    void ReResolution(string sUrl);
    void SeekPlay(int nPos);
    void SetVolume(int nVolume);//设置音量
    void SetMute(BOOL bMute);
    void EnableBtn(BOOL bLast,BOOL bNext);//上下一个启用禁用
    void DoStop(BOOL bPlayEnd = FALSE);
    void Min2Pause();
    /*控制栏控件*/
    void OnBtnStop();
    void OnBtnPlay();//暂停/继续
    void OnBtnSet();//设置
	void OnBtnOpen();//打开本地
    void OnFullScreen();//全屏
    void OnMiniMode();
	ePlayerWndMode GetPlayerShowMode();
	void SetPlayerShowMode(ePlayerWndMode eMode);
    int IsExistFormat(SStringT strPath);  //0：不可用格式 1：视频格式 2：音频格式 3：字幕格式  4：种子文件
    void DownloadAndPlay(list<string> lstFile, BOOL bPlay = TRUE);

    void SetTopMost(bool bTop);//设置置顶
	bool GetTopMostSet();//获取置顶设置
    void ShowVipTip(BOOL b);
    void PlayAndVip(BOOL isPlay,BOOL isVip);

    static BOOL AddMenuItem(pugi::xml_node xmlMenu, int nParentID, SStringT strValue, int nID);
    void ShowAsNoTop();	
	bool PtInPlayer(CPoint pt);
	eLocalFileType GetLocalFileType();
	void SetLocalFileType(eLocalFileType eT);
	void GetFunTVPlayAdress(char* pUrl, HWND hWnd);
	void PlayFucTV(char* pFsp, char* pAdress);

	// 独立控制栏里的浮动栏/列表开关/独立控制栏/收藏按钮
	void ShowCtrlWnd(BOOL bShow = TRUE);
	void ShowFlyHead(BOOL bShow = TRUE);
	void ShowRightTig(BOOL bShow = TRUE);
	// 迷你和全屏下独立控制栏显示隐藏依托浮动栏，标准模式需要自己隐藏
	void HideAloneCtrlBar();

	// 播放状态
	void SetPlayStatus(ePlayStatus eStatus);
	static ePlayStatus GetPlayStatus();
	void ReSetBtnStatus();

	bool IsMiniShow();
	void FunTv_PreGet(sPlayData* pData);
	void FunTv_AfterGet(char* pFsp, char* pAdress);

	void CreatePlayer();
	void ReleasePlayer();

	void RePlayerShow(BOOL b);	//重置播放显示

	void OnOpenfile(int nWnd);
	void OnOpenfloder(int nWnd);
	void AddOtherFile(wstring strSource);

protected:
    //void ShowAudio(BOOL bShow);
	void SetSliderMax();
    void SetShowTime(int nPos, int nDuration);//设置显示时间
    void SetSliderPos(BOOL bReset = FALSE);
    void ShowVolume(BOOL bShow);
    void DoMute();
    void DoPause();
    void DoFullScreen();
    void Forward(int nSpeed);//快进
    void Rewind(int nSpeed);//快退    
    void SetScreenRatio(int nID, BOOL b = FALSE);//设置屏幕比例
    void OnAddSubtitle(int nWnd);//字幕
    void OnHotkey(int nID);//快捷键
    void UpVolume(int nPos=1);//音量加
    void DownVolume(int nPos = 1);//音量减      
    void WhenStartPlay();
    void UpdateResolu(int nResolu);	
	
    /*系统消息处理*/
    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
    void OnDropFiles(HDROP hDropInfo);// 文件拖拽
    void OnSize(UINT nType, CSize size);
    void OnTimer(UINT_PTR nIDEvent);
    void OnCommand(UINT uNotifyCode, int nID, HWND wndCtl);//菜单响应函数
    void OnRButtonDown(UINT nFlags, CPoint point);//右键消息
    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);//键盘消息
    void OnMouseMove(UINT nFlags, CPoint point);
    void OnMove(CPoint ptPos);
    void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
    void OnLButtonDblClk(UINT nFlags, CPoint point);
    BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    void OnMouseLeave();
    void OnClose();
    void OnMaximize();
    void OnRestore();
    void OnMinimize();
	void ShowLoading(BOOL bShow);
    void OnBtnResolution();//分辨率
	void OnBtnLast();//上一集
	void OnBtnNext();//下一集
	
    /*自定义控件*/
    bool SliderEvent(EventArgs* pEvt);//进度条事件
    void OnDefinitionID(int nID);
    void OnBtnVolume();//声音按钮
	void OnNCXButtonDown(int fwButton, short nHittest, CPoint ptPos);
    /*自定义消息*/
    LRESULT WndPro(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT PlayEndPro(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT ReCheckTop(UINT uMsg, WPARAM wParam, LPARAM lParam);

    //soui消息
    EVENT_MAP_BEGIN()
        EVENT_ID_COMMAND_RANGE(881, 883, OnDefinitionID)
        EVENT_NAME_COMMAND(L"btn_volume", OnBtnVolume)
        EVENT_NAME_COMMAND(L"btn_close", OnClose)
        EVENT_NAME_COMMAND(L"btn_min", OnMinimize)        
        EVENT_NAME_COMMAND(L"btn_stop", OnBtnStop)
        EVENT_NAME_COMMAND(L"btn_play", OnBtnPlay)
        EVENT_NAME_COMMAND(L"btn_last", OnBtnLast)
        EVENT_NAME_COMMAND(L"btn_next", OnBtnNext)
        EVENT_NAME_COMMAND(L"btn_set", OnBtnSet)
		EVENT_NAME_COMMAND(L"btn_open", OnBtnOpen)
        EVENT_NAME_COMMAND(L"btn_xiaochuang", OnMiniMode)
        EVENT_NAME_COMMAND(L"btn_fullscreen", OnFullScreen)
        EVENT_NAME_COMMAND(L"btn_resolution", OnBtnResolution)
        EVENT_MAP_END()

        //HostWnd真实窗口消息处理
        BEGIN_MSG_MAP_EX(CPlayerDlg)
		MSG_WM_NCXBUTTONDOWN(OnNCXButtonDown)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_CLOSE(OnClose)
        MSG_WM_DROPFILES(OnDropFiles)
        MSG_WM_TIMER(OnTimer)
        MSG_WM_SIZE(OnSize)
        MSG_WM_COMMAND(OnCommand)
        MSG_WM_RBUTTONDOWN(OnRButtonDown)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
        MSG_WM_KEYDOWN(OnKeyDown)
        MSG_WM_SYSKEYDOWN(OnKeyDown)
        MSG_WM_MOUSEMOVE(OnMouseMove)
        //MSG_WM_MOUSELEAVE(OnMouseLeave)
        MSG_WM_MOUSEWHEEL(OnMouseWheel)
        MSG_WM_MOVE(OnMove)
		//MSG_WM_SYSCOMMAND(OnSysCommand)
		//MSG_WM_ACTIVATE(OnActivate)

        MSG_WM_WINDOWPOSCHANGING(OnWindowPosChanging)
        MESSAGE_HANDLER_EX(UM_RECHECK_TOP, ReCheckTop)
        MESSAGE_HANDLER_EX(UM_FLASH_PLAY_END, PlayEndPro)
        //MESSAGE_HANDLER_EX(UM_ENABLE_STOP, EnableBtnPro)
		MESSAGE_HANDLER_EX(UM_AI_PLAY, WndPro)
		MESSAGE_HANDLER_EX(UM_AI_PAUSE, WndPro)
        MESSAGE_HANDLER_EX(UM_PLAY_PAUSE, WndPro)
		MESSAGE_HANDLER_EX(UM_PLAY_STOP, WndPro)
        MESSAGE_HANDLER_EX(UM_SET_PLAYPOS, WndPro)
        MESSAGE_HANDLER_EX(UM_BTN_SET, WndPro)
        MESSAGE_HANDLER_EX(UM_SET_FULLSCREEN, WndPro)
        MESSAGE_HANDLER_EX(UM_MINI_MODE, WndPro)
        MESSAGE_HANDLER_EX(UM_SHOW_VOLUME,WndPro)
        MESSAGE_HANDLER_EX(UM_VLUME_POS,WndPro)
        MESSAGE_HANDLER_EX(UM_PLAY_ERROR,WndPro)
        MESSAGE_HANDLER_EX(UM_MINI_MAX,WndPro)
        MESSAGE_HANDLER_EX(UM_MINI_MIN,WndPro)
		MESSAGE_HANDLER_EX(UM_PLAY_FUNTV, WndPro)		
        CHAIN_MSG_MAP(SHostWnd)
        REFLECT_NOTIFICATIONS_EX()
    END_MSG_MAP()
private:
    BOOL m_bLayoutInited;
    SFlashCtrl* m_pFlash;
    WINDOWPLACEMENT m_OldWndPlacement; // 记录窗口原始属性
    BOOL m_bFirstStart;//初始播放
    BOOL m_bTigShow;
    BOOL m_bClickTimer;
	BOOL m_bMoveFlag;
    vector<int> m_vecSubCmdID;
    BOOL m_bGifLoading;
    BOOL m_bSetChannel;
    BOOL m_bSetOpen;
    BOOL m_bShowLink;//是否显示会员充值提示
    __int64 m_llResolutionTime;//分辨率切换时已播放时间
    BOOL m_bResolution;
    int m_nTotalTeleplay;//总剧集数
    string m_strIsVip;
    BOOL m_bMiniShow;
	SMiniRect m_sMiniRect;
	bool m_bPlayerCreated;
	CPlayer m_CPlayer;
	
	int m_nIncrement;
	ePlayerWndMode m_eShowMode;
	ePlayerWndMode m_eShowMode_Old;
	bool m_bPlayingTopflag;
	eLocalFileType m_eFileType;
	// 风行
	HANDLE	m_hThread_GetFunTVAdress;
	HMODULE m_hMod_FunTV;
	FUNC_FunTV pFunTVAdress;
	FUNC_FunTV_Stop pFunTV_Stop;
	
	bool m_bFunTvHaveGet;
	sPlayData* m_pFirstData;
	sPlayData* m_pSecondData;
	
public:
	SCtrlDlg* m_dlgCtrl;
    static CPlayerDlg* m_pThis;
    int m_nVolume;
    BOOL m_bMute;//是否静音
    BOOL m_bShowWeb;
    int m_nMiguPlayID;//咪咕当前播放剧集
    int m_nResolu; //1:标清 2:高清 3:超清    
    BOOL m_bStartPlay;
    string m_sMd5;
    BOOL m_bNoMark;
	BOOL m_bShowRight;
	sPlayData m_sPlayData;
	// 播放状态
	static ePlayStatus m_ePlayerStatus;
};

