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
	//�����ź��¼�
	m_hKEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hSEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	//��ʼһ��ping�߳�
	m_hThread = (HANDLE)_beginthreadex(NULL,0,ThreadProc,(void*)this,0,&m_dwID);					
}

CPingThread::~CPingThread()
{
	TerminateThread(m_hThread,0);
	
	CloseHandle(m_hKEvent);
	CloseHandle(m_hSEvent);

	CloseHandle( m_hThread );
}
//ping�̹߳��̺���
UINT CPingThread::ThreadProc(void* lpParam)
{ 
	CPingThread* pThis = reinterpret_cast<CPingThread*>(lpParam);
	while(1)
	{
		HANDLE hObjects[2];
		hObjects[0] = pThis->m_hKEvent;
		hObjects[1] = pThis->m_hSEvent;

		//�ȴ��ź���Ч
		DWORD dwWait = WaitForMultipleObjects(2,hObjects,FALSE,INFINITE);
		if (dwWait == WAIT_OBJECT_0 || dwWait==WAIT_FAILED)		
			break;

		//��ʼping
		if (dwWait == WAIT_OBJECT_0 + 1)
			pThis->m_ping.Ping(pThis->m_nRetries,pThis->m_strHost,pThis->m_hWnd,pThis->m_nMsgID);
	}
	return 0;
}
//��ʼping
void CPingThread::StartPing(UINT nRetries,LPCTSTR strHost,HWND hWnd,UINT nMsgID)
{
	m_strHost = strHost;
	m_hWnd = hWnd;
	m_nMsgID=nMsgID;
	m_nRetries=nRetries;
	//�����ź���Ч�����Կ�ʼ�߳���
	SetEvent(m_hSEvent);
}

void CPingThread::ExitThread()
{
	SetEvent(m_hKEvent);
	WaitForSingleObject(m_hThread,3000);
}