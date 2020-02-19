#pragma once
namespace SOUI
{
    class SCheckBoxEx : public SCheckBox
    {
        SOUI_CLASS_NAME(SCheckBoxEx, L"check1")
    public:
        SCheckBoxEx();

        void OnPaint(IRenderTarget *pRT);

        SOUI_MSG_MAP_BEGIN()
            MSG_WM_PAINT_EX(OnPaint)
        SOUI_MSG_MAP_END()

        SOUI_ATTRS_BEGIN()
            ATTR_SKIN(L"skin1", m_pSkin1, TRUE)
            ATTR_SKIN(L"skin2", m_pSkin2, TRUE)
            ATTR_SKIN(L"skin3", m_pSkin3, TRUE)
            ATTR_SKIN(L"skin4", m_pSkin4, TRUE)
        SOUI_ATTRS_END()

    private:
        ISkinObj *m_pSkin1;
        ISkinObj *m_pSkin2;
        ISkinObj *m_pSkin3;
        ISkinObj *m_pSkin4;
    };
}


