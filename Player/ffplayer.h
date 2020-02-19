#pragma once
#include <atlwin.h>
#include <atlbase.h>
#include <atlapp.h>
#include <atlframe.h>
#include "ffplay.h"
#include "PlayerWnd.h"
#include "MsgWnd.h"

// 播放器线程消息
#define WM_PlayerThread_CMD_Start			(WM_USER + 200)
#define WM_PlayerThread_Init			    (WM_PlayerThread_CMD_Start)
#define WM_PlayerThread_PreOpen				(WM_PlayerThread_Init + 1)
#define WM_PlayerThread_Open			    (WM_PlayerThread_PreOpen + 1)
#define WM_PlayerThread_Quit			    (WM_PlayerThread_Open + 1)		// 此消息需要先退出渲染循环才可以调用
#define WM_PlayerThread_CMD_End				(WM_USER + 400)

// 播放器接口类
class CFFPlayer
{
public:
	CFFPlayer();
	~CFFPlayer();
	
	bool CreatePlayer(HWND hWndParent, LPRECT lpRect);				// 创建播放器，内部启动线程
	void AttachCallBack(IPlayerCallBack* pCallBack);				// 设置回调指针	
	void PlayCmdThread();											// 线程函数，内有消息循环和消息处理函数
	void DestoryPlayCmdThread();
	HWND GetPlayerWnd();											// 窗口相关
	void ReWndSize(int nLeft, int nTop, int nRight, int nBottom);
	ePlayStatus GetPlayStatus();
	// 播放相关-可外部调用
	void OpenAndPlay(const char* pch);
	void Play();
	void Pause();
	void Stop();
	void Seek(LONGLONG dbPos);
	void SetPlayRate(double dbRate);
	// 以下取数据线程和写数据线程为两个，最好加锁保护
	double GetPlayRate();
	void SetVolume(int nVolume);
	int GetVolume();
	void SetMute(bool bMute);
	bool GetMute();
	void SetAudioChannel(int nChannel);
	int GetAudioChannel();
	bool SetVideoSize(long pWidth, long pHeight);
	bool GetVideoSize(long *pWidth, long* pHeight);
	void SetSubtitlefile(const char* lpPath);								// 添加字幕
	int GetSubtitleCount();													// 获取可用字幕数量
	void SetSubtitleSpu(unsigned nSpu);
	//libvlc_track_description_t* GetSpuDescription();
	int GetSubtitleSpu();
	void SetAspectRatio(int nScale);										// 设置屏幕纵横比
	int GetAspectRatio();													// 获取屏幕纵横比
	bool SetEventMessage(HWND hEventWnd, UINT uEventMsg);
	bool OnMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void Close();
	void Quit();

	// 线程内
	void Open(char* p);
	CPlayerWnd m_PlayerWnd;
	CMsgWnd	m_MsgWnd;
	// 线程相关
	HANDLE	m_hPlayCmdThread;
	unsigned int m_playCmdThreadID;
	HANDLE	m_hEvent_Create;
	HANDLE	m_hEvent_Init;
	HANDLE	m_hEvent_Stop;

	// 窗口相关
	HWND m_hWndParent;
	int m_nCtrlHeight;
	static CFFPlayer* m_pThis;
	// 播放库相关
	int m_nflags;
	bool m_bMute;

	int64_t m_duration;
	int64_t m_seekpos;
	int m_nVolume;
	int m_nAudioChannel;
};