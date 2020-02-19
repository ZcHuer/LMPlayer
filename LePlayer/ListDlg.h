#pragma once
#include <vector>
#include <string>
#include <map>
using namespace std;


struct fItem
{
    int nCode;//1:�����ļ� 2:�����ļ�
    int nID;
    int nParentID;
    SStringT strPath;//�ļ�·������URL��ַ
    SStringT strShort;//�ļ��������ַ���
    SStringT strAlbumplayed;//ר���Ѳ����ļ�·��
    BOOL bShowChild;//�Ƿ���ʾ����
    long lTime;//ʱ��
    long lPlayedTime;//�Ѳ���ʱ��
    vector<fItem*> vecChild;
    fItem()
    {
        nCode = 2;
        nID = 0;
        nParentID = 0;
        lTime = 0;
        lPlayedTime = 0;
        strPath = L"";
        strShort = L"";
        strAlbumplayed = L"";
        bShowChild = TRUE;
    }

    void Copy(fItem file)
    {
        nCode = file.nCode;
        nID = file.nID;
        nParentID = file.nParentID;
        strPath = file.strPath;
        strShort = file.strShort;
        strAlbumplayed = file.strAlbumplayed;
        bShowChild = file.bShowChild;
        lTime = file.lTime;
        lPlayedTime = file.lPlayedTime;
        vecChild = vector<fItem*>(file.vecChild.begin(),file.vecChild.end());
    }
};

