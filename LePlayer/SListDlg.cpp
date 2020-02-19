#include "stdafx.h"
#include "SListDlg.h"
#include "PlayerDlg.h"
#include "ClearDlg.h"
#include "MainDlg.h"
#include "../include/LeReport.h"

HWND g_hListWnd;
extern map<int, string> mapKey;


SListDlg::SListDlg() : SHostWnd(_T("LAYOUT:XML_LIST"))
{
    m_nSelID = 0;
    m_sPlayMd5 = "";
    m_nMaxID = 0;
}

SListDlg::~SListDlg()
{
}

int SListDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    SetMsgHandled(FALSE);
    return 0;
}

BOOL SListDlg::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    DWORD dwNewStyle = WS_CHILDWINDOW | WS_CLIPCHILDREN;
    ::SetWindowLong(m_hWnd, GWL_STYLE, dwNewStyle);//设置成新的样式	
    g_hListWnd = m_hWnd;
    SListTitle::m_nSelect = 102;
    ReListWnd();
    return 0;
}

void SListDlg::InsertList(sPlayData* pData, BOOL bPlay/* = FALSE*/)
{
    m_sPlayMd5 = "";
    //是否无痕播放
	char chTmp[MAX_PATH] = { 0 };
	string sValue = "0";
	char* pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("noMark");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}	
    if (atoi(sValue.c_str()) == 1)
		return;
    //string strPath =LeTools::ConvertGBKToUtf8(pData->strSource);
	//string strName =LeTools::ConvertGBKToUtf8(pData->strName);
	string strPath =LeTools::ws2s(pData->strSource);
	string strName =LeTools::ws2s(pData->strName);

	MD5 md5(LeTools::ws2s(pData->strSource));
    string sMd5 = md5.toString();
    if (bPlay)
		m_sPlayMd5 = sMd5;

	CLeDB::GetInstance()->Local()->InsertLocalFileList(sMd5.c_str(), strPath.c_str(), strName.c_str(), 0, 0, 0);

    KillTimer(ET_RELOAD_LIST);
    SetTimer(ET_RELOAD_LIST,500);
	return;
}

void SListDlg::StopPlay(BOOL bPlayEnd)
{
	if (!bPlayEnd)
	{
		SItemWnd::m_nPlayingID = 0;
	}
	else
	{
		char chTmp[MAX_PATH] = { 0 };
		string sValue = "0";
		char* pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("playOrder");
		if (NULL != pchValue)
		{
			sValue = pchValue;
			CLeDB::GetInstance()->ReleaseStr(pchValue);
		}
		
		if (2 == atoi(sValue.c_str()))
		{
			SItemWnd::m_nPlayingID = 0;
		}
	}
		
    SWindow::Invalidate();
}

BOOL SListDlg::PlayNext()
{
    int nPlayID = 0;
	char chTmp[MAX_PATH] = { 0 };
	string sValue = "0";
	char* pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("playOrder");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}
	
    int nOrder = atoi(sValue.c_str());
    switch (nOrder)
    {
    case 0://顺序播放
	{
		nPlayID = SItemWnd::m_nPlayingID + 1;
		if (nPlayID > m_nMaxID)
		{
			return FALSE;
		}
		break;
	}
    case 4://列表循环
    {
        nPlayID = SItemWnd::m_nPlayingID + 1;
        if (nPlayID > m_nMaxID) 
		{
            nPlayID = 1;
            if (nOrder == 0)//顺序播放已经是最后不再处理
                return FALSE;
        } 
    }
	break;
    case 1://随机播放
    {
        srand(time(NULL));
        nPlayID = rand() % m_nMaxID + 1;
    }
	break;
    case 2://单个播放
    {
        return FALSE;//单个播放不处理
    }
	break;
    case 3://单个循环
    {
        nPlayID = SItemWnd::m_nPlayingID;
    }
	break;
    default:
        break;
    }
    OnItemCommand(nPlayID);
    return TRUE;
}

