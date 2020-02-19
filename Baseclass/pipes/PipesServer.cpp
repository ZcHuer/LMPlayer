#include "stdafx.h"
#include "PipesServer.h"

CLockCS CPipeServer::s_loc;
CPipeServer::CPipeServer() : m_uMaxInstances(PIPE_UNLIMITED_INSTANCES), m_lRefCount(1), m_hEventNoRefCount(CreateEvent(NULL, TRUE, FALSE, NULL)), m_factory(this)
#ifdef _USE_ASYNCTASK_
, m_worker(16)
#endif //#ifdef _USE_ASYNCTASK_
{

}

CPipeServer::~CPipeServer()
{
	DestroyServer();
	CloseHandle(m_hEventNoRefCount);
}

DWORD CPipeServer::SendPipeMessage(LPCWSTR lpwsReceiverName, LPCWSTR lpwsSenderName, CPipeMessageBase* lpMsg, DWORD dwTimeout/* = 500*/)
{
	if(NULL == lpwsReceiverName || NULL == lpMsg)
		return PIPE_ERROR_INVALIDARG;

	//Marshal Pipe Message 
	CPipeStreamMarshal Buf;
	lpMsg->SetMessageInfo(lpwsSenderName, lpwsReceiverName);

	PIPE_DEBUG(L"SendPipeMessage(%d,From:%s, To:%s)\r\n", lpMsg->GetMessage(), lpwsSenderName, lpwsReceiverName);
	PIPEMSG_DUMP(lpMsg);

	HRESULT hr = lpMsg->MarshalPipeMsg(&Buf);
	if(FAILED(hr))
	{
		PIPE_DEBUG(L"SendPipeMessage(%d,From:%s, To:%s) Fail:MarshalPipeMsg()\r\n", lpMsg->GetMessage(), lpwsSenderName, lpwsReceiverName);
		return PIPE_ERROR_MARSHAL;
	}

	//Create Pipe Client
	CPipe pipeClient(FALSE);
	if(!pipeClient.CreatePipe(lpwsReceiverName))
	{
		BOOL bOK = FALSE;
		DWORD dwTimeUsed = 0;
		do 
		{
			dwTimeUsed += PERWAIT_TIMEOUT;

			if(!pipeClient.WaitNamedPipe(PERWAIT_TIMEOUT) && ERROR_SEM_TIMEOUT != GetLastError())
				continue;
	
			CAutoLock _l(s_loc);
			bOK = pipeClient.CreatePipe(lpwsReceiverName);
		} while (dwTimeUsed < dwTimeout && !bOK);

		if(!bOK)
		{
			PIPE_DEBUG(L"SendPipeMessage(%d,From:%s, To:%s) Fail:CreatePipe()\r\n", lpMsg->GetMessage(), lpwsSenderName, lpwsReceiverName);
			return PIPE_ERROR_CREATEPIPE;
		}
	}
	
	//Send Pipe Message
	pipeClient.SetNamedPipeHandleState(PIPE_READMODE_MESSAGE);
	if(!pipeClient.WriteData(Buf))
	{
		PIPE_DEBUG(L"SendPipeMessage(%d,From:%s, To:%s) Fail:WriteData()\r\n", lpMsg->GetMessage(), lpwsSenderName, lpwsReceiverName);
		return PIPE_ERROR_WRITE;
	}

	//Read Send Pipe Message Result 
	CPipeStreamMarshal bufRead;
	if(!pipeClient.ReadData(bufRead))
	{
		PIPE_DEBUG(L"SendPipeMessage(%d,From:%s, To:%s) Fail:ReadData()\r\n", lpMsg->GetMessage(), lpwsSenderName, lpwsReceiverName);
		return PIPE_ERROR_READ;
	}
	
	//Unmarshal Pipe Message Result
	CPipeMessageBase* pPipeMsg = NULL;
	hr = CPipeMessageBase::CreatePipeMessageObj(&bufRead, &pPipeMsg);
	if(FAILED(hr))
	{
		PIPE_DEBUG(L"SendPipeMessage(%d,From:%s, To:%s) Fail:CreatePipeMessageObj(CPipeMessageResult)\r\n", lpMsg->GetMessage(), lpwsSenderName, lpwsReceiverName);
		return PIPE_ERROR_UNMARSHAL;
	}
	
	DWORD dwResult = PIPE_ERROR_FAIL;
	CPipeMessageResult* pResult = dynamic_cast<CPipeMessageResult*>(pPipeMsg);
	if(pResult)
	{
		dwResult = pResult->GetProp_Param1();
		if(dwResult != PIPEMSG_SYSTEM_RESULT_OK)
		{
			PIPE_DEBUG(L"SendPipeMessage(%d,From:%s, To:%s) Result:%d\r\n", lpMsg->GetMessage(), lpwsSenderName, lpwsReceiverName, dwResult);
		}
	}	
	ULONG lRef = pPipeMsg->Release();
	ATLASSERT(lRef == 0);
	return dwResult;
}


