#include "stdafx.h"
#include "MainSetDlg.h"
#include <shlobj.h>
#include "MainDlg.h"
#include "ClearDlg.h"
#include "FileAssDEfine.h"
#include "../include/LeReport.h"

extern HWND g_hPlayerWnd;
extern wstring m_strDefaultPath;
extern wstring g_strRootPath;


HWND CMainSetDlg::m_hMsgWnd = NULL;
bool CMainSetDlg::m_bShow = false;

extern mutex mutex_tj;
extern wstring m_strCachePath;
extern wstring m_strDefaultPath;
extern map<int, string> mapKey;

const wchar_t* _PAGE[] = {
    L"常规",
    L"播放",
    L"声音/画面",
    //L"下载",
    L"隐私设置",
    L"快捷键",
    L"文件关联",
    L"升级与更新",
    nullptr
};

class CPageAdapter : public SAdapterBase
{
public:
    CPageAdapter() { m_pOld = nullptr; }
    ~CPageAdapter() {}

    SWindow * m_pOld;

    virtual int getCount()
    {
        int nCount = 0;
        for each (auto var in _PAGE)
        {
            if (var) {
                nCount++;
            }
        }
        return nCount;
    }

    virtual void getView(int position, SWindow * pItem, pugi::xml_node xmlTemplate)
    {
        if (pItem->GetChildrenCount() == 0) {
            pItem->InitFromXml(xmlTemplate);
        }

        SWindow* pWnd = pItem->FindChildByName(L"page");
        if (!pWnd)return;
        if (0 == position)
        {
            pWnd->SetAttribute(L"colorText", L"#CCCCCC");
            m_pOld = pWnd;
        }
        pWnd->SetWindowTextW(_PAGE[position]);
        pItem->SetUserData(position);
        pItem->GetEventSet()->subscribeEvent(EVT_ITEMPANEL_CLICK, Subscriber(&CPageAdapter::OnItemClick, this));
        pItem->GetEventSet()->subscribeEvent(EVT_ITEMPANEL_HOVER, Subscriber(&CPageAdapter::OnItemHover, this));
    }

    bool OnItemClick(EventCmd *pEvt)
    {
        SWindow * pItem = sobj_cast<SWindow>(pEvt->sender);
        if (!pItem)
            return true;
        int nPage = pItem->GetUserData();

        ::PostMessage(CMainSetDlg::m_hMsgWnd,UM_SETTING_PAGE, nPage,0);

        SWindow* pWnd = pItem->FindChildByName(L"page");
        if (pWnd)
        {
            if (m_pOld){
                m_pOld->SetAttribute(L"colorText", L"#FFFFFF");
            }
            pWnd->SetAttribute(L"colorText",L"#FFFFFF");
            m_pOld = pWnd;
        }
        return true;
    }

    bool OnItemHover(EventCmd *pEvt)
    {
        SWindow * pItem = sobj_cast<SWindow>(pEvt->sender);
        if (!pItem)
            return true;
        return true;
    }
};

CMainSetDlg::CMainSetDlg() : SHostWnd(_T("LAYOUT:XML_MAINSETWND"))
{
    m_bLayoutInited = FALSE;
    m_hWndParent = NULL;
    m_bSure = FALSE;
    m_nPage = 0;
    m_pFocusEdit = nullptr;
}

CMainSetDlg::~CMainSetDlg()
{
}

