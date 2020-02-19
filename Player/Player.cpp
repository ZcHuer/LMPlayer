// Player.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "Player.h"
#include "ffplayer.h"

CFFPlayer g_ffplayer;

PLAYER_API CPlayer::CPlayer()
{

}

PLAYER_API CPlayer::~CPlayer()
{

}

bool PLAYER_API CPlayer::CreatePlayer(HWND hWndParent, LPRECT lpRect)
{
	return g_ffplayer.CreatePlayer(hWndParent, lpRect);
}

void PLAYER_API CPlayer::AttachCallBack(IPlayerCallBack* pCallBack)
{
	return g_ffplayer.AttachCallBack(pCallBack);
}

ePlayStatus CPlayer::GetPlayStatus()
{
	return g_ffplayer.GetPlayStatus();
}

void PLAYER_API CPlayer::ReWndSize(int nLeft, int nTop, int nRight, int nBottom)
{
	return g_ffplayer.ReWndSize(nLeft, nTop, nRight, nBottom);
}

void PLAYER_API CPlayer::OpenAndPlay(const char* pch)
{
	return g_ffplayer.OpenAndPlay(pch);
}

void PLAYER_API CPlayer::Play()
{
	return g_ffplayer.Play();
}

void PLAYER_API CPlayer::Pause()
{
	return g_ffplayer.Pause();
}

void PLAYER_API CPlayer::Seek(LONGLONG dbPos)
{
	return g_ffplayer.Seek(dbPos);
}

void PLAYER_API CPlayer::SetPlayRate(double dbRate)
{
	return g_ffplayer.SetPlayRate(dbRate);
}

double PLAYER_API CPlayer::GetPlayRate()
{
	return g_ffplayer.GetPlayRate();
}

void PLAYER_API CPlayer::SetVolume(int nVolume)
{
	return g_ffplayer.SetVolume(nVolume);
}

int PLAYER_API CPlayer::GetVolume()
{
	return g_ffplayer.GetVolume();
}

void CPlayer::SetMute(bool bMute)
{
	return g_ffplayer.SetMute(bMute);
}

bool PLAYER_API CPlayer::GetMute()
{
	return g_ffplayer.GetMute();
}

void PLAYER_API CPlayer::SetAudioChannel(int nChannel)
{
	return g_ffplayer.SetAudioChannel(nChannel);
}

int PLAYER_API CPlayer::GetAudioChannel()
{
	return g_ffplayer.GetAudioChannel();
}

bool PLAYER_API CPlayer::SetVideoSize(long pWidth, long pHeight)
{
	return g_ffplayer.SetVideoSize(pWidth, pHeight);
}

bool PLAYER_API CPlayer::GetVideoSize(long *pWidth, long* pHeight)
{
	return g_ffplayer.GetVideoSize(pWidth, pHeight);
}

void PLAYER_API CPlayer::SetSubtitlefile(const char* lpPath)
{
	return g_ffplayer.SetSubtitlefile(lpPath);
}

int PLAYER_API CPlayer::GetSubtitleCount()
{
	return g_ffplayer.GetSubtitleCount();
}

void PLAYER_API CPlayer::SetSubtitleSpu(unsigned nSpu)
{
	return g_ffplayer.SetSubtitleSpu(nSpu);
}

int PLAYER_API CPlayer::GetSubtitleSpu()
{
	return g_ffplayer.GetSubtitleSpu();
}

void PLAYER_API CPlayer::SetAspectRatio(int nScale)
{
	return g_ffplayer.SetAspectRatio(nScale);
}

int PLAYER_API CPlayer::GetAspectRatio()
{
	return g_ffplayer.GetAspectRatio();
}

void PLAYER_API CPlayer::Stop()
{
	return g_ffplayer.Stop();
}

void PLAYER_API CPlayer::Quit()
{
	return g_ffplayer.Quit();
}
