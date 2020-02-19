#include "../stdafx.h"
#include "SWndBtn.h"

namespace SOUI
{
    SWndBtn::SWndBtn()
    {
        m_nOffset = 0;
        m_pFocusSkin = NULL;
        m_pCloseSkin1 = NULL;
        m_pCloseSkin2 = NULL;
        m_crBgFocus = NULL;
        m_strOtherText = L"";
        m_nTextWidth = 0;
        m_strIndex = L"";
        m_strKeyWord = L"";
        m_nTopOffset = 0;
        m_strRigthText = L"";
        m_bCloseHover = FALSE;
        m_bNcHover = FALSE;
        m_pfSkin1 = nullptr;
        m_pfSkin2 = nullptr;
    }

    void SWndBtn::OnPaint(IRenderTarget * pRT)
    {
		OutputDebugString(L"SWndBtn::OnPaint\n");
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
        if ((m_pFocusSkin && 
            (GetState()&WndState_Hover))|| m_bNcHover)
            m_pFocusSkin->Draw(pRT, rcClient, 0);

        //m_rcClose = rcClient;

        if (m_pCloseSkin1 && (GetState()&WndState_Hover))
        {
            SIZE szImg = m_pCloseSkin1->GetSkinSize();
            m_rcClose = CRect(rcClient.right- szImg.cx-17,
                rcClient.top+ (rcClient.Height()- szImg.cy)/2, 
                rcClient.right-17,
                rcClient.top + (rcClient.Height() - szImg.cy) / 2+ szImg.cy);
            if(m_bCloseHover&&m_pCloseSkin2)
                m_pCloseSkin2->Draw(pRT, m_rcClose, 0);
            else
                m_pCloseSkin1->Draw(pRT, m_rcClose, 0);
        }

        if (m_pfSkin1)
        {
            SIZE szImg = m_pfSkin1->GetSkinSize();
            CRect rcF = CRect(rcClient.left+17,
                rcClient.top + (rcClient.Height() - szImg.cy) / 2,
                rcClient.left+szImg.cx+17,
                rcClient.top + (rcClient.Height() - szImg.cy) / 2 + szImg.cy);
            if ((GetState()&WndState_Hover) && m_pfSkin1)
                m_pfSkin2->Draw(pRT, rcF, 0);
            else
                m_pfSkin1->Draw(pRT, rcF, 0);
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
            if (m_nTopOffset)
            {
                rcText.top -= m_nTopOffset;
                rcText.bottom -= m_nTopOffset;
            }
            
            SPainter painter;
            BeforePaint(pRT, painter);
            if (m_bNcHover) {
                SwndStyle style = GetStyle();
                pRT->SetTextColor(style.GetTextColor(1));
            }
             
            DrawText(pRT, m_strText, m_strText.GetLength(), rcText, GetTextAlign());
            AfterPaint(pRT, painter);
        }

        if (m_strRigthText.GetLength())
        {
            CRect rcText;
            GetTextRect(&rcText);
            rcText.right -= m_nOffset;
            SPainter painter;
            BeforePaint(pRT, painter);
            DrawText(pRT, m_strRigthText, m_strRigthText.GetLength(), rcText, 38);
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
        SetMsgHandled(FALSE);
        m_uData = 0;
		CRect rcClient;
		GetClientRect(&rcClient);
		//m_rcClose = rcClient;
		if (m_pCloseSkin1)
		{
			SIZE szImg = m_pCloseSkin1->GetSkinSize();
			m_rcClose = CRect(rcClient.right - szImg.cx - 20,
				rcClient.top + (rcClient.Height() - szImg.cy) / 2,
				rcClient.right - 20,
				rcClient.top + (rcClient.Height() - szImg.cy) / 2 + szImg.cy);
		}

        if (PtInRect(m_rcClose, pt))
            m_uData = 1;
        else
            m_uData = 0;
    }
    void SWndBtn::OnMouseMove(UINT nFlags, CPoint point)
    {
        SetMsgHandled(FALSE);
		CRect rcClient;
		GetClientRect(&rcClient);
		//m_rcClose = rcClient;
		if (m_pCloseSkin1)
		{
			SIZE szImg = m_pCloseSkin1->GetSkinSize();
			m_rcClose = CRect(rcClient.right - szImg.cx - 20,
				rcClient.top + (rcClient.Height() - szImg.cy) / 2,
				rcClient.right - 20,
				rcClient.top + (rcClient.Height() - szImg.cy) / 2 + szImg.cy);
		}
		
        if (PtInRect(m_rcClose, point)) {
			//关闭按钮
            if (!m_bCloseHover) {
                m_bCloseHover = TRUE;
                //Invalidate();
            }
        }
        else {
			//其他
            if (m_bCloseHover) {
                m_bCloseHover = FALSE;
                //Invalidate();
            }
        }
    }

    void SWndBtn::SetNcHover(BOOL bHover)
    {
        SetMsgHandled(FALSE);
        m_bNcHover = bHover;
        Invalidate();
    }
}