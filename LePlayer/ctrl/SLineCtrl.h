#pragma once

namespace SOUI
{
    class SLineCtrl : public SWindow
    {
        SOUI_CLASS_NAME(SLineCtrl, L"linectrl")
    public:
        SLineCtrl() { m_nTop = 0; }
        void SetTop(int nTop);
    protected:

        void OnPaint(IRenderTarget *pRT);

        SOUI_MSG_MAP_BEGIN()
            MSG_WM_PAINT_EX(OnPaint)
        SOUI_MSG_MAP_END()
    private:
        int m_nTop;
    };
}


