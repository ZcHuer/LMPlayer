#pragma once

namespace SOUI
{
    class SItemWnd : public SWindow
    {
        SOUI_CLASS_NAME(SItemWnd, L"itemwnd")
    public:
        SItemWnd()
        {
            m_nOffset = 0;
            m_pfbtnSkin1 = nullptr;
            m_pfbtnSkin2 = nullptr;
            m_bfBtnHover = FALSE;
            m_crBgFocus = NULL;
            m_nTopOffset = 0;
            m_strTime = L"";
            m_strIndex = L"";
            m_bError = FALSE;
			m_bPause = FALSE;
        }

        SStringT GetItemIndex();
    protected:
        void OnPaint(IRenderTarget *pRT);
        void OnMouseMove(UINT nFlags, CPoint point);
        void OnMouseHover(WPARAM wParam, CPoint ptPos);
        void OnRButtonDown(UINT nFlags, CPoint point);
        void OnLButtonDown(UINT nFlags, CPoint point);
        void OnLButtonUp(UINT nFlags, CPoint point);
        void OnMouseLeave();
    private:

        SOUI_MSG_MAP_BEGIN()
            MSG_WM_PAINT_EX(OnPaint)
            MSG_WM_MOUSEMOVE(OnMouseMove)
            //MSG_WM_MOUSEHOVER(OnMouseHover)
            MSG_WM_RBUTTONDOWN(OnRButtonDown)
            MSG_WM_LBUTTONDOWN(OnLButtonDown)
            MSG_WM_LBUTTONUP(OnLButtonUp)
            //MSG_WM_MOUSELEAVE(OnMouseLeave)
        SOUI_MSG_MAP_END()

        SOUI_ATTRS_BEGIN()
            ATTR_INT(L"textOffset", m_nOffset, FALSE)
            ATTR_SKIN(L"fbtnSkin1", m_pfbtnSkin1, TRUE)
            ATTR_SKIN(L"fbtnSkin2", m_pfbtnSkin2, TRUE)
			ATTR_SKIN(L"fPlayingSkin", m_pfPlayingSkin, TRUE)
            ATTR_COLOR(L"colorbgFocus", m_crBgFocus, TRUE)
            ATTR_COLOR(L"colorKey", m_crKey, TRUE)
            ATTR_STRINGT(L"timeText", m_strTime, FALSE)
            ATTR_STRINGT(L"index", m_strIndex, FALSE)
            ATTR_INT(L"error", m_bError,FALSE)
			ATTR_INT(L"pause", m_bPause, FALSE)
        SOUI_ATTRS_END()

    protected:
        ISkinObj *m_pfbtnSkin1;
        ISkinObj *m_pfbtnSkin2;
		ISkinObj *m_pfPlayingSkin;
        COLORREF m_crBgFocus;
        int m_nOffset;
        CRect m_rcfBtnRect;
		CRect m_rcfPlayRect;
        wstring m_strTime;
        int m_nTopOffset;
        COLORREF m_crKey;
        SStringT m_strIndex;
        BOOL m_bfBtnHover;
        BOOL m_bError;
		BOOL m_bPause;
    public:
        static int m_nPlayingID;	// 当前播放的播放列表节点id
    };
}


