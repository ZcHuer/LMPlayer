#pragma once
namespace SOUI
{
    class SAreaButton :public SImageButton
    {
        SOUI_CLASS_NAME(SAreaButton, L"abtn")
    public:
        SAreaButton();
    protected:
        void OnPaint(IRenderTarget *pRT);
        SOUI_MSG_MAP_BEGIN()
            MSG_WM_PAINT_EX(OnPaint)
        SOUI_MSG_MAP_END()

    SOUI_ATTRS_BEGIN()
            ATTR_COLOR(L"colorPlay", m_crPlay, TRUE)
            ATTR_INT(L"textOffset", m_nTextOffset,FALSE)
            ATTR_STRINGT(L"keyData", m_strData,FALSE)
    SOUI_ATTRS_END()
    private:
        COLORREF m_crPlay;
        int m_nTextOffset;
    public:
        static long m_nCheckID;
        SStringT m_strData;
    };
}


