#include "souistd.h"
#include "control/SSliderBar.h"

namespace SOUI
{

#define TIMERID_NOTIFY1     1
#define TIMERID_DELAY1      2

//////////////////////////////////////////////////////////////////////////
//  SSliderBar
    SSliderBar::SSliderBar()
        : m_bDrag(FALSE)
        , m_uHtPrev(-1)
        , m_pSkinThumb(GETBUILTINSKIN(SKIN_SYS_SLIDER_THUMB))
        , m_bThumbInRail(FALSE)
        , m_bThumbZero(FALSE)
        , m_bReversal(FALSE)
        , m_bFocusChange(FALSE)
{
    m_evtSet.addEvent(EVENTID(EventSliderPos));
	m_bValueShow = FALSE;
    m_crAB = RGBA(255,255,255,255);
    m_llApos = m_llBpos = 0;
    m_nThumbResize = 0;
}

SSliderBar::~SSliderBar()
{
}

int SSliderBar::HitTest(CPoint pt)
{
    CRect rc;
    rc = GetPartRect(SC_THUMB);
    rc.left += m_nThumbResize;
    rc.right -= m_nThumbResize;
    if (rc.PtInRect(pt))
        return SC_THUMB;

    rc = GetPartRect(SC_SELECT);
    if (rc.PtInRect(pt))
        return SC_SELECT;

    rc = GetPartRect(SC_RAIL);
    if (rc.PtInRect(pt))
        return SC_RAIL;

    return -1;
}

bool SSliderBar::isDrag()
{
	return m_bDrag; 
}

void SSliderBar::SetApos()
{
    m_llApos = GetValue();
}

void SSliderBar::SetBpos()
{
    m_llBpos = GetValue();
    if (m_llApos>m_llBpos)
    {
        m_llApos = 0;
    }
}

void SSliderBar::RemoveAB()
{
    m_llApos = m_llBpos = 0;
}

bool SSliderBar::IsHaveAB()
{
    if (m_llBpos>m_llApos)
    {
        return true;
    }
    return false;
}

bool SSliderBar::IsBpos()
{
    if (m_llBpos==GetValue())
    {
        return true;
    }
    return false;
}

bool SSliderBar::IsOutAB()
{
    __int64 llPos = GetValue();
    if ((llPos>m_llBpos|| llPos<m_llApos)&& m_llBpos!=0)
    {
        return true;
    }
    return false;
}

__int64 SSliderBar::GetApos()
{
    return m_llApos;
}

SSliderBar::RANGE SSliderBar::_GetPartRange( int nLength,
    int nThumbSize, BOOL bThumbInRail, 
    int nMin,int nMax,int nValue, 
    UINT uSBCode )
{
    int nRailLen = nLength;
    if (!m_bThumbZero&&SC_RAIL!=uSBCode&&!m_bReversal){
        nRailLen -= nThumbSize;
    }

    int nHalfThumb = nThumbSize/2;
    int nSelect = 0;
    if (nMax!= nMin){
        nSelect = (long long)nRailLen*(nValue - nMin) / (nMax - nMin);
    }
     
    RANGE rRet={0};
    switch(uSBCode)
    {
    case SC_RAILBACK://轨道背景
        rRet.value1=0;
        rRet.value2=nLength;
        break;
    case SC_RAIL:   //轨道区
        rRet.value1=0;
        rRet.value2=nLength;
        break;
    case SC_SELECT://选中
        rRet.value1=0;
        rRet.value2=nSelect;
        break;
    case SC_THUMB:
        if (m_bThumbZero)
        {
            rRet.value1 = nSelect - nHalfThumb;
            rRet.value2 = nSelect + nHalfThumb;
        }
        else
        {
            rRet.value1 = nSelect;
            rRet.value2 = nSelect + nThumbSize;
        }
        break;
    }
    
    return rRet;
}

CRect SSliderBar::GetPartRect(UINT uSBCode)
{
    SASSERT(m_pSkinThumb);
    CRect rcClient;
    GetClientRect(&rcClient);
    SIZE szThumb = m_pSkinThumb->GetSkinSize();
    SIZE szRail  = m_pSkinBg->GetSkinSize();

    if(IsVertical())
    {
        RANGE r = _GetPartRange(rcClient.Height(),szThumb.cy,m_bThumbInRail,m_nMinValue,m_nMaxValue,m_nValue,uSBCode);
        CRect rc(rcClient.left,r.value1,rcClient.right,r.value2);
        //chenwang 2018.04.14 翻转从末尾开始绘制
        if (m_bReversal&&
            (SC_THUMB == uSBCode|| SC_SELECT == uSBCode))
        {
            SIZE szPos = m_pSkinPos->GetSkinSize();
            rc.top = rcClient.top + rcClient.Height() - r.value2;
            rc.bottom = rcClient.bottom - r.value1;
            rc.left += (rcClient.Width() - szPos.cx) / 2;
            rc.right = rc.left + szPos.cx;
            if (SC_THUMB == uSBCode)
            {
                rc.left = rc.left + rc.Width() / 2 - szThumb.cx / 2;
                rc.right = rc.left + szThumb.cx;
                rc.top += szThumb.cy / 2;
                rc.bottom += szThumb.cy / 2;
            }
            return rc;
        }

        rc.OffsetRect(0,rcClient.top);
        int nSliderSize = max(szThumb.cx,szRail.cx);
        rc.DeflateRect((rcClient.Width()-nSliderSize)/2,0);
        if(uSBCode != SC_THUMB){
            rc.DeflateRect((nSliderSize-szRail.cx)/2,0);
        }
        return rc;
    }else
    {
		//
		RANGE r = _GetPartRange(rcClient.Width(), szThumb.cx, m_bThumbInRail, m_nMinValue, m_nMaxValue, m_nValue, uSBCode);
		CRect rc(r.value1, rcClient.top, r.value2, rcClient.bottom);
		rc.OffsetRect(rcClient.left, 0);
		int   nSliderSize = max(szThumb.cy, szRail.cy);
		rc.DeflateRect(0, (rcClient.Height() - nSliderSize) / 2);
		if (uSBCode != SC_THUMB)
		{
			rc.DeflateRect(0, (nSliderSize - szRail.cy) / 2);
		}
		else
		{
			//rc.left -= szThumb.cx / 2;
			//rc.right = rc.left + szThumb.cx;
		}
		return rc;

        return rc;
    }
}

void SSliderBar::OnPaint(IRenderTarget * pRT)
{
    SASSERT(m_pSkinThumb && m_pSkinBg && m_pSkinPos);

    SPainter painter;

    BeforePaint(pRT, painter);

    CRect rcRail=GetPartRect(SC_RAILBACK);
    if (m_bFocusChange&&!m_bValueShow)
        rcRail.top = rcRail.bottom - 2;
    m_pSkinBg->Draw(pRT,rcRail,0);
    if(m_nValue!=m_nMinValue)
    {
        CRect rcSel=GetPartRect(SC_SELECT);
        if (m_bFocusChange&&!m_bValueShow)
            rcSel.top = rcSel.bottom - 2;
        m_pSkinPos->Draw(pRT,rcSel,0);
    }
    if (!m_bFocusChange||
        (m_bFocusChange&&m_bValueShow))
    {
        CRect rcThumb = GetPartRect(SC_THUMB);
        int nState = 0;//normal
        if (m_bDrag) nState = 2;//pushback
        else if (m_uHtPrev == SC_THUMB) nState = 1;//hover
        if (m_pSkinThumb->GetStates() == 1)nState = 0;
        m_pSkinThumb->Draw(pRT, rcThumb, nState);
    }
    
    CRect rc = GetWindowRect();

    if (m_llApos!=0)
    {
        SStringT str = _T("A");
        int nSelect = (long long)rcRail.Width()*(m_llApos - m_nMinValue) / (m_nMaxValue - m_nMinValue);
        CRect rcText(rcRail.left + nSelect-5, rcRail.top, rcRail.left+nSelect+5, rcRail.bottom+5);
        pRT->SetTextColor(m_crAB);
        pRT->DrawTextW(str, str.GetLength(), rcText, DT_SINGLELINE | DT_VCENTER);
    }
    if (m_llBpos!=0)
    {
        SStringT str = _T("B");
        int nSelect = (long long)rcRail.Width()*(m_llBpos - m_nMinValue) / (m_nMaxValue - m_nMinValue);
        CRect rcText(rcRail.left + nSelect - 5, rcRail.top, rcRail.left + nSelect + 5, rcRail.bottom + 5);
        pRT->SetTextColor(m_crAB);
        pRT->DrawTextW(str, str.GetLength(), rcText, DT_SINGLELINE | DT_VCENTER);
    }
    
    AfterPaint(pRT, painter);
}

void SSliderBar::OnLButtonUp(UINT nFlags, CPoint point)
{
    ReleaseCapture();
	m_bValueShow = FALSE;
    if (m_bDrag)
    {
        m_bDrag = FALSE;
		/// 陈旺 @2017.01.03 修改拖动之后任然回到初始播放位置
		Invalidate();
		NotifyPos(SC_THUMB, m_nValue);
		///end
    }
    OnMouseMove(nFlags,point);
}

void SSliderBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
    SetCapture();
	m_bValueShow = TRUE;
    UINT uHit = HitTest(point);
    if (uHit == SC_THUMB)
    {
        m_bDrag    = TRUE;
        m_ptDrag   = point;
        m_nDragValue=m_nValue;
        Invalidate();
    }
    else
    {
        CRect rcRail=GetPartRect(SC_RAIL);
        int nValue=0;
        if(IsVertical())
        {
            nValue= (point.y-rcRail.top)*(m_nMaxValue-m_nMinValue+1)/rcRail.Height()+m_nMinValue;
        }else
        {
            nValue= (long long)(point.x-rcRail.left)*(m_nMaxValue-m_nMinValue+1)/rcRail.Width()+m_nMinValue;
        }
        //chenwang 2018.04.14 翻转起始位置
        if (m_bReversal){
            nValue = m_nMaxValue - nValue;
        }
        //end
        SetValue(nValue);
        //NotifyPos(SC_THUMB,m_nValue);
        //Invalidate();

        m_bDrag    = TRUE;
        m_ptDrag   = point;
        m_nDragValue=m_nValue;
    }
}

