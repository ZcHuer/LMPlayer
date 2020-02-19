#pragma once
#include "Base.h"
#include "BaseThread.h"
#include "Bind.h"
#include <vector>

class IAsyncTask;
class IAsyncTaskStatusCallback
{
public:
	IAsyncTaskStatusCallback(){};
	virtual ~IAsyncTaskStatusCallback(){};

public:
	virtual bool OnStart(IAsyncTask* lpTask) = 0;
	virtual bool OnComplete(IAsyncTask* lpTask) = 0;
};

class IAsyncTask
{
public:
	IAsyncTask(){}
	virtual ~IAsyncTask(){}

public:
	virtual __int64 Id() = 0;
	virtual bool Execute() = 0;
	virtual bool Cancel() = 0;
	virtual bool WaitComplete(DWORD dwTimeout) = 0;
	virtual void SetAsyncTaskStatusCallback(IAsyncTaskStatusCallback* lpStatusCallback) = 0;
};

#define DEF_ASYNCTASK_RETRYCOUNT		2

//=================================================================================================

class CAsyncTaskBase : public IAsyncTask
	, public CBaseThread
{
public:
	CAsyncTaskBase() : m_pCBTaskStatus(NULL), m_n64TaskId(_TaskID())
	{

	}

	CAsyncTaskBase(const CAsyncTaskBase& ref) : m_pCBTaskStatus(ref.m_pCBTaskStatus), m_n64TaskId(ref.m_n64TaskId)
	{

	}

	CAsyncTaskBase& operator=(const CAsyncTaskBase& ref)
	{
		if(this != &ref)
		{
			m_pCBTaskStatus = ref.m_pCBTaskStatus;
			m_n64TaskId = ref.m_n64TaskId;
		}
		return *this;
	}

	virtual ~CAsyncTaskBase()
	{

	}

private:
	static __int64 _TaskID()
	{
		static __int64 s_n64TaskID = 1000;
		static CLockCS s_lock;
		AUTOLOCK(s_lock);
		return ++s_n64TaskID;
	}

public://IAsyncTask
	virtual __int64 Id()
	{
		return m_n64TaskId;
	}

	virtual bool Execute()
	{
		if(CBaseThread::IsRunning())
			return false;

		CStringA strTaskName; strTaskName.Format("Task_%I64u", m_n64TaskId);
		HRESULT hr = CBaseThread::StartThread(strTaskName, NULL);
		return S_OK == hr;
	}

	virtual bool Cancel()
	{
		if(!CBaseThread::IsRunning())
			return true;

		HRESULT hr = CBaseThread::StopThread();
		return S_OK == hr;
	}

	virtual bool WaitComplete(DWORD dwTimeout)
	{
		bool bForceDestroy = false;
		DWORD dwWaitObj = CBaseThread::Wait(NULL, 0, dwTimeout, true);
		switch(dwWaitObj)
		{
		case WAIT_OBJECT_0://abort
			{
				bForceDestroy = true;
				break;
			}
		case WAIT_OBJECT_0 + 1://complete
			{
				break;
			}
		default:
			{
				bForceDestroy = true;
				break;
			}
		}
		if(bForceDestroy && CBaseThread::IsRunning())
		{
			CBaseThread::SetStopTimeout(500);
			DestroyThread();
		}
		return true;
	}

	virtual void SetAsyncTaskStatusCallback(IAsyncTaskStatusCallback* lpStateCallback)
	{
		m_pCBTaskStatus = lpStateCallback;
	}


public://CBaseThread
	virtual void BeforeRun(LPVOID lpvParam)
	{
		__super::BeforeRun(lpvParam);

		if(m_pCBTaskStatus)
			m_pCBTaskStatus->OnStart(this);
	}

	virtual void AfterRun(LPVOID lpvParam)
	{
		if(m_pCBTaskStatus)
			m_pCBTaskStatus->OnComplete(this);
		__super::AfterRun(lpvParam);
	}

	virtual UINT Run(LPVOID lpvParam)
	{
		ATLASSERT(FALSE);
		return 0;
	}

protected:
	IAsyncTaskStatusCallback* m_pCBTaskStatus;
	__int64 m_n64TaskId;
};

