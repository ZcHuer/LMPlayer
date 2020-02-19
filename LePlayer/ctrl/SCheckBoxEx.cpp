#include "../stdafx.h"
#include "SCheckBoxEx.h"
namespace SOUI
{
    SCheckBoxEx::SCheckBoxEx()
    {
        m_pSkin1 = m_pSkin2 = m_pSkin3 = m_pSkin4 = nullptr;
    }

    void SCheckBoxEx::OnPaint(IRenderTarget * pRT)
    {
        m_nIconOffset = 2;
        CRect rcCheckBox = GetCheckRect();

        DWORD dwState = GetState();
        if (dwState&WndState_Hover)
            m_pSkin2->Draw(pRT, rcCheckBox, 0);
        else
            m_pSkin1->Draw(pRT, rcCheckBox, 0);

        if (dwState & WndState_Check) {
            SIZE sz = m_pSkin3->GetSkinSize();
            CRect rcFocus = CRect(0,0,0,0);
            rcFocus.left = rcCheckBox.left+(rcCheckBox.Width() - sz.cx)/2;
            rcFocus.right = rcFocus.left + sz.cx;
            rcFocus.top = rcCheckBox.top+ (rcCheckBox.Height() - sz.cy)/2;
            rcFocus.bottom = rcFocus.top + sz.cy;
            m_pSkin3->Draw(pRT, rcFocus, 0);
        }
        __super::OnPaint(pRT);
    }
}