void SSliderBar::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_bValueShow = TRUE;
    if (m_bDrag)
    {
        CRect rcRail=GetPartRect(SC_RAIL);
		
        int nInterHei=(IsVertical()?rcRail.Height():rcRail.Width());
        int nDragLen=IsVertical()?(point.y-m_ptDrag.y):(point.x-m_ptDrag.x);
        //chenwang 2018.04.14 翻转起始位置
        if (m_bReversal) {
            nDragLen = IsVertical() ? (m_ptDrag.y - point.y) : (m_ptDrag.x - point.x);
        }
        //end
        
        int nSlide=(long long)nDragLen*(m_nMaxValue-m_nMinValue+1)/nInterHei;

        int nNewTrackPos=m_nDragValue+nSlide;
        if(nNewTrackPos<m_nMinValue)
        {
            nNewTrackPos=m_nMinValue;
        }
        else if(nNewTrackPos>m_nMaxValue)
        {
            nNewTrackPos=m_nMaxValue;
        }
       
        if(nNewTrackPos!=m_nValue)
        {
            m_nValue=nNewTrackPos;
            Invalidate();
            NotifyPos(SC_THUMB,m_nValue);
        }
    }
    else
    {
        int uHit = HitTest(point);
        if (uHit != m_uHtPrev && (m_uHtPrev==SC_THUMB || uHit==SC_THUMB))
        {
            m_uHtPrev = uHit;
            CRect rcThumb = GetPartRect(SC_THUMB);
            InvalidateRect(rcThumb);
        }
    }
    Invalidate();
}

