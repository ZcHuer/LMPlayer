#pragma once

namespace SOUI
{
    class SListTitle :
        public SLink
    {
        SOUI_CLASS_NAME(SListTitle, L"listtitle")
    public:

        void OnPaint(IRenderTarget *pRT);
        
        SOUI_MSG_MAP_BEGIN()
            MSG_WM_PAINT_EX(OnPaint)
            SOUI_MSG_MAP_END()

        SOUI_ATTRS_BEGIN()
            ATTR_COLOR(L"colorSelect", m_crSelect,TRUE)
            SOUI_ATTRS_END()

        static int m_nSelect;
    private:
        COLORREF m_crSelect;//бЁжа
    };
}


