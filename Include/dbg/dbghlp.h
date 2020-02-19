/*
 * Copyright(C) 2009,baofeng 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DBGHLp.h
 * 摘    要：
 * 当前版本：1.0
 * 作    者：dayxuyi
 * 创建日期：2009年10月17日
 */
#ifndef _DEBUG_HELP_H_
#define _DEBUG_HELP_H_
#include <stdio.h>          
#include <stdarg.h>             
#include <wchar.h>     
#define _DEBUG_MAX_STRINGBUFLENGTH_ 2048

class CDbgPrint{
public:
	CDbgPrint(LPCSTR lpFile,int nline)
		:src_file(lpFile)
		,src_line(nline)
	{

	}
	void operator()(const char * fmt, ...)const
	{
		char Buffer[_DEBUG_MAX_STRINGBUFLENGTH_]={0};
		va_list ap;
		int len = src_file ? _snprintf_s(Buffer, _countof(Buffer), _TRUNCATE, "%s (%d): ", src_file, src_line) : 0;
		va_start(ap, fmt);
		vsprintf_s(Buffer+len,_DEBUG_MAX_STRINGBUFLENGTH_-len, fmt, ap);
		va_end(ap);
		strcat_s(Buffer, _countof(Buffer), "\n");
		OutputDebugStringA(Buffer);
	}
	void operator()(const wchar_t * fmt, ...)const
	{
		wchar_t Buffer[_DEBUG_MAX_STRINGBUFLENGTH_- 1] = {0};
		va_list ap;

		int len = 0;
		if (src_file)
		{
			int srcFileLen = strlen(src_file);
			wchar_t *wFileName = new wchar_t[srcFileLen + 1];
			memset(wFileName,0,(srcFileLen+1)*2);
			MultiByteToWideChar(CP_ACP,NULL,src_file,srcFileLen,wFileName,srcFileLen);
			len += _snwprintf_s(Buffer, _countof(Buffer), _TRUNCATE,  L"%s (%d): ",wFileName, src_line);
			delete[] wFileName;
		}
		va_start(ap, fmt);
		vswprintf_s(Buffer+len, _DEBUG_MAX_STRINGBUFLENGTH_-len - 1,fmt, ap);
		va_end(ap);
		wcscat_s(Buffer, _countof(Buffer), L"\n");
		OutputDebugStringW(Buffer);
	}

private :
	LPCSTR	src_file;
	int	    src_line;
};

#define KR_TRACE_RECT(rect) \
{\
	char szMes[128] = {0};\
	sprintf(szMes," "#rect##" : left=%d, top=%d, right=%d, bottom=%d (%d,%d)",\
	(rect).left, (rect).top, (rect).right, (rect).bottom,(rect).right-(rect).left,(rect).bottom-(rect).top);\
	KR_TRACE(szMes);\
}
#define KR_TRACE_RECT2(rect) \
{\
	char szMes[128] = {0};\
	sprintf(szMes," "#rect##" : left=%d, top=%d, right=%d, bottom=%d (%d,%d)",\
	(rect).left, (rect).top, (rect).right, (rect).bottom,(rect).right-(rect).left,(rect).bottom-(rect).top);\
	KR_TRACE2(szMes);\
}

#if defined(_DEBUG) || defined(_TRACE_FORCE_)
	#define KR_TRACE  CDbgPrint(__FILE__,__LINE__)
	#define KR_TRACE2  CDbgPrint(NULL,0)
#else
	#define KR_TRACE
	#define KR_TRACE2 
#endif

#endif//_DEBUG_HELP_H_