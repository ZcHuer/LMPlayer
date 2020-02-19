/********************************************************************
//Created:		2010/05/06
//Filename: 	URLEncode.h
//Arranged:		Liu xiaochun
//ToDo:			Encode URL.
*********************************************************************/

#ifndef __URLENCODE__H__
#define __URLENCODE__H__

#include <string>
#include <windows.h> 
using namespace std;
 

class CURLEncode
{
public:
	//delete outside.
	static BOOL WCHAR2char(const WCHAR* pSrc, OUT char** ppDest, UINT cp = CP_ACP) 
	{
		if (!pSrc || !ppDest) 
			return FALSE;

		BOOL bRet   = FALSE;
		int nRet    = 0;
		UINT uLen	= 0;

		nRet = WideCharToMultiByte(cp, 0, pSrc, -1, NULL, 0, NULL, NULL);
		if (!nRet)
			goto Exit0;

		uLen = nRet + 1;
		*ppDest = new char[uLen];
		if (!*ppDest)
			goto Exit0;

		ZeroMemory(*ppDest, sizeof(char) * uLen);
		nRet = WideCharToMultiByte(cp, 0, pSrc, -1, *ppDest, nRet, NULL, NULL);
		if (!nRet)
			goto Exit0;

		bRet = TRUE;

Exit0:
		return bRet;
	}

	static std::string EncodeURL(const char* pBufDummy, int nLen)
	{
		if(!pBufDummy || !nLen) return std::string("");
		const unsigned char  * pBuf = (const unsigned char*)pBufDummy ;
		static std::string   s_HexTable = "0123456789ABCDEF",
			s_UnsafeTable = "\"<>%\\^[]`'+$,@:;/!#?=&()" ;
		std::string   s ;
		for (int i=0 ; i < nLen ; i++)
			if ((pBuf[i] > 32) && (pBuf[i] < 123) && (s_UnsafeTable.find(pBuf[i]) == std::string::npos))
			{
				s += pBuf[i] ;
			}
			else
			{
				s += "%" ;
				s += s_HexTable[pBuf[i] / 16] ;
				s += s_HexTable[pBuf[i] % 16] ;
			}
			return s ;
	}

	static std::string EncodeURL(const wchar_t* pwBufDummy, int nLen, UINT cp = CP_ACP)
	{
		if(!pwBufDummy || !nLen) return std::string("");
		char* pDes = NULL;
		WCHAR2char(pwBufDummy, &pDes,cp);
		if(!pDes) return std::string("");
		std::string strResult = EncodeURL(pDes, (int)strlen(pDes));
		delete pDes;
		return strResult;
	}
};
 
#endif//__URLENCODE__H__