#include "stdafx.h"
#include "AESDecoder.h"

CAESDecoder::CAESDecoder(void) : m_bAllocBuf(TRUE)
{

}

CAESDecoder::~CAESDecoder(void)
{

}

LPBYTE CAESDecoder::_MAlloc(DWORD dwSize)
{
	dwSize += 4;
	dwSize = ((dwSize >> 2) << 2);

	LPBYTE pbyRtn = new BYTE[dwSize];
	if(pbyRtn)
		memset(pbyRtn, 0, dwSize);
	return pbyRtn;
}

VOID CAESDecoder::_MFree(LPBYTE lpbyBuf)
{
	if(lpbyBuf)
		delete [] lpbyBuf;
}

//=================================================================================================

BOOL CAESDecoder::AESDecode(LPBYTE lpbyInput, DWORD dwInputLength, LPCSTR lpsKey, LPBYTE* lppOutput, DWORD* lpdwOutputLength)
{
	if(NULL == lpbyInput || dwInputLength == 0
		|| NULL == lpsKey 
		|| NULL == lppOutput || NULL == lpdwOutputLength)
		return FALSE;

	LPBYTE pbyFixedKey = NULL;//need _MFree()
	DWORD dwFixedKeySize = 0;
	if(!FixKey(lpsKey, &pbyFixedKey, &dwFixedKeySize))
		return FALSE;

	LPBYTE pbyDecBuf = NULL;
	DWORD  dwDecBufSize = 0;
	if(IsBlockMode())
	{
		DWORD dwBlockSize = GetBlockSize();
		ATLASSERT(dwBlockSize);
		dwDecBufSize = (((dwInputLength - 1) / dwBlockSize) + 1) * dwBlockSize;
	}
	else
	{
		dwDecBufSize = dwInputLength;
	}
		
	if(m_bAllocBuf)
	{
		pbyDecBuf = _MAlloc(dwDecBufSize);//need _MFree()
	}
	else
	{
		pbyDecBuf = *lppOutput;
		if(*lpdwOutputLength < dwDecBufSize)
			return FALSE;
	}

	if(NULL == pbyDecBuf)
	{
		_MFree(pbyFixedKey);
		return FALSE;
	}

	memcpy(pbyDecBuf, lpbyInput, dwInputLength);

	InitDecInstance(pbyFixedKey, dwFixedKeySize);

	Decode_ECB(pbyDecBuf, dwDecBufSize);

	*lppOutput = pbyDecBuf;
	*lpdwOutputLength = dwDecBufSize;
	
	_MFree(pbyFixedKey);

	return TRUE;
}

BOOL CAESDecoder::AESEncode(LPBYTE lpbyInput, DWORD dwInputLength, LPCSTR lpsKey, LPBYTE* lppOutput, DWORD* lpdwOutputLength)
{
	if(NULL == lpbyInput || dwInputLength == 0
		|| NULL == lpsKey 
		|| NULL == lppOutput || NULL == lpdwOutputLength)
		return FALSE;

	LPBYTE pbyFixedKey = NULL;//need _MFree()
	DWORD dwFixedKeySize = 0;
	if(!FixKey(lpsKey, &pbyFixedKey, &dwFixedKeySize))
		return FALSE;

	LPBYTE pbyDecBuf = NULL;
	DWORD  dwDecBufSize = 0;
	if(IsBlockMode())
	{
		DWORD dwBlockSize = GetBlockSize();
		ATLASSERT(dwBlockSize);
		dwDecBufSize = (((dwInputLength - 1) / dwBlockSize) + 1) * dwBlockSize;
	}
	else
	{
		dwDecBufSize = dwInputLength;
	}

	if(m_bAllocBuf)
	{
		pbyDecBuf = _MAlloc(dwDecBufSize);//need _MFree()
	}
	else
	{
		pbyDecBuf = *lppOutput;
		if(*lpdwOutputLength < dwDecBufSize)
			return FALSE;
	}

	if(NULL == pbyDecBuf)
	{
		_MFree(pbyFixedKey);
		return FALSE;
	}

	memcpy(pbyDecBuf, lpbyInput, dwInputLength);

	InitDecInstance(pbyFixedKey, dwFixedKeySize);

	Encode_ECB(pbyDecBuf, dwDecBufSize);

	*lppOutput = pbyDecBuf;
	*lpdwOutputLength = dwDecBufSize;

	_MFree(pbyFixedKey);

	return TRUE;
}

//=================================================================================================

DWORD CAESDecoder::GetMaxKeyLength()
{
	return _AES_MAX_KEYLENGTH;
}

