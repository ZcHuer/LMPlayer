// LibraryIcon.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "FileAssociateIcon.h"

HINSTANCE _hInstance = NULL;
BOOL APIENTRY DllMain( HINSTANCE hInstance, DWORD  dwcall, LPVOID lpReserved )
{
	if( dwcall == DLL_PROCESS_ATTACH )
	{
		_hInstance = hInstance;
	}
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
UINT __cdecl GetAssociateIconId(LPCWSTR lpExt)
{
	return CFileAssociateIcon::GetIconId(lpExt);
}

HICON __cdecl GetAssociateIcon(LPCWSTR lpExt)
{
	return CFileAssociateIcon::GetIcon(lpExt);
}

BOOL __cdecl IsDefaultAssociate(LPCWSTR lpExt)
{
	return CFileAssociateIcon::IsDefaultAss(lpExt);
}

LPCWSTR __cdecl EnumFileExt(DWORD dwIndex)
{
	return CFileAssociateIcon::EnumExt(dwIndex);
}