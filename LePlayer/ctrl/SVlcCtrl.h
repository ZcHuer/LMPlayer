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
        void SetAspectRatio(const char* lpRatio);//������Ļ����
        void SetAspectRatio(int nRatio);//0��ԭʼ 1��16:9 2��4:3 3������
        void Stop();
        void SeekTo(__int64 llTime);//���Ž���������
        BOOL IsMute();
        void SetMute(BOOL b);
        void SetVolume(int nVoulme);
        void SeekForward(int nSpeed);//���
        void SeekBackward(int nSpeed);//����
        BOOL VolumeIncrease(int nPos);//������
        BOOL VolumeReduce(int nPos);//������
        int GetVolume();//��ȡ��ǰ����
        void Pause();
        int IsPlaying();//�ļ��Ƿ����ڲ���
        __int64 GetTotalTime();//��ȡ��Ƶ��ʱ��
        __int64 GetNowTime();//��ȡ��ǰ����ʱ��
        void SetAudio(BOOL b);
        void GetVideoSize(int *w, int *h);// ��ȡ��Ƶ���
        void SetAudioChannel(int nChannel);
        int GetAudioChannel();
    protected:
        void OnPaint(IRenderTarget *pRT);
        static void StartPlaying(void *data);		// ��ʼ����
        static void PosChanging(void *data);		// ���Ž��ȸı�
        static void Playend(void *data);			// ���Ž���

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
