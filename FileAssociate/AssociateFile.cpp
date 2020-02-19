#include "StdAfx.h"
#include "AssociateFile.h"
#include <shlobj.h>
//#include "../../../Include/def/KRDef.h"
#include "../DllHelper/LibIconHelper.h"
#include "DefaultFileAssociate.h"


#ifdef _DEBUG
#define new new( _NORMAL_BLOCK, __FILE__, __LINE__) 
#endif

CAssociateFile::CAssociateFile(void)
{
}

CAssociateFile::~CAssociateFile(void)
{
}
bool CAssociateFile::IsFileAssociate(LPCWSTR lpExt)
{
	if(IsWindowXP())
	{
		return IsFileAssociate_XP(lpExt);	
	}else
	{
		return IsFileAssociate_WIN7(lpExt);
	}
}
bool CAssociateFile::IsFileAssociate_WIN7(LPCWSTR lpExt)
{
	return JudgeIsFileAssociate(lpExt);

	//sSubkey.Format( L"%s\\.%s\\UserChoice", REG_FILEEXTS, lpExt );
	//bool bRet = CrtRegGetString( HKEY_CURRENT_USER, sSubkey, L"Progid", szAssoc, sizeof(szAssoc) );
	//if( bRet )
	//{
	//	return IsMyAssociate(szAssoc);
	//}
	//else
	//{
	//	sSubkey.Format( L".%s", lpExt );
	//	bRet = CrtRegGetString( HKEY_CLASSES_ROOT, sSubkey, L"", szAssoc, sizeof(szAssoc) );
	//	if( bRet )
	//		return IsMyAssociate(szAssoc);
	//}
}


bool CAssociateFile::IsFileAssociate_XP(LPCWSTR lpExt)
{
	if(!JudgeIsFileAssociate(lpExt))
	{
		return false;
	}
	//if( bRet )
	//{
	//	bRet = IsMyAssociate(szAssoc);

	//	// 增加Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.xxx 的Progid判断
	//	sSubkey.Format( L"%s\\.%s", REG_FILEEXTS, lpExt );
	//	bool bRet = CrtRegGetString( HKEY_CURRENT_USER, sSubkey, L"Progid", szAssoc, MAX_PATH );
	//	if ( bRet )
	//	{
	//		bRet = IsMyAssociate(szAssoc);
	//	}
	//}
	//else
	//{
	//	sSubkey.Format( L".%s", lpExt );
	//	bRet = CrtRegGetString( HKEY_CLASSES_ROOT, sSubkey, L"", szAssoc, MAX_PATH );
	//	if( bRet )
	//		bRet =  IsMyAssociate(szAssoc);
	//}

	CString sExt, sAssocKey, sSubkey;
	WCHAR szData[MAX_PATH] = L"";

	sExt.Format( L".%s", lpExt );
	sAssocKey.Format( L"%s%s", NAME_FileAssoc, lpExt );
	//判断我们的节点
	// [HKEY_CLASSES_ROOT\.avi]
	// @="LePlayer.avi"
	CrtRegGetString(HKEY_CLASSES_ROOT, sExt, L"", szData, MAX_PATH );
	if( StrStrI(szData, sAssocKey) == NULL )
	{	
		return false;
	}
	
	sSubkey.Format( _T("%s\\shell\\open\\command"),  sAssocKey );
	CrtRegGetString(HKEY_CLASSES_ROOT, sSubkey, L"", szData, MAX_PATH );
	if( StrStrI(szData, L"StormPlayer.exe") == NULL )
	{
		return false;
	}
	return true;
}

bool CAssociateFile::IsMyAssociate(LPCWSTR lpAssoc)
{
	if( StrStrI(lpAssoc, NAME_FileAssoc) == NULL )
		return false;

	return CrtRegIsKeyExist(HKEY_CLASSES_ROOT, lpAssoc);
}

bool CAssociateFile::Associate(LPCWSTR lpExt, bool bAssociate)
{
	if( lpExt == NULL )
		return false;

	bool bRet = false;
	if( bAssociate )
		bRet = AddFileAssociate(lpExt);
	else
		bRet = DeleteFileAssociate(lpExt);
	return bRet;
}

