#include "include\souistd.h"
#include "helper\SMenuEx.h"
#include "helper\SplitString.h"
extern HWND g_hPlayerWnd;
namespace SOUI
{
#define TIMERID_POPSUBMENU  100
#define TIME_PUPSUBMENU     500

#define WIDTH_MENU_INIT     10000
#define WIDTH_MENU_MAX      2000
#define WIDTH_MENU_MIN      50
	enum ePlayerWndMode
	{
		ePlayerWndMode_Init,
		ePlayerWndMode_Nomal,
		ePlayerWndMode_Mini,
		ePlayerWndMode_Full
	};
    //////////////////////////////////////////////////////////////////////////
    class SMenuExRoot : public SWindow
    {
        SOUI_CLASS_NAME(SMenuExRoot,L"menuRoot")
        friend class SMenuEx;
        friend class SMenuExItem;
    protected:
        ISkinObj * m_pItemSkin;
        ISkinObj * m_pIconSkin;
        ISkinObj * m_pCheckSkin;
        ISkinObj * m_pArrowSkin;
        ISkinObj * m_pSepSkin;

        SMenuEx  * m_pMenuEx;

        int         m_nItemHei;
        int         m_nIconBarWidth;
        int         m_nTextOffset;
		int			m_nArrowOffset; //arrwOffset
		int			m_nHotKeyRightOffset;
        CPoint      m_ptIcon;
        CRect       m_rcMargin;
        int         m_nMinWidth;
        COLORREF m_crHover;
        COLORREF m_crSep;
        int m_nSepHight;
        SOUI_ATTRS_BEGIN()
            ATTR_SKIN(L"itemSkin",m_pItemSkin,FALSE)
            ATTR_SKIN(L"checkSkin",m_pCheckSkin,FALSE)
            ATTR_SKIN(L"sepSkin",m_pSepSkin,FALSE)
            ATTR_SKIN(L"arrowSkin",m_pArrowSkin,FALSE)
            ATTR_INT(L"itemHeight",m_nItemHei,FALSE)
            ATTR_POINT(L"iconPos", m_ptIcon,FALSE)
            ATTR_SKIN(L"iconSkin",m_pIconSkin,FALSE)
            ATTR_RECT(L"margin", m_rcMargin,FALSE)
            ATTR_INT(L"iconBarWidth",m_nIconBarWidth,FALSE)
            ATTR_INT(L"textOffset",m_nTextOffset,FALSE)
			ATTR_INT(L"arrwOffset", m_nArrowOffset, FALSE)
            ATTR_INT(L"minWidth",m_nMinWidth,FALSE)
            ATTR_COLOR(L"colorHover",m_crHover, TRUE)
            ATTR_COLOR(L"colorSep", m_crSep, TRUE)
            ATTR_INT(L"sepHeight", m_nSepHight,FALSE)
        SOUI_ATTRS_END()

    public:
        SMenuExRoot(SMenuEx * pMenuEx)
            :m_pItemSkin(GETBUILTINSKIN(SKIN_SYS_MENU_SKIN))
            ,m_pSepSkin(GETBUILTINSKIN(SKIN_SYS_MENU_SEP))
            ,m_nIconBarWidth(0)
            ,m_pCheckSkin(GETBUILTINSKIN(SKIN_SYS_MENU_CHECK))
            ,m_pIconSkin(NULL)
            ,m_pArrowSkin(GETBUILTINSKIN(SKIN_SYS_MENU_ARROW))
            ,m_nItemHei(26)
            ,m_nMinWidth(WIDTH_MENU_MIN)
            ,m_pMenuEx(pMenuEx)
            ,m_nTextOffset(0)
        {
            m_nSepHight = 0;
        }

        SMenuExItem * GetNextMenuItem(SMenuExItem *pItem,BOOL bForword,int nCount=0);

        CSize CalcMenuSize()
        {
            CRect rcContainer(0,0,WIDTH_MENU_INIT,WIDTH_MENU_INIT);
            return GetDesiredSize(rcContainer);
        }

        virtual CSize GetDesiredSize(LPCRECT pRcContainer)
        {
            CSize szRet;
            SWindow *pItem = GetWindow(GSW_FIRSTCHILD);
            while(pItem)
            {
                CSize szItem = pItem->GetDesiredSize(pRcContainer);
                if(szItem.cx<WIDTH_MENU_MAX)
                    szRet.cx = max(szRet.cx,szItem.cx);
                szRet.cy += szItem.cy;
                pItem = pItem->GetWindow(GSW_NEXTSIBLING);
            }
            szRet.cx += m_rcMargin.left + m_rcMargin.right;
            szRet.cy += m_rcMargin.top + m_rcMargin.bottom;

            if(szRet.cx > m_nMaxWidth && m_nMaxWidth!=-1)
                szRet.cx = m_nMaxWidth;
            if(szRet.cx < m_nMinWidth)
                szRet.cx = m_nMinWidth;
            return szRet;
        }