class CListDlg :
    public SHostWnd
{
public:
    CListDlg();
    ~CListDlg();

    void GetPlayInfo(SStringT strUrl);
    void SetPlay(SStringT strUrl);
    BOOL GetPlyaUrl(string sContid,int nPlayID);
    void UpdateList(BOOL bLocal=TRUE);
    void InsertList(SStringT strPath,BOOL bPlay=FALSE);
    void InsertDB(string path);
    void InsertThread();
    void Stop();
    void UpdateBtnStatus();
    void Write2Disk();
    void ReadRecord();
    void SetPlayed(SStringT strPath,__int64 llPlayed);
    long GetPlayed(SStringT strPath);
	void Screen2Item();
    int GetMaxNumber();
    __int64 GetTotalTime(SStringT strPath);
    void ResetTimes(SStringT strPath,long lTime);
    void DrawItemNoPlay(SStringT strPath);
    void DelItemOfPath(string sPath);
protected:
    void ReDrawItem(int nID,BOOL bPlay);//�ػ�item
    BOOL MakeItemString(int nCode,fItem* file,BOOL bMaxSize,SStringT& strList);//��װitem��Ϣ
    void GetTimeThread();
    void Play(int nID);
    int Path2ID(SStringT strPath);
    int Path2ParentID(SStringT strPath);//�����ļ�����ȡ�б�ID
    void SetTime(SStringT strPath ,int nTime);
    fItem* GetFileinfoFormID(int nID);
    fItem* GetItemFormIndex(int nIndex);
    int GetIndexFormID(long nID);
    void ReShowTime(SItemWnd* pItem,int nTime);//������ʾʱ��
    void SortList(int nID);
    void AddList(int nCode,string strPath,string sStr,int nTotallTime,__int64 llPlayed);
    
    int IsSortList(CPoint point);
    void BtnSwitch(int nID);
    void DelItem(int nID);

    void MoveList(CPoint point);
    void ClearInvalid();
    SStringT GetString(SStringT str);
    SStringT GetGetThumbnailImage(SStringT strUrl,int w,int h,BOOL bFlag=FALSE);
    void ImageThread(SStringT strUrl, SStringT strPath, SStringT strBmp,int w, int h,BOOL bFlag);
    SStringT GetItemString(int nID,POINT pt,SStringT strTitle,SStringT strPath);
    void SetShowvip(BOOL b);
   
    //ϵͳ��Ϣ��Ӧ
    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
    int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
    void OnDropFiles(HDROP hDropInfo);// �ļ���ק
    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);//������Ϣ
    void OnCommand(UINT uNotifyCode, int nID, HWND wndCtl);//�˵���Ӧ����
    void OnMouseMove(UINT nFlags, CPoint point);
    void OnMouseLeave();
    void OnLButtonDown(UINT nFlags, CPoint point);
    void OnLButtonUp(UINT nFlags, CPoint point);
    void OnLButtonDblClk(UINT nFlags, CPoint point);
    void OnRButtonDown(UINT nFlags, CPoint point);
    void OnTimer(UINT_PTR nIDEvent);

    //�ؼ���Ϣ����
    void OnTitleCtrl(int nID);
    void OnBtnAdd();
    void OnBtnClear();
    void OnBtnSx();
    void OnBtnJx();
    void OnBtnZhankai();
    void OnBtnList(int nID);
    void OnVip();
    void OnBtnMorevideo();//�������

    //�Զ�����Ϣ����
    LRESULT ListRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT MsgPlay(UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT ItemClick(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT ItemIvalid(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnClearList(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT UpdateSkinPro(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnGetPlayUrl(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnPlayInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnSetImage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void SendTj(const TCHAR* sCategory, const TCHAR*  sAction, const TCHAR* sKey, const TCHAR* sValue);
    void TongjiThread(const TCHAR* sCategory, const TCHAR*  sAction, const TCHAR* sKey, const TCHAR* sValue);

    //soui��Ϣ
    EVENT_MAP_BEGIN()
        EVENT_ID_COMMAND_RANGE(100, 102, OnTitleCtrl)
        EVENT_ID_COMMAND_RANGE(1,50, OnBtnList)
        EVENT_NAME_COMMAND(L"btn_add", OnBtnAdd)
        EVENT_NAME_COMMAND(L"btn_clear", OnBtnClear)
        EVENT_NAME_COMMAND(L"btn_sx", OnBtnSx)
        EVENT_NAME_COMMAND(L"btn_jx", OnBtnJx)
        EVENT_NAME_COMMAND(L"btn_zhankai", OnBtnZhankai)
        EVENT_NAME_COMMAND(L"btn_vip", OnVip)
        EVENT_NAME_COMMAND(L"btn_morevideo",OnBtnMorevideo)
        EVENT_MAP_END()

        //HostWnd��ʵ������Ϣ����
        BEGIN_MSG_MAP_EX(CListDlg)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_DROPFILES(OnDropFiles)
        MSG_WM_KEYDOWN(OnKeyDown)
        MSG_WM_SYSKEYDOWN(OnKeyDown)
        MSG_WM_COMMAND(OnCommand)
        MSG_WM_TIMER(OnTimer)
        MSG_WM_MOUSEMOVE(OnMouseMove)
        MSG_WM_MOUSELEAVE(OnMouseLeave)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_RBUTTONDOWN(OnRButtonDown)
        MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
        MESSAGE_HANDLER_EX(UM_LIST_RBUTTONDOWN, ListRButtonDown)
        MESSAGE_HANDLER_EX(UM_LIST_PLAY, MsgPlay)
        MESSAGE_HANDLER_EX(UM_ITEM_CLICK, ItemClick)
        MESSAGE_HANDLER_EX(UM_INVALID, ItemIvalid)
        MESSAGE_HANDLER_EX(UM_CLEAR_LIST, OnClearList)
        MESSAGE_HANDLER_EX(UM_UPDATE_SKIN, UpdateSkinPro)
        MESSAGE_HANDLER_EX(UM_GEY_PLAYURL, OnGetPlayUrl)
        MESSAGE_HANDLER_EX(UM_PLAYINFO, OnPlayInfo)
        MESSAGE_HANDLER_EX(UM_SETIMAGE,OnSetImage)
        CHAIN_MSG_MAP(SHostWnd)
        REFLECT_NOTIFICATIONS_EX()
    END_MSG_MAP()
private:
    BOOL m_bLayoutInited;
    vector<fItem*> m_vecListData;
    vector <SStringT> m_vecInsert;
    SStringT m_strPlaypath;     //���ڲ����ļ�·��
    int m_nListID;
    HANDLE m_hEvent;
    map <SStringT, __int64> m_mapFilesTime;
    BOOL m_bNoprompt;
    BOOL m_bButtondown;
    int m_nDownID;
    int m_nPos;
    CPoint m_ptMove;
    int m_nListStatus;  //�б�״̬ 0��ȫ�� 1������ 2������
    BOOL m_bSortList;
    __int64 m_llNumber;
    BOOL m_bSwitchTimer;
    int m_nSwitchID;
    BOOL m_bFirstItem;
    int m_nOldHoverID;
    CLSID m_bmpClsid;
    vector<string> m_vecGuesslist;

    vector<string> m_vecData; //���������
    HANDLE m_hDbEvent;
    CMySqlite m_sql;
};