void SSliderBar::OnMouseLeave()
{
	m_bValueShow = FALSE;
    if (!m_bDrag && m_uHtPrev==SC_THUMB)
    {
        CRect rcThumb = GetPartRect(SC_THUMB);
        m_uHtPrev=-1;
    }
    Invalidate();
}

LRESULT SSliderBar::NotifyPos(UINT uCode, int nPos)
{
    EventSliderPos evt(this);
    evt.nPos = nPos;

    return FireEvent(evt);
}

CSize SSliderBar::GetDesiredSize(LPCRECT pRcContainer)
{
    SASSERT(m_pSkinBg && m_pSkinThumb);
    CSize szRet;
    SIZE sizeBg = m_pSkinBg->GetSkinSize();
    SIZE sizeThumb= m_pSkinThumb->GetSkinSize();
    
    if(IsVertical())
    {
        szRet.cx=max(sizeBg.cx,sizeThumb.cx);
        szRet.cy=sizeBg.cy;
    }else
    {
        szRet.cy=max(sizeBg.cy,sizeThumb.cy);
        szRet.cx= sizeBg.cx;
    }
    return szRet;
}

void SSliderBar::OnColorize(COLORREF cr)
{
    __super::OnColorize(cr);
    if(m_pSkinThumb) m_pSkinThumb->OnColorize(cr);
}

}//end of namespace