BOOL SListDlg::PlayLast()
{
    int nPlayID = 0;
	char chTmp[MAX_PATH] = { 0 };
	string sValue = "0";
	char* pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("playOrder");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}
	
    int nOrder = atoi(sValue.c_str());
    switch (nOrder)
    {
    case 0:
    case 4:
    {
        nPlayID = SItemWnd::m_nPlayingID - 1;
        if (nPlayID < 1) {
            nPlayID = m_nMaxID;
            if (nOrder == 0)//顺序播放已经是最后不再处理
                return FALSE;
        }
    }break;
    case 1:
    {
        srand(time(NULL));
        nPlayID = rand() % m_nMaxID + 1;
    }break;
    case 2:
    {
        return FALSE;//单个播放不处理
    }break;
    case 3:
    {
        nPlayID = SItemWnd::m_nPlayingID;
    }break;
    default:
        break;
    }
    OnItemCommand(nPlayID);
    return TRUE;
}

void SListDlg::DoClearList()
{
	CLeDB::GetInstance()->Local()->DelLocalFileList();
    m_sPlayMd5 = "";
	SItemWnd::m_nPlayingID = 0;
    ReListWnd();
}

void SListDlg::GetNumAndTotal(int& nNum, int&nTotal)
{
	nNum = SItemWnd::m_nPlayingID;
	nTotal = m_nMaxID;
	return;
}

void SListDlg::UpdateListItemTimer()
{
	m_nMaxID = 0;
	TiXmlDocument doc;

	void* pVoid_Local = NULL;	
	CLeDB::GetInstance()->Local()->SelectLocalFileList(&pVoid_Local);
	if (NULL == pVoid_Local)
		return;
	vector<sLocalFileItem*>* pVec_Local = (vector<sLocalFileItem*>*)pVoid_Local;
	string sPos = "0";	
	for (vector<sLocalFileItem*>::iterator it = pVec_Local->begin(); it != pVec_Local->end(); it++)
	{
		sLocalFileItem* pinfo = *it;
		string strmd5 = pinfo->sMd5;
		string strpath = pinfo->sPath;
		string strname = pinfo->sName;
		if (strname.length() > 0)
			ItemList(doc, sPos, strname, pinfo->lCurrPlayTime, strmd5, pinfo->bError);
		sPos = "[1";
	}

	TiXmlPrinter print;
	doc.Accept(&print);
	const char* lpDoc = print.CStr();
	SStringT sStr =LeTools::s2ws(lpDoc).c_str();
	SWindow* pWnd = FindChildByName(L"play_list");
	if (!pWnd)return;
	WCHAR wBuff[256] = { 0 };
	wsprintf(wBuff, L"0,0,-0,@%d", (pVec_Local->size()) * 49 - 1);
	pWnd->SetAttribute(L"pos", wBuff);
	SWindow *pChild = pWnd->GetWindow(GSW_FIRSTCHILD);
	while (pChild)
	{
		SWindow *pNext = pChild->GetWindow(GSW_NEXTSIBLING);
		pChild->DestroyWindow();
		pChild = pNext;
	}
	pWnd->CreateChildren(sStr);
	CLeDB::GetInstance()->ReleaseVecLocal(pVoid_Local);
}
void SListDlg::ReListWnd()
{
    m_nMaxID = 0;
    TiXmlDocument doc;

	void* pVoid_Local = NULL;
    CLeDB::GetInstance()->Local()->SelectLocalFileList(&pVoid_Local);
	if (NULL == pVoid_Local)
		return;
	vector<sLocalFileItem*>* pVec_Local = (vector<sLocalFileItem*>*)pVoid_Local;
    string sPos = "0";
	for (vector<sLocalFileItem*>::iterator it = pVec_Local->begin(); it != pVec_Local->end(); it++)
	{
 		sLocalFileItem* pinfo = *it;
 		string strmd5 = pinfo->sMd5;
 		string strpath = pinfo->sPath;
 		string strname = pinfo->sName;
		if (strname.length() > 0)
 			ItemList(doc, sPos, strname, pinfo->lCurrPlayTime, strmd5, pinfo->bError);
 		sPos = "[1";
	}

    TiXmlPrinter print;
    doc.Accept(&print);
    const char* lpDoc = print.CStr();
    SStringT sStr =LeTools::s2ws(lpDoc).c_str();
    SWindow* pWnd = FindChildByName(L"play_list");
    if (!pWnd)
		return;
    WCHAR wBuff[256] = { 0 };
    wsprintf(wBuff, L"0,0,-0,@%d", (pVec_Local->size())*49-1);
    pWnd->SetAttribute(L"pos", wBuff);
    SWindow *pChild = pWnd->GetWindow(GSW_FIRSTCHILD);
    while (pChild) 
	{
        SWindow *pNext = pChild->GetWindow(GSW_NEXTSIBLING);
        pChild->DestroyWindow();
        pChild = pNext;
    }
    pWnd->CreateChildren(sStr);
	CLeDB::GetInstance()->ReleaseVecLocal(pVoid_Local);
}

