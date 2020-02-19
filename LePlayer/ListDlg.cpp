#include "stdafx.h"
#include "ListDlg.h"
#include "PlayerDlg.h"
#include "ClearDlg.h"
#include "MediaInfoDLL.h"
#include <regex>
using namespace MediaInfoDLL;

CAVPlayer m_Player;
extern HWND g_hWnd;
HWND g_hListWnd = NULL;
extern _Set g_set;
extern wstring g_strConfigPath;
extern int g_nShowChild;
extern int g_nShowStart;
extern int g_nHoverID;
extern int g_nPlayingID;
extern map<int, _HotKey*> g_mapHotkey;
extern mutex mutex_tj;
extern wstring g_strBaseUrl;
extern long g_lDefaultTime;
extern SStringT g_strToken;
extern string g_sDbPath;
mutex mtx_time;
mutex mtx_update;
mutex mtx_play;
mutex mtx_list;
mutex mtx_db;
extern wstring g_strSmartHome;

#define ITEM_HEIGHT 49
CListDlg::CListDlg() : SHostWnd(_T("LAYOUT:XML_LISTWND"))
{
    m_bLayoutInited = FALSE;
    m_strPlaypath = _T("");
    m_hEvent = CreateEvent(NULL,TRUE,FALSE, NULL);
    m_hDbEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
    m_bNoprompt = FALSE;
    m_bButtondown = FALSE;
    m_nDownID = 0;
    m_nPos = 0;
    m_nListStatus = 0;
    m_llNumber = 0;
    m_bSwitchTimer = 0;
    m_bSwitchTimer = FALSE;
    m_bFirstItem = FALSE;
    m_nOldHoverID = 0;

    thread th = thread(&CListDlg::InsertThread, this);
    th.detach();
}

CListDlg::~CListDlg()
{
}

void CListDlg::UpdateList(BOOL bLocal)
{
    mtx_update.lock();
    if (bLocal)
        Write2Disk();//记录信息到本地
    
    SListWnd* pLWnd = FindChildByName2<SListWnd>(L"play_list");
    if (!pLWnd) 
    {
        SWindow::Invalidate();
        mtx_update.unlock();
        return;
    }

    //清空原列表
    pLWnd->Clear();
    //如果列表为空，不再处理
    if (m_vecListData.size() == 0)
    {
        //重置上下一个等按钮可用状态
        UpdateBtnStatus();
        //禁用清空按钮
        SWindow* pWnd = FindChildByName(L"btn_clear");
        if (pWnd) pWnd->EnableWindow(0);
        ::PostMessage(CPlayerDlg::m_pThis->m_hWnd, UM_ENABLE_STOP, 0, 0);
        SWindow::Invalidate();
        mtx_update.unlock();
        return;
    }
    //取消清空按钮禁用
    SWindow* pWnd = FindChildByName(L"btn_clear");
    if (pWnd) pWnd->EnableWindow(1);
    //更新新列表信息
    SStringT strChild = L"";
    BOOL bShowBtn;//是否显示继续按钮
    fItem* file;
    BOOL bMaxSize = TRUE;
    //判断是否使用最大列表宽度
    int nHeight = pLWnd->GetWindowRect().Height();
    if (m_vecListData.size()*ITEM_HEIGHT - 1>nHeight) {
        bMaxSize = FALSE;
    }
    m_bFirstItem = TRUE;
    for (size_t i = 0; i < m_vecListData.size(); i++)
    {
        file = m_vecListData[i];
        int nStatus = 0;
        if (file->vecChild.size())nStatus = 1;
        //添加一级item
        if (!MakeItemString(nStatus, file, bMaxSize, strChild))continue;
        //添加二级item
        if (file->bShowChild&&file->vecChild.size())
        {
            nStatus = 2;
            for (size_t iChild = 0; iChild < file->vecChild.size(); iChild++)
            {
                fItem* fcld = file->vecChild[iChild];
                if (!MakeItemString(nStatus, fcld, bMaxSize, strChild)) continue;
            }
        }
    }
    pLWnd->ResetChild(strChild);
    //如果当前正在播放，需要突出显示播放项
    g_nPlayingID = Path2ID(m_strPlaypath);//获取播放项ID
    if (g_nPlayingID)
    {
        ReDrawItem(g_nPlayingID,TRUE);
    }
    //重置上下一个等按钮可用状态
    UpdateBtnStatus();
    SWindow::Invalidate();
    mtx_update.unlock();
}

/*
函数功能：重绘Item
nID：item的ID
bPlay：当前ID正在播放与否
*/
void CListDlg::ReDrawItem(int nID, BOOL bPlay)
{
    SItemWnd* pItem = FindChildByID2<SItemWnd>(nID);
    fItem* file = GetFileinfoFormID(nID);
    if (!file) return;

    if (pItem)
    {
        //修改文字颜色
        SStringT strColor = bPlay == TRUE ? L"#40c2fd" : L"#989898";
        pItem->SetAttribute(L"colorText", strColor);
        if (bPlay)
        {
            SStatic* pText = pItem->FindChildByName2<SStatic>(L"playstates");
            if (pText)
            {
                pText->SetAttribute(L"show", L"1");
                pText->SetWindowTextW(L"播放中");
            }
        }
        else
        {
            //是否显示继续按钮
            /******20170808修改，只用在hover状态下显示按钮*******/
            //if (file->lPlayedTime == 0)
                ReShowTime(pItem, file->lTime);
        }
        /*******20170808修改，只用在hover状态下显示按钮*******
        //继续按钮是否显示
        BOOL bShowBtn = FALSE;
        if (file->lPlayedTime != 0 && !bPlay) bShowBtn = TRUE;
        SImageButton1* pBtn = pItem->FindChildByName2<SImageButton1>(L"btn_jx");
        SStringT strShowBtn = bShowBtn == TRUE ? L"1" : L"0";
        if (pBtn)pBtn->SetAttribute(L"show", strShowBtn);
        */
    }
    
    //如果是子item那么需要修改父item显示状态
    if (file->nParentID)
    {
        //找到父项并修改状态
        for each (auto var in m_vecListData)
        {
            if (var->nID == file->nParentID)
            {
                pItem = FindChildByID2<SItemWnd>(var->nID);
                SStringT strColor = bPlay == TRUE ? L"#40c2fd" : L"#989898";
                if (pItem) pItem->SetAttribute(L"colorText", strColor);
                break;
            }
        }
    }
}

BOOL CListDlg::MakeItemString(int nCode, fItem* file,BOOL bMaxSize, SStringT& strList)
{
    //判断是否是需要显示的列表分类
    if (m_nListStatus != 0 && m_nListStatus != file->nCode) {
        return FALSE;
    }
    BOOL bShowBtn = FALSE;
    
    //不记录播放进度
    if ((file->nCode == 1 && !g_set.m_bOnlineProgress) ||
        (file->nCode == 2 && !g_set.m_bLocalProgress))
    {
        file->lPlayedTime = 0;
    }
    /**20170808修改，只用在hover状态下显示按钮**
    //是否显示继续按钮
    if (file->lPlayedTime > 0)bShowBtn = TRUE;
    */
    strList += SListWnd::GetChildString(nCode, file->nID, file->strPath, !bShowBtn,
        file->lTime, bShowBtn, bMaxSize, file->bShowChild,m_bFirstItem);
    m_bFirstItem = FALSE;
    return TRUE;
}

/*获取文件时长*/
void CListDlg::GetTimeThread()
{
    SStringT strPath;
    while (true)
    {
        WaitForSingleObject(m_hEvent, INFINITE);
  
        while (m_vecInsert.size())
        {
            mtx_time.lock();
            auto var = m_vecInsert.begin();
            if (var == m_vecInsert.end()) {
                mtx_time.unlock();
                continue;
            }
            strPath = *var;
            m_vecInsert.erase(var);
            mtx_time.unlock();
            /*
            STimeDlg dlg;
            int nTime = dlg.DoModel(strPath);
            */
            __int64 nTime = GetTotalTime(strPath)/1000;
            m_mapFilesTime[strPath] = nTime;
            SetTime(strPath, nTime);
        }
        ResetEvent(m_hEvent);
    }
}

void CListDlg::InsertList(SStringT strPath,BOOL bPlay /*= FALSE*/)
{
    //保存到数据库
    InsertDB(QwBase::ws2s(strPath.GetBuffer(0)));

    if (bPlay)
        m_strPlaypath = strPath;
    //获取相似度字符串
    SStringT strShort = strPath.Mid(strPath.ReverseFind('\\')+1);
    strShort = strShort.Mid(0, strShort.ReverseFind('.'));
    strShort = GetString(strShort);

    //查找是否有可以放入同一个专辑
    int nParentID = 0;
    fItem* fParent = nullptr;
    for each (auto var in m_vecListData)
    {
        //文件已添加不在处理
        if (strPath.Compare(var->strPath) == 0) return;
    }
    fParent = nullptr;
    for each (auto var in m_vecListData)
    {
        //找到可编入同一专辑的文件
        if (strShort.Compare(var->strShort)==0&& nParentID==0)
        {
            nParentID = var->nID;
            fParent = var;
        }
        //继续在子项中查找文件是否以添加
        if (var->vecChild.size())
        {
            for (size_t i = 0; i < var->vecChild.size(); i++)
            {
                fItem* fChild = var->vecChild[i];
                //文件已添加不在处理
                if (strPath.Compare(fChild->strPath) == 0) return;
            }
        }
        else
        {
            //已找到同一专辑文件，跳出查找
            if (fParent)break;
        }
    }

    //生成新文件相关信息
    fItem* file = new fItem;
    file->nCode = 2;//默认为添加本地文件
    file->nID = ++m_llNumber;
    file->nParentID = nParentID;
    file->strPath = strPath;
    file->strShort = strShort;
    //获取视频文件时长
    auto iter = m_mapFilesTime.find(strPath);
    if (iter != m_mapFilesTime.end())
    {
        file->lTime = iter->second;//已获取到直接使用
    }
    else
    {
        //获取时长
        mtx_time.lock();
        m_vecInsert.push_back(strPath);
        mtx_time.unlock();
        SetEvent(m_hEvent);
    }
    //如果原列表中无信息，需要重置清理按钮可用状态
    if (m_vecListData.size() == 0)
    {
        SWindow* pWnd = FindChildByName(L"btn_clear");
        if (pWnd) pWnd->EnableWindow(1);
    }
    //加入到列表中

    if (fParent){
        //文件加入到专辑中
		//专辑未创建需要创建专辑，即将已存在的文件自加一次作为专辑第一项
        if (fParent->vecChild.size() == 0) {
            fItem* fileP = new fItem;
            fileP->Copy(*fParent);
            fileP->nID = ++m_llNumber;
            fileP->nParentID = fParent->nID;
            fParent->vecChild.push_back(fileP);
        }
        fParent->vecChild.push_back(file);
    }
    else
    {
        m_vecListData.push_back(file);
    }
}