BOOL CMainSetDlg::DoModel()
{
	m_bShow = true;
    if (!m_hWndParent) {
        m_hWndParent = ::GetActiveWindow();
    }

    BOOL bEnableParent = FALSE;
    if (m_hWndParent && m_hWndParent != ::GetDesktopWindow() && ::IsWindowEnabled(m_hWndParent)) {
        ::EnableWindow(m_hWndParent, FALSE);
        bEnableParent = TRUE;
    }
    m_hMsgWnd = Create(m_hWndParent, WS_POPUP | WS_CLIPCHILDREN, 0, 0, 0, 0, 0);

    if (!m_hMsgWnd) {
        return m_bSure;
    }

    CSimpleWnd::SendMessage(WM_INITDIALOG, (WPARAM)m_hWnd);

    HWND hWndLastActive = ::SetActiveWindow(m_hWnd);

    if (GetExStyle()&WS_EX_TOOLWINDOW)
        ::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
    else
        ::ShowWindow(m_hWnd, SW_SHOWNORMAL);

    SetWindowPos(HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    MSG msg;
    while (GetMessage(&msg, 0, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // From MFC
    // hide the window before enabling the parent, etc.

    if (IsWindow())
    {
        CSimpleWnd::SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
    }

    if (bEnableParent)
        ::EnableWindow(m_hWndParent, TRUE);

    ::SetActiveWindow(m_hWndParent);

    if (IsWindow())
        CSimpleWnd::DestroyWindow();

	m_bShow = false;
    return m_bSure;
}

/*
设置文件关联
*/
void CMainSetDlg::SetFileExt()
{
    //文件关联
    SWindow* pWnd;
    CLeDB::GetInstance()->ThingBegin();
    for (size_t i = EXT_ID_asf; i <= EXT_ID_gsm; i++)
    {
        pWnd = FindChildByID(i);
        if (pWnd){
			CLeDB::GetInstance()->FileAss()->SetAssociation(i,pWnd->IsChecked());
        }
    }
	CLeDB::GetInstance()->ThingEnd();
    ::PostMessage(CMainDlg::g_hMainWnd, UM_EXT_SET,0,0);
}

void CMainSetDlg::Init()
{
	SWindow* pWnd = nullptr;
	pWnd = FindChildByName(L"hk_tip");
	if (pWnd)
		pWnd->SetAttribute(L"show", L"0");

    /******************常规*********************************/
    //开机自启动
	string sValue = "0";
	char* pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("auto");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}	
    BOOL bAuto = atoi(sValue.c_str());
    pWnd = FindChildByID(2000);
    if (pWnd)
        pWnd->SetCheck(bAuto);

    //最小化到托盘，或者直接退出
	pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("quit");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}	
    BOOL bQuit = atoi(sValue.c_str());
	if (bQuit)
	{
		pWnd = FindChildByID(2002);
		if (pWnd)
			pWnd->SetCheck(1);
	}
	else
	{
		pWnd = FindChildByID(2001);
		if (pWnd)
			pWnd->SetCheck(1);
	}
    /********************播放************************************************/
    //视频窗口风格
	pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("wndStyle");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}	
    int nValue = atoi(sValue.c_str());
	pWnd = FindChildByID(nValue + 2010);
	if (pWnd) pWnd->SetCheck(1);
	//点击暂停播放
	pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("clickPause");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}	
    nValue = atoi(sValue.c_str());
	pWnd = FindChildByID(2025);
	if (pWnd)
		pWnd->SetCheck(nValue);
    //视频最小化时
	pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("minPause");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}	
    nValue = atoi(sValue.c_str());
	pWnd = FindChildByID(2021);
	if (pWnd) 
		pWnd->SetCheck(nValue);
    //快进速度
	pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("forwardSpeed");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}	
    nValue = atoi(sValue.c_str());
    SComboBox* pCmbo = FindChildByName2<SComboBox>(L"cmbx_forward");
    if (pCmbo)
    {
        for (size_t i = 0; i < 30; i++)
        {
            WCHAR cBuff[64] = { 0 };
            wsprintf(cBuff,L"%d",i+1);
            pCmbo->InsertItem(i, cBuff,0,0 );
        }
        pCmbo->SetCurSel(nValue-1);
    }
    //快退
	pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("rewindSpeed");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}	
    nValue = atoi(sValue.c_str());
    pCmbo = FindChildByName2<SComboBox>(L"cmbx_rewind");
    if (pCmbo)
    {
        for (size_t i = 0; i < 30; i++)
        {
            WCHAR cBuff[64] = { 0 };
            wsprintf(cBuff, L"%d", i + 1);
            pCmbo->InsertItem(i, cBuff, 0, 0);
        }
        pCmbo->SetCurSel(nValue - 1);
    }
    /*********************声音画面********************************************/
    //声道
	pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("channel");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}	
    nValue = atoi(sValue.c_str());
	pWnd = FindChildByID(nValue + 2040);
	if (pWnd) pWnd->SetCheck(1);
    //画面比例
	pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("screenRatio");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}	
    nValue = atoi(sValue.c_str());
	pWnd = FindChildByID(nValue + 2045);
	if (pWnd) pWnd->SetCheck(1);
    /*********************隐私设置********************************************/
    //退出时是否清空播放记录
	pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("quitClear");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}	
    nValue = atoi(sValue.c_str());
	pWnd = FindChildByID(2078);
	if (pWnd)pWnd->SetCheck(nValue);
    //是否开启无痕播放
	pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("noMark");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}	
    nValue = atoi(sValue.c_str());
	pWnd = FindChildByID(2079);
	if (pWnd) pWnd->SetCheck(nValue);

    /*********************更新设置********************************************/
    //是否自动更新
	pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("autoUpdate");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}	
    nValue = atoi(sValue.c_str());
	if (nValue)
	{
		pWnd = FindChildByID(2080);
		if (pWnd)
			pWnd->SetCheck(1);
	}
	else
	{
		pWnd = FindChildByID(2081);
		if (pWnd)
			pWnd->SetCheck(1);
	}

    //快捷键
    m_mapHotkey.clear();
    for (size_t i = HK_OPENFILE; i <= HK_FULLSCREEN; i++)
    {
        InitHotKey(i);
    }

    //文件关联
	CLeDB::GetInstance()->ThingBegin();
    for (size_t i = EXT_ID_asf; i <= EXT_ID_gsm; i++)
    {
        pWnd = FindChildByID(i);
        if (pWnd)pWnd->SetCheck(CLeDB::GetInstance()->FileAss()->GetAssociation(i));
    }
	CLeDB::GetInstance()->ThingEnd();

    UpdateRadio();
}

void CMainSetDlg::InitHotKey(int nID)
{
    SEdit* pEdit = FindChildByID2<SEdit>(nID);
    if (pEdit)
    {
        _HotKey hk;
		CLeDB::GetInstance()->HotKey()->GetHotkey(mapKey[nID].c_str(), hk);
        pEdit->SetWindowTextW(hk.GetWString().c_str());
        pEdit->GetEventSet()->subscribeEvent(EVT_SETFOCUS, Subscriber(&CMainSetDlg::OnFocusEvent, this));
        pEdit->GetEventSet()->subscribeEvent(EVT_KILLFOCUS, Subscriber(&CMainSetDlg::OnKillFocusEvent, this));
        m_mapHotkey[nID] = hk;
    }
}