UINT CPipeServer::Run(LPVOID lpvParam)
{
	AddRef();
	do 
	{
		CPipe* pPipe = new CPipe(TRUE);
		if(NULL == pPipe)
		{
			PIPE_DEBUG(L"CPipeServer::Run():: new(CPipe(%s)) Failed\r\n", m_strServerName);
			break;
		}

		if(!pPipe->CreatePipe(m_strServerName))
		{
			PIPE_DEBUG(L"CPipeServer::Run()::CreatePipe(%s) Failed[%d]\r\n", m_strServerName, GetLastError());
			delete pPipe;
			Sleep(5);
			continue;
		}
		
		BOOL bConnected = pPipe->ConnectNamedPipe(TRUE);
		if(bConnected || (!bConnected && GetLastError() == ERROR_PIPE_CONNECTED))
		{
			if(CBaseThread::IsAbort())
			{
				delete pPipe;
				PIPE_DEBUG(L"CPipeServer::Run()::ConnectNamePipe(%s) Abort\r\n", m_strServerName);
				break;
			}

			AddRef();
			if(CPipeServer::CreateWorkThread(pPipe, this))
				continue;			
			ReleaseRef();

			PIPE_DEBUG(L"CPipeServer::Run()::CreateWorkThread(%s) Failed\r\n", m_strServerName);
			pPipe->ConnectNamedPipe(FALSE);

			delete pPipe;
			Sleep(50);
			continue;
		}
		else
		{			
			delete pPipe;

			if(CBaseThread::IsAbort())
			{
				PIPE_DEBUG(L"CPipeServer::Run()::ConnectNamedPipe(%s) QuitPipeServer[%d]\r\n", m_strServerName, GetLastError());
				break;
			}

			PIPE_DEBUG(L"CPipeServer::Run()::ConnectNamedPipe(%s) Failed[%d]\r\n", m_strServerName, GetLastError());
			Sleep(50);
			continue;
		}
	} while (!IsAbort());
	ReleaseRef();
	return 1;
}

BOOL CPipeServer::CreateServer(LPCWSTR lpwsName, UINT uMaxInstances/* = PIPE_UNLIMITED_INSTANCES*/)
{
	if(NULL == lpwsName)
	{
		PIPE_DEBUG(L"CPipeServer::CreateServer() Failed:Name is Null\r\n");
		return FALSE;
	}

	if(CBaseThread::IsRunning())
	{
		PIPE_DEBUG(L"CPipeServer::CreateServer() Failed:Already Running\r\n");
		return FALSE;
	}

	m_strServerName = lpwsName;
	m_uMaxInstances = uMaxInstances;

	HRESULT hr = s_msgSender.StartThread("PipeServer_MessageSender", NULL);
	if(FAILED(hr))
	{
		PIPE_DEBUG(L"CPipeServer::CreateServer() Faile:Sender Thread Pool Start Failed\r\n");
		return hr;
	}

	BOOL bRtn = SUCCEEDED(CBaseThread::StartThread(NULL, NULL));
	if(!bRtn)
	{
		PIPE_DEBUG(L"CPipeServer::CreateServer() Faile:StartThread()\r\n");
	}
	return bRtn;
}

