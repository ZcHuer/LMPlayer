#include "stdafx.h"
#include "PipeStream.h"

CPipeStream::CPipeStream() : m_pbyzBuffer(NULL), m_uSize(0)
{

}

CPipeStream::~CPipeStream()
{
	DestroyStream();
}

UINT CPipeStream::GetStreamSize()
{
	return m_uSize;
}

LPBYTE CPipeStream::GetStream()
{
	return m_pbyzBuffer;
}

LPBYTE CPipeStream::CreateStream(UINT uSize)
{
	if(m_pbyzBuffer)
		delete [] m_pbyzBuffer;

	m_pbyzBuffer = NULL;
	m_uSize = 0;

	if(uSize > 0)
	{
		m_uSize = (((uSize + 0xF) >> 4) << 4); //按16字节对齐		
		m_pbyzBuffer = new BYTE[m_uSize];
		memset(m_pbyzBuffer, 0, m_uSize);

		m_uSize = uSize;
	}

	return m_pbyzBuffer;
}

void CPipeStream::DestroyStream()
{
	if(m_pbyzBuffer)
	{
		delete [] m_pbyzBuffer;
		m_pbyzBuffer = NULL;
	}
	m_uSize = 0;
}

LPBYTE CPipeStream::Attach(LPBYTE lpbyBuf, UINT uSize, BOOL bReleaseOld/* = TRUE*/)
{
	if(bReleaseOld)
	{
		if(m_pbyzBuffer)
		{
			delete [] m_pbyzBuffer;
			m_uSize = 0;
			m_pbyzBuffer = NULL;
		}
	}

	m_uSize = uSize;
	m_pbyzBuffer = lpbyBuf;

	return m_pbyzBuffer;
}

LPBYTE CPipeStream::Detach()
{
	LPBYTE lpby = m_pbyzBuffer;

	m_uSize = 0;
	m_pbyzBuffer = NULL;

	return lpby;
}

//=================================================================================================
CPipeStreamMarshal::CPipeStreamMarshal() : m_uPointer(0)
{

}

CPipeStreamMarshal::~CPipeStreamMarshal()
{

}

bool CPipeStreamMarshal::AppendTextW(LPCWSTR lpwsText)
{
	UINT uLength = NULL == lpwsText ? 0 : lstrlenW(lpwsText);
	if(!IsMemValid(EDT_TEXT_W, uLength))
	{
		UINT uNeedSize = GetDataLengthByType(EDT_TEXT_W, uLength);
		if(!MakeBufferValid(uNeedSize > DEF_PIPESTREAM_BUFFERSIZE ? uNeedSize : 0))
			return false;
	}

	ATLASSERT(IsMemValid(EDT_TEXT_W, uLength));

	UINT uPointer = GetPointer();

	UINT cbText = sizeof(WCHAR)*(uLength+1);//带结尾符

	if(AppendDataType(EDT_TEXT_W) 
		&& AppendData((LPBYTE)&cbText, sizeof(UINT))
		&& AppendData((LPBYTE)lpwsText, cbText))
	{
		return true;
	}

	m_uPointer = uPointer;
	return false;
}

bool CPipeStreamMarshal::AppendTextA(LPCSTR lpsText)
{
	UINT uLength = NULL == lpsText ? 0 : lstrlenA(lpsText);
	if(!IsMemValid(EDT_TEXT_A, uLength))
	{
		UINT uNeedSize = GetDataLengthByType(EDT_TEXT_A, uLength);
		if(!MakeBufferValid(uNeedSize > DEF_PIPESTREAM_BUFFERSIZE ? uNeedSize : 0))
			return false;
	}

	ATLASSERT(IsMemValid(EDT_TEXT_A, uLength));

	UINT uPointer = GetPointer();

	UINT cbText = sizeof(CHAR)*(uLength+1);//带结尾符

	if(AppendDataType(EDT_TEXT_A) 
		&& AppendData((LPBYTE)&cbText, sizeof(UINT))
		&& AppendData((LPBYTE)lpsText, cbText))
	{
		return true;
	}

	m_uPointer = uPointer;
	return false;
}