void CListDlg::InsertDB(string path)
{
    mtx_db.lock();
    m_vecData.push_back(path);
    mtx_db.unlock();
    SetEvent(m_hDbEvent);
}

void CListDlg::InsertThread()
{
    while (true)
    {
        WaitForSingleObject(m_hDbEvent, INFINITE);
        while (m_vecData.size())
        {
            string sPath = "";
            mtx_db.lock();
            auto var = m_vecData.begin();
            if (var == m_vecData.end()) {
                mtx_db.unlock();
                continue;
            }
            sPath = *var;
            m_vecData.erase(var);
            mtx_db.unlock();
            //判断文件是否存在
            if (!PathFileExistsA(sPath.c_str()))continue;
            //获取文件名
            string sName = sPath.substr(sPath.rfind("\\")+1);
            sName = sName.substr(0, sName.rfind("."));
            //获取文件MD5值
            MD5 md5(sPath);
            string sMd5 = md5.toString();
            //获取视频总时长，单位秒
            long lTotalTime = GetTotalTime(QwBase::s2ws(sPath).c_str())/1000;
            m_sql.InsertPlayRecod(sName,0,lTotalTime,sPath,sMd5,2,0);
        }
    }
}

void CListDlg::Stop()
{
    ReDrawItem(g_nPlayingID,FALSE);
    g_nPlayingID = 0;
    m_strPlaypath = L"";
}

void CListDlg::UpdateBtnStatus()
{
    int nMaxNum = GetMaxNumber();
    //列表为空||单个播放||顺序播放且列表为1
    if (nMaxNum ==0||g_set.m_nPlayStyle==2||
        (g_set.m_nPlayStyle==0&& nMaxNum ==1))
    {
        ::PostMessage(g_hWnd,UM_LAST_NEXT_ENABLE,0,0);
        return;
    }

    int nIndex = GetIndexFormID(g_nPlayingID);

    if ( g_set.m_nPlayStyle == 1 
        || g_set.m_nPlayStyle == 4 
        || g_set.m_nPlayStyle == 3
        || (g_set.m_nPlayStyle==0&& nIndex>1&& nIndex<nMaxNum))
    {
        ::PostMessage(g_hWnd, UM_LAST_NEXT_ENABLE, 1, 1);
        return;
    }

    if (g_set.m_nPlayStyle == 0&& nIndex <=1)
    {
        ::PostMessage(g_hWnd, UM_LAST_NEXT_ENABLE, 0, 1);
        return;
    }

    if (g_set.m_nPlayStyle == 0 && nIndex <= nMaxNum)
    {
        ::PostMessage(g_hWnd, UM_LAST_NEXT_ENABLE, 1, 0);
        return;
    }
}

void CListDlg::Write2Disk()
{
    return;
    wstring wsPath = g_strConfigPath.substr(0, g_strConfigPath.rfind(L"\\") + 1);
    wsPath += L"record.db";

    //清空播放记录
    if (g_set.m_bClearPlayList){
        DeleteFileW(wsPath.c_str());
        return;
    }
    //记住播放进度
    Json::Reader jr;
    Json::Value jvData;
    Json::Value jvItem;
    jr.parse(LIST_JSON, jvItem);
    for (size_t i = 0; i < m_vecListData.size(); i++)
    {
        fItem* file = m_vecListData[i];
		if (file->vecChild.size())
		{
            //专辑主项不记录,记录子项
            for (size_t i = 0; i < file->vecChild.size(); i++)
            {
                fItem* fChild = file->vecChild[i];
                jvItem["code"] = fChild->nCode;
                jvItem["path"] = QwBase::ws2s(fChild->strPath.GetBuffer(0));
                jvItem["string"] = QwBase::ws2s(fChild->strShort.GetBuffer(0));
                jvItem["totalltime"] = fChild->lTime;
                jvItem["played"] = fChild->lPlayedTime;
                jvData.append(jvItem);
            }
			continue;
		}
        jvItem["code"] = file->nCode;
        jvItem["path"] = QwBase::ws2s(file->strPath.GetBuffer(0));
        jvItem["string"] = QwBase::ws2s(file->strShort.GetBuffer(0));
        jvItem["totalltime"] = file->lTime;
        jvItem["played"] = file->lPlayedTime;
        jvData.append(jvItem);
    }
    Json::FastWriter writer;
    std::string sData = writer.write(jvData);

    DeleteFileW(wsPath.c_str());

    FILE* file;
    _wfopen_s(&file, wsPath.c_str(),L"wb");
    if (file){
        fwrite(sData.data(), sData.size(),1,file);
        fclose(file);
    }
}

void CListDlg::ReadRecord()
{
    vector<_PlayRecod> vec;
    m_sql.SelectPlayRecod(vec);

    for (size_t i = 0; i < vec.size(); i++)
    {
        _PlayRecod rec = vec[i];
        int nCode = rec.nType+1;
        string strPath = rec.sPath;
        SStringT sPath = QwBase::s2ws(strPath).c_str();
        SStringT strShort = sPath.Mid(sPath.ReverseFind('\\') + 1);
        strShort = strShort.Mid(0, strShort.ReverseFind('.'));
        strShort = GetString(strShort);

        string str = QwBase::ws2s(strShort.GetBuffer(0));

        int nTotallTime = rec.lTotalTime;
        __int64 llPlayed = rec.lCurrPlayTime;
        AddList(nCode,strPath, str, nTotallTime, llPlayed);
    }
    UpdateList(FALSE);
    return;

    wstring wsPath = g_strConfigPath.substr(0, g_strConfigPath.rfind(L"\\") + 1);
    wsPath += L"record.db";
    FILE* file;
    _wfopen_s(&file, wsPath.c_str(), L"rb");
    if (!file) return;
    _fseeki64(file, 0, SEEK_END);
    __int64 llSize = _ftelli64(file);
    _fseeki64(file, 0, SEEK_SET);
    char* cData = (char*)malloc(llSize + 1);
    memset(cData, 0, llSize + 1);
    fread(cData, llSize,1,file);
    fclose(file);

    Json::Reader jr;
    Json::Value jvData;
    if (jr.parse(cData, jvData))
    {
        for (size_t i = 0; i < jvData.size(); i++)
        {
            Json::Value& jvItem = jvData[i];
            int nCode = jvItem["code"].asInt();
            string strPath = jvItem["path"].asString();
            string sStr = jvItem["string"].asString();
            int nTotallTime = jvItem["totalltime"].asInt();
            __int64 llPlayed = jvItem["played"].asInt64();
            AddList(nCode, strPath, sStr, nTotallTime, llPlayed);
        }
    }
    free(cData);

    UpdateList(FALSE);
}

void CListDlg::SetPlayed(SStringT strPath, __int64 llPlayed)
{
    int nID = Path2ID(strPath);
    fItem* file = GetFileinfoFormID(nID);
    if (!file)return;

    if (g_set.m_bLocalProgress&&file->nCode==2)
        file->lPlayedTime = llPlayed;

    if (g_set.m_bOnlineProgress&&file->nCode == 1)
        file->lPlayedTime = llPlayed;    
}

long CListDlg::GetPlayed(SStringT strPath)
{
    int nID = Path2ID(strPath);
    fItem* file = GetFileinfoFormID(nID);
    if (!file)return 0;

    if ((file->nCode == 1 && g_set.m_bOnlineProgress) ||
        (file->nCode == 2 && g_set.m_bLocalProgress))
    {
        return file->lPlayedTime;
    }
    return 0;
}

void CListDlg::Play(int nID)
{
    fItem* file = GetFileinfoFormID(nID);
    if (!file)return;
    //点击的是专辑
    if (file->vecChild.size()){
        //设置专辑播放状态
        ReDrawItem(nID, TRUE);
        BOOL bFind = FALSE;
        //由于点击的是专辑标题，则从专辑第一个或者专辑已播放开始播放
        if (g_set.m_bStartFromPlayed && !file->strAlbumplayed.IsEmpty())
        {
            SStringT strAlbumplayed = file->strAlbumplayed;

            for (size_t i = 0; i < file->vecChild.size(); i++)
            {
                fItem* fChild = file->vecChild[i];
                if (fChild->strPath.Compare(strAlbumplayed) == 0)
                {
                    bFind = TRUE;
                    nID = fChild->nID;
                    file = fChild;
                    break;
                }
            }
        }
        //未找到专辑已播放，默认从第一个开始播放
        if (!bFind)
        {
            file = file->vecChild[0];
            nID = file->nID;
        }
    }
    //文件如果已经不存在，则不在播放
    if (!PathFileExists(file->strPath)) return;

    SStringT strPath = file->strPath;
    g_nPlayingID = nID;
    CPlayerDlg::m_pThis->PlayFile(strPath);
    //修改上一个播放显示状态
    int nOldId = Path2ID(m_strPlaypath);
    if (nOldId != g_nPlayingID)
        ReDrawItem(nOldId, FALSE);
    //修改播放文件项播放状态

    ReDrawItem(nID, TRUE);
    
    m_strPlaypath = strPath;
    //改变上下一个按钮状态
    UpdateBtnStatus();

     //记录专辑已播放
    if (g_set.m_bStartFromPlayed&&file->nParentID!=0)
    {
        for each (auto var in m_vecListData)
        {
            if (var->nID== file->nParentID)
            {
                var->strAlbumplayed = strPath;
            }
        }
    }
}

