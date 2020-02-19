#pragma once
#include "Pipe.h"
#include "PipeMessage.h"
#include "..\asynctask.h"

typedef DWORD PIPE_ERROR;

#ifndef DEF_POSTER_THREAD
#define DEF_POSTER_THREAD			8
#endif //DEF_POSTER_THREAD

#define PERWAIT_TIMEOUT				20
class CPipeMessageHandler
{
public:
	CPipeMessageHandler(){}
	virtual ~CPipeMessageHandler(){}

public:
	virtual void OnPipesMessage(CPipeMessageBase* lpMessage) = 0;
};

class CPostPipeMessageCallback
{
public:
	CPostPipeMessageCallback(){}
	virtual ~CPostPipeMessageCallback(){}

public:
	virtual void OnPostPipeMessage(PIPE_ERROR err, CPipeMessageBase* lpMessage) = 0;
};

class CPipeServer : public CBaseThread
{
public:
	CPipeServer();
	~CPipeServer();

	static CTaskPool s_msgSender; 
	static PIPE_ERROR SendPipeMessage(LPCWSTR lpwsReceiverName, LPCWSTR lpwsSenderName, CPipeMessageBase* lpMsg, DWORD dwTimeout = 500);
	static PIPE_ERROR PostPipeMessage(LPCWSTR lpwsReceiverName, LPCWSTR lpwsSenderName, CPipeMessageBase* lpMsg, CPostPipeMessageCallback* lpCallback, DWORD dwTimeout = 500);

public://CBaseThread
	virtual UINT Run(LPVOID lpvParam);

public:
	virtual BOOL CreateServer(LPCWSTR lpwsName, UINT uMaxInstances = PIPE_UNLIMITED_INSTANCES);
	virtual BOOL DestroyServer();

	virtual void SetPipesMessageHandler(CPipeMessageHandler* lpPipesMsgHandler);
	LPCWSTR GetPipeName()
	{
		return m_strServerName;
	}

private:	
	struct PipeWorkThreadParam
	{
		CPipe* pPipe;
		CPipeServer* pServer;
	};
	static BOOL CreateWorkThread(CPipe* lpPipe, CPipeServer* lpThis);
	static UINT __stdcall _WorkThreadProc(LPVOID lpvParam);

	PIPE_ERROR PipeWork(CPipe* lpPipe);
	LONG AddRef();
	LONG ReleaseRef();

private:
	CPipeMessageHandler* m_pPipesMsgHandler;
	CLockCS m_lock;
	static CLockCS s_loc;
	LONG m_lRefCount;
	HANDLE m_hEventNoRefCount;

private:
	CString m_strServerName;
	UINT m_uMaxInstances;

#ifdef _USE_ASYNCTASK_
	CTaskPool m_worker;
#endif //#ifdef _USE_ASYNCTASK_

	CTrackableObjectFactory<CPipeServer> m_factory;
};

class CPipeMsgPoster : public CAsyncTaskBase

{
public:
	CPipeMsgPoster();
	virtual ~CPipeMsgPoster();

	static CPipeMsgPoster* CreateMsgPoster(LPCWSTR lpwsReceiverName, LPCWSTR lpwsSenderName, CPipeMessageBase* lpMsg, CPostPipeMessageCallback* lpCallback, DWORD dwTimeout);

public://CAsyncTaskBase
	virtual UINT Run(LPVOID lpvParam);
	virtual bool Cancel();

public:
	HRESULT SetProp_Message(CPipeMessageBase* lpMsg);
	CPipeMessageBase* GetProp_Message();
	
public:
	PIPEMSG_PROP2(LPCWSTR, CString, Receiver);
	PIPEMSG_PROP2(LPCWSTR, CString, Sender);
	PIPEMSG_PROP(CPostPipeMessageCallback*, Callback);
	PIPEMSG_PROP(DWORD, Timeout);

private:
	CPipeStreamMarshal m_streamMsg;
};


enum EHeartState
{
	EHS_Dead = 0,
	EHS_Active = 1,
};

class CPipeMsgServerEventHandler : public CPipeMessageHandler
{
public:
	CPipeMsgServerEventHandler(){}
	virtual ~CPipeMsgServerEventHandler(){}

public:
	virtual void OnAddClient(LPCWSTR lpwsClientName) = 0;
	virtual void OnRemoveClient(LPCWSTR lpwsClientName, bool bDead) = 0;
	virtual void OnClientStateChanged(LPCWSTR lpwsClientName, EHeartState eState) = 0;
	virtual void OnForceQuit() = 0;
};

class CPipeMsgClientEventHandler : public CPipeMessageHandler
{
public:
	CPipeMsgClientEventHandler(){}
	virtual ~CPipeMsgClientEventHandler(){}

public:
	virtual void OnServerDead(LPCWSTR lpwsServerName) = 0;
	virtual void OnForceQuit() = 0;
};

template<class T, class I = CPipeMessageHandler>
class CPipeMessageHandlerImpl : public I
{
public:
	virtual void OnPipesMessage(CPipeMessageBase* lpMessage)
	{
		if(NULL == lpMessage)
			return ;

		bool bRtn = _PipeMessageProc(lpMessage);
	}

public:
	virtual bool _PipeMessageProc(CPipeMessageBase* lpMessage){ return false;}//see BEGIN_PIPEMSG_MAP
};

template<class T, class I = CPipeMessageHandler>
class CPipeMessageHandlerSerializeImpl : public CPipeMessageHandlerImpl<T, I>
{
public:
	CPipeMessageHandlerSerializeImpl()
		:m_dwWorkThread(GetCurrentThreadId()), m_factory(this)
	{}

public://CPipeMessageHandler
	virtual void OnPipesMessage(CPipeMessageBase* lpMessage)
	{
		if(NULL == lpMessage)
			return ;

		if(GetCurrentThreadId() != m_dwWorkThread)
		{
			BOOL bOk = FALSE;
			lpMessage->AddRef();
			FBind::FCall* pCall = FBind::bind(&CPipeMessageHandlerSerializeImpl<T, I>::OnPipesMessage, m_factory.GetTrackObject(), lpMessage);
			T* pThis = static_cast<T*>(this);
			if(pThis)
				bOk = pThis->PostToThread(pCall);
			if(!bOk)
				lpMessage->Release();
			return ;
		}

		CPipeMessageHandlerImpl<T, I>::OnPipesMessage(lpMessage);
		lpMessage->Release();
	}

protected:
	DWORD GetWorkThreadID(){return m_dwWorkThread;}
	void SetWorkThreadID(DWORD dwTID){m_dwWorkThread = dwTID;}

protected:
	DWORD m_dwWorkThread;
	CTrackableObjectFactory<CPipeMessageHandlerSerializeImpl> m_factory;
};