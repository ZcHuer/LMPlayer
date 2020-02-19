#pragma once
#include "PipeDef.h"

#pragma warning(disable : 4311)
#pragma warning(disable : 4312)

typedef HRESULT (__stdcall * PFNCreateInstance)(DWORD dwMessage, LPVOID* lppMsg);

struct _PIPEMSG_MAP_ENTRY
{
	DWORD					dwMessage;
	LPCWSTR					pwsMessageName;
	PFNCreateInstance		pfnCreateInstance;
};

#pragma section("PMSGMAP$__a", read)
#pragma section("PMSGMAP$__z", read)
#pragma section("PMSGMAP$__m", read)

extern "C"
{
	__declspec(selectany) __declspec(allocate("PMSGMAP$__a")) _PIPEMSG_MAP_ENTRY* __pPipeMsgMapEntryFirst = NULL;
	__declspec(selectany) __declspec(allocate("PMSGMAP$__z")) _PIPEMSG_MAP_ENTRY* __pPipeMsgMapEntryLast = NULL;
}

#if !defined(_M_IA64)
#pragma comment(linker, "/merge:PMSGMAP=.rdata")
#endif


#ifndef PIPEMSGMAP_ENTRY_PRAGMA
#if defined(_M_IX86)
#define PIPEMSGMAP_ENTRY_PRAGMA(class) __pragma(comment(linker, "/include:___pPipeMsgMapEntry_" #class));
#elif defined(_M_IA64) || defined(_M_AMD64) || (_M_ARM)
#define PIPEMSGMAP_ENTRY_PRAGMA(class) __pragma(comment(linker, "/include:__pPipeMsgMapEntry_" #class));
#else
#error Unknown Platform. define OBJECT_ENTRY_PRAGMA
#endif
#endif	//PIPEMSGMAP_ENTRY_PRAGMA

#define PIPEMSG_ENTRY_AUTO(msgID, class) \
	const _PIPEMSG_MAP_ENTRY __objPipeMsgMap_##class = {msgID, _T(#msgID), class::_CreateInstance}; \
	extern "C" __declspec(allocate("PMSGMAP$__m")) __declspec(selectany) const _PIPEMSG_MAP_ENTRY* const __pPipeMsgMapEntry_##class = &__objPipeMsgMap_##class; \
	PIPEMSGMAP_ENTRY_PRAGMA(class)

_PIPEMSG_MAP_ENTRY** _FirstPipeMsgMapEntry();
_PIPEMSG_MAP_ENTRY** _LastPipeMsgMapEntry();
_PIPEMSG_MAP_ENTRY* _GetPipeMsgEntry(DWORD dwMsgID);

