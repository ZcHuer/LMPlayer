#pragma once
#include "PipeGlobal.h"
#include "PipeStream.h"

#define PIPEMSG_NAME_LEGNTH					128
#define PIPEMSG_FLAG						0x47534D50
#define PIPEMSG_VERSION						1

#pragma pack(1)
struct PipeMsgMarshalHeader
{
	UINT	uFlag;							//消息标识'PMSG'
	UINT	uVersion;						//消息版本
	DWORD	dwMessage;						//消息号码
	WCHAR	wszSender[PIPEMSG_NAME_LEGNTH];		//发送管道名
	WCHAR	wszReceiver[PIPEMSG_NAME_LEGNTH];	//接收管道名
	UINT	cbParamSize;					//参数区长度
	BYTE	byzParam[1];					//参数区,具体格式由消息类型决定
};
#pragma pack()


class CRefPipeMessage : public IUnknown
{
public:
	CRefPipeMessage() : m_lRefCount(0) {}
	virtual ~CRefPipeMessage() { ATLASSERT(m_lRefCount == 0);}

public:
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject){return E_NOINTERFACE;}
	virtual ULONG STDMETHODCALLTYPE AddRef( void)
	{
		return InterlockedIncrement(&m_lRefCount);
	}
	virtual ULONG STDMETHODCALLTYPE Release( void)
	{
		LONG lRef = InterlockedDecrement(&m_lRefCount);
		if(lRef == 0)
		{
			delete this;
		}
		ATLASSERT(lRef >= 0);
		return lRef;
	}
private:
	LONG m_lRefCount;
};

template<class T>
class CStackPipeMessage : public T
{
public:
	CStackPipeMessage():T()
	{

	}
	virtual ~CStackPipeMessage()
	{

	}

public:
	virtual ULONG STDMETHODCALLTYPE Release( void)
	{
		return 1;
	}
};

template<class T>
class CStackPipeMessage2 : public T
{
public:
	CStackPipeMessage2(DWORD dwMsgID):T(dwMsgID){}
	virtual ~CStackPipeMessage2(){}

public:
	virtual ULONG STDMETHODCALLTYPE Release( void)
	{
		return 1;
	}
};

class CPipeMessageBase : public  CRefPipeMessage
{
public:
	CPipeMessageBase(DWORD dwMessage);
	virtual ~CPipeMessageBase(void);

	static HRESULT CreatePipeMessageObj(CPipeStreamMarshal* lpMarshalStream, CPipeMessageBase** lppPipeMsg);
	static HRESULT CreatePipeMessageObj(DWORD dwMsgID, CPipeMessageBase** lppPipeMsg);
	template<class T>
	static HRESULT CreatePipeMessageObjT(DWORD dwMsgID, T** lppPipeMsg)
	{
		CPipeMessageBase* pMsg = NULL;
		HRESULT hr = CreatePipeMessageObj(dwMsgID, &pMsg);
		if(FAILED(hr))
			return hr;

		*lppPipeMsg = dynamic_cast<T*>(pMsg);
		if(NULL == (*lppPipeMsg))
		{
			ULONG uRef = pMsg->Release();
			ATLASSERT(uRef == 0);
			return E_NOINTERFACE;
		}
		return S_OK;
	}

protected:
	static UINT GetPipeMsgMarshalHeaderSize();
	static HRESULT GetPipeMsgMarshalHeader(CPipeStreamMarshal* lpBuffer, PipeMsgMarshalHeader& pipemsgMarshalHeader);
	static HRESULT FillPipeMsgMarshalHeader(DWORD dwMsgID, LPCWSTR lpwsSender, LPCWSTR lpwsReceiver, DWORD dwDataLength, CPipeStreamMarshal* lpBuffer);

public:
	DWORD GetMessage();
	BOOL SetMessageInfo(LPCWSTR lpwsSender, LPCWSTR lpwsReceiver);
	LPCWSTR GetSender();
	LPCWSTR GetReceiver();

protected:
	HRESULT SetMessageID(DWORD dwMsgID);
	CPipeStream* GetParamBuffer();

public:
	virtual HRESULT Dump(CString& strDump);
	virtual HRESULT MarshalPipeMsg(CPipeStreamMarshal* lpBuffer);
	virtual HRESULT UnmarshalPipeMsg(CPipeStreamMarshal* lpBuffer);

protected:
	virtual HRESULT OnDump(CString& strDump){return E_NOTIMPL;}
	virtual HRESULT OnMarshalPipeMsg(CPipeStreamMarshal& buf){return S_FALSE;}
	virtual HRESULT OnUnmarshalPipeMsg(CPipeStreamMarshal& buf){return S_FALSE;}

private:
	HRESULT DumpMarshalStream(CPipeStreamMarshal& buf, CString& strDump);
		
private:
	DWORD	m_dwMessageID;
	CString m_strSender;
	CString m_strReceiver;
	UINT m_uParamSize;
	CPipeStream m_buffer;
};

