#include "stdafx.h"
#include "MsgWnd.h"

IPlayerCallBack* CMsgWnd::m_pCallBack = NULL;
ePlayStatus CMsgWnd::m_ePlayerStatus = ePS_Init;

CMsgWnd::CMsgWnd()
{
	m_hWnd = NULL;
}

CMsgWnd::~CMsgWnd()
{
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	ePlayerMsg ePM = (ePlayerMsg)message;
	switch (ePM)
	{
	case EPLAYERMSG_START:
	{
		CMsgWnd::SetPlayStatus(ePS_Playing);
		if (CMsgWnd::m_pCallBack)
			CMsgWnd::m_pCallBack->OnPlayStart((int)lParam);
	}
	break;
	case EPLAYERMSG_PLAYING:
	{
		if (CMsgWnd::m_pCallBack)
			CMsgWnd::m_pCallBack->OnPlaying((int)wParam, (int)lParam);
	}
	break;
	case EPLAYERMSG_END:
	{
		CMsgWnd::SetPlayStatus(ePS_Stoped);
		if (CMsgWnd::m_pCallBack)
			CMsgWnd::m_pCallBack->OnPlayEnd();
	}
	break;
	case EPLAYERMSG_FILE_INVALID:
	{
		CMsgWnd::SetPlayStatus(ePS_Stoped);
		if (CMsgWnd::m_pCallBack)
			CMsgWnd::m_pCallBack->OnFileInvalid();
	}
	break;
	default:
		break;
	}
	switch (message)
	{	
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void CMsgWnd::CreateMsgWnd(HWND hWndParent)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = 0;
	wcex.hIcon = 0;
	wcex.hCursor = 0;
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = L"CMsgWnd";
	wcex.hIconSm = 0;
	RegisterClassExW(&wcex);

	m_hWnd = CreateWindowW(L"CMsgWnd", L"PlayerMsgWnd", 0, 0, 0, 0, 0, hWndParent, NULL, NULL, NULL);	
	return;
}

HWND CMsgWnd::GetHwnd()
{
	return m_hWnd;
}

void CMsgWnd::AttachCallBack(IPlayerCallBack* pCallBack)
{
	m_pCallBack = pCallBack;
	return;
}

void CMsgWnd::SetPlayStatus(ePlayStatus eStatus)
{
	if (m_ePlayerStatus == eStatus)
		return;
	m_ePlayerStatus = eStatus;
}