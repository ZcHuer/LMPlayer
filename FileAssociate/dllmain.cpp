// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"


#include "AssociateFile.h"
#include "AssociateDisc.h"
#include "SetupAssociate.h"
#include "../DllHelper/LibIconHelper.h"
#include "../Include/CRTinclude/crt.h"


#ifdef _DEBUG
#define new new( _NORMAL_BLOCK, __FILE__, __LINE__) 
#endif

HINSTANCE _hInstance = NULL;
CLibIconHelper g_LibIcon;
bool g_bHasPower = true;


BOOL APIENTRY DllMain( HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/ )
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		_hInstance = hInstance;
		if( IsWindowVistaLatter() )
		{
			if( !IsHasUAC() ) 
				g_bHasPower = false;
		}
		g_LibIcon.Init(_hInstance);
		DisableThreadLibraryCalls(hInstance);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		g_LibIcon.Uninit();
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
STDAPI DllRegisterServer(void)
{
	CSetupAssociate::SetupFileAssociate();
	return S_OK;
}

STDAPI DllUnregisterServer(void)
{
	CSetupAssociate::RemoveAllFileAssociate();
	return S_OK;
}

void WINAPI InstallFileAssociate()
{
	CSetupAssociate::InstallByUpdate();
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool _cdecl IsHasFileAssociate(LPCWSTR lpExt)			//例：IsHasFileAssociate(L"avi")
{
	return CAssociateFile::IsFileAssociate(lpExt);
}


bool _cdecl AssociateFile(LPCWSTR lpExt, bool bAssociate)	//例:AssociateFile( L"avi", true );
{
	bool bRet = false;
	if( g_bHasPower )
		bRet = CAssociateFile::Associate(lpExt, bAssociate );
	//else
	//	::MessageBoxW(NULL, L"抱歉，权限不足!", L"提示",MB_OK);	// 正常情况下不会弹

	return bRet;
}

bool _cdecl SaveTempAssociateItem(LPCWSTR lpExt, bool bAssociate)
{
	return CSetupAssociate::SaveAssociateState(lpExt, bAssociate);
}

void _cdecl CleanAllTempAssociateItem()
{
	CSetupAssociate::CleanAllAssociateState();
}

bool _cdecl FastAssociateFile(LPCWSTR lpExt, bool bAssociate)	//例:AssociateFile( L"avi", true );
{
	return CAssociateFile::FastAssociate(lpExt, bAssociate );
}

bool _cdecl Apply()
{
	if( !g_bHasPower )
	{
		//弹出UAC
		CString sCmd;
		TCHAR szDll[MAX_PATH] = L"";
		TCHAR szApp[MAX_PATH] = L"";

		GetModuleFileName( _hInstance, szDll, _countof(szDll) );
		GetCurrentFilePath( _hInstance, L"StormPlayer.exe", szApp, _countof(szApp) );
		sCmd.Format( L"/Shell /Dll=\"%s\",UACFileAssociate", szDll );
		if(!UACExecuteApp( szApp, sCmd, INFINITE ))
		{
			CleanAllTempAssociateItem();
			return false;
		}
	}
	else
	{
		CSetupAssociate::FileAssociateFromTempWithPower();
	}

	return true;
}

//该函数给/Shell用
void _cdecl UACFileAssociate( LPCWSTR lpCmd )
{
	CSetupAssociate::FileAssociateFromTempWithPower();
}

bool _cdecl DiscAssociateDVD(bool bAssociate)
{
	return CAssociateDisc::AssociateDVD(bAssociate);
}

bool _cdecl DiscAssociateDVDAutoplay(bool bAssociate)
{
	return CAssociateDisc::AssociateDVDAutoplay(bAssociate);
}

bool _cdecl DiscAssociateUsbAutoplay(bool bAssociate)
{
	return CAssociateDisc::AssociateUsbAutoplay(bAssociate);
}

HICON _cdecl GetFileIcon(LPCWSTR lpExt)
{
	return g_LibIcon.GetAssociateIcon(lpExt);
}