        virtual BOOL InitFromXml(pugi::xml_node xmlNode)
        {
			//找到根节点，获取在根节点上配置的全局菜单对象属性
			pugi::xml_node xmlRoot = xmlNode.root().first_child();
			if(xmlNode != xmlRoot)
			{
				SObject::InitFromXml(xmlRoot);
			}

			BOOL bRet = __super::InitFromXml(xmlNode);
			SetWindowText(_T(""));//防止子菜单显示父级菜单项的文本。
            return bRet;
        }    

        virtual BOOL CreateChildren(pugi::xml_node xmlNode)
        {
            pugi::xml_node xmlItem = xmlNode.first_child();
            while(xmlItem)
            {
                SWindow *pMenuItem = CreateMenuItem(xmlItem.name());
                if(pMenuItem)
                {
                    InsertChild(pMenuItem);
                    pMenuItem->InitFromXml(xmlItem);
                    pMenuItem->SetAttribute(L"pos",L"0,[0",TRUE);
                }
                xmlItem = xmlItem.next_sibling();
            }
            return TRUE;
        }

        virtual void UpdateChildrenPosition()
        {
            CRect rcClient = GetClientRect();
            rcClient.DeflateRect(m_rcMargin.left,m_rcMargin.top,m_rcMargin.right,m_rcMargin.bottom);

            SWindow *pChild = GetWindow(GSW_FIRSTCHILD);
            CRect rcItem = rcClient;
            rcItem.bottom = rcItem.top;
            while(pChild)
            {
                CSize szItem = pChild->GetDesiredSize(rcClient);
                rcItem.top = rcItem.bottom;
                rcItem.bottom += szItem.cy;
                pChild->Move(rcItem);
                pChild = pChild->GetWindow(GSW_NEXTSIBLING);
            }
        }

        SWindow * CreateMenuItem(const SStringW & strItemName);
    };

    //////////////////////////////////////////////////////////////////////////
    // SMenuExItem

    SMenuExItem::~SMenuExItem()
    {
        if(m_pSubMenu) 
        {
            delete m_pSubMenu;
        }
    }

    SMenuExItem::SMenuExItem(SMenuEx *pOwnerMenu,ISkinObj *pItemSkin, COLORREF crHover) :m_pSubMenu(NULL)
        ,m_pOwnerMenu(pOwnerMenu)
        ,m_iIcon(-1)
		,m_iIconHover(-1)
        ,m_bCheck(FALSE)
        ,m_bRadio(FALSE)
		,m_bShowHead(FALSE)
        ,m_cHotKey(0)
    {
        m_pBgSkin = pItemSkin;
        m_style.m_bTrackMouseEvent=TRUE;
        m_style.SetAttribute(L"align",L"left",TRUE);
        m_crHover = crHover;
    }

