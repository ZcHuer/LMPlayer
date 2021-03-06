#pragma once

#include <core/shostwnd.h>

namespace SOUI
{
    typedef BOOL(*pfnMenuCall) (pugi::xml_node xmlMenu, int nParentID, SStringT strValue,int nID);
    class SMenuEx;
    class SOUI_EXP SMenuExItem : public SWindow
    {
        SOUI_CLASS_NAME(SMenuExItem,L"menuItem")
    public:
        SMenuExItem(SMenuEx *pOwnerMenu,ISkinObj *pItemSkin, COLORREF crHover);

        ~SMenuExItem();

        SMenuEx * GetSubMenu();

        SMenuEx * GetOwnerMenu();

        void HideSubMenu();

        void ShowSubMenu(BOOL bCheckFirstItem);

        TCHAR GetHotKey() const;

        void OnSubMenuHided(BOOL bUncheckItem);
    protected:
        virtual BOOL CreateChildren(pugi::xml_node xmlNode);

        virtual CSize GetDesiredSize(LPCRECT pRcContainer);

        virtual void GetTextRect(LPRECT pRect);


    protected:

        BOOL OnEraseBkgnd(IRenderTarget *pRT);

        void OnPaint(IRenderTarget *pRT);

        SOUI_MSG_MAP_BEGIN()
            MSG_WM_ERASEBKGND_EX(OnEraseBkgnd)
            MSG_WM_PAINT_EX(OnPaint)
        SOUI_MSG_MAP_END()

    protected:
        SOUI_ATTRS_BEGIN()
			ATTR_SKIN(L"headSkin", m_pHeadSkin, TRUE)
			ATTR_SKIN(L"headSkinFocus", m_pHeadShowFocus, TRUE)
			ATTR_SKIN(L"headSkinCheck", m_pHeadShowCheck, TRUE)
			ATTR_INT(L"bshowHead", m_bShowHead, TRUE)
            ATTR_INT(L"icon",m_iIcon,TRUE)
			ATTR_INT(L"iconHover", m_iIconHover, TRUE)
            ATTR_INT(L"check",m_bCheck,TRUE)
            ATTR_INT(L"radio",m_bRadio,TRUE)
            ATTR_CHAR(L"hotKey",m_cHotKey,FALSE)
			ATTR_STRINGT(L"hotkeyText",m_strHotKey, FALSE)
            ATTR_FONT(L"font", m_hFont, FALSE)
        SOUI_ATTRS_END()
		//by xdb
		ISkinObj *m_pHeadSkin;			/*header skin normal */
		ISkinObj *m_pHeadShowFocus;     /*header skin focuse*/
		ISkinObj *m_pHeadShowCheck;     /*header skin focuse*/
		BOOL m_bShowHead;
        SMenuEx * m_pSubMenu;
        SMenuEx * m_pOwnerMenu;
        int       m_iIcon;
		int		  m_iIconHover;
        BOOL      m_bCheck;
        BOOL      m_bRadio;    
        TCHAR     m_cHotKey;
		SStringT  m_strHotKey;
        COLORREF m_crHover;
        CAutoRefPtr<IFont>  m_hFont;
    };

    class SOUI_EXP SMenuEx : protected SHostWnd
    {
        friend class SMenuExItem;
        friend class SMenuExRunData;
    public:
        SMenuEx(void);
        virtual ~SMenuEx(void);

		BOOL LoadRes(LPCTSTR pszResID);
		BOOL LoadMenu();
        BOOL LoadMenu(LPCTSTR pszMenu);
        BOOL LoadMenu(pugi::xml_node xmlNode);
		BOOL SetItemValue(int nID, LPCTSTR strKey, LPCTSTR strValue);
        void AddChildItem(int nParentID,SStringT strValue,int nID, pfnMenuCall lpCall);
        UINT TrackPopupMenu(UINT flag,int x,int y,HWND hOwner);
        void SetWidth(int nMainWidth,int nChildWidth);
        SMenuExItem * GetParentItem() {return m_pParent;}
        SMenuEx * GetSubMenu(int nID);
        SMenuExItem * GetMenuItem(int nID);
		void setShowMode(int show);
    protected:
        int OnMouseActivate(HWND wndTopLevel, UINT nHitTest, UINT message);
        void OnTimer(UINT_PTR timeID);
        void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		BOOL SetValue(pugi::xml_node xmlMenu, int nID, LPCTSTR strKey, LPCTSTR strValue);
        BEGIN_MSG_MAP_EX(SMenuEx)
            MSG_WM_MOUSEACTIVATE(OnMouseActivate)
            MSG_WM_TIMER(OnTimer)
            MSG_WM_KEYDOWN(OnKeyDown)
            CHAIN_MSG_MAP(SHostWnd)
        END_MSG_MAP()
    protected:
        virtual BOOL _HandleEvent(EventArgs *pEvt);

        void ShowMenu(UINT uFlag,int x,int y,BOOL bChild);
        void HideMenu(BOOL bUncheckParentItem);
        void HideSubMenu();
        void RunMenu(HWND hOwner);

        void PopupSubMenu(SMenuExItem * pItem,BOOL bCheckFirstItem);
        void OnSubMenuHided(BOOL bUncheckItem);

        SMenuEx(SMenuExItem *pParent);
        SMenuExItem * m_pParent;
        SMenuExItem * m_pHoverItem;
        SMenuExItem * m_pCheckItem;
		pugi::xml_document m_xmlDoc;
        int m_nMainWidth, m_nChildWidth;
		HWND m_hParant;
		int showmode;
    };

}
