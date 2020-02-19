#pragma once

// ²¥·ÅÆ÷´°¿ÚÀà
class CPlayerWnd : public  CFrameWindowImpl<CPlayerWnd>
{
public:
	CPlayerWnd();
	~CPlayerWnd();
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandel);
	DECLARE_FRAME_WND_CLASS(_T("CPlayerWnd"), NULL);
	BEGIN_MSG_MAP(CPlayerWnd)
		CHAIN_MSG_MAP(CFrameWindowImpl<CPlayerWnd>)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()
};