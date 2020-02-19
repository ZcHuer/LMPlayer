#include "stdafx.h"
#include "BindDownloadCallback.h"
#include "Update.h"

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
	//CString cstr;
	//cstr.Format(L"%d-%d-%d--->>>> %s \n", ulSatusCode, ulProgress, ulProgressMax, szStatusText);
	//OutputDebugStringW(cstr.GetString());
	if (ulProgress > 0)
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