    void SMenuExItem::OnPaint(IRenderTarget *pRT)
    {
        //__super::OnPaint(pRT);
        
        CRect rc = GetClientRect();
        SMenuExRoot * pMenuRoot = sobj_cast<SMenuExRoot>(GetRoot()->GetWindow(GSW_FIRSTCHILD));
        SASSERT(pMenuRoot);
        rc.right = rc.left+pMenuRoot->m_nIconBarWidth;
        rc.left += pMenuRoot->m_ptIcon.x;
        rc.top +=pMenuRoot->m_ptIcon.y;
        if(m_bShowHead == FALSE && (m_bCheck || m_bRadio)&& pMenuRoot->m_pCheckSkin)
        {
            SASSERT(pMenuRoot->m_pCheckSkin);
            int nState=0;
            if(m_bRadio)
                nState = m_bCheck?0:1;
            SIZE size = pMenuRoot->m_pCheckSkin->GetSkinSize();
            CRect rcIcon(rc.left,rc.top,rc.left+ size.cx,rc.top+ size.cy);
			int iwidth = rcIcon.Width();
			int ihight = rcIcon.Height();
			rcIcon.top += (rc.Height() - ihight) / 2;
			rcIcon.bottom = rcIcon.top + ihight;
			if (m_nTextLeft >iwidth)
			{
				rcIcon.left += (m_nTextLeft -iwidth)/2;
			}
			else
			{
				rcIcon.left += (rc.Height() - ihight) / 2;
			}
			rcIcon.right = rcIcon.left + iwidth;
			//by xdb
            pMenuRoot->m_pCheckSkin->Draw(pRT,rcIcon, (GetState()&WndState_Hover) ? 1 : 0);
        }else if(m_bShowHead == FALSE && (m_bCheck || m_bRadio)&&pMenuRoot->m_pIconSkin)
        {
            CRect rcIcon(rc.TopLeft(),pMenuRoot->m_pIconSkin->GetSkinSize());
			// chenwang 2017.01.12
			if ((GetState()==WndState_Hover)&& m_iIconHover!=-1)
			{
				pMenuRoot->m_pIconSkin->Draw(pRT, rcIcon, m_iIconHover);
			}
			else
			{
				pMenuRoot->m_pIconSkin->Draw(pRT, rcIcon, m_iIcon);
			}
		}
		

        if(m_pSubMenu)
        {
            CRect rcArrow = GetClientRect();
            CSize szArrow = pMenuRoot->m_pArrowSkin->GetSkinSize();
            rcArrow.left = rcArrow.right - szArrow.cx;
            rcArrow.DeflateRect(0,(rcArrow.Height()-szArrow.cy)/2);
			rcArrow.left -= pMenuRoot->m_nArrowOffset;
			rcArrow.right -= pMenuRoot->m_nArrowOffset;

            pMenuRoot->m_pArrowSkin->Draw(pRT,rcArrow,(GetState()&WndState_Hover)?1:0);
        }

		//draw text
		CRect rcTxt = GetClientRect();
		if (pMenuRoot)
		{
			rcTxt.right -= pMenuRoot->m_nTextOffset;
		}

        SwndStyle style = GetStyle();

        CRect rcText;
        GetTextRect(rcText);
        if (m_nTextLeft) {
            rcText.left += m_nTextLeft;
        }

		COLORREF crOld;

        if (GetState() == WndState_Normal)
            crOld = pRT->SetTextColor(style.GetTextColor(0));

        if (GetState()&WndState_Check)
            crOld = pRT->SetTextColor(style.GetTextColor(1));

        if ((m_bCheck || m_bRadio) && !pMenuRoot->m_pCheckSkin)
            pRT->SetTextColor(style.GetTextColor(2));

        if (GetState() == GetState()&WndState_Check || GetState()&WndState_PushDown || GetState()&WndState_Hover)
            crOld = pRT->SetTextColor(style.GetTextColor(1));
        
        if (GetState()&WndState_Disable) {
            pRT->SetTextColor(style.GetTextColor(3));
        }

        pRT->DrawText(m_strHotKey, m_strHotKey.GetLength(), &rcTxt, DT_SINGLELINE | DT_VCENTER | DT_RIGHT | DT_END_ELLIPSIS);
        pRT->DrawText(m_strText, m_strText.GetLength(), rcText, GetTextAlign());
		if (GetState() == WndState_Hover)
			pRT->SetTextColor(crOld);

		if (m_bShowHead == TRUE && m_pHeadSkin &&m_pHeadShowFocus)
		{
			GetTextRect(rcText);
			CRect rcIcon(rc.TopLeft(), m_pHeadSkin->GetSkinSize());
			int width1 = rcIcon.Width();
			int height1 = rcIcon.Height();
			int cy = (rc.Height() - height1) / 2;
			rcIcon.top = rcText.top+ cy;
			rcIcon.bottom = rcIcon.top + height1;
			if (m_nTextLeft >width1)
			{
				rcIcon.left = (m_nTextLeft - width1) / 2;
			}
			else
			{
				rcIcon.left = cy;
			}
			rcIcon.right = rcIcon.left + width1;
			//xdbleo
			
			if (GetState() != WndState_Normal ||(GetState() == GetState()&WndState_Check) || (GetState()&WndState_PushDown) || (GetState()&WndState_Hover))
			{
				m_pHeadShowFocus->Draw(pRT, rcIcon, 0);
			}
			else if ((m_bCheck || m_bRadio))
			{
				m_pHeadShowCheck->Draw(pRT, rcIcon, 0);
			}
			else 
			{
				m_pHeadSkin->Draw(pRT, rcIcon, 0);
			}

		}
    }

    BOOL SMenuExItem::OnEraseBkgnd(IRenderTarget *pRT)
    {
        int nState = 0;
        if (GetState()&WndState_Disable)
        {
            nState = 2;
        }
        else if (GetState()&WndState_Check || GetState()&WndState_PushDown || GetState()&WndState_Hover)
        {
            nState = 1;
        }

        if (!m_pBgSkin)
        {
            CRect rc = GetClientRect();
            if ((nState == WndState_Hover))
            {
                pRT->FillSolidRect(&rc, m_crHover);
            }
            else
            {
                pRT->FillSolidRect(&rc, m_style.m_crBg);
            }
        }
        else
        {
            if (nState >= m_pBgSkin->GetStates()) nState = 0;
            m_pBgSkin->Draw(pRT, GetClientRect(), nState);
        }

        return TRUE;
    }

    void SMenuExItem::GetTextRect(LPRECT pRect)
    {
        GetClientRect(pRect);
        SMenuExRoot * pMenuRoot = sobj_cast<SMenuExRoot>(GetRoot()->GetWindow(GSW_FIRSTCHILD));
        SASSERT(pMenuRoot);
        pRect->left += pMenuRoot->m_nIconBarWidth + pMenuRoot->m_nTextOffset;
        if(m_pSubMenu) pRect->right -= pMenuRoot->m_pArrowSkin->GetSkinSize().cx;
    }

    CSize SMenuExItem::GetDesiredSize(LPCRECT pRcContainer)
    {
		if (!IsVisible())
			return CSize();
        CSize szRet = __super::GetDesiredSize(pRcContainer);

        SMenuExRoot * pMenuRoot = sobj_cast<SMenuExRoot>(GetRoot()->GetWindow(GSW_FIRSTCHILD));
        SASSERT(pMenuRoot);
        if(!m_layout.IsSpecifySize(PD_X))
        {
            szRet.cx += pMenuRoot->m_nIconBarWidth + pMenuRoot->m_nTextOffset*2;
            if (!m_strHotKey.IsEmpty())
            {
                CAutoRefPtr<IRenderTarget> pRT;
                GETRENDERFACTORY->CreateRenderTarget(&pRT, 0, 0);
                CAutoRefPtr<IFont> oldFont;
                pRT->SelectObject(m_hFont, (IRenderObj**)&oldFont);
                SIZE szTxt;
                pRT->MeasureText(m_strHotKey, m_strHotKey.GetLength(), &szTxt);
                pRT->SelectObject(oldFont);
                szRet.cx += szTxt.cx+20;
            }
            
            if(m_pSubMenu) szRet.cx += pMenuRoot->m_pArrowSkin->GetSkinSize().cx;//加上子菜单箭头宽度
            
        }
        if(!m_layout.IsSpecifySize(PD_Y))
        {
            szRet.cy = pMenuRoot->m_nItemHei;                
        }

        return szRet;
    }

