#include "stdafx.h"
#include "PipeMsgServer.h"

CPipeMember::CPipeMember(LPCWSTR lpwsPipeName) : strPipeName(lpwsPipeName), dwHeart(0), eState(EHS_Active)
{

}	

CPipeMember::CPipeMember(const CPipeMember& ref)
{
	strPipeName = ref.strPipeName;
	dwHeart = ref.dwHeart;
	eState = ref.eState;
}

CPipeMember& CPipeMember::operator=(const CPipeMember& ref)
{
	if(this != &ref)
	{
		strPipeName = ref.strPipeName;
		dwHeart = ref.dwHeart;
		eState = ref.eState;
	}
	return *this;
}

CPipeMember::~CPipeMember()
{

}

//=================================================================================================

CPipeMemberMap::CPipeMemberMap()
{

}

CPipeMemberMap::CPipeMemberMap(const CPipeMemberMap& ref)
{
	for(MemberMap::iterator it = m_mapMembers.begin(); it != m_mapMembers.end(); ++it)
	{
		if(it->second)
		{
			m_mapMembers[it->first] = new CPipeMember(*(it->second));
		}
	}
}

CPipeMemberMap& CPipeMemberMap::operator=(const CPipeMemberMap& ref)
{
	if(this != &ref)
	{
		ClearMap();

		for(MemberMap::iterator it = m_mapMembers.begin(); it != m_mapMembers.end(); ++it)
		{
			if(it->second)
			{
				m_mapMembers[it->first] = new CPipeMember(*(it->second));
			}
		}
	}
	return *this;
}

CPipeMemberMap::~CPipeMemberMap()
{
	ClearMap();
}

bool CPipeMemberMap::AddMember(CPipeMember* lpMember)
{
	if(NULL  == lpMember)
		return false;

	if(FindMember(lpMember->strPipeName))
		return false;

	m_mapMembers[lpMember->strPipeName] = lpMember;
	return true;
}

CPipeMember* CPipeMemberMap::RemoveMember(LPCWSTR lpwsPipeName)
{
	MemberMap::iterator it = m_mapMembers.find(lpwsPipeName);
	if(it == m_mapMembers.end())
		return NULL;

	CPipeMember* pMem = it->second;
	m_mapMembers.erase(it);

	return pMem;
}

CPipeMember* CPipeMemberMap::FindMember(LPCWSTR lpwsPipeName)
{
	if(NULL== lpwsPipeName)
		return NULL;

	MemberMap::iterator it = m_mapMembers.find(lpwsPipeName);
	if(it == m_mapMembers.end())
		return NULL;

	CPipeMember* pMem = it->second;
	return pMem;
}

void CPipeMemberMap::ClearMap()
{
	for(MemberMap::iterator it = m_mapMembers.begin(); it != m_mapMembers.end(); ++it)
	{
		CPipeMember* pItem = it->second;
		if(pItem)
		{
			delete pItem;
		}
	}
	m_mapMembers.clear();
}

size_t CPipeMemberMap::GetMemberCount()
{
	return m_mapMembers.size();
}

CPipeMember* CPipeMemberMap::GetMemberOf(size_t nIndex)
{
	MemberMap::iterator it = m_mapMembers.begin();
	size_t n = 0;
	for(; it != m_mapMembers.end(); ++n, ++it)
	{
		if(n == nIndex)
		{
			return it->second;
		}
	}
	return NULL;
}

//=================================================================================================
CPipeMsgServer::CPipeMsgServer(LPCWSTR lpwsName, UINT uMaxInstances /*= PIPE_UNLIMITED_INSTANCES*/) : m_timeridHeartCheck(0)
, m_dwHeartCheckElapse(_HEART_CHECK_ELAPSE_)
, m_uMaxInstances(uMaxInstances)
, m_strServerName(lpwsName)
{
	ATLASSERT(lpwsName);
}

CPipeMsgServer::~CPipeMsgServer()
{
	StopServer();
}

BOOL CPipeMsgServer::StartServer(DWORD dwHeatCheckElapse/* = _HEART_CHECK_ELAPSE_*/)
{
	if(CMessageThread::IsRunning())
	{
		PIPE_DEBUG(L"CPipeMsgServer::StartServer(%s) Failed:AlreadyRunning\r\n", (LPCWSTR)m_strServerName);
		return FALSE;
	}

	m_pipeServer.SetPipesMessageHandler(this);
	if(!m_pipeServer.CreateServer(m_strServerName, m_uMaxInstances))
	{
		PIPE_DEBUG(L"CPipeMsgServer::StartServer(%s) Failed:CreateServer()\r\n", (LPCWSTR)m_strServerName);
		return FALSE;
	}

	m_dwHeartCheckElapse = dwHeatCheckElapse;
	HRESULT hr = CMessageThread::StartThread(NULL, NULL);
	if(S_OK != hr)
	{
		PIPE_DEBUG(L"CPipeMsgServer::StartServer(%s) Failed:StartMessageThread\r\n", (LPCWSTR)m_strServerName);
		m_pipeServer.DestroyServer();
		return FALSE;
	}
	PIPE_DEBUG(L"CPipeMsgServer::StartServer(%s) \r\n", (LPCWSTR)m_strServerName);
	return TRUE;
}