int CListDlg::Path2ID(SStringT strPath)
{
    fItem* file;
    for (size_t i = 0; i < m_vecListData.size(); i++)
    {
        file = m_vecListData[i];
        if (file->vecChild.size() ==0 && strPath.Compare(file->strPath)==0)
        {
            return file->nID;
        }
        if (file->vecChild.size())
        {
            for each (auto var in file->vecChild)
            {
                if (strPath.Compare(var->strPath) == 0) return var->nID;
            }
        }
    }
    return 0;
}

int CListDlg::Path2ParentID(SStringT strPath)
{
    int nId = Path2ID(strPath);
    fItem* file = GetFileinfoFormID(nId);
    if (!file)return 0;
    return file->nParentID;
}

void CListDlg::SetTime(SStringT strPath,int nTime)
{
    int nID = Path2ID(strPath);
    fItem* file = GetFileinfoFormID(nID);
    if (file)
        file->lTime = nTime;
    
    SItemWnd* pItem = FindChildByID2<SItemWnd>(nID);
    if (pItem&&strPath.Compare(m_strPlaypath)!=0)
        ReShowTime(pItem, nTime);
}

fItem * CListDlg::GetFileinfoFormID(int nID)
{
    for each (auto var in m_vecListData)
    {
        if (var->nID == nID)
        {
            return var;
        }
        if (var->vecChild.size())
        {
            for each (auto fChild in var->vecChild)
            {
                if (fChild->nID == nID)
                {
                    return fChild;
                }
            }
        }
    }
    return nullptr;
}

void CListDlg::ReShowTime(SItemWnd* pItem,int nTime)
{
    if (!pItem) return;

    WCHAR wBuff[128] = { 0 };
    int nSec = nTime % 60;
    int nMin = nTime / 60;
    wsprintf(wBuff, L"%02d:%02d", nMin,nSec);
    SWindow* pWnd = pItem->FindChildByName(L"playstates");
    if (pWnd) pWnd->SetWindowTextW(wBuff);
}

void CListDlg::SortList(int nID)
{
    if (m_nDownID == 0 || nID == 0 || m_nDownID == nID) return;
    fItem* fDown = GetFileinfoFormID(m_nDownID);
    fItem* fUp = GetFileinfoFormID(nID);
    if (!fDown || !fUp)return;
    int nDownIndex = GetIndexFormID(m_nDownID);
    int nUpIndex = GetIndexFormID(nID);
    //主item不能拖入子item中，子item不能移动到不同的主item中
    if ((fDown->nParentID==0&& fUp->nParentID)||(fDown->nParentID!= fUp->nParentID))
        return;
    //均在主项中
    if (fDown->nParentID== fUp->nParentID&&fUp->nParentID==0)
    {
        vector<fItem*>::iterator iter = m_vecListData.begin();
        //移除
        for (iter; iter!= m_vecListData.end();)
        {
            if ((*iter)->nID == m_nDownID) {
                iter = m_vecListData.erase(iter);
                break;
            }
            iter++;
        }
        //插入
        iter = m_vecListData.begin();
        for (iter; iter != m_vecListData.end();)
        {
            if ((*iter)->nID == nID) {
                //向下移动时，插入到鼠标点起处之后，否则在之前
                if (nDownIndex < nUpIndex)iter++;
                m_vecListData.insert(iter, fDown);
                break;
            }
            iter++;
        }
    }
    else   //子项
    {
        for each (auto var in m_vecListData)
        {
            if (var->nID == fDown->nParentID)
            {
                vector<fItem*>::iterator iter = var->vecChild.begin();
                //移除
                for (iter; iter != var->vecChild.end();)
                {
                    if ((*iter)->nID == m_nDownID) {
                        iter = var->vecChild.erase(iter);
                        break;
                    }
                    iter++;
                }
                //插入
                iter = var->vecChild.begin();
                for (iter; iter != var->vecChild.end();)
                {
                    if ((*iter)->nID == nID) {
                        //向下移动时，插入到鼠标点起处之后，否则在之前
                        if (nDownIndex < nUpIndex)iter++;
                        var->vecChild.insert(iter, fDown);
                        break;
                    }
                    iter++;
                }
            }
        }
    }

	SListWnd* pList = FindChildByName2<SListWnd>(L"list");
	if(pList) pList->SaveOldViewOrigin();
    UpdateList(FALSE);
	if (pList) pList->UpdateViewOrigin();
}

void CListDlg::AddList(int nCode, string strPath, string sStr, int nTotallTime, __int64 llPlayed)
{
    SStringT sPath = QwBase::s2ws(strPath).c_str();
    SStringT sString = QwBase::s2ws(sStr).c_str();
    int nParent = 0;
    fItem* fParent = nullptr;
    for each (auto var in m_vecListData)
    {
        //文件已添加
        if (sPath.Compare(var->strPath) == 0)
            return;
        for each (auto fChild in var->vecChild)
        {
            //文件已添加
            if (sPath.Compare(fChild->strPath) == 0)
                return;
        }
        if (sString.Compare(var->strShort)==0)
        {
            nParent = var->nID;
            fParent = var;
        }
    }

    fItem* file = new fItem;
    file->nCode = 2;
    file->strPath = sPath;
    file->nID = ++m_llNumber;
    file->nParentID = nParent;
    file->strShort = sString;
    file->lTime = nTotallTime;
    file->lPlayedTime = llPlayed;
    if (nTotallTime == 0)
    {
        mtx_time.lock();
        m_vecInsert.push_back(sPath);
        mtx_time.unlock();
        SetEvent(m_hEvent);
    }

    if (m_vecListData.size() == 0)
    {
        SWindow* pWnd = FindChildByName(L"btn_clear");
        if (pWnd) pWnd->EnableWindow(1);
    }

    if (fParent)
    {
        if (fParent->vecChild.size() == 0)
        {
            fItem* fileP = new fItem;
            fileP->Copy(*fParent);
            fileP->nID = ++m_llNumber;
            fileP->nParentID = fParent->nID;
            fParent->vecChild.push_back(fileP);
        }
        fParent->vecChild.push_back(file);
    }
    else
        m_vecListData.push_back(file);
}

void CListDlg::Screen2Item()
{
    SListWnd* pList = FindChildByName2<SListWnd>(L"list");
    SItemWnd* pItem = FindChildByID2<SItemWnd>(g_nPlayingID);
    if (pList&&pItem)
    {
        CRect rcItem = pItem->GetWindowRect();
        CRect rcList = pList->GetWindowRect();
        if (rcItem.bottom>rcList.bottom)
        {
            int nHeight = g_nPlayingID*ITEM_HEIGHT;
            int nPos = nHeight - pList->GetPageHeght();
            nPos = nPos < 0 ? 0 : nPos;
            pList->SetScrollPos(nPos);
        }
        if (rcItem.top < rcList.top)
        {
            int nPos = (g_nPlayingID - 1)*ITEM_HEIGHT;
            pList->SetScrollPos(nPos);
        }
    }
}

int CListDlg::IsSortList(CPoint point)
{
    int nID = 0;
    //鼠标为点下
    if (!m_bButtondown) return nID;
    m_bButtondown = FALSE;

	CRect rcWnd = CRect(0,0,0,0);
	SWindow* pWndl = FindChildByName(L"list");
	if(pWndl) rcWnd = pWndl->GetWindowRect();
    if (!PtInRect(&rcWnd, point)) return nID; //鼠标在列表外不处理
    SItemWnd* pWnd;
    for each (auto var in m_vecListData)
    {
        pWnd = FindChildByID2<SItemWnd>(var->nID);
        if (pWnd){
            CRect rcWnd = pWnd->GetWindowRect();
            if (PtInRect(&rcWnd, point))
            {
                nID = var->nID;
                goto SORTEND;
            }
        }
        //检查需要显示的子项
        if (var->bShowChild)
        {
            for each (auto fChild in var->vecChild)
            {
                pWnd = FindChildByID2<SItemWnd>(fChild->nID);
                if (pWnd) {
                    CRect rcWnd = pWnd->GetWindowRect();
                    if (PtInRect(&rcWnd, point))
                    {
                        nID = fChild->nID;
                        goto SORTEND;
                    }
                }
            }
        }

    }
    //未找到，判断是否在列表最后
    if (!nID)
    {
        int nMax = GetMaxNumber();
        fItem* file = GetItemFormIndex(nMax);
        pWnd = FindChildByID2<SItemWnd>(file->nID);
        if (pWnd)
        {
            CRect rcWnd = pWnd->GetWindowRect();
            if (point.y >= rcWnd.top)
                nID = file->nID;
        }
    }

SORTEND:
    if (nID == m_nDownID)
        nID = 0;
    return nID;
}

void CListDlg::BtnSwitch(int nID)
{
    fItem* file = GetFileinfoFormID(nID);
    if (!file || !file->vecChild.size())return;
    file->bShowChild = !file->bShowChild;
    SListWnd* pList = FindChildByName2<SListWnd>(L"list");
    if (pList) pList->SaveOldViewOrigin();
    UpdateList(FALSE);
    if (pList) pList->UpdateViewOrigin();
}

