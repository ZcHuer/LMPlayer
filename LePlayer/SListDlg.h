#pragma once
class SListDlg : public SHostWnd
{
public:
    SListDlg();
    ~SListDlg();

    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
    void InsertList(sPlayData* pData, BOOL bPlay=FALSE);
    void StopPlay(BOOL bPlayEnd = FALSE);
    BOOL PlayNext();
    BOOL PlayLast();
    void DoClearList();
	void GetNumAndTotal(int& nNum, int&nTotal);
protected:
    void ReListWnd();
	void UpdateListItemTimer();
    void ItemList(TiXmlDocument & doc, string sPos,string sName,
        int nTime,
        string sIndex,BOOL bError);
    string Time2Str(int nTime);
    void SelPaly();
    void DelItem();
    void ClearInvalid();
    //自定义消息
    void OnItemCommand(int nID);
    void OnBtnAdd();
    void OnBtnClear();
    void OnBtnSx();
    //系统消息处理
    void OnTimer(UINT_PTR nIDEvent);
    void OnRButtonDown(UINT nFlags, CPoint point);
    void OnCommand(UINT uNotifyCode, int nID, HWND wndCtl);
    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);//键盘消息
    //自定义消息处理
    LRESULT WndPro(UINT uMsg, WPARAM wParam, LPARAM lParam);
    //soui消息
    EVENT_MAP_BEGIN()
        EVENT_NAME_COMMAND(L"btn_add", OnBtnAdd)
        EVENT_NAME_COMMAND(L"btn_clear", OnBtnClear)
        EVENT_NAME_COMMAND(L"btn_sx", OnBtnSx)
        EVENT_ID_COMMAND_RANGE(1,10000,OnItemCommand)
    EVENT_MAP_END()

    //HostWnd真实窗口消息处理
    BEGIN_MSG_MAP_EX(SListDlg)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_TIMER(OnTimer)
        MSG_WM_RBUTTONDOWN(OnRButtonDown)
        MSG_WM_COMMAND(OnCommand)
        MSG_WM_KEYDOWN(OnKeyDown)
        MSG_WM_SYSKEYDOWN(OnKeyDown)
        MESSAGE_HANDLER_EX(UM_UPDATE_SKIN, WndPro)
        MESSAGE_HANDLER_EX(UM_LIST_RBUTTONDOWN, WndPro)
        MESSAGE_HANDLER_EX(UM_REDRAW_LISTWND,WndPro)
		MESSAGE_HANDLER_EX(UM_REDRAW_LISTWND_NOSORT, WndPro)
        CHAIN_MSG_MAP(SHostWnd)
        REFLECT_NOTIFICATIONS_EX()
    END_MSG_MAP()

private:    
    int m_nSelID;
    string m_sPlayMd5;//当前正在播放文件的MD5
    int m_nMaxID;
	int m_status;
};