bool CMainSetDlg::OnFocusEvent(EventArgs * pEvt)
{
    m_pFocusEdit = sobj_cast<SEdit>(pEvt->sender);
    SetUseEnable(1);
	SWindow* pWnd = FindChildByName(L"hk_tip");
	if(pWnd) pWnd->SetAttribute(L"show", L"0");
    return false;
}

bool CMainSetDlg::OnKillFocusEvent(EventArgs * pEvt)
{
    m_pFocusEdit = nullptr;
    return false;
}

bool CMainSetDlg::OnEditNotify(EventArgs * pEvt)
{
    EventRENotify * pEr = (EventRENotify*)pEvt;
    if (pEr->iNotify == EN_CHANGE)
    {
       
    }
    return false;
}

void CMainSetDlg::SetPercentText(int nPos)
{
    SWindow* pWnd = FindChildByName(L"text_percent");
    if (pWnd)
    {
        WCHAR wBuff[512] = { 0 };
        wsprintf(wBuff, L"最大占用%d%%", nPos);
        pWnd->SetWindowTextW(wBuff);
    }
}

int CMainSetDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    SetMsgHandled(FALSE);
    return 0;
}

BOOL CMainSetDlg::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    m_bLayoutInited = TRUE;
    SListView* pListView = FindChildByName2<SListView>(L"pagelist");
    if (pListView)
    {
        ILvAdapter *pAdapter = new CPageAdapter;
        pListView->SetAdapter(pAdapter);
        pAdapter->Release();
    }
    pListView->SetSel(0);

    SComboBox * pCBox = FindChildByName2<SComboBox>(L"cmbx_forward");
    if (pCBox){
        pCBox->GetEventSet()->subscribeEvent(EVT_SETFOCUS, Subscriber(&CMainSetDlg::OnCboxFocusEvent, this));
    }
    pCBox = FindChildByName2<SComboBox>(L"cmbx_rewind");
    if (pCBox) {
        pCBox->GetEventSet()->subscribeEvent(EVT_SETFOCUS, Subscriber(&CMainSetDlg::OnCboxFocusEvent, this));
    }
    pCBox = FindChildByName2<SComboBox>(L"cmbo_rendermode");
    if (pCBox) {
        pCBox->GetEventSet()->subscribeEvent(EVT_SETFOCUS, Subscriber(&CMainSetDlg::OnCboxFocusEvent, this));
    }

    SSliderBar* pSlider = FindChildByName2<SSliderBar>(L"brightness");
    if (pSlider) {
        pSlider->GetEventSet()->subscribeEvent(EVT_SLIDER_POS, Subscriber(&CMainSetDlg::SliderEvent, this));
    }
    pSlider = FindChildByName2<SSliderBar>(L"contrastratio");
    if (pSlider) {
        pSlider->GetEventSet()->subscribeEvent(EVT_SLIDER_POS,
            Subscriber(&CMainSetDlg::SliderEvent, this));
    }
    pSlider = FindChildByName2<SSliderBar>(L"saturation");
    if (pSlider) {
        pSlider->GetEventSet()->subscribeEvent(EVT_SLIDER_POS, Subscriber(&CMainSetDlg::SliderEvent, this));
    }
    pSlider = FindChildByName2<SSliderBar>(L"spacepercent");
    if (pSlider) {
        pSlider->GetEventSet()->subscribeEvent(EVT_SLIDER_POS, Subscriber(&CMainSetDlg::SliderEvent, this));
    }

    Init();

    SEdit* pEdit = FindChildByName2<SEdit>(L"maxspeed");
    if (pEdit)
    {
        pEdit->GetEventSet()->subscribeEvent(EVT_RE_NOTIFY,
            Subscriber(&CMainSetDlg::OnEditNotify, this));
    }
    SetUseEnable(0);
    return 0;
}

void CMainSetDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    SetMsgHandled(FALSE);
	SWindow* pWnd = FindChildByName(L"hk_tip");
	if(pWnd) pWnd->SetAttribute(L"show", L"0");
    if (!m_pFocusEdit) {
        return;
    }

    BOOL bAtl = FALSE,bShift = FALSE, bCtrl = FALSE;
    if (GetAsyncKeyState(VK_MENU) >> ((sizeof(SHORT) * 8) - 1))
        bAtl = TRUE;

    if (GetAsyncKeyState(VK_SHIFT) >> ((sizeof(SHORT) * 8) - 1))
        bShift = TRUE;

    if (GetAsyncKeyState(VK_CONTROL) >> ((sizeof(SHORT) * 8) - 1))
        bCtrl = TRUE;

    _HotKey hk;
    hk.SetKey(bCtrl, bAtl, bShift, nChar);
    wstring wsKey = hk.GetWString();
    if (wsKey.empty()) {
        return;
    }

    int nID = m_pFocusEdit->GetID();

    for (size_t i = HK_OPENFILE; i <= HK_FULLSCREEN; i++)
    {
        if (m_mapHotkey[i].IsSame(hk) && nID != i) {
			SWindow* pWnd = FindChildByName(L"hk_tip");
			if (pWnd) pWnd->SetAttribute(L"show", L"1");
            SStringT str;
            switch (i)
            {
            case HK_OPENFILE:str = L"与 打开文件 快捷键冲突，请重新设置"; break;
            case HK_OPENFLODER:str = L"与 打开文件夹 快捷键冲突，请重新设置"; break;
            case HK_OPENURL:str = L"与 打开URL 快捷键冲突，请重新设置"; break;
            case HK_PAUSE:str = L"与 暂停/播放 快捷键冲突，请重新设置"; break;
            case HK_STOP:str = L"与 停止 快捷键冲突，请重新设置"; break;
            case HK_QUIT:str = L"与 退出 快捷键冲突，请重新设置"; break;
            case HK_FORWARD:str = L"与 快进 快捷键冲突，请重新设置"; break;
            case HK_REWIND:str = L"与 快退 快捷键冲突，请重新设置"; break;
            case HK_FORWARD30:str = L"与 快进30秒 快捷键冲突，请重新设置"; break;
            case HK_REWIND30:str = L"与 快退30秒 快捷键冲突，请重新设置"; break;
            case HK_UPVOICE:str = L"与 提高音量 快捷键冲突，请重新设置"; break;
            case HK_DOWNVOICE:str = L"与 降低音量 快捷键冲突，请重新设置"; break;
            case HK_MUTE:str = L"与 静音 快捷键冲突，请重新设置"; break;
            case HK_FULLSCREEN:str = L"与 全屏/普通 快捷键冲突，请重新设置"; break;
            default:
                break;
            }
			if (pWnd) pWnd->SetWindowTextW(str);
            return;
        }
    }

    m_pFocusEdit->SetWindowTextW(wsKey.c_str());
    m_mapHotkey[nID]._Copy(hk);
}

