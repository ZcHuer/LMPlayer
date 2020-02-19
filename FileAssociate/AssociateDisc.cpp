#include "StdAfx.h"
#include "AssociateDisc.h"


#ifdef _DEBUG
#define new new( _NORMAL_BLOCK, __FILE__, __LINE__) 
#endif

#define REG_AUTOPLAYHANDLERSL _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\AutoplayHandlers")
CAssociateDisc::CAssociateDisc(void)
{
}

CAssociateDisc::~CAssociateDisc(void)
{
}
//备份注册表键值
void CopyRegValue(HKEY hKey, LPCWSTR lpSubKey, LPCWSTR lpKeyValue)
{
	WCHAR szKeyValueBK[MAX_PATH] = L"";
	DWORD dwKeyValueType = REG_SZ;
	DWORD dwMax = sizeof(szKeyValueBK);

	CString strKeyNameBak;
	strKeyNameBak.Format(L"%s.backup", lpKeyValue);

	if (SHGetValue(hKey, lpSubKey, strKeyNameBak,&dwKeyValueType, szKeyValueBK,&dwMax) != ERROR_SUCCESS)
	{
		dwMax = sizeof(szKeyValueBK);
		if (SHGetValue(hKey, lpSubKey,lpKeyValue,&dwKeyValueType, szKeyValueBK,&dwMax) == ERROR_SUCCESS)
		{
			CrtRegSetString(hKey, lpSubKey, strKeyNameBak, szKeyValueBK);
		}

	}
}

//还原键值
void RestoreRegValue(HKEY hKye, LPCWSTR lpSubKey, LPCWSTR lpKeyValue)
{
	WCHAR szKeyValueBK[MAX_PATH] = L"";
	DWORD dwKeyValueType = REG_SZ;
	DWORD dwMax = MAX_PATH;
	CString strKeyNameBak;
	strKeyNameBak.Format(L"%s.backup", lpKeyValue);

	if (SHGetValue(hKye, lpSubKey,strKeyNameBak,&dwKeyValueType, szKeyValueBK,&dwMax) == ERROR_SUCCESS)
	{
		CrtRegSetString(hKye, lpSubKey, lpKeyValue, szKeyValueBK);
		CrtRegDeleteKeyValue(hKye, lpSubKey, strKeyNameBak);
	}		
}

bool CAssociateDisc::AssociateDVD(bool bAssociate)
{
	if( bAssociate )
	{
		CString sIcon;
		WCHAR szLibIcon[MAX_PATH] = L"";

		GetCurrentFilePath( _hInstance, L"LibIcon.dll", szLibIcon, _countof(szLibIcon) );
		sIcon.Format( L"%s,-202", szLibIcon);

		CrtRegSetString( HKEY_CLASSES_ROOT, L"DVD\\DefaultIcon", L"", sIcon );
		CrtRegSetString( HKEY_CLASSES_ROOT, L"DVD\\shell", L"", L"play" );

		CrtRegSetString( HKEY_CLASSES_ROOT, L"DVD\\shell\\play", L"", L"使用 联想视频 播放(&P)" );

        CString sSubkey,sExePath;
		WCHAR szAppFile[MAX_PATH] = L"";
		GetCurrentFilePath( _hInstance, L"StormPlayer.exe", szAppFile, _countof(szAppFile) );
		sExePath.Format(L"\"%s\" /Type=LocalFile /Disc=\"%%1\" /DiscType=DVD /From=Shell", szAppFile);
		
		CrtRegSetString( HKEY_CLASSES_ROOT,L"DVD\\shell\\play\\command", L"",sExePath );	

	}
	else
	{   
		CrtRegSetString(HKEY_CLASSES_ROOT, L"DVD\\DefaultIcon", L"", L"shell32.dll,40");
		CrtRegSetString(HKEY_CLASSES_ROOT, L"DVD\\shell", L"", L"Play");
		if( IsWindowXP() )
			CrtRegSetString(HKEY_CLASSES_ROOT, L"DVD\\shell\\play", L"", L"播放(&P)");
		else
			CrtRegSetString(HKEY_CLASSES_ROOT, L"DVD\\shell\\play", L"", L"&Play");

		LPCWSTR lpValue = L"\"%ProgramFiles%\\Windows Media Player\\wmplayer.exe\" /prefetch:4 /device:DVD \"%L\"";
		SHSetValue( HKEY_CLASSES_ROOT, L"DVD\\shell\\play\\command",  L"", REG_EXPAND_SZ, lpValue, wcslen(lpValue)*sizeof(WCHAR) );
	}

	return true;
}

