#include "stdafx.h"
#include "IniHelper.h"
#include <assert.h>
#include <Shlwapi.h>
#include <strsafe.h>
#include <tchar.h>
#include <ShlObj.h>

#include "LogBase.h"

namespace {
    std::wstring PathFindFolder(const wchar_t* lpPath) {
        wchar_t buffer[MAX_PATH] = { 0 };
        StringCbCopy(buffer, MAX_PATH, lpPath);
        ::PathRemoveFileSpec(buffer);
        return std::wstring(buffer);
    }

    bool MakeSureDirPathExist(LPCTSTR lpcsDirPath)
    {
        if (false == PathFileExists(lpcsDirPath))
        {
            if (ERROR_SUCCESS != SHCreateDirectory(NULL, lpcsDirPath))
            {
                return false;
            }
        }
        return true;
    }
}

IniHelper::IniHelper(LPCWSTR lpcsIniFile)
{
    ZeroMemory( m_csIniFile, MAX_PATH*sizeof(_TCHAR ) );
    wcscpy_s( m_csIniFile, MAX_PATH, lpcsIniFile );

    std::wstring strFolder = PathFindFolder(m_csIniFile);
    if (!strFolder.empty() && !MakeSureDirPathExist(strFolder.c_str()))
    {
        LOGPRINTTIMESTAMP(L"IniHelper::IniHelper MakeSureDirPathExist fail: %s, GetLastError() = %d", m_csIniFile, GetLastError());
    }
}

IniHelper::~IniHelper()
{
}

bool IniHelper::SetInt( LPCWSTR lpcsSectionName, LPCWSTR lpcsKeyName, int nValue )
{
//     if ( false == PathFileExists( m_csIniFile ) )
//         return false;

    wchar_t pszValue[20] = {0};
    StringCbPrintf(pszValue, sizeof(pszValue), TEXT("%d"), nValue);
    return (TRUE == WritePrivateProfileString( lpcsSectionName, lpcsKeyName, pszValue, m_csIniFile )) ? true : false;
}

int IniHelper::GetInt( LPCWSTR lpcsSectionName, LPCWSTR lpcsKeyName, int nDefault )
{
    if ( false == PathFileExists( m_csIniFile ) )
        return nDefault;

    return (int)GetPrivateProfileInt( lpcsSectionName, lpcsKeyName, nDefault, m_csIniFile );
}

bool IniHelper::SetInt64( LPCWSTR lpcsSectionName, LPCWSTR lpcsKeyName, LONGLONG nValue )
{
//     if ( false == PathFileExists( m_csIniFile ) )
//         return false;

    wchar_t pszValue[64] = {0};
    StringCbPrintf(pszValue, sizeof(pszValue), TEXT("%I64d"), nValue);
    return (TRUE == WritePrivateProfileString( lpcsSectionName, lpcsKeyName, pszValue, m_csIniFile )) ? true : false;
}

LONGLONG IniHelper::GetInt64( LPCWSTR lpcsSectionName, LPCWSTR lpcsKeyName, LONGLONG nDefault )
{
    if ( false == PathFileExists( m_csIniFile ) )
        return nDefault;

    wchar_t pszValue[64] = {0};
    int nRet = GetPrivateProfileString( lpcsSectionName, lpcsKeyName, NULL, pszValue, 64 , m_csIniFile );
    if ( nRet == -1 || _tcslen( pszValue ) <= 0 )	{
        return nDefault;
    }

    return _wtoi64( pszValue );
}

bool IniHelper::SetUint64( LPCWSTR lpcsSectionName, LPCWSTR lpcsKeyName, ULONGLONG nValue )
{
//     if ( false == PathFileExists( m_csIniFile ) )
//         return false;

    wchar_t pszValue[64] = {0};
    StringCbPrintf(pszValue, sizeof(pszValue), TEXT("%I64u"), nValue);
    return (TRUE == WritePrivateProfileString( lpcsSectionName, lpcsKeyName, pszValue, m_csIniFile )) ? true : false;
}

ULONGLONG IniHelper::GetUint64( LPCWSTR lpcsSectionName, LPCWSTR lpcsKeyName, ULONGLONG nDefault )
{
    if ( false == PathFileExists( m_csIniFile ) )
        return nDefault;

    wchar_t pszValue[64] = {0};
    int nRet = GetPrivateProfileString( lpcsSectionName, lpcsKeyName, NULL, pszValue, 64 , m_csIniFile );
    if ( nRet == -1 || wcslen( pszValue ) <= 0 )	{
        return nDefault;
    }

    return _wcstoui64( pszValue, NULL, 10 );
}