    BOOL SMenuExItem::CreateChildren(pugi::xml_node xmlNode)
    {
        __super::CreateChildren(xmlNode);
        pugi::xml_node xmlChild = xmlNode.child(SMenuExItem::GetClassName());
        if(xmlChild)
        {//有子菜单
            m_pSubMenu = new SMenuEx(this);
            m_pSubMenu->LoadMenu(xmlNode);
        }
        return TRUE;
    }

    void SMenuExItem::OnSubMenuHided(BOOL bUncheckItem)
    {
        m_pOwnerMenu->OnSubMenuHided(bUncheckItem);
    }

    TCHAR SMenuExItem::GetHotKey() const
    {
        return m_cHotKey;
    }

    void SMenuExItem::ShowSubMenu(BOOL bCheckFirstItem)
    {
        if(!m_pSubMenu) return;
        m_pOwnerMenu->PopupSubMenu(this,bCheckFirstItem);
    }

    void SMenuExItem::HideSubMenu()
    {
        if(!m_pSubMenu) return;
        m_pSubMenu->HideMenu(FALSE);
    }

    SMenuEx * SMenuExItem::GetOwnerMenu()
    {
        return m_pOwnerMenu;
    }

    SMenuEx * SMenuExItem::GetSubMenu()
    {
        return m_pSubMenu;
    }

    //////////////////////////////////////////////////////////////////////////


    class SMenuExSep: public SMenuExItem
    {
        SOUI_CLASS_NAME(SMenuExSep,L"sep")
    public:
        SMenuExSep(SMenuEx *pOwnerMenu,ISkinObj *pItemSkin, COLORREF crSep,int nHeight)
            :SMenuExItem(pOwnerMenu,pItemSkin,m_crHover)
        {
            m_crSep = crSep;
            m_dwState |= WndState_Disable;
            m_bDisable = TRUE;
            m_nSepHeight = nHeight;
        }

        virtual CSize GetDesiredSize(LPCRECT pRcContainer)
        {
            SMenuExRoot * pMenuRoot = sobj_cast<SMenuExRoot>(GetRoot()->GetWindow(GSW_FIRSTCHILD));
            SASSERT(pMenuRoot);
            CSize szRet;
            szRet.cx = WIDTH_MENU_INIT;
            if(!m_layout.IsSpecifySize(PD_Y))
            {
                if(m_pBgSkin)
                    szRet.cy = m_pBgSkin->GetSkinSize().cy;
                else
                    szRet.cy = m_nSepHeight;
            }else
			{
				szRet.cy = m_layout.GetSpecifySize(PD_Y);
			}
            return szRet;
        }

    protected:

        BOOL OnEraseBkgnd(IRenderTarget *pRT)
        {
            CRect rcClient = GetClientRect();
            if (!m_pBgSkin)
            {
                CPoint pts[2];
                CAutoRefPtr<IPen> curPen, oldPen;
                pRT->CreatePen(PS_SOLID, m_crSep,1, &curPen);
                pRT->SelectObject(curPen, (IRenderObj**)&oldPen);

                pts[0].x = rcClient.left;
                pts[0].y = rcClient.top + m_nSepHeight/2;
                pts[1].x = rcClient.right;
                pts[1].y = rcClient.top + m_nSepHeight/2;
                pRT->DrawLines(pts, 2);
            }
            else
            {
                m_pBgSkin->Draw(pRT, rcClient, 0);
            }
            
            return TRUE;    
        }

        SOUI_MSG_MAP_BEGIN()
            MSG_WM_ERASEBKGND_EX(OnEraseBkgnd)
        SOUI_MSG_MAP_END()

        COLORREF m_crSep;
        int m_nSepHeight;
    };

    //////////////////////////////////////////////////////////////////////////

    SWindow * SMenuExRoot::CreateMenuItem(const SStringW & strItemName)
    {
        if(strItemName == SMenuExItem::GetClassName())
        {
            return new SMenuExItem(m_pMenuEx,m_pItemSkin,m_crHover);
        }else if(strItemName == SMenuExSep::GetClassName())
        {
            return new SMenuExSep(m_pMenuEx,m_pSepSkin,m_crSep,m_nSepHight);
        }else
        {
            return NULL;
        }
    }