void SListDlg::ItemList(TiXmlDocument & doc, string sPos,
    string sName, int nTime, string sIndex, BOOL bError)
{
    TiXmlElement* elMain = new TiXmlElement("itemwnd");
    doc.LinkEndChild(elMain);
    char cPos[128] = { 0 };
    sprintf_s(cPos, "0,%s,-0,@28", sPos.c_str());
    elMain->SetAttribute("pos", cPos);
    static int nId = 0;
    char cBuff[64] = { 0 };
    ltoa(++m_nMaxID, cBuff,10);
    elMain->SetAttribute("id", cBuff);
    elMain->SetAttribute("dotted", "1");
    if (bError) {
        elMain->SetAttribute("error", "1");
    }
    elMain->SetAttribute("timeText", Time2Str(nTime).c_str());
    elMain->SetAttribute("index", sIndex.c_str());
    if(!bError)
        elMain->SetAttribute("class", "cls_listwnd");
    else
        elMain->SetAttribute("class", "cls_listwnd1");
	if(ePS_Pause == m_status)
		elMain->SetAttribute("pause", "1");
	else
		elMain->SetAttribute("pause", "0");
	//offset="-1,-0.5"
	//elMain->SetAttribute("textOffset", 10);
	elMain->SetAttribute("fPlayingSkin", "cls_playing");
	//string strname =LeTools::ConvertUtf8ToGBK(sName).c_str();
	
	TiXmlText* xmlTex = new TiXmlText(sName.c_str());
    elMain->LinkEndChild(xmlTex);
    if (m_sPlayMd5 == sIndex)
        SItemWnd::m_nPlayingID = m_nMaxID;
}

string SListDlg::Time2Str(int nTime)
{
    string sTime = "";
    if (nTime)
    {
        char cBuff[128] = { 0 };
        if (nTime / 60) {
            sprintf_s(cBuff, "%d分", nTime / 60);
            sTime.append(cBuff);
            memset(cBuff,0,128);
        }
        if (nTime % 60)
        {
            sprintf_s(cBuff, "%d秒", nTime % 60);
            sTime.append(cBuff);
            memset(cBuff, 0, 128);
        }
    }
    return sTime;
}

void SListDlg::SelPaly()
{
    SItemWnd* pItem = FindChildByID2<SItemWnd>(m_nSelID);
    if (!pItem)
		return;    

	BOOL bPlaying = SItemWnd::m_nPlayingID == m_nSelID ? TRUE : FALSE;
	if (bPlaying)
	{
		::PostMessage(CPlayerDlg::m_pThis->m_hWnd, UM_PLAY_PAUSE, 0, 0);
	}
	else
	{
		SItemWnd::m_nPlayingID = m_nSelID;

		wstring sIndex = pItem->GetItemIndex().GetBuffer(0);
		string sMd5 = LeTools::ws2s(sIndex);
		// 数据库是单字节类型的，为了满足特殊字符，播放列表保存时直接双字节强制转换了，取出可逆
		char* cBuff_Path = CLeDB::GetInstance()->Local()->SelectPath(sMd5.c_str());
		wstring wstrPath =LeTools::ConvertUtf8ToGBKT(cBuff_Path);
		wstring wstrName = wstrPath.substr(wstrPath.rfind(L"\\") + 1);

		// 播放消息统一发给maindlg处理
		sPlayData* pData = new sPlayData;
		pData->eSignal = eSignalType_PlayList;
		pData->eSrcType = eSourceType_Local;
		pData->strSource = wstrPath.c_str();
		pData->strName = wstrName.c_str();
		pData->nNum = 0;
		pData->nTotal = 0;
		pData->sMd5 = sIndex;
		pData->ifConsume = _T("2");
		pData->sRD.videoId =LeTools::ws2s(wstrName);
		::PostMessage(CMainDlg::g_hMainWnd, UM_PLAY, (WPARAM)pData, 0);
	}
    
    pItem->SetUserData(0);
    SWindow::Invalidate();
}