void CMainSetDlg::OnClose()
{
    PostMessage(WM_QUIT);
	CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_SET_QUIT);
}

void CMainSetDlg::OnMin()
{
    ::SendMessage(CMainDlg::g_hMainWnd, WM_SYSCOMMAND, SC_MINIMIZE,0);
}

void CMainSetDlg::OnSure()
{
	CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_SET_SAVE);
    OnUse();
    OnClose();	
}

void CMainSetDlg::OnUse()
{
    SWindow* pWnd = NULL;
    switch (m_nPage)
    {
    case 0://常规
    {
        //关闭时是否退出程序
        pWnd = FindChildByID(2002);
        if (pWnd) 
		{
            int nValue = pWnd->IsChecked() ? 1 : 0;
			CLeDB::GetInstance()->Set()->UpdateSingleSet("quit",LeTools::Num2Str(nValue).c_str());
			CLeReport::GetInstance()->SaveData_Bool(ERD_BOOL_SET_CLOSEMIN, !(pWnd->IsChecked()));
			CLeReport::GetInstance()->SaveData_Bool(ERD_BOOL_SET_CLOSEQUIT, pWnd->IsChecked());
        }

		pWnd = FindChildByID(2000);
		if (pWnd)
		{
			int nValue = pWnd->IsChecked() ? 1 : 0;
			CLeDB::GetInstance()->Set()->UpdateSingleSet("auto",LeTools::Num2Str(nValue).c_str());
			LeTools::SetAutoRun(eLmplayer, NULL, nValue);
			CLeReport::GetInstance()->SaveData_Bool(ERD_BOOL_SET_AUTORUN, pWnd->IsChecked());
		}
        break;
    }
    case 1://播放
    {
        //视频窗口风格
		int nValue = 0;
        for (int i = 0; i < 3; i++)
        {			
			pWnd = FindChildByID(i + 2010);
            if (pWnd&&pWnd->IsChecked()) {
                nValue = i;
                break;
            }
        }
		CLeDB::GetInstance()->Set()->UpdateSingleSet("wndStyle",LeTools::Num2Str(nValue).c_str());
        //点击暂停播放
		pWnd = FindChildByID(2025);
        if (pWnd) 
		{
            nValue = pWnd->IsChecked() ? 1 : 0;
			CLeDB::GetInstance()->Set()->UpdateSingleSet("clickPause",LeTools::Num2Str(nValue).c_str());
        }
        //视频最小化时
		pWnd = FindChildByID(2021);
        if (pWnd) 
		{
			nValue = pWnd->IsChecked() ? 1 : 0;
			CLeDB::GetInstance()->Set()->UpdateSingleSet("minPause",LeTools::Num2Str(nValue).c_str());
        } 
		//快进速度
		pWnd = FindChildByName(L"cmbx_forward");
        if (pWnd) 
		{
            nValue = ((SComboBox*)pWnd)->GetCurSel() + 1;
			CLeDB::GetInstance()->Set()->UpdateSingleSet("forwardSpeed",LeTools::Num2Str(nValue).c_str());
        }
        //快退速度
		pWnd = FindChildByName(L"cmbx_rewind");
        if (pWnd) 
		{
            nValue = ((SComboBox*)pWnd)->GetCurSel() + 1;
			CLeDB::GetInstance()->Set()->UpdateSingleSet("rewindSpeed",LeTools::Num2Str(nValue).c_str());
        }
        break;
    }
    case 2://声音/画面
    {		
        //声道选择
		int nValue = 0;
        for (int i = 0; i < 5; i++)
        {
			pWnd = FindChildByID(i + 2040);
            if (pWnd&&pWnd->IsChecked()) 
			{
                nValue = i+1;
                ::PostMessage(g_hPlayerWnd, WM_COMMAND, MAKEWPARAM(AudioChannel_Stereo+i,5),
                (LPARAM)m_hWnd);
                break;
            }
        }
		CLeDB::GetInstance()->Set()->UpdateSingleSet("channel",LeTools::Num2Str(nValue).c_str());
        //画面比例
        for (int i = 0; i < 4; i++)
        {
			pWnd = FindChildByID(i + 2045);
            if (pWnd&&pWnd->IsChecked()) 
			{
                nValue = i;
                //实时更新
                ::PostMessage(g_hPlayerWnd, WM_COMMAND, MAKEWPARAM(SCREEN_NORMAL + i, 5),
                    (LPARAM)m_hWnd);
                break;
            }
        }
		CLeDB::GetInstance()->Set()->UpdateSingleSet("screenRatio",LeTools::Num2Str(nValue).c_str());
        break;
    }
    case 3://隐私设置
    {		
        //退出时是否清空播放记录
		int nValue = 0;
		pWnd = FindChildByID(2078);
        if (pWnd) 
		{
            nValue = pWnd->IsChecked() ? 1 : 0;
			CLeDB::GetInstance()->Set()->UpdateSingleSet("quitClear",LeTools::Num2Str(nValue).c_str());
			CLeReport::GetInstance()->SaveData_Bool(ERD_BOOL_SET_CLOSECLEAR, pWnd->IsChecked());			
        }
            
        //是否开启无痕播放
		pWnd = FindChildByID(2079);
        if (pWnd) 
		{
			nValue = pWnd->IsChecked() ? 1 : 0;
			CLeDB::GetInstance()->Set()->UpdateSingleSet("noMark",LeTools::Num2Str(nValue).c_str());
			CLeReport::GetInstance()->SaveData_Bool(ERD_BOOL_SET_RUNCLEAR, pWnd->IsChecked());
        }
        break;
    }
    case 4://快捷键
    {
        //快捷键
		CLeDB::GetInstance()->ThingBegin();
        for (int i = (int)HK_OPENFILE; i <= (int)HK_FULLSCREEN; i++)
        {
			CLeDB::GetInstance()->HotKey()->SetHotkey(mapKey[i].c_str(),m_mapHotkey[i]);
        }
		CLeDB::GetInstance()->ThingEnd();

		pWnd = FindChildByName(L"hk_tip");
		if(pWnd) 
			pWnd->SetAttribute(L"show", L"0");
        break;
    }
    case 5://文件关联
    {
        SetFileExt();
        break;
    }
    case 6://升级与更新
    {
        //是否自动更新
		int nValue = 0;
		pWnd = FindChildByID(2080);
        if (pWnd) {
			nValue = pWnd->IsChecked() ? 1 : 0;
			CLeDB::GetInstance()->Set()->UpdateSingleSet("autoUpdate",LeTools::Num2Str(nValue).c_str());
			CLeReport::GetInstance()->SaveData_Bool(ERD_BOOL_SET_AUTOUPDATE, pWnd->IsChecked());
			CLeReport::GetInstance()->SaveData_Bool(ERD_BOOL_SET_UPDATETIP, !(pWnd->IsChecked()));
        }
        break;
    }
    default:
        break;
    }
    
    SetUseEnable(0);
    Tongji();//发送设置统计
}

