#pragma once
#include <atlwin.h>
#include <atlbase.h>
#include <atlapp.h>
#include <atlframe.h>
#include "ffplay.h"
#include "PlayerWnd.h"
#include "MsgWnd.h"

// �������߳���Ϣ
#define WM_PlayerThread_CMD_Start			(WM_USER + 200)
#define WM_PlayerThread_Init			    (WM_PlayerThread_CMD_Start)
#define WM_PlayerThread_PreOpen				(WM_PlayerThread_Init + 1)
#define WM_PlayerThread_Open			    (WM_PlayerThread_PreOpen + 1)
#define WM_PlayerThread_Quit			    (WM_PlayerThread_Open + 1)		// ����Ϣ��Ҫ���˳���Ⱦѭ���ſ��Ե���
#define WM_PlayerThread_CMD_End				(WM_USER + 400)

// �������ӿ���
class CFFPlayer
{
public:
	CFFPlayer();
	~CFFPlayer();
	
	bool CreatePlayer(HWND hWndParent, LPRECT lpRect);				// �������������ڲ������߳�
	void AttachCallBack(IPlayerCallBack* pCallBack);				// ���ûص�ָ��	
	void PlayCmdThread();											// �̺߳�����������Ϣѭ������Ϣ������
	void DestoryPlayCmdThread();
	HWND GetPlayerWnd();											// �������
	void ReWndSize(int nLeft, int nTop, int nRight, int nBottom);
	ePlayStatus GetPlayStatus();
	// �������-���ⲿ����
	void OpenAndPlay(const char* pch);
	void Play();
	void Pause();
	void Stop();
	void Seek(LONGLONG dbPos);
	void SetPlayRate(double dbRate);
	// ����ȡ�����̺߳�д�����߳�Ϊ��������ü�������
	double GetPlayRate();
	void SetVolume(int nVolume);
	int GetVolume();
	void SetMute(bool bMute);
	bool GetMute();
	void SetAudioChannel(int nChannel);
	int GetAudioChannel();
	bool SetVideoSize(long pWidth, long pHeight);
	bool GetVideoSize(long *pWidth, long* pHeight);
	void SetSubtitlefile(const char* lpPath);								// �����Ļ
	int GetSubtitleCount();													// ��ȡ������Ļ����
	void SetSubtitleSpu(unsigned nSpu);
	//libvlc_track_description_t* GetSpuDescription();
	int GetSubtitleSpu();
	void SetAspectRatio(int nScale);										// ������Ļ�ݺ��
	int GetAspectRatio();													// ��ȡ��Ļ�ݺ��
	bool SetEventMessage(HWND hEventWnd, UINT uEventMsg);
	bool OnMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void Close();
	void Quit();

	// �߳���
	void Open(char* p);
	CPlayerWnd m_PlayerWnd;
	CMsgWnd	m_MsgWnd;
	// �߳����
	HANDLE	m_hPlayCmdThread;
	unsigned int m_playCmdThreadID;
	HANDLE	m_hEvent_Create;
	HANDLE	m_hEvent_Init;
	HANDLE	m_hEvent_Stop;

	// �������
	HWND m_hWndParent;
	int m_nCtrlHeight;
	static CFFPlayer* m_pThis;
	// ���ſ����
	int m_nflags;
	bool m_bMute;

	int64_t m_duration;
	int64_t m_seekpos;
	int m_nVolume;
	int m_nAudioChannel;
};