void SListDlg::DelItem()
{
    SItemWnd* pItem = FindChildByID2<SItemWnd>(m_nSelID);
    if (!pItem)
		return;
    SStringT sIndex = pItem->GetItemIndex();
    string sMd5 =LeTools::ws2s(sIndex.GetBuffer(0));
    
	CLeDB::GetInstance()->Local()->DelLocalFileItem(sMd5.c_str());

	if (0 == m_sPlayMd5.compare(sMd5))
	{
		m_sPlayMd5 = "";
		SItemWnd::m_nPlayingID = 0;
	}    
	
    ReListWnd();
}

void SListDlg::ClearInvalid()
{
	void* pVoid_Local = NULL;
	CLeDB::GetInstance()->Local()->SelectLocalFileList(&pVoid_Local);
	if (NULL == pVoid_Local)
		return;

	vector<sLocalFileItem*>* pVec_Local = (vector<sLocalFileItem*>*)pVoid_Local;

    for each (auto var in *pVec_Local)
    {
        if (!PathFileExistsA(var->sPath.c_str()))
		{
			CLeDB::GetInstance()->Local()->DelLocalFileItem(var->sMd5.c_str());
        }
    }
    ReListWnd();
	CLeDB::GetInstance()->ReleaseVecLocal(pVoid_Local);
	return;
}

void SListDlg::OnItemCommand(int nID)
{
    m_sPlayMd5 = "";
	SItemWnd::m_nPlayingID = 0;
    SItemWnd* pItem = FindChildByID2<SItemWnd>(nID);
    if (!pItem)
		return;

    string sMd5 =LeTools::ws2s(pItem->GetItemIndex().GetBuffer(0));	
    SItemWnd::m_nPlayingID = nID;
	
	// 数据库是单字节类型的，为了满足特殊字符，播放列表保存时直接双字节强制转换了，取出可逆
	char* cBuff_Path = CLeDB::GetInstance()->Local()->SelectPath(sMd5.c_str());
	wstring wstrPath =LeTools::ConvertUtf8ToGBKT(cBuff_Path);
	wstring wstrName = wstrPath.substr(wstrPath.rfind(L"\\") + 1);

	// 播放消息统一发给maindlg处理
	sPlayData* pData = new sPlayData;
	pData->eSignal = eSignalType_PlayList;
	pData->eSrcType = eSourceType_Local;
	pData->strSource = wstrPath.c_str();
	pData->sMd5 = pItem->GetItemIndex();
	pData->strName = wstrName.c_str();
	pData->nNum = 0;
	pData->nTotal = 0;
	pData->sRD.videoId =LeTools::ws2s(wstrName.c_str());
	pData->ifConsume = _T("2");
	::PostMessage(CMainDlg::g_hMainWnd, UM_PLAY, (WPARAM)pData, 0);
    
    pItem->SetUserData(0);
    SWindow::Invalidate();
}

void SListDlg::OnBtnAdd()
{
	CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_FILELIST_ADD);
    CRect rcWnd = GetWindowRect();
    CPoint pt;
    pt.x = rcWnd.left;
    pt.y = rcWnd.top;

    SWindow* pWnd = FindChildByName(L"btn_add");
    if (pWnd)
    {
        rcWnd = pWnd->GetWindowRect();
        pt.x += rcWnd.left;
        pt.y += rcWnd.bottom + 10;
    }
    
    SMenuEx menu;
    menu.LoadRes(_T("smenuex:add_menuex"));
    /*
    SMenu menu;
    menu.LoadRes(_T("smenu:add_menu"));
    */
    _HotKey hk;
	CLeDB::GetInstance()->HotKey()->GetHotkey(mapKey[HK_OPENFILE].c_str(),hk);
    menu.SetItemValue(10, L"hotkeyText", hk.GetWString().c_str());
	CLeDB::GetInstance()->HotKey()->GetHotkey(mapKey[HK_OPENFLODER].c_str(), hk);
    menu.SetItemValue(11, L"hotkeyText", hk.GetWString().c_str());
    menu.LoadMenu();
    menu.TrackPopupMenu(0, pt.x, pt.y, m_hWnd);	
}

void SListDlg::OnBtnClear()
{
    CClearDlg dlg;
    BOOL bClear = dlg.DoModel(1);    
	sReportData rd;
	CLeReport::GetInstance()->SendRTD_Play("30", "1", "clear", eSourceType_Local, rd);
	if (bClear)
	{
		DoClearList();
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_FILELIST_CLEAR);
	}
	return;
}