template<class T>
HRESULT __stdcall __CreateInstance(T** lppMsg)
{
	if(NULL == lppMsg)
		return E_INVALIDARG;

	T* pT = new T();
	if(NULL == pT)
		return E_OUTOFMEMORY;

	*lppMsg = pT;
	return S_OK;
}

template<class T>
HRESULT __stdcall __CreateInstance(DWORD dwMessage, T** lppMsg)
{
	if(NULL == lppMsg)
		return E_INVALIDARG;

	T* pT = new T(dwMessage);
	if(NULL == pT)
		return E_OUTOFMEMORY;

	*lppMsg = pT;
	return S_OK;
}

#define DEFAULT_PIPEMSG_CREATOR(cls) public: static HRESULT __stdcall _CreateInstance(DWORD dwMessage, LPVOID* lppMsg) \
											{ cls* pMsg = NULL; \
												HRESULT hr = __CreateInstance<cls>(&pMsg); \
												if(SUCCEEDED(hr)){*lppMsg = (LPVOID)dynamic_cast<CPipeMessageBase*>(pMsg);} \
											return hr;}

#define DEFAULT_PIPEMSG_CREATOR2(cls) public: static HRESULT __stdcall _CreateInstance(DWORD dwMessage, LPVOID* lppMsg) \
											{ cls* pMsg = NULL; \
												HRESULT hr = __CreateInstance<cls>(dwMessage, &pMsg); \
												if(SUCCEEDED(hr)){*lppMsg = (LPVOID)dynamic_cast<CPipeMessageBase*>(pMsg);} \
											return hr;}

#define PIPEMSG_PROP_MEMBER(t,n)			private: t m_##n;
#define PIPEMSG_PROP_GET(t,n)				public: t GetProp_##n (){return m_##n;}
#define PIPEMSG_PROP_SET(t,n)				public: void SetProp_##n (t prop##n){m_##n = prop##n;}

#define PIPEMSG_PROP_JUMP_GET(t, n, j)		public: t GetProp_##n (){return GetProp_##j ();}
#define PIPEMSG_PROP_JUMP_SET(t, n, j)		public: void SetProp_##n (t prop##n){ SetProp_##j ((t)prop##n); }

#define PIPEMSG_PROP_MEMJUMP_GET(t, n, j)	public: t GetProp_##n (){return (t)m_##j;}
#define PIPEMSG_PROP_MEMJUMP_SET(t, n, j)	public: void SetProp_##n (t prop##n){ m_##j =(t)prop##n; }

#define PIPEMSG_PROP(t,n)					PIPEMSG_PROP_MEMBER(t,n) \
											PIPEMSG_PROP_GET(t,n) \
											PIPEMSG_PROP_SET(t,n)


