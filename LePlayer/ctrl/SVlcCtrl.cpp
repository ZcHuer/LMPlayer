#include "../stdafx.h"
#include "SVlcCtrl.h"

namespace SOUI
{
    HWND SVlvCtrl::m_hMsgWnd = NULL;

    void SVlvCtrl::OnPaint(IRenderTarget * pRT)
    {
        //获取目标DC大小
        CRect rcDes;
        ::GetWindowRect(m_hPlayerWnd, rcDes);
        HDC hDc = pRT->GetDC();
        //处理DC拷贝拉伸时失真
        ::SetStretchBltMode(hDc, HALFTONE);
        ::SetBrushOrgEx(hDc, 0, 0, NULL);

        if (!m_bPaint)
        {
            ::StretchBlt(hDc, 0, 0, rcDes.Width(), rcDes.Height(), m_hOldDc, 0, 0, rcDes.Width(), rcDes.Height(), SRCCOPY);
            return;
        }
        
        //获取视频源DC
        HWND hPlayerWnd = m_hPlayerWnd; 
        hPlayerWnd = ::GetWindow(m_hPlayerWnd, GW_CHILD);
        hPlayerWnd = ::GetWindow(hPlayerWnd, GW_CHILD);
        HDC hPlayerDC = ::GetDC(hPlayerWnd);
        
        //获取源DC大小
        CRect rcSrc;
        ::GetWindowRect(hPlayerWnd, rcSrc);
        int nW=0, nH=0, nX=0, nY=0;
        switch (m_nRatio)
        {
        //原始
        case 0:
        {
            nW = rcSrc.Width();
            nH = rcSrc.Height();
            break;
        }
        //16:9
        case 1:
        {
            nW = rcDes.Width();
            nH = (float)nW / 16 * 9;
            if (nH>rcDes.Height()){
                nH = rcDes.Height();
                nW = (float)nH / 9 * 16;
            }
            break;
        }
        //4:3
        case 2:
        {
            nW = rcDes.Width();
            nH = (float)nW / 4 * 3;
            if (nH>rcDes.Height()) {
                nH = rcDes.Height();
                nW = (float)nH / 3 * 4;
            }
            break;
        }
        //铺满
        case 3:
        {
            nW = rcDes.Width();
            nH = rcDes.Height();
            break;
        }
        default:
            break;
        }
        nX = (rcDes.Width() - nW) / 2;
        nY = (rcDes.Height() - nH) / 2;
        
        ::StretchBlt(hDc, nX, nY, nW, nH, hPlayerDC, 0, 0, rcSrc.Width(), rcSrc.Height(), SRCCOPY);
        ReleaseDC(NULL,hDc);
        ReleaseDC(NULL, hPlayerDC);
    }

    void SVlvCtrl::SetHWND(HWND hWnd)
    {
        m_hPlayerWnd = hWnd;
        m_avPlayer.SetHWND(hWnd);
        m_avPlayer.SetCallbackPlaying(StartPlaying);
        m_avPlayer.SetCallbackPosChanged(PosChanging);
        m_avPlayer.SetCallbackEndReached(Playend);
       
    }
    void SVlvCtrl::PlayFile(const char * pPath)
    {
        m_avPlayer.Play(pPath);
    }

    void SVlvCtrl::SetSubtitlefile(const char * pPath)
    {
        m_avPlayer.SetSubtitlefile(pPath);
    }

    HWND SVlvCtrl::GetHWND()
    {
        return m_hPlayerWnd;
    }

    void SVlvCtrl::Stop()
    {
        m_avPlayer.Stop();
    }

    void SVlvCtrl::StartPlaying(void * data)
    {
        ::PostMessage(m_hMsgWnd,UM_PLAY_START,0,0);
    }

    void SVlvCtrl::PosChanging(void * data)
    {
        ::PostMessage(m_hMsgWnd, UM_PLAYING, 0, 0);
    }

    void SVlvCtrl::Playend(void * data)
    {
        ::PostMessage(m_hMsgWnd, UM_PLAY_END, 0, 0);
    }
    void SVlvCtrl::SetMsgHWND(HWND hWnd)
    {
        m_hMsgWnd = hWnd;
    }
    __int64 SVlvCtrl::GetTotalTime()
    {
        return m_avPlayer.GetTotalTime();
    }
    __int64 SVlvCtrl::GetNowTime()
    {
        return m_avPlayer.GetTime();
    }
    void SVlvCtrl::SeekTo(__int64 llTime)
    {
        m_avPlayer.SeekTo(llTime);
    }
    BOOL SVlvCtrl::IsMute()
    {
        return m_avPlayer.IsMute();
    }
    void SVlvCtrl::SetMute(BOOL b)
    {
        m_avPlayer.SetMute(b);
    }
    void SVlvCtrl::SetVolume(int nVoulme)
    {
        m_avPlayer.Volume(nVoulme);
    }
    void SVlvCtrl::Pause()
    {
        m_avPlayer.Pause();
    }
    int SVlvCtrl::IsPlaying()
    {
        return m_avPlayer.IsPlaying();
    }
    void SVlvCtrl::SetAspectRatio(const char * lpRatio)
    {
        return m_avPlayer.SetAspectRatio(lpRatio);
    }
    void SVlvCtrl::SeekForward(int nSpeed)
    {
        m_avPlayer.SeekForward(nSpeed);
    }
    void SVlvCtrl::SeekBackward(int nSpeed)
    {
        m_avPlayer.SeekBackward(nSpeed);
    }
    BOOL SVlvCtrl::VolumeIncrease(int nPos)
    {
        return m_avPlayer.VolumeIncrease(nPos);
    }
    BOOL SVlvCtrl::VolumeReduce(int nPos)
    {
        return m_avPlayer.VolumeReduce(nPos);
    }
    int SVlvCtrl::GetVolume()
    {
        return m_avPlayer.GetVolume();
    }
    int SVlvCtrl::GetSubtitleCount()
    {
        return m_avPlayer.GetSubtitleCount();
    }
    void SVlvCtrl::SetSubtitleSpu(unsigned nSpu)
    {
        m_avPlayer.SetSubtitleSpu(nSpu);
    }
    libvlc_track_description_t* SVlvCtrl::GetSpuDescription()
    {
        return m_avPlayer.GetSpuDescription();
    }

    int SVlvCtrl::GetSubtitleSpu()
    {
        return m_avPlayer.GetSubtitleSpu();
    }
    void SVlvCtrl::GetVideoSize(int * w, int * h)
    {
        m_avPlayer.GetVideoSize(w,h);
    }
    void SVlvCtrl::SetAspectRatio(int nRatio)
    {
        m_nRatio = nRatio;
    }
    void SVlvCtrl::SetAudioChannel(int nChannel)
    {
        m_avPlayer.SetAudioChannel(nChannel);
    }
    int SVlvCtrl::GetAudioChannel()
    {
        return m_avPlayer.GetAudioChannel();
    }
    void SVlvCtrl::ReSetPaint(BOOL b)
    {
        m_bPaint = b;
        if (m_hOldDc)
        {
            ::ReleaseDC(m_hPlayerWnd, m_hOldDc);
            m_hOldDc = NULL;
        }
        m_hOldDc = GetRenderTarget()->GetDC();
        Invalidate();
    }
}


