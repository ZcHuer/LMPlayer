#include "stdafx.h"
#include "PipeMessageBase.h"
#include <typeinfo>

CPipeMessageBase::CPipeMessageBase(DWORD dwMessage) : m_dwMessageID(dwMessage), m_uParamSize(0)
{

}

CPipeMessageBase::~CPipeMessageBase(void)
{

}

extern _PIPEMSG_MAP_ENTRY* _GetPipeMsgEntry(DWORD dwMsgID);
HRESULT CPipeMessageBase::CreatePipeMessageObj(CPipeStreamMarshal* lpMarshalStream, CPipeMessageBase** lppPipeMsg)
{
	if(NULL == lpMarshalStream || NULL == lppPipeMsg)
		return E_INVALIDARG;
	
	PipeMsgMarshalHeader pm = {0};
	HRESULT hr = CPipeMessageBase::GetPipeMsgMarshalHeader(lpMarshalStream, pm);
	if(FAILED(hr))
		return hr;

	CPipeMessageBase* pMsg = NULL;
	hr = CreatePipeMessageObj(pm.dwMessage, &pMsg);
	if(FAILED(hr))
		return hr;

	hr = pMsg->UnmarshalPipeMsg(lpMarshalStream);
	if(SUCCEEDED(hr))
	{
		*lppPipeMsg = pMsg;
	}
	else
	{
		ULONG uRef = pMsg->Release();
		ATLASSERT(uRef == 0);
	}	
	return hr;
}

HRESULT CPipeMessageBase::CreatePipeMessageObj(DWORD dwMsgID, CPipeMessageBase** lppPipeMsg)
{
	if(NULL == lppPipeMsg)
		return E_INVALIDARG;

	_PIPEMSG_MAP_ENTRY* pEntry = _GetPipeMsgEntry(dwMsgID);
	if(NULL == pEntry)
		return E_NOTIMPL;

	CPipeMessageBase* pObj = NULL;
	HRESULT hr = pEntry->pfnCreateInstance(dwMsgID, (LPVOID*)&pObj);
	if(FAILED(hr))
		return hr;

	*lppPipeMsg = pObj;
	pObj->AddRef();
	return S_OK;
}

UINT CPipeMessageBase::GetPipeMsgMarshalHeaderSize()
{
	return sizeof(PipeMsgMarshalHeader) - sizeof((*(PipeMsgMarshalHeader*)0).byzParam);
}

HRESULT CPipeMessageBase::GetPipeMsgMarshalHeader(CPipeStreamMarshal* lpBuffer, PipeMsgMarshalHeader& pipemsgMarshalHeader)
{
	if(NULL == lpBuffer)
		return E_INVALIDARG;

	UINT uSize = lpBuffer->GetStreamSize();
	if(uSize < GetPipeMsgMarshalHeaderSize())
		return E_INVALIDARG;

	memcpy(&pipemsgMarshalHeader, lpBuffer->GetStream(), GetPipeMsgMarshalHeaderSize());

	if(pipemsgMarshalHeader.uFlag != PIPEMSG_FLAG)
		return E_FAIL;

	return S_OK;
}

HRESULT CPipeMessageBase::FillPipeMsgMarshalHeader(DWORD dwMsgID, LPCWSTR lpwsSender, LPCWSTR lpwsReceiver, DWORD dwDataLength, CPipeStreamMarshal* lpBuffer)
{
	if(NULL == lpBuffer)
		return E_INVALIDARG;

	UINT uSize = lpBuffer->GetStreamSize();
	if(uSize < CPipeMessageBase::GetPipeMsgMarshalHeaderSize())
		return E_OUTOFMEMORY;

	PipeMsgMarshalHeader pm = {0};
	pm.uFlag = PIPEMSG_FLAG;
	pm.uVersion = PIPEMSG_VERSION;
	pm.dwMessage = dwMsgID;
	lstrcpyn(pm.wszSender, lpwsSender, PIPEMSG_NAME_LEGNTH);
	lstrcpyn(pm.wszReceiver, lpwsReceiver, PIPEMSG_NAME_LEGNTH);
	pm.cbParamSize = dwDataLength;

	memcpy(lpBuffer->GetStream(), &pm, GetPipeMsgMarshalHeaderSize());
	return S_OK;
}