DWORD CAESDecoder::GetSupportKeyLength(DWORD* lpdwSupportLengthArray, DWORD dwArraySize)
{
	if(NULL == lpdwSupportLengthArray)
		return _AES_SUPPORT_KEYLENGTH_COUNT;

	DWORD dwRtn = 0;
	for(; dwRtn < dwArraySize && dwRtn < _AES_SUPPORT_KEYLENGTH_COUNT; ++dwRtn)
	{
		switch(dwRtn)
		{
		case 0:
			lpdwSupportLengthArray[dwRtn] = _AES_SUPPORT_KEYLENGTH_16;
			break;
		case 1:
			lpdwSupportLengthArray[dwRtn] = _AES_SUPPORT_KEYLENGTH_24;
			break;
		case 2:
			lpdwSupportLengthArray[dwRtn] = _AES_SUPPORT_KEYLENGTH_32;
			break;
		case _AES_SUPPORT_KEYLENGTH_COUNT:
		default:
			ATLASSERT(FALSE);
		}
	}
	return dwRtn;
}

DWORD CAESDecoder::FixKey(LPCSTR lpsKey, LPBYTE* lppFixedKey, DWORD* lpdwFixedKeyLength)
{
	ATLASSERT(lpsKey);
	DWORD dwKeyLength = lstrlenA(lpsKey);

	LPBYTE pbyFixedKey = NULL;
	DWORD dwFixedKeyLen = 0;

	DWORD dwMaxKeyLen = GetMaxKeyLength();
	if(dwKeyLength > dwMaxKeyLen)
		dwKeyLength = dwMaxKeyLen;
	
	DWORD dwSupportCount = GetSupportKeyLength(NULL, 0);
	DWORD* pSuppKeyLenArray = new DWORD[dwSupportCount];
	dwSupportCount = GetSupportKeyLength(pSuppKeyLenArray, dwSupportCount);
	if(dwSupportCount == 0)
	{
		ATLASSERT(FALSE);
		dwFixedKeyLen = dwKeyLength;
	}
	else if(dwSupportCount == 1)
	{
		ATLASSERT(FALSE);
		dwFixedKeyLen = pSuppKeyLenArray[0];
	}
	else 
	{
		dwFixedKeyLen = dwMaxKeyLen;
		for(DWORD i = 0; i < dwSupportCount; ++i)
		{
			if(pSuppKeyLenArray[i] >= dwKeyLength
				&& pSuppKeyLenArray[i] < dwFixedKeyLen)
			{
				dwFixedKeyLen = pSuppKeyLenArray[i];
			}
		}
	}
	delete [] pSuppKeyLenArray;

	if(dwFixedKeyLen == 0)
	{
		ATLASSERT(FALSE);
		dwFixedKeyLen = dwMaxKeyLen;
	}

	pbyFixedKey = _MAlloc(dwFixedKeyLen);
	if(NULL == pbyFixedKey)
		return FALSE;

	memcpy(pbyFixedKey, lpsKey, dwKeyLength);

	*lppFixedKey = pbyFixedKey;
	*lpdwFixedKeyLength = dwFixedKeyLen;
	return TRUE;
}

BOOL CAESDecoder::IsBlockMode()
{
	return TRUE;
}

DWORD CAESDecoder::GetBlockSize()
{
	return _AES_BLOCK_SIZE;
}

BOOL CAESDecoder::Decode_ECB(LPBYTE lpbyInOut, DWORD& dwInOutLength)
{
	ATLASSERT(lpbyInOut);
	ATLASSERT(dwInOutLength > 0);

	DWORD dwBlockSize = GetBlockSize();
	LPBYTE pWork = lpbyInOut;
	
	DWORD dwBlockCount = dwInOutLength/dwBlockSize;
	for(DWORD i = 0; i < dwBlockCount; ++i)
	{
		_mcrypt_rijndael_decrypt(&m_riDecInstance, (word8*)pWork);
		pWork += dwBlockSize;
	}

	return TRUE;
}

BOOL CAESDecoder::Encode_ECB(LPBYTE lpbyInOut, DWORD& dwInOutLength)
{
	ATLASSERT(lpbyInOut);
	ATLASSERT(dwInOutLength > 0);

	DWORD dwBlockSize = GetBlockSize();
	LPBYTE pWork = lpbyInOut;

	DWORD dwBlockCount = dwInOutLength/dwBlockSize;
	for(DWORD i = 0; i < dwBlockCount; ++i)
	{
		_mcrypt_rijndael_encrypt(&m_riDecInstance, (word8*)pWork);
		pWork += dwBlockSize;
	}

	return TRUE;
}

BOOL CAESDecoder::InitDecInstance(LPBYTE lpbyKey, DWORD dwKeyLength)
{
	_mcrypt_rijndael_gentables();
	_mcrypt_rijndael_set_key(&m_riDecInstance, 
								_AES_CRYPT_BITS,
								dwKeyLength / 4,
								lpbyKey);
	return TRUE;
}

