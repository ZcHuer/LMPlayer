#include "stdafx.h"
#include "DataStream.h"

CDataStream::CDataStream(void) : m_dwMaxSize(0), m_dwValidSize(0), m_byzBuffer(NULL)
{

}

CDataStream::CDataStream(DWORD dwMaxSize) : m_dwMaxSize(0), m_dwValidSize(0), m_byzBuffer(NULL)
{
	HRESULT hr = Alloc(dwMaxSize);
	ATLASSERT(SUCCEEDED(hr));
}

CDataStream::CDataStream(LPBYTE lpbyBuffer, DWORD dwMaxSize, DWORD dwValidSize) 
		: m_dwMaxSize(dwMaxSize), m_dwValidSize(dwValidSize), m_byzBuffer(lpbyBuffer)
{
	ATLASSERT(FALSE);
}

CDataStream::CDataStream(const CDataStream& ref) : m_dwMaxSize(0), m_dwValidSize(0), m_byzBuffer(NULL)
{
	HRESULT hr = Alloc(ref.m_dwMaxSize);
	if(SUCCEEDED(hr))
	{
		m_dwValidSize = ref.m_dwValidSize;
		memcpy(m_byzBuffer, ref.m_byzBuffer, m_dwValidSize);
	}
}

CDataStream& CDataStream::operator=(const CDataStream& ref)
{
	if(this != &ref)
	{
		ATLASSERT(FALSE);
	}
	return *this;
}

CDataStream::~CDataStream(void)
{
	Free();
}

HRESULT CDataStream::Alloc(DWORD dwMaxSize)
{
	if(0 == dwMaxSize || dwMaxSize > MAX_STREAR_SIZE)
		return E_INVALIDARG;

	Free();
	m_dwMaxSize = (((dwMaxSize >> 2) + 1) << 2);//4字节对齐
	m_byzBuffer = new BYTE[m_dwMaxSize];
	if(NULL == m_byzBuffer)
	{
		m_dwMaxSize = 0;
		return E_FAIL;
	}
	
	memset(m_byzBuffer, 0, m_dwMaxSize);
	return S_OK;
}

HRESULT CDataStream::Realloc(DWORD dwMaxSize)
{
	CDataStream data;
	HRESULT hr = data.Alloc(dwMaxSize);
	if(FAILED(hr))
		return hr;

	if(GetValidSize() > 0)
	{
		if(dwMaxSize > GetValidSize())
		{
			memcpy(data.GetBuffer(0), GetBuffer(0), GetValidSize());
			data.SetValidSize(GetValidSize());
			hr = S_OK;
		}
		else
		{
			memcpy(data.GetBuffer(0), GetBuffer(0), dwMaxSize);
			data.SetValidSize(dwMaxSize);
			hr = S_FALSE;
		}
	}

	Free();
	Attach(data.Detach(), data.GetValidSize(), data.GetMaxSize());
	return hr;
}


HRESULT CDataStream::Free()
{
	if(m_byzBuffer)
		delete [] m_byzBuffer;

	m_byzBuffer = NULL;
	m_dwMaxSize = 0;
	m_dwValidSize = 0;
	return S_OK;
}

LPBYTE CDataStream::Attach(LPBYTE lpbyBuffer, DWORD dwSize, DWORD dwMaxSize)
{
	LPBYTE pbyRtn = Detach();
	m_byzBuffer = lpbyBuffer;
	m_dwMaxSize = dwMaxSize;
	m_dwValidSize = dwSize;
	return pbyRtn;
}

LPBYTE CDataStream::Detach()
{
	LPBYTE pbyRet = m_byzBuffer;
	m_byzBuffer = NULL;
	m_dwMaxSize = 0;
	m_dwValidSize = 0;
	return pbyRet;
}

LPBYTE CDataStream::GetBuffer(DWORD dwOffset)
{
	if (dwOffset > m_dwMaxSize)//Fix Bug: Json::Value.parse(GetBuffer(0)/*const char* beginDoc*, GetBuffer(MaxSize)/*const char* endDoc*/, ...)
		return NULL;

	return (LPBYTE)(m_byzBuffer + dwOffset);
}

DWORD CDataStream::GetValidSize()
{
	return m_dwValidSize;
}

void CDataStream::SetValidSize(DWORD dwSize)
{
	m_dwValidSize = dwSize;
	if(m_dwValidSize > m_dwMaxSize)
		m_dwValidSize = m_dwMaxSize;	
}

DWORD CDataStream::GetMaxSize()
{
	return m_dwMaxSize;
}

//=================================================================================================

bool CDataStreamEx::SetPointer(UINT uOffset, bool bEnd/* = false*/)
{
	if(bEnd)
	{
		if(CDataStream::GetMaxSize() < uOffset)
			return false;

		m_uPointer = CDataStream::GetMaxSize() - uOffset;
	}
	else
	{
		if(uOffset > CDataStream::GetMaxSize())
			return false;

		m_uPointer = uOffset;
	}
	return true;
}

UINT CDataStreamEx::GetPointer()
{
	return m_uPointer;
}

bool CDataStreamEx::AppendData(LPBYTE lpby, UINT uSize)
{
	ATLASSERT(lpby);
	ATLASSERT(uSize > 0);

	if(m_uPointer + uSize > CDataStream::GetMaxSize())
		return false;

	LPBYTE pbyBuffer = CDataStream::GetBuffer(0);
	LPBYTE pbyBufWrite = pbyBuffer + m_uPointer;
	memcpy(pbyBufWrite, lpby, uSize);

	m_uPointer += uSize;
	CDataStream::SetValidSize(m_uPointer);
	return true;
}