bool CAssociateFile::FastAssociate(LPCWSTR lpExt, bool bAssociate)
{
	if( lpExt == NULL )
		return false;

	if( IsWindowXP() )
		Associate(lpExt, bAssociate);

	bool bRet = true;
	if( bAssociate )
	{
		CString sAssocKey, sSubkey;
		WCHAR szData[MAX_PATH] = L"";

		sAssocKey.Format( L"%s%s", NAME_FileAssoc, lpExt );
		sSubkey.Format( L"%s\\.%s\\UserChoice", REG_FILEEXTS, lpExt );
		bool bRet = CrtRegGetString( HKEY_CURRENT_USER, sSubkey, L"Progid", szData, sizeof(szData) );
		if( bRet )
		{
			if( StrStrI(szData, NAME_FileAssoc) == NULL )
			{
				CrtRegSetString(HKEY_CURRENT_USER, sSubkey, NAME_FileAssoc_Bak, szData );
			}
		}

		CrtRegSetString(HKEY_CURRENT_USER, sSubkey, L"Progid", sAssocKey );


		// 增加Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.xxx 的Progid的处理
		sAssocKey.Format( L"%s%s", NAME_FileAssoc, lpExt );
		sSubkey.Format( L"%s\\.%s", REG_FILEEXTS, lpExt );
		bRet = CrtRegGetString( HKEY_CURRENT_USER, sSubkey, L"Progid", szData, sizeof(szData) );
		if( bRet )
		{
			if( StrStrI(szData, NAME_FileAssoc) == NULL )
			{
				CrtRegSetString(HKEY_CURRENT_USER, sSubkey, NAME_FileAssoc_Bak, szData );
			}
		}

		CrtRegSetString(HKEY_CURRENT_USER, sSubkey, L"Progid", sAssocKey );


		// 增加HKEY_CLASSES_ROOT\\.xxx 的Progid的处理
		sSubkey.Format( L".%s", lpExt );
		bRet = CrtRegGetString( HKEY_CLASSES_ROOT, sSubkey, L"", szData, sizeof(szData) );
		if( bRet )
		{
			if( StrStrI(szData, NAME_FileAssoc) == NULL )
			{
				CrtRegSetString(HKEY_CLASSES_ROOT, sSubkey, NAME_FileAssoc_Bak, szData );
			}
		}
		CrtRegSetString(HKEY_CLASSES_ROOT, sSubkey, L"", sAssocKey );
	}
	else
	{
		CString sSubkey;
		WCHAR szData[MAX_PATH] = L"";
		//删除用户手工选择
		//HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.AAC\UserChoice
		sSubkey.Format( L"%s\\.%s\\UserChoice", REG_FILEEXTS, lpExt );
		bRet = CrtRegGetString( HKEY_CURRENT_USER, sSubkey, NAME_FileAssoc_Bak, szData, sizeof(szData) );
		if( bRet )
		{
			CrtRegSetString(HKEY_CURRENT_USER, sSubkey, L"Progid", szData );
			CrtRegDeleteKeyValue(HKEY_CURRENT_USER, sSubkey, NAME_FileAssoc_Bak );
		}

		sSubkey.Format( L"%s\\.%s", REG_FILEEXTS, lpExt );
		bRet = CrtRegGetString( HKEY_CURRENT_USER, sSubkey, NAME_FileAssoc_Bak, szData, sizeof(szData) );
		if( bRet )
		{
			CrtRegSetString(HKEY_CURRENT_USER, sSubkey, L"Progid", szData );
			CrtRegDeleteKeyValue(HKEY_CURRENT_USER, sSubkey, NAME_FileAssoc_Bak );
		}

		sSubkey.Format( L".%s", lpExt );
		bRet = CrtRegGetString( HKEY_CLASSES_ROOT, L"", NAME_FileAssoc_Bak, szData, sizeof(szData) );
		if( bRet )
		{
			CrtRegSetString(HKEY_CLASSES_ROOT, sSubkey, L"", szData );
			CrtRegDeleteKeyValue(HKEY_CLASSES_ROOT, sSubkey, NAME_FileAssoc_Bak );
		}
	}
	return true;
}

