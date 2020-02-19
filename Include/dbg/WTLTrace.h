//////////////////////////////////////////////////////////////////////////
// �汾��V1.0
// ������2009.9.13
// ���ߣ���ɭ��
//------------------------------------------------------------------------
// ���ܣ� ����ϵͳATLTrace���������£�������������ԣ�����д��һ��
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
//����ǿ�����
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