//=================================================================================================
DWORD CPipeMessageBase::GetMessage()
{
	return m_dwMessageID;
}

BOOL CPipeMessageBase::SetMessageInfo(LPCWSTR lpwsSender, LPCWSTR lpwsReceiver)
{
	if(NULL == lpwsSender
		|| NULL == lpwsReceiver)
	{
		return FALSE;
	}
	m_strSender = lpwsSender;
	m_strReceiver = lpwsReceiver;
	return TRUE;
}

LPCWSTR CPipeMessageBase::GetSender()
{
	return m_strSender;
}

LPCWSTR CPipeMessageBase::GetReceiver()
{
	return m_strReceiver;
}

HRESULT CPipeMessageBase::SetMessageID(DWORD dwMsgID)
{
	m_dwMessageID = dwMsgID;
	return S_OK;
}

CPipeStream* CPipeMessageBase::GetParamBuffer()
{
	CPipeStream* pBuf = new CPipeStream();
	if(pBuf)
	{
		if(pBuf->CreateStream(m_uParamSize))
		{
			memcpy(pBuf->GetStream(), m_buffer.GetStream()+GetPipeMsgMarshalHeaderSize(), m_uParamSize);
		}
		else
		{
			delete pBuf;
			pBuf = NULL;
		}
	}
	return pBuf;
}

//=================================================================================================
HRESULT CPipeMessageBase::Dump(CString& strDump)
{
	CString strDump1;
	HRESULT hr1 = OnDump(strDump1);
	if(S_OK != hr1)
	{
		CPipeStreamMarshal bufEx;
		if(FAILED(OnMarshalPipeMsg(bufEx)))
		{
			strDump1 = L"Marshal Message Failed";
		}	
		else
		{
			CString strMarshalDump;
			HRESULT hr = DumpMarshalStream(bufEx, strMarshalDump);
			if(SUCCEEDED(hr))
			{
				if(hr1 == S_FALSE)
					strDump1.Append(strMarshalDump);
				else
					strDump1 = strMarshalDump;
			}
			else
			{
				if(hr1 == S_FALSE)
					strDump1.Append(L"DumpMarshalStreamFailed");
				else
					strDump1 = L"DumpMarshalStreamFailed";
			}
		}
	}
	SYSTEMTIME st ={0};
	GetLocalTime(&st);

	_PIPEMSG_MAP_ENTRY* pEntry = _GetPipeMsgEntry(GetMessage());

	CString strClassName((LPCSTR)(typeid(*this).name()));
	strDump.Format(L"===>>>PIPE MSG DUMP BEGIN<<<===[PID:%d, TID:%d]\r\n"
					L"[%d, %s(%s), From:%s, To:%s]\r\n"
					L"\tMessageData[\r\n%s\t]\r\n"
					L"[%04d-%02d-%02d %02d:%02d:%02d.%03d]\r\n"
					L"===>>> PIPE MSG DUMP END <<<===\r\n", 
					GetCurrentProcessId(), GetCurrentThreadId(),
					GetMessage(), pEntry?pEntry->pwsMessageName:L"UnknownMessage", 
					(LPCWSTR)strClassName, GetSender(), GetReceiver(), 
					(LPCWSTR)strDump1,
					(DWORD)st.wYear, (DWORD)st.wMonth, (DWORD)st.wDay, 
					(DWORD)st.wHour, (DWORD)st.wMinute, (DWORD)st.wSecond, (DWORD)st.wMilliseconds);
	return hr1;
}

