#include "../stdafx.h"
#include "SListTitle.h"

namespace SOUI
{
    int SListTitle::m_nSelect = 0;
    void SListTitle::OnPaint(IRenderTarget *pRT)
    {
        SPainter painter;
        BeforePaint(pRT, painter);
        CRect rcText;
        GetTextRect(rcText);
        if (m_nSelect== GetID())
        {
            pRT->SetTextColor(m_crSelect);
            CRect rcRect = GetParent()->GetWindowRect();
            rcRect.left = rcText.left;
            rcRect.right = rcText.right;
            rcRect.top = rcRect.bottom - 2;
            pRT->FillSolidRect(&rcRect, m_crSelect);
        }
        DrawText(pRT, m_strText, m_strText.GetLength(), rcText, GetTextAlign());

        AfterPaint(pRT, painter);
    }

}

