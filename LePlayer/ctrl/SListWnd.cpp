#include "../stdafx.h"
#include "SListWnd.h"

#define MaxSize 292
#define MinSize 277
#define BUFF_SIZE 1024

namespace SOUI
{
    void SListWnd::OnMouseMove(UINT nFlags, CPoint point)
    {
        SetMsgHandled(FALSE);
    }

    void SListWnd::OnLButtonDown(UINT nFlags, CPoint point)
    {
        SetMsgHandled(FALSE);
    }

    void SListWnd::OnLButtonUp(UINT nFlags, CPoint point)
    {
        SetMsgHandled(FALSE);
    }

    void SListWnd::OnMouseHover(WPARAM wParam, CPoint ptPos)
    {
        SetMsgHandled(FALSE);
    }

    void SListWnd::OnRButtonDown(UINT nFlags, CPoint point)
    {
        SetMsgHandled(FALSE);
        ::PostMessage(m_hParentWnd, UM_LIST_RBUTTONDOWN,0,MAKELPARAM(point.x, point.y));
    }
    void SListWnd::OnMouseLeave()
    {
        SetMsgHandled(FALSE);
    }

    void SListWnd::SetParentWnd(HWND hWnd)
    {
        m_hParentWnd = hWnd;
    }

    void SListWnd::Clear()
    {
        SWindow *pChild = GetWindow(GSW_FIRSTCHILD);
        while (pChild) 
        {
            SWindow *pNext = pChild->GetWindow(GSW_NEXTSIBLING);
            pChild->DestroyWindow();
            pChild = pNext;
        }
        SetViewSize(CSize(0,0));
        UpdateScrollBar();
        Invalidate();
    }

    void SListWnd::SetScrollPos(int nPos)
    {
        OnScroll(TRUE, SB_THUMBTRACK, nPos);
    }

    int SListWnd::GetPageHeght()
    {
        return m_siVer.nPage;
    }

    void SListWnd::MoveScroll(BOOL bUp)
    {
        if (bUp)
        {
            OnScroll(TRUE, SB_LINEUP,0);
        }
        else
        {
            OnScroll(TRUE, SB_LINEDOWN, 0);
        }
    }

    void SListWnd::SaveOldViewOrigin()
    {
        m_ptViewOrigin = GetViewOrigin();
        m_siScroll = m_siVer;
    }

    void SListWnd::UpdateViewOrigin()
    {
        SetViewOrigin(m_ptViewOrigin);
        m_siVer = m_siScroll;
        Invalidate();
    }
}


