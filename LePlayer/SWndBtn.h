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
        SOUI_MSG_MAP_BEGIN()
            MSG_WM_PAINT_EX(OnPaint)
            MSG_WM_LBUTTONDOWN(OnLButtonDown)
        SOUI_MSG_MAP_END()

        SOUI_ATTRS_BEGIN()
            ATTR_INT(L"textOffset", m_nOffset, FALSE)
            ATTR_INT(L"textWidth", m_nTextWidth, FALSE)
            ATTR_SKIN(L"focusSkin", m_pFocusSkin, TRUE)
            ATTR_SKIN(L"closeSkin", m_pCloseSkin, TRUE)
            ATTR_COLOR(L"colorbgFocus", m_crBgFocus, TRUE)
            ATTR_STRINGT(L"otherText", m_strOtherText, FALSE)
            ATTR_STRINGT(L"indexStr", m_strIndex, FALSE)
            ATTR_STRINGT(L"keyWord", m_strKeyWord, FALSE)
            ATTR_COLOR(L"colorKey", m_crKey, TRUE)
        SOUI_ATTRS_END()
            
    private:
        ISkinObj *m_pFocusSkin;
        ISkinObj *m_pCloseSkin;
        COLORREF m_crBgFocus;
        int m_nOffset;
        CRect m_rcClose;
        SStringT m_strOtherText;
        int m_nTextWidth;
       
        COLORREF m_crKey;
    public:
        SStringT m_strIndex;
        SStringT m_strKeyWord;
    };
}