bool CAssociateDisc::AssociateDVDAutoplay(bool bAssociate)
{
	TCHAR szHandlerTest[128] = {0};
	DWORD dwType;
	DWORD dwSize = sizeof(szHandlerTest);
	CString  strDvdHandlersSubKey,strDvdArrivalSubKey, strEventHandlDefSelSubKey;

	strDvdHandlersSubKey.Format(L"%s\\Handlers\\LePlayerDVDHandler", REG_AUTOPLAYHANDLERSL);
	strDvdArrivalSubKey.Format(L"%s\\EventHandlers\\PlayDVDMovieOnArrival", REG_AUTOPLAYHANDLERSL);
	strEventHandlDefSelSubKey.Format(L"%s\\EventHandlersDefaultSelection", REG_AUTOPLAYHANDLERSL);

	if( bAssociate )
	{
		if (ERROR_SUCCESS != SHGetValue(HKEY_LOCAL_MACHINE, strDvdHandlersSubKey, L"", &dwType, szHandlerTest, &dwSize))
		{
            CrtRegSetString(HKEY_LOCAL_MACHINE, strDvdHandlersSubKey, L"Action", L"使用 联想视频 播放DVD");

			CString strIconLocation;
			WCHAR szAppFile[MAX_PATH] = L"";
			GetCurrentFilePath( _hInstance, L"StormPlayer.exe", szAppFile, _countof(szAppFile) );
			strIconLocation.Format(L"%s,-%d", szAppFile, 0);
			CrtRegSetString(HKEY_LOCAL_MACHINE, strDvdHandlersSubKey,  L"DefaultIcon", strIconLocation);

			CrtRegSetString(HKEY_LOCAL_MACHINE, strDvdHandlersSubKey, L"InvokeProgID", L"DVD");
			CrtRegSetString(HKEY_LOCAL_MACHINE, strDvdHandlersSubKey, L"InvokeVerb", L"play");
			CrtRegSetString(HKEY_LOCAL_MACHINE, strDvdHandlersSubKey,L"Provider", L"联想视频");
		}

		CrtRegSetString(HKEY_LOCAL_MACHINE, strDvdArrivalSubKey,L"LePlayerDVDHandler", L"");

		//为PlayDVDMovieOnArrival创建备份键PlayDVDMovieOnArrival_Bak
		CopyRegValue(HKEY_CURRENT_USER, strEventHandlDefSelSubKey,  L"PlayDVDMovieOnArrival");
// 		SHSetValue(HKEY_CURRENT_USER, strEventHandlDefSelSubKey, L"PlayDVDMovieOnArrival", REG_SZ, 
// 				L"SuperPlayerDVDHandler",_tcslen( L"LePlayerDVDHandler")*sizeof(WCHAR));
		CrtRegSetString(HKEY_CURRENT_USER, strEventHandlDefSelSubKey, L"PlayDVDMovieOnArrival", L"SuperPlayerDVDHandler");
	}
	else
	{
		CrtRegDeleteKeyValue(HKEY_LOCAL_MACHINE,strDvdArrivalSubKey, L"LePlayerDVDHandler");
		CrtRegDeleteKeyValue(HKEY_LOCAL_MACHINE,strDvdHandlersSubKey, NULL);
		//SHDeleteValue(HKEY_LOCAL_MACHINE, strDvdHandlersSubKey, L"LePlayerDVDHandler");
		CrtRegDeleteKeyValue(HKEY_LOCAL_MACHINE,strDvdHandlersSubKey, L"LePlayerDVDHandler");
		//将PlayDVDMovieOnArrival还原为关联前值
		RestoreRegValue(HKEY_CURRENT_USER, strEventHandlDefSelSubKey, L"PlayDVDMovieOnArrival");
	}

	return true;
}