HRESULT CPipeMessageBase::MarshalPipeMsg(CPipeStreamMarshal* lpBuffer)
{
	if(NULL == lpBuffer)
		return E_INVALIDARG;

	CPipeStreamMarshal bufEx;
	if(FAILED(OnMarshalPipeMsg(bufEx)))
		return E_FAIL;

	UINT uExLength = bufEx.GetPointer();//缓冲区可能很大
	UINT uBufferSize = GetPipeMsgMarshalHeaderSize() + uExLength;
	if(NULL == lpBuffer->CreateStream(uBufferSize))
		return E_OUTOFMEMORY;

	HRESULT hr = FillPipeMsgMarshalHeader(GetMessage(), GetSender(), GetReceiver(), uExLength, lpBuffer);
	if(FAILED(hr))
		return hr;

	memcpy(lpBuffer->GetStream() + GetPipeMsgMarshalHeaderSize(), bufEx.GetStream(), uExLength);

	return S_OK;
}

HRESULT CPipeMessageBase::UnmarshalPipeMsg(CPipeStreamMarshal* lpBuffer)
{
	if(NULL == lpBuffer)
		return E_INVALIDARG;

	PipeMsgMarshalHeader pm = {0};
	HRESULT hr = CPipeMessageBase::GetPipeMsgMarshalHeader(lpBuffer, pm);
	if(FAILED(hr))
		return hr;

	LPBYTE pbyBuf = m_buffer.CreateStream(GetPipeMsgMarshalHeaderSize() + pm.cbParamSize);
	if(NULL == pbyBuf)
		return E_FAIL;

	memcpy(pbyBuf, lpBuffer->GetStream(), GetPipeMsgMarshalHeaderSize() + pm.cbParamSize);

	pm.wszReceiver[PIPEMSG_NAME_LEGNTH-1] = L'\0';
	pm.wszSender[PIPEMSG_NAME_LEGNTH-1] = L'\0';
	m_dwMessageID = pm.dwMessage;
	m_strReceiver = pm.wszReceiver;
	m_strSender = pm.wszSender;
	m_uParamSize = pm.cbParamSize;

	//解析参数
	hr = S_FALSE;
	CPipeStream* pParamBuf = GetParamBuffer();
	if(pParamBuf)
	{
		CPipeStreamMarshal bufEx;
		bufEx.Attach(pParamBuf->GetStream(), pParamBuf->GetStreamSize());

		hr = OnUnmarshalPipeMsg(bufEx);
		
		bufEx.Detach();
		delete pParamBuf;
	}
	return hr;
}