void CListDlg::DelItem(int nID)
{
    BOOL bClear = TRUE;
    if (!m_bNoprompt)
    {
        CClearDlg dlg;
        bClear = dlg.DoModel(0);
        m_bNoprompt = dlg.IsNoprompt();
    }
    if (!bClear) return;

    //获取当前正在播放视频列表ID
    int nPlayingID = Path2ID(m_strPlaypath);

    //如果删除的是正在播放的列表项，则停止播放
    if (nPlayingID == nID)
        CPlayerDlg::m_pThis->DoStop();
    else
    {
        //如果正在播放的视频包含在删除项中，需要停止播放
        nPlayingID = Path2ParentID(m_strPlaypath);
        if (nPlayingID == nID)
            CPlayerDlg::m_pThis->DoStop();
    }
    vector<fItem*>::iterator iter = m_vecListData.begin();
    fItem* file;
    BOOL bFind = FALSE;
    for (iter; iter!=m_vecListData.end();)
    {
        file = (*iter);
        if (file->nID==nID)
        {
            m_vecListData.erase(iter);
            for (size_t i = 0; i < file->vecChild.size(); i++)
            {
                string sPath = QwBase::ws2s(file->vecChild[i]->strPath.GetBuffer(0));
                MD5 md5(sPath);
                string sMd5 = md5.toString();
                m_sql.DelPlayRecod(sMd5);
                delete file->vecChild[i];
            }
            string sPath = QwBase::ws2s(file->strPath.GetBuffer(0));
            MD5 md5(sPath);
            string sMd5 = md5.toString();
            m_sql.DelPlayRecod(sMd5);
            delete file;
            bFind = TRUE;
            break;
        }
        fItem* fChild;
        vector<fItem*>::iterator iChild = file->vecChild.begin();
        for (iChild; iChild!=file->vecChild.end();)
        {
            fChild = (*iChild);
            if (fChild->nID == nID)
            {
                file->vecChild.erase(iChild);
                string sPath = QwBase::ws2s(fChild->strPath.GetBuffer(0));
                MD5 md5(sPath);
                string sMd5 = md5.toString();
                m_sql.DelPlayRecod(sMd5);
                delete fChild;
                bFind = TRUE;
                break;
            }
            iChild++;
        }
        if (bFind) break;
        iter++;
    }

    if (m_vecListData.size() == 0) {
		SWindow* pWnd = FindChildByName(L"btn_clear");
		if(pWnd) pWnd->EnableWindow(0);
    }

	SListWnd* pList = FindChildByName2<SListWnd>(L"list");
	if(pList) pList->SaveOldViewOrigin();
    UpdateList();
	if (pList) pList->UpdateViewOrigin();
}

int CListDlg::GetMaxNumber()
{
    int nMax = 0;
    for each (auto var in m_vecListData)
    {
        if (var->vecChild.size())
            nMax += var->vecChild.size();
        nMax++;
    }
    return nMax;
}

__int64 CListDlg::GetTotalTime(SStringT strPath)
{
    MediaInfo MI;
    String strTime;
    MI.Open(strPath.GetBuffer(0));
    strTime = MI.Get(stream_t::Stream_Video, 0, L"Duration");
    if (strTime.length()==0)
        strTime = MI.Get(stream_t::Stream_Audio, 0, L"Duration");
    MI.Close();
    return _wtoi64(strTime.c_str());
}

void CListDlg::ResetTimes(SStringT strPath, long lTime)
{
    if (m_mapFilesTime[strPath]==0)
    {
        m_mapFilesTime[strPath] = lTime;
        SetTime(strPath, lTime);
    }
}

void CListDlg::DrawItemNoPlay(SStringT strPath)
{
    long lID = Path2ID(strPath);
    ReDrawItem(lID,FALSE);
}

void CListDlg::DelItemOfPath(string sPath)
{
    int nID = Path2ID(QwBase::s2ws(sPath).c_str());

    //获取当前正在播放视频列表ID
    int nPlayingID = Path2ID(m_strPlaypath);

    //如果删除的是正在播放的列表项，则停止播放
    if (nPlayingID == nID)
        CPlayerDlg::m_pThis->DoStop();
    else
    {
        //如果正在播放的视频包含在删除项中，需要停止播放
        nPlayingID = Path2ParentID(m_strPlaypath);
        if (nPlayingID == nID)
            CPlayerDlg::m_pThis->DoStop();
    }

    vector<fItem*>::iterator iter = m_vecListData.begin();
    fItem* file;
    BOOL bFind = FALSE;
    for (iter; iter != m_vecListData.end();)
    {
        file = (*iter);
        if (file->nID == nID)
        {
            m_vecListData.erase(iter);
            for (size_t i = 0; i < file->vecChild.size(); i++)
            {
                delete file->vecChild[i];
            }
            delete file;
            bFind = TRUE;
            break;
        }
        fItem* fChild;
        vector<fItem*>::iterator iChild = file->vecChild.begin();
        for (iChild; iChild != file->vecChild.end();)
        {
            fChild = (*iChild);
            if (fChild->nID == nID)
            {
                file->vecChild.erase(iChild);
                delete fChild;
                bFind = TRUE;
                break;
            }
            iChild++;
        }
        if (bFind) break;
        iter++;
    }

    if (m_vecListData.size() == 0) {
        SWindow* pWnd = FindChildByName(L"btn_clear");
        if (pWnd) pWnd->EnableWindow(0);
    }

    SListWnd* pList = FindChildByName2<SListWnd>(L"list");
    if (pList) pList->SaveOldViewOrigin();
    UpdateList();
    if (pList) pList->UpdateViewOrigin();

}

int CListDlg::GetIndexFormID(long nID)
{
    int nCount = 0;
    for each (auto var in m_vecListData)
    {
        nCount++;
        if (var->nID == nID)
            return nCount;
        for each (auto cld in var->vecChild)
        {
            nCount++;
            if (cld->nID == nID)
                return nCount;
        }
    }
    return 0;
}

fItem * CListDlg::GetItemFormIndex(int nIndex)
{
    int nCount = 0;
    for each (auto var in m_vecListData)
    {
        nCount++;
        if (nCount == nIndex)
            return var;
        int nNum = var->vecChild.size();
        if (nNum)
        {
            if (nCount+ nNum<nIndex)
                nCount += nNum;
            else
            {
                for (size_t i = 0; i < nNum; i++)
                {
                    if (++nCount== nIndex)
                        return var->vecChild[i];
                }
            }
        }
    }
    return nullptr;
}

void CListDlg::MoveList(CPoint point)
{
    if (!m_bButtondown)return;
	SListWnd* pList = FindChildByName2<SListWnd>(L"list");
    if (point.y<m_ptMove.x)
    {
		if (pList) pList->MoveScroll(TRUE);
    }
    else if (point.y>m_ptMove.y)
    {
		if (pList) pList->MoveScroll(FALSE);
    }
}

void CListDlg::ClearInvalid()
{
    BOOL bUpdate = FALSE;
    vector<fItem*>::iterator iter = m_vecListData.begin();
    for (; iter!= m_vecListData.end();)
    {
        fItem* file = *iter;
		//非专辑项
		if (file->vecChild.size()==0)
		{
			if (!PathFileExists(file->strPath))
			{
				delete file;
				iter = m_vecListData.erase(iter);
				bUpdate = TRUE;
				continue;
			}
		}
        else//专辑项
        {
            vector<fItem*>::iterator iCld = file->vecChild.begin();
            for (; iCld != file->vecChild.end();)
            {
                fItem* fCld = *iCld;
                if (!PathFileExists(fCld->strPath))
                {
                    delete file;
                    iCld = m_vecListData.erase(iCld);
                    bUpdate = TRUE;
                    continue;
                }
                iCld++;
            }
            //当前专辑全部无效，则删除专辑
            if (file->vecChild.size()==0)
            {
                delete file;
                iter = m_vecListData.erase(iter);
                continue;
            }
            //如果当前专辑只存在一个文件，则专辑取消
            if (file->vecChild.size() == 1)
            {
                fItem* fChild = file->vecChild[0];
                file->Copy(*fChild);
                delete fChild;
            }
        }
        iter++;
    }

    if (bUpdate)
        UpdateList();
}

SStringT CListDlg::GetString(SStringT str)
{
    SStringT sString = str;
    for (size_t i = 0; i < 10; i++)
    {
        sString.Remove(48+i);
    }
	if (sString.IsEmpty()){
		sString = L"number11";
	}
    return sString;
}

SStringT CListDlg::GetGetThumbnailImage(SStringT strUrl, int w, int h,BOOL bFlag)
{
    if (strUrl.GetLength() == 0)return L"";
    WCHAR wBuff[MAX_PATH + 1] = { 0 };
    GetTempPath(MAX_PATH + 1, wBuff);

    wstring wsPath = wBuff;
    wstring wsName = strUrl.GetBuffer(0);
    wsName = wsName.substr(wsName.rfind(L"publish"));
    wsPath.append(wsName);
    while (wsPath.find(L"/")!=wstring::npos)
    {
        wsPath.replace(wsPath.find(L"/"), 1,L"\\");
    }
    string strPath = QwBase::ws2s(wsPath).c_str();
    strPath = strPath.substr(0, strPath.rfind("\\")+1);
    MakeSureDirectoryPathExists(strPath.c_str());

    wstring wsLocalPath = wsPath;
    wsLocalPath = wsLocalPath.substr(0, wsLocalPath.rfind(L"."));
    wsLocalPath.append(L".bmp");
    if (PathFileExists(wsLocalPath.c_str()))
    {
        //文件已存在，直接返回
        if(bFlag)
            SendMessage(UM_SETIMAGE, (WPARAM)wsLocalPath.c_str());
        return wsLocalPath.c_str();
    }

    thread th = thread(&CListDlg::ImageThread,this, strUrl, wsPath.c_str(), wsLocalPath.c_str(),w,h, bFlag);
    th.detach();
    return wsLocalPath.c_str();
}

