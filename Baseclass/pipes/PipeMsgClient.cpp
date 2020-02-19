#include "StdAfx.h"
#include "PipeMsgClient.h"

CPipeMsgClient::CPipeMsgClient(LPCWSTR lpwsName, UINT uMaxInstances/* = PIPE_UNLIMITED_INSTANCES*/) : m_strClientName(lpwsName)
, m_uMaxInstances(uMaxInstances)
, m_dwHeartBeatElapse(_HEART_BEAT_ELAPSE_)
, m_dwTimerIDHeartBeat(0)
{
	ATLASSERT(NULL != lpwsName);
}

CPipeMsgClient::~CPipeMsgClient(void)
{
	StopClient();
}

BOOL CPipeMsgClient::StartClient(LPCWSTR lpwsServerName, DWORD dwHeartBeatElapse/* = _HEART_BEAT_ELAPSE_*/)
{
	if(NULL == lpwsServerName)
	{
		PIPE_DEBUG(L"CPipeMsgClient::StartClient(%s) Failed:ServerNameNull [%s]\r\n", lpwsServerName, (LPCWSTR)m_strClientName);
		return FALSE;
	}

	if(CMessageThread::IsRunning())
	{
		PIPE_DEBUG(L"CPipeMsgClient::StartClient(%s) Failed:AlreadyRunning [%s]\r\n", lpwsServerName, (LPCWSTR)m_strClientName);
		return FALSE;
	}

	m_pipeServer.SetPipesMessageHandler(this);
	if(!m_pipeServer.CreateServer(m_strClientName, m_uMaxInstances))
	{
		PIPE_DEBUG(L"CPipeMsgClient::StartClient(%s) Failed:CreateServer() [%s]\r\n", lpwsServerName, (LPCWSTR)m_strClientName);
		return FALSE;
	}

	m_strServerName = lpwsServerName;
	m_dwHeartBeatElapse = dwHeartBeatElapse;
	HRESULT hr = CMessageThread::StartThread(NULL, NULL);
	if(S_OK != hr)
	{
		m_pipeServer.DestroyServer();
		PIPE_DEBUG(L"CPipeMsgClient::StartClient(%s) Failed:StartMessageThread [%s]\r\n", lpwsServerName, (LPCWSTR)m_strClientName);
		return FALSE;
	}
	PIPE_DEBUG(L"CPipeMsgClient::StartClient(%s) [%s]\r\n", lpwsServerName, (LPCWSTR)m_strClientName);
	return TRUE;
}

BOOL CPipeMsgClient::StopClient()
{
	CMessageThread::DestroyThread();
	m_pipeServer.DestroyServer();
	m_pipeServer.SetPipesMessageHandler(NULL);
	
	PIPE_DEBUG(L"CPipeMsgClient::StopClient() [%s]\r\n", (LPCWSTR)m_strClientName);
	return TRUE;
}

PIPE_ERROR CPipeMsgClient::SendPipeMessage(CPipeMessageBase* lpMsg, DWORD dwTimeout/* = 500*/)
{
	return CPipeServer::SendPipeMessage(m_strServerName, m_strClientName, lpMsg, dwTimeout);
}

PIPE_ERROR CPipeMsgClient::PostPipeMessage(CPipeMessageBase* lpMsg, CPostPipeMessageCallback* lpCallback/* = NULL*/, DWORD dwTimeout/* = 500*/)
{
	return CPipeServer::PostPipeMessage(m_strServerName, m_strClientName, lpMsg, lpCallback, dwTimeout);
}

