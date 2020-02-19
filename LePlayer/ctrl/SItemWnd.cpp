#include "../stdafx.h"
#include "SItemWnd.h"

extern HWND g_hListWnd;
namespace SOUI
{
    int SItemWnd::m_nPlayingID = 0;
    void SItemWnd::OnPaint(IRenderTarget * pRT)
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
        if (m_bError)
        {
            if (m_strText.GetLength())
            {
                CRect rcText;
                GetTextRect(&rcText);
                rcText.left += m_nOffset;
                rcText.right = rcClient.right - m_nOffset;
                rcText.right -= 45;

                if (m_nTopOffset)
                {
                    rcText.top -= m_nTopOffset;
                    rcText.bottom -= m_nTopOffset;
                }

                SPainter painter;
                BeforePaint(pRT, painter);
                DrawText(pRT, m_strText, m_strText.GetLength(), rcText, GetTextAlign());
                AfterPaint(pRT, painter);
            }

            SStringT strError = L"无效";
            CRect rcText;
            GetTextRect(&rcText);
            rcText.right -= m_nOffset;
            SPainter painter;
            BeforePaint(pRT, painter);
            DrawText(pRT, strError, strError.GetLength(), rcText, 38);
            AfterPaint(pRT, painter);
            return;
        }
        //m_rcfBtnRect = rcClient;
        BOOL bPlaying = m_nPlayingID == GetID() ? TRUE : FALSE;

        if (!bPlaying&&m_pfbtnSkin1 && (GetState()&WndState_Hover)
           && CLeDB::GetInstance()->Local()->GetPlayProcess_ByMd5_Local(LeTools::ws2s(m_strIndex.GetBuffer(0)).c_str()))
        {
            SIZE szImg = m_pfbtnSkin1->GetSkinSize();
            m_rcfBtnRect = CRect(rcClient.right - szImg.cx - m_nOffset,
                rcClient.top + (rcClient.Height() - szImg.cy) / 2,
                rcClient.right - m_nOffset,
                rcClient.top + (rcClient.Height() - szImg.cy) / 2 + szImg.cy);
            if (m_bfBtnHover&&m_pfbtnSkin2)
            {
                m_pfbtnSkin2->Draw(pRT, m_rcfBtnRect, 0);
            }
            else
            {
                m_pfbtnSkin1->Draw(pRT, m_rcfBtnRect, 0);
            }            
        }

        if (m_strText.GetLength())
        {
            CRect rcText;
            GetTextRect(&rcText);
            rcText.left += m_nOffset;
            rcText.right = rcClient.right - m_nOffset;
            if (!bPlaying) {
                if (m_pfbtnSkin1 && (GetState()&WndState_Hover)&& m_rcfBtnRect.left){
                    rcText.right = m_rcfBtnRect.left - m_nOffset;
                }
                else if (m_strTime.length())
                    rcText.right = rcClient.right - m_nOffset - m_strTime.length() * 15;
            }
            else{
                rcText.right -= 45;
            }
            
            if (m_nTopOffset)
            {
                rcText.top -= m_nTopOffset;
                rcText.bottom -= m_nTopOffset;
            }

            SPainter painter;
            BeforePaint(pRT, painter);
            if(bPlaying)
                pRT->SetTextColor(m_crKey);
            DrawText(pRT, m_strText, m_strText.GetLength(), rcText, GetTextAlign());
            AfterPaint(pRT, painter);
        }

        if (!bPlaying&&m_strTime.length()&& !(GetState()&WndState_Hover))
        {
            CRect rcText;
            GetTextRect(&rcText);
            rcText.right -= m_nOffset;
            SPainter painter;
            BeforePaint(pRT, painter);
            DrawText(pRT, m_strTime.c_str(), m_strTime.length(), rcText, 38);
            AfterPaint(pRT, painter);
        }
        if (bPlaying)
        {
			// Playging
			if (m_pfPlayingSkin)
			{
				SIZE szImg = m_pfPlayingSkin->GetSkinSize();
				m_rcfPlayRect = CRect(8,
					rcClient.top + (rcClient.Height() - szImg.cy) / 2,
					22,
					rcClient.top + (rcClient.Height() - szImg.cy) / 2 + szImg.cy);

				m_pfPlayingSkin->Draw(pRT, m_rcfPlayRect, 0);
			}
			
            SStringT sText = L"播放中";
			if (TRUE ==m_bPause)
			{
				sText = L"暂停中";
			}
            CRect rcText;
            GetTextRect(&rcText);
            rcText.right -= m_nOffset;
           
            SPainter painter;
            BeforePaint(pRT, painter);
            pRT->SetTextColor(m_crKey);
            pRT->DrawText(sText, sText.GetLength(), rcText, 38);
            AfterPaint(pRT, painter);
        }
		else
		{
			SStringT sText = L"";
			CRect rcText;
			GetTextRect(&rcText);
			rcText.right -= m_nOffset;

			SPainter painter;
			BeforePaint(pRT, painter);
			pRT->SetTextColor(m_crKey);
			pRT->DrawText(sText, sText.GetLength(), rcText, 38);
			AfterPaint(pRT, painter);
		}
    }

    void SItemWnd::OnMouseHover(WPARAM wParam, CPoint ptPos)
    {
        SetMsgHandled(FALSE);
    }

    void SItemWnd::OnMouseMove(UINT nFlags, CPoint point)
    {
        SetMsgHandled(FALSE);
        if (m_bError) {
            m_bfBtnHover = FALSE;
            return;
        }
        if (PtInRect(m_rcfBtnRect, point)) {
            if (!m_bfBtnHover) {
                m_bfBtnHover = TRUE;
                Invalidate();
            }
        }
        else {
            if (m_bfBtnHover) {
                m_bfBtnHover = FALSE;
                Invalidate();
            }
        }
    }

    void SItemWnd::OnRButtonDown(UINT nFlags, CPoint point)
    {
        SetMsgHandled(FALSE);
        ::PostMessage(g_hListWnd, UM_LIST_RBUTTONDOWN, GetID(), MAKELPARAM(point.x, point.y));
    }

    void SItemWnd::OnLButtonDown(UINT nFlags, CPoint point)
    {
        SetMsgHandled(FALSE);
    }

    void SItemWnd::OnLButtonUp(UINT nFlags, CPoint point)
    {
        m_uData = 0;
        if (PtInRect(m_rcfBtnRect, point))
            m_uData = 1;
        else
            m_uData = 0;
        SetMsgHandled(FALSE);
    }

    void SItemWnd::OnMouseLeave()
    {
        SetMsgHandled(FALSE);
    }

    SStringT SItemWnd::GetItemIndex()
    {
        return m_strIndex;
    }
}