void CListDlg::ImageThread(SStringT strUrl, SStringT strPath, SStringT strBmp,int w, int h, BOOL bFlag)
{
    HRESULT hRes = URLDownloadToFile(0, strUrl.GetBuffer(0), strPath.GetBuffer(0), 0, 0);
    if (hRes == S_OK)
    {
        Gdiplus::Bitmap bmp(strPath.GetBuffer(0));
        Image* pScaledImage = bmp.GetThumbnailImage(w, h, NULL, NULL);//获取缩略图
        pScaledImage->Save(strBmp.GetBuffer(0), &m_bmpClsid);
        delete pScaledImage;
        DeleteFile(strPath.GetBuffer(0));
        if (bFlag)
            SendMessage(UM_SETIMAGE, (WPARAM)strBmp.GetBuffer(0));
    }
    else
    {
        if (bFlag)
            PostMessage(UM_SETIMAGE);
    }
}

SStringT CListDlg::GetItemString(int nID, POINT pt, SStringT strTitle, SStringT strPath)
{
    SStringT strItem = L"";
    WCHAR wBuff[1024] = { 0 };
    wsprintf(wBuff,L"<imgbtn1 id=\"%d\" pos=\"%d,%d,@130,@80\" \
original=\"0\" focusSkin=\"list_focus\" skin=\"skin_teledefault\" path=\"%s\" cursor=\"hand\"/>\r\n",
nID, pt.x, pt.y, strPath);
    strItem.Append(wBuff);
    strItem.Append(L"\r\n");
    memset(wBuff,0,1024*sizeof(WCHAR));
    wsprintf(wBuff,L"<text pos=\"{0,[0,@130,@22\" colorText=\"#989898\" \
font=\"size:12\">%s</text>", strTitle.GetBuffer(0));
    strItem.Append(wBuff);
    strItem.Append(L"\r\n");
    return strItem;
}

void CListDlg::SetShowvip(BOOL b)
{
    SStringT sStr = b == TRUE ? L"10,[10,@272,@36" : L"10,[10,@0,@0";
    SWindow* pWnd = FindChildByName(L"btn_vip");
    if (pWnd)pWnd->SetAttribute(L"pos", sStr);
}

void CListDlg::GetPlayInfo(SStringT strUrl)
{
    mtx_list.lock();

    CDownloader down;
    string sResult = "";
    string sUrl = MIGU_HTTP_IP;
    sUrl.append("/lenovoTokenLogin.msp?lenovoToken=");
    sUrl.append(QwBase::ws2s(g_strToken.GetBuffer(0)));
    down.DownloadTo(sUrl, &sResult);

    string sData = "";
    down.DownloadTo(QwBase::ws2s(strUrl.GetBuffer(0)),&sData);

    if (sData.length())
    {
        sData = QwBase::urlDecode(sData.c_str());
        sData = QwBase::ConvertUtf8ToGBK(sData);
        char* cBuff = (char*)malloc(sizeof(char)*( sData.length() + 1));
        memset(cBuff,0, sizeof(char)*(sData.length() + 1));
        CopyMemory(cBuff, sData.c_str(), sData.length());
        PostMessage(UM_PLAYINFO,0, (LPARAM)cBuff);
    }
    mtx_list.unlock();
}

void CListDlg::SetPlay(SStringT strUrl)
{
    thread th = thread(&CListDlg::GetPlayInfo, this, strUrl);
    th.detach();
}

BOOL CListDlg::GetPlyaUrl(string sContid, int nPlayID)
{
    if (nPlayID != CPlayerDlg::m_pThis->m_nPlayID) return FALSE;//当前不再播放，跳过

    wstring strUrl = g_strBaseUrl;
    if (strUrl.find(L"?cid=")!=wstring::npos){
        strUrl = strUrl.substr(0, strUrl.find(L"?cid="));
    }
    strUrl.append(L"?cid=");
    strUrl.append(QwBase::s2ws(sContid));


    HttpRequest req(strUrl.c_str(), NULL, NULL);
    BOOL bSucc = req.doGet();

    if (nPlayID != CPlayerDlg::m_pThis->m_nPlayID) return FALSE;//当前不再播放，跳过

    if (bSucc)
    {
        string sData = req.getStream().str();
        sData = QwBase::urlDecode(sData.c_str());
        sData = QwBase::ConvertUtf8ToGBK(sData);
        Json::Reader jr;
        Json::Value jv;
        if (jr.parse(sData, jv))
        {
            if (jv.size() >= 1)
            {
                Json::Value jvData = jv[0];

                string strPlay = jvData["play"].asString();

                Resolution* stResolution = new Resolution;
                //分辨率列表
                Json::Value jvList = jvData["playList"];
                if (jvList["play1"].isString())
                {
                    stResolution->play1 = jvList["play1"].asString();
                }
                if (jvList["play2"].isString())
                {
                    stResolution->play2 = jvList["play2"].asString();
                }
                if (jvList["play3"].isString())
                {
                    stResolution->play3 = jvList["play3"].asString();
                }

                if (jvData["pilotPlay"].isString())
                {
                    stResolution->sPilot = jvData["pilotPlay"].asString();
                }
                jvList = jvData["pilotPlayList"];
                if (jvList["play41"].isString())
                {
                    stResolution->play41 = jvList["play41"].asString();
                }
                if (jvList["play42"].isString())
                {
                    stResolution->play42 = jvList["play42"].asString();
                }
                if (jvList["play43"].isString())
                {
                    stResolution->play43 = jvList["play43"].asString();
                }
                CPlayerDlg::m_pThis->SetResolution(stResolution);
                int nResolu = 2;
                if (CPlayerDlg::m_pThis->m_nResolu == 1 && stResolution->play1.length()){
                    strPlay = stResolution->play1;
                    nResolu = 1;
                }
                if (CPlayerDlg::m_pThis->m_nResolu == 2 && stResolution->play2.length()) {
                    strPlay = stResolution->play2;
                    nResolu = 2;
                }
                if (CPlayerDlg::m_pThis->m_nResolu == 3 && stResolution->play3.length()) {
                    strPlay = stResolution->play3;
                    nResolu = 3;
                }

                //名称
                if (jvData["name"].isString()) {
                    string strname = jvData["name"].asString();
                    mtx_play.lock();
                    SWindow* pWnd = FindChildByName(L"img_title");
                    if (pWnd)pWnd->SetWindowTextW(QwBase::s2ws(strname).c_str());
                    CPlayerDlg::m_pThis->PlayUrl(strPlay, strname, nResolu);
                    mtx_play.unlock();
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

int CListDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    SetMsgHandled(FALSE);
    return 0;
}

BOOL CListDlg::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    m_sql.InitSql(g_sDbPath);

    m_bLayoutInited = TRUE;
    GetEncoderClsid(L"image/bmp", &m_bmpClsid);//定义一个获取指定图片格式的编码器
    //DragAcceptFiles(m_hWnd, TRUE);//允许拖拽文件到窗口
    SListTitle::m_nSelect = 102;
    g_hListWnd = m_hWnd;
    SListWnd* pLWnd = FindChildByName2<SListWnd>(L"list");
    if (pLWnd) {
        pLWnd->SetParentWnd(m_hWnd);
        CRect rcWnd = pLWnd->GetWindowRect();
        m_ptMove.x = rcWnd.top + 10;
        m_ptMove.y = rcWnd.bottom - 10;
    }

    thread th = thread(&CListDlg::GetTimeThread,this);
    th.detach();

    DWORD dwNewStyle = WS_CHILDWINDOW | WS_CLIPCHILDREN;
    ::SetWindowLong(m_hWnd, GWL_STYLE, dwNewStyle);//设置成新的样式

    if (m_vecListData.size()==0){
		SWindow* pWnd = FindChildByName(L"btn_clear");
		if(pWnd) pWnd->EnableWindow(0);
    }

    return 0;
}

int CListDlg::GetEncoderClsid(const WCHAR * format, CLSID * pClsid)
{
    UINT  num = 0;          // number of image encoders
    UINT  size = 0;         // size of the image encoder array in bytes

    ImageCodecInfo* pImageCodecInfo = NULL;
    GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;  // Failure
    pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL)
        return -1;  // Failure

    GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j)
    {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Success
        }
    }
    free(pImageCodecInfo);
    return -1;  // Failure
}

void CListDlg::OnDropFiles(HDROP hDropInfo)
{
    ::PostMessage(g_hWnd, WM_DROPFILES, (WPARAM)hDropInfo, 0);
}

void CListDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    SetMsgHandled(FALSE);
    UINT lParam = (nFlags << 16) | nRepCnt;
    ::PostMessage(CPlayerDlg::m_pThis->m_hWnd, WM_KEYDOWN, nChar, lParam);
}

