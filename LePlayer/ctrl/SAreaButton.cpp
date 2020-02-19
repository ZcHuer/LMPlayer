#include "../stdafx.h"
#include "SAreaButton.h"

namespace SOUI
{
    long SAreaButton::m_nCheckID = 0;
    SAreaButton::SAreaButton()
    {
        m_nTextOffset = 0;
        m_strData = L"";
    }

    void SAreaButton::OnPaint(IRenderTarget * pRT)
    {
        CRect rcClient;
        GetClientRect(&rcClient);
        COLORREF crBg = m_style.m_crBg;
        if (CR_INVALID != crBg) {
            pRT->FillSolidRect(&rcClient, crBg);
        }

        if ((GetState()&WndState_Hover) 
            || m_nCheckID == GetID())
        {
            CRect rcWnd = GetWindowRect();
            CPoint pts[5];
            pts[0].x = rcWnd.left;
            pts[0].y = rcWnd.top;

            pts[1].x = rcWnd.right-1;
            pts[1].y = rcWnd.top;

            pts[2].x = rcWnd.right-1;
            pts[2].y = rcWnd.bottom-1;

            pts[3].x = rcWnd.left;
            pts[3].y = rcWnd.bottom-1;

            pts[4].x = rcWnd.left;
            pts[4].y = rcWnd.top;

            CAutoRefPtr<IPen> curPen, oldPen;
            pRT->CreatePen(PS_SOLID, m_crPlay, 1,&curPen);
            pRT->SelectObject(curPen, (IRenderObj**)&oldPen);
            pRT->DrawLines(pts, 5);
            pRT->SelectObject(oldPen);
            curPen.Release();
            oldPen.Release();
        }
        SwndStyle style = GetStyle();
        pRT->SetTextColor(style.GetTextColor(0));
        if (m_nCheckID == GetID())
            pRT->SetTextColor(m_crPlay);

        CRect rcText;
        GetTextRect(&rcText);
        rcText.DeflateRect(m_nTextOffset,0);
        pRT->DrawText(m_strText, m_strText.GetLength(), rcText, GetTextAlign());
    }
}

