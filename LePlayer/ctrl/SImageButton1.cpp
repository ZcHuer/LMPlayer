#include "../stdafx.h"
#include "SImageButton1.h"
namespace SOUI
{
    void SImageButton1::OnPaint(IRenderTarget *pRT)
    {
        CRect rcClient;
        GetClientRect(&rcClient);

        CRect rcText;
        GetTextRect(&rcText);

        if (!m_pBgSkin)
        {
            COLORREF crBg = m_style.m_crBg;
            if ((GetState()&WndState_Hover)
                && m_crBgEx != CR_INVALID)
                crBg = m_crBgEx;
            if (CR_INVALID != crBg){
                pRT->FillSolidRect(&rcClient, crBg);
            }
        }
        else
        {
            SIZE szImg = m_pBgSkin->GetSkinSize();
            if(!m_pImg&&m_strSkinPath.GetLength())
                m_pImg = SResLoadFromFile::LoadImage(m_strSkinPath);

            if (m_pImg)
                szImg = m_pImg->Size();

            if (!m_bOriginal)
                szImg.cx = rcClient.Width(); szImg.cy = rcClient.Height();
            CRect rcImg = CRect(rcClient.left + m_szImgOffset.cx, rcClient.top
                + m_szImgOffset.cy, rcClient.left + m_szImgOffset.cx + szImg.cx,
                rcClient.top + m_szImgOffset.cy + szImg.cy);

            if (m_pImg)
            {
                CRect rcImg(CPoint(0, 0), m_pImg->Size());
                pRT->DrawBitmapEx(rcClient, m_pImg, &rcImg, MAKELONG(EM_STRETCH, kNone_FilterLevel));
            }
            else
            {
                int nState = 0;
                if (m_pBgSkin->GetStates() == 3)
                {
                    if (GetState()&WndState_Disable)
                        nState = 2;
                    else if (GetState()&WndState_Check || 
                        GetState()&WndState_PushDown || GetState()&WndState_Hover)
                        nState = 1;
                }
                
                if(m_pEnableSkin&&m_bDisable)
                    m_pEnableSkin->Draw(pRT, rcImg, 0);
                else if(m_pFocusSkin && (GetState()&WndState_Hover))
                    m_pFocusSkin->Draw(pRT, rcImg, 0);
                else
                    m_pBgSkin->Draw(pRT, rcImg, nState);
            }
            rcText = CRect(rcClient.left - 5, rcImg.bottom, rcClient.right + 5, rcClient.bottom);
            if (m_strStartAlign.Compare(_T("right"))==0)
                rcText = CRect(rcImg.right+m_nOffset, rcClient.top,
                    rcClient.right, rcClient.bottom);
            if (m_strStartAlign.Compare(_T("center")) == 0){
                rcText = rcClient;
            }
        }

        if (m_bShowRect)
        {
            CRect rcWnd = GetWindowRect();
            CPoint pts[5];
            pts[0].x = rcWnd.left;
            pts[0].y = rcWnd.top;

            pts[1].x = rcWnd.right - 1;
            pts[1].y = rcWnd.top;

            pts[2].x = rcWnd.right - 1;
            pts[2].y = rcWnd.bottom - 1;

            pts[3].x = rcWnd.left;
            pts[3].y = rcWnd.bottom - 1;

            pts[4].x = rcWnd.left;
            pts[4].y = rcWnd.top;

            CAutoRefPtr<IPen> curPen, oldPen;
            pRT->CreatePen(PS_SOLID, m_crFocus, 1, &curPen);
            pRT->SelectObject(curPen, (IRenderObj**)&oldPen);
            pRT->DrawLines(pts, 5);
            pRT->SelectObject(oldPen);
            curPen.Release();
            oldPen.Release();
        }

        SPainter painter;
        BeforePaint(pRT, painter);
        DrawText(pRT, m_strText, m_strText.GetLength(), rcText, GetTextAlign());
        AfterPaint(pRT, painter);
    }
    CSize SImageButton1::GetDesiredSize(LPCRECT pRcContainer)
    {
        CSize szRet;
        if (m_pBgSkin){
            m_pBgSkin->GetSkinSize();
        }else{
            return szRet;
        }
        if (szRet.cx == 0 || szRet.cy == 0)
            szRet = __super::GetDesiredSize(pRcContainer);
        return szRet;
    }
    HRESULT SImageButton1::OnAttrUrl(const SStringW & strValue, BOOL bLoading)
    {
        m_strSkinPath = strValue;
        if (m_pImg) {
            m_pImg->Release();
            m_pImg = nullptr;
        }
        m_pImg = SResLoadFromFile::LoadImage(strValue);
        return E_NOTIMPL;
    }
    void SImageButton1::ShowRect(BOOL bShow)
    {
        m_bShowRect = bShow;
    }
}