void CListDlg::OnCommand(UINT uNotifyCode, int nID, HWND wndCtl)
{
    switch (nID)
    {
    case CMD_PLAY:
    {
        Play(m_nListID);
        return;
    }
    case CMD_DEL:
    {
        DelItem(m_nListID);
        return;
    }
    case CMD_DELALL:
    {
        OnBtnClear();
        return;
    }
    case CMD_OPENFLODER:
    {
        fItem* file = GetFileinfoFormID(m_nListID);
        if (file)
        {
            SStringT strPath = file->strPath;
            strPath = _T("/select,") + strPath;
            ShellExecute(nullptr, L"open", L"explorer.exe", strPath, NULL, SW_SHOWNORMAL);
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

void CListDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    SetMsgHandled(FALSE);
    /******20170808修改，只用在hover状态下显示按钮*******/
    if (m_nOldHoverID != g_nHoverID)
    {
        SItemWnd* pItem = FindChildByID2<SItemWnd>(m_nOldHoverID);
        if (pItem)
        {
            SImageButton1* pBtn = pItem->FindChildByName2<SImageButton1>(L"btn_jx");
            if (pBtn)pBtn->SetAttribute(L"show", L"0");
            SWindow* pWnd = pItem->FindChildByName(L"playstates");
            if (pWnd) pWnd->SetAttribute(L"show", L"1");
        }
        m_nOldHoverID = g_nHoverID;
    }
    if (g_nHoverID)
    {
        SItemWnd* pItem = FindChildByID2<SItemWnd>(g_nHoverID);
        if (!pItem || !pItem->HitTest(point))
        {
            g_nHoverID = 0;
            SWindow::Invalidate();
        }
        else
        {
            /******20170808修改，只用在hover状态下显示按钮*******/
            fItem* file = GetFileinfoFormID(g_nHoverID);
            if (file&&g_nHoverID != g_nPlayingID&&file->lPlayedTime)
            {
                SImageButton1* pBtn = pItem->FindChildByName2<SImageButton1>(L"btn_jx");
                if (pBtn)pBtn->SetAttribute(L"show", L"1");
                SWindow* pWnd = pItem->FindChildByName(L"playstates");
                if (pWnd) pWnd->SetAttribute(L"show", L"0");
            }
        }
    }
    if (m_bButtondown)
    {
        static BOOL bSetNO = FALSE;
        int nID = 0;
        CRect rcWnd = FindChildByName(L"list")->GetWindowRect();
        if (!PtInRect(&rcWnd,point))
        {
            bSetNO = TRUE;
        }
        else
        {
            bSetNO = FALSE;
            SItemWnd* pWnd;
            BOOL bFind = FALSE;
            for each (auto var in m_vecListData)
            {
                pWnd = FindChildByID2<SItemWnd>(var->nID);
                if (pWnd) {
                    CRect rcWnd = pWnd->GetWindowRect();
                    if (PtInRect(&rcWnd, point)) {
                        nID = var->nID;
                        bFind = TRUE;
                        break;
                    }
                }
                if (var->bShowChild)
                {
                    for each (auto cld in var->vecChild)
                    {
                        pWnd = FindChildByID2<SItemWnd>(cld->nID);
                        if (pWnd) {
                            CRect rcWnd = pWnd->GetWindowRect();
                            if (PtInRect(&rcWnd, point)) {
                                nID = cld->nID;
                                bFind = TRUE;
                                break;
                            }
                        }
                    }
                }
                if (bFind) break;
            }
        }

        fItem* file = GetFileinfoFormID(m_nDownID);
        fItem* fHover = GetFileinfoFormID(nID);
        if (fHover&&file)
        {
            if ((file->nParentID == 0 && fHover->nParentID) || (file->nParentID != fHover->nParentID))
            {
                bSetNO = TRUE;
            }
        }

        if (bSetNO)
            SetCursor(LoadCursor(NULL, IDC_NO));
        else
            SetCursor(LoadCursor(NULL, IDC_ARROW));
    }

    MoveList(point);
}

void CListDlg::OnMouseLeave()
{
    SetMsgHandled(FALSE);
    /******20170808修改，只用在hover状态下显示按钮*******/
    SItemWnd* pItem = FindChildByID2<SItemWnd>(m_nOldHoverID);
    if (pItem)
    {
        SImageButton1* pBtn = pItem->FindChildByName2<SImageButton1>(L"btn_jx");
        if (pBtn)pBtn->SetAttribute(L"show", L"0");
    }
    m_nOldHoverID = 0;


    g_nHoverID = 0;
    SWindow::Invalidate();
}

void CListDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    SetMsgHandled(FALSE);
    ::PostMessage(g_hWnd, WM_LBUTTONDOWN, 0, 0);//关闭选集窗口
}

void CListDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    SetMsgHandled(FALSE);
    SetCursor(LoadCursor(NULL, IDC_ARROW));
    m_bSortList = FALSE;
    int nID = IsSortList(point);
    if (nID)
    {
        SortList(nID); //移动item位置
        m_bSortList = TRUE;
    }
}

void CListDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    SetMsgHandled(FALSE);
    SItemWnd* pItem = FindChildByID2<SItemWnd>(g_nHoverID);
    if (pItem && pItem->HitTest(point))
    {
        Play(g_nHoverID);
    }
}

void CListDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
    SetMsgHandled(FALSE);
    ::PostMessage(g_hWnd, WM_LBUTTONDOWN, 0, 0);//关闭选集窗口
}

void CListDlg::OnTimer(UINT_PTR nIDEvent)
{
    SetMsgHandled(FALSE);
    if (T_LIST_SWITCH == nIDEvent)
    {
        if (!m_bSortList){
            BtnSwitch(m_nSwitchID);
        }
        
        KillTimer(T_LIST_SWITCH);
        m_bSwitchTimer = FALSE;
        m_nSwitchID = 0;
    }
}

void CListDlg::OnTitleCtrl(int nID)
{
    SListTitle::m_nSelect = nID;
    if (m_nListStatus!= nID - 100)
    {
        m_nListStatus = nID - 100;
        UpdateList(FALSE);
    }
    if (100 == nID)
    {
        SendTj(_T("list_btn"), _T("click"), _T("tab_all"), _T("tab_all"));
    }
    if (101 == nID)
    {
        SendTj(_T("list_btn"), _T("click"), _T("tab_online"), _T("tab_online"));
    }
    if (102 == nID)
    {
        SendTj(_T("list_btn"), _T("click"), _T("tab_local"), _T("tab_local"));
    }
    SWindow::Invalidate();
}

void CListDlg::OnBtnAdd()
{
    CRect rcWnd = GetWindowRect();
    CPoint pt;
    pt.x = rcWnd.left;
    pt.y = rcWnd.top;

    SWindow* pWnd = FindChildByName(L"btn_add");
    if (pWnd)
    {
        rcWnd = pWnd->GetWindowRect();
        pt.x += rcWnd.left;
        pt.y += rcWnd.bottom+10;
    }

    /*
    SMenuEx menu;
    menu.LoadRes(_T("smenuex:add_menuex"));
    */
    SMenu menu;
    menu.LoadRes(_T("smenu:add_menu"));
    menu.SetItemValue(10, L"hotkeyText", g_mapHotkey[HK_OPENFILE]->GetString());
    menu.SetItemValue(11, L"hotkeyText", g_mapHotkey[HK_OPENFLODER]->GetString());

    menu.LoadMenu();
    menu.TrackPopupMenu(0, pt.x, pt.y, m_hWnd);

    SendTj(_T("list_btn"), _T("click"), _T("btn_addfile"), _T("btn_addfile"));
}

void CListDlg::OnBtnSx()
{
    CRect rcWnd = GetWindowRect();
    CPoint pt;
    pt.x = rcWnd.left;
    pt.y = rcWnd.top;

    SWindow* pWnd = FindChildByName(L"btn_sx");
    if (pWnd)
    {
        rcWnd = pWnd->GetWindowRect();
        pt.x += rcWnd.left;
        pt.y += rcWnd.bottom+10;
    }

    /*
    SMenuEx menu;
    menu.LoadRes(_T("smenuex:sx_menuex"));
    */
    SMenu menu;
    menu.LoadRes(_T("smenu:sx_menu"));
    menu.SetItemValue(80 + g_set.m_nPlayStyle, L"radio", L"1");
    menu.LoadMenu();
    menu.TrackPopupMenu(0, pt.x, pt.y, m_hWnd);

    SendTj(_T("list_btn"), _T("click"), _T("btn_playmode"), _T("btn_playmode"));
}

void CListDlg::OnBtnJx()
{
    SItemWnd* pItem = FindChildByID2<SItemWnd>(g_nHoverID);
    if (pItem)
    {
        Play(g_nHoverID);
    }
}

void CListDlg::OnBtnZhankai()
{
    static BOOL bZhankai = TRUE;
    int nHeight = 50;
    SStatic* pText = FindChildByName2<SStatic>(L"text_detail");
    if (pText)
    {
        if (bZhankai) nHeight = pText->GetTextHeight();

        WCHAR wBuff[64] = { 0 };
        wsprintf(wBuff,L"20,[10,-10,@%d", nHeight);
        pText->SetAttribute(L"pos", wBuff);
    }
    SStringT str = bZhankai == TRUE ? L"收起" : L"展开>";
    SWindow* pWnd = FindChildByName(L"btn_zhankai");
    if (pWnd)pWnd->SetWindowTextW(str);
    bZhankai = !bZhankai;
}

void CListDlg::OnBtnList(int nID)
{
    wstring strUrl = g_strBaseUrl;
    if (strUrl.find(L"?cid=") != wstring::npos) {
        strUrl = strUrl.substr(0, strUrl.find(L"?cid="));
    }
    strUrl.append(L"?cid=");
    if (nID <= m_vecGuesslist.size())
    {
        strUrl.append(QwBase::s2ws(m_vecGuesslist[nID-1]));
        thread th = thread(&CListDlg::GetPlayInfo,this, strUrl.c_str());
        th.detach();
    }
}

void CListDlg::OnVip()
{
    wstring wsCmdLine = L"-id:咪咕";
    wsCmdLine.append(L" -url:https://www.miguvideo.com/wap/resource/lenovo_pc/memberCenter/user_center.jsp?skin=dark&token=");
    wsCmdLine.append(g_strToken.GetBuffer(0));
    ShellExecute(NULL, L"open", g_strSmartHome.c_str(),wsCmdLine.c_str(), NULL, SW_SHOW);
}

