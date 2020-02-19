// PingThread.cpp: implementation of the CPingThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PingThread.h"
#include "process.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPingThread::CPingThread()
{
	m_dwID = 0;  
	m_hThread = NULL;
	//创建信号事件
	m_hKEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hSEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	//开始一个ping线程
	m_hThread = (HANDLE)_beginthreadex(NULL,0,ThreadProc,(void*)this,0,&m_dwID);					
}

CPingThread::~CPingThread()
{
	TerminateThread(m_hThread,0);
	
	CloseHandle(m_hKEvent);
	CloseHandle(m_hSEvent);

	CloseHandle( m_hThread );
}
//ping线程过程函数
UINT CPingThread::ThreadProc(void* lpParam)
{ 
	CPingThread* pThis = reinterpret_cast<CPingThread*>(lpParam);
	while(1)
	{
		HANDLE hObjects[2];
		hObjects[0] = pThis->m_hKEvent;
		hObjects[1] = pThis->m_hSEvent;

		//等待信号有效
		DWORD dwWait = WaitForMultipleObjects(2,hObjects,FALSE,INFINITE);
		if (dwWait == WAIT_OBJECT_0 || dwWait==WAIT_FAILED)		
			break;

		//开始ping
		if (dwWait == WAIT_OBJECT_0 + 1)
			pThis->m_ping.Ping(pThis->m_nRetries,pThis->m_strHost,pThis->m_hWnd,pThis->m_nMsgID);
	}
	return 0;
}
//开始ping
void CPingThread::StartPing(UINT nRetries,LPCTSTR strHost,HWND hWnd,UINT nMsgID)
{
	m_strHost = strHost;
	m_hWnd = hWnd;
	m_nMsgID=nMsgID;
	m_nRetries=nRetries;
	//设置信号有效，可以开始线程了
	SetEvent(m_hSEvent);
}

void CPingThread::ExitThread()
{
	SetEvent(m_hKEvent);
	WaitForSingleObject(m_hThread,3000);
}