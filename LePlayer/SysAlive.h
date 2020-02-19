#include "stdafx.h"

unsigned int WINAPI _StartAliveThread(void *param);

static bool AliveMsgWait(HANDLE hObject, DWORD dwTime)
{
	DWORD  dwCur = GetTickCount();
	dwTime == INFINITE ? 0 : dwTime;

	while (true)
	{
		DWORD result = MsgWaitForMultipleObjects(1, &hObject, FALSE, dwTime, QS_ALLINPUT);
		if (result == (WAIT_OBJECT_0))
			return true;

		MSG msg;
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			bool bQuit = (msg.message == WM_QUIT);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (bQuit)
				return false;
		}
		else
		{
			//防止CPU处理不过来
			::Sleep(10);
		}
		if (dwTime && (GetTickCount() - dwCur >= dwTime))
		{
			break;
		}
	}
	return false;
}



class SysAlive
{
public:
	SysAlive();
	~SysAlive();

	static void Alive()
	{
		Exit();

		// 如果已经有线程了，就返回
		if (NULL != m_hAliveThread)
			return;

		if (NULL == m_hEvent_Exit)
			m_hEvent_Exit = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		// 如果已经有了，重置一下
		ResetEvent(m_hEvent_Exit);

		// 启动线程
		unsigned int nAliveThreadID;
		m_hAliveThread = (HANDLE)_beginthreadex(NULL, 0, _StartAliveThread, 0, 0, &nAliveThreadID);

		return;
	}

	static void Exit()
	{
		if (NULL != m_hEvent_Exit)
			::SetEvent(m_hEvent_Exit);
		m_hAliveThread = NULL;
	}

	static HANDLE m_hEvent_Exit;
	static HANDLE m_hAliveThread;
};

HANDLE SysAlive::m_hAliveThread = NULL;
HANDLE SysAlive::m_hEvent_Exit = NULL;

SysAlive::SysAlive()
{
}

SysAlive::~SysAlive()
{
	Exit();
	
	if (m_hAliveThread != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hAliveThread);
		m_hAliveThread = INVALID_HANDLE_VALUE;
	}	
	if (m_hEvent_Exit != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hEvent_Exit);
		m_hEvent_Exit = INVALID_HANDLE_VALUE;
	}
}

unsigned int WINAPI _StartAliveThread(void *param)
{
	if (SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED | ES_DISPLAY_REQUIRED) == NULL)
	{
		// try XP variant as well just to make sure 
		SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED);
	}

	while (!AliveMsgWait(SysAlive::m_hEvent_Exit, 500))
	{
		Sleep(1000);
	}

	return 0;
}