void CListDlg::OnBtnMorevideo()
{
    ShellExecute(NULL, L"open", g_strSmartHome.c_str(),
        L"-from:lenoveoplayer", NULL, SW_SHOW);
}

/*清空列表*/
void CListDlg::OnBtnClear()
{
    BOOL bClear = TRUE;
    if (!m_bNoprompt)
    {
        CClearDlg dlg;
        bClear = dlg.DoModel(1);
        m_bNoprompt = dlg.IsNoprompt();
    }
    if (bClear)
    {
        //清理列表是停止正在播放的视频
        CPlayerDlg::m_pThis->DoStop();

        SListWnd* pLWnd = FindChildByName2<SListWnd>(L"play_list");
        if (pLWnd) {
            pLWnd->Clear();
        }

        vector <fItem*>::iterator iter = m_vecListData.begin();
        for (; iter!= m_vecListData.end();)
        {
            fItem* file = *iter;
            if (m_nListStatus!=0&&m_nListStatus!= file->nCode){
                iter++;
                continue;
            }
            delete *iter;
            iter = m_vecListData.erase(iter);
        }
        g_nPlayingID = 0;
		SWindow* pWnd = FindChildByName(L"btn_clear");
		if (pWnd) pWnd->EnableWindow(0);
        UpdateBtnStatus();
        SWindow::Invalidate();

        m_sql.DelPlayRecod();
    }
    ::PostMessage(CPlayerDlg::m_pThis->m_hWnd, UM_ENABLE_STOP, 0, 0);
    SendTj(_T("list_btn"), _T("click"), _T("btn_clearlist"), _T("btn_clearlist"));
}

LRESULT CListDlg::ListRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    m_nListID = wParam;
    CPoint point = CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    CRect rcWnd;
    ::GetWindowRect(m_hWnd, rcWnd);

    SMenu menu;
    menu.LoadRes(_T("smenu:list_menu"));
    /*
    SMenuEx menu;
    menu.LoadRes(_T("smenuex:list_menuex"));
    menu.SetWidth(188, 103);
    */

    //清空列表项
    if (m_vecListData.size()){
        menu.SetItemValue(CMD_DELALL, L"disable", L"0");
        menu.SetItemValue(200,L"disable",L"0");
    }
    //删除项
    if (wParam){
        menu.SetItemValue(CMD_DEL, L"disable", L"0");
    }
    fItem* file = GetFileinfoFormID(wParam);
    if (wParam&&file&&PathFileExists(file->strPath))
    {
        menu.SetItemValue(CMD_PLAY, L"disable", L"0");
        menu.SetItemValue(CMD_OPENFLODER, L"disable", L"0");
    }

    menu.SetItemValue(10, L"hotkeyText", g_mapHotkey[HK_OPENFILE]->GetString());
    menu.SetItemValue(11, L"hotkeyText", g_mapHotkey[HK_OPENFLODER]->GetString());

    menu.SetItemValue(80+g_set.m_nPlayStyle, L"radio", L"1");

    if (g_set.m_bOpenNotrace)
        menu.SetItemValue(18, L"radio", L"1");
    else
        menu.SetItemValue(19, L"radio", L"1");
 
    menu.LoadMenu();
    menu.TrackPopupMenu(0, rcWnd.left + point.x, rcWnd.top + point.y, m_hWnd);
    return 0;
}

LRESULT CListDlg::MsgPlay(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (m_vecGuesslist.size())
    {
        wstring strUrl = g_strBaseUrl;
        if (strUrl.find(L"?cid=") != wstring::npos) {
            strUrl = strUrl.substr(0, strUrl.find(L"?cid="));
        }
        strUrl.append(L"?cid=");
        strUrl.append(QwBase::s2ws(m_vecGuesslist[0]));
        thread th = thread(&CListDlg::GetPlayInfo, this, strUrl.c_str());
        th.detach();
    }
    /*
    int nMaxNumer = GetMaxNumber();
    if (g_set.m_nPlayStyle==2|| nMaxNumer ==0)//单个播放
    {
        CPlayerDlg::m_pThis->DoStop();
        return 0;
    }
    if (g_set.m_nPlayStyle == 3)//单个循环
    {
        Play(g_nPlayingID);
        return 0;
    }
    if (g_set.m_nPlayStyle == 1)//随机播放
    {
        int nPlayId = g_nPlayingID;
        srand(time(NULL));
        while (nPlayId==g_nPlayingID)
        {
            int nIndex = rand() % nMaxNumer + 1;
            fItem* file = GetItemFormIndex(nIndex);
            nPlayId = file->nID;
        }
        Play(nPlayId);
        return 0;
    }

    int nIndex = GetIndexFormID(g_nPlayingID);

    if (wParam==0)//上一个
    {
        nIndex--;
        if (nIndex<1 && g_set.m_nPlayStyle==4){
            nIndex = nMaxNumer;
        }

        fItem* file = GetItemFormIndex(nIndex);
        if (file)
        {
            //遇到父item需要继续减1；
            if (file->vecChild.size())
            {
                nIndex--;
                //顺序播放以到最前，不再播放
                if (nIndex < 1 && g_set.m_nPlayStyle == 0) {
                    CPlayerDlg::m_pThis->DoStop();
                    return 0;
                }
                file = GetItemFormIndex(nIndex);
                if (!file) {
                    CPlayerDlg::m_pThis->DoStop();
                    return 0;
                }
            }
            Play(file->nID);
        }
        
    }
    else //下一个
    {
        nIndex++;
        if (nIndex>nMaxNumer && g_set.m_nPlayStyle == 0){
            CPlayerDlg::m_pThis->DoStop();
            return 0;//顺序播放到最后
        }
        if (nIndex>nMaxNumer&& g_set.m_nPlayStyle == 4){
            nIndex = 1;
        }
        fItem* file = GetItemFormIndex(nIndex);
        if (file) Play(file->nID);
    }
    */
    return 0;
}

LRESULT CListDlg::ItemClick(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (wParam == 0)
    {
        //当列表数量大于1时才可以对列表排序
        if (m_vecListData.size()>1){
            m_nDownID = lParam;
            m_bButtondown = TRUE;
        }
    }
    else
    {
        if (m_bSwitchTimer)
        {
            m_nSwitchID = 0;
            KillTimer(T_LIST_SWITCH);
            m_bSwitchTimer = FALSE;
        }
        else
        {
            m_nSwitchID = lParam;
            m_bSwitchTimer = TRUE;
            SetTimer(T_LIST_SWITCH, 200);
        }
    }
    return 0;
}

LRESULT CListDlg::ItemIvalid(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    SItemWnd* pItem = FindChildByID2<SItemWnd>(lParam);
    if (pItem) 
    {
        SStatic* pText = pItem->FindChildByName2<SStatic>(L"playstates");
        if (wParam == 1)
        {
            pItem->SetAttribute(L"colorText", L"#666666");
            if (pText)
            {
                pText->SetWindowTextW(L"无效");
                pText->SetAttribute(L"show", L"1");
            }
            fItem* file = GetFileinfoFormID(lParam);
            if (file&&file->lPlayedTime!=0)
            {
                SImageButton1* pBtn = pItem->FindChildByName2<SImageButton1>(L"btn_jx");
                if (pBtn){
                    pBtn->SetAttribute(L"show", L"0");
                }
            }
        }
        else
        {
            fItem* file = GetFileinfoFormID(lParam);
            if(file)
                ReShowTime(pItem, file->lTime);
        }
    }
    return 0;
}

LRESULT CListDlg::OnClearList(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    //清理列表是停止正在播放的视频
    CPlayerDlg::m_pThis->DoStop();

    SListWnd* pLWnd = FindChildByName2<SListWnd>(L"list");
    if (pLWnd) {
        pLWnd->Clear();
    }
    vector <fItem*>::iterator iter = m_vecListData.begin();
    for (; iter != m_vecListData.end();)
    {
        fItem* file = *iter;
        if (m_nListStatus != 0 && m_nListStatus != (*iter)->nCode) {
            iter++;
            continue;
        }
        delete *iter;
        iter = m_vecListData.erase(iter);
    }
    g_nPlayingID = 0;
	SWindow* pWnd = FindChildByName(L"btn_clear");
	if (pWnd) pWnd->EnableWindow(0);
    UpdateBtnStatus();
    ::PostMessage(CPlayerDlg::m_pThis->m_hWnd, UM_ENABLE_STOP, 0, 0);
    SWindow::Invalidate();
    return 0;
}