    SMenuExItem * SMenuExRoot::GetNextMenuItem(SMenuExItem *pItem,BOOL bForword,int nCount)
    {
        if(nCount==GetChildrenCount()) return NULL;

        SMenuExItem *pRet = NULL;
        if(pItem)
        {
            SASSERT(pItem->GetParent() == this);
            pRet = (SMenuExItem *)pItem->GetWindow(bForword?GSW_NEXTSIBLING:GSW_PREVSIBLING);
        }
        if(!pRet)
        {
            pRet = (SMenuExItem *)GetWindow(bForword?GSW_FIRSTCHILD:GSW_LASTCHILD);
        }

        if(!pRet->IsDisabled(TRUE)) 
            return pRet;
        else
            return GetNextMenuItem(pRet,bForword,nCount+1);
    }



    //////////////////////////////////////////////////////////////////////////
	class SMenuExRunData : public TObjRefImpl<IObjRef>
    {
        friend class SMenuEx;
    public:
        SMenuExRunData(HWND hOwner):m_hOwner(hOwner),m_bExit(FALSE),m_nCmdID(-1)
        {

        }

        BOOL IsMenuWnd(HWND hWnd)
        {
            SPOSITION pos = m_lstMenuEx.GetTailPosition();
            while(pos)
            {
                if(m_lstMenuEx.GetPrev(pos)->m_hWnd == hWnd) return TRUE;
            }
            return FALSE;
        }

        void PushMenuEx(SMenuEx * pMenu)
        {
            m_lstMenuEx.AddTail(pMenu);
        }

        SMenuEx * GetMenuEx()
        {
            if(m_lstMenuEx.IsEmpty()) return 0;
            return m_lstMenuEx.GetTail();
        }

        SMenuEx * PopMenuEx()
        {
            SASSERT(!m_lstMenuEx.IsEmpty());
            SMenuEx *pMenuEx = m_lstMenuEx.RemoveTail();
            return pMenuEx;
        }

        SMenuEx * SMenuExFromHwnd(HWND hWnd)
        {
            SPOSITION pos = m_lstMenuEx.GetTailPosition();
            while(pos)
            {
                SMenuEx * pMenuEx = m_lstMenuEx.GetPrev(pos);
                if(pMenuEx->m_hWnd == hWnd) return pMenuEx;
            }
            return NULL;
        }

        BOOL IsMenuExited()
        {
            return m_bExit;
        }

        void ExitMenu(int nCmdID)
        {
            m_bExit=TRUE;
            m_nCmdID = nCmdID;
        }

        int GetCmdID(){return m_nCmdID;}

        HWND GetOwner(){return m_hOwner;}
    protected:
        SList<SMenuEx*> m_lstMenuEx;

        BOOL m_bExit;
        int  m_nCmdID;
        HWND m_hOwner;
    };

    static SMenuExRunData * s_MenuData = NULL;

    //////////////////////////////////////////////////////////////////////////
    SMenuEx::SMenuEx(void)
        :m_pParent(NULL)
        ,m_pHoverItem(NULL)
        ,m_pCheckItem(NULL)
    {
        m_hostAttr.m_bTranslucent = TRUE;
        m_nMainWidth = 0;
        m_nChildWidth = 0;
		showmode = 0;
    }

    SMenuEx::SMenuEx(SMenuExItem *pParent):m_pParent(pParent),m_pHoverItem(NULL),m_pCheckItem(NULL)
    {
    }

    SMenuEx::~SMenuEx(void)
    {
        if(IsWindow())
            DestroyWindow();
    }

	BOOL SOUI::SMenuEx::LoadRes(LPCTSTR pszResID)
	{
		SStringTList strMenu;
		if (1 == SplitString<SStringT, TCHAR>(pszResID, _T(':'), strMenu))
			strMenu.InsertAt(0, _T("SMENUEX"));

		BOOL bLoad = LOADXML(m_xmlDoc, strMenu[1], strMenu[0]);
		if (!bLoad) return FALSE;
	}

	BOOL SOUI::SMenuEx::LoadMenu()
	{
		LoadMenu(m_xmlDoc.first_child());
		return 0;
	}

	BOOL SMenuEx::LoadMenu(LPCTSTR pszMenu)
    {
        SStringTList strMenu;
        if(1==SplitString<SStringT,TCHAR>(pszMenu,_T(':'),strMenu))
            strMenu.InsertAt(0,_T("SMENUEX"));

        pugi::xml_document xmlMenu;
        BOOL bLoad = LOADXML(xmlMenu,strMenu[1],strMenu[0]);
        if(!bLoad) return FALSE;
        return LoadMenu(xmlMenu.first_child());
    }

    BOOL SMenuEx::LoadMenu(pugi::xml_node xmlNode)
    {
        if(IsWindow()) return FALSE;
        if(xmlNode.name() != SStringW(SMenuExRoot::GetClassName())
            && xmlNode.name() != SStringW(SMenuExItem::GetClassName()))
            return FALSE;

        HWND hWnd = Create(NULL,WS_POPUP,WS_EX_TOOLWINDOW|WS_EX_TOPMOST,0,0,0,0);
        pugi::xml_document souiXml;
        souiXml.append_child(L"SOUI").append_attribute(L"translucent").set_value(1);
        _InitFromXml(souiXml.child(L"SOUI"),0,0);

        if(!hWnd) return FALSE;


        SMenuExRoot *pMenuRoot = new SMenuExRoot(this);
        InsertChild(pMenuRoot);

		pMenuRoot->InitFromXml(xmlNode);

        pMenuRoot->GetLayout()->SetFitContent(PD_ALL);

        return TRUE;
    }