bool CPipeStreamMarshal::AppendByteArray(LPBYTE lpbyBuffer, UINT uSize)
{
	if(NULL == lpbyBuffer || uSize == 0)
		return false;

	if(!IsMemValid(EDT_BYTEARRAY, uSize))
	{
		UINT uNeedSize = GetDataLengthByType(EDT_BYTEARRAY, uSize);
		if(!MakeBufferValid(uNeedSize > DEF_PIPESTREAM_BUFFERSIZE ? uNeedSize : 0))
			return false;
	}

	ATLASSERT(IsMemValid(EDT_BYTEARRAY, uSize));

	UINT uPointer = GetPointer();

	if(AppendDataType(EDT_BYTEARRAY) 
		&& AppendData((LPBYTE)&uSize, sizeof(UINT))
		&& AppendData((LPBYTE)lpbyBuffer, uSize))
	{
		return true;
	}

	m_uPointer = uPointer;
	return false;
}

//=================================================================================================

bool CPipeStreamMarshal::GetTextLength(UINT& nLength, bool bChangePointer/* = false*/)
{
	if(!IsMemValid(EDT_TEXT_W))
		return false;

	UINT uPointer = GetPointer();

	EDataType eType = EDT_Unknwn;
	if(!GetDataType(eType))
	{
		m_uPointer = uPointer;
		return false;
	}

	if(eType != EDT_TEXT_W)
	{
		m_uPointer = uPointer;
		return false;
	}

	UINT uLength = 0;
	if(!GetData((LPBYTE)&uLength, sizeof(UINT)))
	{
		m_uPointer = uPointer;
		return false;
	}

	if(!bChangePointer)
	{
		//特殊处理,将pointer设置回原始位置,供GetText 使用
		m_uPointer = uPointer;
	}

	nLength = uLength/sizeof(WCHAR);
	return true;
}

bool CPipeStreamMarshal::GetText(LPWSTR lpwsText, UINT& nLength)
{
	if(NULL == lpwsText)
		return false;

	UINT uLength = 0;
	if(!GetTextLength(uLength, true))
		return false;

	uLength *= sizeof(WCHAR);
	if(uLength > nLength*sizeof(WCHAR))
		return false;

	UINT uPointer = GetPointer();
	if(!GetData((LPBYTE)lpwsText, uLength))
	{
		m_uPointer = uPointer;
		return false;
	}

	return true;
}

bool CPipeStreamMarshal::GetTextLengthA(UINT& nLength, bool bChangePointer/* = false*/)
{
	if(!IsMemValid(EDT_TEXT_A))
		return false;

	UINT uPointer = GetPointer();

	EDataType eType = EDT_Unknwn;
	if(!GetDataType(eType))
	{
		m_uPointer = uPointer;
		return false;
	}

	if(eType != EDT_TEXT_A)
	{
		m_uPointer = uPointer;
		return false;
	}

	UINT uLength = 0;
	if(!GetData((LPBYTE)&uLength, sizeof(UINT)))
	{
		m_uPointer = uPointer;
		return false;
	}

	if(!bChangePointer)
	{
		//特殊处理,将pointer设置回原始位置,供GetText 使用
		m_uPointer = uPointer;
	}

	nLength = uLength/sizeof(CHAR);
	return true;
}

bool CPipeStreamMarshal::GetTextA(LPSTR lpsText, UINT& nLength)
{
	if(NULL == lpsText)
		return false;

	UINT uLength = 0;
	if(!GetTextLengthA(uLength, true))
		return false;

	uLength *= sizeof(CHAR);
	if(uLength > nLength*sizeof(CHAR))
		return false;

	UINT uPointer = GetPointer();
	if(!GetData((LPBYTE)lpsText, uLength))
	{
		m_uPointer = uPointer;
		return false;
	}

	return true;
}

