#include "stdafx.h"
#include "PlayerWnd.h"

CPlayerWnd::CPlayerWnd()
{

}

CPlayerWnd::~CPlayerWnd()
{
}

LRESULT CPlayerWnd::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandel)
{
	DWORD dw = ::GetCurrentThreadId();
	PAINTSTRUCT ps;
	HDC hDc = ::BeginPaint(m_hWnd, &ps);
	Graphics gra(hDc);
	gra.Clear(Gdiplus::Color(255, 0, 0, 0));
	::EndPaint(m_hWnd, &ps);
	ReleaseDC(hDc);
	return DefWindowProc();
}