// [HKEY_CLASSES_ROOT\.avi]
// @="LePlayer.avi"
// [HKEY_CLASSES_ROOT\.avi\OpenWithProgIds]
// "WMP11.AssocFile.AVI"=hex(0):

// OpenwithList和OpenwithProgIds有什么区别 
// OpenwithList 与 OpenwithProgIds 是定义“打开方式”列表的两种途径，
//前者直接添加程序名称、后者添加应用程序的 ClassIDs。
//当这两个地方同时定义了相同的程序时，重复的项目将被自动合并。

bool CAssociateFile::AddFileAssociate(LPCWSTR lpExt)
{
	//先创建我们的文件关联节点
	BFKey_AddAssociate(lpExt);

	CString sExt, sAssocKey, sSubkey;
	WCHAR szData[MAX_PATH] = L"";
	
	sExt.Format( L".%s", lpExt );
	sAssocKey.Format( L"%s%s", NAME_FileAssoc, lpExt );

	//如果没有我们的标识，要备份
	CrtRegGetString( HKEY_CLASSES_ROOT, sExt, L"", szData, sizeof(szData) );
	if( StrStrI(szData, NAME_FileAssoc) == NULL )
	{
		CrtRegSetString(HKEY_CLASSES_ROOT, sExt, NAME_FileAssoc_Bak, szData );
	}

	//写入我们的节名
	// [HKEY_CLASSES_ROOT\.avi]
	// @="LePlayer.avi"
	CrtRegSetString(HKEY_CLASSES_ROOT, sExt, L"", sAssocKey );

	// [HKEY_CLASSES_ROOT\.avi\OpenWithProgIds]
	// "WMP11.AssocFile.AVI"=hex(0):
	sSubkey.Format( L"%s\\OpenWithProgIds", sExt );
	SHSetValue( HKEY_CLASSES_ROOT, sSubkey, sAssocKey, REG_BINARY, NULL, 0 );

	//用户手动选择
	AddUserChoice(lpExt);

	return true;
}

