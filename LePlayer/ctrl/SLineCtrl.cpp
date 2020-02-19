#include "../stdafx.h"
#include "SLineCtrl.h"

namespace SOUI
{
    void SLineCtrl::OnPaint(IRenderTarget *pRT)
    {
        if (m_nTop)
        {
            CRect rcWnd = GetWindowRect();
            Gdiplus::Point pts[5];

            pts[0].X = rcWnd.left;
            pts[0].Y = rcWnd.bottom - 1;
            pts[1].X = rcWnd.left + m_nTop-6;
            pts[1].Y = rcWnd.bottom - 1;
            pts[2].X = rcWnd.left + m_nTop;
            pts[2].Y = rcWnd.top;
            pts[3].X = rcWnd.left + m_nTop+6;
            pts[3].Y = rcWnd.bottom - 1;
            pts[4].X = rcWnd.right;
            pts[4].Y = rcWnd.bottom - 1;

            Bitmap bmp(rcWnd.Width(), rcWnd.Height(), PixelFormat32bppPARGB);
            Graphics g(pRT->GetDC());
            Gdiplus::Pen myGdipen(Color(64, 194, 253), 1.0F);
            //设置平滑模式
            g.SetSmoothingMode(SmoothingModeAntiAlias);
            g.DrawLines(&myGdipen, pts, 5);
        }
    }
    void SLineCtrl::SetTop(int nTop)
    {
        m_nTop = nTop;
        SWindow::Invalidate();
    }
}