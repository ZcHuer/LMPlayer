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
	bool bSetModeOver;		// �ж��Ƿ�����ģʽ��ɣ��м�״̬ʱ��������onsize������
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
	void Play(sPlayData* pData);					// �����������ţ�����/����/������������web��/�ſᣬ�������á���
    void ReResolution(string sUrl);
    void SeekPlay(int nPos);
    void SetVolume(int nVolume);//��������
    void SetMute(BOOL bMute);
    void EnableBtn(BOOL bLast,BOOL bNext);//����һ�����ý���
    void DoStop(BOOL bPlayEnd = FALSE);
    void Min2Pause();
    /*�������ؼ�*/
    void OnBtnStop();
    void OnBtnPlay();//��ͣ/����
    void OnBtnSet();//����
	void OnBtnOpen();//�򿪱���
    void OnFullScreen();//ȫ��
    void OnMiniMode();
	ePlayerWndMode GetPlayerShowMode();
	void SetPlayerShowMode(ePlayerWndMode eMode);
    int IsExistFormat(SStringT strPath);  //0�������ø�ʽ 1����Ƶ��ʽ 2����Ƶ��ʽ 3����Ļ��ʽ  4�������ļ�
    void DownloadAndPlay(list<string> lstFile, BOOL bPlay = TRUE);

    void SetTopMost(bool bTop);//�����ö�
	bool GetTopMostSet();//��ȡ�ö�����
    void ShowVipTip(BOOL b);
    void PlayAndVip(BOOL isPlay,BOOL isVip);

    static BOOL AddMenuItem(pugi::xml_node xmlMenu, int nParentID, SStringT strValue, int nID);
    void ShowAsNoTop();	
	bool PtInPlayer(CPoint pt);
	eLocalFileType GetLocalFileType();
	void SetLocalFileType(eLocalFileType eT);
	void GetFunTVPlayAdress(char* pUrl, HWND hWnd);
	void PlayFucTV(char* pFsp, char* pAdress);

	// ������������ĸ�����/�б���/����������/�ղذ�ť
	void ShowCtrlWnd(BOOL bShow = TRUE);
	void ShowFlyHead(BOOL bShow = TRUE);
	void ShowRightTig(BOOL bShow = TRUE);
	// �����ȫ���¶�����������ʾ�������и���������׼ģʽ��Ҫ�Լ�����
	void HideAloneCtrlBar();

	// ����״̬
	void SetPlayStatus(ePlayStatus eStatus);
	static ePlayStatus GetPlayStatus();
	void ReSetBtnStatus();

	bool IsMiniShow();
	void FunTv_PreGet(sPlayData* pData);
	void FunTv_AfterGet(char* pFsp, char* pAdress);

	void CreatePlayer();
	void ReleasePlayer();

	void RePlayerShow(BOOL b);	//���ò�����ʾ

	void OnOpenfile(int nWnd);
	void OnOpenfloder(int nWnd);
	void AddOtherFile(wstring strSource);

protected:
    //void ShowAudio(BOOL bShow);
	void SetSliderMax();
    void SetShowTime(int nPos, int nDuration);//������ʾʱ��
    void SetSliderPos(BOOL bReset = FALSE);
    void ShowVolume(BOOL bShow);
    void DoMute();
    void DoPause();
    void DoFullScreen();
    void Forward(int nSpeed);//���
    void Rewind(int nSpeed);//����    
    void SetScreenRatio(int nID, BOOL b = FALSE);//������Ļ����
    void OnAddSubtitle(int nWnd);//��Ļ
    void OnHotkey(int nID);//��ݼ�
    void UpVolume(int nPos=1);//������
    void DownVolume(int nPos = 1);//������      
    void WhenStartPlay();
    void UpdateResolu(int nResolu);	
	
    /*ϵͳ��Ϣ����*/
    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
    void OnDropFiles(HDROP hDropInfo);// �ļ���ק
    void OnSize(UINT nType, CSize size);
    void OnTimer(UINT_PTR nIDEvent);
    void OnCommand(UINT uNotifyCode, int nID, HWND wndCtl);//�˵���Ӧ����
    void OnRButtonDown(UINT nFlags, CPoint point);//�Ҽ���Ϣ
    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);//������Ϣ
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
    void OnBtnResolution();//�ֱ���
	void OnBtnLast();//��һ��
	void OnBtnNext();//��һ��
	
    /*�Զ���ؼ�*/
    bool SliderEvent(EventArgs* pEvt);//�������¼�
    void OnDefinitionID(int nID);
    void OnBtnVolume();//������ť
	void OnNCXButtonDown(int fwButton, short nHittest, CPoint ptPos);
    /*�Զ�����Ϣ*/
    LRESULT WndPro(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT PlayEndPro(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT ReCheckTop(UINT uMsg, WPARAM wParam, LPARAM lParam);

    //soui��Ϣ
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

        //HostWnd��ʵ������Ϣ����
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
    WINDOWPLACEMENT m_OldWndPlacement; // ��¼����ԭʼ����
    BOOL m_bFirstStart;//��ʼ����
    BOOL m_bTigShow;
    BOOL m_bClickTimer;
	BOOL m_bMoveFlag;
    vector<int> m_vecSubCmdID;
    BOOL m_bGifLoading;
    BOOL m_bSetChannel;
    BOOL m_bSetOpen;
    BOOL m_bShowLink;//�Ƿ���ʾ��Ա��ֵ��ʾ
    __int64 m_llResolutionTime;//�ֱ����л�ʱ�Ѳ���ʱ��
    BOOL m_bResolution;
    int m_nTotalTeleplay;//�ܾ缯��
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
	// ����
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
    BOOL m_bMute;//�Ƿ���
    BOOL m_bShowWeb;
    int m_nMiguPlayID;//�乾��ǰ���ž缯
    int m_nResolu; //1:���� 2:���� 3:����    
    BOOL m_bStartPlay;
    string m_sMd5;
    BOOL m_bNoMark;
	BOOL m_bShowRight;
	sPlayData m_sPlayData;
	// ����״̬
	static ePlayStatus m_ePlayerStatus;
};

