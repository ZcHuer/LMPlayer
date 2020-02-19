#pragma once
#include "stdafx.h"
#include "BindHelper.h"

namespace FBind
{

CFCallWorker::CFCallWorker(void) : m_dwResultPtr(0)
{

}

CFCallWorker::~CFCallWorker(void)
{

}


//=================================================================================================
//FBind::IFCallWorker
UINT CFCallWorker::s_uFCallRegMsg = 0;

DWORD CFCallWorker::GetWorkerThreadID()
{
	return CMessageThread::GetThreadID();
}

BOOL CFCallWorker::SyncFCall(FBind::FCall* lpCall, DWORD_PTR dwpResultPtr)
{
	if(s_uFCallRegMsg == 0)
	{
		ATLASSERT(FALSE);
		return FALSE;
	}

	_FCallInfo param(lpCall, dwpResultPtr, FALSE);
	BOOL bRtn = CMessageThread::SendThreadMsg(s_uFCallRegMsg, (WPARAM)&param, 0, NULL);
	if(!bRtn)
	{
		ATLASSERT(FALSE);
	}
	return bRtn;
}

BOOL CFCallWorker::AsyncFCall(FBind::FCall* lpCall, DWORD_PTR dwpResultPtr)
{
	if(s_uFCallRegMsg == 0)
	{
		ATLASSERT(FALSE);
		return FALSE;
	}
	_FCallInfo* param = new _FCallInfo(lpCall, dwpResultPtr, TRUE);
	BOOL bRtn = CMessageThread::PostThreadMsg(s_uFCallRegMsg, (WPARAM)param, 0);
	if(!bRtn)
	{
		ATLASSERT(FALSE);
		if(param)
			delete param;
	}
	return bRtn;
}

BOOL CFCallWorker::AsyncFCall(FBind::FCall* lpCall, DWORD_PTR dwpResultPtr, HMODULE hModule)
{
	if (s_uFCallRegMsg == 0)
	{
		ATLASSERT(FALSE);
		return FALSE;
	}
	_FCallInfo* param = new _FCallInfo(lpCall, dwpResultPtr, TRUE, hModule);
	BOOL bRtn = CMessageThread::PostThreadMsg(s_uFCallRegMsg, (WPARAM)param, 0);
	if (!bRtn)
	{
		ATLASSERT(FALSE);
		if (param)
			delete param;
	}
	return bRtn;
}

BOOL CFCallWorker::AsyncFCall(FBind::FCall* lpCall, DWORD_PTR dwpResultPtr, HMODULE hModule, LPCSTR szFunc)
{
	if (s_uFCallRegMsg == 0)
	{
		ATLASSERT(FALSE);
		return FALSE;
	}
	_FCallInfo* param = new _FCallInfo(lpCall, dwpResultPtr, TRUE, hModule, szFunc);
	BOOL bRtn = CMessageThread::PostThreadMsg(s_uFCallRegMsg, (WPARAM)param, 0);
	if (!bRtn)
	{
		ATLASSERT(FALSE);
		if (param)
			delete param;
	}
	return bRtn;
}

DWORD_PTR CFCallWorker::GetCurrentResultPtr()
{
	return m_dwResultPtr;
}

//=================================================================================================
//CMessageThread
void CFCallWorker::BeforeRun(LPVOID lpvParam)
{
	CFCallWorker::SetCurrentThreadWorker(this);
	s_uFCallRegMsg = ::RegisterWindowMessage(L"FCallWorker_FCall_Register_Message_String");
	__super::BeforeRun(lpvParam);
}

void CFCallWorker::AfterRun(LPVOID lpvParam)
{
	__super::AfterRun(lpvParam);
	CFCallWorker::SetCurrentThreadWorker(NULL);
}

LRESULT CFCallWorker::OnMessage(UINT uMsg, WPARAM wp, LPARAM lp, BOOL& bHandled)
{
	if(s_uFCallRegMsg == uMsg)
	{
		bHandled = TRUE;
	
		_FCallInfo* param = reinterpret_cast<_FCallInfo*>(wp);
		if(param)
		{
			m_dwResultPtr = param->_dwResultPtr;
			FBind::FCall* pCall = param->_pCall;
			if(pCall)
			{
				pCall->Call();
				if(param->_bPost)
					FBind::DeleteT(&pCall);
			}
			m_dwResultPtr = 0;

			if(param->_bPost)
				delete param;
		}		
		return 0;
	}
	return __super::OnMessage(uMsg, wp, lp, bHandled);
}


//=================================================================================================
DWORD CFCallWorker::_TLSIndexInitializer::s_dwTLSIndex = TLS_OUT_OF_INDEXES;

CFCallWorker::_TLSIndexInitializer::_TLSIndexInitializer()
{
	CFCallWorker::_TLSIndexInitializer::s_dwTLSIndex = TlsAlloc();
	if(CFCallWorker::_TLSIndexInitializer::s_dwTLSIndex == TLS_OUT_OF_INDEXES)
		throw(CFCallWorker::_TLSIndexInitializer::s_dwTLSIndex);
}

CFCallWorker::_TLSIndexInitializer::~_TLSIndexInitializer()
{
	if(TLS_OUT_OF_INDEXES != CFCallWorker::_TLSIndexInitializer::s_dwTLSIndex)
	{
		TlsFree(CFCallWorker::_TLSIndexInitializer::s_dwTLSIndex);
		CFCallWorker::_TLSIndexInitializer::s_dwTLSIndex = TLS_OUT_OF_INDEXES;
	}
}

DWORD CFCallWorker::_TLSIndexInitializer::GetTLSIndex()
{
	return s_dwTLSIndex;
}

CFCallWorker::_TLSIndexInitializer CFCallWorker::s_TLSIndex;
CFCallWorker* CFCallWorker::GetCurrentThreadWorker()
{
	if(TLS_OUT_OF_INDEXES == s_TLSIndex.GetTLSIndex())
	{
		ATLASSERT(FALSE);
		return NULL;
	}

	CFCallWorker* pCurThread = (CFCallWorker*)TlsGetValue(s_TLSIndex.GetTLSIndex());
	return pCurThread;
}

BOOL CFCallWorker::SetCurrentThreadWorker(CFCallWorker* lpWorker)
{
	if(TLS_OUT_OF_INDEXES == s_TLSIndex.GetTLSIndex())
	{
		ATLASSERT(FALSE);
		return FALSE;
	}
	return TlsSetValue(s_TLSIndex.GetTLSIndex(), (LPVOID)lpWorker);
}

}//namespace