//DeleteFileAssociate(L"avi");
bool CAssociateFile::DeleteFileAssociate(LPCWSTR lpExt)
{
	CString sExt, sAssocKey, sSubkey;
	WCHAR szData[MAX_PATH] = L"";

	sExt.Format( L".%s", lpExt );
	sAssocKey.Format( L"%s%s", NAME_FileAssoc, lpExt );

	//取出我们的备份值进行恢复，并删除备份。
	bool bRet = CrtRegGetString( HKEY_CLASSES_ROOT, sExt, NAME_FileAssoc_Bak, szData, sizeof(szData) );
	if( bRet )
	{
		//这里做两层判断：要恢复的Key已不存或就是我们的Key
		bool bResetKey = false;
		if( !CrtRegIsKeyExist( HKEY_CLASSES_ROOT, szData) )
			bResetKey = true;
		else
		{
			if( StrStrI(szData, NAME_FileAssoc) != NULL )
				bResetKey = true;
		}
		CrtRegDeleteKeyValue(HKEY_CLASSES_ROOT, sExt, NAME_FileAssoc_Bak );

		//如果是要恢复到系统默认，则使用系统默认
		if( bResetKey )
		{
			LPCWSTR lpDefFileAssociate = CDefaultFileAssociate::GetDefaultFileAssociate(lpExt);
			if( lpDefFileAssociate )
				CrtRegSetString(HKEY_CLASSES_ROOT, sExt, L"", lpDefFileAssociate );
			else
				SHDeleteKey( HKEY_CLASSES_ROOT, sExt );
		}
		else
			CrtRegSetString(HKEY_CLASSES_ROOT, sExt, L"", szData );
	}
	else
	{
		//等默认写好，再打开
		//如果取不到备份的键值,判断是不是系统默认的,不是系统默认，则删除
		LPCWSTR lpDefFileAssociate = CDefaultFileAssociate::GetDefaultFileAssociate(lpExt);
		if( lpDefFileAssociate == NULL )
			SHDeleteKey( HKEY_CLASSES_ROOT, sExt );
	}


	//删除用户手工选择
	//HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.AAC\UserChoice
	sSubkey.Format( L"%s\\.%s\\UserChoice", REG_FILEEXTS, lpExt );
	bRet = CrtRegGetString( HKEY_CURRENT_USER, sSubkey, NAME_FileAssoc_Bak, szData, sizeof(szData) );
	if( bRet )
	{
		CrtRegSetString(HKEY_CURRENT_USER, sSubkey, L"Progid", szData );
		CrtRegDeleteKeyValue(HKEY_CURRENT_USER, sSubkey, NAME_FileAssoc_Bak );
	}
	//Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.xxx
	sSubkey.Format( L"%s\\.%s", REG_FILEEXTS, lpExt );
	bRet = CrtRegGetString( HKEY_CURRENT_USER, sSubkey, NAME_FileAssoc_Bak, szData, sizeof(szData) );
	if( bRet )
	{
		CrtRegSetString(HKEY_CURRENT_USER, sSubkey, L"Progid", szData );
		CrtRegDeleteKeyValue(HKEY_CURRENT_USER, sSubkey, NAME_FileAssoc_Bak );
	}

	// [HKEY_CLASSES_ROOT\.avi\OpenWithProgIds]
	// "WMP11.AssocFile.AVI"=hex(0):
	sSubkey.Format( L"%s\\OpenWithProgIds", sExt );
	CrtRegDeleteKeyValue( HKEY_CLASSES_ROOT, sSubkey, sAssocKey );

	return true;
}

//////////////////////////////////////////////////////////////////////////
//关联的删除与添加,[注]删除节点只在Dll UnReg才删除，取消关联不删除。
//////////////////////////////////////////////////////////////////////////
void CAssociateFile::BFKey_AddAssociate(LPCWSTR lpExt)
{
	CString sExt, sAssocKey, sSubkey;

	sExt.Format( L".%s", lpExt );
	sAssocKey.Format( L"%s%s", NAME_FileAssoc, lpExt );

	//创建我们的关联节
	//.bov 和 .bfp 文件需要特殊处理
	AddShell( HKEY_CLASSES_ROOT, sAssocKey, GetExtType(sExt));

	//添加关联图标
	AddShellIcon( HKEY_CLASSES_ROOT, sAssocKey, lpExt );


	//添加这个节点
	// 	[HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.avi\OpenWithProgids]
	// 	"LePlayer.avi"=hex(0):
	sSubkey.Format( L"%s\\.%s\\OpenWithProgids", REG_FILEEXTS, lpExt );
	SHSetValue( HKEY_CLASSES_ROOT, sSubkey, sAssocKey, REG_BINARY, NULL, 0 );
}

void CAssociateFile::BFKey_DeleteAssociate(LPCWSTR lpExt)
{
	CString sExt, sAssocKey, sSubkey;
	WCHAR szData[MAX_PATH] = L"";

	sExt.Format( L".%s", lpExt );
	sAssocKey.Format( L"%s%s", NAME_FileAssoc, lpExt );

	//删除我们的关联节
	CrtRegDeleteKeyValue(HKEY_CLASSES_ROOT,  sAssocKey , NULL);

	//删除这个节
	// 	[HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.avi\OpenWithProgids]
	// 	"LePlayer.avi"=hex(0):
	sSubkey.Format( L"%s\\.%s\\OpenWithProgids", REG_FILEEXTS, lpExt );
	CrtRegDeleteKeyValue(HKEY_CLASSES_ROOT, sSubkey, sAssocKey);
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CAssociateFile::FlushShellIcon()
{
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_FLUSHNOWAIT, 0, 0);
}