void CMainSetDlg::OnCancel()
{
    OnClose();
	CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_SET_CENCEL);
}

void CMainSetDlg::OnDefaults()
{
    switch (m_nPage)
    {
    case 0://常规
    {
		CLeDB::GetInstance()->Set()->UpdateSingleSet("quit", "0");
        break;
    }
    case 1://播放
    {
        CLeDB::GetInstance()->Set()->UpdateSingleSet("wndStyle", "1");
        CLeDB::GetInstance()->Set()->UpdateSingleSet("clickPause", "1");
        CLeDB::GetInstance()->Set()->UpdateSingleSet("minPause", "1");
        CLeDB::GetInstance()->Set()->UpdateSingleSet("forwardSpeed", "10");
        CLeDB::GetInstance()->Set()->UpdateSingleSet("rewindSpeed", "10");
        break;
    }
    case 2://声音/画面
    {
        CLeDB::GetInstance()->Set()->UpdateSingleSet("screenRatio", "0");
        CLeDB::GetInstance()->Set()->UpdateSingleSet("channel", "0");
        break;
    }
    case 3://隐私设置
    {
        CLeDB::GetInstance()->Set()->UpdateSingleSet("quitClear", "0");
        CLeDB::GetInstance()->Set()->UpdateSingleSet("noMark", "0");
        break;
    }
    case 4://快捷键
    {
		CLeDB::GetInstance()->HotKey()->DefaultHotkey();
        break;
    }
    case 5://文件关联
    {
		CLeDB::GetInstance()->FileAss()->DefaultAss(1);
        ::PostMessage(CMainDlg::g_hMainWnd, UM_EXT_SET, 0, 0);
        break;
    }
    case 6://升级与更新
    {
		CLeDB::GetInstance()->Set()->UpdateSingleSet("autoUpdate", "0");
        break;
    }
    default:
        break;
    }

    Init();
    SetUseEnable(TRUE);

    Tongji();
	CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_SET_DEFAULT);
}

int CALLBACK BrowseCallbackProc1(HWND hwnd, UINT uMsg, LPARAM /*lParam*/, LPARAM lpData)
{
    if (uMsg == BFFM_INITIALIZED)
    {
        SendMessage(hwnd, BFFM_SETSELECTION, (WPARAM)TRUE, lpData);
    }
    return 0;
}

