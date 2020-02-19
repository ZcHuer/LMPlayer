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
	CPing	m_ping;				//ping����
	CString m_strHost;			//������ַ
	HWND	m_hWnd;				//������
	UINT    m_nMsgID;           //������ϢID
	UINT	m_nRetries;         //���Դ���

	UINT	m_dwID;				//�߳�ID
	HANDLE	m_hThread;			//�߳̾��
	HANDLE	m_hKEvent;		
	HANDLE	m_hSEvent;
};

#endif // !defined(AFX_PINGTHREAD_H__88E7C296_2B7A_4A67_AA7B_C89EA8D3BAAC__INCLUDED_)