bool CDataStreamEx::GetData(LPBYTE lpby, UINT uSize)
{
	ATLASSERT(lpby);
	ATLASSERT(uSize > 0);

	if(m_uPointer + uSize > CDataStream::GetValidSize())
		return false;

	LPBYTE pbyBufRead = CDataStream::GetBuffer(m_uPointer);
	memcpy(lpby, pbyBufRead, uSize);

	m_uPointer += uSize;
	return true;
}

bool CDataStreamEx::AppendDataType(EDataType eType)
{
	ATLASSERT(m_uPointer + sizeof(STREAMDATATYPE) <= CDataStream::GetMaxSize());

	STREAMDATATYPE dt = (STREAMDATATYPE)eType;
	return AppendData((LPBYTE)&dt, sizeof(STREAMDATATYPE));
}

bool CDataStreamEx::GetDataType(EDataType& eType)
{
	ATLASSERT(m_uPointer + sizeof(STREAMDATATYPE) <= CDataStream::GetValidSize());
	STREAMDATATYPE dt = (STREAMDATATYPE)EDT_Unknwn;

	bool bRtn = GetData((LPBYTE)&dt, sizeof(STREAMDATATYPE));
	if(bRtn)
		eType = (EDataType)dt;

	return bRtn;
}

UINT CDataStreamEx::GetDataLengthByType(EDataType eType, UINT uSize/* = 0*/)
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
	return sizeof(STREAMDATATYPE)+uMinSize;
}

bool CDataStreamEx::IsMemValid(EDataType etype, UINT uSize/* = 0*/)
{
	UINT uMaxOffset = CDataStream::GetMaxSize();
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

bool CDataStreamEx::MakeBufferValid(UINT uSize/* = 0*/, BOOL bCopyBuffer/* = TRUE*/)
{
	UINT uSizeNew = 0;
	UINT uSizeCur = CDataStream::GetMaxSize()-m_uPointer;
	if(uSizeCur < uSize)
	{
		uSizeNew = uSize;
	}
	else
	{
		uSizeNew = uSizeCur + DEF_DATASTREAMBUFFER_SIZE;
	}
	uSizeNew += m_uPointer;

	if(bCopyBuffer)
	{
		LPBYTE pbyCur = CDataStream::Detach();
		HRESULT hr = CDataStream::Alloc(uSizeNew);
		if(FAILED(hr))
			return false;
		LPBYTE pbyNew = CDataStream::GetBuffer(0);
		if(pbyCur)
			memcpy(pbyNew, pbyCur, m_uPointer);
		delete [] pbyCur;
	}
	else
	{
		CDataStream::Alloc(uSizeNew);
	}
	return (CDataStream::GetMaxSize() >= uSizeNew);
}


bool CDataStreamEx::GetTextLength(UINT& nLength, bool bChangePointer/* = false*/)
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

bool CDataStreamEx::GetText(LPWSTR lpwsText, UINT& nLength)
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

bool CDataStreamEx::GetTextLengthA(UINT& nLength, bool bChangePointer/* = false*/)
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

bool CDataStreamEx::GetTextA(LPSTR lpsText, UINT& nLength)
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


bool CDataStreamEx::AppendTextW(LPCWSTR lpwsText)
{
	UINT uLength = NULL == lpwsText ? 0 : lstrlenW(lpwsText);
	if(!IsMemValid(EDT_TEXT_W, uLength))
	{
		UINT uNeedSize = GetDataLengthByType(EDT_TEXT_W, uLength);
		if(!MakeBufferValid(uNeedSize > DEF_DATASTREAMBUFFER_SIZE ? uNeedSize : 0))
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

bool CDataStreamEx::AppendTextA(LPCSTR lpsText)
{
	UINT uLength = NULL == lpsText ? 0 : lstrlenA(lpsText);
	if(!IsMemValid(EDT_TEXT_A, uLength))
	{
		UINT uNeedSize = GetDataLengthByType(EDT_TEXT_A, uLength);
		if(!MakeBufferValid(uNeedSize > DEF_DATASTREAMBUFFER_SIZE ? uNeedSize : 0))
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

bool CDataStreamEx::AppendByteArray(LPBYTE lpbyBuffer, UINT uSize)
{
	if(NULL == lpbyBuffer || uSize == 0)
		return false;

	if(!IsMemValid(EDT_BYTEARRAY, uSize))
	{
		UINT uNeedSize = GetDataLengthByType(EDT_BYTEARRAY, uSize);
		if(!MakeBufferValid(uNeedSize > DEF_DATASTREAMBUFFER_SIZE ? uNeedSize : 0))
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


bool CDataStreamEx::ParseStringW(CDataStreamEx& buf, CStringW& strW)
{
	UINT uTextLength = 0;
	if(!buf.GetTextLength(uTextLength))
		return FALSE;

	LPWSTR psBuf = strW.GetBufferSetLength(uTextLength);
	bool bRtn = buf.GetText(psBuf, uTextLength);
	strW.ReleaseBuffer();

	return bRtn;
}

bool CDataStreamEx::ParseStringA(CDataStreamEx& buf, CStringA& strA)
{
	UINT uTextLength = 0;
	if(!buf.GetTextLengthA(uTextLength))
		return false;

	LPSTR psBuf = strA.GetBufferSetLength(uTextLength);
	bool bRtn = buf.GetTextA(psBuf, uTextLength);
	strA.ReleaseBuffer();

	return bRtn;
}