	BOOL SOUI::SMenuEx::SetItemValue(int nID, LPCTSTR strKey, LPCTSTR strValue)
	{
		return SetValue(m_xmlDoc.child(L"menuRoot"), nID, strKey, strValue);
	}

    void SOUI::SMenuEx::AddChildItem(int nParentID, SStringT strValue, int nID, pfnMenuCall lpCall)
    {
        if (lpCall)
        {
            lpCall(m_xmlDoc.child(L"menuRoot"), nParentID, strValue, nID);
        }
    }

    SMenuExItem * SMenuEx::GetMenuItem(int nID)
    {
        return FindChildByID2<SMenuExItem>(nID);
    }

    SMenuEx * SMenuEx::GetSubMenu(int nID)
    {
        SMenuExItem * pItem = GetMenuItem(nID);
        if(!pItem) return NULL;
        return pItem->GetSubMenu();
    }

    UINT SMenuEx::TrackPopupMenu(UINT flag,int x,int y,HWND hOwner)
    {
        if(!IsWindow()) return -1;
		if(!s_MenuData)
			s_MenuData = new SMenuExRunData(hOwner);
		else
			s_MenuData->AddRef();

        HWND hActive = hOwner;
        if(!hActive) hActive = ::GetActiveWindow();

		m_hParant = hActive;
        ShowMenu(flag,x,y,FALSE);
        RunMenu(hOwner);
        HideMenu(FALSE);

        if(hActive)
        {
            CPoint pt;
            GetCursorPos(&pt);
            ::ScreenToClient(hActive,&pt);
            ::PostMessage(hActive,WM_MOUSEMOVE,0,MAKELPARAM(pt.x,pt.y));
        }

        int nRet = s_MenuData->GetCmdID();

		if(0 == s_MenuData->Release())
		{
			s_MenuData = NULL;
		}
        if(flag & TPM_RETURNCMD)
        {
            return nRet;
        }else
        {
            ::SendMessage(hOwner,WM_COMMAND,MAKEWPARAM(nRet,0),0);
            return TRUE;
        }

    }

    void SOUI::SMenuEx::SetWidth(int nMainWidth, int nChildWidth)
    {
        m_nMainWidth = nMainWidth;
        m_nChildWidth = nChildWidth;
    }

