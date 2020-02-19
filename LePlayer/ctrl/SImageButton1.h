#pragma once
namespace SOUI
{
    class SImageButton1 :
        public SImageButton
    {
        SOUI_CLASS_NAME(SImageButton1, L"imgbtn1")
    public:
        SImageButton1()
        {
            m_pBgSkin = nullptr;
            m_pFocusSkin = nullptr;
            m_pEnableSkin = nullptr;
            m_nOffset = 0;
            m_szImgOffset.cx = 0;
            m_szImgOffset.cy = 0;
            m_bOriginal = TRUE;
            m_pImg = nullptr;
            m_crFocus = NULL;
            m_bShowRect = FALSE;
            m_strSkinPath = L"";
            m_strIndex = L"";
            m_crBgEx = CR_INVALID;
        }
        void OnPaint(IRenderTarget *pRT);
        CSize GetDesiredSize(LPCRECT pRcContainer);
        HRESULT OnAttrUrl(const SStringW & strValue, BOOL bLoading);
        void ShowRect(BOOL bShow);
        SOUI_MSG_MAP_BEGIN()
            MSG_WM_PAINT_EX(OnPaint)
        SOUI_MSG_MAP_END()

        SOUI_ATTRS_BEGIN()
            ATTR_INT(L"textOffset", m_nOffset,FALSE)
            ATTR_INT(L"original", m_bOriginal,FALSE)
            ATTR_SIZE(L"imgOffset", m_szImgOffset, FALSE)
            ATTR_STRINGT(L"startAlign", m_strStartAlign,FALSE)
            ATTR_CUSTOM(L"path", OnAttrUrl)
            ATTR_SKIN(L"focusSkin", m_pFocusSkin, TRUE)
            ATTR_SKIN(L"enableSkin", m_pEnableSkin, TRUE)
			ATTR_SKIN(L"Skin", m_pBgSkin, TRUE)
            ATTR_COLOR(L"colorFocus", m_crFocus, TRUE)
            ATTR_COLOR(L"bgColorex", m_crBgEx, TRUE)
            ATTR_STRINGT(L"index", m_strIndex, FALSE)
        SOUI_ATTRS_END()

    private:
        int m_nOffset;
        int m_nImgOffset;
        BOOL m_bOriginal; //是否绘制原始图片大小
        SIZE m_szImgOffset;
        SStringT m_strStartAlign;
        ISkinObj *m_pFocusSkin;  /**< ISkinObj对象 */
        ISkinObj *m_pEnableSkin;
        IBitmap* m_pImg;
        COLORREF m_crFocus;
        COLORREF m_crBgEx;
        BOOL m_bShowRect;
        SStringT m_strSkinPath;
    public:
        SStringT m_strIndex;
    };
}