BOOL CPipeServer::DestroyServer()
{
	//停止消息发送
	s_msgSender.ClearPool(NULL);
	s_msgSender.DestroyThread();

	//停止管道控制线程
	CBaseThread::StopThread();
	do 
	{
		//是从ConnectNamedPipe()返回 ???有其他方法么???
		CPipe pipe(FALSE);
		pipe.CreatePipe(m_strServerName);
		pipe.DestoryPipe();
	} while (WAIT_TIMEOUT == CBaseThread::Wait(NULL, 0));

	//等待管道控制线程退出
	BOOL bRtn = SUCCEEDED(CBaseThread::DestroyThread());

#ifdef _USE_ASYNCTASK_
	m_worker.ClearPool(NULL);
#endif //#ifdef _USE_ASYNCTASK_

	if(m_lRefCount > 1)//等待管道处理线程退出
		WaitForSingleObject(m_hEventNoRefCount, INFINITE);

	PIPE_DEBUG(L"CPipeServer::DestroyServer() Complete\r\n");
	return bRtn;
}

void CPipeServer::SetPipesMessageHandler(CPipeMessageHandler* lpPipesMsgHandler)
{
	CAutoLock _l(m_lock);
	m_pPipesMsgHandler = lpPipesMsgHandler;
}

BOOL CPipeServer::CreateWorkThread(CPipe* lpPipe, CPipeServer* lpThis)
{
	if(NULL == lpPipe)
	{
		ATLASSERT(FALSE);
		return FALSE;
	}

#ifdef _USE_ASYNCTASK_
	FBind::FCall* pCall = FBind::bind(&CPipeServer::PipeWork, lpThis->m_factory.GetTrackObject(), PIPE_ERROR(PIPE_ERROR_INVALIDARG), lpPipe);
	if(NULL == pCall)
	{
		ATLASSERT(FALSE);
		return FALSE;
	}
	CAsyncTask* pTask = new CAsyncTask(pCall);
	if(NULL == pTask)
	{
		delete pCall;
		ATLASSERT(FALSE);
		return FALSE;
	}
	EAddTaskResult ret = lpThis->m_worker.AddTask(pTask);
	if(EATR_Pending != ret)
	{
		PIPE_DEBUG(L"CPipeServer::CreateWorkThread() Failed:Worker Thread Pool Add Task Failed\r\n");
		delete pTask;
		return FALSE;
	}
	return TRUE;
#else //#ifdef _USE_ASYNCTASK_
	PipeWorkThreadParam* pParam = new PipeWorkThreadParam;
	if(NULL == pParam)
	{
		ATLASSERT(FALSE);
		return FALSE;
	}

	pParam->pPipe = lpPipe;
	pParam->pServer = lpThis;

	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, _WorkThreadProc, (LPVOID)pParam, 0, NULL);
	if(NULL == hThread)
	{
		delete pParam;
		return FALSE;
	}

	CloseHandle(hThread);
	return TRUE;
#endif//#ifdef _USE_ASYNCTASK_
}

UINT __stdcall CPipeServer::_WorkThreadProc(LPVOID lpvParam)
{
	PipeWorkThreadParam* pParam = (PipeWorkThreadParam*)lpvParam;
	if(NULL == pParam)
	{
		ATLASSERT(FALSE);
		return 0;
	}

	if(pParam->pServer)
	{
		CoInitialize(NULL);
		__try
		{
			pParam->pServer->PipeWork(pParam->pPipe);
		}
		__finally
		{
			CoUninitialize();
		}		
	}
	else
	{
		ATLASSERT(FALSE);
	}

	delete pParam;
	return 1;
}

