#pragma once
#include <map>
#include "..\Base.h"
#include "PipesServer.h"

class CPipeMember
{
public:
	CPipeMember(LPCWSTR lpwsPipeName);
	CPipeMember(const CPipeMember& ref);
	CPipeMember& operator=(const CPipeMember& ref);
	virtual ~CPipeMember();

public:
	CString strPipeName;
	DWORD dwHeart;
	EHeartState eState;
};

struct nocaseless
	: public std::binary_function<CString, CString, bool>
{
	bool operator()(const CString& _Left, const CString& _Right) const
	{	// apply operator< to operands
		int nRtn = _Left.CompareNoCase(_Right);
		return (nRtn < 0);
	}
};

class CPipeMemberMap
{
public:
	CPipeMemberMap();
	CPipeMemberMap(const CPipeMemberMap& ref);
	CPipeMemberMap& operator=(const CPipeMemberMap& ref);
	virtual ~CPipeMemberMap();

public:
	bool AddMember(CPipeMember* lpMember);
	CPipeMember* RemoveMember(LPCWSTR lpwsPipeName);
	CPipeMember* FindMember(LPCWSTR lpwsPipeName);
	void ClearMap();

	size_t GetMemberCount();
	CPipeMember* GetMemberOf(size_t nIndex);

private:
	typedef std::map<CString, CPipeMember*, nocaseless> MemberMap;
	MemberMap m_mapMembers;
};

#ifndef _HEART_CHECK_ELAPSE_
#define _HEART_CHECK_ELAPSE_		3000
#endif//#ifndef _HEART_CHECK_ELAPSE_
#define _HEART_CHECK_ELAPSE_MIN		500

#define BRORDCAST_PIPEMSG			L"BroadcastPipeMessage"

class CPipeMsgServer : public CMessageThread
					, public CEventHandleProxyT<CPipeMsgServerEventHandler>
{
public:
	CPipeMsgServer(LPCWSTR lpwsName, UINT uMaxInstances = PIPE_UNLIMITED_INSTANCES);
	virtual ~CPipeMsgServer();

public:
	BOOL StartServer(DWORD dwHeatCheckElapse = _HEART_CHECK_ELAPSE_);
	BOOL StopServer();

	PIPE_ERROR SendPipeMessage(LPCWSTR lpwsReveiver, CPipeMessageBase* lpMsg, DWORD dwTimeout = 500);
	PIPE_ERROR PostPipeMessage(LPCWSTR lpwsReveiver, CPipeMessageBase* lpMsg, CPostPipeMessageCallback* lpCallback = NULL, DWORD dwTimeout = 500);

	PIPE_ERROR BroadcastPipeMessage(CPipeMessageBase* lpMsg, DWORD dwTimeout = 500, bool bSend = true, CPostPipeMessageCallback* lpCallback = NULL);

public://CBaseThread
	virtual void BeforeRun(LPVOID lpvParam);
	virtual void AfterRun(LPVOID lpvParam);
	virtual LRESULT OnMessage(UINT uMsg, WPARAM wp, LPARAM lp, BOOL& bHandled);

public://CPipeServerEventHandler
	virtual void OnPipesMessage(CPipeMessageBase* lpMessage);
	virtual void OnAddClient(LPCWSTR lpwsClientName);
	virtual void OnRemoveClient(LPCWSTR lpwsClientName, bool bDead);
	virtual void OnClientStateChanged(LPCWSTR lpwsClientName, EHeartState eState);
	virtual void OnForceQuit();

protected:
	void OnPipeMsgOperator(CPipeMessageBase* lpMessage);

	void OnOpHeart(CPipeMessageHeart* lpHeart);
	void OnOpAddClient(CPipeMessageOperator* lpOP);
	void OnOpRemoveClient(CPipeMessageOperator* lpOP);
	void OnHeartCheck();

private:
	CLockCS m_lck;
	CPipeMemberMap m_mapClients;

private:
	CString m_strServerName;
	UINT m_uMaxInstances;
	CPipeServer m_pipeServer;

private://HeartCheck
	UINT_PTR m_timeridHeartCheck;
	DWORD m_dwHeartCheckElapse;
};

