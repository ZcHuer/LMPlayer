#include "stdafx.h"
#include "CnvEncoder.h"
#include<vector>


#define DEFAULT_CNV_SIZE		256

CnvEncoder::CnvEncoder()
	: wcsBuf(NULL)
	, mbBuf(NULL)
	, wcsSize(0)
	, mbSize(0)
	, pfnEncode(NULL)
{
}

CnvEncoder::CnvEncoder(int nCharset)
	: wcsBuf(NULL)
	, mbBuf(NULL)
	, wcsSize(0)
	, mbSize(0)
{
	SetCharset(nCharset);
}

void CnvEncoder::SetCharset(int nCharset)
{
	if( nCharset == utf8 )
		pfnEncode = &CnvEncoder::UTF82UTF8;
	else if( nCharset == gb2312 || nCharset == gbk )
		pfnEncode = &CnvEncoder::GB2UTF8;
}

void CnvEncoder::__wcsalloc(int nBytes)
{
	if( nBytes < wcsSize ) return;
	if( wcsBuf == NULL ) {
		wcsSize = (nBytes > DEFAULT_CNV_SIZE ? nBytes : DEFAULT_CNV_SIZE);
		wcsBuf = (wchar_t*)malloc(wcsSize * sizeof(wchar_t));
	}
	else {
		do {
			wcsSize <<= 1;
		} while( wcsSize < nBytes );
		wcsBuf = (wchar_t*)realloc(wcsBuf, wcsSize * sizeof(wchar_t));
	}
}

void CnvEncoder::__mballoc(int nBytes)
{
	if( nBytes < mbSize ) return;
	if( mbBuf == NULL ) {
		mbSize = (nBytes > DEFAULT_CNV_SIZE ? nBytes : DEFAULT_CNV_SIZE);
		mbBuf = (char*)malloc(mbSize);
	}
	else {
		do {
			mbSize <<= 1;
		} while( mbSize < nBytes );
		mbBuf = (char*)realloc(mbBuf, mbSize);
	}
}

const wchar_t* CnvEncoder::Ansi2Unicode(const char* szAnsi)
{
	int wcsLen = ::MultiByteToWideChar(CP_ACP, NULL, szAnsi, strlen(szAnsi), NULL, 0);
	__wcsalloc(wcsLen + 1);
	::MultiByteToWideChar(CP_ACP, NULL, szAnsi, strlen(szAnsi), wcsBuf, wcsLen);
	wcsBuf[wcsLen] = 0;
	return wcsBuf;
}

const wchar_t* CnvEncoder::UTF82Unicode(const char* szU8, int len/*= -1*/)
{
	int wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, szU8, len, NULL, 0);
	__wcsalloc(wcsLen + 1);
	::MultiByteToWideChar(CP_UTF8, NULL, szU8, len, wcsBuf, wcsLen);
	wcsBuf[wcsLen] = 0;
	return wcsBuf;
}

wstring CnvEncoder::ConvertUTF82Unicode(const string& sU8)
{
	int wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, sU8.c_str(), -1, NULL, 0);
	vector<TCHAR> wcsBuf(wcsLen+1);
	::MultiByteToWideChar(CP_UTF8, NULL, sU8.c_str(), -1, &wcsBuf[0], wcsLen);
	return wcsBuf.data();
}

const char* CnvEncoder::UTF82UTF8(const char* szU8, int& cbSize)
{
	if( cbSize == -1 )
		cbSize = (int)strlen(szU8);
	return szU8;
}

const char* CnvEncoder::GB2UTF8(const char* szGB, int& cbSize)
{
	// to unicode
	int wcsLen = ::MultiByteToWideChar(CP_ACP, NULL, szGB, cbSize, NULL, 0);
	__wcsalloc(wcsLen);
	::MultiByteToWideChar(CP_ACP, 0, szGB, cbSize, wcsBuf, wcsLen);
	// to utf8
	int gbkLen = ::WideCharToMultiByte(CP_UTF8, NULL, wcsBuf, wcsLen, NULL, 0, NULL, NULL);
	__mballoc(gbkLen);
	::WideCharToMultiByte (CP_UTF8, 0, wcsBuf, wcsLen, mbBuf, gbkLen, NULL, NULL);
	cbSize = gbkLen;
	return mbBuf;
}

string CnvEncoder::Unicode2Utf8(LPCWSTR wszString)
{
	string sOut;
	int u8Len = ::WideCharToMultiByte(CP_UTF8, NULL, wszString, wcslen(wszString), NULL, 0, NULL, NULL);
	char* szU8 = new char[u8Len + 1];
	::WideCharToMultiByte(CP_UTF8, NULL, wszString, wcslen(wszString), szU8, u8Len, NULL, NULL);
	szU8[u8Len] = 0;
	sOut = szU8;
	delete []szU8;
	return sOut;
}

string CnvEncoder::Unicode2GB2312(LPCWSTR wszUnicodeString)
{
	string sOut;
	const UINT nCodePage = 936; // GB2312
	int nlen = ::WideCharToMultiByte(nCodePage,0,wszUnicodeString,-1,NULL,0,NULL,NULL);
	char* szBuf = (char*)malloc(nlen + 1);
	::WideCharToMultiByte(nCodePage, 0, wszUnicodeString, -1, szBuf, nlen, NULL, NULL);
	szBuf[nlen] = 0;
	sOut = szBuf;
	free(szBuf);
	return sOut;
}


// Only alphanum is safe.
static const char SAFE[256] =
{
    /*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
    /* 0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 1 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 2 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 3 */ 1,1,1,1, 1,1,1,1, 1,1,0,0, 0,0,0,0,
    
    /* 4 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    /* 5 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
    /* 6 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    /* 7 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
    
    /* 8 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 9 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* A */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* B */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    
    /* C */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* D */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* E */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* F */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};

string CnvEncoder::URLEncode(const string& sSrc)
{
    const char DEC2HEX[] = "0123456789ABCDEF";
    const unsigned char * pSrc = (const unsigned char *)sSrc.c_str();
    const int SRC_LEN = sSrc.length();
    unsigned char * const pStart = new unsigned char[SRC_LEN * 3];
    unsigned char * pEnd = pStart;
    const unsigned char * const SRC_END = pSrc + SRC_LEN;

    for (; pSrc < SRC_END; ++pSrc)
	{
		if (SAFE[*pSrc]) 
            *pEnd++ = *pSrc;
        else
        {
            // escape this char
            *pEnd++ = '%';
            *pEnd++ = DEC2HEX[*pSrc >> 4];
            *pEnd++ = DEC2HEX[*pSrc & 0x0F];
        }
	}

    std::string sResult((char *)pStart, (char *)pEnd);
    delete [] pStart;
    return sResult;
}

void CnvEncoder::Release()
{
	if( wcsBuf ) {
		free(wcsBuf);
		wcsBuf = NULL;
		wcsSize = 0;
	}
	if( mbBuf ) {
		free(mbBuf);
		mbBuf = NULL;
		mbSize = 0;
	}
}
