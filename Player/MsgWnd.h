#pragma once
#include "Player.h"

class CMsgWnd
{
public:
	CMsgWnd();
	~CMsgWnd();

	void CreateMsgWnd(HWND hWndParent);
	HWND GetHwnd();
	void AttachCallBack(IPlayerCallBack* pCallBack);
	static void SetPlayStatus(ePlayStatus eStatus);

	static IPlayerCallBack* m_pCallBack;
	static ePlayStatus m_ePlayerStatus;

private:
	HWND m_hWnd;	
};