//=================================================================================================
class CAsyncTask : public CAsyncTaskBase
{
public:
	CAsyncTask() : m_pFunc(NULL)
	{

	}

	CAsyncTask(FBind::FCall* lpFunc) : m_pFunc(lpFunc)
	{

	}

	CAsyncTask(const CAsyncTask& ref) : m_pFunc(ref.m_pFunc), CAsyncTaskBase(ref)
	{

	}

	CAsyncTask& operator=(const CAsyncTask& ref)
	{
		if(this != &ref)
		{
			FBind::DeleteT(&m_pFunc);
			m_pFunc = ref.m_pFunc;
			CAsyncTaskBase::operator =(ref);
		}
		return *this;
	}

	virtual ~CAsyncTask()
	{
		FBind::DeleteT(&m_pFunc);
	}

public:
	void SetTaskFunc(FBind::FCall* lpFunc)
	{
		FBind::DeleteT(&m_pFunc);
		m_pFunc = lpFunc;
	}

public:
	virtual UINT Run(LPVOID lpvParam)
	{
		if(m_pFunc)
		{
			bool bRtn = m_pFunc->Call();
			ATLASSERT(bRtn);
		}
		return 0;
	}

protected:
	FBind::FCall* m_pFunc;
};


//=================================================================================================
class CTaskQueue
{
public:
	CTaskQueue()
	{

	}

	~CTaskQueue()
	{
		ATLASSERT(m_vecTasks.size() == 0);
	}

public:
	size_t Count()
	{
		return m_vecTasks.size();
	}

	bool Push(IAsyncTask* lpTask)
	{
		if(NULL == lpTask)
			return false;

		m_vecTasks.push_back(lpTask);
		return true;
	}

	IAsyncTask* Pop()
	{
		IAsyncTask* pTask = NULL;
		TaskQueue::iterator it = m_vecTasks.begin();
		if(it != m_vecTasks.end())
		{
			pTask = *it;
			m_vecTasks.erase(it);
		}
		return pTask;
	}

	bool Remove(IAsyncTask* lpTask)
	{
		TaskQueue::iterator it = m_vecTasks.begin();
		for(; it != m_vecTasks.end(); ++it)
		{
			if(*it == lpTask)
			{
				m_vecTasks.erase(it);
				return true;
			}
		}
		return false;
	}

	IAsyncTask* Remove(__int64 n64ID)
	{
		IAsyncTask* pTask = NULL;
		TaskQueue::iterator it = m_vecTasks.begin();
		for(; it != m_vecTasks.end(); ++it)
		{
			if((*it)->Id() == n64ID)
			{
				pTask = *it;
				m_vecTasks.erase(it);
				break;
			}
		}
		return pTask;
	}

private:
	typedef std::vector<IAsyncTask*> TaskQueue;
	TaskQueue m_vecTasks;
};


//=================================================================================================
enum EAddTaskResult
{
	EATR_Fail = -1,
	EATR_OK = 0,
	EATR_Waitting,	
	EATR_Pending,
};

#define TMSG_TASKPOOL_ADDTASK		(WM_USER+1)
#define TMSG_TASKPOOL_DELTASK		(WM_USER+2)
#define TMSG_TASKPOOL_COMPLETE		(WM_USER+3)
#define TMSG_TASKPOOL_NEXTTASK		(WM_USER+4)
#define TMSG_TASKPOOL_FORCECLEAR	(WM_USER+5)

typedef void (__stdcall * PFNExitTask)(IAsyncTask* lpTask);

