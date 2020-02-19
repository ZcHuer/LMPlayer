#pragma once
class CMainSetDlg :
    public SHostWnd
{
public:
    CMainSetDlg();
    ~CMainSetDlg();

    BOOL DoModel();
    void SetFileExt();
protected:
    void Init();
    void InitHotKey(int nID);
    bool OnFocusEvent(EventArgs* pEvt);
    bool OnKillFocusEvent(EventArgs* pEvt);

    bool OnEditNotify(EventArgs* pEvt);

    void SetPercentText(int nPos);
    //系统消息处理
    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);//键盘消息

    //控件处理
    void OnClose();
    void OnMin();
    void OnSure();
    void OnUse();
    void OnCancel();
    void OnDefaults();//恢复默认
    void OnPath();
    void OnClearList();
	void OnAllCheck();
	void OnNoCheck();
	void OnDefaultCheck();
    void Tongji();

	void ExtCheck(BOOL bCheck);

    //文件关联
    void OnWindowMedia();  //WindowsMedia媒体
    void OnRealMedia();    //Real媒体文件
    void OnQuickTimeMedia();//QuickTime媒体文件
    void OnMpegMedia();     //MPEG1/2媒体
    void OnMpeg4Media();    //MPEG4媒体
    void On3gppMedia();     //3GPP媒体
    void OnFlashMedia();    //flash媒体
    void OnVcdMedia();      //VCD/DVD视频
    void OnOtherMedia();    //其他视频格式
    void OnAudioMedia();    //音频文件

    //自定义消息处理
    LRESULT ChangePage(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    void Setting(int nID);
    void UpdateRadio();
    void SetUseEnable(BOOL b);
    bool OnCboxFocusEvent(EventArgs* pEvt);
    bool SliderEvent(EventArgs* pEvt);			// 进度条事件
    void OnTimer(UINT_PTR nIDEvent);
    //soui消息
    EVENT_MAP_BEGIN()
        EVENT_NAME_COMMAND(L"btn_close", OnClose)
        EVENT_NAME_COMMAND(L"btn_min", OnMin)
        EVENT_NAME_COMMAND(L"btn_sure",OnSure)
        EVENT_NAME_COMMAND(L"btn_use", OnUse)
        EVENT_NAME_COMMAND(L"btn_cancel", OnCancel)
        EVENT_NAME_COMMAND(L"btn_defaults",OnDefaults)
        EVENT_NAME_COMMAND(L"btn_path",OnPath)
        EVENT_NAME_COMMAND(L"btn_clearlist",OnClearList)
        EVENT_ID_COMMAND_RANGE(2000, 5999, Setting)
        EVENT_NAME_COMMAND(L"WindowsMedia", OnWindowMedia)
        EVENT_NAME_COMMAND(L"RealMedia", OnRealMedia)
        EVENT_NAME_COMMAND(L"QuickTime", OnQuickTimeMedia)
        EVENT_NAME_COMMAND(L"mpeg", OnMpegMedia)
        EVENT_NAME_COMMAND(L"mpeg4", OnMpeg4Media)
        EVENT_NAME_COMMAND(L"3gpp", On3gppMedia)
        EVENT_NAME_COMMAND(L"flash", OnFlashMedia)
        EVENT_NAME_COMMAND(L"vcd/dvd", OnVcdMedia)
        EVENT_NAME_COMMAND(L"other", OnOtherMedia)
        EVENT_NAME_COMMAND(L"audio", OnAudioMedia)
		EVENT_NAME_COMMAND(L"all_check", OnAllCheck)
		EVENT_NAME_COMMAND(L"no_check", OnNoCheck)
		EVENT_NAME_COMMAND(L"default_check", OnDefaultCheck)
        EVENT_MAP_END()

    //HostWnd真实窗口消息处理
    BEGIN_MSG_MAP_EX(CMainSetDlg)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_TIMER(OnTimer)
		MSG_WM_CLOSE(OnClose)
        MSG_WM_KEYDOWN(OnKeyDown)
        MSG_WM_SYSKEYDOWN(OnKeyDown)
        MESSAGE_HANDLER_EX(UM_SETTING_PAGE, ChangePage)
        CHAIN_MSG_MAP(SHostWnd)
        REFLECT_NOTIFICATIONS_EX()
        END_MSG_MAP()

private:
    BOOL        m_bLayoutInited;
    HWND        m_hWndParent;
    BOOL        m_bSure;
    int         m_nPage;
    map <int, _HotKey> m_mapHotkey;

    SEdit* m_pFocusEdit;
    std::list<std::pair<std::tstring, std::tstring>> software_param;
public:
    static HWND m_hMsgWnd;
	static bool m_bShow;
};