bool CPipeStreamMarshal::GetByteLength(UINT& nLength, bool bChangePointer/* = false*/)
{
	if(!IsMemValid(EDT_BYTEARRAY))
		return false;

	UINT uPointer = GetPointer();

	EDataType eType = EDT_Unknwn;
	if(!GetDataType(eType))
	{
		m_uPointer = uPointer;
		return false;
	}

	if(eType != EDT_BYTEARRAY)
	{
		m_uPointer = uPointer;
		return false;
	}

	UINT uLength = 0;
	if(!GetData((LPBYTE)&uLength, sizeof(UINT)))
	{
		m_uPointer = uPointer;
		return false;
	}

	if(!bChangePointer)
	{
		//特殊处理,将pointer设置回原始位置,供GetByteArray 使用
		m_uPointer = uPointer;
	}

	nLength = uLength;
	return true;
}

bool CPipeStreamMarshal::GetByteArray(LPBYTE lpbyBuffer, UINT nLength)
{
	if(NULL == lpbyBuffer)
		return false;

	UINT uLength = 0;
	if(!GetByteLength(uLength, true))
		return false;

	uLength *= sizeof(BYTE);
	if(uLength > nLength)
		return false;

	UINT uPointer = GetPointer();
	if(!GetData((LPBYTE)lpbyBuffer, uLength))
	{
		m_uPointer = uPointer;
		return false;
	}

	return true;
}

//=================================================================================================

bool CPipeStreamMarshal::SetPointer(UINT uOffset, bool bEnd/* = false*/)
{
	if(bEnd)
	{
		if(CPipeStream::GetStreamSize() < uOffset)
			return false;

		m_uPointer = CPipeStream::GetStreamSize() - uOffset;
	}
	else
	{
		if(uOffset > CPipeStream::GetStreamSize())
			return false;

		m_uPointer = uOffset;
	}
	return true;
}

UINT CPipeStreamMarshal::GetPointer()
{
	return m_uPointer;
}

bool CPipeStreamMarshal::AppendData(LPBYTE lpby, UINT uSize)
{
	ATLASSERT(lpby);
	ATLASSERT(uSize > 0);

	if(m_uPointer + uSize > CPipeStream::GetStreamSize())
		return false;

	LPBYTE pbyBuffer = CPipeStream::GetStream();
	LPBYTE pbyBufWrite = pbyBuffer + m_uPointer;
	memcpy(pbyBufWrite, lpby, uSize);

	m_uPointer += uSize;
	return true;
}

bool CPipeStreamMarshal::GetData(LPBYTE lpby, UINT uSize)
{
	ATLASSERT(lpby);
	ATLASSERT(uSize > 0);

	if(m_uPointer + uSize > CPipeStream::GetStreamSize())
		return false;

	LPBYTE pbyBufRead = CPipeStream::GetStream() + m_uPointer;
	memcpy(lpby, pbyBufRead, uSize);

	m_uPointer += uSize;
	return true;
}

bool CPipeStreamMarshal::AppendDataType(EDataType eType)
{
	ATLASSERT(m_uPointer + sizeof(PIPE_DATATYPE) <= CPipeStream::GetStreamSize());

	PIPE_DATATYPE dt = (PIPE_DATATYPE)eType;
	return AppendData((LPBYTE)&dt, sizeof(PIPE_DATATYPE));
}

bool CPipeStreamMarshal::GetDataType(EDataType& eType)
{
	ATLASSERT(m_uPointer + sizeof(PIPE_DATATYPE) <= CPipeStream::GetStreamSize());
	PIPE_DATATYPE dt = (PIPE_DATATYPE)EDT_Unknwn;

	bool bRtn = GetData((LPBYTE)&dt, sizeof(PIPE_DATATYPE));
	if(bRtn)
		eType = (EDataType)dt;

	return bRtn;
}

