#pragma once
#include "../VLC\AVPlayer.h"

namespace SOUI
{
    class SVlvCtrl : public SWindow
    {
        SOUI_CLASS_NAME(SVlvCtrl, L"vlcctrl")
    public:
        SVlvCtrl(){
            m_hPlayerWnd = NULL;
            m_nRatio = 0;
            m_bPaint = TRUE;
            m_hOldDc = NULL;
        }
        void ReSetPaint(BOOL b);
        void SetHWND(HWND hWnd);
        void SetMsgHWND(HWND hWnd);
        HWND GetHWND();
        void PlayFile(const char* pPath);
        void SetSubtitlefile(const char* pPath);
        int GetSubtitleCount();
        void SetSubtitleSpu(unsigned nSpu);
        int GetSubtitleSpu();
        libvlc_track_description_t* GetSpuDescription();
        void SetAspectRatio(const char* lpRatio);//设置屏幕比例
        void SetAspectRatio(int nRatio);//0：原始 1：16:9 2：4:3 3：铺满
        void Stop();
        void SeekTo(__int64 llTime);//播放进度跳动到
        BOOL IsMute();
        void SetMute(BOOL b);
        void SetVolume(int nVoulme);
        void SeekForward(int nSpeed);//快进
        void SeekBackward(int nSpeed);//快退
        BOOL VolumeIncrease(int nPos);//音量加
        BOOL VolumeReduce(int nPos);//音量减
        int GetVolume();//获取当前音量
        void Pause();
        int IsPlaying();//文件是否正在播放
        __int64 GetTotalTime();//获取视频总时长
        __int64 GetNowTime();//获取当前播放时间
        void SetAudio(BOOL b);
        void GetVideoSize(int *w, int *h);// 获取视频宽高
        void SetAudioChannel(int nChannel);
        int GetAudioChannel();
    protected:
        void OnPaint(IRenderTarget *pRT);
        static void StartPlaying(void *data);		// 开始播放
        static void PosChanging(void *data);		// 播放进度改变
        static void Playend(void *data);			// 播放结束

        SOUI_MSG_MAP_BEGIN()
            MSG_WM_PAINT_EX(OnPaint)
        SOUI_MSG_MAP_END()

    private:
        CAVPlayer m_avPlayer;
        HWND m_hPlayerWnd;
        static HWND m_hMsgWnd;
        int m_nRatio;
        BOOL m_bPaint;
        HDC m_hOldDc;
    };
}
