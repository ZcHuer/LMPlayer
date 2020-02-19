#pragma once
namespace SOUI
{
    class SWndBtn : public SImageButton
    {
        SOUI_CLASS_NAME(SWndBtn, L"wndbtn")
    public:
        SWndBtn();
        void OnPaint(IRenderTarget *pRT);
        void OnLButtonDown(UINT nFlags, CPoint pt);
        void OnMouseMove(UINT nFlags, CPoint point);

        SOUI_MSG_MAP_BEGIN()
            MSG_WM_PAINT_EX(OnPaint)
            MSG_WM_LBUTTONDOWN(OnLButtonDown)
            MSG_WM_MOUSEMOVE(OnMouseMove)
        SOUI_MSG_MAP_END()

        SOUI_ATTRS_BEGIN()
            ATTR_INT(L"textOffset", m_nOffset, FALSE)
            ATTR_INT(L"topOffset", m_nTopOffset, FALSE)
            ATTR_INT(L"textWidth", m_nTextWidth, FALSE)
            ATTR_SKIN(L"focusSkin", m_pFocusSkin, TRUE)
            ATTR_SKIN(L"closeSkin1", m_pCloseSkin1, TRUE)
            ATTR_SKIN(L"closeSkin2", m_pCloseSkin2, TRUE)
            ATTR_SKIN(L"fSkin1", m_pfSkin1, TRUE)
            ATTR_SKIN(L"fSkin2", m_pfSkin2, TRUE)
            ATTR_COLOR(L"colorbgFocus", m_crBgFocus, TRUE)
            ATTR_STRINGT(L"otherText", m_strOtherText, FALSE)
            ATTR_STRINGT(L"rightText", m_strRigthText, FALSE)
            ATTR_STRINGT(L"indexStr", m_strIndex, FALSE)
            ATTR_STRINGT(L"keyWord", m_strKeyWord, FALSE)
            ATTR_COLOR(L"colorKey", m_crKey, TRUE)
        SOUI_ATTRS_END()
            
    public:
        void SetNcHover(BOOL bHover);
    private:
        ISkinObj *m_pFocusSkin;
        ISkinObj *m_pCloseSkin1;
        ISkinObj *m_pCloseSkin2;
        ISkinObj *m_pfSkin1;
        ISkinObj *m_pfSkin2;
        COLORREF m_crBgFocus;
        int m_nOffset;
        CRect m_rcClose;
        SStringT m_strOtherText;
        SStringT m_strRigthText;
        int m_nTextWidth;
        int m_nTopOffset;
        COLORREF m_crKey;
        BOOL m_bCloseHover;
        BOOL m_bNcHover;
    public:
        SStringT m_strIndex;
        SStringT m_strKeyWord;
    };
}

