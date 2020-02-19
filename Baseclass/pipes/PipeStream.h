#pragma once
#include "PipeDef.h"

#define DEF_PIPESTREAM_BUFFERSIZE			1024

class CPipeStream
{
public:
	CPipeStream();
	virtual ~CPipeStream();

public:
	UINT GetStreamSize();
	LPBYTE GetStream();
	LPBYTE CreateStream(UINT uSize);
	void   DestroyStream();

	LPBYTE Attach(LPBYTE lpbyBuf, UINT uSize, BOOL bReleaseOld = TRUE);
	LPBYTE Detach();

private:
	UINT m_uSize;
	LPBYTE m_pbyzBuffer;
};

enum EDataType
{
	EDT_Unknwn = 0,
	EDT_BYTE = 1,
	EDT_CHAR,
	EDT_USHORT,
	EDT_SHORT,
	EDT_UINT,
	EDT_INT,
	EDT_ULONG,
	EDT_LONG,
	EDT_UINT64,
	EDT_INT64,
	EDT_TEXT_W,
	EDT_TEXT_A,
	EDT_BYTEARRAY,
};

typedef WORD			PIPE_DATATYPE;

class CPipeStreamMarshal : public CPipeStream
{
public:
	CPipeStreamMarshal();
	virtual ~CPipeStreamMarshal();

public:
	bool AppendBYTE(BYTE value)			{return AppendT<BYTE,		EDT_BYTE>	(value);}
	bool AppendCHAR(CHAR value)			{return AppendT<CHAR,		EDT_CHAR>	(value);}	
	bool AppendUSHORT(USHORT value)		{return AppendT<USHORT,		EDT_USHORT>	(value);}
	bool AppendSHORT(SHORT value)		{return AppendT<SHORT,		EDT_SHORT>	(value);}
	bool AppendUINT(UINT value)			{return AppendT<UINT,		EDT_UINT>	(value);}
	bool AppendINT(INT value)			{return AppendT<INT,		EDT_INT>	(value);}
	bool AppendULONG(ULONG value)		{return AppendT<ULONG,		EDT_ULONG>	(value);}
	bool AppendLONG(LONG value)			{return AppendT<LONG,		EDT_LONG>	(value);}
	bool AppendUINT64(ULONGLONG value)	{return AppendT<ULONGLONG,	EDT_UINT64>	(value);}
	bool AppendINT64(LONGLONG value)	{return AppendT<LONGLONG,	EDT_INT64>	(value);}
	bool AppendTextW(LPCWSTR lpwsText);
	bool AppendTextA(LPCSTR lpsText);
	bool AppendByteArray(LPBYTE lpbyBuffer, UINT uSize);

public:
	bool GetBYTE(BYTE& value)			{return GetValueT<BYTE,		EDT_BYTE>	(value);}
	bool GetCHAR(CHAR& value)			{return GetValueT<CHAR,		EDT_CHAR>	(value);}
	bool GetUSHORT(USHORT& value)		{return GetValueT<USHORT,	EDT_USHORT>	(value);}
	bool GetSHORT(SHORT& value)			{return GetValueT<SHORT,	EDT_SHORT>	(value);}
	bool GetUINT(UINT& value)			{return GetValueT<UINT,		EDT_UINT>	(value);}
	bool GetINT(INT& value)				{return GetValueT<INT,		EDT_INT>	(value);}
	bool GetULONG(ULONG& value)			{return GetValueT<ULONG,	EDT_ULONG>	(value);}
	bool GetLONG(LONG& value)			{return GetValueT<LONG,		EDT_LONG>	(value);}
	bool GetINT64(LONGLONG& value)		{return GetValueT<LONGLONG, EDT_INT64>	(value);}
	bool GetUINT64(ULONGLONG& value)	{return GetValueT<ULONGLONG,EDT_UINT64>	(value);}
	bool GetTextLength(UINT& nLength, bool bChangePointer = false);
	bool GetText(LPWSTR lpwsText, UINT& nLength);
	bool GetTextLengthA(UINT& nLength, bool bChangePointer = false);
	bool GetTextA(LPSTR lpsText, UINT& nLength);
	bool GetByteLength(UINT& nLength, bool bChangePointer = false);
	bool GetByteArray(LPBYTE lpbyBuffer, UINT uLength);

public:
	bool SetPointer(UINT uOffset, bool bEnd = false);
	UINT GetPointer();

private:
	bool AppendData(LPBYTE lpby, UINT uSize);
	bool GetData(LPBYTE lpby, UINT uSize);

	bool AppendDataType(EDataType eType);
	bool GetDataType(EDataType& eType);
	UINT GetDataLengthByType(EDataType eType, UINT uSize = 0);

	bool IsMemValid(EDataType etype, UINT uSize = 0);
	bool MakeBufferValid(UINT uSize = 0, BOOL bCopyBuffer = TRUE);

private:
	template<class T, EDataType t>
	inline bool AppendT(T value)
	{
		if(!IsMemValid(t))
		{
			if(!MakeBufferValid())
				return false;
		}
		ATLASSERT(IsMemValid(t));
		UINT uPointer = GetPointer();
		if(!AppendDataType(t))
		{
			m_uPointer = uPointer;
			return false;
		}
		if(!AppendData((LPBYTE)&value, sizeof(T)))
		{
			m_uPointer = uPointer;
			return false;
		}
		return true;
	}

	template<class T, EDataType t>
	inline bool GetValueT(T& value)
	{
		if(!IsMemValid(t))
			return false;

		UINT uPointer = GetPointer();
		EDataType eType = EDT_Unknwn;
		if(!GetDataType(eType))
		{
			m_uPointer = uPointer;
			return false;
		}
		if(eType != t)
		{
			m_uPointer = uPointer;
			return false;
		}
		if(!GetData((LPBYTE)&value, sizeof(T)))
		{
			m_uPointer = uPointer;
			return false;
		}
		return true;
	}

private:
	UINT m_uPointer;
};

bool ParseStringW(CPipeStreamMarshal& buf, CStringW& strW);
bool ParseStringA(CPipeStreamMarshal& buf, CStringA& strA);