class CTaskPool : public IAsyncTaskStatusCallback
					, public CMessageThread
{
public:
	CTaskPool(size_t uMaxTaskCount) : m_uMaxTaskCount(uMaxTaskCount), m_bClear(false)
	{

	}

	CTaskPool(const CTaskPool& ref)
	{
		//todo 
		ATLASSERT(FALSE);
	}
	CTaskPool& operator=(const CTaskPool& ref)
	{
		if(this != &ref)
		{
			ATLASSERT(FALSE);
		}
		return *this;
	}

	virtual ~CTaskPool()
	{
		DestroyThread();
	}

public:
	EAddTaskResult AddTask(IAsyncTask* lpTask)
	{
		if(NULL == lpTask)
			return EATR_Fail;

		lpTask->SetAsyncTaskStatusCallback(this);

		if(!CBaseThread::IsRunning())
		{
			HRESULT hr = CMessageThread::StartThread(NULL, NULL);
			if(S_OK != hr)
				return EATR_Fail;
		}
		
		BOOL bPost = CMessageThread::PostThreadMsg(TMSG_TASKPOOL_ADDTASK, 0, (LPARAM)lpTask);
		if(!bPost)
			return EATR_Fail;
		
		return EATR_Pending;
	}

	BOOL RemoveTask(__int64 nTaskID)
	{
		if(!CBaseThread::IsRunning())
			return FALSE;
#ifndef LODWORD
#define LODWORD(_qw)    ((DWORD)(_qw))
#endif //#ifndef LODWORD
#ifndef HIDWORD
#define HIDWORD(_qw)    ((DWORD)(((_qw) >> 32) & 0xffffffff))
#endif //#ifndef HIDWORD
		return CMessageThread::PostThreadMsg(TMSG_TASKPOOL_DELTASK, (WPARAM)HIDWORD(nTaskID), (LPARAM)LODWORD(nTaskID));
	}

	void ClearPool(PFNExitTask pfn)
	{
		{
			AUTOLOCK(m_lock);
			m_bClear = true;
		}

		if(CBaseThread::IsRunning())
		{
			BOOL bPost = CMessageThread::PostThreadMsg(TMSG_TASKPOOL_FORCECLEAR, (WPARAM)pfn, 0);
			if(!bPost)
			{
				ATLASSERT(FALSE);
				return ;
			}
			CBaseThread::Wait(NULL, 0, INFINITE, true);
		}
		m_bClear = false;	
	}

public://CMessageThread
	virtual LRESULT OnMessage(UINT uMsg, WPARAM wp, LPARAM lp, BOOL& bHandled)
	{
		bHandled = TRUE;
		switch(uMsg)
		{
		case TMSG_TASKPOOL_ADDTASK:
			{
				OnMsgAddTask((IAsyncTask*)lp);
				break;
			}
		case TMSG_TASKPOOL_DELTASK:
			{
#ifndef MAKE64
#define MAKE64(l, h)  ((LONGLONG)(((DWORD)((LONGLONG)(l) & 0xffffffff)) | ((LONGLONG)((DWORD)((LONGLONG)(h) & 0xffffffff))) << 32))
#endif //#ifndef MAKE64

				__int64 n64ID = MAKE64(lp,wp);
				OnMsgDelTask(n64ID);
				break;
			}
		case TMSG_TASKPOOL_COMPLETE:
			{
				RemoveCompleteTask((IAsyncTask*)lp);
				{
					AUTOLOCK(m_lock);
					if(m_bClear)
						return 0;
				}
				CMessageThread::PostThreadMsg(TMSG_TASKPOOL_NEXTTASK, 0, 0);
				break;
			}
		case TMSG_TASKPOOL_NEXTTASK:
			{
				{
					AUTOLOCK(m_lock);
					if(m_bClear)
						return 0;
				}

				NextTask();
				break;
			}
		case TMSG_TASKPOOL_FORCECLEAR:
			{
				PFNExitTask pfn = (PFNExitTask)wp;
				ForceClear(pfn);
				CMessageThread::PostThreadMsg(WM_QUIT, 0, 0);
				break;
			}
		default:
			{
				bHandled = FALSE;
				break;
			}
		}
		return 0;
	}

public://IAsyncTaskState
	virtual bool OnStart(IAsyncTask* lpTask)
	{
		return true;
	}

	virtual bool OnComplete(IAsyncTask* lpTask)
	{
		CMessageThread::PostThreadMsg(TMSG_TASKPOOL_COMPLETE, 0, (LPARAM)lpTask);
		return true;
	}

public:
	void RemoveCompleteTask(IAsyncTask* lpTask)
	{
		if(NULL == lpTask)
			return ;

		AUTOLOCK(m_lock);
		bool bRtn = m_runningTask.Remove(lpTask);
		if(bRtn)
			delete lpTask;
	}

	void NextTask()
	{
		{
			AUTOLOCK(m_lock);
			if(m_runningTask.Count() >= m_uMaxTaskCount)
				return ;
		}

		{
			AUTOLOCK(m_lock);		
			IAsyncTask* pTask = m_waitingTask.Pop();
			while(pTask)
			{
				EAddTaskResult rtn = RunTask(pTask);
				if(EATR_Waitting == rtn)
				{
					rtn = AddWaitTask(pTask);
					ATLASSERT(EATR_OK == rtn);
					break;
				}
				else if(EATR_Fail == rtn)
				{
					ATLASSERT(pTask);
					delete pTask;
				}
				else if(EATR_OK == rtn)
				{
					if(m_runningTask.Count() >= m_uMaxTaskCount)
						break;
				}
				else
				{
					ATLASSERT(FALSE);
				}
				pTask = m_waitingTask.Pop();
			}
		}
	}

	void ForceClear(PFNExitTask pfn)
	{
		//Clear Waitting Task
		IAsyncTask* pTask = NULL;
		{
			AUTOLOCK(m_lock);
			pTask = m_waitingTask.Pop();
		}
		while(pTask)
		{
			if(pfn)
				pfn(pTask);
			
			pTask->Cancel();
			pTask->WaitComplete(0);
			delete pTask;

			{
				AUTOLOCK(m_lock);
				pTask = m_waitingTask.Pop();
			}
		}

		//Clear Running Task
		pTask = NULL;
		{
			AUTOLOCK(m_lock);
			pTask = m_runningTask.Pop();
		}
		while(pTask)
		{
			if(pfn)
				pfn(pTask);

			pTask->Cancel();
			pTask->WaitComplete(500);
			delete pTask;

			{
				AUTOLOCK(m_lock);
				pTask = m_runningTask.Pop();
			}
		}
	}

protected:
	void OnMsgAddTask(IAsyncTask* lpTask)
	{
		if(NULL == lpTask)
		{
			ATLASSERT(FALSE);
			return ;
		}

		EAddTaskResult rtn = RunTask(lpTask);
		if(EATR_OK == rtn)
		{
			return ;
		}
		else if(EATR_Waitting == rtn)
		{
			rtn = AddWaitTask(lpTask);
			if(EATR_OK == rtn)
			{
				BOOL bPost = CMessageThread::PostThreadMsg(TMSG_TASKPOOL_NEXTTASK, 0, 0);
				if(bPost)
					return ;
			}
		}
		ATLASSERT(FALSE);
	}

	void OnMsgDelTask(__int64 n64ID)
	{
		//Clear Waitting Task
		{
			AUTOLOCK(m_lock);
			IAsyncTask* pTask = m_waitingTask.Remove(n64ID);
			if(pTask)
			{
				pTask->Cancel();
				pTask->WaitComplete(0);
				delete pTask;
			}
		}

		//Clear Running Task
		{
			AUTOLOCK(m_lock);
			IAsyncTask* pTask = m_runningTask.Remove(n64ID);
			if(pTask)
			{
				pTask->Cancel();
				pTask->WaitComplete(500);
				delete pTask;
			}
		}
	}

	EAddTaskResult RunTask(IAsyncTask* lpTask)
	{
		if(NULL == lpTask)
		{
			ATLASSERT(FALSE);
			return EATR_Fail;
		}

		AUTOLOCK(m_lock);
		if(m_runningTask.Count() >= m_uMaxTaskCount)
		{
			return EATR_Waitting;
		}
		
		m_runningTask.Push(lpTask);

		if(lpTask->Execute())
			return EATR_OK;

		m_runningTask.Remove(lpTask);
		return EATR_Fail;
	}

	EAddTaskResult AddWaitTask(IAsyncTask* lpTask)
	{
		if(NULL == lpTask)
		{
			ATLASSERT(FALSE);
			return EATR_Fail;
		}

		AUTOLOCK(m_lock);
		m_waitingTask.Push(lpTask);
		return EATR_OK;
	}

protected:
	size_t m_uMaxTaskCount;	
	
	CTaskQueue m_waitingTask;
	CTaskQueue m_runningTask;

	CLockCS m_lock;
	bool m_bClear;
};