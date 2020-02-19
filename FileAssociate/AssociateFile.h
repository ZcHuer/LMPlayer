#pragma once

enum EExtType
{
	EET_Normal = 0,
	EET_BOV,
	EET_BFP,
};

class CAssociateFile
{
public:
	CAssociateFile(void);
	virtual ~CAssociateFile(void);

public:
	static bool IsFileAssociate(LPCWSTR lpExt);
	static bool Associate(LPCWSTR lpExt, bool bAssociate);

	static bool FastAssociate(LPCWSTR lpExt, bool bAssociate);

	static void FlushShellIcon();

	static void AddAplication();
	static void RemoveAplication();

	static bool AddUserChoice(LPCWSTR lpExt );

	static void BFKey_AddAssociate(LPCWSTR lpExt);
	static void BFKey_DeleteAssociate(LPCWSTR lpExt);

private:
	static bool IsFileAssociate_WIN7(LPCWSTR lpExt);

	// 保证Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.xxx\\UserChoice
	// Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.xxx
	// 以及HKEY_CLASSES_ROOT\\.xxx下都OK才可断定关联没有问题
	static bool JudgeIsFileAssociate( LPCWSTR lpExt ) 
	{
		CString sSubkey;
		WCHAR szAssoc[MAX_PATH] = L"";
		bool bRet = false;

		//Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.xxx\\UserChoice
		sSubkey.Format( L"%s\\.%s\\UserChoice", REG_FILEEXTS, lpExt );
		bool bR1 = CrtRegGetString( HKEY_CURRENT_USER, sSubkey, L"Progid", szAssoc, MAX_PATH );
		if( bR1 )
		{
			bR1 = IsMyAssociate(szAssoc);
		}

		// 增加Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.xxx 的Progid判断
		sSubkey.Format( L"%s\\.%s", REG_FILEEXTS, lpExt );
		bool bR2 = CrtRegGetString( HKEY_CURRENT_USER, sSubkey, L"Progid", szAssoc, MAX_PATH );
		if ( bR2 )
		{
			bR2 = IsMyAssociate(szAssoc);
		}

		//HKEY_CLASSES_ROOT\\.xxx
		sSubkey.Format( L".%s", lpExt );
		bool bR3 = CrtRegGetString( HKEY_CLASSES_ROOT, sSubkey, L"", szAssoc, MAX_PATH );
		if( bR3 )
			bR3 =  IsMyAssociate(szAssoc);

		return bRet = bR1&&bR2&&bR3;
	}

	static bool IsFileAssociate_XP(LPCWSTR lpExt);
	static bool AddFileAssociate(LPCWSTR lpExt);
	static bool DeleteFileAssociate(LPCWSTR lpExt);

	static bool IsMyAssociate(LPCWSTR lpAssoc);

	static bool AddShell(HKEY hRootKey, LPCWSTR lpSubKey, EExtType eType);
	static bool AddShellIcon(HKEY hRootKey, LPCWSTR lpSubKey, LPCWSTR lpExt);
	static bool AddShellDropTarget(HKEY hrootKey, LPCWSTR lpwssubKey, BOOL bAutoPlay);
	//static bool AddOpenWithList(LPCWSTR lpExt );

	static EExtType GetExtType(LPCWSTR lpwsExt);
};