LRESULT CListDlg::UpdateSkinPro(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    SWindow* pWnd = FindChildByName(L"btn_sx");
    if (pWnd)
    {
        SStringT strSkin = L"loop_sequentialplay";
        SStringT strTip = L"顺序播放";
        switch (g_set.m_nPlayStyle)
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
    UpdateBtnStatus();
    return 0;
}

LRESULT CListDlg::OnGetPlayUrl(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (wParam == CPlayerDlg::m_pThis->m_nPlayID)
    {
        thread th = thread(&CListDlg::GetPlyaUrl,this, (char*)lParam, wParam);
        th.detach();
    }
    return 0;
}

LRESULT CListDlg::OnPlayInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    char* lpData = (char*)lParam;

    Json::Reader jr;
    Json::Value jv;
    if (jr.parse(lpData, jv))
    {
        if (jv.size() >= 1)
        {
            _PlayInfo* pPlay = new _PlayInfo;
            Json::Value jvData = jv[0];
            //获取信息判断是否可以免费播放，是否是VIP会员
            BOOL bCanPlay = FALSE,isVip = FALSE;
            if (jvData["isOrd"].asString() == "1"
                &&jvData["isV"].asString() != "1")
                bCanPlay = TRUE;
            if (jvData["isVip"].asString() == "1")isVip = TRUE;

            /*
            //类型
            if (jvData["DisplayName"].isString()) {
                string strDisplayName = jvData["DisplayName"].asString();
                if (strDisplayName == "电影")
                {
                    g_lDefaultTime = 6 * 60 * 1000;
                }
                else
                {
                    g_lDefaultTime = 0;
                }
            }
            */

            CPlayerDlg::m_pThis->PlayAndVip(bCanPlay, isVip);
            SetShowvip(!isVip);

            string strPlay = jvData["play"].asString();
            //MID
            pPlay->mId = jvData["mId"].asString();
            //nodeid
            pPlay->nodeid = jvData["nodeid"].asString();
            //ID
            if (jvData["contId"].isString()) {
                pPlay->contId = jvData["contId"].asString();
            }
            //prdpackId
            if (jvData["prdpackId"].isString()) {
                pPlay->prdpackId = jvData["prdpackId"].asString();
            }
            //名称
            if (jvData["name"].isString()) {
                pPlay->name = jvData["name"].asString();
                SWindow* pWnd = FindChildByName(L"img_title");
                if (pWnd)pWnd->SetWindowTextW(QwBase::s2ws(pPlay->name).c_str());
            }

            //简介
            if (jvData["Detail"].isString()) {
                string strDetail = jvData["Detail"].asString();
                SWindow* pWnd = FindChildByName(L"text_detail");
                SStringT strShow = L"1";
                if (pWnd)
                {
                    pWnd->SetWindowTextW(QwBase::s2ws(strDetail).c_str());
                    if (((SStatic*)pWnd)->GetTextHeight() <= 50)strShow = L"0";
                }
               
                if (strDetail.length() == 0){
                    strShow = L"0";
                }
 
                pWnd = FindChildByName(L"btn_zhankai");
                if (pWnd)pWnd->SetAttribute(L"show", strShow);
            }
           
            if (jvData["mediaType"].isString()) {
                string strMediaType = jvData["mediaType"].asString();
                SWindow* pWnd = FindChildByName(L"play_type");
                if (pWnd)pWnd->SetWindowTextW(QwBase::s2ws(strMediaType).c_str());
            }
            //图片
            string strImgUrl;
            if (jvData["imgV"].isString()) {
                strImgUrl = jvData["imgV"].asString();
            }
            if (strImgUrl.size())
            {
                GetGetThumbnailImage(QwBase::s2ws(strImgUrl).c_str(), 120, 170,TRUE);
            }
            else
            {
                SImageWnd* pimg = FindChildByName2<SImageWnd>(L"img_play");
                if (pimg) {
                    pimg->SetAttribute(L"skin", L"skin_teledefault");
                    pimg->SetImage(nullptr);
                }
            }

            //导演
            if (jvData["director"].isString()) {
                string strDirector = jvData["director"].asString();
                SWindow* pWnd = FindChildByName(L"director");
                if (pWnd)pWnd->SetWindowTextW(QwBase::s2ws(strDirector).c_str());
            }
            //主演
            if (jvData["star"].isString()) {
                string strStar = jvData["star"].asString();
                SWindow* pWnd = FindChildByName(L"star");
                strStar.insert(0, 11, ' ');
                if (pWnd)pWnd->SetWindowTextW(QwBase::s2ws(strStar).c_str());
            }

            Resolution* stResolution = new Resolution;
            stResolution->play = strPlay;
            //分辨率列表
            Json::Value jvList = jvData["playList"];
            if (jvList["play1"].isString())
            {
                stResolution->play1 = jvList["play1"].asString();
            }
            if (jvList["play2"].isString())
            {
                stResolution->play2 = jvList["play2"].asString();
            }
            if (jvList["play3"].isString())
            {
                stResolution->play3 = jvList["play3"].asString();
            }

            if (jvData["pilotPlay"].isString())
            {
                stResolution->sPilot = jvData["pilotPlay"].asString();
            }
            jvList = jvData["pilotPlayList"];
            if (jvList["play41"].isString())
            {
                stResolution->play41 = jvList["play41"].asString();
            }
            if (jvList["play42"].isString())
            {
                stResolution->play42 = jvList["play42"].asString();
            }
            if (jvList["play43"].isString())
            {
                stResolution->play43 = jvList["play43"].asString();
            }

            CPlayerDlg::m_pThis->SetResolution(stResolution);
            int nResolu = 2;
            //获取当前播放清晰度链接
            if (bCanPlay)
            {
                if (CPlayerDlg::m_pThis->m_nResolu == 1 && stResolution->play1.length()) {
                    strPlay = stResolution->play1;
                    nResolu = 1;
                }
                if (CPlayerDlg::m_pThis->m_nResolu == 2 && stResolution->play2.length()) {
                    strPlay = stResolution->play2;
                    nResolu = 2;
                }
                if (CPlayerDlg::m_pThis->m_nResolu == 3 && stResolution->play3.length()) {
                    strPlay = stResolution->play3;
                    nResolu = 3;
                }
            }
            else
            {
                strPlay = stResolution->sPilot;
                if (CPlayerDlg::m_pThis->m_nResolu == 1 && stResolution->play41.length()) {
                    strPlay = stResolution->play41;
                    nResolu = 1;
                }
                if (CPlayerDlg::m_pThis->m_nResolu == 2 && stResolution->play42.length()) {
                    strPlay = stResolution->play42;
                    nResolu = 2;
                }
                if (CPlayerDlg::m_pThis->m_nResolu == 3 && stResolution->play43.length()) {
                    strPlay = stResolution->play43;
                    nResolu = 3;
                }
            }

            //猜你喜欢
            m_vecGuesslist.clear();
            jvList = jvData["guessList"];
            SWindow* pWnd = FindChildByName(L"wnd_guesslist");
            if (pWnd)
            {
                WCHAR wBuff[256] = { 0 };
                wsprintf(wBuff, L"0,[0,-0,@%d", (jvList.size() / 2 + jvList.size() % 2) * 102 + 10);
                pWnd->SetAttribute(L"pos", wBuff);
            }
            SStringT strList;
            POINT pt;
            for (size_t i = 0; i < jvList.size(); i++)
            {
                Json::Value jvItem = jvList[i];
                pt.x = 10 + i % 2 * 142;
                pt.y = 10 + i / 2 * 102;
                //ID
                if (jvItem["srcContID"].isString()) {
                    string srcContID = jvItem["srcContID"].asString();
                    m_vecGuesslist.push_back(srcContID);
                }
                //片名
                string strName, strImg;
                if (jvItem["name"].isString()) {
                    strName = jvItem["name"].asString();
                }

                //图片
                if (jvItem["imageSrcH"].isString()) {
                    strImg = jvItem["imageSrcH"].asString();
                }

                wstring wsPath = GetGetThumbnailImage(QwBase::s2ws(strImg).c_str(), 130, 80);
                strList.Append(GetItemString(i + 1, pt, QwBase::s2ws(strName).c_str(), wsPath.c_str()));
            }
            SWindow *pChild = pWnd->GetWindow(GSW_FIRSTCHILD);
            while (pChild)
            {
                SWindow *pNext = pChild->GetWindow(GSW_NEXTSIBLING);
                pChild->DestroyWindow();
                pChild = pNext;
            }
            pWnd->CreateChildren(strList);
            CPlayerDlg::m_pThis->m_nPlayID = 1;
            jvList = jvData["Variety"];
            vector<_PlayInfo*> vec;
            for (size_t i = 0; i < jvList.size(); i++)
            {
                Json::Value jvItem = jvList[i];
                _PlayInfo* pTele = new _PlayInfo();
                if (jvItem["newName"].isString()) {
                    pTele->name = jvItem["newName"].asString();
                }
                if (jvItem["contId"].isString()) {
                    pTele->contId = jvItem["contId"].asString();
                }
                if (stricmp(pTele->contId.c_str(), pPlay->contId.c_str()) == 0) {
                    CPlayerDlg::m_pThis->m_nPlayID = i + 1;
                }
                vec.push_back(pTele);
            }
            CPlayerDlg::m_pThis->SetPlayInfo(pPlay);
            CPlayerDlg::m_pThis->PlayUrl(strPlay, pPlay->name, nResolu);
            CPlayerDlg::m_pThis->SetPlayList(1, jvList.size(), vec);
        }
    }
    free(lpData);
    return 0;
}

LRESULT CListDlg::OnSetImage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (wParam!=0)
    {
        wstring wsPath = (WCHAR*)wParam;
        IBitmap *pImg = SResLoadFromFile::LoadImage(wsPath.c_str());
        SImageWnd* pimg = FindChildByName2<SImageWnd>(L"img_play");
        if (pimg&&pImg)pimg->SetImage(pImg);
    }
    else
    {
        SImageWnd* pimg = FindChildByName2<SImageWnd>(L"img_play");
        if (pimg) {
            pimg->SetAttribute(L"skin", L"skin_teledefault");
            pimg->SetImage(nullptr);
        }
    }
    return 0;
}

void CListDlg::SendTj(const TCHAR * sCategory, const TCHAR * sAction, const TCHAR * sKey, const TCHAR * sValue)
{
    thread tj = std::thread(&CListDlg::TongjiThread, this, sCategory, sAction, sKey, sValue);
    tj.detach();
}

void CListDlg::TongjiThread(const TCHAR * sCategory, const TCHAR * sAction, const TCHAR * sKey, const TCHAR * sValue)
{
    std::list<std::pair<std::tstring, std::tstring>> software_param;
    software_param.push_back(std::make_pair(sKey, sValue));
    mutex_tj.lock();
    ludp::trackEvent(sCategory, sAction, 0, 1, software_param);
    mutex_tj.unlock();
    software_param.clear();
}