UINT CPipeStreamMarshal::GetDataLengthByType(EDataType eType, UINT uSize/* = 0*/)
{
	UINT uMinSize = 0;
	switch (eType)
	{
	case EDT_BYTE:
		{
			uMinSize = sizeof(BYTE);
			break;
		}
	case EDT_CHAR:
		{
			uMinSize = sizeof(CHAR);
		}
	case EDT_USHORT:
		{
			uMinSize = sizeof(USHORT);
			break;
		}
	case EDT_SHORT:
		{
			uMinSize = sizeof(SHORT);
			break;
		}
	case EDT_UINT:
		{
			uMinSize = sizeof(UINT);
			break;
		}
	case EDT_INT:
		{
			uMinSize = sizeof(INT);
			break;
		}
	case EDT_ULONG:
		{
			uMinSize = sizeof(ULONG);
			break;
		}
	case EDT_LONG:
		{
			uMinSize = sizeof(LONG);
			break;
		}
	case EDT_UINT64:
		{
			uMinSize = sizeof(ULONGLONG);
			break;
		}
	case EDT_INT64:
		{
			uMinSize = sizeof(LONGLONG);
			break;
		}
	case EDT_TEXT_W:
		{
			uMinSize = sizeof(WCHAR)*(1+uSize) + sizeof(UINT);//计算结尾符,长度
			break;
		}
	case EDT_TEXT_A:
		{
			uMinSize = sizeof(CHAR)*(1+uSize) + sizeof(UINT);//计算结尾符,长度
			break;
		}
	case EDT_BYTEARRAY:
		{
			uMinSize = sizeof(BYTE)*uSize + sizeof(UINT);//长度
			break;
		}
	default:
		{
			ATLASSERT(FALSE);
			break;
		}
	}
	return sizeof(PIPE_DATATYPE)+uMinSize;
}

bool CPipeStreamMarshal::IsMemValid(EDataType etype, UINT uSize/* = 0*/)
{
	UINT uMaxOffset = CPipeStream::GetStreamSize();
	if(m_uPointer >= uMaxOffset)
	{
		//ATLASSERT(FALSE);
		return false;
	}

	UINT uDataSize = GetDataLengthByType(etype, uSize);
	if(uDataSize > (uMaxOffset - m_uPointer))
		return false;

	return true;
}

bool CPipeStreamMarshal::MakeBufferValid(UINT uSize/* = 0*/, BOOL bCopyBuffer/* = TRUE*/)
{
	UINT uSizeNew = 0;
	UINT uSizeCur = CPipeStream::GetStreamSize()-m_uPointer;
	if(uSizeCur < uSize)
	{
		uSizeNew = uSize;
	}
	else
	{
		uSizeNew = uSizeCur + DEF_PIPESTREAM_BUFFERSIZE;
	}
	uSizeNew += m_uPointer;

	if(bCopyBuffer)
	{
		LPBYTE pbyCur = CPipeStream::Detach();
		LPBYTE pbyNew = CPipeStream::CreateStream(uSizeNew);
		if(pbyCur)
			memcpy(pbyNew, pbyCur, m_uPointer);
		delete [] pbyCur;
	}
	else
	{
		CPipeStream::CreateStream(uSizeNew);
	}
	return (CPipeStream::GetStreamSize() == uSizeNew);
}

bool ParseStringW(CPipeStreamMarshal& buf, CStringW& strW)
{
	UINT uTextLength = 0;
	if(!buf.GetTextLength(uTextLength))
		return FALSE;

	LPWSTR psBuf = strW.GetBufferSetLength(uTextLength);
	bool bRtn = buf.GetText(psBuf, uTextLength);
	strW.ReleaseBuffer();

	return bRtn;
}

bool ParseStringA(CPipeStreamMarshal& buf, CStringA& strA)
{
	UINT uTextLength = 0;
	if(!buf.GetTextLengthA(uTextLength))
		return false;

	LPSTR psBuf = strA.GetBufferSetLength(uTextLength);
	bool bRtn = buf.GetTextA(psBuf, uTextLength);
	strA.ReleaseBuffer();

	return bRtn;
}