void CMainSetDlg::OnPath()
{
    SetUseEnable(1);
    wchar_t szPath[MAX_PATH] = { 0 };
    BROWSEINFO brInfo;
    ITEMIDLIST *pItem;

    brInfo.hwndOwner = m_hWnd;// 窗口句柄  
    brInfo.pidlRoot = 0;
    brInfo.pszDisplayName = 0;
    brInfo.lpszTitle = L"选择文件夹";
    brInfo.ulFlags = 0;
    brInfo.lpfn = BrowseCallbackProc1;
    brInfo.iImage = 0;
    brInfo.lParam = LPARAM(m_strCachePath.c_str());
    pItem = SHBrowseForFolder(&brInfo);
    if (pItem != NULL)
    {
        if (SHGetPathFromIDList(pItem, szPath))
        {
			SEdit* pEdit = FindChildByName2<SEdit>(L"edit_cache");
			if(pEdit) pEdit->SetWindowTextW(szPath);
            string strPath =LeTools::ws2s(szPath);
            SWindow* pWnd = FindChildByName(L"edit_msg");
            if (!pWnd) return;
            if (!LeTools::IsFloderCanWrite(strPath.c_str()))
                pWnd->SetAttribute(L"show",L"1");
            else
                pWnd->SetAttribute(L"show", L"0");
        }
    }
}

void CMainSetDlg::OnClearList()
{	
	CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_SET_CLEARHISTROY);
    CClearDlg dlg;
    BOOL bClear = dlg.DoModel(1);
    if (bClear){
        ::PostMessage(CMainDlg::g_hMainWnd, UM_CLEAR_LIST, 0, 0);
    }
}

void CMainSetDlg::OnAllCheck()
{
	ExtCheck(TRUE);
}

void CMainSetDlg::OnNoCheck()
{
	ExtCheck(FALSE);
}

void CMainSetDlg::OnDefaultCheck()
{
	OnDefaults();
}

void CMainSetDlg::Tongji()
{

}

void CMainSetDlg::ExtCheck(BOOL bCheck)
{
	//文件关联
	SWindow* pWnd = nullptr;
	for (size_t i = EXT_ID_asf; i <= EXT_ID_gsm; i++)
	{
		pWnd = FindChildByID(i);
		if (pWnd) pWnd->SetCheck(bCheck);
	}

	pWnd = FindChildByName(L"WindowsMedia");
	if (pWnd) pWnd->SetCheck(bCheck);
	pWnd = FindChildByName(L"RealMedia");
	if (pWnd) pWnd->SetCheck(bCheck);
	pWnd = FindChildByName(L"QuickTime");
	if (pWnd) pWnd->SetCheck(bCheck);
	pWnd = FindChildByName(L"mpeg");
	if (pWnd) pWnd->SetCheck(bCheck);
	pWnd = FindChildByName(L"mpeg4");
	if (pWnd) pWnd->SetCheck(bCheck);
	pWnd = FindChildByName(L"3gpp");
	if (pWnd) pWnd->SetCheck(bCheck);
	pWnd = FindChildByName(L"flash");
	if (pWnd) pWnd->SetCheck(bCheck);
	pWnd = FindChildByName(L"vcd/dvd");
	if (pWnd) pWnd->SetCheck(bCheck);
	pWnd = FindChildByName(L"other");
	if (pWnd) pWnd->SetCheck(bCheck);
	pWnd = FindChildByName(L"audio");
	if (pWnd) pWnd->SetCheck(bCheck);

	SetUseEnable(TRUE);
}