BOOL CAESDecoder::AESDecodeFile(HANDLE hFileSrc, HANDLE hFileDec, LPCSTR lpsKey)
{
	if(INVALID_HANDLE_VALUE == hFileSrc || INVALID_HANDLE_VALUE == hFileDec)
		return FALSE;

	LARGE_INTEGER fileSize;
	if(!GetFileSizeEx(hFileSrc, &fileSize) || fileSize.QuadPart <= 0)
		return FALSE;

	SetFilePointer(hFileSrc, 0, NULL, FILE_BEGIN);
	SetFilePointer(hFileDec, 0, NULL, FILE_BEGIN);
	SetEndOfFile(hFileDec);

	LONGLONG uPart = (fileSize.QuadPart + _AES_FILE_READBUFFER_SIZE - 1) / _AES_FILE_READBUFFER_SIZE;
	if(uPart <= 0)
		return FALSE;

	LPBYTE pbyzBuf = new BYTE[_AES_FILE_READBUFFER_SIZE];
	if(NULL == pbyzBuf)
		return FALSE;

	LPBYTE pbyzDecData = new BYTE[_AES_FILE_CRYPTBUFFER_SIZE];
	if(NULL == pbyzDecData)
	{
		delete [] pbyzBuf;
		return FALSE;
	}

	BOOL bRtn = TRUE;
	m_bAllocBuf = FALSE;
	for(LONGLONG i = 0; i < uPart; ++i)
	{
		DWORD dwReaded = 0;
		if(!ReadFile(hFileSrc, pbyzBuf, _AES_FILE_READBUFFER_SIZE, &dwReaded, NULL))
		{
			bRtn = FALSE;
			break;
		}

		DWORD dwDecDataLength = _AES_FILE_CRYPTBUFFER_SIZE;
		if(!AESDecode(pbyzBuf, dwReaded, lpsKey, &pbyzDecData, &dwDecDataLength))
		{
			bRtn = FALSE;
			break;
		}

		DWORD dwWrited = 0;
		if(!WriteFile(hFileDec, pbyzDecData, dwDecDataLength, &dwWrited, NULL))
		{
			bRtn = FALSE;
			break;
		}
	}
	m_bAllocBuf = TRUE;
	delete [] pbyzBuf;
	delete [] pbyzDecData;

	return bRtn;
}

BOOL CAESDecoder::AESEncodeFile(HANDLE hFileSrc, HANDLE hFileEnc, LPCSTR lpsKey)
{
	if(INVALID_HANDLE_VALUE == hFileSrc || INVALID_HANDLE_VALUE == hFileEnc)
		return FALSE;

	LARGE_INTEGER fileSize;
	if(!GetFileSizeEx(hFileSrc, &fileSize) || fileSize.QuadPart <= 0)
		return FALSE;

	SetFilePointer(hFileSrc, 0, NULL, FILE_BEGIN);
	SetFilePointer(hFileEnc, 0, NULL, FILE_BEGIN);
	SetEndOfFile(hFileEnc);

	LONGLONG uPart = (fileSize.QuadPart + _AES_FILE_READBUFFER_SIZE - 1) / _AES_FILE_READBUFFER_SIZE;
	if(uPart <= 0)
		return FALSE;

	LPBYTE pbyzBuf = new BYTE[_AES_FILE_READBUFFER_SIZE];
	if(NULL == pbyzBuf)
		return FALSE;

	LPBYTE pbyzDecData = new BYTE[_AES_FILE_CRYPTBUFFER_SIZE];
	if(NULL == pbyzDecData)
	{
		delete [] pbyzBuf;
		return FALSE;
	}

	BOOL bRtn = TRUE;
	m_bAllocBuf = FALSE;
	for(LONGLONG i = 0; i < uPart; ++i)
	{
		DWORD dwReaded = 0;
		if(!ReadFile(hFileSrc, pbyzBuf, _AES_FILE_READBUFFER_SIZE, &dwReaded, NULL))
		{
			bRtn = FALSE;
			break;
		}

		DWORD dwDecDataLength = _AES_FILE_CRYPTBUFFER_SIZE;
		if(!AESEncode(pbyzBuf, dwReaded, lpsKey, &pbyzDecData, &dwDecDataLength))
		{
			bRtn = FALSE;
			break;
		}

		DWORD dwWrited = 0;
		if(!WriteFile(hFileEnc, pbyzDecData, dwDecDataLength, &dwWrited, NULL))
		{
			bRtn = FALSE;
			break;
		}
	}
	m_bAllocBuf = TRUE;
	delete [] pbyzBuf;
	delete [] pbyzDecData;

	return bRtn;
}