#pragma once

// #define ZY_DEBUG
#ifdef ZY_DEBUG
// ZY, Macros for debug
#include <atlbase.h>
#include <atlstr.h>

#define DEBUG_INSERT_BREAK if (IsDebuggerPresent()) {__asm{INT 3};}
#define DEBUG_SHOW_MESSAGEBOX do \
{ \
	DWORD proID = ::GetCurrentProcessId(); \
	DWORD threadID = ::GetCurrentThreadId(); \
	CString debugStr; \
	debugStr.Format(_T("ZYDEBUG, %S, Line: %d\nProcessID: %d[0x%x]\nThreadID: %d[0x%x]\n"), __FUNCDNAME__, __LINE__, proID, proID, threadID, threadID); \
	::MessageBox(NULL, debugStr, _T("For Debug"), 0); \
} while(0)

#define DEBUG_OUTPUT_MESSAGE do \
{ \
	DWORD proID = ::GetCurrentProcessId(); \
	DWORD threadID = ::GetCurrentThreadId(); \
	CString debugStr; \
	debugStr.Format(_T("[%d, %d] ZYDEBUG, %s, %d\n"), proID, threadID, __FUNCTIONW__, __LINE__); \
	::OutputDebugString(debugStr); \
} while(0)

#define DEBUG_OUTPUT(FORMAT_STR, ...) do \
{ \
	DWORD proID = ::GetCurrentProcessId(); \
	DWORD threadID = ::GetCurrentThreadId(); \
	CString debugStr; \
	debugStr.Format(_T("[%d, %d] ZYDEBUG, "), proID, threadID); \
	debugStr.AppendFormat(FORMAT_STR, __VA_ARGS__); \
	::OutputDebugString(debugStr); \
} while(0)

#define DEBUG_OUTPUT_ERROR_MESSAGE(err) do \
{ \
	DWORD proID = ::GetCurrentProcessId(); \
	DWORD threadID = ::GetCurrentThreadId(); \
	CString debugStr; \
	debugStr.Format(_T("[%d, %d] ZYDEBUG, "), proID, threadID); \
	WCHAR buffer[0xff] = {0}; \
	::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, err, 0, buffer, 0xff, NULL); \
	debugStr.AppendFormat(_T("%s"), buffer); \
	::OutputDebugString(debugStr); \
} while(0)

class CLogFunc
{
public:
	CLogFunc(LPCTSTR strFunc, int dLine) : 
m_strFunc(strFunc),
	m_dLine(dLine)
{
	DEBUG_OUTPUT(_T("%s:%d start\n"), m_strFunc, m_dLine);
}

~CLogFunc()
{
	DEBUG_OUTPUT(_T("%s:%d end\n"), m_strFunc, m_dLine);
}
private:
	LPCTSTR m_strFunc;
	int m_dLine;
};

#define DEBUG_LOG_FUNC CLogFunc tempLog(__FUNCTIONW__, __LINE__)


#else
#define DEBUG_INSERT_BREAK
#define DEBUG_SHOW_MESSAGEBOX
#define DEBUG_OUTPUT_MESSAGE
#define DEBUG_OUTPUT
#define DEBUG_OUTPUT_ERROR_MESSAGE
#define DEBUG_LOG_FUNC
#endif

