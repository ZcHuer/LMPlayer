#pragma once

namespace SOUI
{

    class SListWnd :
        public SScrollView
    {
        SOUI_CLASS_NAME(SListWnd, L"listwnd")
    public:
        SListWnd() {}
        void Clear();
        void SetParentWnd(HWND hWnd);
        void SetScrollPos(int nPos);
        int GetPageHeght();
        void MoveScroll(BOOL bUp);
        void SaveOldViewOrigin();
        void UpdateViewOrigin();
    protected:
        void OnMouseMove(UINT nFlags, CPoint point);
        void OnLButtonDown(UINT nFlags, CPoint point);
        void OnLButtonUp(UINT nFlags, CPoint point);
        void OnMouseHover(WPARAM wParam, CPoint ptPos);
        void OnRButtonDown(UINT nFlags, CPoint point);
        void OnMouseLeave();

        SOUI_MSG_MAP_BEGIN()
            MSG_WM_MOUSEMOVE(OnMouseMove)
            MSG_WM_LBUTTONDOWN(OnLButtonDown)
            MSG_WM_LBUTTONUP(OnLButtonUp)
            MSG_WM_MOUSEHOVER(OnMouseHover)
            MSG_WM_RBUTTONDOWN(OnRButtonDown)
            MSG_WM_MOUSELEAVE(OnMouseLeave)
        SOUI_MSG_MAP_END()

    private:
        HWND m_hParentWnd;
        CPoint m_ptViewOrigin;
        SCROLLINFO m_siScroll;
    };
}