//添加到打开方式：未知的
void CAssociateFile::AddAplication()
{
	AddShell( HKEY_CLASSES_ROOT, L"Applications\\LePlayer.exe", EET_Normal);
}

void CAssociateFile::RemoveAplication()
{
	SHDeleteKey( HKEY_CLASSES_ROOT, L"Applications\\LePlayer.exe" );
}

//Shell节点添加
// [HKEY_CLASSES_ROOT\LePlayer.avi]
// 
// [HKEY_CLASSES_ROOT\LePlayer.avi\DefaultIcon]
// @="C:\\Program Files\\Baofeng\\StormPlayer\\LibIcon.dll,-201"
// 
// 	[HKEY_CLASSES_ROOT\LePlayer.avi\shell]
// @="open"
// 
// 	[HKEY_CLASSES_ROOT\LePlayer.avi\shell\open]
// @="使用 联想视频 播放(&P)"
// 
// 	[HKEY_CLASSES_ROOT\LePlayer.avi\shell\open\command]
// @="\"C:\\Program Files\\Baofeng\\StormPlayer\\StormPlayer.exe\" \"%1\""
// 
// 	[HKEY_CLASSES_ROOT\LePlayer.avi\shell\play_enqueue]
// @="添加到 联想视频 播放列表(&L)"
// 
// 	[HKEY_CLASSES_ROOT\LePlayer.avi\shell\play_enqueue\command]
// @="\"C:\\Program Files\\Baofeng\\StormPlayer\\StormPlayer.exe\" /addtolist \"%1\""
// 
bool CAssociateFile::AddShell(HKEY hRootKey, LPCWSTR lpSubKey, EExtType eType)
{
	CString sSubkey;
	CString sCmd;
	WCHAR szAppFile[MAX_PATH] = L"";

	sSubkey.Format( _T("%s\\shell"),  lpSubKey );
	CrtRegSetString( hRootKey, sSubkey, L"", L"open" );

	sSubkey.Format( _T("%s\\shell\\open"),  lpSubKey );
	if(eType == EET_BFP)
	{
		CrtRegSetString( hRootKey, sSubkey, L"", L"使用 联想视频 安装(&P)" );
	}
	else
	{
		CrtRegSetString( hRootKey, sSubkey, L"", L"使用 联想视频 播放(&P)" );
	}	

	//Add DropTarget
	if(eType != EET_BFP)
	{
		AddShellDropTarget(hRootKey, sSubkey, TRUE);
	}
	
	GetCurrentFilePath( _hInstance,  L"StormPlayer.exe", szAppFile, _countof(szAppFile) );

	if(EET_BOV == eType)
	{
		sCmd.Format( L"\"%s\" /Type=StormBox /Play /Url=\"%%1\" /From=Shell", szAppFile );
	}
	else if(EET_BFP == eType)
	{
		sCmd.Format(L"\"%s\" /Type=Command /Command=TreePlugin /Url=\"%%1\" /From=Shell", szAppFile);
	}
	else
	{
		sCmd.Format( L"\"%s\" /Type=LocalFile /Play /Url=\"%%1\" /From=Shell", szAppFile );
	}

	sSubkey.Format( _T("%s\\shell\\open\\command"),  lpSubKey );
	CrtRegSetString( hRootKey, sSubkey, L"", sCmd );

	if(EET_BFP == eType)
		return true;

	sSubkey.Format( _T("%s\\shell\\play_enqueue"),  lpSubKey );
	CrtRegSetString( hRootKey, sSubkey, L"", L"添加到 联想视频 播放列表(&L)" );

	//Add DropTarget
	AddShellDropTarget(hRootKey, sSubkey, FALSE);

	if(EET_Normal == eType)
		sCmd.Format( L"\"%s\" /Type=LocalFile /Url=\"%%1\" /From=Shell", szAppFile );
	else
		sCmd.Format( L"\"%s\" /Type=StormBox /Url=\"%%1\" /From=Shell", szAppFile );

	sSubkey.Format( _T("%s\\shell\\play_enqueue\\command"),  lpSubKey );
	CrtRegSetString( hRootKey, sSubkey, L"", sCmd );

	return true;
}