bool CAssociateDisc::AssociateUsbAutoplay(bool bAssociate)
{
	CString  strMediaFilesHandlersSubKey;
	strMediaFilesHandlersSubKey.Format(L"%s\\Handlers\\LePlayerMediaFilesHandler", REG_AUTOPLAYHANDLERSL);

	CString strVideoFilesArrivalSubKey, strEventHandlDefSelSubKey;
	strVideoFilesArrivalSubKey.Format(L"%s\\EventHandlers\\PlayVideoFilesOnArrival", REG_AUTOPLAYHANDLERSL);

	CString strMusicFilesArrivalSubKey;
	strMusicFilesArrivalSubKey.Format(L"%s\\EventHandlers\\PlayMusicFilesOnArrival",REG_AUTOPLAYHANDLERSL);

	CString strMixedContentArrivalSubKey;
	strMixedContentArrivalSubKey.Format(L"%s\\EventHandlers\\MixedContentOnArrival", REG_AUTOPLAYHANDLERSL);

	strEventHandlDefSelSubKey.Format(L"%s\\EventHandlersDefaultSelection" ,REG_AUTOPLAYHANDLERSL);


	TCHAR szHandlerTest[128] = {0};
	DWORD dwType;
	DWORD dwSize = sizeof(szHandlerTest);

	if( bAssociate )
	{
		if (ERROR_SUCCESS != SHGetValue(HKEY_LOCAL_MACHINE, strMediaFilesHandlersSubKey, L"", 
			&dwType, szHandlerTest, &dwSize))
		{
			CrtRegSetString(HKEY_LOCAL_MACHINE, strMediaFilesHandlersSubKey, L"Action",L"使用 联想视频 播放");

			CString strIconLocation;
			WCHAR szAppFile[MAX_PATH] = L"";
			GetCurrentFilePath( _hInstance, L"StormPlayer.exe", szAppFile, _countof(szAppFile) );
			strIconLocation.Format(L"%s,-%d", szAppFile, 0);
			CrtRegSetString(HKEY_LOCAL_MACHINE, strMediaFilesHandlersSubKey, L"DefaultIcon",strIconLocation);

			CString strMediaFilesProgId;
			strMediaFilesProgId.Format(L"%s.%s",L"LePlayer", L"mediafiles");
			CrtRegSetString(HKEY_LOCAL_MACHINE, strMediaFilesHandlersSubKey, L"InvokeProgID",strMediaFilesProgId);

			CrtRegSetString(HKEY_LOCAL_MACHINE, strMediaFilesHandlersSubKey, L"InvokeVerb",L"open");
			CrtRegSetString(HKEY_LOCAL_MACHINE, strMediaFilesHandlersSubKey, L"Provider",L"联想视频");
		}

		CrtRegSetString(HKEY_LOCAL_MACHINE, strVideoFilesArrivalSubKey,L"LePlayerMediaFilesHandler", L"");
		CrtRegSetString(HKEY_LOCAL_MACHINE, strMusicFilesArrivalSubKey, L"LePlayerMediaFilesHandler", L"");
		CrtRegSetString(HKEY_LOCAL_MACHINE, strMixedContentArrivalSubKey, L"LePlayerMediaFilesHandler", L"");

		//为PlayVideoFilesOnArrival键创建备份PlayVideoFilesOnArrival_Bak
		CopyRegValue(HKEY_CURRENT_USER, strEventHandlDefSelSubKey,  L"PlayVideoFilesOnArrival");
// 		SHSetValue(HKEY_CURRENT_USER, strEventHandlDefSelSubKey, L"PlayVideoFilesOnArrival", REG_SZ, 
// 			L"SuperPlayerMediaFilesHandler",_tcslen( L"LePlayerMediaFilesHandler")*sizeof(WCHAR));
		CrtRegSetString(HKEY_CURRENT_USER, strEventHandlDefSelSubKey, L"PlayVideoFilesOnArrival", L"LePlayerMediaFilesHandler");

		//为PlayMusicFilesOnArrival键创建备份PlayMusicFilesOnArrival_Bak
		CopyRegValue(HKEY_CURRENT_USER, strEventHandlDefSelSubKey,  L"PlayMusicFilesOnArrival");
// 		SHSetValue(HKEY_CURRENT_USER, strEventHandlDefSelSubKey, L"PlayMusicFilesOnArrival", REG_SZ, 
// 			L"SuperPlayerMediaFilesHandler",_tcslen( L"LePlayerMediaFilesHandler")*sizeof(WCHAR));
		CrtRegSetString(HKEY_CURRENT_USER, strEventHandlDefSelSubKey, L"PlayMusicFilesOnArrival", L"LePlayerMediaFilesHandler");

        //为MixedContentOnArrival键创建备份键MixedContentOnArrival_Bak
		CopyRegValue(HKEY_CURRENT_USER, strEventHandlDefSelSubKey,  L"MixedContentOnArrival");
// 		SHSetValue(HKEY_CURRENT_USER, strEventHandlDefSelSubKey, L"MixedContentOnArrival", REG_SZ, 
// 			L"SuperPlayerMediaFilesHandler",_tcslen( L"LePlayerMediaFilesHandler")*sizeof(WCHAR));
		CrtRegSetString(HKEY_CURRENT_USER, strEventHandlDefSelSubKey, L"MixedContentOnArrival", L"LePlayerMediaFilesHandler");

	}
	else
	{
		SHDeleteKey( HKEY_LOCAL_MACHINE,strMediaFilesHandlersSubKey );

		CrtRegDeleteKeyValue(HKEY_LOCAL_MACHINE,strVideoFilesArrivalSubKey, L"LePlayerMediaFilesHandler");
		CrtRegDeleteKeyValue(HKEY_LOCAL_MACHINE,strMusicFilesArrivalSubKey, L"LePlayerMediaFilesHandler");
		CrtRegDeleteKeyValue(HKEY_LOCAL_MACHINE,strMixedContentArrivalSubKey, L"LePlayerMediaFilesHandler");

		
		//恢复关联前键值
		RestoreRegValue(HKEY_CURRENT_USER, strEventHandlDefSelSubKey, L"PlayVideoFilesOnArrival");
		RestoreRegValue(HKEY_CURRENT_USER, strEventHandlDefSelSubKey, L"PlayMusicFilesOnArrival");
		RestoreRegValue(HKEY_CURRENT_USER, strEventHandlDefSelSubKey, L"MixedContentOnArrival");
	}

	return true;
}