void CPipeMsgClient::BeforeRun(LPVOID lpvParam)
{
	__super::BeforeRun(lpvParam);
	
	CStackPipeMessage<CPipeMessageOperator> op;
	op.SetProp_Param1(PIPEMSG_SYSTEM_OP_ADDCLIENT);
	PIPE_ERROR err = SendPipeMessage(&op);
	ATLASSERT(PIPEMSG_SYSTEM_RESULT_OK == err);

	if(m_dwHeartBeatElapse < _HEART_BEAT_ELAPSE_MIN)
		m_dwHeartBeatElapse = _HEART_BEAT_ELAPSE_MIN;

	m_dwTimerIDHeartBeat = ::SetTimer(NULL, 0x1234, m_dwHeartBeatElapse, NULL);	

	PIPE_DEBUG(L"CPipeMsgClient::BeforeRun(AddClient:%d) [%s]\r\n", err, (LPCWSTR)m_strClientName);
}

void CPipeMsgClient::AfterRun(LPVOID lpvParam)
{
	::KillTimer(NULL, m_dwTimerIDHeartBeat);

	CStackPipeMessage<CPipeMessageOperator> op;
	op.SetProp_Param1(PIPEMSG_SYSTEM_OP_REMOVECLIENT);
	PIPE_ERROR err = SendPipeMessage(&op);
	ATLASSERT(PIPEMSG_SYSTEM_RESULT_OK == err);

	PIPE_DEBUG(L"CPipeMsgClient::AfterRun(RemoveClient:%d) [%s]\r\n", err, (LPCWSTR)m_strClientName);
	
	__super::AfterRun(lpvParam);
}

LRESULT CPipeMsgClient::OnMessage(UINT uMsg, WPARAM wp, LPARAM lp, BOOL& bHandled)
{
	if(uMsg == WM_TIMER)
	{
		if(wp == m_dwTimerIDHeartBeat)
		{
			OnHeart();
			bHandled = TRUE;
			return 0;
		}
	}
	return __super::OnMessage(uMsg, wp, lp, bHandled);
}

void CPipeMsgClient::OnPipesMessage(CPipeMessageBase* lpMessage)
{
	if(NULL == lpMessage)
		return ;

	if(lpMessage->GetMessage() == PIPEMSG_SYSTEM_OPERATOR)
	{
		OnPipeMsgOperator(lpMessage);
	}
	
	if(lpMessage->GetMessage() > PIPEMSG_BASE)
	{
		EVENTHANDLE_FORLOOP_BEG(p)
		{
			p->OnPipesMessage(lpMessage);
		}
		EVENTHANDLE_FORLOOP_END()
	}
}

void CPipeMsgClient::OnServerDead(LPCWSTR lpwsServerName)
{
	PIPE_DEBUG(L"CPipeMsgClient::OnServerDead(%s) [%s]\r\n", lpwsServerName, (LPCWSTR)m_strClientName);

	EVENTHANDLE_FORLOOP_BEG(p)
	{
		p->OnServerDead(lpwsServerName);
	}
	EVENTHANDLE_FORLOOP_END()
}

void CPipeMsgClient::OnForceQuit()
{
	PIPE_DEBUG(L"CPipeMsgClient::OnForceQuit() [%s]\r\n", (LPCWSTR)m_strClientName);

	EVENTHANDLE_FORLOOP_BEG(p)
	{
		p->OnForceQuit();
	}
	EVENTHANDLE_FORLOOP_END()
}

void CPipeMsgClient::OnPipeMsgOperator(CPipeMessageBase* lpMessage)
{
	CPipeMessageOperator* pOP = dynamic_cast<CPipeMessageOperator*>(lpMessage);
	if(pOP)
	{
		switch(pOP->GetProp_Param1())
		{
		case PIPEMSG_SYSTEM_OP_ISRUNNNING:
			{
				//Donothing
				break;
			}
		case PIPEMSG_SYSTEM_OP_QUIT:
			{
				OnForceQuit();
				break;
			}
		}
	}
}

void CPipeMsgClient::OnHeart()
{
	if(m_strServerName.IsEmpty())
		return ;

	CStackPipeMessage2<CPipeMessageHeart> op(PIPEMSG_SYSTEM_HEART);
	PIPE_ERROR err = SendPipeMessage(&op);
	if(err != PIPEMSG_SYSTEM_RESULT_OK)
	{
		OnServerDead(m_strServerName);
	}
}
