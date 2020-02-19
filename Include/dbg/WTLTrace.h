//////////////////////////////////////////////////////////////////////////
// 版本：V1.0
// 创建：2009.9.13
// 作者：黄森堂
//------------------------------------------------------------------------
// 功能： 由于系统ATLTrace在有中文下，会输出错误，所以，自已写了一个
//------------------------------------------------------------------------
// Copyright (c) www.baofeng.com All Rights Reserved
//////////////////////////////////////////////////////////////////////////
#pragma once
#include <atldef.h>

#ifdef _UNICODE

inline void _cdecl WtlTrace(LPCWSTR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);
	
	int nBuf;
	WCHAR szBuffer[2048];
	
	nBuf = _vsnwprintf(szBuffer, sizeof(szBuffer) / sizeof(WCHAR), lpszFormat, args);
	ATLASSERT(nBuf < sizeof(szBuffer));//Output truncated as it was > sizeof(szBuffer)
	
	OutputDebugStringW(szBuffer);
	va_end(args);
}

#else

inline void _cdecl WtlTrace(LPCSTR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);
	
	int nBuf;
	char szBuffer[2048];
	
	nBuf = _vsnprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);
	ATLASSERT(nBuf < sizeof(szBuffer)); //Output truncated as it was > sizeof(szBuffer)
	
	OutputDebugStringA(szBuffer);
	va_end(args);
}

#endif


//////////////////////////////////////////////////////////////////////////
//允许强制输出
//////////////////////////////////////////////////////////////////////////
#ifdef _FORCE_OUTPUT_STRING
	#define WTLTRACE	WtlTrace
#else
	#ifdef _DEBUG
		#define WTLTRACE	WtlTrace
	#else
		#define WTLTRACE
	#endif
#endif