//Shell/DropTarget节点添加
// CLSID_* 具体见ShellDrag.dll[当有改动及时更新]
// 	[HKEY_CLASSES_ROOT\LePlayer.XXX\shell\open\DropTarget]
// "Clsid"=<<<<CLSID_DropTarget_Open>>>>
// 
// 	[HKEY_CLASSES_ROOT\LePlayer.XXX\shell\play_enqueue\DropTarget]
// "Clsid"=<<<<<CLSID_DropTarget_Addtolist>>>>>

#define KEYNAME_DROPTARGET			L"DropTarget"
#define VALUENAME_CLSID				L"Clsid"
#define CLSID_DropTarget_Open		L"{944AE114-F63B-4AFB-9348-3E0BDAFE1E49}"
#define CLSID_DropTarget_Addtolist	L"{205D2714-073F-473E-BDEF-460D13A47966}"

/* Input : 
	hrootKey  : HKEY_CLASSES_ROOT
	lpwssubKey: [LePlayer.XXX\shell\open] or [LePlayer.XXX\shell\play_enqueue]
	bAutoPlay : TRUE : Use CLSID_DropTarget_Open
				FALSE: Use CLSID_DropTarget_Addtolist	
*/
bool CAssociateFile::AddShellDropTarget(HKEY hRootKey, LPCWSTR lpwsSubKey, BOOL bAutoPlay)
{
	if(NULL == lpwsSubKey)
		return false;
	
	CString strSubkey;
	strSubkey.Format(L"%s\\%s", lpwsSubKey, KEYNAME_DROPTARGET);

	bool bRet = false;
	if(bAutoPlay)
	{
		bRet = CrtRegSetString(hRootKey, strSubkey, VALUENAME_CLSID, CLSID_DropTarget_Open);
	}
	else
	{
		bRet = CrtRegSetString(hRootKey, strSubkey, VALUENAME_CLSID, CLSID_DropTarget_Addtolist);
	}
	return bRet;
}

//添加关联图标
//C:\Program Files\Super Player\LibIcon.dll,-206
//扩展名对应图标放在LibIcon.dll中。
extern CLibIconHelper g_LibIcon;
bool CAssociateFile::AddShellIcon(HKEY hRootKey, LPCWSTR lpSubKey, LPCWSTR lpExt)
{
	CString sSubkey;
	CString sCmd;
	WCHAR szAppFile[MAX_PATH] = L"";
	int nIconId = g_LibIcon.GetAssociateIconId(lpExt);
	if( nIconId == 0 )
		return false;

	sSubkey.Format( L"%s\\DefaultIcon",  lpSubKey );
	GetCurrentFilePath( _hInstance, L"LibIcon.dll", szAppFile, _countof(szAppFile) );
	sCmd.Format( L"%s,-%d", szAppFile, nIconId );
	CrtRegSetString( hRootKey, sSubkey, L"", sCmd );

	return true;
}

//用户手动选择
// [HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.avi]
// 
// [HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.avi\OpenWithList]
// "a"="wmplayer.exe"
// 	"MRUList"="ba"
// 	"b"="StormPlayer.exe"
// 
// 	[HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.avi\OpenWithProgids]
// "WMP11.AssocFile.AVI"=hex(0):
// "LePlayer.avi"=hex(0):
// [HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.mp4\UserChoice]
// "Progid"="WMP11.AssocFile.MP4"
bool CAssociateFile::AddUserChoice(LPCWSTR lpExt )
{
	CString sAssocKey, sSubkey;
	WCHAR szData[MAX_PATH] = L"";

	sAssocKey.Format( L"%s%s", NAME_FileAssoc, lpExt );
	sSubkey.Format( L"%s\\.%s\\UserChoice", REG_FILEEXTS, lpExt );
	bool bRet = CrtRegGetString( HKEY_CURRENT_USER, sSubkey, L"Progid", szData, sizeof(szData) );
	if( bRet )
	{
		if( StrStrI(szData, NAME_FileAssoc) == NULL )
		{
			CrtRegSetString(HKEY_CURRENT_USER, sSubkey, L"Progid", sAssocKey );
			CrtRegSetString(HKEY_CURRENT_USER, sSubkey, NAME_FileAssoc_Bak, szData );
		}
	}
	sSubkey.Format( L"%s\\.%s", REG_FILEEXTS, lpExt );
	bRet = CrtRegGetString( HKEY_CURRENT_USER, sSubkey, L"Progid", szData, sizeof(szData) );
	if( bRet )
	{
		if( StrStrI(szData, NAME_FileAssoc) == NULL )
		{
			CrtRegSetString(HKEY_CURRENT_USER, sSubkey, L"Progid", sAssocKey );
			CrtRegSetString(HKEY_CURRENT_USER, sSubkey, NAME_FileAssoc_Bak, szData );
		}
	}
	return true;
}