BOOL CPipeMsgServer::StopServer()
{
	CMessageThread::DestroyThread();
	m_pipeServer.DestroyServer();
	m_pipeServer.SetPipesMessageHandler(NULL);
	m_mapClients.ClearMap();
	PIPE_DEBUG(L"CPipeMsgServer::StopServer(%s) \r\n", (LPCWSTR)m_strServerName);
	return TRUE;
}

PIPE_ERROR CPipeMsgServer::SendPipeMessage(LPCWSTR lpwsReveiver, CPipeMessageBase* lpMsg, DWORD dwTimeout/* = 500*/)
{
	if(lpwsReveiver && lstrcmpiW(lpwsReveiver, BRORDCAST_PIPEMSG) == 0)
	{
		return BroadcastPipeMessage(lpMsg, dwTimeout, true);
	}

	return CPipeServer::SendPipeMessage(lpwsReveiver, m_strServerName, lpMsg, dwTimeout);
}

PIPE_ERROR CPipeMsgServer::PostPipeMessage(LPCWSTR lpwsReveiver, CPipeMessageBase* lpMsg, CPostPipeMessageCallback* lpCallback/* = NULL*/, DWORD dwTimeout/* = 500*/)
{
	if(lpwsReveiver && lstrcmpiW(lpwsReveiver, BRORDCAST_PIPEMSG) == 0)
	{
		return BroadcastPipeMessage(lpMsg, dwTimeout, false, lpCallback);
	}

	return CPipeServer::PostPipeMessage(lpwsReveiver, m_strServerName, lpMsg, lpCallback, dwTimeout);
}

PIPE_ERROR CPipeMsgServer::BroadcastPipeMessage(CPipeMessageBase* lpMsg, DWORD dwTimeout/* = 500*/, bool bSend/* = true*/, CPostPipeMessageCallback* lpCallback/* = NULL*/)
{
	AUTOLOCK(m_lck);
	size_t nOKCount = 0;
	size_t nCount = m_mapClients.GetMemberCount();
	for(size_t i = 0; i < nCount; ++i)
	{
		CPipeMember* pMem = m_mapClients.GetMemberOf(i);
		if(pMem)
		{
			PIPE_ERROR err = PIPE_ERROR_FAIL;
			if(bSend)
			{
				err = SendPipeMessage(pMem->strPipeName, lpMsg, dwTimeout);
				if(PIPEMSG_SYSTEM_RESULT_OK == err)
					err = PIPE_ERROR_OK;
			}
			else
			{
				err = PostPipeMessage(pMem->strPipeName, lpMsg, lpCallback, dwTimeout);
			}
			if(PIPE_ERROR_OK == err)
				++nOKCount;
		}
	}
	return nOKCount == nCount ? PIPE_ERROR_OK : PIPE_ERROR_BROADCAST_PARTIAL;
}

void CPipeMsgServer::BeforeRun(LPVOID lpvParam)
{
	__super::BeforeRun(lpvParam);

	if(m_dwHeartCheckElapse < _HEART_CHECK_ELAPSE_MIN)
		m_dwHeartCheckElapse = _HEART_CHECK_ELAPSE_MIN;
	m_timeridHeartCheck = SetTimer(NULL, 0x1234, m_dwHeartCheckElapse, NULL);

	PIPE_DEBUG(L"CPipeMsgServer::BeforeRun(%s) \r\n", (LPCWSTR)m_strServerName);
}

void CPipeMsgServer::AfterRun(LPVOID lpvParam)
{
	::KillTimer(NULL, m_timeridHeartCheck);
	__super::AfterRun(lpvParam);
	PIPE_DEBUG(L"CPipeMsgServer::AfterRun(%s) \r\n", (LPCWSTR)m_strServerName);

}

LRESULT CPipeMsgServer::OnMessage(UINT uMsg, WPARAM wp, LPARAM lp, BOOL& bHandled)
{
	if(uMsg == WM_TIMER)
	{
		if(wp == m_timeridHeartCheck)
		{
			OnHeartCheck();
			bHandled = TRUE;
			return 0;
		}
	}
	return __super::OnMessage(uMsg, wp, lp, bHandled);
}