void CMainSetDlg::UpdateRadio()
{
    BOOL bAllCheck = TRUE;
    SWindow* pWnd;
    //WindowsMedia媒体
    for (size_t i = EXT_ID_asf; i <= EXT_ID_dvrms; i++)
    {
        pWnd = FindChildByID(i);
        if (pWnd) bAllCheck = pWnd->IsChecked();
        if (!bAllCheck)break;
    }
    pWnd = FindChildByName(L"WindowsMedia");
    if (pWnd)pWnd->SetCheck(bAllCheck);
    //Real媒体文件
    bAllCheck = TRUE;
    for (size_t i = EXT_ID_rmvb; i <= EXT_ID_smil; i++)
    {
        pWnd = FindChildByID(i);
        if (pWnd) bAllCheck = pWnd->IsChecked();
        if (!bAllCheck)break;
    }
    pWnd = FindChildByName(L"RealMedia");
    if (pWnd)pWnd->SetCheck(bAllCheck);
    //QuickTime媒体文件
    bAllCheck = TRUE;
    for (size_t i = EXT_ID_mov; i <= EXT_ID_qt; i++)
    {
        pWnd = FindChildByID(i);
        if (pWnd) bAllCheck = pWnd->IsChecked();
        if (!bAllCheck)break;
    }
    pWnd = FindChildByName(L"QuickTime");
    if (pWnd)pWnd->SetCheck(bAllCheck);
    //MPEG1/2媒体
    bAllCheck = TRUE;
    for (size_t i = EXT_ID_m1v; i <= EXT_ID_ts; i++)
    {
        pWnd = FindChildByID(i);
        if (pWnd)
        {
            bAllCheck = pWnd->IsChecked();
            if (!bAllCheck)break;
        }
    }
    pWnd = FindChildByName(L"mpeg");
    if (pWnd)pWnd->SetCheck(bAllCheck);
    //MPEG4媒体
    bAllCheck = TRUE;
    for (size_t i = EXT_ID_mp4; i <= EXT_ID_mpeg4; i++)
    {
        pWnd = FindChildByID(i);
        if (pWnd) bAllCheck = pWnd->IsChecked();
        if (!bAllCheck)break;
    }
    pWnd = FindChildByName(L"mpeg4");
    if (pWnd)pWnd->SetCheck(bAllCheck);
    //3GPP媒体
    bAllCheck = TRUE;
    for (size_t i = EXT_ID_3g2; i <= EXT_ID_3gpp; i++)
    {
        pWnd = FindChildByID(i);
        if (pWnd) bAllCheck = pWnd->IsChecked();
        if (!bAllCheck)break;
    }
    pWnd = FindChildByName(L"3gpp");
    if (pWnd)pWnd->SetCheck(bAllCheck);
    //flash媒体
    bAllCheck = TRUE;
    for (size_t i = EXT_ID_flv; i <= EXT_ID_swf; i++)
    {
        pWnd = FindChildByID(i);
        if (pWnd) bAllCheck = pWnd->IsChecked();
        if (!bAllCheck)break;
    }
    pWnd = FindChildByID(EXT_ID_fli);
    if (pWnd) bAllCheck = pWnd->IsChecked();
    pWnd = FindChildByID(EXT_ID_flc);
    if (pWnd) bAllCheck = pWnd->IsChecked();

    pWnd = FindChildByName(L"flash");
    if (pWnd)pWnd->SetCheck(bAllCheck);
    //VCD/DVD视频
    bAllCheck = TRUE;
    pWnd = FindChildByID(EXT_ID_vob);
    if (pWnd) bAllCheck = pWnd->IsChecked();
    pWnd = FindChildByName(L"vcd/dvd");
    if (pWnd)pWnd->SetCheck(bAllCheck);
    //其他视频格式
    bAllCheck = TRUE;
    for (size_t i = EXT_ID_amv; i <= EXT_ID_wtv; i++)
    {
        pWnd = FindChildByID(i);
        if (pWnd) bAllCheck = pWnd->IsChecked();
        if (!bAllCheck)break;
    }
    pWnd = FindChildByName(L"other");
    if (pWnd)pWnd->SetCheck(bAllCheck);
    //音频文件
    bAllCheck = TRUE;
    for (size_t i = EXT_ID_aac; i <= EXT_ID_aif; i++)
    {
        pWnd = FindChildByID(i);
        if (pWnd) bAllCheck = pWnd->IsChecked();
        if (!bAllCheck)break;
    }
    for (size_t i = EXT_ID_arm; i <= EXT_ID_gsm; i++)
    {
        pWnd = FindChildByID(i);
        if (pWnd) bAllCheck = pWnd->IsChecked();
        if (!bAllCheck)break;
    }
    pWnd = FindChildByName(L"audio");
    if (pWnd)pWnd->SetCheck(bAllCheck);
}

//WindowsMedia媒体
void CMainSetDlg::OnWindowMedia()
{
    SWindow* pWnd = FindChildByName(L"WindowsMedia");
    if (pWnd)
    {
        BOOL bChecked = pWnd->IsChecked();
        for (size_t i = EXT_ID_asf; i <= EXT_ID_dvrms; i++)
        {
            pWnd = FindChildByID(i);
            if (pWnd)
                pWnd->SetCheck(bChecked);
        }
    }
	//UpdateRadio();
	SetUseEnable(TRUE);
}

//Real媒体文件
void CMainSetDlg::OnRealMedia()
{
    SWindow* pWnd = FindChildByName(L"RealMedia");
    if (pWnd)
    {
        BOOL bChecked = pWnd->IsChecked();
        for (size_t i = EXT_ID_rmvb; i <= EXT_ID_smil; i++)
        {
            pWnd = FindChildByID(i);
            if (pWnd)
                pWnd->SetCheck(bChecked);
        }
    }
	//UpdateRadio();
	SetUseEnable(TRUE);
}

//QuickTime媒体文件
void CMainSetDlg::OnQuickTimeMedia()
{
    SWindow* pWnd = FindChildByName(L"QuickTime");
    if (pWnd)
    {
        BOOL bChecked = pWnd->IsChecked();
        for (size_t i = EXT_ID_mov; i <= EXT_ID_qt; i++)
        {
            pWnd = FindChildByID(i);
            if (pWnd)
                pWnd->SetCheck(bChecked);
        }
    }
	//UpdateRadio();
	SetUseEnable(TRUE);
}

//MPEG1/2媒体
void CMainSetDlg::OnMpegMedia()
{
    SWindow* pWnd = FindChildByName(L"mpeg");
    if (pWnd)
    {
        BOOL bChecked = pWnd->IsChecked();
        for (size_t i = EXT_ID_m1v; i <= EXT_ID_ts; i++)
        {
            pWnd = FindChildByID(i);
            if (pWnd)
                pWnd->SetCheck(bChecked);
        }
    }
	//UpdateRadio();
	SetUseEnable(TRUE);
}

