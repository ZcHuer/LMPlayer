#include "stdafx.h"
#include "SWndBtn.h"

namespace SOUI
{
    SWndBtn::SWndBtn()
    {
        m_nOffset = 0;
        m_pFocusSkin = NULL;
        m_pCloseSkin = NULL;
        m_crBgFocus = NULL;
        m_strOtherText = L"";
        m_nTextWidth = 0;
        m_strIndex = L"";
        m_strKeyWord = L"";
    }

    void SWndBtn::OnPaint(IRenderTarget * pRT)
    {
        CRect rcClient;
        GetClientRect(&rcClient);

        if (!m_pBgSkin)
        {
            COLORREF crBg = m_style.m_crBg;
            if (GetState()&WndState_Hover&&m_crBgFocus != NULL)
                crBg = m_crBgFocus;
            if (CR_INVALID != crBg) {
                pRT->FillSolidRect(&rcClient, crBg);
            }
        }
        else
        {
            SIZE szImg = m_pBgSkin->GetSkinSize();
            CRect rcImg = CRect(rcClient.left, rcClient.top
                , rcClient.left + szImg.cx, rcClient.top + szImg.cy);
            int nState = 0;
            if (m_pBgSkin->GetStates() == 3)
            {
                if (GetState()&WndState_Disable)
                    nState = 2;
                else if (GetState()&WndState_Check ||
                    GetState()&WndState_PushDown ||
                    GetState()&WndState_Hover)
                    nState = 1;
            }
            m_pBgSkin->Draw(pRT, rcImg, nState);
        }
        if (m_pFocusSkin && 
            (GetState()&WndState_Hover))
            m_pFocusSkin->Draw(pRT, rcClient, 0);

        m_rcClose = rcClient;

        if (m_pCloseSkin && (GetState()&WndState_Hover))
        {
            SIZE szImg = m_pCloseSkin->GetSkinSize();
            m_rcClose = CRect(rcClient.right- szImg.cx-10,
                rcClient.top+ (rcClient.Height()- szImg.cy)/2, 
                rcClient.right-10,
                rcClient.top + (rcClient.Height() - szImg.cy) / 2+ szImg.cy);
            m_pCloseSkin->Draw(pRT, m_rcClose, 0);
        }

        if (m_strText.GetLength())
        {
            CRect rcText;
            GetTextRect(&rcText);
            if (m_style.GetUAlign()==SwndStyle::Align_Left){
                rcText.left += m_nOffset;
                if (m_nTextWidth)
                    rcText.right = rcText.left + m_nTextWidth;
            }
            if (m_style.GetUAlign() == SwndStyle::Align_Right) {
                rcText.right -= m_nOffset;
                if (m_nTextWidth)
                    rcText.left = rcText.right - m_nTextWidth;
            }
            
            SPainter painter;
            BeforePaint(pRT, painter);
            DrawText(pRT, m_strText, m_strText.GetLength(), rcText, GetTextAlign());
            AfterPaint(pRT, painter);
        }

        if (!(GetState()&WndState_Hover)&& m_strOtherText.GetLength())
        {
            CRect rcText;
            GetTextRect(&rcText);
            rcText.right -= m_nOffset;
            SPainter painter;
            BeforePaint(pRT, painter);
            DrawText(pRT, m_strOtherText, m_strOtherText.GetLength(), rcText, 38);
            AfterPaint(pRT, painter);
        }
    }

    void SWndBtn::OnLButtonDown(UINT nFlags, CPoint pt)
    {
        SetMsgHandled(false);
        if (PtInRect(m_rcClose, pt))
            m_uData = 1;
        else
            m_uData = 0;
    }
}