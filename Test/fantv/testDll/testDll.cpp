// testDll.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <xstring>

int _tmain(int argc, _TCHAR* argv[])
{

	wchar_t szCurDir[1024] = {0};
	::GetCurrentDirectoryW(1024, szCurDir);

	std::wstring strPath =szCurDir;

	strPath += L"\\funotc.dll";
	HMODULE m_hMod = ::LoadLibraryW(strPath.c_str());

	char* ret= NULL;
	typedef char* (*FUNC)(char* mac);
	if (NULL != m_hMod)
	{
		FUNC initialize = (FUNC)::GetProcAddress(m_hMod, "GetPlayUrl");
		ret = initialize("fsp://320671/1067397/hd");
	}
	system( "PAUSE "); 
	return 0;
}