HRESULT CPipeMessageBase::DumpMarshalStream(CPipeStreamMarshal& buf, CString& strDump)
{
	UINT uSize = buf.GetStreamSize();
	UINT uCurrentSize = buf.GetPointer();

	buf.SetPointer(0);
	LPBYTE pbyBuf = buf.GetStream();

	bool bRtn = false;
	DWORD dwIndex = 0;
	while(buf.GetPointer() + sizeof(PIPE_DATATYPE) < uCurrentSize)
	{
		++dwIndex;
		CString strData;
		PIPE_DATATYPE* pDataType = (PIPE_DATATYPE*)(pbyBuf + buf.GetPointer());
		switch(*pDataType)
		{
		case EDT_BYTE:
			{
				BYTE byData = 0;
				bRtn = buf.GetBYTE(byData);
				if(bRtn)
					strData.Format(L"\t\tData_%d [%s]\t[%02X,\t%d]\r\n", dwIndex, L"BYTE", (DWORD)byData, (DWORD)byData);
				break;
			}
		case EDT_CHAR:
			{
				CHAR chData = 0;
				bRtn = buf.GetCHAR(chData);
				strData.Format(L"\t\tData_%d [%s]\t[%02X,\t%d]\r\n", dwIndex, L"CHAR", (LONG)chData, (LONG)chData);
				break;
			}
		case EDT_USHORT:
			{
				USHORT usData = 0;
				bRtn = buf.GetUSHORT(usData);
				strData.Format(L"\t\tData_%d [%s]\t[%04X,\t%d]\r\n", dwIndex, L"USHORT", (DWORD)usData, (DWORD)usData);
				break;
			}
		case EDT_SHORT:
			{
				SHORT shData = 0;
				bRtn = buf.GetSHORT(shData);
				strData.Format(L"\t\tData_%d [%s]\t[%04X,\t%d]\r\n", dwIndex, L"SHORT", (LONG)shData, (LONG)shData);
				break;
			}
		case EDT_UINT:
			{
				UINT uData = 0;
				bRtn = buf.GetUINT(uData);
				strData.Format(L"\t\tData_%d [%s]\t[%08X,\t%d]\r\n", dwIndex, L"UINT", uData, uData);
				break;
			}
		case EDT_INT:
			{
				INT nData = 0;
				bRtn = buf.GetINT(nData);
				strData.Format(L"\t\tData_%d [%s]\t[%08X,\t%d]\r\n", dwIndex, L"INT", nData, nData);
				break;
			}
		case EDT_ULONG:
			{
				ULONG uData = 0;
				bRtn = buf.GetULONG(uData);
				strData.Format(L"\t\tData_%d [%s]\t[%08X,\t%d]\r\n", dwIndex, L"ULONG", uData, uData);
				break;
			}
		case EDT_LONG:
			{
				LONG lData = 0;
				bRtn = buf.GetLONG(lData);
				strData.Format(L"\t\tData_%d [%s]\t[%08X,\t%d]\r\n", dwIndex, L"LONG", lData, lData);
				break;
			}
		case EDT_UINT64:
			{
				ULONGLONG ullData = 0;
				bRtn = buf.GetUINT64(ullData);
				strData.Format(L"\t\tData_%d [%s]\t[%I64X,\t%I64d]\r\n", dwIndex, L"UINT64", ullData, ullData);
				break;
			}
		case EDT_INT64:
			{
				LONGLONG llData = 0;
				bRtn = buf.GetINT64(llData);
				strData.Format(L"\t\tData_%d [%s]\t[%I64X,\t%I64d]\r\n", dwIndex, L"INT64", llData, llData);
				break;
			}
		case EDT_TEXT_W:
			{
				CString str;
				bRtn = ParseStringW(buf, str);
				if(str.GetLength() > 20)
				{
					str.Left(15);
					str.Append(L"...");
				}
				strData.Format(L"\t\tData_%d [%s]\t[%s]\r\n", dwIndex, L"TEXTW", (LPCWSTR)str);
				break;
			}
		case EDT_TEXT_A:
			{
				CStringA strA;
				bRtn = ParseStringA(buf, strA);
				CString str((LPCSTR)strA);
				if(str.GetLength() > 20)
				{
					str.Left(15);
					str.Append(L"...");
				}
				strData.Format(L"\t\tData_%d [%s]\t[%s]\r\n", dwIndex, L"TEXTA", (LPCWSTR)str);
				break;
			}
		case EDT_BYTEARRAY:
			{
				CString str;
				UINT uLength = 0;
				bRtn = buf.GetByteLength(uLength);
				if(bRtn && uLength > 0)
				{
					LPBYTE pbyData = new BYTE[uLength];
					bRtn = buf.GetByteArray(pbyData, uLength);
					for(DWORD i = 0; bRtn && i < 32 && i < uLength; ++i)
					{
						WCHAR wszData[8] = {0};
						_ltow_s((long)pbyData[i], wszData, 8, 16);
						str.Append(wszData);
						wszData[0] = 0;
					}
					if(uLength > 32)
						str.Append(L"...");
				}
				strData.Format(L"\t\tData_%d [%s]\t[%s]\r\n", dwIndex, L"BINARY", (LPCWSTR)str);
				break;
			}
		}//switch
		if(!bRtn)
		{
			strData.Append(L"DataMarshalFailed\r\n");
			break;
		}
		strDump.Append(strData);
	}//while
	return bRtn?S_OK:E_FAIL;
}