    void SMenuEx::ShowMenu(UINT uFlag,int x,int y, BOOL bChild)
    {
        SMenuExRoot *pMenuRoot = sobj_cast<SMenuExRoot>(GetRoot()->GetWindow(GSW_FIRSTCHILD));
        SASSERT(pMenuRoot);
        CSize szMenu = pMenuRoot->CalcMenuSize();
        
        /**************chenwang 固定宽度*******************/
        if (m_nMainWidth&&!bChild)
        {
            szMenu.cx = m_nMainWidth;
        }
        if (bChild&&m_nChildWidth)
        {
            //szMenu.cx = m_nChildWidth;
        }
        /*********************************/

        pMenuRoot->Move(CRect(CPoint(),szMenu));
        if(uFlag&TPM_CENTERALIGN)
        {
            x -= szMenu.cx/2;
        }else if(uFlag & TPM_RIGHTALIGN)
        {
            x -= szMenu.cx;
        }

        if(uFlag & TPM_VCENTERALIGN)
        {
            y -= szMenu.cy/2;
        }
        else if(uFlag&TPM_BOTTOMALIGN)
        {
            y -= szMenu.cy;
        }

        HMONITOR hMor = MonitorFromWindow(m_hWnd,MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO mi={sizeof(MONITORINFO),0};
        GetMonitorInfo(hMor,&mi);
        CRect rcMenu(CPoint(x,y),szMenu);
        CRect rcInter;
        rcInter.IntersectRect(&rcMenu,&mi.rcWork);
        if(rcInter!=rcMenu)
        {
            if(m_pParent)
            {
                SMenuEx *pParent = m_pParent->GetOwnerMenu();
                CRect rcParent = pParent->GetWindowRect();
                pParent->ClientToScreen(&rcParent);
                if(rcMenu.right>mi.rcWork.right)
                {
                    rcMenu.MoveToX(x-szMenu.cx-rcParent.Width());
                }           
            }

            int xOffset=0,yOffset=0;
            if(rcMenu.left<mi.rcWork.left) xOffset = mi.rcWork.left - rcMenu.left;
            else if(rcMenu.right>mi.rcWork.right) xOffset = mi.rcWork.right - rcMenu.right;
            if(rcMenu.top < mi.rcWork.top) yOffset = mi.rcWork.top - rcMenu.top;
            else if(rcMenu.bottom> mi.rcWork.bottom) yOffset = mi.rcWork.bottom - rcMenu.bottom;

            rcMenu.OffsetRect(xOffset,yOffset);
        }

        SetWindowPos(HWND_TOPMOST,rcMenu.left,rcMenu.top,szMenu.cx,szMenu.cy,SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_NOOWNERZORDER|SWP_NOSENDCHANGING);        
        s_MenuData->PushMenuEx(this);
    }

    void SMenuEx::HideMenu(BOOL bUncheckParentItem)
    {
        if(!CSimpleWnd::IsWindowVisible()) return;
        HideSubMenu();
        ShowWindow(SW_HIDE);
        if(m_pCheckItem)
        {
            m_pCheckItem->SetCheck(FALSE);
            m_pCheckItem=NULL;
        }
        s_MenuData->PopMenuEx();
        if(m_pParent)
        {
            m_pParent->OnSubMenuHided(bUncheckParentItem);
        }
    }

    void SMenuEx::HideSubMenu()
    {
        if(m_pCheckItem) m_pCheckItem->HideSubMenu();
    }

    int SMenuEx::OnMouseActivate(HWND wndTopLevel, UINT nHitTest, UINT message)
    {
        return MA_NOACTIVATE;
    }
	void SMenuEx::setShowMode(int show)
	{
		showmode = show;
	}
    void SMenuEx::RunMenu(HWND hOwner)
    {
        SASSERT(s_MenuData);

        HWND hOwnerParent = hOwner;
		if (showmode != ePlayerWndMode_Mini)
		{
			while (::GetParent(hOwnerParent))
			{
				hOwnerParent = ::GetParent(hOwnerParent);
			}

		}
        
        
        SetForegroundWindow(hOwnerParent);

        BOOL bMsgQuit(FALSE);
        HWND hCurMenu(NULL);
        for(;;)
        {
            if(s_MenuData->IsMenuExited())
            {
                break;
            }
            /***********change by chenwang 2017.3.24*************/
            if (GetForegroundWindow()!=NULL)
            {
                if (GetForegroundWindow() != hOwnerParent )
                {
                    break;
                }
            }
            /*************************/
            /*
            if (GetForegroundWindow() != hOwnerParent)
            {
                break;
            }
            */

            MSG msg = {0};
            BOOL bInterceptOther(FALSE);

            if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
            {//获取消息，不从消息队列中移除。
                if(msg.message == WM_KEYDOWN
                    || msg.message == WM_SYSKEYDOWN
                    || msg.message == WM_KEYUP
                    || msg.message == WM_SYSKEYUP
                    || msg.message == WM_CHAR
                    || msg.message == WM_IME_CHAR)
                {
                    msg.hwnd = s_MenuData->GetMenuEx()->m_hWnd;
                }
                else if(msg.message == WM_LBUTTONDOWN
                    || msg.message  == WM_RBUTTONDOWN
                    || msg.message  == WM_NCLBUTTONDOWN
                    || msg.message  == WM_NCRBUTTONDOWN
                    || msg.message  ==WM_LBUTTONDBLCLK
                    )
                {
                    //click on other window
                    if(!s_MenuData->IsMenuWnd(msg.hwnd))
                    {
                        s_MenuData->ExitMenu(0);
                        break;
                    }else
                    {
                        SMenuEx *pMenu = s_MenuData->SMenuExFromHwnd(msg.hwnd);
                        pMenu->HideSubMenu();
                    }
                }
                else if(msg.message == WM_QUIT)
                {
                    bMsgQuit = TRUE;
                }

                //移除消息队列中当前的消息。
                MSG msgT;
                ::GetMessage(&msgT, 0, 0, 0);

                //拦截非菜单窗口的MouseMove消息
                if (msg.message == WM_MOUSEMOVE)
                {
                    if(msg.hwnd != hCurMenu)
                    {
                        if(hCurMenu)
                        {
                            ::SendMessage(hCurMenu,WM_MOUSELEAVE,0,0);
                        }
                        hCurMenu = msg.hwnd;
                    }

                    SMenuEx *pMenu = s_MenuData->SMenuExFromHwnd(msg.hwnd);
                    if (!pMenu)
                    {
                        bInterceptOther=TRUE;
                        hCurMenu = NULL;
                    }
                }
                
                if (!bInterceptOther)
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
                
            }
            else
            {
                MsgWaitForMultipleObjects (0, 0, 0, 10, QS_ALLINPUT);
            }

            if(bMsgQuit)
            {
                PostQuitMessage(msg.wParam);
                break;
            }
        }

    }

    BOOL SMenuEx::_HandleEvent(EventArgs *pEvt)
    {
        if(pEvt->sender->IsClass(SMenuExItem::GetClassName()))
        {
            SMenuExItem *pMenuItem = sobj_cast<SMenuExItem>(pEvt->sender);
            if(pEvt->GetID() == EventSwndMouseHover::EventID)
            {
                if(pMenuItem->GetSubMenu() != NULL)
                {
                    CSimpleWnd::SetTimer(TIMERID_POPSUBMENU,TIME_PUPSUBMENU);
                    m_pHoverItem = pMenuItem;
                }
                HideSubMenu();
                return FALSE;
            }else if(pEvt->GetID() == EventSwndMouseLeave::EventID)
            {
                if(pMenuItem->GetSubMenu() != NULL)
                {
                    CSimpleWnd::KillTimer(TIMERID_POPSUBMENU);
                    m_pHoverItem=NULL;
                }
                return FALSE;
            }

            if(pEvt->GetID() != EventCmd::EventID) return FALSE;
            SASSERT(pMenuItem);
            if(pMenuItem->GetSubMenu())
            {
                PopupSubMenu(pMenuItem,FALSE);
                return FALSE;
            }else if(pMenuItem->GetID()==0)
            {
                return FALSE;
            }
            s_MenuData->ExitMenu(pMenuItem->GetID());
            return TRUE;
        }else if(s_MenuData && ::IsWindow(s_MenuData->GetOwner()))
        {
            return ::SendMessage(s_MenuData->GetOwner(),UM_MENUEVENT,0,(LPARAM)pEvt);
        }else
        {
            return FALSE;
        }
    }

    void SMenuEx::OnTimer(UINT_PTR timeID)
    {
        if(timeID == TIMERID_POPSUBMENU)
        {
            PopupSubMenu(m_pHoverItem,FALSE);
        }else
        {
            SetMsgHandled(FALSE);
        }
    }

    void SMenuEx::OnSubMenuHided(BOOL bUncheckItem)
    {
        SASSERT(m_pCheckItem);
        if(!bUncheckItem)
        {
            m_pCheckItem->SetCheck(FALSE);
            m_pCheckItem = NULL;
        }
    }

    void SMenuEx::PopupSubMenu(SMenuExItem * pItem,BOOL bCheckFirstItem)
    {
        //item禁用时不显示子item
        if (pItem->IsDisabled(TRUE)) return;

        CSimpleWnd::KillTimer(TIMERID_POPSUBMENU);
        SMenuEx * pSubMenu = pItem->GetSubMenu();
        SASSERT(pSubMenu);
        CRect rcWnd = GetRoot()->GetWindowRect();
        CRect rcItem = pItem->GetWindowRect();
        rcItem.left=rcWnd.left,rcItem.right=rcWnd.right;
        ClientToScreen(&rcItem);

        m_pCheckItem = pItem;
        m_pCheckItem->SetCheck(TRUE);
        pSubMenu->SetWidth(m_nMainWidth,m_nChildWidth);
        pSubMenu->ShowMenu(0,rcItem.right,rcItem.top,TRUE);
        if(bCheckFirstItem)
        {
            SMenuExRoot *pMenuRoot = sobj_cast<SMenuExRoot>(pSubMenu->GetRoot()->GetWindow(GSW_FIRSTCHILD));
            SASSERT(pMenuRoot);
            SMenuExItem *pFirstItem = pMenuRoot->GetNextMenuItem(NULL,TRUE);
            if(pFirstItem)
            {
                pSubMenu->m_pCheckItem = pFirstItem;
                pFirstItem->SetCheck(TRUE);
            }
        }
    }

    void SMenuEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
    {
        SMenuExRoot *pMenuRoot= sobj_cast<SMenuExRoot>(GetRoot()->GetWindow(GSW_FIRSTCHILD));
        SASSERT(pMenuRoot);
        switch(nChar)
        {
        case VK_UP:
        case VK_DOWN:
            if(m_pCheckItem)
                m_pCheckItem->SetCheck(FALSE);
            m_pCheckItem = pMenuRoot->GetNextMenuItem(m_pCheckItem,nChar==VK_DOWN);
            if(m_pCheckItem)
            {
                m_pCheckItem->SetCheck(TRUE);
                m_pCheckItem->Invalidate();
            }
            break;
        case VK_ESCAPE:
        case VK_LEFT:
            if(m_pParent) 
            {
                HideMenu(TRUE);
            }else
            {
                s_MenuData->ExitMenu(0);
            }
            break;
        case VK_RIGHT:
            if(m_pCheckItem)
            {
                m_pCheckItem->ShowSubMenu(TRUE);
            }
            break;
        case VK_RETURN:
            if(m_pCheckItem) m_pCheckItem->FireCommand();
            break;
            break;
        default:
            if(isprint(nChar))
            {
                nChar = tolower(nChar);
                SMenuExItem *pMenuItem = (SMenuExItem*)pMenuRoot->GetWindow(GSW_FIRSTCHILD);
                while(pMenuItem)
                {
                    if(tolower(pMenuItem->GetHotKey()) == nChar)
                    {
                        pMenuItem->FireCommand();
                        return;
                    }
                    pMenuItem=(SMenuExItem*)pMenuItem->GetWindow(GSW_NEXTSIBLING);
                }        
            }else
            {
                SetMsgHandled(FALSE);
            }
            break;
        }
    }

	BOOL SOUI::SMenuEx::SetValue(pugi::xml_node xmlMenu, int nID, LPCTSTR strKey, LPCTSTR strValue)
	{
		pugi::xml_node xmlItem = xmlMenu.first_child();
		while (xmlItem)
		{
			if (wcscmp(L"menuItem", xmlItem.name()) == 0)
			{
				if (xmlItem.attribute(L"id").as_int(0) == nID)
				{
					xmlItem.attribute(strKey) = strValue;
					return TRUE;
				}

				if (SetValue(xmlItem, nID, strKey, strValue)) {
					return TRUE;
				}
			}

			if (nID == xmlItem.attribute(L"id").as_int(0))
			{
				xmlItem.attribute(strKey) = strValue;
				return TRUE;
			}
			xmlItem = xmlItem.next_sibling();
		}
		return FALSE;
	}

}