//这个不用加，不用处理，黄森堂 2011.7.20
//HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.AAC\OpenWithList
// bool CAssociateFile::AddOpenWithList(LPCWSTR lpExt )
// {
// 	CString sAssocKey, sSubkey;
// 	WCHAR szData[MAX_PATH] = L"";
// 	sSubkey.Format( L"%s\\.%s%s", REG_FILEEXTS, lpExt,L"\\OpenWithList" );
// 	HKEY hKey = NULL;
// 	LONG lRet = RegOpenKey(HKEY_CURRENT_USER, sSubkey, &hKey);
// 	if( lRet != ERROR_SUCCESS )
// 	{
// 		CrtRegSetString(HKEY_CURRENT_USER, sSubkey, L"a", L"StormPlayer.exe" );
// 		CrtRegSetString(HKEY_CURRENT_USER, sSubkey, L"MRUList",L"a" );
// 		return false ;
// 	}
// 	DWORD dwIndex = 0;
// 	char chindex ='a';
// 	bool bfindList = false;
// 	CString sMRUListdata,sNodeName;
// 	while (true)
// 	{
// 		TCHAR name[MAX_PATH] = L"";
// 		TCHAR data[MAX_PATH] = {0};
// 		DWORD namelen, datalen, type;
// 		namelen = MAX_PATH;
// 		datalen = sizeof(data);
// 		lRet = RegEnumValue(hKey, dwIndex, name, &namelen, NULL, &type,
// 			(LPBYTE) data, &datalen);
// 
// 		if (lRet == ERROR_SUCCESS )
// 		{
// 			dwIndex++;
// 			if(wcsicmp(name,L"MRUList")==0)
// 			{
// 				sMRUListdata.Format( L"%s", data);
// 				bfindList = true;
// 			}else
// 			{
// 				if(wcsicmp(data,L"StormPlayer.exe")==0)
// 				{
// 					return true;
// 				}
// 				chindex++;
// 				if(chindex>'z')
// 				{
// 					return false;
// 				}
// 			}
// 
// 		}
// 		else
// 			break;
// 	}
// 
// 	if(bfindList)
// 	{
// 		sMRUListdata.Format( L"%s%c", sMRUListdata,chindex);
// 		CrtRegSetString(HKEY_CURRENT_USER, sSubkey, L"MRUList",sNodeName );
// 	}
// 	sNodeName.Format( L"%c",chindex);
// 	CrtRegSetString(HKEY_CURRENT_USER, sSubkey, sNodeName, L"StormPlayer.exe" );
// 	return true;
// }
// 

#define EXT_NAME_BOV			L".bov"
#define EXT_NAME_BFP			L".bfp"

EExtType CAssociateFile::GetExtType(LPCWSTR lpwsExt)
{
	if(lstrcmpiW(lpwsExt, EXT_NAME_BOV) == 0)
	{
		return EET_BOV;
	}
	else if(lstrcmpiW(lpwsExt, EXT_NAME_BFP) == 0)
	{
		return EET_BFP;
	}
	return EET_Normal;
}