bool IniHelper::SetBool( LPCWSTR lpcsSectionName, LPCWSTR lpcsKeyName, bool bValue )
{
    return SetInt(lpcsSectionName, lpcsKeyName, bValue ? 1 : 0);
}

bool IniHelper::GetBool( LPCWSTR lpcsSectionName, LPCWSTR lpcsKeyName, bool bDefault )
{
    int nValue =  GetInt(lpcsSectionName, lpcsKeyName, bDefault ? 1 : 0);
    return (nValue == 0 ) ? false : true;
}

bool IniHelper::SetString( LPCWSTR lpcsSectionName, LPCWSTR lpcsKeyName, LPCWSTR lpcsValue )
{
//     if ( false == PathFileExists( m_csIniFile ) )
//         return false;

    return (TRUE == WritePrivateProfileString( lpcsSectionName, lpcsKeyName, lpcsValue, m_csIniFile )) ? true : false;
}

std::wstring IniHelper::GetString( LPCWSTR lpcsSectionName, LPCWSTR lpcsKeyName, LPCWSTR lpcsDefault )
{
    if ( false == PathFileExists( m_csIniFile ) )
        return lpcsDefault;


    wchar_t pszValue[MAX_PATH] = {0};
    int nRet = GetPrivateProfileString( lpcsSectionName, lpcsKeyName, lpcsDefault, pszValue, MAX_PATH , m_csIniFile );
    if ( nRet == -1 || wcslen( pszValue ) < 0 )	{
        assert( 0 );
        return lpcsDefault;
    }

    return pszValue;
}

bool IniHelper::DeleteSection( const LPCWSTR lpcsSectionName )
{
    if ( false == PathFileExists( m_csIniFile ) )
        return false;

    return ( TRUE == WritePrivateProfileString( lpcsSectionName, NULL, NULL, m_csIniFile) ) ? true : false;
}

bool IniHelper::DeleteSectionKey( const LPCWSTR lpcsSectionName, LPCWSTR lpcsKeyName )
{
    if ( false == PathFileExists( m_csIniFile ) )
        return false;

    return ( TRUE == WritePrivateProfileString( lpcsSectionName, lpcsKeyName, NULL, m_csIniFile) ) ? true : false;
}

namespace {
    std::wstring GetTestModeFilePath() {
        WCHAR szPath[MAX_PATH] = { 0 };
        SHGetSpecialFolderPath(NULL, szPath, CSIDL_APPDATA, FALSE);
        PathAppend(szPath, L"51talk\\testmode.ini");
        return szPath;
    }

    std::wstring GetConfigFilePath() {
        WCHAR szPath[MAX_PATH] = { 0 };
        SHGetSpecialFolderPath(NULL, szPath, CSIDL_APPDATA, FALSE);
        PathAppend(szPath, L"51talk\\config.ini");
        return szPath;
    }

    std::wstring GetSubConfigFilePath() {
        WCHAR szPath[MAX_PATH] = { 0 };
        SHGetSpecialFolderPath(NULL, szPath, CSIDL_APPDATA, FALSE);
        PathAppend(szPath, L"51talk\\config\\config.ini");
        return szPath;
    }

    std::wstring GetCommonConfigFilePath() {
        WCHAR szPath[MAX_PATH] = { 0 };
        SHGetSpecialFolderPath(NULL, szPath, CSIDL_COMMON_APPDATA, FALSE);
        PathAppend(szPath, L"51talk\\config.ini");
        return szPath;
    }

    std::wstring GetB2SConfigFilePath() {
        WCHAR szPath[MAX_PATH] = { 0 };
        SHGetSpecialFolderPath(NULL, szPath, CSIDL_APPDATA, FALSE);
        PathAppend(szPath, L"51talk\\config\\b2s.ini");
        return szPath;
    }
}

IniHelper* TestModeHelper()
{
    static IniHelper helper(GetTestModeFilePath().c_str());
    return &helper;
}

IniHelper* ConfigHelper()
{
    static IniHelper helper(GetConfigFilePath().c_str());
    return &helper;
}

IniHelper* SubConfigHelper()
{
    static IniHelper helper(GetSubConfigFilePath().c_str());
    return &helper;
}

IniHelper* CommonConfigHelper()
{
    static IniHelper helper(GetCommonConfigFilePath().c_str());
    return &helper;
}

IniHelper* B2SConfigHelper()
{
    static IniHelper helper(GetB2SConfigFilePath().c_str());
    return &helper;
}
