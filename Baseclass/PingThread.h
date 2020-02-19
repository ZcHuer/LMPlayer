// PingThread.h: interface for the CPingThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PINGTHREAD_H__88E7C296_2B7A_4A67_AA7B_C89EA8D3BAAC__INCLUDED_)
#define AFX_PINGTHREAD_H__88E7C296_2B7A_4A67_AA7B_C89EA8D3BAAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Ping.h"
class CPingThread  
{
public:
	CPingThread();
	virtual ~CPingThread();
	static UINT __stdcall  ThreadProc(void* pThis);
	void StartPing(UINT nRetries,LPCTSTR strHost,HWND hWnd,UINT nMsgID);
	void ExitThread();
private:
	CPing	m_ping;				//ping对象
	CString m_strHost;			//主机地址
	HWND	m_hWnd;				//主窗口
	UINT    m_nMsgID;           //返回消息ID
	UINT	m_nRetries;         //尝试次数

	UINT	m_dwID;				//线程ID
	HANDLE	m_hThread;			//线程句柄
	HANDLE	m_hKEvent;		
	HANDLE	m_hSEvent;
};

#endif // !defined(AFX_PINGTHREAD_H__88E7C296_2B7A_4A67_AA7B_C89EA8D3BAAC__INCLUDED_)
