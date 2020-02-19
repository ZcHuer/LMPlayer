#pragma once
#include "PipeMessageBase.h"

class CPipeMessage : public CPipeMessageBase
{
	DEFAULT_PIPEMSG_CREATOR2(CPipeMessage)

public:
	CPipeMessage(DWORD dwMessage) : CPipeMessageBase(dwMessage)
	{
		SetProp_Param0(0);
		SetProp_Param1(0);
		SetProp_Param2W(L"");
		SetProp_Param2A("");
	}
	virtual ~CPipeMessage(void){}

public:
	PIPEMSG_PROP(UINT, Param0)
	PIPEMSG_PROP(UINT, Param1)
	PIPEMSG_PROP2(LPCWSTR, CString, Param2W)
	PIPEMSG_PROP2(LPCSTR, CStringA, Param2A)

protected:
	virtual HRESULT OnMarshalPipeMsg(CPipeStreamMarshal& buf)
	{
		bool bRtn = true;
		bRtn = bRtn && buf.AppendUINT(GetProp_Param0());
		bRtn = bRtn && buf.AppendUINT(GetProp_Param1());
		bRtn = bRtn && buf.AppendTextW(GetProp_Param2W());
		bRtn = bRtn && buf.AppendTextA(GetProp_Param2A());
		return bRtn ? S_OK : E_FAIL;
	}

	virtual HRESULT OnUnmarshalPipeMsg(CPipeStreamMarshal& buf)
	{
		bool bRtn = true;
		bRtn = bRtn && buf.GetUINT(m_Param0);
		bRtn = bRtn && buf.GetUINT(m_Param1);
		bRtn = bRtn && ParseStringW(buf, m_Param2W);
		bRtn = bRtn && ParseStringA(buf, m_Param2A);
		return bRtn ? S_OK : E_FAIL;
	}
};

//=================================================================================================
typedef CPipeMessage CPipeMessageResult;
PIPEMSG_ENTRY_AUTO(PIPEMSG_SYSTEM_RESULT, CPipeMessageResult)

//=================================================================================================
typedef CPipeMessage CPipeMessageHeart;
PIPEMSG_ENTRY_AUTO(PIPEMSG_SYSTEM_HEART, CPipeMessageHeart)

//=================================================================================================

class CPipeMessageOperator : public CPipeMessage
{
	DEFAULT_PIPEMSG_CREATOR(CPipeMessageOperator);

public:
	CPipeMessageOperator() : CPipeMessage(PIPEMSG_SYSTEM_OPERATOR)
	{

	}

public:
	PIPEMSG_PROP2(LPCWSTR, CString, Receiver)

protected:
	virtual HRESULT OnMarshalPipeMsg(CPipeStreamMarshal& buf)
	{
		HRESULT hr = E_FAIL;
		if(FAILED(hr = __super::OnMarshalPipeMsg(buf)))
			return hr;

		bool bRtn = true;
		bRtn = bRtn && buf.AppendTextW(GetProp_Receiver());
		return bRtn ? S_OK : E_FAIL;
	}

	virtual HRESULT OnUnmarshalPipeMsg(CPipeStreamMarshal& buf)
	{
		HRESULT hr = E_FAIL;
		if(FAILED(hr = __super::OnUnmarshalPipeMsg(buf)))
			return hr;

		bool bRtn = true;
		bRtn = bRtn && ParseStringW(buf, m_Receiver);
		return bRtn ? S_OK : E_FAIL;
	}
};

PIPEMSG_ENTRY_AUTO(PIPEMSG_SYSTEM_OPERATOR, CPipeMessageOperator)

//=================================================================================================