PIPE_ERROR CPipeServer::PipeWork(CPipe* lpPipe)
{
	if(NULL == lpPipe)
	{
		ATLASSERT(FALSE);
		ReleaseRef();
		return PIPE_ERROR_INVALIDARG;
	}

	DWORD dwSysResult = PIPEMSG_SYSTEM_RESULT_FAIL;

	//Read Pipe Message
	CPipeStreamMarshal buf;
	if(lpPipe->ReadData(buf))
	{
		CAutoLock _l(m_lock);
		if(m_pPipesMsgHandler)
		{
			CPipeMessageBase* pPipeMsg = NULL;
			HRESULT hr = CPipeMessageBase::CreatePipeMessageObj(&buf, &pPipeMsg);
			if(SUCCEEDED(hr))
			{
				PIPE_DEBUG(L"CPipeServer::PipeWork()::OnPipeMessage(%d,From:%s,To:%s)\r\n", pPipeMsg->GetMessage(), pPipeMsg->GetSender(), pPipeMsg->GetReceiver());
				PIPEMSG_DUMP(pPipeMsg);

				m_pPipesMsgHandler->OnPipesMessage(pPipeMsg);
				dwSysResult = PIPEMSG_SYSTEM_RESULT_OK;
			}
			else
			{
				dwSysResult = PIPEMSG_SYSTEM_RESULT_UNMARSHALMSG;
			}
 			if(pPipeMsg)
 				pPipeMsg->Release();
		}
		else
		{
			dwSysResult = PIPEMSG_SYSTEM_RESULT_NOHANDLER;
		}
	}
	else
	{
		dwSysResult = PIPEMSG_SYSTEM_RESULT_READMSG;	
	}
	if(dwSysResult != PIPEMSG_SYSTEM_RESULT_OK)
		PIPE_DEBUG(L"CPipeServer::PipeWork()::Failed %d [LastErr:%d\t%s] Failed\r\n", dwSysResult, GetLastError(), lpPipe->GetPipeName());

	DWORD dwRtn = PIPE_ERROR_FAIL;
	//回复处理结果
	{
		DWORD dwErr = ERROR_SUCCESS;
		CPipeMessageBase* pMsg = NULL;
		CPipeMessageBase::CreatePipeMessageObj(PIPEMSG_SYSTEM_RESULT, &pMsg);
		CPipeMessageResult* pResult = dynamic_cast<CPipeMessageResult*>(pMsg);
		if(pResult)
		{
			pResult->SetProp_Param1(dwSysResult);
			CPipeStreamMarshal buf;
			HRESULT hr = pResult->MarshalPipeMsg(&buf);
			if(SUCCEEDED(hr))
			{
				if(!lpPipe->WriteData(buf))
				{
					dwErr = GetLastError();
					dwRtn = PIPE_ERROR_WRITE;
				}
				else
				{
					dwRtn = PIPE_ERROR_OK;
				}
			}
			else
			{
				dwRtn = PIPE_ERROR_MARSHAL;
			}
		}
		else
		{
			dwRtn = PIPE_ERROR_CREATEMSG;
		}
		if(pMsg)
		{
			ULONG lRef = pMsg->Release();
			ATLASSERT(lRef == 0);
		}
	
		if(dwRtn != PIPE_ERROR_OK)
			PIPE_DEBUG(L"CPipeServer::PipeWork()::Return Result Failed %d [%d\t%s]\r\n", dwRtn, dwErr, lpPipe->GetPipeName());
	}
	lpPipe->ConnectNamedPipe(FALSE);

	lpPipe->DestoryPipe();

	delete lpPipe;
	ReleaseRef();
	return dwRtn;
}

LONG CPipeServer::AddRef()
{
	LONG lRtn = InterlockedIncrement(&m_lRefCount);

	ATLASSERT(lRtn > 1);
	return lRtn;
}

LONG CPipeServer::ReleaseRef()
{
	LONG lRtn = InterlockedDecrement(&m_lRefCount);
	if(lRtn <= 1)
	{
		ATLASSERT(lRtn >= 1);
		if(m_hEventNoRefCount)
			SetEvent(m_hEventNoRefCount);
	}
	return lRtn;
}