void SListDlg::OnBtnSx()
{
	char chTmp[MAX_PATH] = { 0 };
	string sValue = "0";
	char* pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("playOrder");
	if (NULL != pchValue)
	{
		sValue = pchValue;
		CLeDB::GetInstance()->ReleaseStr(pchValue);
	}
	
    int nOrder = atoi(sValue.c_str());
   
    CRect rcWnd = GetWindowRect();
    CPoint pt;
    pt.x = rcWnd.left;
    pt.y = rcWnd.top;

    SWindow* pWnd = FindChildByName(L"btn_sx");
    if (pWnd)
    {
        rcWnd = pWnd->GetWindowRect();
        pt.x += rcWnd.left;
        pt.y += rcWnd.bottom + 10;
    }

    
    SMenuEx menu;
    menu.LoadRes(_T("smenuex:sx_menuex"));
	/*
    SMenu menu;
    menu.LoadRes(_T("smenu:sx_menu"));
	*/
    //menu.SetItemValue(80 + nOrder, L"radio", L"1");
    menu.LoadMenu();
    menu.TrackPopupMenu(0, pt.x, pt.y, m_hWnd);
}

void SListDlg::OnTimer(UINT_PTR nIDEvent)
{
    SetMsgHandled(FALSE);
    switch (nIDEvent)
    {
    case ET_RELOAD_LIST:
    {
        ReListWnd();
        KillTimer(nIDEvent);
        break;
    }
    default:
        break;
    }
}

void SListDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
    SetMsgHandled(FALSE);
    //::PostMessage(CMainDlg::g_hMainWnd, WM_LBUTTONDOWN, 0, 0);//关闭选集窗口
}

void SListDlg::OnCommand(UINT uNotifyCode, int nID, HWND wndCtl)
{
    switch (nID)
    {
	case OPEN_FILE:
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_FILELIST_OPENFILE);
		break;
	case OPEN_FLODER:
		CLeReport::GetInstance()->SaveData_Count(ERD_COUNT_FILELIST_OPENFLODER);
		break;
    case CMD_PLAY:
    {
        SelPaly();
        return;
    }
    case CMD_DEL:
    {
        DelItem();
        return;
    }
    case CMD_DELALL:
    {
        OnBtnClear();
        return;
    }
    case CMD_OPENFLODER://打开文件所在位置
    {
        SItemWnd* pItem = FindChildByID2<SItemWnd>(m_nSelID);
        if (!pItem)return;
        SStringT sIndex = pItem->GetItemIndex();
        string sMd5 =LeTools::ws2s(sIndex.GetBuffer(0));

        string sPath = CLeDB::GetInstance()->Local()->SelectPath(sMd5.c_str());
		sPath =LeTools::ConvertUtf8ToGBK(sPath);
		wstring wsPath =LeTools::s2ws(sPath);
        if (sPath.length())
        {
			wsPath = _T("/select,") + wsPath;
			ShellExecute(nullptr, _T("open"), _T("explorer.exe"), wsPath.c_str(), NULL, SW_SHOWNORMAL);
            //ShellExecuteA(nullptr, "open", "explorer.exe", sPath.c_str(), NULL, SW_SHOWNORMAL);
        }
        return;
    }
    case 200:
    {
        ClearInvalid();
        return;
    }
    default:
        break;
    }

    UINT wParam = (uNotifyCode << 16) | nID;
    ::PostMessage(CPlayerDlg::m_pThis->m_hWnd, WM_COMMAND, wParam, 3);
}

void SListDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    SetMsgHandled(FALSE);
    UINT lParam = (nFlags << 16) | nRepCnt;
    ::PostMessage(CPlayerDlg::m_pThis->m_hWnd, WM_KEYDOWN, nChar, lParam);
}

