#include "StdAfx.h"
#include "SetupAssociate.h"
#include "AssociateFile.h"
#include "AssociateDisc.h"

#include "../DllHelper/LibIconHelper.h"


#ifdef _DEBUG
#define new new( _NORMAL_BLOCK, __FILE__, __LINE__) 
#endif

CSetupAssociate::CSetupAssociate(void)
{
}

CSetupAssociate::~CSetupAssociate(void)
{
}

void CSetupAssociate::SetupFileAssociate()
{
	HKEY hKey = NULL;
	LONG lRet = RegOpenKey(HKEY_CURRENT_USER, REG_ASSOC_SETUP, &hKey);
	if( lRet == ERROR_SUCCESS )
	{
		DWORD dwIndex = 0;
		while (true)
		{
			TCHAR szExt[MAX_PATH] = {0};
			DWORD dwExtSize = MAX_PATH;
			DWORD dwType = REG_DWORD;

			DWORD dwValue = 0;
			DWORD dwSize = sizeof(DWORD);

			lRet = RegEnumValue(hKey, dwIndex, szExt, &dwExtSize, NULL, &dwType, PBYTE(&dwValue), &dwSize); 
			if (lRet == ERROR_SUCCESS )
			{
				if( dwType == REG_DWORD && dwValue == 1 )
				{
					CAssociateFile::Associate(szExt, true );
				}
				else if (dwType == REG_DWORD && dwValue == 0)
				{
					//当用户没有选择关联，也要创建我们的文件关联
					CAssociateFile::BFKey_AddAssociate(szExt);
				}

				ConfigSetDWORD( szExt, dwValue, ConfigType_FileAssociate );
				dwIndex++;
			}
			else
				break;
		}

		RegCloseKey(hKey);
		SHDeleteKey(HKEY_CURRENT_USER, REG_ASSOC_SETUP);
	}


	CAssociateFile::Associate(L"bov", true );

	//打开方式的列表
	CAssociateFile::AddAplication();

	CAssociateDisc::AssociateDVD(true);
	CAssociateDisc::AssociateDVDAutoplay(true);
	CAssociateDisc::AssociateUsbAutoplay(true);

	CAssociateFile::FlushShellIcon();
}

extern CLibIconHelper g_LibIcon;
void CSetupAssociate::RemoveAllFileAssociate()
{
	DWORD dwIndex = 0;
	do 
	{
		LPCWSTR lpExt = g_LibIcon.EnumFileExt(dwIndex);
		if( lpExt == NULL )
			break;

		CAssociateFile::Associate( lpExt, false );
		CAssociateFile::BFKey_DeleteAssociate(lpExt);


		DWORD dwValue = 0;
		ConfigSetDWORD( lpExt, dwValue, ConfigType_FileAssociate );
		++dwIndex;
	} while (true);

	CAssociateFile::Associate(L"bov", false );
	CAssociateFile::RemoveAplication();

	CAssociateDisc::AssociateDVD(false);
	CAssociateDisc::AssociateDVDAutoplay(false);
	CAssociateDisc::AssociateUsbAutoplay(false);

	CAssociateFile::FlushShellIcon();
}

bool CSetupAssociate::SaveAssociateState(LPCWSTR lpExt, bool bAssociate)
{
	DWORD dwValue = bAssociate?1:0;
	DWORD dwRet = SHSetValue( HKEY_CURRENT_USER, REG_UAC_Associate, lpExt, REG_DWORD,	&dwValue, sizeof(dwValue) );
	if( dwRet == ERROR_SUCCESS )
		return true;

	return false;
}

void CSetupAssociate::CleanAllAssociateState()
{
	SHDeleteKey(HKEY_CURRENT_USER, REG_UAC_Associate);
}

void CSetupAssociate::FileAssociateFromTempWithPower()
{
	HKEY hKey = NULL;
	LONG lRet = RegOpenKey(HKEY_CURRENT_USER, REG_UAC_Associate, &hKey);
	if (lRet != ERROR_SUCCESS)
		return ;

	DWORD dwIndex = 0;
	while (true)
	{
		TCHAR szExt[MAX_PATH] = {0};
		DWORD dwExtSize = MAX_PATH;
		DWORD dwType = REG_DWORD;

		DWORD dwValue = 0;
		DWORD dwSize = sizeof(DWORD);

		lRet = RegEnumValue(hKey, dwIndex, szExt, &dwExtSize, NULL, &dwType, PBYTE(&dwValue), &dwSize); 
		if (lRet == ERROR_SUCCESS)
		{
			if( dwType==REG_DWORD )
				CAssociateFile::Associate(szExt, (dwValue==1?true:false) );
			ConfigSetDWORD( szExt, dwValue, ConfigType_FileAssociate );
			dwIndex++;
		}
		else
			break;
	}

	RegCloseKey(hKey);

	CleanAllAssociateState();

	CAssociateFile::AddAplication();

	CAssociateFile::FlushShellIcon();
}

void CSetupAssociate::InstallByUpdate()
{
	HKEY hKey = NULL;
	LONG lRet = RegOpenKey(HKEY_CURRENT_USER, REG_SUBKEY_FILEASSOCIATE, &hKey);
	if (lRet != ERROR_SUCCESS)
		return ;

	DWORD dwIndex = 0;
	while (true)
	{
		TCHAR szExt[MAX_PATH] = {0};
		DWORD dwExtSize = MAX_PATH;
		DWORD dwType = REG_DWORD;

		DWORD dwValue = 0;
		DWORD dwSize = sizeof(DWORD);

		lRet = RegEnumValue(hKey, dwIndex, szExt, &dwExtSize, NULL, &dwType, PBYTE(&dwValue), &dwSize); 
		if (lRet == ERROR_SUCCESS)
		{
			if( dwType==REG_DWORD )
				CAssociateFile::Associate(szExt, (dwValue==1?true:false) );
			dwIndex++;
		}
		else
			break;
	}

	RegCloseKey(hKey);

	CAssociateFile::AddAplication();

	CAssociateFile::FlushShellIcon();
}