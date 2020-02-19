#pragma once
#include "..\BaseThread.h"
#include "..\base.h"
#include "PipesServer.h"

#ifndef _HEART_BEAT_ELAPSE_
#define _HEART_BEAT_ELAPSE_			1000
#endif //_HEART_BEAT_ELAPSE_
#define _HEART_BEAT_ELAPSE_MIN		400

class CPipeMsgClient : public CMessageThread
						, public CEventHandleProxyT<CPipeMsgClientEventHandler>
{
public:
	CPipeMsgClient(LPCWSTR lpwsName, UINT uMaxInstances = PIPE_UNLIMITED_INSTANCES);
	~CPipeMsgClient(void);

public:
	BOOL StartClient(LPCWSTR lpwsServerName, DWORD dwHeartBeatElapse = _HEART_BEAT_ELAPSE_);
	BOOL StopClient();
	PIPE_ERROR SendPipeMessage(CPipeMessageBase* lpMsg, DWORD dwTimeout = 500);
	PIPE_ERROR PostPipeMessage(CPipeMessageBase* lpMsg, CPostPipeMessageCallback* lpCallback = NULL, DWORD dwTimeout = 500);


public://CBaseThread
	virtual void BeforeRun(LPVOID lpvParam);
	virtual void AfterRun(LPVOID lpvParam);
	virtual LRESULT OnMessage(UINT uMsg, WPARAM wp, LPARAM lp, BOOL& bHandled);

public://CPipeMsgClientEventHandler
	virtual void OnPipesMessage(CPipeMessageBase* lpMessage);
	virtual void OnServerDead(LPCWSTR lpwsServerName);
	virtual void OnForceQuit();

private:
	void OnPipeMsgOperator(CPipeMessageBase* lpMessage);
	void OnHeart();

private:
	CPipeServer m_pipeServer;

private:
	CString m_strServerName;
	CString m_strClientName;
	UINT m_uMaxInstances;
	DWORD m_dwHeartBeatElapse;
	DWORD_PTR m_dwTimerIDHeartBeat;
};