//MPEG4媒体
void CMainSetDlg::OnMpeg4Media()
{
    SWindow* pWnd = FindChildByName(L"mpeg4");
    if (pWnd)
    {
        BOOL bChecked = pWnd->IsChecked();
        for (size_t i = EXT_ID_mp4; i <= EXT_ID_mpeg4; i++)
        {
            pWnd = FindChildByID(i);
            if (pWnd)
                pWnd->SetCheck(bChecked);
        }
    }
	//UpdateRadio();
	SetUseEnable(TRUE);
}

//3GPP媒体
void CMainSetDlg::On3gppMedia()
{
    SWindow* pWnd = FindChildByName(L"3gpp");
    if (pWnd)
    {
        BOOL bChecked = pWnd->IsChecked();
        for (size_t i = EXT_ID_3g2; i <= EXT_ID_3gpp; i++)
        {
            pWnd = FindChildByID(i);
            if (pWnd)
                pWnd->SetCheck(bChecked);
        }
    }
	//UpdateRadio();
	SetUseEnable(TRUE);
}

//flash媒体
void CMainSetDlg::OnFlashMedia()
{
    SWindow* pWnd = FindChildByName(L"flash");
    if (pWnd)
    {
        BOOL bChecked = pWnd->IsChecked();
        for (size_t i = EXT_ID_flv; i <= EXT_ID_swf; i++)
        {
            pWnd = FindChildByID(i);
            if (pWnd) pWnd->SetCheck(bChecked);
        }
    }
	//UpdateRadio();
	SetUseEnable(TRUE);
}

//VCD/DVD视频
void CMainSetDlg::OnVcdMedia()
{
    SWindow* pWnd = FindChildByName(L"vcd/dvd");
    if (pWnd)
    {
        BOOL bChecked = pWnd->IsChecked();
        for (size_t i = EXT_ID_vob; i <= EXT_ID_ifo; i++)
        {
            pWnd = FindChildByID(i);
            if (pWnd)
                pWnd->SetCheck(bChecked);
        }
    }
	//UpdateRadio();
	SetUseEnable(TRUE);
}

//其他视频格式
void CMainSetDlg::OnOtherMedia()
{
    SWindow* pWnd = FindChildByName(L"other");
    if (pWnd)
    {
        BOOL bChecked = pWnd->IsChecked();
        for (size_t i = EXT_ID_mkv; i <= EXT_ID_wtv; i++)
        {
            pWnd = FindChildByID(i);
            if (pWnd)
                pWnd->SetCheck(bChecked);
        }
    }
	//UpdateRadio();
	SetUseEnable(TRUE);
}

//音频文件
void CMainSetDlg::OnAudioMedia()
{
    SWindow* pWnd = FindChildByName(L"audio");
    if (pWnd)
    {
        BOOL bChecked = pWnd->IsChecked();
        for (size_t i = EXT_ID_aac; i <= EXT_ID_aif; i++)
        {
            pWnd = FindChildByID(i);
            if (pWnd)
                pWnd->SetCheck(bChecked);
        }
    }
	//UpdateRadio();
	SetUseEnable(TRUE);
}

LRESULT CMainSetDlg::ChangePage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (m_nPage == wParam)return 0;

	SWindow* pWnd = FindChildByName(L"hk_tip");
	if(pWnd) pWnd->SetAttribute(L"show", L"0");

    pWnd = FindChildByID(m_nPage+1000);
    if (pWnd)
    {
        pWnd->SetAttribute(L"show",L"0");
    }
    m_nPage = wParam;
    pWnd = FindChildByID(m_nPage+1000);
    if (pWnd)
    {
        pWnd->SetAttribute(L"show", L"1");
    }
    SetUseEnable(0);
    return 0;
}

void CMainSetDlg::Setting(int nID)
{
	SWindow* pWnd;
    if (nID == 2061) {
		pWnd = FindChildByName(L"spacepercent");
		if(pWnd) pWnd->EnableWindow(1);
    }
    else {
		pWnd = FindChildByName(L"spacepercent");
		if(pWnd) pWnd->EnableWindow(0);
    }
	pWnd = FindChildByName(L"speedctrl");
    if (nID == 2072){
		if (pWnd)
		{
			pWnd->EnableWindow(1);
			pWnd->SetAttribute(L"enable", L"1");
		}
    }
    else
	{
		if(pWnd) pWnd->EnableWindow(0);
    }

	
    
	//文件关联
	if (5 == m_nPage)
	{
		UpdateRadio();
	}
	
    SetUseEnable(TRUE);
}

void CMainSetDlg::SetUseEnable(BOOL b)
{
    SWindow* pWnd = FindChildByName(L"btn_use");
    if (pWnd) {
        pWnd->EnableWindow(b);
        pWnd->Invalidate();
    }
}

bool CMainSetDlg::OnCboxFocusEvent(EventArgs * pEvt)
{
    SetUseEnable(1);
    return false;
}

bool CMainSetDlg::SliderEvent(EventArgs * pEvt)
{
    SetUseEnable(1);
    if (EVT_SLIDER_POS == pEvt->GetID())
    {
        if (pEvt->sender->GetName() == tr(_T("spacepercent")))
        {
            EventSliderPos* evtPos = (EventSliderPos*)pEvt;
            SetPercentText(evtPos->nPos);
        }
    }
    return false;
}

void CMainSetDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (ET_HIDE_EDITTIP == nIDEvent)
    {
        KillTimer(nIDEvent);
        SWindow* pWnd = FindChildByName(L"tip_edit");
        if (pWnd) pWnd->SetAttribute(L"show", L"0");
    }
}