//=================================================================================================
CPipeMsgPoster::CPipeMsgPoster()
{

}

CPipeMsgPoster::~CPipeMsgPoster()
{

}

CPipeMsgPoster* CPipeMsgPoster::CreateMsgPoster(LPCWSTR lpwsReceiverName, LPCWSTR lpwsSenderName, CPipeMessageBase* lpMsg, CPostPipeMessageCallback* lpCallback, DWORD dwTimeout)
{
	CPipeMsgPoster* pPoster = new CPipeMsgPoster();
	if(NULL == pPoster)
		return NULL;

	pPoster->SetProp_Receiver(lpwsReceiverName);
	pPoster->SetProp_Sender(lpwsSenderName);
	pPoster->SetProp_Callback(lpCallback);
	pPoster->SetProp_Timeout(dwTimeout);

	HRESULT hr = pPoster->SetProp_Message(lpMsg);
	if(FAILED(hr))
	{
		delete pPoster;
		return NULL;
	}
	return pPoster;
}

UINT CPipeMsgPoster::Run(LPVOID lpvParam)
{
	CPostPipeMessageCallback* pCB = GetProp_Callback();
	CPipeMessageBase* pMsg = GetProp_Message();
	if(NULL == pMsg)
	{
		if(pCB)
			pCB->OnPostPipeMessage(PIPE_ERROR_POST_UNMARSHAL, NULL);
		return 0;
	}
	
	PIPE_ERROR eRtn = CPipeServer::SendPipeMessage(GetProp_Receiver(), GetProp_Sender(), pMsg, GetProp_Timeout());
	if(pCB)
		pCB->OnPostPipeMessage(eRtn, pMsg);
	return 0;
}

bool CPipeMsgPoster::Cancel()
{
	if(!__super::IsRunning())
	{
		CPostPipeMessageCallback* pCB = GetProp_Callback();
		if(pCB)
		{
			CPipeMessageBase* pMsg = GetProp_Message();
			if(NULL == pMsg)
				pCB->OnPostPipeMessage(PIPE_ERROR_POST_UNMARSHAL, NULL);
			else
				pCB->OnPostPipeMessage(PIPE_ERROR_POST_CANCEL, pMsg);
		}		
	}
	return __super::Cancel();
}

HRESULT CPipeMsgPoster::SetProp_Message(CPipeMessageBase* lpMsg)
{
	if(NULL == lpMsg)
		return E_INVALIDARG;

	m_streamMsg.DestroyStream();

	HRESULT hr = lpMsg->MarshalPipeMsg(&m_streamMsg);
	if(FAILED(hr))
		return hr;

	return hr;
}

CPipeMessageBase* CPipeMsgPoster::GetProp_Message()
{
	CPipeMessageBase* pMsg = NULL;
	HRESULT hr = CPipeMessageBase::CreatePipeMessageObj(&m_streamMsg, &pMsg);
	if(FAILED(hr))
		return NULL;

	return pMsg;
}
//=================================================================================================
CTaskPool CPipeServer::s_msgSender(DEF_POSTER_THREAD);
PIPE_ERROR CPipeServer::PostPipeMessage(LPCWSTR lpwsReceiverName, LPCWSTR lpwsSenderName, CPipeMessageBase* lpMsg, CPostPipeMessageCallback* lpCallback, DWORD dwTimeout/* = 500*/)
{
	if(NULL == lpwsReceiverName || NULL == lpMsg)
		return PIPE_ERROR_INVALIDARG;

	CPipeMsgPoster* pPoster = CPipeMsgPoster::CreateMsgPoster(lpwsReceiverName, lpwsSenderName, lpMsg, lpCallback, dwTimeout);
	if(NULL == pPoster)
		return PIPE_ERROR_POST_CREATEPOSTER;

	EAddTaskResult eRtn = s_msgSender.AddTask(pPoster);
	if(EATR_Fail == eRtn)
		return PIPE_ERROR_POST_ADDTASK;

	return PIPE_ERROR_OK;
}
