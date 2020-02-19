#include "stdafx.h"
#include "BindDownloadCallback.h"
#include "UpdateRepire.h"

CBindDownloadCallback::CBindDownloadCallback()
{
	m_hMsgWnd = NULL;
}

CBindDownloadCallback::~CBindDownloadCallback()
{
}

//////仅实现OnProgress成员即可  
LRESULT CBindDownloadCallback::OnProgress(ULONG ulProgress,	ULONG ulProgressMax, ULONG ulSatusCode,	LPCWSTR szStatusText)
{
	if (ulSatusCode == 5)
	{
		if (m_hMsgWnd)
			::PostMessage(m_hMsgWnd, eUPDATE_NetError, ulProgress, ulProgressMax);
	}
	else
	{
		if (m_hMsgWnd)
			::PostMessage(m_hMsgWnd, eUPDATE_NetRepair, ulProgress, ulProgressMax);
	}

	if (ulProgress <= ulProgressMax)
	{
		if (m_hMsgWnd)
			::PostMessage(m_hMsgWnd, eUPDATE_Progress, ulProgress, ulProgressMax);
	}
	
	return S_OK;
}

void CBindDownloadCallback::SetMsgWndHandle(HWND hWnd)
{
	m_hMsgWnd = hWnd;
	return;
}