void CPipeMsgServer::OnPipesMessage(CPipeMessageBase* lpMessage)
{
	if(NULL == lpMessage)
		return ;

	bool bDeal = false;
	if(lpMessage->GetMessage() == PIPEMSG_SYSTEM_OPERATOR)
	{
		OnPipeMsgOperator(lpMessage);
	}
	else if(lpMessage->GetMessage() == PIPEMSG_SYSTEM_HEART)
	{
		OnOpHeart(dynamic_cast<CPipeMessageHeart*>(lpMessage));
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

void CPipeMsgServer::OnAddClient(LPCWSTR lpwsClientName)
{
	PIPE_DEBUG(L"CPipeMsgServer::OnAddClient(%s) [%s]\r\n", lpwsClientName, (LPCWSTR)m_strServerName);

	EVENTHANDLE_FORLOOP_BEG(p)
	{
		p->OnAddClient(lpwsClientName);
	}
	EVENTHANDLE_FORLOOP_END()
}

void CPipeMsgServer::OnRemoveClient(LPCWSTR lpwsClientName, bool bDead)
{
	PIPE_DEBUG(L"CPipeMsgServer::OnRemoveClient(%s, %d) [%s]\r\n", lpwsClientName, bDead?1:0, (LPCWSTR)m_strServerName);

	EVENTHANDLE_FORLOOP_BEG(p)
	{
		p->OnRemoveClient(lpwsClientName, bDead);
	}
	EVENTHANDLE_FORLOOP_END()
}

void CPipeMsgServer::OnClientStateChanged(LPCWSTR lpwsClientName, EHeartState eState)
{
	PIPE_DEBUG(L"CPipeMsgServer::OnClientStateChanged(%s,%d) [%s]\r\n", lpwsClientName, (DWORD)eState, (LPCWSTR)m_strServerName);

	EVENTHANDLE_FORLOOP_BEG(p)
	{
		p->OnClientStateChanged(lpwsClientName, eState);
	}
	EVENTHANDLE_FORLOOP_END()
}

void CPipeMsgServer::OnForceQuit()
{
	PIPE_DEBUG(L"CPipeMsgServer::OnForceQuit() [%s]\r\n", (LPCWSTR)m_strServerName);

	EVENTHANDLE_FORLOOP_BEG(p)
	{
		p->OnForceQuit();
	}
	EVENTHANDLE_FORLOOP_END()
}

void CPipeMsgServer::OnPipeMsgOperator(CPipeMessageBase* lpMessage)
{
	CPipeMessageOperator* pOP = dynamic_cast<CPipeMessageOperator*>(lpMessage);
	if(pOP)
	{
		switch(pOP->GetProp_Param1())
		{
		case PIPEMSG_SYSTEM_OP_ADDCLIENT:
			{
				OnOpAddClient(pOP);
				break;
			}
		case PIPEMSG_SYSTEM_OP_REMOVECLIENT:
			{
				OnOpRemoveClient(pOP);
				break;
			}
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

void CPipeMsgServer::OnOpHeart(CPipeMessageHeart* lpHeart)
{
	if(NULL == lpHeart)
		return ;

	LPCWSTR pwsClientName = lpHeart->GetSender();
	bool bChanged = false;
	{
		AUTOLOCK(m_lck);
		CPipeMember* pMem = m_mapClients.FindMember(pwsClientName);
		if(pMem)
		{
			bChanged = (pMem->eState != EHS_Active);
			pMem->dwHeart = GetTickCount();
			pMem->eState = EHS_Active;
		}
	}
	if(bChanged)
		OnClientStateChanged(pwsClientName, EHS_Active);
}

void CPipeMsgServer::OnOpAddClient(CPipeMessageOperator* lpOP)
{
	LPCWSTR pwsClientName = lpOP->GetSender();
	CPipeMember* pMem = NULL;
	{
		AUTOLOCK(m_lck);
		pMem = m_mapClients.FindMember(pwsClientName);
		if(pMem)
		{
			m_mapClients.RemoveMember(pwsClientName);
			delete pMem;
		}
		pMem = new CPipeMember(pwsClientName);
		m_mapClients.AddMember(pMem);
	}
	OnAddClient(pwsClientName);
}

void CPipeMsgServer::OnOpRemoveClient(CPipeMessageOperator* lpOP)
{
	LPCWSTR pwsClientName = lpOP->GetSender();
	CPipeMember* pMem = NULL;
	{
		AUTOLOCK(m_lck);
		pMem = m_mapClients.RemoveMember(pwsClientName);
	}
	if(pMem)
		delete pMem;
	OnRemoveClient(pwsClientName, false);
}

void CPipeMsgServer::OnHeartCheck()
{
	CPipeMemberMap map;
	{
		AUTOLOCK(m_lck);
		size_t nCount = m_mapClients.GetMemberCount();
		for(size_t i = 0; i < nCount; ++i)
		{
			CPipeMember* pMem = m_mapClients.GetMemberOf(i);
			if(pMem)
			{
				EHeartState eStateLast = pMem->eState;
				if(pMem->dwHeart > 0 && GetTickCount() > pMem->dwHeart)
				{
					if(GetTickCount() - pMem->dwHeart > m_dwHeartCheckElapse)
						pMem->eState = EHS_Dead;
					else
						pMem->eState = EHS_Active;
				}
				else
				{
					pMem->dwHeart = GetTickCount();
					pMem->eState = EHS_Active;
				}
				if(eStateLast != pMem->eState)
					map.AddMember(new CPipeMember(*pMem));
			}
		}
	}
	for(size_t i = 0; i < map.GetMemberCount(); ++i)
	{
		CPipeMember* pMem = map.GetMemberOf(i);
		if(pMem)
			OnClientStateChanged(pMem->strPipeName, pMem->eState);
	}
}