LRESULT SListDlg::WndPro(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case UM_UPDATE_SKIN:
    {
        SWindow* pWnd = FindChildByName(L"btn_sx");
        if (pWnd)
        {
            SStringT strSkin = L"loop_sequentialplay";
            SStringT strTip = L"顺序播放";
			char chTmp[MAX_PATH] = { 0 };
			string sValue = "0";
			char* pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("playOrder");
			if (NULL != pchValue)
			{
				sValue = pchValue;
				CLeDB::GetInstance()->ReleaseStr(pchValue);
			}			
            int nOrder = atoi(sValue.c_str());
            switch (nOrder)
            {
            case 0:
            {
                strSkin = L"loop_sequentialplay";
                strTip = L"顺序播放";				
                break;
            }
            case 1:
            {
                strSkin = L"loop_randomcycle";
                strTip = L"随机播放";				
                break;
            }
            case 2:
            {
                strSkin = L"loop_singleplay";
                strTip = L"单个播放";				
                break;
            }
            case 3:
            {
                strSkin = L"loop_singlecycle";
                strTip = L"单个循环";				
                break;
            }
            case 4:
            {
                strSkin = L"loop_listcycle";
                strTip = L"列表循环";				
                break;
            }
            default:
                break;
            }
            pWnd->SetAttribute(L"skin", strSkin);
            pWnd->SetAttribute(L"tip", strTip);
        }
    }
	break;
    case UM_LIST_RBUTTONDOWN:
    {
        m_nSelID = wParam;
        CPoint point = CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        CRect rcWnd;
        ::GetWindowRect(m_hWnd, rcWnd);

        SItemWnd* pItem = FindChildByID2<SItemWnd>(m_nSelID);
        if (!pItem)
			break;
        SMenuEx menu;
		BOOL bPlaying = SItemWnd::m_nPlayingID == m_nSelID ? TRUE : FALSE;
		if (bPlaying && CPlayerDlg::GetPlayStatus() == ePS_Playing)
		{
			menu.LoadRes(_T("smenuex:list_menu_Playgingex"));
		}
		else
		{
			menu.LoadRes(_T("smenuex:list_menuex"));
		}
        
        /*
        SMenuEx menu;
        menu.LoadRes(_T("smenuex:list_menuex"));
        menu.SetWidth(188, 103);
        */
		
		
        //清空列表项
        //if (m_vecListData.size()) 
        {
            menu.SetItemValue(CMD_DELALL, L"disable", L"0");
            menu.SetItemValue(200, L"disable", L"0");
        }
        //删除项
        if (wParam) {
            menu.SetItemValue(CMD_DEL, L"disable", L"0");
        }
        SStringT strIndex = pItem->GetItemIndex();
        string sMd5 =LeTools::ws2s(strIndex.GetBuffer(0));
        string sPath = CLeDB::GetInstance()->Local()->SelectPath(sMd5.c_str());
        if (PathFileExistsA(sPath.c_str()))
        {
            menu.SetItemValue(CMD_PLAY, L"disable", L"0");
            menu.SetItemValue(CMD_OPENFLODER, L"disable", L"0");
        }
        _HotKey hk;
		CLeDB::GetInstance()->HotKey()->GetHotkey(mapKey[HK_OPENFILE].c_str(), hk);
        menu.SetItemValue(10, L"hotkeyText", hk.GetWString().c_str());
		CLeDB::GetInstance()->HotKey()->GetHotkey(mapKey[HK_OPENFLODER].c_str(), hk);
        menu.SetItemValue(11, L"hotkeyText", hk.GetWString().c_str());
		
		char chTmp[MAX_PATH] = { 0 };
		string sValue = "0";
		char* pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("playOrder");
		if (NULL != pchValue)
		{
			sValue = pchValue;
			CLeDB::GetInstance()->ReleaseStr(pchValue);
		}		
        int nOrder = atoi(sValue.c_str());
        menu.SetItemValue(80 + nOrder, L"radio", L"1");

		pchValue = CLeDB::GetInstance()->Set()->GetSingleSet("noMark");
		if (NULL != pchValue)
		{
			sValue = pchValue;
			CLeDB::GetInstance()->ReleaseStr(pchValue);
		}
		
        if (atoi(sValue.c_str()))
            menu.SetItemValue(18, L"radio", L"1");
        else
            menu.SetItemValue(19, L"radio", L"1");

        menu.LoadMenu();
        menu.TrackPopupMenu(0, rcWnd.left + point.x, rcWnd.top + point.y, m_hWnd);
    }break;
    case UM_REDRAW_LISTWND:
    {
		m_status = ePlayStatus(wParam);
        ReListWnd();
    }
	break;
	case UM_REDRAW_LISTWND_NOSORT:
	{
		m_sPlayMd5 = (char *)wParam;
		UpdateListItemTimer();
	}
	break;
    default:
        break;
    }
    return 0;
}