#define PIPEMSG_PROP2(t,mt,n)				PIPEMSG_PROP_MEMBER(mt,n) \
											PIPEMSG_PROP_GET(t,n) \
											PIPEMSG_PROP_SET(t,n)

#define PIPEMSG_PROP_JUMP(t, n, j)			PIPEMSG_PROP_JUMP_GET(t, n, j) \
											PIPEMSG_PROP_JUMP_SET(t, n, j)

#define PIPEMSG_PROP_MEMJUMP(t, n, j)		PIPEMSG_PROP_MEMBER(t, j) \
											PIPEMSG_PROP_MEMJUMP_GET(t, n, j) \
											PIPEMSG_PROP_MEMJUMP_SET(t, n, j)

#define PIPEMSG_DUMP_LEVEL_0				0//Dump Nothing
#define PIPEMSG_DUMP_LEVEL_1				1//Dump All Pipe Message
#define PIPEMSG_DUMP_LEVEL_2				2//Dump Pipe Message Except PIPEMSG_SYSTEM_OPERATOR and OPCode = PIPEMSG_SYSTEM_OP_HEART
#define PIPEMSG_DUMP_LEVEL_3				3//Dump Pipe Message MsgID > PIPEMSG_BASE

#define PIPEMSG_DUMP_LEVEL					PIPEMSG_DUMP_LEVEL_2

#ifndef PIPEMSG_DUMP
#define PIPEMSG_DUMP(x)						if(x){ bool bDump = false; \
												switch(PIPEMSG_DUMP_LEVEL) \
												{case PIPEMSG_DUMP_LEVEL_1:{bDump = true;} break; \
													case PIPEMSG_DUMP_LEVEL_2:{bDump = ((x)->GetMessage() != PIPEMSG_SYSTEM_HEART);}break; \
													case PIPEMSG_DUMP_LEVEL_3:{bDump = ((x)->GetMessage() > PIPEMSG_BASE);}break; \
													case PIPEMSG_DUMP_LEVEL_0:break; \
													default: break; \
												} if(bDump) {CString strDump; (x)->Dump(strDump); PIPE_DEBUG(strDump);} }
#endif //#ifndef PIPEMSG_DUMP


#define _PIPEMSGMAPENTRY_FAIL_(m, c, p) { PIPE_DEBUG(L"ERROR!!! PIPEMSG_MAP Fail : [%d] dynamic_cast<%s *>\r\n", (m), _T(#c)); PIPEMSG_DUMP(p); ATLASSERT(FALSE); }
#define _PIPEMSGMAPEND_FAIL_(p) if(!bRtn){PIPE_DEBUG(L"ERROR!!! PIPEMSG_MAP Fail : No Deal \r\n"); PIPEMSG_DUMP(p);}

/*
DealedMsg : return true

bool MsgFunc(TTT* lpMessage)

bool _PipeMessageProc(CPipeMessageBase* lpMessage);
*/
#define BEGIN_PIPEMSG_MAP(x) public: virtual bool _PipeMessageProc(CPipeMessageBase* lpMessage) \
												{	if(NULL == lpMessage){ATLASSERT(FALSE);return false;} \
													bool bRtn = false; \
													switch(lpMessage->GetMessage()) { \

#define PIPEMSG_MAP_ENTRY(m, c, f)	case m: { c* pMsg = dynamic_cast<c*>(lpMessage); if(pMsg){ bRtn = f(pMsg);}else{_PIPEMSGMAPENTRY_FAIL_(m,c,lpMessage);}} break; 

#define PIPEMSG_MAP_CHAIN(c) default:{bRtn = c::_PipeMessageProc(lpMessage);if(bRtn){break;}}
#define PIPEMSG_MAP_CHAIN_NEXT(c) {bRtn = c::_PipeMessageProc(lpMessage);if(bRtn){break;}}

#define END_PIPEMSG_MAP() } _PIPEMSGMAPEND_FAIL_(lpMessage); return bRtn;}