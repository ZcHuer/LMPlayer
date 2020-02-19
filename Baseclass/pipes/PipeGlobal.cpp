#include "stdafx.h"
#include "PipeGlobal.h"


_PIPEMSG_MAP_ENTRY** _FirstPipeMsgMapEntry()
{
	return &__pPipeMsgMapEntryFirst + 1;
}

_PIPEMSG_MAP_ENTRY** _LastPipeMsgMapEntry()
{
	return &__pPipeMsgMapEntryLast;
}

_PIPEMSG_MAP_ENTRY* _GetPipeMsgEntry(DWORD dwMsgID)
{
	_PIPEMSG_MAP_ENTRY* pEntry = NULL;

	_PIPEMSG_MAP_ENTRY** ppEntry = _FirstPipeMsgMapEntry();
	if(ppEntry)
	{
		for(; ppEntry != _LastPipeMsgMapEntry(); ++ppEntry)
		{
			if(*ppEntry && (*ppEntry)->dwMessage == dwMsgID)
			{
				pEntry = *ppEntry;
				break;
			}
		}
	}
	return pEntry;
}
